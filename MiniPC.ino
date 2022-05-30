#include <HTTPClient.h>
#include <M5StickCPlus.h>
#include "WiFi.h"

const char* ssid     = "JH-Network";
const char* password = "isso naoexiste";

HTTPClient http;

IPAddress local_IP(10, 0, 0, 99);
IPAddress gateway(10, 13, 7, 100);
IPAddress subnet(255, 0, 0, 0);
IPAddress primaryDNS(10, 0, 0, 101); //optional
WiFiServer server(80);

void setup()
{
  M5.begin();
  //M5.Lcd.setRotation(3);  //Rotate the screen.  ????
  pinMode(10, OUTPUT);
  pinMode(2, OUTPUT);
  //M5.Lcd.setCursor(0, 0);
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS)) {
    M5.Lcd.print("STA Failed to configure");
  }

  M5.Lcd.print("Connecting to ");
  M5.Lcd.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    M5.Lcd.print(".");
    M5.Beep.tone(4000);
    delay(50);
    M5.Beep.mute();
  }

  M5.Lcd.println("WiFi connected!");

  M5.Lcd.print("IP address: ");
  M5.Lcd.println(WiFi.localIP());
  M5.Lcd.print("ESP Mac Address: ");
  M5.Lcd.println(WiFi.macAddress());

  server.begin();
}


void loop() {
  WiFiClient serverClient = server.available();
  if (serverClient) {

    char prevprev;
    char prev;
    String currentLine = "";  
    
    while (serverClient.connected()) {
      if (serverClient.available()) {
        char c = serverClient.read();
        if (prevprev == '\n' && prev == '\r' && c == '\n') {
          break;
        }
        currentLine += c;
         // Check to see if the serverClient request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H?")) {
          digitalWrite(10, HIGH);               // GET /H turns the LED on
          serverClient.println("HTTP/1.1 200 OK");
          serverClient.println("Content-Type: text/html");
          serverClient.println("Connection: close");
          serverClient.println();
          
          serverClient.println("");
          M5.Beep.tone(100);
          delay(100);
          M5.Beep.mute();
        }else if (currentLine.endsWith("GET /L?")) {
          digitalWrite(10, LOW);
          serverClient.println("HTTP/1.1 200 OK");
          serverClient.println("Content-Type: text/html");
          serverClient.println("Connection: close");
          serverClient.println();

          serverClient.println("");               // GET /L turns the LED off
          M5.Beep.tone(10000);
          delay(100);
          M5.Beep.mute();
        }else if (currentLine.endsWith("GET /HEALTH?")) {
          serverClient.println("HTTP/1.1 200 OK");
          serverClient.println("Content-Type: text/html");
          serverClient.println("Connection: close");
          serverClient.println();

          serverClient.println("");               // GET /HEALTH  healthcheck
          M5.Beep.tone(2000);
          delay(400);
          M5.Beep.mute();
        } 
        prevprev = prev;
        prev = c;
      }
    }

    serverClient.stop();
  }
}
