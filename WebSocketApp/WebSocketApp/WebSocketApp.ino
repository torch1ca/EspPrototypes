//==================================
//ESP32 WebSocket Server: Toggle LED
//by: Ulas Dikme
//==================================
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

const char* ssid = "SM-A526W6414";
const char* password = "mnsw3939";
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);
#include "html.h"

#define LED 2
int ledEvent = -1;
bool ledIsOn = false;
int ledBreakCounter = 0;
unsigned long timeSave = 0;
unsigned long animDuration = 1000;

//===========================================================

//handle function: send webpage to client
void webpage()
{
  server.send(200,"text/html", webpageCode);
}

//function process event: new data received from client
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t welength)
{
  if (ledEvent == -1){
    ledEvent = *payload;
    ledBreakCounter = 0;
    timeSave = millis();
  }
  // String payloadString = (const char *)payload;
  // Serial.print("payloadString= ");
  // Serial.println(payloadString);

  // if(type == WStype_TEXT) //receive text from client
  // {
  //   byte separator=payloadString.indexOf('=');
  //   String var = payloadString.substring(0,separator);
  //   Serial.print("var= ");
  //   Serial.println(var);
  //   String val = payloadString.substring(separator+1);
  //   Serial.print("val= ");
  //   Serial.println(val);
  //   Serial.println(" ");

  //   if(var == "LEDonoff")
  //   {
  //     LEDonoff = false;
  //     if(val == "ON") LEDonoff = true;
  //   }
  // }
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {Serial.print("."); delay(500);}
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", webpage);
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop()
{
  webSocket.loop();
  server.handleClient();

  if (ledEvent > -1){
    int breaks = ledEvent + ledEvent + 1;
    unsigned long time = millis() - timeSave;
    float timeFactor = (double)time / (double)animDuration;
    float breakInterval = timeFactor / breaks;

    if (timeFactor > breakInterval * ledBreakCounter){

      bool isOdd = ledBreakCounter % 2; 
      /*
      if 0 and 0 (0), light up
      if 0 and 1 (1), good :)
      if 1 and 0 (2), good :)
      if 1 and 1 (3), turn off
      */
      if (ledIsOn == isOdd) {
        ledIsOn = !ledIsOn;
        digitalWrite(LED, ledIsOn);
        ledBreakCounter++;
        if (ledBreakCounter > breaks){
          ledEvent = -1;
        }
      }
    }
  }
  
  // if(LEDonoff == false) 
  //   digitalWrite(LED, LOW);
  // else 
  //   digitalWrite(LED, HIGH);
  
  // String LEDstatus = "OFF";
  // if(LEDonoff == true) LEDstatus = "ON";
  // JSONtxt = "{\"LEDonoff\":\""+LEDstatus+"\"}";
  // webSocket.broadcastTXT(JSONtxt);
}

