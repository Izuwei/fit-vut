<?php
// Autor: Jakub Sadilek
// Login: xsadil07

global $args;    // Argumenty
global $passed;  // Uspesne testy
global $failed;  // Neuspesne testy
global $html;    // HTML stranka

// Trida pro generovani HTML.
class html {
    private $head;   // HTML pred tabulkou
    private $table;  // Tabulka s testy
    private $foot;   // Ukonceni HTML

    // Pridani vysledku testu do html.
    public function add_result($color, $test, $description){
        $this->table .= "            <tr style=\"color:$color\">
                <td>$test</td>
                <td>$description</td> 
            </tr>\n";
    }

    // Vypise cele html.
    public function print_results($passed, $failed){
        echo ("<!DOCTYPE html>
<html>
    <head lang=\"cs\">
        <meta charset=\"UTF-8\">
        <title>Summary</title>
        <style>
            body {
                font-family: Helvetica;
                background-color: #252526;
                color: white;
                font-weight: bold;
            }
            h1, h3 {
                text-align: center;
            }
            table {
                border-collapse: collapse;
                width: auto;
                text-align: left;
                margin: auto;
            }
            td, th {
                padding: 8px;
            }
            th {
                border-bottom: 1px solid silver;
                text-align: center;
            }
        </style>
    </head>
    <body>
        <h1>Test summary</h1>
        <h3 style=\"color:green\">Passed: $passed</h3>
        <h3 style=\"color:red\">Failed: $failed</h3>
        <table>
            <tr>
                <th>Test</th>
                <th>Result</th>
            </tr>
            $this->table
        </table>
    </body>
</html>\n");
    }
}

// Funkce vypise chybovou hlasku a ukonci program.
function error($msg, $code){
    fwrite(STDERR, "$msg\n");
    exit($code);
}

// Vypis napovedy.
function print_help(){
    echo "Autor: Jakub Sadilek, xsadil07
    Skript slouzi k automatickemu testovani skriptu parse.php a interpret.py.\n";
    exit(0);
}

// Funkce Zkontroluje jestli jsou argumenty validni.
function check_args($argc, $argv){
    global $args;
    if ($argc === 2 && $argv[1] === "--help")
        print_help();   // Vypiseme napovedu

    $args = getopt(null, ["directory:", "recursive", "parse-script:", "int-script:", "parse-only", "int-only"]);

    if (count($args) + 1 !== $argc)  // Neznamy argument
        error("Neznamy argument pri volani skriptu.", 10);

    if (array_key_exists("parse-script", $args) && array_key_exists("int-only", $args) ||
        array_key_exists("int-script", $args) && array_key_exists("parse-only", $args) || 
        array_key_exists("int-only", $args) && array_key_exists("parse-only", $args))
        error("Neplatna kombinace parametru pri volani.", 10);   // Nepovolene kombinace argumentu
}

// Funkce vraci cestu ke skriptu, pokud neexistuje ukonci program.
function script_file($type){
    global $args;
    if ($type === "interpret"){  // Interpret
        if (array_key_exists("int-script", $args))
            $script = $args["int-script"];  // Cesta v argumentu
        else
            $script = "./interpret.py";  // Implicitni cesta
    }
    elseif ($type === "parse"){  // Parse
        if (array_key_exists("parse-script", $args))
            $script = $args["parse-script"];  // Cesta v argumentu
        else
            $script = "./parse.php";  // Implicitni cesta
    }

    if (!(is_file($script) && is_readable($script)))   // kontrola existence
        error("Soubor $script neexistuje nebo nedostatecne opravneni.", 11);
    return realpath($script);  // Vracime cestu
}

// Funkce rekurzivne hleda soubory s priponou $format v adresari $path.
// Vraci list s cestami k souborum. Pro vyhledani byl pouzit algoritmus BFS.
function find_files_recursive($path, $format){
    $directories[] = $path;  // List nalezenych adresaru
    $files = [];    // List hledanych souboru

    while(!empty($directories)){    // Hledame dokud mame adresare
        $path = array_shift($directories);
        $directories = array_merge($directories, glob($path . DIRECTORY_SEPARATOR . '*', GLOB_ONLYDIR));
        $files = array_merge($files, glob($path . DIRECTORY_SEPARATOR . '*.' . $format));
    }
    return $files;
}

// Funkce hleda soubory s formatem $format a vrati je.
function find_files($format){
    global $args;
    if (array_key_exists("directory", $args))
        $path = realpath($args["directory"]);  // Zadana cesta
    else
        $path = getcwd();   // Implicitni cesta
    
    if (!is_dir($path))     // Kontrola existence
        error("Neplatna cesta $path", 11);
    
    if (array_key_exists("recursive", $args))
        return find_files_recursive($path, $format);    // Rekurzivni hledani
    else
        return glob($path . DIRECTORY_SEPARATOR . '*.' . $format);  // Nerekurzivni hledani
}

// Funkce vytvori soubor s abs. cestou $file a vlozi do nej obsah $content
// Pokud existuje, vrati existujici bez upravy.
function create_file($file, $content){
    if (!is_file($file)){
        if (file_put_contents($file, "") === false)
            error("Vytvoreni souboru $file selhalo.", 12);
    }
    if (!is_readable($file))
        error("Ze souboru $file nezle cist.", 11);

    return $file;
}

// Funkce testuje pouze parse.
function test_parse(){
    global $passed;
    global $failed;
    global $html;

    $parse = script_file("parse");   // Najdeme skript
    $src_files = find_files("src");  // Najdeme testy
    
    foreach ($src_files as $src){
        if (!is_readable($src))  // Dogenerujeme potrebne soubory pokud treba + prava
            error("Ze souboru $src nelze cist.", 11);
        if (!is_writable($tmp = create_file(str_replace(".src", ".tmp", $src), "")))
            error("Nelze zapisovat do docasneho souboru $tmp.", 12);
        $out = create_file(str_replace(".src", ".out", $src), "");
        $rc = create_file(str_replace(".src", ".rc", $src), "0");

        exec("php7.3 $parse < $src > $tmp 2>/dev/null", $com, $ret);
        $ret_ok = intval(trim(file_get_contents($rc)));  // Ulozime ref. navratovy kod
        if ($ret === 0 && $ret === $ret_ok){    // Test prosel
            exec("java -jar /pub/courses/ipp/jexamxml/jexamxml.jar $out $tmp &>/dev/null", $com, $ret);
            if ($ret !== 0){    // Neprosel, spatny vystup skriptu
                $failed++;
                $html->add_result("red", $src, "Unexpected XML format");
            }
            else {  // Prosel
                $passed++;
                $html->add_result("green", $src, "OK");
            }
        }
        elseif ($ret === $ret_ok) {  // Chybny test prosel
            $passed++;
            $html->add_result("green", $src, "OK");
        }
        else {  // Test neprosel
            $failed++;
            $html->add_result("red", $src, "Unexpected return value $ret, expected $ret_ok");
        }
        exec("rm -f $tmp $out.log");
    }
}

// Funkce zkontroluje vysledek testu a zaznamena je. Soubor tmp_file je nakonci smazan.
// Funkce je pouzita pouze u "int-only" a "both".
function check_result($rc_file, $ret, $src_file, $out_file, $tmp_file){
    global $passed;
    global $failed;
    global $html;

    $ret_ok = intval(trim(file_get_contents($rc_file)));  // Ref. navratova hodnota
    if ($ret === 0 && $ret === $ret_ok){    // Skript prosel
        exec("diff $out_file $tmp_file 2>/dev/null", $diff);
        if (!empty($diff)){  // Vysledek se neshoduje
            $failed++;
            $html->add_result("red", $src_file, "The output doesn't match reference.");
        }
        else {  // Test prosel
            $passed++;
            $html->add_result("green", $src_file, "OK");
        }
    }
    elseif ($ret === $ret_ok) {  // Chybny test prosel
        $passed++;
        $html->add_result("green", $src_file, "OK");
    }
    else {  // Test neprosel
        $failed++;
        $html->add_result("red", $src_file, "Unexpected return value $ret, expected $ret_ok");
    }
    exec("rm -f $tmp_file");
}

// Funkce testuje pouze interpret.
function test_int(){
    $interpret = script_file("interpret");  // Najdeme skript
    $src_files = find_files("src");         // Najdeme testy
    
    foreach ($src_files as $src){
        if (!is_readable($src))
            error("Ze souboru $src nelze cist.", 11);
        if (!is_writable($tmp = create_file(str_replace(".src", ".tmp", $src), "")))
            error("Nelze zapisovat do docasneho souboru $tmp.", 12);
        $out = create_file(str_replace(".src", ".out", $src), "");
        $in = create_file(str_replace(".src", ".in", $src), "");
        $rc = create_file(str_replace(".src", ".rc", $src), "0");

        exec("python3.6 $interpret --source=$src < $in > $tmp 2>/dev/null", $com, $ret);
        check_result($rc, $ret, $src, $out, $tmp);
    }
}

// Funkce testuje parse i interpret
function test_both(){
    $interpret = script_file("interpret");  // Najdeme skripty
    $parse = script_file("parse");
    $src_files = find_files("src");         // Najdeme testy

    foreach ($src_files as $src){
        if (!is_readable($src))  // Kontrola souboru testu
            error("Ze souboru $src nelze cist.", 11);
        if (!is_writable($tmp = create_file(str_replace(".src", ".tmp", $src), "")))
            error("Nelze zapisovat do docasneho souboru $tmp.", 12);
        $out = create_file(str_replace(".src", ".out", $src), "");
        $in = create_file(str_replace(".src", ".in", $src), "");
        $rc = create_file(str_replace(".src", ".rc", $src), "0");

        exec("php7.3 $parse < $src 2>/dev/null | python3.6 $interpret --input=$in > $tmp 2>/dev/null", $com, $ret);
        check_result($rc, $ret, $src, $out, $tmp);  // Kontrola vysledku
    }
}

$passed = 0;
$failed = 0;
$html = new html;

check_args($argc, $argv);

if (array_key_exists("parse-only", $args))
    test_parse();   // Kontrola parseru
elseif (array_key_exists("int-only", $args))
    test_int();     // Kontrola interpretu
else
    test_both();    // Kontrola obojiho
$html->print_results($passed, $failed);
?>
