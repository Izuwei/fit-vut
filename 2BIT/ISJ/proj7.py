#!/usr/bin/env python3
# AUTOR: Jakub Sadílek
# LOGIN: xsadil07
# DNE: 3.5.2018

class TooManyCallsError(Exception):
    '''Definice erroru vyuzite ve funkci limit_calls.'''
    def __init__(self, message):
        '''Inicializace.'''
        super(TooManyCallsError, self).__init__(message)

def limit_calls(max_calls = 2, error_message_tail = "called too often"):
    '''Omezuje maximalni limit volani funkce na 2.'''
    def decorator(function):
        function.counter = 0
        def inc_counter(*args, **kwargs):
            function.counter += 1
            if function.counter > max_calls:
                raise TooManyCallsError('function "' + function.__name__ + '" - ' + error_message_tail)
            return function(*args, **kwargs)
        return inc_counter
    return decorator

def ordered_merge(*args, **kwargs):
    '''Generatorova funkce, ktera je volana s libovolnym poctem interovatelnych objektu a param. selector, udavajicim, z ktereho inter. obj. ma prvek na dane pozici byt, a bude vybirat prvky v zadanem poradi.'''
    selector = kwargs.get("selector", [])
    output = []
    args = list(args)

    for each in range(len(args)):
        args[each] = iter(args[each])

    for each in selector:
        output.append(next(args[each]))

    return output

class Log():
    '''Trida Log'''
    def __init__(self, file_name):
        '''Inicializace tridy.'''
        self.file = open(file_name, "w")

    def logging(self, message):
        '''Pridani logu do souboru.'''
        self.file.write(message + "\n")

    def __enter__(self):
        '''Pri vstupu vypisujeme Begin.'''
        self.file.write("Begin\n")
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        '''Pri vystupu vypisujeme End.'''
        self.file.write("End\n")
        self.file.close()