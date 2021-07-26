<?php
define('DB_SERVER', 'localhost');//TODO(doplnit/změnit)
define('DB_USERNAME', 'root');//TODO(doplnit/změnit)
define('DB_PASSWORD', '');//TODO(doplnit/změnit)
define('DB_NAME', 'WAP');//TODO(doplnit/změnit)

// pokus o pripojnei k db
$link = mysqli_connect(DB_SERVER, DB_USERNAME, DB_PASSWORD, DB_NAME);

// kontrola pripojeni
if($link === false){
    die("ERROR: Could not connect. " . mysqli_connect_error());
}

date_default_timezone_set("Europe/Prague");

$homepage_url = "127.0.0.1/"

?>