import System.Environment
import System.IO
import Data.List
import Data.Bifunctor

type Nonterm = String
type Term = String

-- L -> xR, L ∈ N, R ∈ N, x ∈ T*
type Rule = (Nonterm, [String])

-- G = { N, T, R, S }
data Grammar = Grammar { nonterms :: [Nonterm]  -- N
                       , terms :: [Term]        -- T
                       , rules :: [Rule]        -- P
                       , initNonterm :: Nonterm        -- S
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

loadInput :: [Char] -> IO String
loadInput file
    | null file = getContents
    | otherwise = readFile file

split :: Char -> [Char] -> [[Char]]
split sep str =
    case break (==sep) str of
        (a, sep:b) -> a : split sep b
        (a, "") -> [a]

loadGrammar :: [[Char]] -> Grammar
loadGrammar (nonterms : terms : initNonterm : rules) =
    Grammar (checkNonterms (getSymbols nonterms))
            (checkTerms (getSymbols terms))
            (checkRules (getRules rules))
            (checkNonterm initNonterm)        -- Udělat kontroly na všechno
    where
        getSymbols line = split ',' line

        checkNonterm symbol
            | symbol `elem` getSymbols nonterms = symbol
            | otherwise = error ("Symbol '" ++ symbol ++ "' is not nonterminal.")

        getRules lines = [splitRule line | line <- lines]

        splitRule rule@(x:y:z:xs)
            | [y, z] == "->" = ([x], map (: []) xs)
            | otherwise = error ("Unknown rule format: " ++ rule)

        splitRule rule = error ("Unknown rule format: " ++ rule)

        checkNonterms symbols = [map isUppercase symbol | symbol <- symbols]
            where
                isUppercase symbol
                    | symbol `elem` ['A' .. 'Z'] = symbol
                    | otherwise = error ("Nonterminal '" ++ [symbol] ++ "' must be in range A-Z.")

        checkTerms symbols = [map isLowercase symbol | symbol <- symbols]
            where
                isLowercase symbol
                    | symbol `elem` ['a' .. 'z'] = symbol
                    | otherwise = error ("Terminal '" ++ [symbol] ++ "' must be in range a-z.")

        checkRules rls = [Data.Bifunctor.bimap checkNonterm checkRight rule | rule <- rls]
            where
                isAllTerm [] = True
                isAllTerm [t] = True
                isAllTerm (t:ts)
                    | t `notElem` getSymbols terms = False
                    | otherwise = isAllTerm ts
                checkRight nut =
                    if (last nut `elem` getSymbols nonterms && length nut /= 1)
                      || last nut `elem` getSymbols terms
                      || (last nut == "#" && length nut == 1) then      -- Opravit že levy neterminal se nenachazi na zadne prave strane?

                        if isAllTerm nut then
                            nut
                        else
                            error (concat nut ++ " contains bad symbols on the wrong positions.")

                    else
                        error (concat nut ++ " is unsupported right side of the rule.")

showGrammar :: Grammar -> IO ()
showGrammar grammar = do
    putStrLn (intercalate "," (nonterms grammar))
    putStrLn (intercalate "," (terms grammar))
    putStrLn (initNonterm grammar)
    mapM_ (\rule -> putStrLn (fst rule ++ "->" ++ concat (snd rule))) (rules grammar)

nontermGenerator :: (Foldable t, Show a, Num a) => a -> [Char] -> t [Char] -> [Char]
nontermGenerator n nonterm nonterms
    | (head nonterm : show n) `elem` nonterms = nontermGenerator (n+1) nonterm nonterms
    | otherwise = head nonterm : show n

getNonterms :: [[Char]] -> [([Char], [[Char]])] -> [[Char]]
getNonterms terms rules = sort $ removeDuplicities $ getNontermList terms rules
    where
        getNontermList terms [] = []
        getNontermList terms (r:rs) = [fst r] ++ filter (`notElem` (terms ++ ["#"])) (snd r) ++ getNontermList terms rs
        removeDuplicities [] = []
        removeDuplicities (x:xs)
            | x `elem` xs = removeDuplicities xs
            | otherwise = x : removeDuplicities xs

transformGrammar :: Grammar -> Grammar
transformGrammar grammar =
    Grammar (getNonterms (terms grammar) transformedRules)
            (terms grammar)
            transformedRules
            (initNonterm grammar)
    where
        transformedRules = transformRules (nonterms grammar) (rules grammar)

        cascadeRule nonterms (l, r) =
            (newNonterms, (l, [head r, newNonterm]) : newRules)
            where
                newNonterm = nontermGenerator 1 l nonterms
                (newNonterms, newRules) = transformRule (nonterms ++ [newNonterm]) (newNonterm, tail r)

        transformTerminalRule nonterms (l, r) =
            (nonterms ++ [newNonterm], [(l, [head r, newNonterm]), (newNonterm, ["#"])])
            where
                newNonterm = nontermGenerator 1 l nonterms

        transformRule nonterms rule@(l, r)
            | length r == 1 && last r == "#" = (nonterms, [rule])
            | length r == 1 = transformTerminalRule nonterms rule -- For not eps rules use this: (nonterms, [rule])
            | length r == 2 && last r `elem` nonterms = (nonterms, [rule])
            | otherwise = cascadeRule nonterms rule
            -- comment: | length r > 2 && last r `elem` nonterms = cascadeRule nonterms rule

        transformRules nonterms [rule] = snd $ transformRule nonterms rule
        transformRules nonterms (rule:rules) =  newRules ++ transformRules newNonterms rules
            where
                (newNonterms, newRules) = transformRule nonterms rule

loadFSM :: Grammar -> FSM
loadFSM grammar =
    FSM ([i | (_, i) <- convTable])
        (terms grammar)
        (getTransitions (rules grammar))
        (getState (initNonterm grammar) convTable)
        (getFinalStates (rules grammar))
    where
        convTable = nontermsToStates (nonterms grammar)

        nontermsToStates = numberStates 1
            where
                numberStates _ [] = []
                numberStates n (x:xs) = (x, n) : numberStates (n+1) xs

        getState nonterm [] = error ("Unknown Nonterm: " ++ nonterm)
        getState nonterm ((n, i):xs)
            | nonterm == n = i
            | otherwise = getState nonterm xs

        getTransition (l, r) = Transition (getState l convTable)
                                          (head r)
                                          (getState (last r) convTable)

        getTransitions [] = []
        getTransitions (rule:rules)
            | length (snd rule) == 1 = getTransitions rules
            | otherwise = getTransition rule : getTransitions rules

        getFinalStates rules = map (\(a, _) -> getState a convTable) (filter (\(_, y) -> last y == "#") rules)

showFSM :: FSM -> IO ()
showFSM fsm = do
    putStrLn (intercalate "," [show state | state <- states fsm])
    putStrLn (intercalate "," (alphabet fsm))
    print (initState fsm)
    putStrLn (intercalate "," [show state | state <- finalStates fsm])
    mapM_ putStrLn $ sort [intercalate "," [show (state t), symbol t, show (nextState t)] | t <- transitions fsm]

main :: IO ()
main = do
    args <- getArgs

    let (option, file) = parseArgs args
    content <- lines <$> loadInput file

    let grammar = loadGrammar content

    case option of
        0 -> showGrammar grammar
        1 -> showGrammar $ transformGrammar grammar
        2 -> showFSM $ loadFSM $ transformGrammar grammar

    -- putStr (show $ transformGrammar grammar)
    -- showFSM fsm
    -- putStr (show $ nontermsToStates )
    -- putStr (show $ loadFSM (Grammar {nonterms = ["A","B"], terms = ["A","A1","A2","B"], rules = [("A",["a","A1"]),("A1",["a","B"]),("A",["c","A2"]),("A2",["c","B"]),("B",["b","B"]),("B",["#"])], initNonterm = "A"}))
