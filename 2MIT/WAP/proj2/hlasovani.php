<?php

require_once "config.php";

$terminy_array = array();
$err_msg = "";
$nazev ="";

//zobrazeni moznosti z DB, info predano v URL (GET)
if($_SERVER["REQUEST_METHOD"] == "GET") {
    if (isset($_GET['id'])) {
        // pripraveni sql dotazu
        $sql = "select TerminID, Nazev, Cas from Schuzky inner join Terminy using (SchuzkaID) where SchuzkaID = ? order by Cas asc";
        if($stmt = mysqli_prepare($link, $sql)) {

            mysqli_stmt_bind_param($stmt, "i", $param_id);
            $param_id = $_GET['id'];
            if(mysqli_stmt_execute($stmt)) {
                // ulozeni vysledku
                mysqli_stmt_store_result($stmt);
                mysqli_stmt_bind_result($stmt,$termin_id, $nazev, $cas);
                while(mysqli_stmt_fetch($stmt))
                {
                    $tmp_array = array();
                    $nazev = htmlspecialchars($nazev);
                    $tmp_array["id"] = $termin_id;
                    $tmp_array["cas"] = $cas;
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

//hlasovani pro moznosti, info je POST
if($_SERVER["REQUEST_METHOD"] == "POST") {
    if (isset($_POST['hlasovat']) && isset($_POST["SchuzkaID"])) {
        $schuzka_id = $_POST["SchuzkaID"];
        if ( isset($_POST["hlasovat_checkbox"]) )
        {
            foreach ($_POST["hlasovat_checkbox"] as $hlas_id)
            {
                $sql = "update Terminy set PocetHlasu = PocetHlasu+1 WHERE SchuzkaID = ? and TerminID = ?";
                if($stmt = mysqli_prepare($link, $sql))
                {
                    mysqli_stmt_bind_param($stmt, "ii",$param_schuzka_id,$param_termin_id);
                    $param_schuzka_id = $schuzka_id;
                    $param_termin_id = $hlas_id;

                    if (!mysqli_stmt_execute($stmt))
                    {
                        echo "Error";
                    }
                }
            }
            //cookie se nastavi pouze pokud byl vybran nejaky termin pro hlasovani
            setcookie($schuzka_id, "voted", time() + (5 * 365 * 24 * 3600));//nastaveni cookie, ze pro dane id bylo hlasovano (s expiraci za 5 let)
        }
        header('Location: vysledky.php?id='.$schuzka_id);

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
        <script type="text/javascript" src="https://code.jquery.com/jquery-3.5.1.js"></script>
    </head>
    <body class="main-page">
        <nav class="navbar navbar-expand-md fixed-top navbar-dark py-0">
            <a class="navbar-brand" href="<?php echo $homepage_url; ?>">
                <span class="navbar-logo-text">Meet </span><span class="navbar-logo-text-2">Poll</span>
            </a>
        </nav>
        <div class="container">
            <h1 align="center">Hlasování o události:</h1>
            <form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" onsubmit="return TestIfVoted()" method="post" align="center">
                <input type="hidden" name="SchuzkaID" id="SchuzkaID" value="<?php echo $param_id; ?>">

                <?php
                if(!empty($terminy_array))
                {
                    echo "<h4>",$nazev,"</h4>\n";
                    echo "<hr class=\"col-2 hr-left\" align=\"center\">\n";
                    echo "<table style=\"width: 100%; font-size: 1.5em\">";
                    foreach ($terminy_array as $item) {
                        $cas = $item["cas"];
                        $id = $item["id"];
                        echo "
                <tr>
                    <td><label>$cas<input class=\"ml-4 mt-4 voting-checkbox\" type=\"checkbox\" name=\"hlasovat_checkbox[]\" value=\"$id\"></label></td >
                </tr>";
                    }
                    echo "</table>";
                }
                echo $err_msg;

                ?>
                <tr>
                    <td colspan="3"><input class="btn btn-warning main-btn vote-btn mt-5 mb-5" type="submit" name="hlasovat" value="Hlasovat"></td>
                </tr>
            </form>
            <div class="mb-5" align="center">
                <a class="bottom-href" href="vysledky.php<?php echo "?id=".$param_id; ?>">Výsledky</a>
            </div>
        </div>
</body>
</html>

<script>
    function TestIfVoted()
    {
        let c = document.cookie.split(";");
        let id = $("#SchuzkaID")[0].value;
        for (var i = 0; i < c.length; i++)
        {
            let cookie = c[i].split("=");
            if(cookie[0].trim()===id && cookie[1]==="voted")
            {//pokud je zde cookie se jmene id a hodnotou voted, jiz bylo hlasovano pro tento termin
                alert("Pro tento termin jiz bylo hlasovano");
                return false
            }
        }
        return true;
    }
</script>

