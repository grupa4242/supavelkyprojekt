<?php

set_time_limit(0);

while (true)
{
// disconnected every 5 seconds...
receive_message('192.168.2.111','8051',15);
}

function receive_message($ipServer,$portNumber,$nbSecondsIdle)
{
  // creating the socket...
  $socket = stream_socket_server('tcp://'.$ipServer.':'.$portNumber, $errno, $errstr);
  if (!$socket)
  {
    echo "$errstr ($errno)<br />\n";
  }
  else
   {
    // while there is connection, i'll receive it... if I didn't receive a message within $nbSecondsIdle seconds, the following function will stop.
    while ($conn = @stream_socket_accept($socket,$nbSecondsIdle))
    {
     $message = fread($conn, 1024);
     echo '<br>I have received that : '.$message;
     fputs ($conn, "OK\n");
     //saves data to DB 

        $conn = init_connection();

        $stmt = $conn->prepare("INSERT INTO rec_data(value) VALUES( ? )");
        $stmt->bind_param('s',$message);
        $stmt->execute();

        $values = json_decode($message,true);
        if (json_last_error() == 0){
          $stmt = $conn->prepare("INSERT INTO rec_data(created_on,station_id,temperature,humidity,pressure) VALUES( ? , ? , ? , ? , ? )");
          $station_id = $values["stationid"];

          foreach ($values["vals"] as $key => $value) {
            
            $stmt->bind_param('siiii',$value['timestamp'],$station_id,$value['T'],$value['RH'],$value['P']);
            if ($stmt->execute()) {
              echo "<br>Entry created.";
            }
          }
          
          
        }
        else
        { 
          echo "<br>Error parsing data.";
        }
        
          $stmt->close();

    }

    fclose($socket);
  }
}

function init_connection(){
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
    return $conn;
}
?>