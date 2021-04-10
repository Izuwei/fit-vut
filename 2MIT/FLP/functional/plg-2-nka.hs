-- FIT VUT

-- PLG-2-NKA

-- Autor: Jakub Sadilek
-- Login: xsadil07
-- Datum: 11.4.2021

import System.Environment
import System.IO
import Data.List

type Nonterm = String
type Term = String

-- L -> xR, L ∈ N, R ∈ N, x ∈ T*
type Rule = (Nonterm, [String])

-- G = { N, T, R, S }
data Grammar = Grammar { nonterms :: [Nonterm]   -- N
                       , terms :: [Term]         -- T
                       , rules :: [Rule]         -- P
                       , initNonterm :: Nonterm  -- S
                       } deriving (Show, Eq)

type State = Integer
type Symbol = String

-- qa -> p, q ∈ Q, p ∈ Q, a ∈ E
data Transition = Transition { state :: State
                             , symbol :: Symbol
                             , nextState :: State
                             } deriving (Show, Eq)

-- A = { Q, E, R, q0, F }
data FSM = FSM { states :: [State]              -- Q
               , alphabet :: [Symbol]           -- E
               , transitions :: [Transition]    -- R
               , initState :: State             -- q0
               , finalStates :: [State]         -- F
               } deriving (Show, Eq)

-- Funkce vrati dvojici (priznak, nazevSouboru) podle vstupnich param. programu.
parseArgs :: Num a => [[Char]] -> (a, [Char])
parseArgs args =
    case args of
        ["-i"] -> (0, [])
        ["-1"] -> (1, [])
        ["-2"] -> (2, [])
        ["-i", file] -> (0, file)
        ["-1", file] -> (1, file)
        ["-2", file] -> (2, file)
        _ -> error "Unexpected program parameters."

-- Funkce nacte obsah podle formatu parametru pro soubor.
loadInput :: [Char] -> IO String
loadInput file
    | null file = getContents       -- Nezadan soubor, cteme od uzivatele (stdin)
    | otherwise = readFile file     -- nacteme ze souboru

-- Funkce rozdeli retezec podle zadaneho oddelovace, pote vrati list stringu.
split :: Char -> [Char] -> [[Char]]
split sep str =
    case break (==sep) str of
        (a, sep:b) -> a : split sep b
        (a, "") -> [a]

-- Funkce dostane list stringu (radku ze souboru) a prevede ho na gramatiku.
loadGrammar :: [[Char]] -> Grammar
loadGrammar (nonterms : terms : initNonterm : rules) =
    Grammar (checkDuplicities (checkNonterms (getSymbols nonterms)))   -- Neterminaly
            (checkDuplicities (checkTerms (getSymbols terms)))         -- Terminaly
            (checkDuplicities (checkRules (getRules rules)))           -- Pravidla
            (checkNonterm initNonterm)                                 -- Poc. neterminal
    where
        getSymbols line = split ',' line

        -- Funkce zkontroluje, jestli zadany symbol patri do mnoziny neterminalu.
        checkNonterm symbol
            | symbol `elem` getSymbols nonterms = symbol
            | otherwise = error ("Symbol '" ++ symbol ++ "' is not nonterminal.")

        -- Funkce dostane radky ze vstupu a kazdy prevede na strukturu pravidla (N, [N u T]).
        getRules lines = [splitRule line | line <- lines]

        -- Funkce rozdeli pravidlo po znaku sipka (->) a vrati obe strany jako dvojici.
        splitRule rule@(x:y:z:xs)
            | [y, z] == "->" = ([x], map (: []) xs)
            | otherwise = error ("Unknown rule format: " ++ rule)

        splitRule rule = error ("Unknown rule format: " ++ rule)

        -- Funkce zkontroluje, jestli symbol je dlouhy prave jeden znak.
        isOne symbol
            | length symbol == 1 = symbol
            | otherwise = error ("Length of symbol '" ++ symbol ++ "' must be 1.")

        -- Funkce zkontroluje, jestli list neobsahuje duplicity, protoze v mnozine nejsou povoleny.
        checkDuplicities [] = []
        checkDuplicities (x:xs)
            | x `elem` xs = error "Duplicities are not allowed."
            | otherwise = x : checkDuplicities xs

        -- Funkce zkontruluje, jestli list neterminalu odpovida tvaru ze zadani.
        checkNonterms symbols = [map isUppercase (isOne symbol) | symbol <- symbols]
            where
                isUppercase symbol
                    | symbol `elem` ['A' .. 'Z'] = symbol
                    | otherwise = error ("Nonterminal '" ++ [symbol] ++ "' must be in range A-Z.")

        -- Funkce zkontruluje, jestli list terminalu odpovida tvaru ze zadani.
        checkTerms symbols = [map isLowercase (isOne symbol) | symbol <- symbols]
            where
                isLowercase symbol
                    | symbol `elem` ['a' .. 'z'] = symbol
                    | otherwise = error ("Terminal '" ++ [symbol] ++ "' must be in range a-z.")

        -- Funkce zkontroluje, jestli pravidla jsou ve tvaru PLG.
        checkRules rls = [(checkNonterm (fst rule), checkRight (snd rule)) | rule <- rls]
            where
                isAllTerm [] = True     -- Kontrola, jestli list obsahuje pouze terminaly. Vraci bool.
                isAllTerm [t] = True
                isAllTerm (t:ts)
                    | t `notElem` getSymbols terms = False
                    | otherwise = isAllTerm ts
                
                -- Funkce zkontroluje pravou stranu pravidla (xyz.., aB).
                checkRight [] = error "Bad right side of rule."
                checkRight nut =
                    if (last nut `elem` getSymbols nonterms && length nut /= 1)
                      || last nut `elem` getSymbols terms
                      || (last nut == "#" && length nut == 1) then

                        if isAllTerm nut then
                            nut
                        else
                            error (concat nut ++ " contains bad symbols on the wrong positions.")

                    else
                        error (concat nut ++ " is unsupported right side of the rule.")

loadGrammar _ = error "Insufficient input."

-- Funkce vypise gramatiku ve tvaru podle zadani.
showGrammar :: Grammar -> IO ()
showGrammar grammar = do
    putStrLn (intercalate "," (nonterms grammar))
    putStrLn (intercalate "," (terms grammar))
    putStrLn (initNonterm grammar)
    mapM_ (\rule -> putStrLn (fst rule ++ "->" ++ concat (snd rule))) (rules grammar)

-- Funkce dostane pocatecni cislo, neterminal a list neterminalu.
-- Pote vrati novy neterminal, ktery neni obsazen v zadanem listu neterminalu.
nontermGenerator :: Integer -> [Char] -> [[Char]] -> [Char]
nontermGenerator n nonterm nonterms
    | (head nonterm : show n) `elem` nonterms = nontermGenerator (n+1) nonterm nonterms
    | otherwise = head nonterm : show n

-- Funkce dostane pravidla gramatiky, neterminaly a terminaly.
-- Pak vrati list vsech neterminalu, ktere jsou obsazeny v pravidlech + zadanem listu.
getNonterms :: [[Char]] -> [[Char]] -> [([Char], [[Char]])] -> [[Char]]
getNonterms nonterms terms rules = sort $ removeDuplicities $ nonterms ++ getNontermList terms rules
    where
        getNontermList terms [] = []
        getNontermList terms (r:rs) = [fst r] ++ filter (`notElem` (terms ++ ["#"])) (snd r) ++ getNontermList terms rs
        removeDuplicities [] = []
        removeDuplicities (x:xs)
            | x `elem` xs = removeDuplicities xs
            | otherwise = x : removeDuplicities xs

-- Funkce transformuje gramatiku podle TIN algoritmu ve vete 3.2.
transformGrammar :: Grammar -> Grammar
transformGrammar grammar =
    Grammar (getNonterms (nonterms grammar) (terms grammar) transformedRules)  -- Neterminaly
            (terms grammar)                                 -- Terminaly
            transformedRules                                -- Pravidla
            (initNonterm grammar)                           -- Pocatecni neterminal
    where
        -- Nova transformovana pravidla
        transformedRules = transformRules (nonterms grammar) (rules grammar)

        -- Transformuje dlouha pravidla do sekvence kratkych pravidel.
        -- Vytvori nove pravidlo a rerekurzivne opakuje proces pro zbytek daneho pravidla.
        cascadeRule nonterms (l, r) =
            (newNonterms, (l, [head r, newNonterm]) : newRules)
            where
                newNonterm = nontermGenerator 1 l nonterms -- novy neterminal
                (newNonterms, newRules) = transformRule (nonterms ++ [newNonterm]) (newNonterm, tail r)

        -- Transformuje ukoncujici pravidlo jako X->a na X->aX1, X1->eps.
        -- Vrati dvojici (noveNeterminaly, novaPravidla).
        transformTerminalRule nonterms (l, r) =
            (nonterms ++ [newNonterm], [(l, [head r, newNonterm]), (newNonterm, ["#"])])
            where
                newNonterm = nontermGenerator 1 l nonterms

        -- Funkce transformuje jedno zadane pravidlo.
        -- Vrati dvojici jako (noveNeterminaly, novaPravidla).
        transformRule nonterms rule@(l, r)
            | length r == 1 && last r == "#" = (nonterms, [rule])   -- X->#
            | length r == 1 = transformTerminalRule nonterms rule   -- X->a
            | length r == 2 && last r `elem` nonterms = (nonterms, [rule])  -- X->aB
            | otherwise = cascadeRule nonterms rule     -- ostatni dlouha pravidla X->aaaa, X->aaaaB

        -- Funkce dostane list pravidel, transformuje je a pak vrati nova pravidla.
        transformRules _ [] = []
        transformRules nonterms [rule] = snd $ transformRule nonterms rule
        transformRules nonterms (rule:rules) =  newRules ++ transformRules newNonterms rules
            where
                (newNonterms, newRules) = transformRule nonterms rule

-- Funkce prevede gramatiku na konecny automat.
loadFSM :: Grammar -> FSM
loadFSM grammar =
    FSM ([i | (_, i) <- convTable])                 -- stavy
        (terms grammar)                             -- abeceda
        (getTransitions (rules grammar))            -- prechody
        (getState (initNonterm grammar) convTable)  -- poc. stav
        (getFinalStates (rules grammar))            -- list ukoncujicich stavu
    where
        convTable = nontermsToStates (nonterms grammar) -- tabulka prevodu neterm. na stavy

        -- Funkce dostane list neterminalu a kazdemu neterminalu priradi unikatni cislo.
        -- Cislovani zacina od 1 a s kazdym neterm. se zvysuje. Vraci list dvojic.
        nontermsToStates = numberStates 1
            where
                numberStates _ [] = []
                numberStates n (x:xs) = (x, n) : numberStates (n+1) xs

        -- Funcke dostane neterm. a prevadeci tabulku a vrati stav, ktery odpovida zadanemu neterm.
        getState nonterm [] = error ("Unknown Nonterm: " ++ nonterm)
        getState nonterm ((n, i):xs)
            | nonterm == n = i
            | otherwise = getState nonterm xs

        -- Funkce prevede pravidlo gramatiky na prechod automatu.
        getTransition (l, r) = Transition (getState l convTable)
                                          (head r)
                                          (getState (last r) convTable)

        -- Funkce prevede list pravidel na list prechodu.
        getTransitions [] = []
        getTransitions (rule:rules)
            | length (snd rule) == 1 = getTransitions rules   -- preskoc ukoncujici pravidla X->#
            | otherwise = getTransition rule : getTransitions rules

        -- Funkce z gramatickych pravidel vrati vsechny ukoncujici stavy automatu.
        getFinalStates rules = map (\(a, _) -> getState a convTable) (filter (\(_, y) -> last y == "#") rules)

-- Funkce vypise konecny automat ve tvaru podle zadani.
showFSM :: FSM -> IO ()
showFSM fsm = do
    putStrLn (intercalate "," [show state | state <- states fsm])       -- stavy
    putStrLn (concat (alphabet fsm))                                    -- abeceda
    print (initState fsm)                                               -- poc. stav
    putStrLn (intercalate "," [show state | state <- sort (finalStates fsm)])  -- konecne stavy
    mapM_ putStrLn $ sort [intercalate "," [show (state t), symbol t, show (nextState t)] | t <- transitions fsm]   -- prechody

main :: IO ()
main = do
    args <- getArgs

    let (option, file) = parseArgs args
    content <- fmap lines $ loadInput file

    let grammar = loadGrammar content

    case option of
        0 -> showGrammar grammar                            -- -i
        1 -> showGrammar $ transformGrammar grammar         -- -1
        2 -> showFSM $ loadFSM $ transformGrammar grammar   -- -2
