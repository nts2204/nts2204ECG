#include <SocketIoClient.h>
#include <WiFi.h>

const char *ssid = "nts2204";
const char *password = "nts2204!";
const char *host = "172.20.10.2";
const int port = 5000;

SocketIoClient webSocket;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  webSocket.on("message", [](const char *payload, size_t length) {
    Serial.println("Message received: ");
    Serial.write(payload, length);
    Serial.println();
  });

  webSocket.begin(host, port);
}
unsigned long oldTime = 0;
unsigned long _time = 50;

void loop() {
  webSocket.loop();

  if (millis() - oldTime > _time) {
    char *message = "\"day la code\"";
    int b = random(0, 11);
    webSocket.emit("status", String(analogRead(A0)).c_str());
    oldTime = millis();
  }
}