

//impostazione router casa nuova piano superiore
const char* ssid = "TP-LINK_FF52";
const char* password = "23395985";
#include "WiFi.h" // ESP32 WiFi include
#include "HTTPClient.h"
#include "ESPAsyncWebServer.h"// pre creare server
//non funziona l'impostazione ip
// set ip address
IPAddress local_IP(192, 168, 0, 11);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional


//ledpin falsh è il 4
#define ledpin 4

//Your IP address or domain name with URL path
const char* serverNameState = "http://192.168.0.21/state";
const char* serverNameReset = "http://192.168.0.21/reset";

unsigned long previousMillis = 0;
const long interval = 5000; 


String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "--"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

void ConnectToWiFi()
{
 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to "); Serial.println(ssid);
 
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(500);
 
    if ((++i % 16) == 0)
    {
      Serial.println(F(" still trying to connect"));
    }
  }
 
  Serial.print(F("Connected. My IP address is: "));
  Serial.println(WiFi.localIP());

    Serial.println("risetto indirizzo ip");
WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS);

    Serial.print(F("Connected. My IP address is: "));
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  Serial.println("setto il pin di output del LED ");
  pinMode(ledpin, OUTPUT);    // sets the digital pin 4 led pin as output
  Serial.println("settato il pin di output del LED ");
  ConnectToWiFi();
}


void loop() {
  
  unsigned long currentMillis = millis();
  
  if(currentMillis - previousMillis >= interval) {
     // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED ){ 
      String inputstate= httpGETRequest(serverNameState);//acquisisco dato da scheda server
      Serial.print("Dato ricevuto ");
      Serial.println(inputstate);
      if(inputstate=="1"){
        digitalWrite(ledpin, HIGH);
        Serial.println("connesso pulsante premuto");
        }
      if(inputstate=="0"){        
        digitalWrite(ledpin, LOW);
        Serial.println("connesso pulsante non premuto");     
      }
      if(inputstate!="0" && inputstate!="1")
        Serial.println("possibili problemi di connessione ocn la scheda");          

        /*
      switch (inputstate)
      case "1":
        digitalWrite(ledpin, HIGH);
        Serial.println("connesso pulsante premuto");
        break;
      case "0":
        digitalWrite(ledpin, LOW);
        Serial.println("connesso pulsante non premuto");
       default:
        Serial.println("possibili problemi di connessione ocn la scheda");
        }*/
        inputstate="2";//resettiamao lo stato
        
      

      // save the last HTTP GET Request
      previousMillis = currentMillis;
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }
}
