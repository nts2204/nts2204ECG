#include<Arduino.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <SPI.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <SPIFFS.h>
#include <FS.h>


#define ST 4000 // sampling time
AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
// Replace with your network credentials
const char* ssid     = "Infinity";
const char* password = "11110000"; 
unsigned char event = 0;
char monitor =0 , stop = 0 ;
int long count=0;
int long count1=0;
double g [12];
int msg_buf;
 String d1 ;
 String d2 ;
 String d3 ;
hw_timer_t * timer = NULL;

void IRAM_ATTR isr() {
  event = 1;
  }

void timer0(){
   //timer setup
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &isr, true);
  timerAlarmWrite(timer, ST, true);
  timerAlarmEnable(timer);
}
  ////////////////////////////////////

// Callback: send homepage


// Callback: send aboutpage
char webpage[] PROGMEM = R"=====(
  <html>

<head> </head>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<script type="text/javascript" src="jquery1.js"></script>
<script type="text/javascript" src="jquery.cancolor.js"></script>
<script type="text/javascript" src="jquery.flot.js"></script>
<script type="text/javascript" src="jquery.flot.saturated.js"></script>
<script type="text/javascript" src="jquery.flot.drawSeries.js"></script>
<script type="text/javascript" src="jquery.flot.axislabels.js"></script>
<!--<link rel="stylesheet" type="text/css" href="style.css">-->
<style>
    body {
        direction: auto;
        background-color: rgba(249, 250, 249, 0.5);
        border-radius: 2px;
    }
    
    * {
        box-sizing: border-box;
    }
    
    [class*="col-s"] {
        float: left;
    }
    /*  For mobile phones:*/
    
    [class*="col-"] {
        width: 100%;
        float: left;
    }
    
    @media only screen and (min-width: 600px) {
        /* For tablets: */
        .col-s-6 {
            width: 50%;
        }
        .col-s-12 {
            width: 100%;
        }
    }
    
    @media only screen and (min-width: 768px) {
        /* For desktop: */
        .col-6 {
            width: 50%;
        }
        .col-12 {
            width: 100%;
        }
    }
    
    fieldset {
        background-color: #EAF2F8;
        width: 100%;
        padding-top: 0;
        margin-top: 105px;
        padding-bottom: 30px;
        border: none;
    }
    
    button {
        width: 90px;
        height: 45px;
        background-color: deeppink;
        font-size: 18px;
        font-weight: 700;
        font-style: oblique;
        margin-left: 5px;
        border: none;
    }
    
    button:hover {
        background-color: lime;
        border-radius: 10px;
    }
    
    h2 {
        text-align: center;
    }
    
    #setting {
        height: 50px;
        width: 98%;
        top: 0;
        background-color: skyblue;
        position: fixed;
        right: 1%;
        left: 1%;
        z-index: 4;
    }
    
    a {
        font-size: 30px;
        font-style: oblique;
    }
    
    legend {
        font-size: 35px;
        font-style: oblique;
        text-shadow: 2px blue;
    }
    
    #time,
    #temp {
        height: 50px;
        text-align: center;
        padding-top: 15px;
        background-color: skyblue;
        font-size: 20px;
        font-weight: 900;
        font-style: oblique;
    }
</style>

<script>
    setInterval(function dte() {
            window.time.innerHTML = new Date().toLocaleString("en", {
                day: "numeric",
                month: "short",
                year: "numeric",
                hour: "numeric",
                minute: "numeric",
                second: "numeric",
            });
        }

        , 1000);
</script>

<body onload="get();">
    <div id="setting">
        <div id="time" class="col-6 col-s-12 "></div>
        <div id="temp" class="col-6 col-s-12 ">Temperture</div>
    </div>

    <fieldset>
        <legend> Monitoring</legend>
        <h2>ECG Signals Chart </h2>
        <button id="Monitor">Monitor</button>
        <Button id="stop-button">Stop </Button>
        <button id="record1">10 Sec </button>
        <button id="record2">20 Sec </button>
        <button id="record3">30 Sec </button>
        <button id="record4">60 Sec </button>
        </br>
        </br>

        <div class="col-6 col-s-12 ">
            <div id="div2" style="height: 110px; width: 100%; background-color: white;  "> </div>

            <div id="div3" style="height: 110px; width: 100%; background-color: white; "> </div>

            <div id="div9" style="height: 110px; width: 100%;  background-color: white;"> </div>

            <div id="div10" style="height: 110px; width: 100%; background-color: white; "> </div>

            <div id="div11" style="height: 110px; width: 100%; background-color: white; "> </div>

            <div id="div12" style="height: 110px; width: 100%; background-color: white; "> </div>
        </div>

        <div class="col-6 col-s-12 ">
            <div id="div8" style="height: 110px; width: 100%; background-color: white; "> </div>

            <div id="div4" style="height: 110px; width: 100%; background-color: white; "> </div>

            <div id="div5" style="height: 110px; width: 100%;  background-color: white;"> </div>

            <div id="div6" style="height: 110px; width: 100%;  background-color: white;"> </div>

            <div id="div7" style="height: 110px; width: 100%; background-color: white; "> </div>

            <div id="div1" style="height: 110px; width: 100%; background-color: white; "> </div>
        </div>

    </fieldset>
    <script>
        var data1 = [];
        var op1 = {
            series: {
                lines: {
                    show: true,
                    lineWidth: 2
                },
                shadowSize: 0
            },
            yaxis: {
                axisLabel: "V6",
                axisLabelUseCanvas: true,
                axisLabelFontSizePixels: 25,
                axisLabelFontFamily: 'Verdana, Arial',
                axisLabelPadding: 0,
                showTickLabels: 'none',
            },
            xaxis: {
                showTickLabels: 'none',
                showTicks: false,
            },
            grid: {
                backgroundColor: "#000000 ",
                tickColor: "#008040"
            }
        };
        $.plot($("#div1"), [data1], op1);
        var data2 = [];
        var op2 = {
            series: {
                lines: {
                    show: true,
                    lineWidth: 2
                },
                shadowSize: 0
            },
            yaxis: {
                axisLabel: "I",
                axisLabelUseCanvas: true,
                axisLabelFontSizePixels: 25,
                axisLabelFontFamily: 'Verdana, Arial',
                axisLabelPadding: 0,
                showTickLabels: 'none'
            },
            xaxis: {
                showTickLabels: 'none',
                showTicks: false,
            },
            grid: {
                backgroundColor: "# 000000 ",
                tickColor: "#008040"
            }
        };
        $.plot($("#div2"), [data2], op2);
        var data3 = [];
        var op3 = {
            series: {
                lines: {
                    show: true,
                    lineWidth: 2
                },
                shadowSize: 0
            },
            yaxis: {
                axisLabel: "II",
                axisLabelUseCanvas: true,
                axisLabelFontSizePixels: 25,
                axisLabelFontFamily: 'Verdana, Arial',
                axisLabelPadding: 0,
                showTickLabels: 'none'
            },
            xaxis: {
                showTickLabels: 'none',
                showTicks: false,
            },
            grid: {
                backgroundColor: "# 000000 ",
                tickColor: "#008040"
            }
        };
        $.plot($("#div3"), [data3], op3);
        var data4 = [];
        var op4 = {
            series: {
                lines: {
                    show: true,
                    lineWidth: 2
                },
                shadowSize: 0
            },
            yaxis: {
                axisLabel: "V2",
                axisLabelUseCanvas: true,
                axisLabelFontSizePixels: 25,
                axisLabelFontFamily: 'Verdana, Arial',
                axisLabelPadding: 0,
                showTickLabels: 'none'
            },
            xaxis: {
                showTickLabels: 'none',
                showTicks: false,
            },
            grid: {
                backgroundColor: "# 000000 ",
                tickColor: "#008040"
            }
        };
        $.plot($("#div4"), [data4], op4);
        var data5 = [];
        var op5 = {
            series: {
                lines: {
                    show: true,
                    lineWidth: 2
                },
                shadowSize: 0
            },
            yaxis: {
                axisLabel: "V3",
                axisLabelUseCanvas: true,
                axisLabelFontSizePixels: 25,
                axisLabelFontFamily: 'Verdana, Arial',
                axisLabelPadding: 0,
                showTickLabels: 'none'
            },
            xaxis: {
                showTickLabels: 'none',
                showTicks: false,
            },
            grid: {
                backgroundColor: "# 000000 ",
                tickColor: "#008040"
            }
        };
        $.plot($("#div5"), [data5], op5);
        var data6 = [];
        var op6 = {
            series: {
                lines: {
                    show: true,
                    lineWidth: 2
                },
                shadowSize: 0
            },
            yaxis: {
                axisLabel: "V4",
                axisLabelUseCanvas: true,
                axisLabelFontSizePixels: 25,
                axisLabelFontFamily: 'Verdana, Arial',
                axisLabelPadding: 0,
                showTickLabels: 'none'
            },
            xaxis: {
                showTickLabels: 'none',
                showTicks: false,
            },
            grid: {
                backgroundColor: "# 000000 ",
                tickColor: "#008040"
            }
        };
        $.plot($("#div6"), [data6], op6);
        var data7 = [];
        var op7 = {
            series: {
                lines: {
                    show: true,
                    lineWidth: 2
                },
                shadowSize: 0
            },
            yaxis: {
                axisLabel: "V5",
                axisLabelUseCanvas: true,
                axisLabelFontSizePixels: 25,
                axisLabelFontFamily: 'Verdana, Arial',
                axisLabelPadding: 0,
                showTickLabels: 'none'
            },
            xaxis: {
                showTickLabels: 'none',
                showTicks: false,
            },
            grid: {
                backgroundColor: "# 000000 ",
                tickColor: "#008040"
            }
        };
        $.plot($("#div7"), [data7], op7);
        var data8 = [];
        var op8 = {
            series: {
                lines: {
                    show: true,
                    lineWidth: 2
                },
                shadowSize: 0
            },
            yaxis: {
                axisLabel: "V1",
                axisLabelUseCanvas: true,
                axisLabelFontSizePixels: 25,
                axisLabelFontFamily: 'Verdana, Arial',
                axisLabelPadding: 0,
                showTickLabels: 'none'
            },
            xaxis: {
                showTickLabels: 'none',
                showTicks: false,
            },
            grid: {
                backgroundColor: "#000000 ",
                tickColor: "#008040"
            }
        };
        $.plot($("#div8"), [data8], op8);
        var data9 = [];
        var op9 = {
            series: {
                lines: {
                    show: true,
                    lineWidth: 2
                },
                shadowSize: 0
            },
            yaxis: {
                axisLabel: "III",
                axisLabelUseCanvas: true,
                axisLabelFontSizePixels: 25,
                axisLabelFontFamily: 'Verdana, Arial',
                axisLabelPadding: 0,
                showTickLabels: 'none'
            },
            xaxis: {
                showTickLabels: 'none',
                showTicks: false,
            },
            grid: {
                backgroundColor: "#000000 ",
                tickColor: "#008040"
            }
        };
        $.plot($("#div9"), [data9], op9);
        var data10 = [];
        var op10 = {
            series: {
                lines: {
                    show: true,
                    lineWidth: 2
                },
                shadowSize: 0
            },
            yaxis: {
                axisLabel: "aVR",
                axisLabelUseCanvas: true,
                axisLabelFontSizePixels: 25,
                axisLabelFontFamily: 'Verdana, Arial',
                axisLabelPadding: 0,
                showTickLabels: 'none'
            },
            xaxis: {
                showTickLabels: 'none',
                showTicks: false,
            },
            grid: {
                backgroundColor: "#000000 ",
                tickColor: "#008040"
            }
        };
        $.plot($("#div10"), [data10], op10);
        var data11 = [];
        var op11 = {
            series: {
                lines: {
                    show: true,
                    lineWidth: 2
                },
                shadowSize: 0
            },
            yaxis: {
                axisLabel: "aVL",
                axisLabelUseCanvas: true,
                axisLabelFontSizePixels: 25,
                axisLabelFontFamily: 'Verdana, Arial',
                axisLabelPadding: 0,
                showTickLabels: 'none'
            },
            xaxis: {
                showTickLabels: 'none',
                showTicks: false,
            },
            grid: {
                backgroundColor: "#000000 ",
                tickColor: "#008040"
            }
        };
        $.plot($("#div11"), [data11], op11);
        var data12 = [];
        var op12 = {
            series: {
                lines: {
                    show: true,
                    lineWidth: 2
                },
                shadowSize: 0
            },
            yaxis: {
                axisLabel: "aVF",
                axisLabelUseCanvas: true,
                axisLabelFontSizePixels: 25,
                axisLabelFontFamily: 'Verdana, Arial',
                axisLabelPadding: 0,
                showTickLabels: 'none'
            },
            xaxis: {
                showTickLabels: 'none',
                showTicks: false,
            },
            grid: {
                backgroundColor: "#000000 ",
                tickColor: "#008040"
            }
        };
        $.plot($("#div12"), [data12], op12);
        /*=====================================================*/
        function tow_d(d) {
            var res = [];
            for (var i = 0; i < 1250; ++i) {
                res.push([i, d[i]])
            }
            return res;
        }


        /*----------------------------*/
        function get() {
            var count = 0;
            x1 = new Array(1250);
            x2 = new Array(1250);
            x3 = new Array(1250);
            x4 = new Array(1250);
            x5 = new Array(1250);
            x6 = new Array(1250);
            x7 = new Array(1250);
            x8 = new Array(1250);
            x9 = new Array(1250);
            x10 = new Array(1250);
            x11 = new Array(1250);
            x12 = new Array(1250);
            if ("WebSocket" in window) {
                var ws = new WebSocket("ws://' + window.location.hostname + ':81/");
                ws.onopen = function() {
                    console.log("WebSocket is open now.")
                };
                ws.onmessage = function(evt) {
                    var received_msg = evt.data;
                    var g = received_msg.split(',');
                    var w = (count % 25) * 50;
                    count++
                    x1.splice(w, 50,
                        g[0], g[12], g[24], g[36], g[48], g[60], g[72], g[84], g[96], g[108], g[120], g[132], g[144], g[156], g[168], g[180], g[192], g[204], g[216], g[228], g[240], g[252], g[264], g[276], g[288], g[300], g[312], g[324], g[336], g[348], g[360], g[372], g[384], g[396], g[408], g[420], g[432], g[444], g[456], g[468], g[480], g[492], g[504], g[516], g[528], g[540], g[552], g[564], g[576], g[588]);
                    data1 = tow_d(x1);
                    $.plot($("#div1"), [data1], op1);

                    x2.splice(w, 50,
                        g[1], g[13], g[25], g[37], g[49], g[61], g[73], g[85], g[97], g[109], g[121], g[133], g[145], g[157], g[169], g[181], g[193], g[205], g[217], g[229], g[241], g[253], g[265], g[277], g[289], g[301], g[313], g[325], g[337], g[349], g[361], g[373], g[385], g[397], g[409], g[421], g[433], g[445], g[457], g[469], g[481], g[493], g[505], g[517], g[529], g[541], g[553], g[565], g[577], g[589]);
                    data2 = tow_d(x2);
                    $.plot($("#div2"), [data2], op2);

                    x3.splice(w, 50,
                        g[2], g[14], g[26], g[38], g[50], g[62], g[74], g[86], g[98], g[110], g[122], g[134], g[146], g[158], g[170], g[182], g[194], g[206], g[218], g[230], g[242], g[254], g[266], g[278], g[290], g[302], g[314], g[326], g[338], g[350], g[362], g[374], g[386], g[398], g[410], g[422], g[434], g[446], g[458], g[470], g[482], g[494], g[506], g[518], g[530], g[542], g[554], g[566], g[578], g[590]);
                    data3 = tow_d(x3);
                    $.plot($("#div3"), [data3], op3);

                    x4.splice(w, 50,
                        g[3], g[15], g[27], g[39], g[51], g[63], g[75], g[87], g[99], g[111], g[123], g[135], g[147], g[159], g[171], g[183], g[195], g[207], g[219], g[231], g[243], g[255], g[267], g[279], g[291], g[303], g[315], g[327], g[339], g[351], g[363], g[375], g[387], g[399], g[411], g[423], g[435], g[447], g[459], g[471], g[483], g[495], g[507], g[519], g[531], g[543], g[555], g[567], g[579], g[591]);
                    data4 = tow_d(x4);
                    $.plot($("#div4"), [data4], op4);

                    x5.splice(w, 50,
                        g[4], g[16], g[28], g[40], g[52], g[64], g[76], g[88], g[100], g[112], g[124], g[136], g[148], g[160], g[172], g[184], g[196], g[208], g[220], g[232], g[244], g[256], g[268], g[280], g[292], g[304], g[316], g[328], g[340], g[352], g[364], g[376], g[388], g[400], g[412], g[424], g[436], g[448], g[460], g[472], g[484], g[496], g[508], g[520], g[532], g[544], g[556], g[568], g[580], g[592]);
                    data5 = tow_d(x5);
                    $.plot($("#div5"), [data5], op5);

                    x6.splice(w, 50,
                        g[5], g[17], g[29], g[41], g[53], g[65], g[77], g[89], g[101], g[113], g[125], g[137], g[149], g[161], g[173], g[185], g[197], g[209], g[221], g[233], g[245], g[257], g[269], g[281], g[293], g[305], g[317], g[329], g[341], g[353], g[365], g[377], g[389], g[401], g[413], g[425], g[437], g[449], g[461], g[473], g[485], g[497], g[509], g[521], g[533], g[545], g[557], g[569], g[581], g[593]);
                    data6 = tow_d(x6);
                    $.plot($("#div6"), [data6], op6);

                    x7.splice(w, 50,
                        g[6], g[18], g[30], g[42], g[54], g[66], g[78], g[90], g[102], g[114], g[126], g[138], g[150], g[162], g[174], g[186], g[198], g[210], g[222], g[234], g[246], g[258], g[270], g[282], g[294], g[306], g[318], g[330], g[342], g[354], g[366], g[378], g[390], g[402], g[414], g[426], g[438], g[450], g[462], g[474], g[486], g[498], g[510], g[522], g[534], g[546], g[558], g[570], g[582], g[594]);
                    data7 = tow_d(x7);
                    $.plot($("#div7"), [data7], op7);

                    x8.splice(w, 50,
                        g[7], g[19], g[31], g[43], g[55], g[67], g[79], g[91], g[103], g[115], g[127], g[139], g[151], g[163], g[175], g[187], g[199], g[211], g[223], g[235], g[247], g[259], g[271], g[283], g[295], g[307], g[319], g[331], g[343], g[355], g[367], g[379], g[391], g[403], g[415], g[427], g[439], g[451], g[463], g[475], g[487], g[499], g[511], g[523], g[535], g[547], g[559], g[571], g[583], g[595]);
                    data8 = tow_d(x8);
                    $.plot($("#div8"), [data8], op8);

                    x9.splice(w, 50,
                        g[8], g[20], g[32], g[44], g[56], g[68], g[80], g[92], g[104], g[116], g[128], g[140], g[152], g[164], g[176], g[188], g[200], g[212], g[224], g[236], g[248], g[260], g[272], g[284], g[296], g[308], g[320], g[332], g[344], g[356], g[368], g[380], g[392], g[404], g[416], g[428], g[440], g[452], g[464], g[476], g[488], g[500], g[512], g[524], g[536], g[548], g[560], g[572], g[584], g[596]);
                    data9 = tow_d(x9);
                    $.plot($("#div9"), [data9], op9);

                    x10.splice(w, 50,
                        g[9], g[21], g[33], g[45], g[57], g[69], g[81], g[93], g[105], g[117], g[129], g[141], g[153], g[165], g[177], g[189], g[201], g[213], g[225], g[237], g[249], g[261], g[273], g[285], g[297], g[309], g[321], g[333], g[345], g[357], g[369], g[381], g[393], g[405], g[417], g[429], g[441], g[453], g[465], g[477], g[489], g[501], g[513], g[525], g[537], g[549], g[561], g[573], g[585], g[597]);
                    data10 = tow_d(x10);
                    $.plot($("#div10"), [data10], op10);

                    x11.splice(w, 50,
                        g[10], g[22], g[34], g[46], g[58], g[70], g[82], g[94], g[106], g[118], g[130], g[142], g[154], g[166], g[178], g[190], g[202], g[214], g[226], g[238], g[250], g[262], g[274], g[286], g[298], g[310], g[322], g[334], g[346], g[358], g[370], g[382], g[394], g[406], g[418], g[430], g[442], g[454], g[466], g[478], g[490], g[502], g[514], g[526], g[538], g[550], g[562], g[574], g[586], g[598]);
                    data11 = tow_d(x11);
                    $.plot($("#div11"), [data11], op11);

                    x12.splice(w, 50,
                        g[11], g[23], g[35], g[47], g[59], g[71], g[83], g[95], g[107], g[119], g[131], g[143], g[155], g[167], g[179], g[191], g[203], g[215], g[227], g[239], g[251], g[263], g[275], g[287], g[299], g[311], g[323], g[335], g[347], g[359], g[371], g[383], g[395], g[407], g[419], g[431], g[443], g[455], g[467], g[479], g[491], g[503], g[515], g[527], g[539], g[551], g[563], g[575], g[587], g[599]);
                    data12 = tow_d(x12);
                    $.plot($("#div12"), [data12], op12);

                };
                ws.onclose = function(evt) {
                    console.log("Connection is closed...");
                };

                var monitor = document.getElementById("Monitor");

                monitor.onclick = function() {
                    if (ws.readyState === WebSocket.OPEN) {
                        var start1 = 1;
                        ws.send(start1);
                    }
                }
                var buttonStop = document.getElementById("stop-button");

                buttonStop.onclick = function() {
                    if (ws.readyState === WebSocket.OPEN) {
                        count = 0;
                        var stop1 = 0;
                        ws.send(stop1); /* ws.close();*/
                    }
                }
                var rect1 = document.getElementById("record1");
                rect1.onclick = function() {

                    if (ws.readyState === WebSocket.OPEN) {
                        var y1 = 2;
                        ws.send(y1);
                    }
                }
                var rect2 = document.getElementById("record2");
                rect2.onclick = function() {

                    if (ws.readyState === WebSocket.OPEN) {
                        var y2 = 3;
                        ws.send(y2);
                    }
                }
                var rect3 = document.getElementById("record3");
                rect3.onclick = function() {

                    if (ws.readyState === WebSocket.OPEN) {
                        var y3 = 4;
                        ws.send(y3);
                    }
                }
                var rect4 = document.getElementById("record4");
                rect4.onclick = function() {

                    if (ws.readyState === WebSocket.OPEN) {
                        var x4 = 5;
                        ws.send(x4);
                    }
                }

            } else {

                alert("WebSocket NOT supported by your Browser!");
            }
        }
    </script>
    </br>
    <a href=about.html target="blanck">About</a>
    </br>
    </br>
</body>

</html>
)=====";
  ////////////////////////////////////



void webSocketEvent(uint8_t num,WStype_t type,uint8_t *payload,size_t length)
{

  // Figure out the type of WebSocket event
  switch (type)
  {
      
        case WStype_DISCONNECTED:   
        Serial.printf("[%u] Disconnected!\n",num);   
            break;
        case WStype_CONNECTED: 
            {
              IPAddress ip = webSocket.remoteIP(num); 
               Serial.printf("[%u] Connection from ", num);
               Serial.println(ip.toString());  
            }
            break;
        
        case WStype_TEXT:
            {
                // Print out raw message
              Serial.print(" Received text: ");
               for (int i = 0; i < length; i++)
              {
               msg_buf = (int)payload[i];
              } 

              switch (msg_buf)
               {
                case 49:
                   monitor = 1;
                   stop = 0;
                  timer0();
                 break;
                 case 48:
                      stop = 1;
                break;
               default:
                break;
                }

            }   
    Serial.println(msg_buf);
    Serial.println();
            break; 
               default:
                break; 
             
    }
}


void setup() {
//  timer0();

  Serial.begin(115200);

  ////////////////////////////////////
  WiFi.softAP(ssid,password);
  IPAddress ip = WiFi.softAPIP();
  Serial.print(" IP address: ");
  Serial.println(ip);
  Serial.println("Welcome");

  ////////////////////////////////////////

    // On HTTP request for root, provide index.html and javascript files
  /*server.on("/", HTTP_GET, homepage);
  server.on("/about.html", HTTP_GET, aboutpage);
  server.on("/jquery.cancolor.js", HTTP_GET, concolor);
  server.on("/jquery.flot.axislabels.js", HTTP_GET, axislabels);
  server.on("/jquery.flot.drawSeries.js", HTTP_GET, drawSeries);
  server.on("/jquery.flot.js", HTTP_GET, flotjs);
  server.on("/jquery.flot.saturated.js", HTTP_GET, saturated);
  server.on("/jquery1.js", HTTP_GET, jquery1);*/

  //////////////////////////////////////////////////////////

  server.begin();         // start server
  webSocket.begin();     // start webSocket server
  webSocket.onEvent(webSocketEvent); // Callback to handle for websocket events

   }

void loop() {
  
  // Look for and handle WebSocket data
  webSocket.loop();
//////////////////////////////////
  
    if ((event == 1) && (monitor == 1))
  
    // if (event == 1) 
  {
     count++;
    for (int i = 0; i < 12; i++)
    {
    // g [i] = random(99.00);
      g [i] =i*3.25;
    }
    
    for (int i = 0; i < 11; i++)
    {
      d1 += String(g [i], 2) + ",";
    }
    d1 += String(g [11], 2);
    //Serial.println(d1);
    d2 += d1 +",";  //d2 is an array of  elements

if(count % 50 == 0)
{
   // Serial.println(d2); 
 //d3=d2+count1;
  webSocket.broadcastTXT(d2);    // send data to all connected clients pio run -t uploadfs
   d2 = "";
   count1++ ;
  
}

  d1="";
  event = 0;
}


if(stop == 1){
      count=0;
      count1=0;
      monitor=0;
      stop = 0;
      timerStop(timer);
      timerAlarmDisable(timer);

}

}
