#define DEVICE_ID 2 change
#define DEVICE_NAME "" fill
#define TOKEN ""fill


//THIS is some example how to make remoteMe works with IotWebConf

#include <RemoteMe.h>
#include <RemoteMeSocketConnector.h>
#include <ESP8266WiFi.h>

#include <DNSServer.h>
#include <IotWebConf.h>

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "testThing";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "xxxxxxxx";

DNSServer dnsServer;
WebServer server(80);

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword);



RemoteMe& remoteMe = RemoteMe::getInstance(TOKEN, DEVICE_ID);



boolean connected=false;
void wifiConnected(){
  Serial.println("wifi connected");
   
    remoteMe.setConnector(new RemoteMeSocketConnector());

    remoteMe.sendRegisterDeviceMessage(DEVICE_NAME);
connected=true;
  
}

void setup() 
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting up...");

  // -- Initializing the configuration.
   iotWebConf.setWifiConnectionCallback(&wifiConnected);
     iotWebConf.setConfigPin(D1);
  iotWebConf.init();

  // -- Set up required URL handlers on the web server.
  server.on("/", handleRoot);
  server.on("/config", []{ iotWebConf.handleConfig(); });
  server.onNotFound([](){ iotWebConf.handleNotFound(); });

  Serial.println("Ready.");


  
  
    
}

void loop() 
{
  // -- doLoop should be called as frequently as possible.
  iotWebConf.doLoop();

  if (connected){
    remoteMe.loop();
  }
}

/**
 * Handle web requests to "/" path.
 */
void handleRoot()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>IotWebConf 01 Minimal</title></head><body>Hello world!";
  s += "Go to <a href='config'>configure page</a> to change settings.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}