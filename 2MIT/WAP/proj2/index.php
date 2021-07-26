<?php

require_once "config.php";

$datum_predvyplnit = date("Y-m-d");
$cas_predvyplnit = date("H:i");

//zalozeni hlasovani
if($_SERVER["REQUEST_METHOD"] == "POST") {
    $schuzka_id = 0;
    if (isset($_POST['create']) && isset($_POST["termin_date"]) && isset($_POST["termin_time"]) && isset($_POST["nazev"])) {
        $termin_dates = $_POST["termin_date"];
        $termin_times = $_POST["termin_time"];
        $delete_date = date("Y-m-d");
        for ($i = 0 ; $i < count($termin_dates) ; $i++)
        {//datum smazani je posledni mozny hlasovaci termin + 14dni
            if($termin_dates[$i] > $delete_date)
            {
                $delete_date = $termin_dates[$i];
            }
        }
        $delete_date = date('Y-m-d', strtotime($delete_date. ' + 14 days'));

        $sql = "insert into Schuzky (Nazev, DatumSmazani) values (?,?)";
        if($stmt = mysqli_prepare($link, $sql))
        {
            mysqli_stmt_bind_param($stmt, "ss",$param_nazev,$param_datum);
            $param_nazev = $_POST["nazev"];
            $param_datum = $delete_date;

            if (!mysqli_stmt_execute($stmt))
            {
                echo "Error";
            }
            else
            {
                $schuzka_id = mysqli_insert_id($link);//id prave vlozeneho prvku
                for ($i = 0 ; $i < count($termin_dates) ; $i++)
                {
                    $new_date_time = $param_datum = date("Y-m-d H:i:s", strtotime($termin_dates[$i]." ".$termin_times[$i]));
                    $sql = "insert into Terminy (Cas, SchuzkaID,PocetHlasu) values (?,?,0)";
                    if($stmt = mysqli_prepare($link, $sql))
                    {
                        mysqli_stmt_bind_param($stmt, "ss", $param_cas, $param_id);
                        $param_cas = $new_date_time;
                        $param_id = $schuzka_id;

                        if (!mysqli_stmt_execute($stmt))
                        {
                            echo "Error";
                        }
                    }
                }

                header('Location: hlasovani.php?id='.$schuzka_id);
            }
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
        <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.0-beta3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-eOJMYsd53ii+scO/bJGFsiCZc+5NDVN2yr8+0RDqr0Ql0h+rP48ckxlpbzKgwra6" crossorigin="anonymous">
        <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.0.0-beta3/dist/js/bootstrap.bundle.min.js" integrity="sha384-JEW9xMcG8R+pH31jmWH6WWP0WintQrMb4s7ZOdauHnUtxwoG2vI5DkLtS3qm9Ekf" crossorigin="anonymous"></script>

        <script type="text/javascript" src="https://code.jquery.com/jquery-3.5.1.js"></script>
    </head>

    <body class="main-page">
        <nav class="navbar navbar-expand-md fixed-top navbar-dark py-0">
        	<a class="navbar-brand" href="#">
                <span class="navbar-logo-text">Meet </span><span class="navbar-logo-text-2">Poll</span>
            </a>
        </nav>

        <div class="container">
            <div class="row">
                <div class="col-md">

                </div>
                <div class="col-6">
                    <div class="create-event">
                        <div class="row">
                            <div class="col">
                                <h1>Zadejte čas události</h1>
                                <hr class="col-2 hr-left" align="left">
                            </div>
                        </div>
                        <div class="row mt-3 ">
                            <div class="col ml-xl-5">
                                <h5>Datum</h5>
                                <input class="date-width" type="date" name="new_termin_date" id="new_termin_date" value="<?php echo $datum_predvyplnit; ?>">
                            </div>
                            <div class="col">
                                <h5>Čas</h5>
                                <input class="time-width" type="time" name="new_termin_time" id="new_termin_time"  value="<?php echo $cas_predvyplnit; ?>" >
                            </div>
                            <div class="col">

                            </div>
                        </div>
                        <div class="row">
                            <div class="col">

                            </div>
                            <div class="col-11">
                                <button class="btn btn-warning main-btn mt-5" type="button" name="add" id="add">Přidat termín</button>
                            </div>
                        </div>
                    </div>

                    <div class="generate-event">
                        <div class="row mt-5">
                            <div class="col">
                                <h1>Generovat čas události</h1>
                                <hr class="col-2 hr-left" align="left">
                            </div>
                        </div>

                        <div class="row mt-3">
                            <div class="col ml-xl-5">
                                <h5><label for="gen_termin_date_from" >Datum od</label></h5>
                                <input class="date-width" type="date" name="gen_termin_date_from" id="gen_termin_date_from" value="<?php echo $datum_predvyplnit; ?>" >
                            </div>

                            <div class="col">
                                <h5><label for="gen_termin_date_to" >Datum do</label></h5>
                                <input class="date-width" type="date" name="gen_termin_date_to" id="gen_termin_date_to" value="<?php echo $datum_predvyplnit; ?>" >
                            </div>
                            <div class="col">

                            </div>
                        </div>

                        <div class="row mt-5">
                            <div class="col ml-xl-5">
                                <h5><label for="gen_termin_time_from" >Čas od</label></h5>
                                <input class="time-width" type="time" name="gen_termin_time_from" id="gen_termin_time_from" value="<?php echo $cas_predvyplnit; ?>" >
                            </div>

                            <div class="col">
                                <h5><label for="gen_termin_time_to" >Čas do</label></h5>
                                <input class="time-width" type="time" name="gen_termin_time_to" id="gen_termin_time_to" value="<?php echo $cas_predvyplnit; ?>" >
                            </div>

                            <div class="col">
                                <h5><label for="gen_termin_time_spacing" >Rozestup</label></h5>
                                <input class="time-width" type="time" name="gen_termin_time_spacing" id="gen_termin_time_spacing" value="00:30">
                            </div>
                            <div class="col">

                            </div>
                        </div>

                        <div class="row mt-5">

                            <div class="col ml-xl-5">
                                <input class="voting-checkbox-generate btn-check" type="checkbox" name="day_picker" id="day_picker_mo" checked>
                                <label class="btn btn-outline-success" for="day_picker_mo">Pondělí</label>
                            </div>
                            <div class="col">
                                <input class="voting-checkbox-generate btn-check" type="checkbox" name="day_picker" id="day_picker_tu" checked>
                                <label class="btn btn-outline-success" for="day_picker_tu">Úterý</label>
                            </div>
                            <div class="col">
                                <input class="voting-checkbox-generate btn-check" type="checkbox" name="day_picker" id="day_picker_we" checked>
                                <label class="btn btn-outline-success" for="day_picker_we">Středa</label>
                            </div>
                            <div class="col">
                                <input class="voting-checkbox-generate btn-check" type="checkbox" name="day_picker" id="day_picker_th" checked>
                                <label class="btn btn-outline-success" for="day_picker_th">Čtvrtek</label>
                            </div>
                            <div class="col">
                                <input class="voting-checkbox-generate btn-check" type="checkbox" name="day_picker" id="day_picker_fr" checked>
                                <label class="btn btn-outline-success" for="day_picker_fr">Pátek</label>
                            </div>
                            <div class="col">
                                <input class="voting-checkbox-generate btn-check" type="checkbox" name="day_picker" id="day_picker_sa">
                                <label class="btn btn-outline-success" for="day_picker_sa">Sobota</label>
                            </div>
                            <div class="col">
                                <input class="voting-checkbox-generate btn-check" type="checkbox" name="day_picker" id="day_picker_su">
                                <label class="btn btn-outline-success" for="day_picker_su">Neděle</label>
                            </div>

                        </div>

                        <div class="row">
                            <div class="col">

                            </div>
                            <div class="col-11">
                                <button class="btn btn-warning main-btn mt-5" type="button" name="generate" id="generate">Generovat termíny</button>
                            </div>

                        </div>
                    </div>
                </div>

                <div class="col-4 mb-5" style="border-left: 0.3em solid #fc0">
                    <form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>"  onsubmit="return validateForm()"  method="post">
                        <div class="row">
                            <div class="col">
                                <h1><label for="nazev">Vytvářená událost</label></h1>
                                <hr class="col-4 hr-left" align="left">
                            </div>
                        </div>
                        <div class="row">
                            <div class="col ml-xl-5">
                                <h5><label for="nazev">Název Události</label></h5>
                                <input type="text" name="nazev" id="nazev" required>
                            </div>
                        </div>
                        <div class="row mt-5 ml-xl-5">
                            <div id="terminy">

                            </div>
                        </div>
                        <div class="row">
                            <div class="col-3">

                            </div>
                            <div class"col-9">
                                <input class="btn btn-warning main-btn vote-btn mt-5 mb-5 ml-xl-5" type="submit" name="create" value="Založit Hlasování">
                            </div>
                        </div>
                    </form>
                </div>
                <div class="col-md">

                </div>
            </div>
        </div>
    </body>
</html>

<script>

    function validateForm()
    {
        const exist_date = $(".input_termin_date");
        const exist_time = $(".input_termin_time");
        if(exist_date.length <1)
        {
            alert("Nebyl přidán žádný termín");
            return false;
        }
        for (var i = 0; i < exist_date.length; i++)
        {
            let d = exist_date[i].value;
            let t = exist_time[i].value;
            let cnt = already_exists(d,t);
            if(cnt>1)
            {//pokud se zde termin vyskytuje vic jak 1 (on sam) tak se jedna o duplicitu
                alert("Duplikace terminu: "+d+" "+t);
                return false
            }
        }
        return true;
    }
    function already_exists(date,time)
    {//funkce kontrolujici zda uz v seznamu je dany termni (pripadne vrati pocet shodnych terminu)
        var cnt=0;
        const exist_date = $(".input_termin_date");
        const exist_time = $(".input_termin_time");
        for (var i = 0; i < exist_date.length; i++)
        {
            if(date === exist_date[i].value && time === exist_time[i].value)
                cnt++;
        }
        return cnt;
    }
    jQuery(document).ready(function ($) {

        $("#add").click(function (e) {
            //funkce pro pridani 1 terminu (kontroluje i zda uz neni na seznamu, va takovem pripadu se termin neprida)
            const src_date = $("#new_termin_date")[0].value;
            const src_time = $("#new_termin_time")[0].value;
            if(src_date && src_time)
            {
                if(already_exists(src_date,src_time))
                    return;
                $("#terminy").append(
                    '<div class="mt-1">' +
                    '<input type="date" class="input_termin_date date-width date-height" name="termin_date[]" id="termin_date" value="'+src_date+'">' +
                    '<input type="time" class="input_termin_time time-width time-height" name="termin_time[]" id="termin_time" value="'+src_time+'">' +
                    '<button  class="delete btn btn-danger">Odebrat Termín</button>' +
                    '<br>'+
                    '<br>'+
                    '</div>'
                );
            }

        });

        $("#generate").click(function (e) {
            //funkce pro generovani terminu
            const src_date_from = new Date($("#gen_termin_date_from")[0].value);
            const src_date_to = new Date($("#gen_termin_date_to")[0].value);
            const src_time_from = $("#gen_termin_time_from")[0].value.split(":");
            const src_time_to = $("#gen_termin_time_to")[0].value.split(":");
            const src_time_spacing = $("#gen_termin_time_spacing")[0].value.split(":");
            if(src_date_from && src_time_to && src_time_from && src_time_to && src_time_spacing)
            {
                if(src_date_from>src_time_to)
                    return;//datum od je vetsi nez do

                var array_days = [];
                for(var datum=src_date_from; datum<=src_date_to; datum.setDate(datum.getDate()+1))
                {//prvne se vygeneruji dny pro ktere budou terminy
                    switch (datum.getDay()) {
                        case 0: if (! $("#day_picker_su")[0].checked) continue; break;
                        case 1: if (! $("#day_picker_mo")[0].checked) continue; break;
                        case 2: if (! $("#day_picker_tu")[0].checked) continue; break;
                        case 3: if (! $("#day_picker_we")[0].checked) continue; break;
                        case 4: if (! $("#day_picker_th")[0].checked) continue; break;
                        case 5: if (! $("#day_picker_fr")[0].checked) continue; break;
                        case 6: if (! $("#day_picker_sa")[0].checked) continue; break;
                    }
                    array_days.push(new Date(datum));
                }


                var array_times = [];
                var cas_from = parseInt(src_time_from[0], 10)*60+parseInt(src_time_from[1], 10);
                var cas_to = parseInt(src_time_to[0], 10)*60+parseInt(src_time_to[1], 10);
                var rozestup = parseInt(src_time_spacing[0], 10)*60+parseInt(src_time_spacing[1], 10);

                if(cas_from>cas_to || rozestup ==0)
                    return;//cas od je vetsi nez do

                for(var cas = cas_from; cas<=cas_to ; cas+=rozestup )
                {//nasledne se vygeneruji casy pro ktere budou terminy
                    array_times.push([Math.floor(cas/60),cas%60]);
                }
                for(var j = 0; j < array_days.length;j++)
                {
                    var d = array_days[j].toISOString().split('T')[0];
                    for(var k = 0; k < array_times.length;k++)
                    {//pridani terminu pro kazdy zvoleny den a kazdy zvoleny cas
                        var h= array_times[k][0];
                        h = h < 10 ? '0'+h : h;
                        var m= array_times[k][1];
                        m = m < 10 ? '0'+m : m;
                        if(already_exists(d,h+':'+m))
                            continue;//(kontroluje i zda uz neni na seznamu, va takovem pripadu se termin neprida)
                        $("#terminy").append(
                            '<div class="mt-1">' +
                            '<input type="date" class="input_termin_date date-width date-height" name="termin_date[]" id="termin_date" value="'+d+'">' +
                            '<input type="time" class="input_termin_time time-width time-height" name="termin_time[]" id="termin_time" value="'+ h+':'+m+'">' +
                            '<button  class="delete btn btn-danger">Odebrat Termín</button>' +
                            '<br>'+
                            '<br>'+
                            '</div>'
                        );
                    }
                }

            }

        });

        $("body").on("click", ".delete", function (e) {
            $(this).parent("div").remove();
        });//funkce pro ostranovani terminu


    });
</script>