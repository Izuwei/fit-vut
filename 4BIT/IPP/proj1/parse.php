<?php
// Autor: Jakub Sadilek
// Login: xsadil07
// Projekt obsahuje rozsireni STATP.

// Deklarace globalnich promennych pro rozsireni.
global $loc;
global $comm;
global $labels;
global $jumps;

// Trida instrukce
class instruction {
    public $opcode;             // Opcode instrukce
    public $arguments = [];     // List hodnot argumentu
    public $types = [];         // List typu argumentu

    // Konstruktor
    public function __construct($line) {
        global $jumps;
        $this->check_syntax($line);
        if ($this->opcode == "JUMP" || $this->opcode == "CALL" || $this->opcode == "JUMPIFEQ" || $this->opcode == "JUMPIFNEQ")
            $jumps++;
    }

    // Kontrola syntaxe (FSM)
    private function check_syntax($line) {
        $this->opcode = strtoupper($line[0]);   // Ulozime opcode
        switch ($this->opcode){
            case "MOVE":
            case "INT2CHAR":
            case "TYPE":
            case "STRLEN":
            case "NOT":
                return $this->ins_v_s($line);   // <var> <symb>
            case "CREATEFRAME":
            case "PUSHFRAME":
            case "POPFRAME":
            case "RETURN":
            case "BREAK":
                return $this->ins_noop($line);  // Zadne operandy
            case "DEFVAR":
            case "POPS":
                return $this->ins_v($line);     // <var>
            case "CALL":
            case "LABEL":
            case "JUMP":
                return $this->ins_l($line);     // <label>
            case "PUSHS":
            case "WRITE":   // BOOL?
            case "EXIT":
            case "DPRINT":
                return $this->ins_s($line);     // <symb>
            case "ADD":
            case "SUB":
            case "MUL":
            case "IDIV":
            case "EQ":
            case "GT":
            case "LT":
            case "AND":
            case "OR":
            case "CONCAT":
            case "GETCHAR":
            case "SETCHAR":
            case "STRI2INT":
                return $this->ins_v_s_s($line);  // Aritm. instrukce
            case "READ":
                return $this->ins_v_t($line);   // <var> <type>
            case "JUMPIFEQ":
            case "JUMPIFNEQ":
                return $this->ins_l_s_s($line);  // <label> <symb> <symb>
            default:    // Error, neplatny opcode
                error("Neznama instrukce $this->opcode.", 22);
        }
    }

    // Instrukce CREATEFRAME, PUSHFRAME, POPFRAME, RETURN a BREAK.
    private function ins_noop($line){
        if (count($line) !== 1)     // Kontrola poctu operandu
            error("Instrukce $line[0] neprijma zadne operandy.", 23);
    }

    // Funkce zkontroluje platnost jmena, pripadne ukonci program.
    private function check_name($name){
        if (!preg_match('#^[A-Za-z_\-$&%*!?][A-z_\-$&%*!?0-9]*$#', $name))
            error("Neplatny nazev $name.", 23); // Kontrola nazvu regexem
    }

    // Funkce zkontroluje jestli jde o platnou konstantu, jinak ukonci program.
    private function check_const($constant){    // ZKONROLOVAT NIL A DODELAT STRING.
        $const_list = explode('@', $constant, 2);
        if (count($const_list) !== 2)   // Konstanta ma dve casti oddelene @.
            error("Spatny format konstanty $constant", 23);
        array_push($this->arguments, $const_list[1]);   // Ulozime hodnotu.
        array_push($this->types, $const_list[0]);       // Ulozime typ.

        if ($const_list[0] == "int"){    // Kontrola cisla.
            if (!preg_match('#^[+-]?[0-9]+$#', $const_list[1]))
                error("$const_list[1] neni platne cislo.", 23);
        }
        elseif ($const_list[0] == "string"){ // Kontrola stringu.
            if (!preg_match('#^(\\\\\d{3,}|[^\\\\\s])*$#', $const_list[1]))
                error("Neplatna hodnota stringu $const_list[1].", 23); // Kontrola stringu regexem
        }
        elseif ($const_list[0] == "bool"){   // Kontrola pravdivostni hodnoty
            if ($const_list[1] !== "true" && $const_list[1] !== "false")
                error("$const_list[1] neni platny boolean typ.", 23);
        }
        elseif ($const_list[0] == "nil"){   // Kontrola typu nil
            if ($const_list[1] !== "nil")
                error("$const_list[1] neni platna hodnota pro typ nil.", 23);
        }
        else    // Neplatny typ konstanty -> konec
            error("Neznamy typ konstanty: $const_list[0]", 23);
    }

    // Funkce zkontroluje spravnou definici promenne.
    private function check_var($variable){
        $var_list = explode('@', $variable);
        if (count($var_list) !== 2)     // Kontrola casti promenne
            error("Spatny format promenne $variable.", 23);
        if ($var_list[0] !== "GF" && $var_list[0] !== "LF" && $var_list[0] != "TF")
            error("Neznamy ramec $var_list[0] u promenne $variable.", 23);   // Kontrola definice ramce       
        $this->check_name($var_list[1]);    // Kontrola jmena promenne
        array_push($this->types, "var");    // Ulozime typ promenne -> var
        array_push($this->arguments, $variable);    // Ulozime nazev promenne vc. ramce
    }

    // Funkce zkontroluje zda zadany operand odpovida promenne nebo zadanemu typu.
    private function check_symb($operand){
        $type = explode('@', $operand)[0];  // Rozdelime na typ a hodnotu
        if ($type == "GF" || $type == "LF" || $type == "TF")
            $this->check_var($operand);    // Kontrola promenne
        else
            $this->check_const($operand);   // Kontrola konstanty
    }

    // Instrukce MOVE, TYPE, NOT, STRLEN a INT2CHAR.
    private function ins_v_s($line){    // <var> <symb>
        if (count($line) !== 3)     // Kontrola poctu operandu.
            error("Spatny pocet operandu instrukce $line[0].", 23);
        $this->check_var($line[1]);    // Kontrola prvniho operandu.
        $this->check_symb($line[2]);
    }

    // Instrukce DEFVAR a POPS.
    private function ins_v($line){  // <var>
        if (count($line) !== 2)  // Kontrola poctu operandu.
            error("Spatny pocet operandu instrukce $line[0].", 23);
        $this->check_var($line[1]);     // Kontrola operandu.
    }

    // Instrukce CALL, LABEL a JUMP.
    private function ins_l($line){  // <label>
        global $labels;
        if (count($line) !== 2) // Kontrola poctu operandu
            error("Spatny pocet operandu instrukce CALL.", 23); 
        if ($this->opcode == "LABEL")
            $labels++;    
        $this->check_name($line[1]);   // Kontrola nazvu navesti
        array_push($this->types, "label");       // Ulozime typ
        array_push($this->arguments, $line[1]);  // Ulozime nazev
    }

    // Instrukce PUSHS, WRITE, EXIT a DPRINT.
    private function ins_s($line){       // <symb>.
        if (count($line) !== 2)  // Kontrola poctu operandu
            error("Spatny pocet operandu instrukce $line[0].", 23);
        $this->check_symb($line[1]);   // Kontrola symbolu
    }

    // Instrukce ADD, SUB, MUL, IDIV, EQ, GT, LT, AND, OR, 
    // AND, OR, STRI2INT, CONCAT, GETCHAR a SETCHAR.
    private function ins_v_s_s($line){  // <var> <symb> <symb>
        if (count($line) !== 4)  // Kontrola poctu operandu
            error("Spatny pocet operandu instrukce $line[0].", 23);
        $this->check_var($line[1]);    // Kontrola prvniho operandu
        $this->check_symb($line[2]);  // Kontrola druheho operandu
        $this->check_symb($line[3]);  // Kontrola tretiho operandu
    }

    // Instrukce READ.
    private function ins_v_t($line){    // <var> <type>
        if (count($line) !== 3)  // Kontrola poctu parametru.
            error("Spatny pocet operandu instrukce READ.", 23);
        $this->check_var($line[1]);    // Kontrola prvniho parametru.
        if ($line[2] !== "int" && $line[2] !== "string" && $line[2] !== "bool") // Kontrola druheho.
            error("Druhy parametr instrukce READ musi byt typ: int, string nebo bool.", 23);
        array_push($this->types, "type");   // Ulozime typ druheho op.
        array_push($this->arguments, $line[2]); // Ulozime hodnotu druheho op.
    }

    // Instrukce JUMPIFEQ a JUMPIFNEQ.
    private function ins_l_s_s($line){  // <label> <symb> <symb>
        if (count($line) !== 4)  // Kontrola poctu operandu.
            error("Spatny pocet operandu instrukce $line[0].", 23);
        $this->check_name($line[1]);   // Kontrola navesti.
        array_push($this->types, "label");
        array_push($this->arguments, $line[1]);
        $this->check_symb($line[2]);  // Kontrola druheho operandu
        $this->check_symb($line[3]);  // Kontrola tretiho operandu
    }
}


// Trida XML formatu.
class domXML {
    private $xml;
    private $program;
    
    // Konstruktor
    public function __construct(){
        $this->xml = new DomDocument('1.0', 'UTF-8');   // Hlavicka
        $this->xml->formatOutput = true;    // Formatovani
        $this->program = $this->xml->createElement("program");
        $this->program->setAttribute("language", "IPPcode19");
        $this->xml->appendChild($this->program);    // Pridani elem. program
    }

    // Pridani instrukce do XML.
    public function add_ins($ins){
        static $calls = 1;  // Pocitadlo operandu
        $instruction = $this->xml->createElement("instruction");    // Elem. instrukce
        $instruction->setAttribute("order", $calls);    // Pridame atributy
        $instruction->setAttribute("opcode", $ins->opcode);
        $this->program->appendChild($instruction);  // Pridani elem. instruction

        $ins->arguments = str_replace("&", "&amp;", $ins->arguments);   // Spec. znaky
        $ins->arguments = str_replace("<", "&lt;", $ins->arguments);
        $ins->arguments = str_replace(">", "&gt;", $ins->arguments);
        $ins->arguments = str_replace('"', "&quot;", $ins->arguments);
        $ins->arguments = str_replace("'", "&apos;", $ins->arguments);

        $argnum = count($ins->types) + 1;
        for ($i = 1; $i < $argnum; $i++){   // Pro kazdy operand
            $argument = $this->xml->createElement("arg$i", $ins->arguments[$i-1]);
            $argument->setAttribute("type", $ins->types[$i-1]);
            $instruction->appendChild($argument);
        }
        $calls++;
    }

    // Vypis XML.
    public function print_data(){
        echo $this->xml->saveXML(NULL, LIBXML_NOEMPTYTAG);
    }
}


// Funkce vypise chybovou hlasku 'msg' a ukonci program cislem 'code'.
function error($msg, $code){
    fwrite(STDERR, "$msg\n");
    exit($code);
}

// Funkce odstrani komentare a prebytecne mezery na radce.
function strip_line($line){
    global $comm;
    if (($pos = strpos($line, "#")) !== false){  // Odstranime komentare
        $line = substr($line, 0, $pos);
        $comm++;
    }
    $line = preg_replace('#[\s\t]+#', ' ', $line);  // Odstranime prebytecne mezery
    return $line;
}

// Vypiseme napovedu.
function print_help(){
    echo "Autor: Jakub Sadilek, xsadil07
    Skript nacte ze standardniho vstupu zdrojovy kod v jazyku IPPcode19,
    zkontroluje lexikalni a syntaktickou spravnost kodu a vypise na 
    standardni vystup XML reprezentaci programu dle zadani.
    Pripadne vrati chybovou hlasku vcetne odpovidajici navratove hodnoty.\n";
    exit(0);
}

// Funkce vypise statistiky do souboru pokud je pozadano.
function print_stats(){
    global $file;
    global $options;
    global $loc;
    global $comm;
    global $labels;
    global $jumps;

    foreach($options as $key => $opt){
        switch ($key){
            case "stat":
                break;
            case "loc":         // Lines of code
                fwrite($file, "$loc\n");
                break;
            case "comments":    // Komentare
                fwrite($file, "$comm\n");
                break;
            case "labels":      // Navesti
                fwrite($file, "$labels\n");
                break;
            case "jumps":       // Skoky
                fwrite($file, "$jumps\n");
                break;
        }
    }
    fclose($file);
}

// Kontrola argumentu. Vraci FALSE pokud nejsou pozadovany statistiky, jinak TRUE.
function check_args($argc, $argv){
    global $options;
    global $file;

    if ($argc == 1)     // Zadne argumenty
        return false;
    elseif ($argc == 2 && $argv[1] == "--help")  // Napoveda
        print_help();

    $options = getopt(null, ["stats:", "loc", "comments", "labels", "jumps"]);
    if ((count($options) + 1) !== $argc)    // Kontrola spravnosti argumentu
        error("Prebytecny nebo neznamy argument.", 10);
    elseif (!array_key_exists("stats", $options))   // Povinny arg. stats=file
        error("Chybi argument stats=", 10);
    else {      // Spravne argumenty
        if (($file = @fopen($options["stats"], 'w')) == false)   // Otevreme pro zapis stat
            error("Chyba pri otevreni souboru. Mozna spatna prava.", 12);
        else
            return true;
    }
}

$loc = 0;   // Pocatecni inicializace statistik
$comm = 0;
$labels = 0;
$jumps = 0;

$stats = check_args($argc, $argv);

if (strtolower(trim(strip_line(fgets(STDIN)))) !== ".ippcode19")  // Kontrola shebangu
    error("Spatny nebo chybejici shebang IPPcode19.", 21);

$xml = new domXML();    // Vytvorime XML

while($line = fgets(STDIN)){   // Nacitame z STDIN
    if (strlen($line = trim(strip_line($line))) == 0)
        continue;   // Pokracujeme pokud nacteny radek je prazdny.
    $line = explode(' ', $line);    // Vytvorime list
    $ins = new instruction($line);  // Nacteme instrukci
    $xml->add_ins($ins);            // Pridame do XML
    $loc++;
}

$xml->print_data();     // Vypiseme XML
if ($stats == true)
    print_stats();      // Vypiseme statistiky
?>
