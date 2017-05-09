
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

MDNSResponder mdns;

ESP8266WebServer server(80);

void setup(void){
  Serial.begin(115200);
  serverInit();
  Serial.println("http server has started");
}
void loop() {
  server.handleClient();
  delay(1);
}

void serverInit() {
  server.onNotFound(handleNotFound);
  server.on("/", handleRoot);
  server.on("/stop", stopCommand);
  server.on("/left", turnLeftCommand);
  server.on("/right", turnRightCommand);
  server.on("/forward", moveForwardCommand);
  server.on("/backward", moveBackwardCommand);
  server.begin();
}
void handleRoot() {
  server.send(200, "text/plain", "OK");
}

void handleNotFound() {
  server.send(404, "text/plain", "not found");
}

void turnLeftCommand() {
  Serial.write('l');
  server.send(200, "text/plain", "OK");
}

void turnRightCommand() {
  Serial.write('r');
  server.send(200, "text/plain", "OK");
}

void moveForwardCommand() {
  Serial.write('f');
  server.send(200, "text/plain", "OK");
}

void moveBackwardCommand() {
  Serial.write('b');
  server.send(200, "text/plain", "OK");
}

void stopCommand(){
  Serial.write('!');
  server.send(200, "text/plain", "OK");
}

