<?php

require_once "config.php";

$terminy_array = array();
$err_msg = "";
$nazev ="";

if($_SERVER["REQUEST_METHOD"] == "GET") {
    if (isset($_GET['id'])) {
        // pripraveni sql dotazu
        $sql = "select Nazev, Cas, PocetHlasu from Schuzky inner join Terminy using (SchuzkaID) where SchuzkaID = ? order by Cas asc";
        if($stmt = mysqli_prepare($link, $sql)) {

            mysqli_stmt_bind_param($stmt, "i", $param_id);
            $param_id = $_GET['id'];
            if(mysqli_stmt_execute($stmt)) {
                // ulozeni vysledku
                mysqli_stmt_store_result($stmt);
                mysqli_stmt_bind_result($stmt, $nazev, $cas, $pocet_hlasu);
                while(mysqli_stmt_fetch($stmt))
                {
                    $tmp_array = array();
                    $nazev = htmlspecialchars($nazev);
                    $tmp_array["cas"] = $cas;
                    $tmp_array["pocet_hlasu"] = $pocet_hlasu;
                    $terminy_array[] = $tmp_array;
                }
                if(empty($terminy_array))
                {
                    $err_msg = "Na zadané id nebylo nic nalazeno.";
                }
            }
            else
            {
                $err_msg = "Něco se pokazilo, zkuste to znovu později.";
            }

        }
        else
        {
            $err_msg="Něco se pokazilo, zkuste to znovu později.";
        }

    }
}
?>

<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0, shrink-to-fit=no">
        <title>Plánovač schůzek</title>
        <link rel="stylesheet" href="style.css?v=<?php echo time(); ?>">
        <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css" integrity="sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T" crossorigin="anonymous">
        <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.0-beta3/dist/js/bootstrap.bundle.min.js" integrity="sha384-JEW9xMcG8R+pH31jmWH6WWP0WintQrMb4s7ZOdauHnUtxwoG2vI5DkLtS3qm9Ekf" crossorigin="anonymous"></script>
    </head>
    <body class="main-page">
        <nav class="navbar navbar-expand-md fixed-top navbar-dark py-0">
            <a class="navbar-brand" href="<?php echo $homepage_url; ?>">
                <span class="navbar-logo-text">Meet </span><span class="navbar-logo-text-2">Poll</span>
            </a>
        </nav>
        <div class="container">
            <h1 align="center">Výsledky hlasování o události:</h1>
            <?php
            if(!empty($terminy_array))
            {
            echo "<h4 align=\"center\">",$nazev,"</h4>\n";
            echo "<hr class=\"col-2 hr-left\" align=\"center\">\n";
            echo "<br>\n";
            echo "<br>\n";
            echo "<table style=\"width: 30%; margin: auto; font-size: 1.5em\">
            <tr align=\"center\" style=\"border-bottom: 0.2em solid white; border-top: 0.2em solid white;\">
                <th style=\"border-right: 0.2em solid white;\">Termín</th>
                <th>Počet hlasů</th>
            </tr>";
            foreach ($terminy_array as $item) {
            $cas = $item["cas"];
            $pocet_hlasu = $item["pocet_hlasu"];
            echo "
            <tr align=\"center\" style=\"border-bottom: 1px solid grey;\">
                <td style=\"border-right: 0.2em solid white;\">$cas</td>
                <td>$pocet_hlasu</td>
            </tr>";
            }
            echo "</table>";
            }
            echo $err_msg;

            ?>
            <div class="mt-5 mb-5" align="center">
                <a class="bottom-href" href="hlasovani.php<?php echo "?id=".$param_id; ?>">Hlasovat</a>
            </div>

        </div>
    </body>
</html>

