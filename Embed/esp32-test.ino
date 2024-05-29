#include <ESPmDNS.h>
#include <WiFi.h>
#include <WebServer.h> //192.168.1.105
#include <ElegantOTA.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

//WiFi want to connect
#define WIFI_SSID  "PondKuB_WIFI"
#define WIFI_PASS  "0831764572"

// #define WIFI_SSID  "CoEIoT"
// #define WIFI_PASS  "iot.coe.psu.ac.th"

// #define WIFI_SSID  "Mook"
// #define WIFI_PASS  "mookkykiki"

WebServer webServer(80);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
bool mqttEmailmessage_Recieved = false;
bool mqttTimerStart = false;
bool ClawMachineStatus = true;

#define APP_MQTT_SERVER  "192.168.1.104"
#define APP_MQTT_PORT    1883

const char *index_html PROGMEM = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.7.1/jquery.min.js"></script>
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.6.2/dist/css/bootstrap.min.css">
    <script src="https://cdn.jsdelivr.net/npm/popper.js@1.16.1/dist/umd/popper.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@4.6.2/dist/js/bootstrap.bundle.min.js"></script>

    <!-- Font -->
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Aladin&family=Allerta&family=Fira+Sans:ital,wght@0,100;0,200;0,300;0,400;0,500;0,600;0,700;0,800;0,900;1,100;1,200;1,300;1,400;1,500;1,600;1,700;1,800;1,900&family=Slackside+One&display=swap" rel="stylesheet">
    <link href="https://fonts.googleapis.com/css2?family=Maven+Pro:wght@400..900&family=Pacifico&display=swap" rel="stylesheet">
    <link href="https://fonts.googleapis.com/css2?family=Anuphan:wght@100..700&family=Chakra+Petch:ital,wght@0,300;0,400;0,500;0,600;0,700;1,300;1,400;1,500;1,600;1,700&display=swap" rel="stylesheet">

    <title>Claw Machine</title>
    <style>
      
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        .bg {
            background-color: rgb(241, 240, 235);
            height: 100%; 
        }

        .logo-text h1 {
            color: #333;
            font-family: "Allerta", sans-serif;
            font-weight: 500;
            font-style: normal;
            font-size: 25px;
            margin-top: 25px;
            margin-left: 15px;
        }

        .logo img{
            height: 60px;
            margin-left: 15px;
            margin-top: 15px;
            
        }
        
        nav {
            display: flex;
            justify-content: space-between;
            /* border: 1px solid red; */
            bottom: 0;
            /* padding: 10px; */
            height: 60px;
            background-color: rgb(241, 240, 235);
            display: flex;
            justify-content: left;
            align-items: center;
        }

        .marquee-text {
          color: black;
          font-family: "Anuphan", sans-serif;
          font-weight: 500;
          font-style: normal;
          font-size: 18px;
          display: none;
          justify-content: space-between;
        }

        .banner {
            /* border: 1px solid red; */
            color: black;
            height: 550px;
            display: grid;
            grid-template-columns: repeat(2,1fr);
            /* display: flex;
            justify-content: center;
            align-items: center; */
        }

        .banner-img {
            width: 476px;
        }

        .banner-img img{
            margin: 30px;
            padding: 50px;
            width: 500px;
            /* height: 600px; */
        }

        .banner-info {
            text-align: center;
            justify-content: center;
            align-items: center;
            padding: 120px;
            margin: 30px;
        }

        .banner-info h1 {
            font-family: "Fira Sans", sans-serif;
            font-weight: 600;
            font-style: italic;
            font-size: 50px;
        }

        .banner-info h2 {
            font-family: "Slackside One", cursive;
            font-weight: 400;
            font-style: normal;
            font-size: 35px;
            color: #333;
        }


        .footer-main {
            display: grid;
            grid-template-columns: repeat(2,1fr);
            background-color: #333;
            color: white;
            text-align: center;
            padding: 20px;
            font-family: "Maven Pro", sans-serif;
            font-optical-sizing: auto;
            font-weight: 700;
            font-style: normal
        }

        .footer-contact {
            text-align: center;
        }

        @media (max-width: 500px) {
            .footer-about, .footer-services, .footer-contact, .footer-follow {
                width: 100%;
                margin-bottom: 20px;
            }
        }

        .button-81 {
            background-color: #fff;
            border: 0 solid #e2e8f0;
            border-radius: 1.5rem;
            box-sizing: border-box;
            color: #F28585;
            cursor: pointer;
            display: inline-block;
            font-family: "Basier circle",-apple-system,system-ui,"Segoe UI",Roboto,"Helvetica Neue",Arial,"Noto Sans",sans-serif,"Apple Color Emoji","Segoe UI Emoji","Segoe UI Symbol","Noto Color Emoji";
            font-size: 1.1rem;
            font-weight: 600;
            line-height: 1;
            padding: 1rem 1.6rem;
            text-align: center;
            text-decoration: none #0d172a solid;
            text-decoration-thickness: auto;
            transition: all .1s cubic-bezier(.4, 0, .2, 1);
            box-shadow: 0px 1px 2px rgba(166, 175, 195, 0.25);
            user-select: none;
            -webkit-user-select: none;
            touch-action: manipulation;
            }

        .button-81:hover {
            background-color: #F28585;
            color: #fff;
        }

        @media (min-width: 768px) {
            .button-81 {
                font-size: 1.125rem;
                padding: 1rem 2rem;
            }
        }

        .button-81 a {
            text-decoration: none;
        }
    
        .swal2-title {
            font-family: "Maven Pro", sans-serif;
            font-optical-sizing: auto;
            font-weight: 700;
            font-style: normal;
            font-size: 25px;
        }

        .swal2.text {
            font-family: "Maven Pro", sans-serif;
            font-optical-sizing: auto;
            font-weight: 700;
            font-style: normal;
            font-size: 20px;
        }


 
    </style>
</head>
<body>
<script src="https://code.jquery.com/jquery-3.6.0.min.js"></script>
<script src="https://cdn.jsdelivr.net/npm/sweetalert2@11"></script>
<script>
    // Function to fetch status and show/hide marquee accordingly
    function fetchStatus() {
        $.get("/getStatus", function(data) {
            // Update status in UI
            $("#status").text(data.status);

            document.getElementById("button-81").addEventListener("click", handleButtonClick)

            function handleButtonClick() {
                if (data.status === false) {
                    showToast();
                    console.log("show");
                } else {
                    window.location.href = "/payment";
                }
            }
    
        });
    }

    function showToast() {
        Swal.fire({
        title: "Error",
        text: "Claw Machine by Sleepless is temporarily unavaliable.",
        icon: "error",
        showConfirmButton: false,
        timer: 3000
        });
    }

    // Call the function to fetch status initially
    fetchStatus();

    // Set timeout to fetch status again after a delay (e.g., 1 second)
    setInterval(fetchStatus, 1000);
</script>

    <div class="bg">
        <!-- Nav Section -->
        <nav>
            <div class="logo">
                <a href="https://pic.in.th/image/2SkB3M"><img src="https://img2.pic.in.th/pic/sleepless3-lotus.th.png" alt="sleepless3-lotus.png"/></a>
            </div>
            <div class="logo-text">
                <h1>Claw Machine Website</h1>
            </div>
        </nav>
        <!-- Nav Section -->

        <!-- Banner Section -->
        <section class="banner">
            <div class="banner-img">
                <img src="https://cdn-icons-png.flaticon.com/512/879/879998.png">
            </div>
            <div class="banner-info">
                <a href="https://pic.in.th/image/27GTb6"><img src="https://img5.pic.in.th/file/secure-sv1/sleepless3.md.png" alt="sleepless3.png"/></a>
                <!-- <h1>Sleepless</h1> -->
                <h2>No time to stop!</h2>
                <br>
                <button class="button-81" id="button-81" type="button">Play</button>
            </div>
        </section>
        <!-- Banner Section -->

        <!-- Footer Section -->
            <footer>
                <div class="footer-main">
                    <div class="footer-about">
                        <h3 style="color: #e6a4a4;">ABOUT US</h3>
                        <br>
                        <p style="font-weight: 400;">This is the mini project of 240-371 IOT SYSTEM DEVELOPER MODULE</p>
                    </div>
                    <div class="footer-contact">
                        <h3 style="color: #e6a4a4;">SLEEPLESS MEMBER</h3>
                        <br>
                        <p style="font-weight: 400;">Mr. Sathaporn Punboong Student ID:6410110516</p>
                        <p style="font-weight: 400;">Mr. Chatchai Jankaew   Student ID:6410110644</p>
                        <p style="font-weight: 400;">Miss. Yanisa Thaenthong Student ID:6410110649</p>
                    </div>
                </div>
            </footer>
        <!-- Footer Section -->
    </div>
</body>
</html>
)";

const char *payment_html PROGMEM = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.7.1/jquery.min.js"></script>
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.6.2/dist/css/bootstrap.min.css">
    <script src="https://cdn.jsdelivr.net/npm/popper.js@1.16.1/dist/umd/popper.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@4.6.2/dist/js/bootstrap.bundle.min.js"></script>

    <!-- Font -->
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Aladin&family=Allerta&family=Fira+Sans:ital,wght@0,100;0,200;0,300;0,400;0,500;0,600;0,700;0,800;0,900;1,100;1,200;1,300;1,400;1,500;1,600;1,700;1,800;1,900&family=Slackside+One&display=swap" rel="stylesheet">
    <link href="https://fonts.googleapis.com/css2?family=Maven+Pro:wght@400..900&family=Pacifico&display=swap" rel="stylesheet">
    <title>Payment Page</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        .bg {
            background-color: rgb(241, 240, 235);
            height: 100%; 
        }

        .logo-text h1 {
            color: #333;
            font-family: "Allerta", sans-serif;
            font-weight: 500;
            font-style: normal;
            font-size: 25px;
            margin-top: 25px;
            margin-left: 15px;
        }

        .logo img{
            height: 60px;
            margin-left: 15px;
            margin-top: 15px;
            
        }
        
        nav {
            display: flex;
            justify-content: space-between;
            /* border: 1px solid red; */
            bottom: 0;
            /* padding: 10px; */
            height: 55px;
            background-color: rgb(241, 240, 235);
            display: flex;
            justify-content: left;
            align-items: center;
        }

        .banner {
            /* border: 1px solid red; */
            height: 550px;
            display: grid;
            grid-template-columns: repeat(2,1fr);
            display: flex;
            justify-content: center;
            align-items: center;
        }
        .container{
            display: flex;
            width: 50%;
            height: 80%;
            border: 3px solid #ccc;
            border-radius: 7px;
            background-color: white;
        }
        .left{
            width: 35%;
            font-weight: bold;
            font-family: "Allerta", sans-serif;
            font-optical-sizing: auto;
            font-weight: 600;
            font-style: normal;
            font-size: 20px;
        }
        .left p{
            padding: 20px 20px;
            color: #7E91C6;
        }
        .methods{
            padding: 10px 0;
        }
        .methods div{
            padding: 5px 20px;
            cursor: pointer;
            font-size: 15px;
        }
        .methods div:hover{
            box-shadow: 0 0 10px rgba(57, 59, 53, 0.196);
        }
        .methods div::after{
            content: '';
            display: inline-block;
            color: #F28585;
            width: 0;
            height: 0;
            border-color: transparent;
            border-style: solid;
            border-width: 4px 0 4px 4px;
            border-left-color: var(--fondy-success);
            margin-left: 10px;
        }
        .center{
            width: 65%;
            border-left: 3px solid #ccc;
            height: 100%;
        }

        .qr-details{
            margin-top: 15px;
            display: none;
            justify-content: center;
            align-items: center;
            text-align: center;
            font-family: "Allerta", sans-serif;
            font-optical-sizing: auto;
            font-weight: 600;
            font-style: normal;
            font-size: 35px;
        }

        .qr-image {
            text-align: center;
            padding: 25px 115px;
        }

        .coin-details {
            display: none;
            text-align: center;
            justify-content: center;
            align-items: center;
            margin-top: 35%;
            font-family: "Allerta", sans-serif;
            font-optical-sizing: auto;
            font-weight: 600;
            font-style: normal;
            font-size: 25px;
        }

        .footer-main {
            display: grid;
            grid-template-columns: repeat(2,1fr);
            background-color: #333;
            color: white;
            text-align: center;
            padding: 20px;
            font-family: "Maven Pro", sans-serif;
            font-optical-sizing: auto;
            font-weight: 700;
            font-style: normal
        }

        .footer-contact {
            text-align: center;
        }
    </style>
</head>
<body>
    <div class="bg">
        <!-- Nav Section -->
        <nav>
            <div class="logo">
                <a href="https://pic.in.th/image/2SkB3M"><img src="https://img2.pic.in.th/pic/sleepless3-lotus.th.png" alt="sleepless3-lotus.png"/></a>
            </div>
            <div class="logo-text">
                <h1>Claw Machine Website</h1>
            </div>
        </nav>
        <!-- Nav Section -->

        <!-- Banner Section Start -->
        <section class="banner">
            <div class="container">
                <div class="left">
                    <p>Payment methods</p>
                    <hr style="border:1px solid #ccc; margin: 0 20px;">
                    <div class="methods">
                        <div onclick="doFun_qrcode();" id="tColorA"><i class="fa-solid fa-credit-card" style="color: #333;"></i> QR Promptpay</div>
                        <div onclick="doFun_coin();" id="tColorB"><i class="fa-solid fa-building-columns"></i> Pay by cash</div>
                    </div>
                    <hr style="border:1px solid #ccc; margin: 0 20px;">
                </div>
                <div class="center">
                    <div class="qr-details">
                        <p style="color: #333;">Scan to pay</p>
                        <div class="qr-image">
                            <div id="qrcode"></div>
                        </div>
                    </div>
                    <div class="coin-details">
                        <p style="color: #333;">20 Baht. / 1 Game (ONLY COIN)</p>
                    </div>
                </div>
            </div>
        </section>
        <!-- Banner Section End -->

        </div>
        <!-- Footer Section Start -->
            <footer>
                <div class="footer-main">
                    <div class="footer-about">
                        <h3 style="color: #e6a4a4;">ABOUT US</h3>
                        <br>
                        <p style="font-weight: 400;">This is the mini project of 240-371 IOT SYSTEM DEVELOPER MODULE</p>
                    </div>
                    <div class="footer-contact">
                        <h3 style="color: #e6a4a4;">SLEEPLESS MEMBER</h3>
                        <br>
                        <p style="font-weight: 400;">Mr. Sathaporn Punboong Student ID:6410110516</p>
                        <p style="font-weight: 400;">Mr. Chatchai Jankaew   Student ID:6410110644</p>
                        <p style="font-weight: 400;">Miss. Yanisa Thaenthong Student ID:6410110649</p>
                    </div>
                </div>
            </footer>
        <!-- Footer Section End -->
    </div>

	<script>
        let tColorA = document.getElementById('tColorA'),
        tColorB = document.getElementById('tColorB'),
        iconA = document.querySelector('.fa-credit-card'),
        iconB = document.querySelector('.fa-building-columns'),
        cDetails = document.querySelector('.qr-details');
        coinDetails = document.querySelector('.coin-details');


        function doFun_qrcode(){
        tColorA.style.color = "#F28585";
        tColorB.style.color = "#444";
        iconA.style.color = "#F28585";
        iconB.style.color = "#aaa";
        cDetails.style.display = "block";
        coinDetails.style.display = "none";
        }
        function doFun_coin(){
        tColorA.style.color = "#444";
        tColorB.style.color = "#F28585";
        iconA.style.color = "#aaa";
        iconB.style.color = "#F28585";
        cDetails.style.display = "none";
        coinDetails.style.display = "block";
        }
    </script>

    <script src="https://cdn.jsdelivr.net/npm/sweetalert2@11"></script>
    <script type="text/javascript">
function showSuccess() {
    $(document).ready(function() {
        Swal.fire({ 
            title: "Payment Successful",
            text: "It's time to Play!",
            icon: "success",
            showConfirmButton: false,
        }).then(function() {
            // Make an AJAX request to the server to update timer status and unlock servo
            $.ajax({
                url: "/successAction",
                type: "POST",
                data: { time: true },
                success: function(response) {
                    console.log("Timer status updated and servo unlocked.");
                    // Redirect to game page
                    window.location.href = '/game';
                    setEmailStatus(false);
                },
                error: function(xhr, status, error) {
                    console.error("Error:", error);
                }
            });
        });
    });
}


        // Function to fetch the MQTT status from the server
        function fetchMQTTStatus() {
            $.get("/getPaymentData", function(data) {
                // Update the HTML with the received MQTT status
                $("#mqtt").text(data.mqtt ? "True" : "False");

                // Check if the payment is completed and trigger the success message
                if (data.mqtt) {
                    showSuccess();
                    console.log(data.mqtt);
                } else {
                    console.log("no payment");
                }
            });
        }

        // Function to set setEmailStatusthe MQTT status on the server
        function setEmailStatus(status) {
            $.post("/setPaymentData", { mqtt: status });
        }

        // Call the fetchMQTTStatus function periodically to update the status
        setInterval(fetchMQTTStatus, 3000);
    </script>

    <script type="text/javascript">      
      // get QR-dom
      var qr_dom = document.getElementById('qrcode');
      var payment_status_dom = document.getElementById('payment_status');

      // render QR function
      function render_qr(){
        var acc_id = "0635854029";
        var amount = 20; // Fixed amount for demonstration
        var txt = PromptPayQR.gen_text(acc_id, amount);

        qr_dom.innerHTML = "";
        if(txt){
          new QRCode(qr_dom, txt);
        }
      }

      // bind acc_id, amount, download
      document.getElementById('tColorA').addEventListener('click', render_qr)

    var PromptPayQR = {

  // https://gist.github.com/chitchcock/5112270
  // Modified from http://automationwiki.com/index.php?title=CRC-16-CCITT
  checksum: function(s){
    var crcTable = [
      0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5,
      0x60c6, 0x70e7, 0x8108, 0x9129, 0xa14a, 0xb16b,
      0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210,
      0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
      0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c,
      0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
      0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b,
      0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
      0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6,
      0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738,
      0xf7df, 0xe7fe, 0xd79d, 0xc7bc, 0x48c4, 0x58e5,
      0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
      0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969,
      0xa90a, 0xb92b, 0x5af5, 0x4ad4, 0x7ab7, 0x6a96,
      0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc,
      0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
      0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03,
      0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd,
      0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6,
      0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
      0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a,
      0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb,
      0xd10c, 0xc12d, 0xf14e, 0xe16f, 0x1080, 0x00a1,
      0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
      0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c,
      0xe37f, 0xf35e, 0x02b1, 0x1290, 0x22f3, 0x32d2,
      0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb,
      0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
      0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447,
      0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
      0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2,
      0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
      0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9,
      0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827,
      0x18c0, 0x08e1, 0x3882, 0x28a3, 0xcb7d, 0xdb5c,
      0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
      0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0,
      0x2ab3, 0x3a92, 0xfd2e, 0xed0f, 0xdd6c, 0xcd4d,
      0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07,
      0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
      0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba,
      0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
      0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
    ];
    var crc = 0xFFFF;
    var j, i;
    for (i = 0; i < s.length; i++) {
        c = s.charCodeAt(i);
        if (c > 255) {
            throw new RangeError();
        }
        j = (c ^ (crc >> 8)) & 0xFF;
        crc = crcTable[j] ^ (crc << 8);
    }
    // return %04d
    var chksum = ((crc ^ 0) & 0xFFFF).toString(16);
    return ('0000' + chksum).slice(-4).toUpperCase();
  },

  gen_text: function(acc_id, amount){
    var pp_acc_id = '';
    var pp_amount = '';
    var pp_chksum = '';

    // process acc_id
    if(!acc_id){ // acc_id not found
      return null;
    }
    else if(acc_id.match(/^\d{15}$/)){ // truemoney e-wallet
      pp_acc_id = '0315' + acc_id;
    }
    else if(acc_id.match(/^\d{13}$/)){ // card-id
      pp_acc_id = '0213' + acc_id;
    }
    else if(acc_id.match(/^\d{10}$/)){ // tel-no
      pp_acc_id = '01130066' + acc_id.substr(1);
    }
    else { // invalid acc_id
      return null;
    }
    // process amount
    if(amount){
      amount = amount.toString();
      pp_amount = '54' + ('00' + amount.length).slice(-2) + amount;
    }
    // build pp string
    var field_29 = "0016A000000677010111" + pp_acc_id;
    var pp_str = "000201010211"
                  + "29" + field_29.length + field_29
                  + "5303764"
                  + pp_amount
                  + "5802TH"
                  + "6304";
    // process checksum
    pp_chksum = this.checksum(pp_str);
    pp_str += pp_chksum;
    return pp_str;
  },
}

var QRCode;!function(){function a(a){this.mode=c.MODE_8BIT_BYTE,this.data=a,this.parsedData=[];for(var b=[],d=0,e=this.data.length;e>d;d++){var f=this.data.charCodeAt(d);f>65536?(b[0]=240|(1835008&f)>>>18,b[1]=128|(258048&f)>>>12,b[2]=128|(4032&f)>>>6,b[3]=128|63&f):f>2048?(b[0]=224|(61440&f)>>>12,b[1]=128|(4032&f)>>>6,b[2]=128|63&f):f>128?(b[0]=192|(1984&f)>>>6,b[1]=128|63&f):b[0]=f,this.parsedData=this.parsedData.concat(b)}this.parsedData.length!=this.data.length&&(this.parsedData.unshift(191),this.parsedData.unshift(187),this.parsedData.unshift(239))}function b(a,b){this.typeNumber=a,this.errorCorrectLevel=b,this.modules=null,this.moduleCount=0,this.dataCache=null,this.dataList=[]}function i(a,b){if(void 0==a.length)throw new Error(a.length+'/'+b);for(var c=0;c<a.length&&0==a[c];)c++;this.num=new Array(a.length-c+b);for(var d=0;d<a.length-c;d++)this.num[d]=a[d+c]}function j(a,b){this.totalCount=a,this.dataCount=b}function k(){this.buffer=[],this.length=0}function m(){return'undefined'!=typeof CanvasRenderingContext2D}function n(){var a=!1,b=navigator.userAgent;return/android/i.test(b)&&(a=!0,aMat=b.toString().match(/android ([0-9]\.[0-9])/i),aMat&&aMat[1]&&(a=parseFloat(aMat[1]))),a}function r(a,b){for(var c=1,e=s(a),f=0,g=l.length;g>=f;f++){var h=0;switch(b){case d.L:h=l[f][0];break;case d.M:h=l[f][1];break;case d.Q:h=l[f][2];break;case d.H:h=l[f][3]}if(h>=e)break;c++}if(c>l.length)throw new Error('Too long data');return c}function s(a){var b=encodeURI(a).toString().replace(/\%[0-9a-fA-F]{2}/g,'a');return b.length+(b.length!=a?3:0)}a.prototype={getLength:function(){return this.parsedData.length},write:function(a){for(var b=0,c=this.parsedData.length;c>b;b++)a.put(this.parsedData[b],8)}},b.prototype={addData:function(b){var c=new a(b);this.dataList.push(c),this.dataCache=null},isDark:function(a,b){if(0>a||this.moduleCount<=a||0>b||this.moduleCount<=b)throw new Error(a+','+b);return this.modules[a][b]},getModuleCount:function(){return this.moduleCount},make:function(){this.makeImpl(!1,this.getBestMaskPattern())},makeImpl:function(a,c){this.moduleCount=4*this.typeNumber+17,this.modules=new Array(this.moduleCount);for(var d=0;d<this.moduleCount;d++){this.modules[d]=new Array(this.moduleCount);for(var e=0;e<this.moduleCount;e++)this.modules[d][e]=null}this.setupPositionProbePattern(0,0),this.setupPositionProbePattern(this.moduleCount-7,0),this.setupPositionProbePattern(0,this.moduleCount-7),this.setupPositionAdjustPattern(),this.setupTimingPattern(),this.setupTypeInfo(a,c),this.typeNumber>=7&&this.setupTypeNumber(a),null==this.dataCache&&(this.dataCache=b.createData(this.typeNumber,this.errorCorrectLevel,this.dataList)),this.mapData(this.dataCache,c)},setupPositionProbePattern:function(a,b){for(var c=-1;7>=c;c++)if(!(-1>=a+c||this.moduleCount<=a+c))for(var d=-1;7>=d;d++)-1>=b+d||this.moduleCount<=b+d||(this.modules[a+c][b+d]=c>=0&&6>=c&&(0==d||6==d)||d>=0&&6>=d&&(0==c||6==c)||c>=2&&4>=c&&d>=2&&4>=d?!0:!1)},getBestMaskPattern:function(){for(var a=0,b=0,c=0;8>c;c++){this.makeImpl(!0,c);var d=f.getLostPoint(this);(0==c||a>d)&&(a=d,b=c)}return b},createMovieClip:function(a,b,c){var d=a.createEmptyMovieClip(b,c),e=1;this.make();for(var f=0;f<this.modules.length;f++)for(var g=f*e,h=0;h<this.modules[f].length;h++){var i=h*e,j=this.modules[f][h];j&&(d.beginFill(0,100),d.moveTo(i,g),d.lineTo(i+e,g),d.lineTo(i+e,g+e),d.lineTo(i,g+e),d.endFill())}return d},setupTimingPattern:function(){for(var a=8;a<this.moduleCount-8;a++)null==this.modules[a][6]&&(this.modules[a][6]=0==a%2);for(var b=8;b<this.moduleCount-8;b++)null==this.modules[6][b]&&(this.modules[6][b]=0==b%2)},setupPositionAdjustPattern:function(){for(var a=f.getPatternPosition(this.typeNumber),b=0;b<a.length;b++)for(var c=0;c<a.length;c++){var d=a[b],e=a[c];if(null==this.modules[d][e])for(var g=-2;2>=g;g++)for(var h=-2;2>=h;h++)this.modules[d+g][e+h]=-2==g||2==g||-2==h||2==h||0==g&&0==h?!0:!1}},setupTypeNumber:function(a){for(var b=f.getBCHTypeNumber(this.typeNumber),c=0;18>c;c++){var d=!a&&1==(1&b>>c);this.modules[Math.floor(c/3)][c%3+this.moduleCount-8-3]=d}for(var c=0;18>c;c++){var d=!a&&1==(1&b>>c);this.modules[c%3+this.moduleCount-8-3][Math.floor(c/3)]=d}},setupTypeInfo:function(a,b){for(var c=this.errorCorrectLevel<<3|b,d=f.getBCHTypeInfo(c),e=0;15>e;e++){var g=!a&&1==(1&d>>e);6>e?this.modules[e][8]=g:8>e?this.modules[e+1][8]=g:this.modules[this.moduleCount-15+e][8]=g}for(var e=0;15>e;e++){var g=!a&&1==(1&d>>e);8>e?this.modules[8][this.moduleCount-e-1]=g:9>e?this.modules[8][15-e-1+1]=g:this.modules[8][15-e-1]=g}this.modules[this.moduleCount-8][8]=!a},mapData:function(a,b){for(var c=-1,d=this.moduleCount-1,e=7,g=0,h=this.moduleCount-1;h>0;h-=2)for(6==h&&h--;;){for(var i=0;2>i;i++)if(null==this.modules[d][h-i]){var j=!1;g<a.length&&(j=1==(1&a[g]>>>e));var k=f.getMask(b,d,h-i);k&&(j=!j),this.modules[d][h-i]=j,e--,-1==e&&(g++,e=7)}if(d+=c,0>d||this.moduleCount<=d){d-=c,c=-c;break}}}},b.PAD0=236,b.PAD1=17,b.createData=function(a,c,d){for(var e=j.getRSBlocks(a,c),g=new k,h=0;h<d.length;h++){var i=d[h];g.put(i.mode,4),g.put(i.getLength(),f.getLengthInBits(i.mode,a)),i.write(g)}for(var l=0,h=0;h<e.length;h++)l+=e[h].dataCount;if(g.getLengthInBits()>8*l)throw new Error('code length overflow. ('+g.getLengthInBits()+'>'+8*l+')');for(g.getLengthInBits()+4<=8*l&&g.put(0,4);0!=g.getLengthInBits()%8;)g.putBit(!1);for(;;){if(g.getLengthInBits()>=8*l)break;if(g.put(b.PAD0,8),g.getLengthInBits()>=8*l)break;g.put(b.PAD1,8)}return b.createBytes(g,e)},b.createBytes=function(a,b){for(var c=0,d=0,e=0,g=new Array(b.length),h=new Array(b.length),j=0;j<b.length;j++){var k=b[j].dataCount,l=b[j].totalCount-k;d=Math.max(d,k),e=Math.max(e,l),g[j]=new Array(k);for(var m=0;m<g[j].length;m++)g[j][m]=255&a.buffer[m+c];c+=k;var n=f.getErrorCorrectPolynomial(l),o=new i(g[j],n.getLength()-1),p=o.mod(n);h[j]=new Array(n.getLength()-1);for(var m=0;m<h[j].length;m++){var q=m+p.getLength()-h[j].length;h[j][m]=q>=0?p.get(q):0}}for(var r=0,m=0;m<b.length;m++)r+=b[m].totalCount;for(var s=new Array(r),t=0,m=0;d>m;m++)for(var j=0;j<b.length;j++)m<g[j].length&&(s[t++]=g[j][m]);for(var m=0;e>m;m++)for(var j=0;j<b.length;j++)m<h[j].length&&(s[t++]=h[j][m]);return s};for(var c={MODE_NUMBER:1,MODE_ALPHA_NUM:2,MODE_8BIT_BYTE:4,MODE_KANJI:8},d={L:1,M:0,Q:3,H:2},e={PATTERN000:0,PATTERN001:1,PATTERN010:2,PATTERN011:3,PATTERN100:4,PATTERN101:5,PATTERN110:6,PATTERN111:7},f={PATTERN_POSITION_TABLE:[[],[6,18],[6,22],[6,26],[6,30],[6,34],[6,22,38],[6,24,42],[6,26,46],[6,28,50],[6,30,54],[6,32,58],[6,34,62],[6,26,46,66],[6,26,48,70],[6,26,50,74],[6,30,54,78],[6,30,56,82],[6,30,58,86],[6,34,62,90],[6,28,50,72,94],[6,26,50,74,98],[6,30,54,78,102],[6,28,54,80,106],[6,32,58,84,110],[6,30,58,86,114],[6,34,62,90,118],[6,26,50,74,98,122],[6,30,54,78,102,126],[6,26,52,78,104,130],[6,30,56,82,108,134],[6,34,60,86,112,138],[6,30,58,86,114,142],[6,34,62,90,118,146],[6,30,54,78,102,126,150],[6,24,50,76,102,128,154],[6,28,54,80,106,132,158],[6,32,58,84,110,136,162],[6,26,54,82,110,138,166],[6,30,58,86,114,142,170]],G15:1335,G18:7973,G15_MASK:21522,getBCHTypeInfo:function(a){for(var b=a<<10;f.getBCHDigit(b)-f.getBCHDigit(f.G15)>=0;)b^=f.G15<<f.getBCHDigit(b)-f.getBCHDigit(f.G15);return(a<<10|b)^f.G15_MASK},getBCHTypeNumber:function(a){for(var b=a<<12;f.getBCHDigit(b)-f.getBCHDigit(f.G18)>=0;)b^=f.G18<<f.getBCHDigit(b)-f.getBCHDigit(f.G18);return a<<12|b},getBCHDigit:function(a){for(var b=0;0!=a;)b++,a>>>=1;return b},getPatternPosition:function(a){return f.PATTERN_POSITION_TABLE[a-1]},getMask:function(a,b,c){switch(a){case e.PATTERN000:return 0==(b+c)%2;case e.PATTERN001:return 0==b%2;case e.PATTERN010:return 0==c%3;case e.PATTERN011:return 0==(b+c)%3;case e.PATTERN100:return 0==(Math.floor(b/2)+Math.floor(c/3))%2;case e.PATTERN101:return 0==b*c%2+b*c%3;case e.PATTERN110:return 0==(b*c%2+b*c%3)%2;case e.PATTERN111:return 0==(b*c%3+(b+c)%2)%2;default:throw new Error('bad maskPattern:'+a)}},getErrorCorrectPolynomial:function(a){for(var b=new i([1],0),c=0;a>c;c++)b=b.multiply(new i([1,g.gexp(c)],0));return b},getLengthInBits:function(a,b){if(b>=1&&10>b)switch(a){case c.MODE_NUMBER:return 10;case c.MODE_ALPHA_NUM:return 9;case c.MODE_8BIT_BYTE:return 8;case c.MODE_KANJI:return 8;default:throw new Error('mode:'+a)}else if(27>b)switch(a){case c.MODE_NUMBER:return 12;case c.MODE_ALPHA_NUM:return 11;case c.MODE_8BIT_BYTE:return 16;case c.MODE_KANJI:return 10;default:throw new Error('mode:'+a)}else{if(!(41>b))throw new Error('type:'+b);switch(a){case c.MODE_NUMBER:return 14;case c.MODE_ALPHA_NUM:return 13;case c.MODE_8BIT_BYTE:return 16;case c.MODE_KANJI:return 12;default:throw new Error('mode:'+a)}}},getLostPoint:function(a){for(var b=a.getModuleCount(),c=0,d=0;b>d;d++)for(var e=0;b>e;e++){for(var f=0,g=a.isDark(d,e),h=-1;1>=h;h++)if(!(0>d+h||d+h>=b))for(var i=-1;1>=i;i++)0>e+i||e+i>=b||(0!=h||0!=i)&&g==a.isDark(d+h,e+i)&&f++;f>5&&(c+=3+f-5)}for(var d=0;b-1>d;d++)for(var e=0;b-1>e;e++){var j=0;a.isDark(d,e)&&j++,a.isDark(d+1,e)&&j++,a.isDark(d,e+1)&&j++,a.isDark(d+1,e+1)&&j++,(0==j||4==j)&&(c+=3)}for(var d=0;b>d;d++)for(var e=0;b-6>e;e++)a.isDark(d,e)&&!a.isDark(d,e+1)&&a.isDark(d,e+2)&&a.isDark(d,e+3)&&a.isDark(d,e+4)&&!a.isDark(d,e+5)&&a.isDark(d,e+6)&&(c+=40);for(var e=0;b>e;e++)for(var d=0;b-6>d;d++)a.isDark(d,e)&&!a.isDark(d+1,e)&&a.isDark(d+2,e)&&a.isDark(d+3,e)&&a.isDark(d+4,e)&&!a.isDark(d+5,e)&&a.isDark(d+6,e)&&(c+=40);for(var k=0,e=0;b>e;e++)for(var d=0;b>d;d++)a.isDark(d,e)&&k++;var l=Math.abs(100*k/b/b-50)/5;return c+=10*l}},g={glog:function(a){if(1>a)throw new Error('glog('+a+')');return g.LOG_TABLE[a]},gexp:function(a){for(;0>a;)a+=255;for(;a>=256;)a-=255;return g.EXP_TABLE[a]},EXP_TABLE:new Array(256),LOG_TABLE:new Array(256)},h=0;8>h;h++)g.EXP_TABLE[h]=1<<h;for(var h=8;256>h;h++)g.EXP_TABLE[h]=g.EXP_TABLE[h-4]^g.EXP_TABLE[h-5]^g.EXP_TABLE[h-6]^g.EXP_TABLE[h-8];for(var h=0;255>h;h++)g.LOG_TABLE[g.EXP_TABLE[h]]=h;i.prototype={get:function(a){return this.num[a]},getLength:function(){return this.num.length},multiply:function(a){for(var b=new Array(this.getLength()+a.getLength()-1),c=0;c<this.getLength();c++)for(var d=0;d<a.getLength();d++)b[c+d]^=g.gexp(g.glog(this.get(c))+g.glog(a.get(d)));return new i(b,0)},mod:function(a){if(this.getLength()-a.getLength()<0)return this;for(var b=g.glog(this.get(0))-g.glog(a.get(0)),c=new Array(this.getLength()),d=0;d<this.getLength();d++)c[d]=this.get(d);for(var d=0;d<a.getLength();d++)c[d]^=g.gexp(g.glog(a.get(d))+b);return new i(c,0).mod(a)}},j.RS_BLOCK_TABLE=[[1,26,19],[1,26,16],[1,26,13],[1,26,9],[1,44,34],[1,44,28],[1,44,22],[1,44,16],[1,70,55],[1,70,44],[2,35,17],[2,35,13],[1,100,80],[2,50,32],[2,50,24],[4,25,9],[1,134,108],[2,67,43],[2,33,15,2,34,16],[2,33,11,2,34,12],[2,86,68],[4,43,27],[4,43,19],[4,43,15],[2,98,78],[4,49,31],[2,32,14,4,33,15],[4,39,13,1,40,14],[2,121,97],[2,60,38,2,61,39],[4,40,18,2,41,19],[4,40,14,2,41,15],[2,146,116],[3,58,36,2,59,37],[4,36,16,4,37,17],[4,36,12,4,37,13],[2,86,68,2,87,69],[4,69,43,1,70,44],[6,43,19,2,44,20],[6,43,15,2,44,16],[4,101,81],[1,80,50,4,81,51],[4,50,22,4,51,23],[3,36,12,8,37,13],[2,116,92,2,117,93],[6,58,36,2,59,37],[4,46,20,6,47,21],[7,42,14,4,43,15],[4,133,107],[8,59,37,1,60,38],[8,44,20,4,45,21],[12,33,11,4,34,12],[3,145,115,1,146,116],[4,64,40,5,65,41],[11,36,16,5,37,17],[11,36,12,5,37,13],[5,109,87,1,110,88],[5,65,41,5,66,42],[5,54,24,7,55,25],[11,36,12],[5,122,98,1,123,99],[7,73,45,3,74,46],[15,43,19,2,44,20],[3,45,15,13,46,16],[1,135,107,5,136,108],[10,74,46,1,75,47],[1,50,22,15,51,23],[2,42,14,17,43,15],[5,150,120,1,151,121],[9,69,43,4,70,44],[17,50,22,1,51,23],[2,42,14,19,43,15],[3,141,113,4,142,114],[3,70,44,11,71,45],[17,47,21,4,48,22],[9,39,13,16,40,14],[3,135,107,5,136,108],[3,67,41,13,68,42],[15,54,24,5,55,25],[15,43,15,10,44,16],[4,144,116,4,145,117],[17,68,42],[17,50,22,6,51,23],[19,46,16,6,47,17],[2,139,111,7,140,112],[17,74,46],[7,54,24,16,55,25],[34,37,13],[4,151,121,5,152,122],[4,75,47,14,76,48],[11,54,24,14,55,25],[16,45,15,14,46,16],[6,147,117,4,148,118],[6,73,45,14,74,46],[11,54,24,16,55,25],[30,46,16,2,47,17],[8,132,106,4,133,107],[8,75,47,13,76,48],[7,54,24,22,55,25],[22,45,15,13,46,16],[10,142,114,2,143,115],[19,74,46,4,75,47],[28,50,22,6,51,23],[33,46,16,4,47,17],[8,152,122,4,153,123],[22,73,45,3,74,46],[8,53,23,26,54,24],[12,45,15,28,46,16],[3,147,117,10,148,118],[3,73,45,23,74,46],[4,54,24,31,55,25],[11,45,15,31,46,16],[7,146,116,7,147,117],[21,73,45,7,74,46],[1,53,23,37,54,24],[19,45,15,26,46,16],[5,145,115,10,146,116],[19,75,47,10,76,48],[15,54,24,25,55,25],[23,45,15,25,46,16],[13,145,115,3,146,116],[2,74,46,29,75,47],[42,54,24,1,55,25],[23,45,15,28,46,16],[17,145,115],[10,74,46,23,75,47],[10,54,24,35,55,25],[19,45,15,35,46,16],[17,145,115,1,146,116],[14,74,46,21,75,47],[29,54,24,19,55,25],[11,45,15,46,46,16],[13,145,115,6,146,116],[14,74,46,23,75,47],[44,54,24,7,55,25],[59,46,16,1,47,17],[12,151,121,7,152,122],[12,75,47,26,76,48],[39,54,24,14,55,25],[22,45,15,41,46,16],[6,151,121,14,152,122],[6,75,47,34,76,48],[46,54,24,10,55,25],[2,45,15,64,46,16],[17,152,122,4,153,123],[29,74,46,14,75,47],[49,54,24,10,55,25],[24,45,15,46,46,16],[4,152,122,18,153,123],[13,74,46,32,75,47],[48,54,24,14,55,25],[42,45,15,32,46,16],[20,147,117,4,148,118],[40,75,47,7,76,48],[43,54,24,22,55,25],[10,45,15,67,46,16],[19,148,118,6,149,119],[18,75,47,31,76,48],[34,54,24,34,55,25],[20,45,15,61,46,16]],j.getRSBlocks=function(a,b){var c=j.getRsBlockTable(a,b);if(void 0==c)throw new Error('bad rs block @ typeNumber:'+a+'/errorCorrectLevel:'+b);for(var d=c.length/3,e=[],f=0;d>f;f++)for(var g=c[3*f+0],h=c[3*f+1],i=c[3*f+2],k=0;g>k;k++)e.push(new j(h,i));return e},j.getRsBlockTable=function(a,b){switch(b){case d.L:return j.RS_BLOCK_TABLE[4*(a-1)+0];case d.M:return j.RS_BLOCK_TABLE[4*(a-1)+1];case d.Q:return j.RS_BLOCK_TABLE[4*(a-1)+2];case d.H:return j.RS_BLOCK_TABLE[4*(a-1)+3];default:return void 0}},k.prototype={get:function(a){var b=Math.floor(a/8);return 1==(1&this.buffer[b]>>>7-a%8)},put:function(a,b){for(var c=0;b>c;c++)this.putBit(1==(1&a>>>b-c-1))},getLengthInBits:function(){return this.length},putBit:function(a){var b=Math.floor(this.length/8);this.buffer.length<=b&&this.buffer.push(0),a&&(this.buffer[b]|=128>>>this.length%8),this.length++}};var l=[[17,14,11,7],[32,26,20,14],[53,42,32,24],[78,62,46,34],[106,84,60,44],[134,106,74,58],[154,122,86,64],[192,152,108,84],[230,180,130,98],[271,213,151,119],[321,251,177,137],[367,287,203,155],[425,331,241,177],[458,362,258,194],[520,412,292,220],[586,450,322,250],[644,504,364,280],[718,560,394,310],[792,624,442,338],[858,666,482,382],[929,711,509,403],[1003,779,565,439],[1091,857,611,461],[1171,911,661,511],[1273,997,715,535],[1367,1059,751,593],[1465,1125,805,625],[1528,1190,868,658],[1628,1264,908,698],[1732,1370,982,742],[1840,1452,1030,790],[1952,1538,1112,842],[2068,1628,1168,898],[2188,1722,1228,958],[2303,1809,1283,983],[2431,1911,1351,1051],[2563,1989,1423,1093],[2699,2099,1499,1139],[2809,2213,1579,1219],[2953,2331,1663,1273]],o=function(){var a=function(a,b){this._el=a,this._htOption=b};return a.prototype.draw=function(a){function g(a,b){var c=document.createElementNS('http://www.w3.org/2000/svg',a);for(var d in b)b.hasOwnProperty(d)&&c.setAttribute(d,b[d]);return c}var b=this._htOption,c=this._el,d=a.getModuleCount();Math.floor(b.width/d),Math.floor(b.height/d),this.clear();var h=g('svg',{viewBox:'0 0 '+String(d)+' '+String(d),width:'100%',height:'100%',fill:b.colorLight});h.setAttributeNS('http://www.w3.org/2000/xmlns/','xmlns:xlink','http://www.w3.org/1999/xlink'),c.appendChild(h),h.appendChild(g('rect',{fill:b.colorDark,width:'1',height:'1',id:'template'}));for(var i=0;d>i;i++)for(var j=0;d>j;j++)if(a.isDark(i,j)){var k=g('use',{x:String(i),y:String(j)});k.setAttributeNS('http://www.w3.org/1999/xlink','href','#template'),h.appendChild(k)}},a.prototype.clear=function(){for(;this._el.hasChildNodes();)this._el.removeChild(this._el.lastChild)},a}(),p='svg'===document.documentElement.tagName.toLowerCase(),q=p?o:m()?function(){function a(){this._elImage.src=this._elCanvas.toDataURL('image/png'),this._elImage.style.display='block',this._elCanvas.style.display='none'}function d(a,b){var c=this;if(c._fFail=b,c._fSuccess=a,null===c._bSupportDataURI){var d=document.createElement('img'),e=function(){c._bSupportDataURI=!1,c._fFail&&_fFail.call(c)},f=function(){c._bSupportDataURI=!0,c._fSuccess&&c._fSuccess.call(c)};return d.onabort=e,d.onerror=e,d.onload=f,d.src='data:image/gif;base64,iVBORw0KGgoAAAANSUhEUgAAAAUAAAAFCAYAAACNbyblAAAAHElEQVQI12P4//8/w38GIAXDIBKE0DHxgljNBAAO9TXL0Y4OHwAAAABJRU5ErkJggg==',void 0}c._bSupportDataURI===!0&&c._fSuccess?c._fSuccess.call(c):c._bSupportDataURI===!1&&c._fFail&&c._fFail.call(c)}if(this._android&&this._android<=2.1){var b=1/window.devicePixelRatio,c=CanvasRenderingContext2D.prototype.drawImage;CanvasRenderingContext2D.prototype.drawImage=function(a,d,e,f,g,h,i,j){if('nodeName'in a&&/img/i.test(a.nodeName))for(var l=arguments.length-1;l>=1;l--)arguments[l]=arguments[l]*b;else'undefined'==typeof j&&(arguments[1]*=b,arguments[2]*=b,arguments[3]*=b,arguments[4]*=b);c.apply(this,arguments)}}var e=function(a,b){this._bIsPainted=!1,this._android=n(),this._htOption=b,this._elCanvas=document.createElement('canvas'),this._elCanvas.width=b.width,this._elCanvas.height=b.height,a.appendChild(this._elCanvas),this._el=a,this._oContext=this._elCanvas.getContext('2d'),this._bIsPainted=!1,this._elImage=document.createElement('img'),this._elImage.style.display='none',this._el.appendChild(this._elImage),this._bSupportDataURI=null};return e.prototype.draw=function(a){var b=this._elImage,c=this._oContext,d=this._htOption,e=a.getModuleCount(),f=d.width/e,g=d.height/e,h=Math.round(f),i=Math.round(g);b.style.display='none',this.clear();for(var j=0;e>j;j++)for(var k=0;e>k;k++){var l=a.isDark(j,k),m=k*f,n=j*g;c.strokeStyle=l?d.colorDark:d.colorLight,c.lineWidth=1,c.fillStyle=l?d.colorDark:d.colorLight,c.fillRect(m,n,f,g),c.strokeRect(Math.floor(m)+.5,Math.floor(n)+.5,h,i),c.strokeRect(Math.ceil(m)-.5,Math.ceil(n)-.5,h,i)}this._bIsPainted=!0},e.prototype.makeImage=function(){this._bIsPainted&&d.call(this,a)},e.prototype.isPainted=function(){return this._bIsPainted},e.prototype.clear=function(){this._oContext.clearRect(0,0,this._elCanvas.width,this._elCanvas.height),this._bIsPainted=!1},e.prototype.round=function(a){return a?Math.floor(1e3*a)/1e3:a},e}():function(){var a=function(a,b){this._el=a,this._htOption=b};return a.prototype.draw=function(a){for(var b=this._htOption,c=this._el,d=a.getModuleCount(),e=Math.floor(b.width/d),f=Math.floor(b.height/d),g=['<table style="border:0;border-collapse:collapse;">'],h=0;d>h;h++){g.push('<tr>');for(var i=0;d>i;i++)g.push('<td style="border:0;border-collapse:collapse;padding:0;margin:0;width:"+e+"px;height:"+f+"px;background-color:"+(a.isDark(h,i)?b.colorDark:b.colorLight)+";"></td>');g.push('</tr>')}g.push('</table>'),c.innerHTML=g.join('');var j=c.childNodes[0],k=(b.width-j.offsetWidth)/2,l=(b.height-j.offsetHeight)/2;k>0&&l>0&&(j.style.margin=l+'px '+k+'px')},a.prototype.clear=function(){this._el.innerHTML=''},a}();QRCode=function(a,b){if(this._htOption={width:256,height:256,typeNumber:4,colorDark:'#000000',colorLight:'#ffffff',correctLevel:d.H},'string'==typeof b&&(b={text:b}),b)for(var c in b)this._htOption[c]=b[c];'string'==typeof a&&(a=document.getElementById(a)),this._android=n(),this._el=a,this._oQRCode=null,this._oDrawing=new q(this._el,this._htOption),this._htOption.text&&this.makeCode(this._htOption.text)},QRCode.prototype.makeCode=function(a){this._oQRCode=new b(r(a,this._htOption.correctLevel),this._htOption.correctLevel),this._oQRCode.addData(a),this._oQRCode.make(),this._el.title=a,this._oDrawing.draw(this._oQRCode),this.makeImage()},QRCode.prototype.makeImage=function(){'function'==typeof this._oDrawing.makeImage&&(!this._android||this._android>=3)&&this._oDrawing.makeImage()},QRCode.prototype.clear=function(){this._oDrawing.clear()},QRCode.CorrectLevel=d}();
</script>
</body>
</html>
)";

const char *game_html PROGMEM = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.7.1/jquery.min.js"></script>
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.6.2/dist/css/bootstrap.min.css">
    <script src="https://cdn.jsdelivr.net/npm/popper.js@1.16.1/dist/umd/popper.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@4.6.2/dist/js/bootstrap.bundle.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/sweetalert2@11"></script>

    <!-- Font -->
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Aladin&family=Allerta&family=Fira+Sans:ital,wght@0,100;0,200;0,300;0,400;0,500;0,600;0,700;0,800;0,900;1,100;1,200;1,300;1,400;1,500;1,600;1,700;1,800;1,900&family=Slackside+One&display=swap" rel="stylesheet">
    <link href="https://fonts.googleapis.com/css2?family=Maven+Pro:wght@400..900&family=Pacifico&display=swap" rel="stylesheet">
    <link href="https://fonts.googleapis.com/css2?family=Chakra+Petch:ital,wght@0,300;0,400;0,500;0,600;0,700;1,300;1,400;1,500;1,600;1,700&display=swap" rel="stylesheet">
    <title>Game Page</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        .bg {
            background-color: rgb(241, 240, 235);
            height: 100%; 
        }

        .logo-text h1 {
            color: #333;
            font-family: "Allerta", sans-serif;
            font-weight: 500;
            font-style: normal;
            font-size: 25px;
            margin-top: 25px;
            margin-left: 15px;
        }

        .logo img{
            height: 60px;
            margin-left: 15px;
            margin-top: 15px;
            
        }
        
        nav {
            display: flex;
            justify-content: space-between;
            /* border: 1px solid red; */
            bottom: 0;
            /* padding: 10px; */
            height: 55px;
            background-color: rgb(241, 240, 235);
            display: flex;
            justify-content: left;
            align-items: center;
        }

        .banner {
            /* border: 1px solid red; */
            color: black;
            height: 550px;
            display: grid;
            grid-template-columns: repeat(2,1fr);
            display: flex;
            justify-content: center;
            align-items: center;
        }

        .base-timer {
            position: relative;
            width: 400px;
            height: 400px;
        }

        .base-timer__svg {
            transform: scaleX(-1);
        }

        .base-timer__circle {
            fill: none;
            stroke: none;
        }

        .base-timer__path-elapsed {
            stroke-width: 7px;
            stroke: grey;
        }

        .base-timer__path-remaining {
            stroke-width: 7px;
            stroke-linecap: round;
            transform: rotate(90deg);
            transform-origin: center;
            transition: 1s linear all;
            fill-rule: nonzero;
            stroke: currentColor;
        }

        .base-timer__path-remaining.green {
            color: rgb(91, 186, 143);
        }

        .base-timer__path-remaining.orange {
            color: rgb(227, 183, 94);
        }

        .base-timer__path-remaining.red {
            color: rgb(245, 68, 68);
        }

        .base-timer__label {
            position: absolute;
            width: 400px;
            height: 380px;
            top: 0;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 90px;
            color: #333;
            font-family: "Chakra Petch", sans-serif;
            font-weight: 400;
            font-style: normal;
        }

        .footer-main {
            display: grid;
            grid-template-columns: repeat(2,1fr);
            background-color: #333;
            color: white;
            text-align: center;
            padding: 20px;
            font-family: "Maven Pro", sans-serif;
            font-optical-sizing: auto;
            font-weight: 700;
            font-style: normal
        }

        .footer-contact {
            text-align: center;
        }

 
    </style>
</head>
<body>
    <div class="bg">
        <!-- Nav Section -->
        <nav>
            <div class="logo">
                <a href="https://pic.in.th/image/2SkB3M"><img src="https://img2.pic.in.th/pic/sleepless3-lotus.th.png" alt="sleepless3-lotus.png"/></a>
            </div>
            <div class="logo-text">
                <h1>Claw Machine Website</h1>
            </div>
        </nav>
        <!-- Nav Section -->
            
        <!-- Banner Section -->
        <section class="banner">
            <div id="app"></div>
        </section>
        <!-- Banner Section -->

        <!-- Footer Section -->
            <footer>
                <div class="footer-main">
                    <div class="footer-about">
                        <h3 style="color: #e6a4a4;">ABOUT US</h3>
                        <br>
                        <p style="font-weight: 400;">This is the mini project of 240-371 IOT SYSTEM DEVELOPER MODULE</p>
                    </div>
                    <div class="footer-contact">
                        <h3 style="color: #e6a4a4;">SLEEPLESS MEMBER</h3>
                        <br>
                        <p style="font-weight: 400;">Mr. Sathaporn Punboong Student ID:6410110516</p>
                        <p style="font-weight: 400;">Mr. Chatchai Jankaew   Student ID:6410110644</p>
                        <p style="font-weight: 400;">Miss. Yanisa Thaenthong Student ID:6410110649</p>
                    </div>
                </div>
            </footer>
        <!-- Footer Section -->
    </div>

    <script>
        // Credit: Mateusz Rybczonec
        
        const FULL_DASH_ARRAY = 283;
        const WARNING_THRESHOLD = 20;
        const ALERT_THRESHOLD = 10;
        
        const COLOR_CODES = {
          info: {
            color: "green"
          },
          warning: {
            color: "orange",
            threshold: WARNING_THRESHOLD
          },
          alert: {
            color: "red",
            threshold: ALERT_THRESHOLD
          }
        };
        
        const TIME_LIMIT = 60;
        let timePassed = 0;
        let timeLeft = TIME_LIMIT;
        let timerInterval = null;
        let remainingPathColor = COLOR_CODES.info.color;
        
        document.getElementById("app").innerHTML = `
        <div class="base-timer">
          <svg class="base-timer__svg" viewBox="0 0 100 100" xmlns="http://www.w3.org/2000/svg">
            <g class="base-timer__circle">
              <circle class="base-timer__path-elapsed" cx="50" cy="50" r="45"></circle>
              <path
                id="base-timer-path-remaining"
                stroke-dasharray="283"
                class="base-timer__path-remaining ${remainingPathColor}"
                d="
                  M 50, 50
                  m -45, 0
                  a 45,45 0 1,0 90,0
                  a 45,45 0 1,0 -90,0
                "
              ></path>
            </g>
          </svg>
          <span id="base-timer-label" class="base-timer__label">${formatTime(
            timeLeft
          )}</span>
        </div>
        `;
        
        startTimer();
        
        function onTimesUp() {
            clearInterval(timerInterval);
            hideTimer();
            showTimeout();
            // Make an AJAX request to the server to update timer status and lock servo
            $.ajax({
                url: "/timeoutAction",
                type: "POST",
                data: { time: false },
                success: function(response) {
                    console.log("Timer status updated and servo locked.");
                },
                error: function(xhr, status, error) {
                    console.error("Error:", error);
                }
            });
        }

        
        function hideTimer() {
            document.querySelector(".base-timer").style.display = "none";
        }
        
        function showTimeout(){
            Swal.fire({
                title: "TIME OUT",
                icon: "warning",
                timer: 3000,
                showConfirmButton: false,
            }).then((result) => {
                if (result.isConfirmed) {
                    window.location.href = '/';
                }
            });
            setTimeout(() => {
                window.location.href = '/';
            }, 3000);
        }
        
        function startTimer() {
          timerInterval = setInterval(() => {
            timePassed = timePassed += 1;
            timeLeft = TIME_LIMIT - timePassed;
            document.getElementById("base-timer-label").innerHTML = formatTime(
              timeLeft
            );
            setCircleDasharray();
            setRemainingPathColor(timeLeft);
        
            if (timeLeft === 0) {
              onTimesUp();
            }
          }, 1000);
        }
        
        function formatTime(time) {
          const minutes = Math.floor(time / 60);
          let seconds = time % 60;
        
          if (seconds < 10) {
            seconds = `0${seconds}`;
          }
        
          return `${minutes}:${seconds}`;
        }
        
        function setRemainingPathColor(timeLeft) {
          const { alert, warning, info } = COLOR_CODES;
          if (timeLeft <= alert.threshold) {
            document
              .getElementById("base-timer-path-remaining")
              .classList.remove(warning.color);
            document
              .getElementById("base-timer-path-remaining")
              .classList.add(alert.color);
          } else if (timeLeft <= warning.threshold) {
            document
              .getElementById("base-timer-path-remaining")
              .classList.remove(info.color);
            document
              .getElementById("base-timer-path-remaining")
              .classList.add(warning.color);
          }
        }
        
        function calculateTimeFraction() {
          const rawTimeFraction = timeLeft / TIME_LIMIT;
          return rawTimeFraction - (1 / TIME_LIMIT) * (1 - rawTimeFraction);
        }
        
        function setCircleDasharray() {
          const circleDasharray = `${(
            calculateTimeFraction() * FULL_DASH_ARRAY
          ).toFixed(0)} 283`;
          document
            .getElementById("base-timer-path-remaining")
            .setAttribute("stroke-dasharray", circleDasharray);
        }
    </script>
</body>
</html>
)";

void Servo_Machine_unlock(){
  String MachineTopic = "esp32/servo";
  String MachineStateStr = "unlock";
  mqttClient.publish(MachineTopic.c_str(), MachineStateStr.c_str());
  Serial.println(MachineStateStr);
}

void Servo_Machine_lock(){
  String MachineTopic = "esp32/servo";
  String MachineStateStr = "lock";
  mqttClient.publish(MachineTopic.c_str(), MachineStateStr.c_str());
  Serial.println(MachineStateStr);
}


void setupWiFi() {
  Serial.println();
  Serial.println("Connecting to " + String(WIFI_SSID) + " ...");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Failed to connect to the Wi-Fi network, restarting...");
      delay(2000);
      ESP.restart();
  }

  // Initialize mDNS
  if (!MDNS.begin("sleepless")) {   // Set the hostname to "sleepless.local"
    Serial.println("Error setting up MDNS responder!");
    while(1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");

  Serial.println("Wi-Fi connected, browse to http://" + WiFi.localIP().toString());
}

void setupWebServer() {
    webServer.on("/", HTTP_GET, []() {
        webServer.send(200, "text/html", index_html);
    });

    webServer.on("/getStatus", HTTP_GET, []() {
            StaticJsonDocument<200> doc;
            doc["status"] = ClawMachineStatus;

            String json;
            serializeJson(doc, json);

            webServer.send(200, "application/json", json);
    });

    webServer.on("/payment", HTTP_GET, []() {
        String page = payment_html;
        webServer.send(200, "text/html", page);
    });

    webServer.on("/getPaymentData", HTTP_GET, []() {
            StaticJsonDocument<200> doc;
            doc["mqtt"] = mqttEmailmessage_Recieved;

            String json;
            serializeJson(doc, json);

            webServer.send(200, "application/json", json);
        });

    webServer.on("/setPaymentData", HTTP_POST, []() {
        mqttEmailmessage_Recieved = webServer.arg("mqtt") == "true" ? true : false;
        webServer.send(200, "text/plain", "Success");
    });

        webServer.on("/successAction", HTTP_POST, []() {
            // Update timer status to true
            mqttTimerStart = true;
            // Publish MQTT message to unlock servo
            Servo_Machine_unlock();
            webServer.send(200, "text/plain", "Success");
        });


    webServer.on("/game", HTTP_GET, []() {
        webServer.send(200, "text/html", game_html);
    });

    webServer.on("/timeoutAction", HTTP_POST, []() {
        // Update timer status to false
        mqttTimerStart = false;
        // Publish MQTT message to lock servo
        Servo_Machine_lock();
        webServer.send(200, "text/plain", "Success");
    });

  webServer.begin();
}

void mqttCallback(const char *topic, byte *message, unsigned int length) {
  String topicStr(topic);
  String messageStr((uint8_t *)message, length);
  String topicTokens[5];

  Serial.println("MQTT message received on topic: " + topicStr + ", message: " + messageStr);

  for (int i = 0; i < 5; i++) {
      int slashPos = topicStr.indexOf("/");
      topicTokens[i] = topicStr.substring(0, slashPos);
      topicStr = topicStr.substring(slashPos + 1);
      // Serial.println("Slash: " + String(slashPos) + ", Token " + String(i) + ": " + topicTokens[i]);
  }

  if (topicTokens[0] == "machine" && topicTokens[1] == "status" && messageStr == "off") {
    ClawMachineStatus = false;
    Serial.println("Message: " + messageStr);
  }
  else if(topicTokens[0] == "machine" && topicTokens[1] == "status" && messageStr == "on"){
    ClawMachineStatus = true;
    Serial.println("Message: " + messageStr);
  }
  else if(topicTokens[0] == "esp32" && topicTokens[1] == "email" && messageStr == "Payment-completed") {
    mqttEmailmessage_Recieved = true;
    Serial.println("Message: " + messageStr);
  }
  else if(topicTokens[0] == "esp32" && topicTokens[1] == "coin" && messageStr == "Payment-completed"){
    mqttEmailmessage_Recieved = true;
    Serial.println("Message: " + messageStr);
  }
  else {
    Serial.println("Invalid MQTT message received on topic: " + String(topic));
  }
}

void setupMqttClient() {
    mqttClient.setServer(APP_MQTT_SERVER, APP_MQTT_PORT);
    mqttClient.setCallback(mqttCallback);

    Serial.println("Connecting to MQTT broker " + String(APP_MQTT_SERVER) + ":" + String(APP_MQTT_PORT));

    while(!mqttClient.connected()) {
        if (mqttClient.connect("ESP32-web")) {
            // device/<MAC_ADDRESS>/led/<LED_NUMBER>/command
            String EmailTopic = "esp32/email";
            String CoinTopic = "esp32/coin";
            String StatusTopic = "machine/status";

            Serial.println("Connected to MQTT broker");
            Serial.println("Subscribing to topic: " + StatusTopic);
            Serial.println("Subscribing to topic: " + EmailTopic);
            Serial.println("Subscribing to topic: " + CoinTopic);

            mqttClient.subscribe(StatusTopic.c_str());
            mqttClient.subscribe(EmailTopic.c_str());
            mqttClient.subscribe(CoinTopic.c_str());
        }
        else {
            Serial.println("Failed to connect to MQTT broker, retrying in 5 seconds...");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    setupWiFi();
    setupWebServer();
    setupMqttClient();
    ElegantOTA.begin(&webServer);
}

void loop() {
    webServer.handleClient();
    mqttClient.loop();
    ElegantOTA.loop();
}