-- FIT VUT
-- Author: Jakub Sadilek
-- Login: xsadil07

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

-- Function returns pair (index, filename) according to program parameters.
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

-- Function loads content according to the format of file parameter.
loadInput :: [Char] -> IO String
loadInput file
    | null file = getContents       -- No file, so get it from user
    | otherwise = readFile file     -- Load from file

-- Function splits string by given separator, then returns list of strings.
split :: Char -> [Char] -> [[Char]]
split sep str =
    case break (==sep) str of
        (a, sep:b) -> a : split sep b
        (a, "") -> [a]

-- Function gets a list of strings (from file) and convert it into a grammar.
loadGrammar :: [[Char]] -> Grammar
loadGrammar (nonterms : terms : initNonterm : rules) =
    Grammar (checkNonterms (getSymbols nonterms))   -- Nonterms
            (checkTerms (getSymbols terms))         -- Terms
            (checkRules (getRules rules))           -- Rules
            (checkNonterm initNonterm)              -- initial nonterm
    where
        getSymbols line = split ',' line

        -- Function checks if given symbol is in set of nonterminals.
        checkNonterm symbol
            | symbol `elem` getSymbols nonterms = symbol
            | otherwise = error ("Symbol '" ++ symbol ++ "' is not nonterminal.")

        -- Function get lines from input and parse them into rule struct (N, [N u T]).
        getRules lines = [splitRule line | line <- lines]

        -- Function splits rule on arrow (->) and returns both sides in pair.
        splitRule rule@(x:y:z:xs)
            | [y, z] == "->" = ([x], map (: []) xs)
            | otherwise = error ("Unknown rule format: " ++ rule)

        splitRule rule = error ("Unknown rule format: " ++ rule)

        -- Function checks if symbol is not empty. If is empty - error is raised.
        isNotEmpty symbol
            | null symbol = error "Empty symbols are not allowed."
            | otherwise = symbol

        -- Function checks if list of symbols is full of nonterms according to assignment.
        checkNonterms symbols = [map isUppercase (isNotEmpty symbol) | symbol <- symbols]
            where
                isUppercase symbol
                    | symbol `elem` ['A' .. 'Z'] = symbol
                    | otherwise = error ("Nonterminal '" ++ [symbol] ++ "' must be in range A-Z.")

        -- Function checks if list of symbols is full of terms according to assignment.
        checkTerms symbols = [map isLowercase (isNotEmpty symbol) | symbol <- symbols]
            where
                isLowercase symbol
                    | symbol `elem` ['a' .. 'z'] = symbol
                    | otherwise = error ("Terminal '" ++ [symbol] ++ "' must be in range a-z.")

        -- Function checks if rules are in shape of right linear grammar.
        checkRules rls = [(checkNonterm (fst rule), checkRight (snd rule)) | rule <- rls]
            where
                isAllTerm [] = True     -- Function checks if list of strings is full of terms, then returns bool.
                isAllTerm [t] = True
                isAllTerm (t:ts)
                    | t `notElem` getSymbols terms = False
                    | otherwise = isAllTerm ts
                
                -- Functions checks right side of the rule (xyz.., aB).
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

-- Function prints grammar according the assignment.
showGrammar :: Grammar -> IO ()
showGrammar grammar = do
    putStrLn (intercalate "," (nonterms grammar))
    putStrLn (intercalate "," (terms grammar))
    putStrLn (initNonterm grammar)
    mapM_ (\rule -> putStrLn (fst rule ++ "->" ++ concat (snd rule))) (rules grammar)

-- Function gets starting number, nonterm and list of nonterms.
-- Then functions returns a new nonterm that is not in given list of nonterms.
nontermGenerator :: Integer -> [Char] -> [[Char]] -> [Char]
nontermGenerator n nonterm nonterms
    | (head nonterm : show n) `elem` nonterms = nontermGenerator (n+1) nonterm nonterms
    | otherwise = head nonterm : show n

-- Functions gets grammar rules and terms.
-- Then returns all nonterms, which are included in rules.
getNonterms :: [[Char]] -> [([Char], [[Char]])] -> [[Char]]
getNonterms terms rules = sort $ removeDuplicities $ getNontermList terms rules
    where
        getNontermList terms [] = []
        getNontermList terms (r:rs) = [fst r] ++ filter (`notElem` (terms ++ ["#"])) (snd r) ++ getNontermList terms rs
        removeDuplicities [] = []
        removeDuplicities (x:xs)
            | x `elem` xs = removeDuplicities xs
            | otherwise = x : removeDuplicities xs

-- Function transforms grammar according TIN algorithm in sentence 3.2.
transformGrammar :: Grammar -> Grammar
transformGrammar grammar =
    Grammar (getNonterms (terms grammar) transformedRules)  -- Nonterms
            (terms grammar)                                 -- Terms
            transformedRules                                -- Rules
            (initNonterm grammar)                           -- Initial nonterm
    where
        -- New transformed rules
        transformedRules = transformRules (nonterms grammar) (rules grammar)

        -- Transform long rule into sequence of short rules.
        -- Creates a new rule and recursively do this for rest of the given rule.
        cascadeRule nonterms (l, r) =
            (newNonterms, (l, [head r, newNonterm]) : newRules)
            where
                newNonterm = nontermGenerator 1 l nonterms -- newNonterm
                (newNonterms, newRules) = transformRule (nonterms ++ [newNonterm]) (newNonterm, tail r)

        -- Function transforms simple terminal rule like X->a into X->aX1, X1->eps.
        -- Returned in pair (new Nonterms, new Rules).
        transformTerminalRule nonterms (l, r) =
            (nonterms ++ [newNonterm], [(l, [head r, newNonterm]), (newNonterm, ["#"])])
            where
                newNonterm = nontermGenerator 1 l nonterms

        -- Function transform a single rule.
        -- Returns pair as (new Nonterms, new Rule).
        transformRule nonterms rule@(l, r)
            | length r == 1 && last r == "#" = (nonterms, [rule])   -- Terminal rule X->#
            | length r == 1 = transformTerminalRule nonterms rule   -- Simple rule X->a. For not eps rules use this: (nonterms, [rule])
            | length r == 2 && last r `elem` nonterms = (nonterms, [rule])  -- X->aB
            | otherwise = cascadeRule nonterms rule     -- Other long rules like X->aaaa, X->aaaaB

        -- Function gets list of rules and transform them, then new rules are returned.
        transformRules _ [] = error "There is no rules."
        transformRules nonterms [rule] = snd $ transformRule nonterms rule
        transformRules nonterms (rule:rules) =  newRules ++ transformRules newNonterms rules
            where
                (newNonterms, newRules) = transformRule nonterms rule

-- Functions covenrts grammar into FSM.
loadFSM :: Grammar -> FSM
loadFSM grammar =
    FSM ([i | (_, i) <- convTable])                 -- states
        (terms grammar)                             -- alphabet
        (getTransitions (rules grammar))            -- transitions
        (getState (initNonterm grammar) convTable)  -- initial state
        (getFinalStates (rules grammar))            -- list of terminal states
    where
        convTable = nontermsToStates (nonterms grammar) -- Map nonterms into states

        -- Function gets list of nonterms and each nonterm is associated with number.
        -- Numbers starts from 1 and increases with each nonterm. Retured as list of pairs.
        nontermsToStates = numberStates 1
            where
                numberStates _ [] = []
                numberStates n (x:xs) = (x, n) : numberStates (n+1) xs

        -- Function gets nonterm and convTable and returns state, which corresponds to given nonterm.
        getState nonterm [] = error ("Unknown Nonterm: " ++ nonterm)
        getState nonterm ((n, i):xs)
            | nonterm == n = i
            | otherwise = getState nonterm xs

        -- Function convert single rule into transition.
        getTransition (l, r) = Transition (getState l convTable)
                                          (head r)
                                          (getState (last r) convTable)

        -- Function converts list of rules into list of transitions.
        getTransitions [] = []
        getTransitions (rule:rules)
            | length (snd rule) == 1 = getTransitions rules   -- skip terminal rules X->#
            | otherwise = getTransition rule : getTransitions rules

        -- Function returns all terminal states from grammar rules.
        getFinalStates rules = map (\(a, _) -> getState a convTable) (filter (\(_, y) -> last y == "#") rules)

-- Function prints FSM according the assignment.
showFSM :: FSM -> IO ()
showFSM fsm = do
    putStrLn (intercalate "," [show state | state <- states fsm])       -- states
    putStrLn (concat (alphabet fsm))                                    -- alphabet
    print (initState fsm)                                               -- initial state
    putStrLn (intercalate "," [show state | state <- finalStates fsm])  -- final states
    mapM_ putStrLn $ sort [intercalate "," [show (state t), symbol t, show (nextState t)] | t <- transitions fsm]   -- transitions

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

    -- putStr (show $ transformGrammar grammar)
    -- showFSM fsm
    -- putStr (show $ grammar)
    -- putStr (show $ loadFSM (Grammar {nonterms = ["A","B"], terms = ["A","A1","A2","B"], rules = [("A",["a","A1"]),("A1",["a","B"]),("A",["c","A2"]),("A2",["c","B"]),("B",["b","B"]),("B",["#"])], initNonterm = "A"}))
