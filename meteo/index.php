<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8">
        <link rel="shortcut icon" type="image/x-icon" href="css/cloud-icon-12870.png" />
		<link href="https://fonts.googleapis.com/css?family=Nunito" rel="stylesheet">
		<link rel="stylesheet" href="css/font-awesome.css">
		<link rel="stylesheet" href="css/styles.css">
		<title>3G meteostanica</title>
	</head>
	<body>
		<div class="uvod">

			<h1>3G <span>METEOSTANICA</span></h1>
		</div>
		<div class="navbar">
			<ul class="menu">
				<li class="selected"><a href="#teplota"><i class="fa fa-thermometer-half fa-4x" aria-hidden="true"></i><h2 class="nazov">Teplota</h2></a></li>
				<li><a href="#tlak"><i class="fa fa-circle-thin fa-4x" aria-hidden="true"></i><h2 class="press">Tlak</h2></a></li>
				<li><a href="#vlhkost"><i class="fa fa-percent fa-4x" aria-hidden="true"></i><h2 class="nazov2">Vlhkosť</h2></a></li>
				<li><a href="#info"><i class="fa fa-info fa-4x" aria-hidden="true"></i>
					<h2 class="nazov2">Info</h2></a></li>

			</ul>

		</div>
		<div class="content"></div>


			<div class="obsah tlak" id="tlak">
				<div id="container2" style="min-width: 80%;max-width: 80%; height: 80%; margin: 0 auto"></div>
			</div>
			<div class="obsah teplota" id="teplota">
				<div id="container1" style="min-width: 80%;max-width: 80%; height: 80%; margin:0 auto"></div>
			</div>
			<div class="obsah vlhkost" id="vlhkost">
				<div id="container3" style="min-width: 80%;max-width: 80%; height: 80%; margin: 0 auto"></div>
			</div>
			<div class="obsah info" id="info">
				<p>Autori:
				<br>
                <br>Lenka Tomíková
                <br>Matúš Pavelek
                <br>Tomáš Hečko
                <br>Michal Balog
				<br><br>
				Github:
                <a href="https://github.com/grupa4242/supavelkyprojekt" target="_blank">https://github.com/grupa4242/supavelkyprojekt</a>
				<br>

				</p>



			</div>


		<script src="js/jquery-3.1.0.min.js"></script>
		<!--<script src="js/script.js"></script>-->
        <?php
       
        $servername = "localhost";
        $username = "root";
        $password = "";
        $dbname = "meteo";

        // Create connection
        $conn = new mysqli($servername,$username, NULL,$dbname);
        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        }
        
        $result = $conn->query("SELECT * FROM rec_data ORDER BY created_on");

        $date = array();
        $temperature = array();
        $humidity = array();
        $pressure = array();

        while($row = $result->fetch_assoc()){
            if (empty($row['value'])) {       
               /*echo "<pre>";
                var_dump($row);
                echo "</pre>";*/
                $date[] = $row['created_on'];
                $temperature[] = $row['temperature'];
                $humidity[] = $row['humidity'];
                $pressure[] = $row['pressure'];
                //echo $row[''] . '<br />';
            }
        }


        ?>
		<script type="text/javascript">
			$(function () {
        Highcharts.chart('container1', {
        title: {
            text: 'Teplota',
            x: -20 //center
        },

        xAxis: {
            categories: [ <?php echo "'".join($date,"','")."'" ?>]
        },
        yAxis: {
            title: {
                text: 'Teplota (C°)'
            },
            plotLines: [{
                value: 0,
                width: 1,
                color: '#808080'
            }]
        },
        tooltip: {
            valueSuffix: '°C'
        },

        series: [{
            name: 'Bratislava',
            data: [<?php echo join($temperature,',') ?>]
        }]
    });
});

$(function () {
    Highcharts.chart('container2', {
        title: {
            text: 'Tlak',
            x: -20 //center
        },

        xAxis: {
            categories: [ <?php echo "'".join($date,"','")."'" ?>]
        },
        yAxis: {
            title: {
                text: 'Tlak (Pa)'
            },
            plotLines: [{
                value: 0,
                width: 1,
                color: '#808080'
            }]
        },
        tooltip: {
            valueSuffix: 'Pa'
        },
        series: [{
            name: 'Bratislava',
            data: [<?php echo join($pressure,',') ?>]
        }
    ]
    });
});

$(function () {
    Highcharts.chart('container3', {
        title: {
            text: 'Vlhkosť',
            x: -20 //center
        },

        xAxis: {
            categories: [ <?php echo "'".join($date,"','")."'" ?>]
        },
        yAxis: {
            title: {
                text: 'Vlhkosť (%)'
            },
            plotLines: [{
                value: 0,
                width: 1,
                color: '#808080'
            }]
        },
        tooltip: {
            valueSuffix: '%'
        },

        series: [{
            name: 'Bratislava',
            data: [<?php echo join($humidity,',') ?>]
        }]
    });
});

/*galeria*/

    var galleries = $('.obsah');
        galleries.hide();

        var selected = $('.menu').find('.selected'),
            selectedGallery;

        function showGallery(selected){
            if ( selected.length ) {
            var id = selected.find('a, pressure').attr('href');
            selectedGallery = $(id);
        }
        var newGallery = selectedGallery.length ? selectedGallery : galleries.eq(0);
        galleries.not(newGallery).hide();
        newGallery.fadeIn();

        }

        showGallery(selected);

        $('.navbar a').on('click',function(event){
            var li = $(this).parent();

                li.addClass('selected')
                .siblings().removeClass('selected');
                showGallery(li);
                event.preventDefault();
});
		</script>
		<script src="https://code.highcharts.com/highcharts.js"></script>
		<script src="https://code.highcharts.com/modules/exporting.js"></script>
	</body>
</html>