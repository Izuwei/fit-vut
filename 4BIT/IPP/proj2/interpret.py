#!/usr/bin/env python3.6
# Autor: Jakub Sadílek
# Login: xsadil07

import sys
import argparse
import xml.etree.ElementTree as ET
import re


def Error(msg, code):
    """Funkce pro vypíše chybovou hlášku a ukončí skript odpovídajícím kódem."""
    sys.stderr.write(f"{msg}\n")
    exit(code)


class ArgumentParser(argparse.ArgumentParser):
    def error(self, message):
        """Redefinice hlášky při nevalidních argumentech."""
        sys.stderr.write("Bad arguments, try --help for help.\n")
        exit(10)


class Variable:
    """Třída reprezentující proměnnou."""
    def __init__(self, var):
        self.name = var["value"].split('@')[1]
        self.value = None
        self.dType = None

    def __repr__(self):
        return str({"Name": self.name, "Type:": self.dType, "Value:": self.value})


class Interpret:
    """Třída interpretující příkazy."""
    def __init__(self):
        self.FrameStack = []
        self.GlobalFrame = []
        self.LocalFrame = []
        self.TempFrame = []
        self.DataStack = []
        self.Labels = []
        self.CallStack = []
        self.Instr = None
        self.LFactive = False
        self.TFactive = False
        self.Position = 0
        self.iCounter = 0

    def doIns(self, ins):
        self.Instr = ins        # Přiřazení aktuální instrukce
        self.Position += 1      # Pozice v kódu
        self.iCounter += 1      # Počítadlo instrukcí

        if ins.opcode == "MOVE": self.doMOVE()
        elif ins.opcode == "CREATEFRAME": self.doCREATEFRAME()
        elif ins.opcode == "PUSHFRAME": self.doPUSHFRAME()
        elif ins.opcode == "POPFRAME": self.doPOPFRAME()
        elif ins.opcode == "DEFVAR": self.doDEFVAR()
        elif ins.opcode == "CALL": self.doCALL()
        elif ins.opcode == "RETURN": self.doRETURN()
        elif ins.opcode == "PUSHS": self.doPUSHS()
        elif ins.opcode == "POPS": self.doPOPS()
        elif ins.opcode in ["ADD", "SUB", "MUL", "IDIV"]: self.doArith()
        elif ins.opcode in ["EQ", "GT", "LT"]: self.doRel()
        elif ins.opcode in ["AND", "OR", "NOT"]: self.doLogic()
        elif ins.opcode == "INT2CHAR": self.doINT2CHAR()
        elif ins.opcode == "STRI2INT": self.doSTRI2INT()
        elif ins.opcode == "READ": self.doREAD()
        elif ins.opcode == "WRITE": self.doWRITE()
        elif ins.opcode == "CONCAT": self.doCONCAT()
        elif ins.opcode == "STRLEN": self.doSTRLEN()
        elif ins.opcode == "GETCHAR": self.doGETCHAR()
        elif ins.opcode == "SETCHAR": self.doSETCHAR()
        elif ins.opcode == "TYPE": self.doTYPE()
        elif ins.opcode == "LABEL": pass
        elif ins.opcode == "JUMP": self.doJUMP()
        elif ins.opcode in ["JUMPIFEQ", "JUMPIFNEQ"]: self.doJUMPIF()
        elif ins.opcode == "EXIT": self.doEXIT()
        elif ins.opcode == "DPRINT": self.doDPRINT()
        elif ins.opcode == "BREAK": self.doBREAK()
        else: Error(f"Unknown opcode {ins.opcode}.", 32)

        return self.Position

    def ExistFrame(self, frame):
        """Funkce vrací pravdivostní hodnotu, podle existence rámce."""
        if frame == "GF":
            return True
        elif frame == "LF" and self.LFactive:
            return True
        elif frame == "TF" and self.TFactive:
            return True
        else:
            return False

    def getVar(self, var):
        """Funkce vratí proměnnou jestli je definována, jinak error, jestli neexistuje."""
        frame = var["value"].split('@')[0]  # Typ rámce
        name = var["value"].split('@')[1]   # Jméno proměnné

        if frame == "GF":
            for each in self.GlobalFrame:
                if each.name == name:
                    return each
        elif frame == "LF" and self.LFactive:
            for each in self.LocalFrame:
                if each.name == name:
                    return each
        elif frame == "TF" and self.TFactive:
            for each in self.TempFrame:
                if each.name == name:
                    return each
        else:
            Error(f"Frame {frame} does not exist.", 55)
        Error(f"Undefined variable {name} in {frame}.", 54)

    def getSymbData(self, symb):
        """Funkce vrátí typ a data ze symbolu či konstanty."""
        DataType = symb["type"]     # Typ symbolu
        data = symb["value"]        # Hodnota symbolu

        if DataType == "var":       # Je to proměnná?
            var = self.getVar(symb)
            DataType = var.dType    # Přepíšeme typ prom. na typ, který obsahuje
            data = var.value        # Vložíme data pro výpis

        if DataType is None or data is None:
            Error(f"Try to read undefined data.", 56)

        return DataType, data       # Vracíme <type> <value>

    def doMOVE(self):
        """Funkce přesune data do proměnné."""
        dest = self.getVar(self.Instr.args[0])
        dest.dType, dest.value = self.getSymbData(self.Instr.args[1])

    def doCREATEFRAME(self):
        """Funkce vytvoří nový TF."""
        self.TempFrame = []     # Nový prázdný TF rámec
        self.TFactive = True

    def doPUSHFRAME(self):
        """Funkce uloží TF na vrchol zásobníku rámců."""
        if not self.ExistFrame("TF"):   # Existuje TF?
            Error("PUSHFRAME error: Temporary frame does not exist.", 55)
        if self.ExistFrame("LF"):       # Pokud existuje LF pushnem ho
            self.FrameStack.append(self.LocalFrame)
        self.LocalFrame = self.TempFrame.copy()  # Push TF
        self.LFactive = True
        self.TFactive = False

    def doPOPFRAME(self):
        """Funkce uloží do TF vrchol zásobníku rámců, pokud je."""
        if not self.ExistFrame("LF"):            # Existuje LF?
            Error("POPFRAME error: Local frame does not exist.", 55)
        self.TempFrame = self.LocalFrame.copy()  # Pop LF
        self.TFactive = True
        if len(self.FrameStack):    # Aktualizujeme LF pokud je další na stacku
            self.LocalFrame = self.FrameStack.pop()
        else:
            self.LFactive = False

    def doDEFVAR(self):
        """Funkce definuje novou proměnnou"""
        frame, name = self.Instr.args[0]["value"].split('@')   # Rámec proměnné

        if frame == "GF":
            for each in self.GlobalFrame:
                if each.name == name:   # Redefinice je zakázaná.
                    Error(f"Variable {self.Instr.args[0]['value']} is already defined.", 52)
            self.GlobalFrame.append(Variable(self.Instr.args[0]))
        elif frame == "LF" and self.LFactive:
            for each in self.LocalFrame:
                if each.name == name:
                    Error(f"Variable {self.Instr.args[0]['value']} is already defined.", 52)
            self.LocalFrame.append(Variable(self.Instr.args[0]))
        elif frame == "TF" and self.TFactive:
            for each in self.TempFrame:
                if each.name == name:
                    Error(f"Variable {self.Instr.args[0]['value']} is already defined.", 52)
            self.TempFrame.append(Variable(self.Instr.args[0]))
        else:
            Error(f"Undefined frame {frame}.", 55)  # Nedefinovaný rámec

    def doCALL(self):
        """Funkce uloží aktuální pozici v kódu a skočí na návěští."""
        self.CallStack.append(self.Position)    # Návratovou hodnotu uložíme na stack
        self.doJUMP()

    def doRETURN(self):
        """Funkce se vrátí na předchozí pozici po CALL instrukci."""
        if not self.CallStack:  # Není stack prázdý?
            Error("CallStack is empty, cannot return.", 56)
        self.Position = self.CallStack.pop()

    def doPUSHS(self):
        """Funkce uloží data na datový zásobník."""
        DataType, data = self.getSymbData(self.Instr.args[0])
        self.DataStack.append({"type": DataType, "value": data})

    def doPOPS(self):
        """Funkce POPne data z datového zasobníku."""
        if not self.DataStack:
            Error("DataStack is empty.", 56)

        var = self.getVar(self.Instr.args[0])
        data = self.DataStack.pop()
        var.dType = data["type"]
        var.value = data["value"]

    def doArith(self):
        """Funkce provede základní aritmetické instrukce: ADD, SUB, MUL, DIV."""
        dest = self.getVar(self.Instr.args[0])                  # <var>
        S1type, S1val = self.getSymbData(self.Instr.args[1])    # <symb>
        S2type, S2val = self.getSymbData(self.Instr.args[2])    # <symb>

        if S1type == "int" and S1type == S2type:    # Symboly typu integer
            dest.dType = "int"
            try:
                if self.Instr.opcode == "ADD":
                    dest.value = S1val + S2val
                elif self.Instr.opcode == "SUB":
                    dest.value = S1val - S2val
                elif self.Instr.opcode == "MUL":
                    dest.value = S1val * S2val
                elif self.Instr.opcode == "IDIV":
                    dest.value = S1val // S2val
            except ArithmeticError:
                Error("Division by zero.", 57)
        else:
            Error(f"{self.Instr.opcode} work only with integers.", 53)

    def doRel(self):
        """Funkce provede relační instrukce: EQ, GT, LT."""
        dest = self.getVar(self.Instr.args[0])                  # <var>
        S1type, S1val = self.getSymbData(self.Instr.args[1])    # <symb>
        S2type, S2val = self.getSymbData(self.Instr.args[2])    # <symb>

        if self.Instr.opcode == "GT" and S1type == S2type and S1type in ["int", "string", "bool"]:
            dest.value = S1val > S2val
        elif self.Instr.opcode == "LT" and S1type == S2type and S1type in ["int", "string", "bool"]:
            dest.value = S1val < S2val
        elif self.Instr.opcode == "EQ":
            if S1type == "nil" and S2type == "nil":
                dest.value = True
            elif S1type == "nil" or S2type == "nil":
                dest.value = False
            elif S1type == S2type:
                dest.value = S1val == S2val
            else:
                Error(f"{self.Instr.opcode} incompatible operands.", 53)
        else:
            Error(f"{self.Instr.opcode}: Incopatible arguments.", 53)

        dest.dType = "bool"

    def doLogic(self):
        """Funkce provede logicke instrukce: AND, OR, NOT."""
        dest = self.getVar(self.Instr.args[0])                  # <var>
        S1type, S1val = self.getSymbData(self.Instr.args[1])    # <symb>

        if S1type == "bool":
            if self.Instr.opcode == "AND":
                S2type, S2val = self.getSymbData(self.Instr.args[2])  # <symb>
                if S1type == S2type:
                    dest.value = S1val and S2val
            elif self.Instr.opcode == "OR":
                S2type, S2val = self.getSymbData(self.Instr.args[2])  # <symb>
                if S1type == S2type:
                    dest.value = S1val or S2val
            elif self.Instr.opcode == "NOT" and S1type == "bool":
                dest.value = not S1val
        else:
            Error(f"{self.Instr.opcode} incompatible operands.", 53)

        dest.dType = "bool"

    def doINT2CHAR(self):
        """Funcke převede celočíselnou hodnotu na znak."""
        dest = self.getVar(self.Instr.args[0])                 # <var>
        DataType, data = self.getSymbData(self.Instr.args[1])  # <symb>

        if DataType != "int":   # Pouze čísla
            Error(f"{self.Instr.opcode} receives only numbers.", 53)

        try:
            dest.dType = "string"
            dest.value = chr(data)
        except Exception:
            Error("Integer passed to INT2CHAR is out of range.", 58)

    def doSTRI2INT(self):
        """Provede instrukci STRI2INT."""
        dest = self.getVar(self.Instr.args[0])                   # <var>
        StrType, StrData = self.getSymbData(self.Instr.args[1])  # <symb>
        IntType, IntData = self.getSymbData(self.Instr.args[2])  # <symb>

        if StrType != "string" or IntType != "int":
            Error("Bad type of arguments in instruction STRI2INT.", 53)
        elif IntData < 0:               # Index musí být > 0
            Error("STRI2INT: index must be greater or equal to 0.", 58)

        try:
            dest.dType = "int"
            dest.value = ord(StrData[IntData])
        except LookupError:
            Error("STRI2INT: Index out of string.", 58)

    def doREAD(self):
        """Funkce provede instrukci READ."""
        global args
        dest = self.getVar(self.Instr.args[0])  # <var>
        DataType = self.Instr.args[1]["value"]  # <type>

        try:
            if args.input:      # ze vstupního souboru
                global InputFile
                data = InputFile.pop(0)

            else:               # z STDIN
                data = input()
        except Exception:
            data = ''           # Defaultní hodnota

        if DataType == "int":   # Převedeme
            try:
                dest.value = int(data)
            except ValueError:
                dest.value = 0
        elif DataType == "string":
            dest.value = data
        elif DataType == "bool":
            if data.lower() == "true":
                dest.value = True
            else:
                dest.value = False

        dest.dType = DataType

    def doWRITE(self):
        """Funkce provede instrukci WRITE."""
        DataType, data = self.getSymbData(self.Instr.args[0])
        if DataType == "int":       # Výpis čísel
            print(f"{data}", end='')
        elif DataType == "string":  # Výpis řetězce
            print(f"{data}", end='')
        elif DataType == "bool":    # Výpis bool
            print(f"{str(data).lower()}", end='')
        elif DataType == "nil":     # Výpis nilu
            print("", end='')

    def doCONCAT(self):
        """Funkce provede instrukci CONCAT."""
        dest = self.getVar(self.Instr.args[0])                 # <var>
        S1Type, S1data = self.getSymbData(self.Instr.args[1])  # <symb>
        S2Type, S2data = self.getSymbData(self.Instr.args[2])  # <symb>

        if S1Type != S2Type or S1Type != "string":
            Error("CONCAT works only with strings.", 53)

        dest.dType = "string"
        dest.value = S1data + S2data    # Konkatenace

    def doSTRLEN(self):
        """Funkce provede instrukci STRLEN."""
        dest = self.getVar(self.Instr.args[0])                   # <var>
        srcType, srcData = self.getSymbData(self.Instr.args[1])  # <symb>

        if srcType != "string":
            Error("STRLEN works only with string.", 53)

        dest.dType = "int"
        dest.value = len(srcData)

    def doGETCHAR(self):
        """Funkce provede instrukci GETCHAR."""
        dest = self.getVar(self.Instr.args[0])                 # <var>
        S1type, S1data = self.getSymbData(self.Instr.args[1])  # <symb>
        S2type, S2data = self.getSymbData(self.Instr.args[2])  # <symb>

        if S1type != "string" or S2type != "int":
            Error("STRLEN works only with string.", 53)
        elif S2data < 0:
            Error("GETCHAR: index must be greater or equal to 0.", 58)

        dest.dType = "string"
        try:
            dest.value = S1data[S2data]
        except LookupError:
            Error("GETCHAR: index out of string.", 58)

    def doSETCHAR(self):
        """Funkce provede instrukci SETCHAR."""
        dest = self.getVar(self.Instr.args[0])                 # <var>
        S1type, S1data = self.getSymbData(self.Instr.args[1])  # <symb>
        S2type, S2data = self.getSymbData(self.Instr.args[2])  # <symb>

        if dest.dType != "string" or S1type != "int" or S2type != "string":
            Error("SETCHAR: Incopatible arguments.", 53)
        elif len(S2data) == 0 or len(dest.value) <= S1data or S1data < 0:
            Error("SETCHAR: Index out of string or empty.", 58)

        dest.value = dest.value[:S1data] + S2data[0] + dest.value[S1data + 1:]

    def doTYPE(self):
        """Funkce provede instrukci TYPE."""
        dest = self.getVar(self.Instr.args[0])   # <var>
        if self.Instr.args[1]["type"] == "var":  # <symb>
            src = self.getVar(self.Instr.args[1])
            srcType = src.dType
        else:
            srcType = self.Instr.args[1]["type"]

        if srcType is None:     # Neinicializovaná proměnná
            dest.value = ""
        else:
            dest.value = srcType

        dest.dType = "string"

    def doLABEL(self, ins):
        """Funkce provede instrukci LABEL."""
        for label in self.Labels:   # Není návěští již definované?
            if label["name"] == ins[0].text:
                Error(f"Label {label['name']} already defined.", 52)
        self.Labels.append({"name": ins[0].text, "position": int(ins.attrib["order"])})

    def doJUMP(self):
        """Funkce provede instrukci JUMP"""
        for label in self.Labels:   # Najdeme návěští.
            if label["name"] == self.Instr.args[0]["value"]:
                self.Position = label["position"]  # Posun na danou pozici.
                return
        Error(f"Label {self.Instr.args[0]['value']} not found.", 52)

    def doJUMPIF(self):
        """Funkce provede instrukce JUMPIFEQ nebo JUMPIFNEQ."""
        S1type, S1data = self.getSymbData(self.Instr.args[1])  # <symb>
        S2type, S2data = self.getSymbData(self.Instr.args[2])  # <symb>

        if S1type != S2type:
            Error(f"{self.Instr.opcode} incompatible operands.", 53)

        if self.Instr.opcode == "JUMPIFEQ":
            jmp = True if S1data == S2data else False
        elif self.Instr.opcode == "JUMPIFNEQ":
            jmp = True if S1data != S2data else False

        if jmp: self.doJUMP()

    def doEXIT(self):
        """Funkce provede instrukci EXIT"""
        DataType, data = self.getSymbData(self.Instr.args[0])  # <symb>
        if DataType != "int":
            Error("You can exit program only with int value.", 53)
        if 0 > data or data > 49:
            Error("You can exit only with value from 0 to 49.", 57)
        exit(data)

    def doDPRINT(self):
        """Funkce provede instrukci DPRINT."""
        DataType, data = self.getSymbData(self.Instr.args[0])
        if DataType == "int":       # Výpis čísel
            sys.stderr.write(f"{data}")
        elif DataType == "string":  # Výpis řetězce
            sys.stderr.write(f"{data}")
        elif DataType == "bool":    # Výpis bool
            sys.stderr.write(f"{str(data).lower()}")
        elif DataType == "nil":
            sys.stderr.write("")
        else:                       # Hodnota není inicializována
            Error(f"Variable {data} has no value.", 56)

    def doBREAK(self):
        """Funkce provede instrukci BREAK."""
        sys.stderr.write(f"""Position in code: {self.Position}\n
            Instructions done: {self.iCounter}\n
            Frame content:\n
                Global frame: {self.GlobalFrame}\n
                Local frame: {self.LocalFrame}\n
                Temporary frame: {self.TempFrame}\n
                Others in stack: {self.FrameStack}\n
            Active frames:\n
                Local frame: {self.LFactive}\n
                Temporary frame: {self.TFactive}\n
            Detected labels:\n
                {self.Labels}\n
            Data stack:\n
                {self.DataStack}\n
            Call stack:\n
                {self.CallStack}\n""")


class Instruction:
    """Třída reprezentuje instrukci."""
    def __init__(self, insXML):
        self.opcode = insXML.attrib["opcode"].upper()   # opcode
        self.args = []                                  # list argumentu

        for argument in insXML:  # Argument reprezentujeme slovníkem.
            self.args.append({"type": argument.attrib["type"], "value": argument.text})

        self.Scanner()

    def Scanner(self):
        """Funkce zkontroluje syntax."""
        if self.opcode in ["CREATEFRAME", "PUSHFRAME", "POPFRAME", "RETURN", "BREAK"]:
            self.InsNoArgs()
        elif self.opcode in ["DEFVAR", "POPS"]:
            self.InsVar()
        elif self.opcode in ["CALL", "LABEL", "JUMP"]:
            self.InsLab()
        elif self.opcode in ["PUSHS", "WRITE", "EXIT", "DPRINT"]:
            self.InsSymb()
        elif self.opcode in ["MOVE", "INT2CHAR", "TYPE", "STRLEN", "NOT"]:
            self.InsVarSymb()
        elif self.opcode in ["JUMPIFEQ", "JUMPIFNEQ"]:
            self.InsLabSymbSymb()
        elif self.opcode in ["ADD", "SUB", "MUL", "IDIV", "EQ", "GT", "LT", 
        "AND", "OR", "CONCAT", "GETCHAR", "SETCHAR", "STRI2INT"]:
            self.InsVarSymbSymb()
        elif self.opcode == "READ":
            self.InsVarType()
        else:
            Error(f"Unknown opcode {self.opcode}", 32)

    def StrConv(self, string):
        """Funkce převede escape sekvence ve stringu."""
        i = 0
        for char in string["value"]:    # Každé písmenko
            if char == '\\':            # Konvertujeme escape sekvenci
                val = int(string["value"][i+1] + string["value"][i+2] + string["value"][i+3])

                if 0 <= val <= 999:     # Kontrola rozmezí, podle zadání
                    string["value"] = string["value"][:i] + chr(val) + string["value"][i+4:]
                else:
                    Error(f"Wrong escape sequence in string: {string['value']}", 32)

                i -= 3
            i += 1

    def CheckName(self, name):
        """Funkce zkontroluje názvy návěští a proměnných."""
        if re.match(r'^[A-Za-z_\-$&%*!?][A-z_\-$&%*!?0-9]*$', name) is None or not name:
            Error(f"Invalid name {name}.", 32)

    def CheckVariable(self, variable):
        """Funkce zkontroluje správnou definici proměnných,"""
        if variable["type"] != "var":   # Type musí být 'var'
            Error(f"Variable's type must be 'var' not {variable['type']}.", 32)

        value = variable["value"].split('@')

        if len(value) != 2:
            Error(f"Invalid name for variable {variable['value']}.", 32)
        if value[0] not in ["GF", "LF", "TF"]:  # Kontrola rámců
            Error(f"Unknown frame in variable {variable['value']}.", 32)

        self.CheckName(value[1])

    def CheckSymbol(self, symbol):
        """Funkce zkontroluje správnou definici symbolů resp. konstant."""
        if symbol["type"] == "int" and symbol["value"] is not None:  # Kontrola intu
            if re.match(r'^[+-]?[0-9]+$', symbol['value']) is None:
                Error(f"Invalid number {symbol['value']}.", 32)
            symbol["value"] = int(symbol["value"])
        elif symbol["type"] == "string":    # Kontrola stringu
            if symbol["value"] is not None:
                if re.match(r'^((\\\d{3,})|([^\\\s#]))*$', symbol['value']) is None:
                    Error(f"Invalid string '{symbol['value']}'.", 32)
                self.StrConv(symbol)
            else:
                symbol["value"] = ""
        elif symbol["type"] == "bool" and symbol["value"] is not None:  # Kontrola boolu
            if re.match(r'^(true|false)$', symbol['value']) is None:
                Error(f"Invalid bool '{symbol['value']}'.", 32)
            symbol["value"] = True if symbol["value"] == "true" else False
        elif symbol["type"] == "nil" and symbol["value"] is not None:   # Kontrola nilu
            if re.match(r'^nil$', symbol['value']) is None:
                Error(f"Invalid nil '{symbol['value']}'.", 32)
        else:               # Neznamy typ symbolu
            Error(f"Unknown type '{symbol['type']}' or value '{symbol['value']}'.", 32)

    def InsNoArgs(self):
        """Kontrola instrukcí bez argumentů."""
        if len(self.args) != 0:
            Error(f"Instruction {self.opcode} does not accept any arguments.", 32)

    def InsVar(self):
        """Kontrola instrukcí s proměnnou <var>."""
        if len(self.args) != 1:
            Error(f"Instruction {self.opcode} receives one argument.", 32)
        self.CheckVariable(self.args[0])

    def InsLab(self):
        """Kontrola instrukci s návěštím <label>."""
        if len(self.args) != 1:
            Error(f"Instruction {self.opcode} receives one argument.", 32)
        if self.args[0]["type"] != "label":
            Error(f"{self.opcode} expected label not {self.args[0]['type']}.", 32)
        self.CheckName(self.args[0]["value"])

    def InsSymb(self):
        """Kontrola instrukcí se symbolem <symb>."""
        if len(self.args) != 1:
            Error(f"Instruction {self.opcode} receives one argument.", 32)

        if self.args[0]["type"] == "var":   # Proměnná nebo konstanta
            self.CheckVariable(self.args[0])
        else:
            self.CheckSymbol(self.args[0])

    def InsVarSymb(self):
        """Kontrola instrukcí s proměnnou a symbolem."""
        if len(self.args) != 2:
            Error(f"Instruction {self.opcode} receives two arguments.", 32)

        self.CheckVariable(self.args[0])
        if self.args[1]["type"] == "var":   # Proměnná nebo konstanta
            self.CheckVariable(self.args[1])
        else:
            self.CheckSymbol(self.args[1])

    def InsLabSymbSymb(self):
        """Kontrola instrukcí s návěštím a dvouma symbolama."""
        if len(self.args) != 3:
            Error(f"Instruction {self.opcode} receives three arguments.", 32)

        if self.args[0]["type"] != "label":
            Error(f"Instruction {self.opcode} expects label as first argument.", 32)
        self.CheckName(self.args[0]["value"])

        for i in range(1, 3):   # Proměnná nebo konstanta poslední 2 argumenty
            if self.args[i]["type"] == "var":
                self.CheckVariable(self.args[i])
            else:
                self.CheckSymbol(self.args[i])

    def InsVarSymbSymb(self):
        """Kontrola instrukcí s proměnnou a dvouma symbolama."""
        if len(self.args) != 3:
            Error(f"Instruction {self.opcode} receives three arguments.", 32)

        self.CheckVariable(self.args[0])
        for i in range(1, 3):   # Proměnná nebo konstanta poslední 2 argumenty
            if self.args[i]["type"] == "var":
                self.CheckVariable(self.args[i])
            else:
                self.CheckSymbol(self.args[i])

    def InsVarType(self):
        """Kontrola instrukcí s proměnnou a typem."""
        if len(self.args) != 2:
            Error(f"Instruction {self.opcode} receives two arguments.", 32)

        self.CheckVariable(self.args[0])
        if self.args[1]["type"] == "type" and self.args[1]["value"] is not None:  # Kontrola typu
            if re.match(r'^(int|string|bool)$', self.args[1]['value']) is None:
                Error(f"Invalid 'type' '{self.args[1]['value']}'.", 32)
        else:
            Error(f"Instruction {self.opcode} expect type 'type' as second argument.", 32)


class XML:
    """Třída načte XML, seřadího a validuje."""
    def __init__(self, XMLfile):
        try:
            self.data = ET.fromstring(XMLfile)  # Naparsujeme XML
        except ET.ParseError:
            Error("Error while parsing XML data.", 31)

        self.CheckFormat()
        self.SortData()

    def CheckFormat(self):
        """Funkce zkontroluje hlavičku XML."""
        if self.data.tag != "program" or "language" not in self.data.attrib:
            Error("Wrong XML format.", 32)

        if self.data.attrib["language"] != "IPPcode19":
            Error("Language must be IPPcode19.", 32)

        for attr in self.data.attrib:
            if attr != "language" and attr != "name" and attr != "description":
                Error(f"Not allowed attribute {attr}.", 32)

    def SortData(self):
        """Funkce seřadí data a zkontroluje validnost podle zadání."""
        try:    # Seřazení instrukcí
            self.data[:] = sorted(self.data, key=lambda child: int(child.attrib["order"]))
        except TypeError:
            Error("Instruction must have attribute 'order'.", 32)

        order = 1
        for ins in self.data:   # S každou instrukcí
            if ins.tag != "instruction":    # Kontrola tagu instrukce
                Error("Instruction must have tag 'instruction'.", 32)

            ins[:] = sorted(ins, key=lambda child: child.tag)   # Seřazení argumentu
            if "opcode" not in ins.attrib:  # Kontrola atributu opcode v instrukci
                Error("Instruction must have attribute 'opcode'.", 32)

            if int(ins.attrib["order"]) != order:   # Kontrola očíslování instrukci
                Error(f"Missing value 'order={order}'.", 32)

            order += 1
            for attr in ins.attrib:      # Kontrola přebytečných atributů v instrukci
                if attr != "opcode" and attr != "order":
                    Error(f"Unexpected attribute {attr} in instruction.", 32)

            i = 1
            for arg in ins:              # S každým argumentem
                if "type" not in arg.attrib:    # Kontrola atibutu type.
                    Error("Argument must have attribute 'type'.", 32)

                for attr in arg.attrib:  # Kontrola přebytečných atributu v argumentu
                    if attr != "type":
                        Error(f"Unexpected attribute {attr} in argument.", 32)

                if arg.tag != ("arg" + str(i)):  # Kontrola tagu
                    Error(f"Missing or duplicit argument tag arg{i}.", 32)
                i += 1


def PrintHelp():
    """Funkce vypíše nápovědu pokud je o ní požádáno."""
    print("""Autor: Jakub Sadílek
        Login: xsadil07
        Arguments: --source='XMLfile' --input='InputFile'
        Script requires at least one argument.""")
    exit(0)


def LoadFile(filename):
    """Funkce načte obsah souboru a vrátího jako string."""
    output = ""
    try:
        if filename is not None:  # Předan soubor, jinak z STDIN
            with open(filename, 'r', encoding='utf-8') as data:
                for line in data:
                    output += line
        else:
            for line in sys.stdin:
                output += line
    except IOError:
        Error(f"An error occurred when opening file {filename}.", 11)
    return output


def ParseArgs():
    """Funkce validuje a obstará parametry interpretu."""
    global args
    parser = ArgumentParser(add_help=False)
    parser.add_argument('--help', action='store_true')
    parser.add_argument('--source')
    parser.add_argument('--input')
    args = parser.parse_args()

    if args.help:   # Výpis nápovědy
        if len(sys.argv) == 2:
            PrintHelp()
        else:
            Error("For help use --help only.", 10)
    elif not (args.source or args.input):   # Zadán aspoň jeden soubor.
        Error("Script requires at least one argument --source or --input. Use --help for help.", 10)

    XMLfile = LoadFile(args.source)
    if args.input:  # Pokud není zadán input file, budeme načítat z STDIN podle potřeby
        InputFile = LoadFile(args.input)
        InputFile = InputFile.split('\n')
    else:
        InputFile = None
    return XMLfile, InputFile


args = None                         # globání argumenty
XMLfile, InputFile = ParseArgs()    # Načteme obsahy souborů
XMLdata = XML(XMLfile)              # Naparsujeme XML
interp = Interpret()
instructions = []

for each in XMLdata.data:           # Naparsujeme instrukce
    instructions.append(Instruction(each))
    if each.attrib["opcode"] == "LABEL":
        interp.doLABEL(each)        # Návěští inicializujeme před interpretací

i = 0
while i < len(instructions):           # Samotná interpretace
    i = interp.doIns(instructions[i])  # Vracíme hodnoty kvůli příp. JUMPům
