#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>

#include <ArduinoJson.h>

#include <WebSocketsClient.h>
#include <SocketIOclient.h>
WiFiMulti WiFiMulti;
SocketIOclient socketIO;
const char* ssid     = "INFINITUMF89F_2.4"; //"INFINITUMC2A2_2.4"
const char* password = "MM3Xx2D4Qc";        //"Dus2qzqyFC"
const char* server_host = " 192.168.1.7"; //Enter server adress
const uint16_t server_port = 3000; // Enter server port
int payload_entero;
#define VARIABLE_LABEL "sensor" // Assing the variable label
#define ONBOARD_LED 2
#define USE_SERIAL Serial
#define SENSOR A0 // Set the A0 as SENSOR
const char *hostname = "nts2204";
const char *eventname = "new_message";
char str_sensor[10];
char IP_end[4];
char camHostname[50];
IPAddress camIP;
 
/****************************************
 * Main Functions
 ****************************************/
 void socketIOEvent(socketIOmessageType_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case sIOtype_DISCONNECT:
    USE_SERIAL.printf("[IOc] Disconnected!\n");
    break;
  case sIOtype_CONNECT:
    USE_SERIAL.printf("[IOc] Connected to url: %s\n", payload);

    // join default namespace (no auto join in Socket.IO V3)
    socketIO.send(sIOtype_CONNECT, "/");
    break;
  case sIOtype_EVENT:
  {
    char *sptr = NULL;
    int id = strtol((char *)payload, &sptr, 10);
    USE_SERIAL.printf("[IOc] get event: %s id: %d\n", payload, id);
    if (id)
    {
      payload = (uint8_t *)sptr;
    }
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload, length);
    if (error)
    {
      USE_SERIAL.print(F("deserializeJson() failed: "));
      USE_SERIAL.println(error.c_str());
      return;
    }

    String eventName = doc[0];
    USE_SERIAL.printf("[IOc] event name: %s\n", eventName.c_str());

    // Message Includes a ID for a ACK (callback)
    if (id)
    {
      // creat JSON message for Socket.IO (ack)
      DynamicJsonDocument docOut(1024);
      JsonArray array = docOut.to<JsonArray>();

      // add payload (parameters) for the ack (callback function)
      JsonObject param1 = array.createNestedObject();
      param1["now"] = millis();

      // JSON to String (serializion)
      String output;
      output += id;
      serializeJson(docOut, output);

      // Send event
      socketIO.send(sIOtype_ACK, output);
    }

  }
  break;
  case sIOtype_ACK:
    USE_SERIAL.printf("[IOc] get ack: %u\n", length);
    break;
  case sIOtype_ERROR:
    USE_SERIAL.printf("[IOc] get error: %u\n", length);
    break;
  case sIOtype_BINARY_EVENT:
    USE_SERIAL.printf("[IOc] get binary: %u\n", length);
    break;
  case sIOtype_BINARY_ACK:
    USE_SERIAL.printf("[IOc] get binary ack: %u\n", length);
    break;
  }
}
void setup()
{
  pinMode(SENSOR, INPUT); 
  // USE_SERIAL.begin(921600);
  USE_SERIAL.begin(115200);

  // Serial.setDebugOutput(true);
  //USE_SERIAL.setDebugOutput(true);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--)
  {
    USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
    USE_SERIAL.flush();
    delay(1000);
  }

  //WiFiMulti.addAP("SSID", "passpasspass");
  WiFiMulti.addAP(ssid, password);
  

  // WiFi.disconnect();
  while (WiFiMulti.run() != WL_CONNECTED)
  {
    delay(100);
  }

  String ip = WiFi.localIP().toString();
  USE_SERIAL.printf("[SETUP] WiFi Connected %s\n", ip.c_str());

  /* ADDED 2022.08.13.
   * 
   */
  // Print ESP32 Local IP Address
  camIP = WiFi.localIP();
  Serial.println(camIP);

  // add last ip segment to hostname
  const String tmp = String(camIP[3]);
  strcpy(camHostname, hostname);
  strcpy(IP_end, tmp.c_str());
  //    Serial.println("lastIP:");
  //    Serial.print(tmp);
  strcat(camHostname, "-");
  strcat(camHostname, IP_end);
  //    Serial.println("newHostname:");
  //    Serial.print(camHostname);

  //setupCamera();
  // server address, port and URL
  // socketIO.begin("10.11.100.100", 8880, "/socket.io/?EIO=4");
  socketIO.begin(server_host, server_port, "/socket.io/?EIO=4");

  // event handler
  socketIO.onEvent(socketIOEvent);
}

unsigned long messageTimestamp = 0;
void loop()
{
  socketIO.loop();

  uint64_t now = millis();

  if (now - messageTimestamp > 2000)
  {

    messageTimestamp = now;
    float sensor = analogRead(SENSOR);
    dtostrf(sensor, 4, 2, str_sensor);
    Serial.println(str_sensor); 
    // create JSON message for Socket.IO (event)
    DynamicJsonDocument doc(200);
    JsonArray array = doc.to<JsonArray>();
    array.add(eventname);
    JsonObject param1 = array.createNestedObject();
    //param1["hostname"] = camHostname;
    param1["sensor"] = sensor;
 
    String output;
    serializeJson(doc, output);
    
    socketIO.sendEVENT(output);

    // Print JSON for debugging
    USE_SERIAL.println(output);
  }
  delay(5);
}
