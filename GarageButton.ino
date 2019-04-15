// GarageButton.ino - Elizabeth Greene 2019
// Based on samplecode from https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WebServer/examples/HelloServer/HelloServer.ino

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "wifiSSID"
#define STAPSK  "wifiPassword"
#endif

#define GPIORELAY 0

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

void handleRoot() {
  digitalWrite(LED_BUILTIN, 1);
  server.send(200, "text/html","<!DOCTYPE html><html><head><title>Garage Door Button</title><style> div { display: block; text-align: center;}</style></head><body><div><form action=\"\\push\" method=\"post\"><input type=\"submit\" value=\"Push it\"/></form></div></body></html>");
}

void handlePush() {
  server.send(200, "text/html", "<!DOCTYPE html><html><head><title>Garage Door Button</title><style>div { display: block; text-align: center;}</style>  </head>  <body><div>    Button has been pushed.  </div></body></html>");
  digitalWrite(GPIORELAY, HIGH);
  delay(2000);                      // Wait for a second
  digitalWrite(GPIORELAY, LOW);
}

void handleNotFound() {
  digitalWrite(LED_BUILTIN, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(LED_BUILTIN, 0);
}

void setup(void) {
  pinMode(GPIORELAY, OUTPUT);   // Initialize the GPIO as the output pin for the relay
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the built-in LED as an output

  digitalWrite(LED_BUILTIN, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/push", []() {
    // server.send(200, "text/plain", "this works as well");
    handlePush();
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();


}
