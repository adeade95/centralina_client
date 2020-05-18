/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-client-server-wi-fi/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "TP-LINK_F552";
const char* password = "23395985";
// set ip address
IPAddress local_IP(192, 168, 0, 21);
IPAddress gateway(192, 168, 01, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

//ledpin falsh è il 4
#define ledpin 4

//Your IP address or domain name with URL path
const char* serverNameState = "http://192.168.0.21/state";
const char* serverNameReset = "http://192.168.0.21/reset";

/*
const char* serverNameTemp = "http://192.168.4.1/temperature";
const char* serverNameHumi = "http://192.168.4.1/humidity";
const char* serverNamePres = "http://192.168.4.1/pressure";

#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>

//#define SCREEN_WIDTH 128 // OLED display width, in pixels
//#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
/*#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

String temperature;
String humidity;
String pressure;
*/
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

void setup() {
  Serial.begin(115200);
  pinMode(ledpin, OUTPUT);    // sets the digital pin 4 led pin as output
  /*
  // Address 0x3C for 128x64, you might need to change this value (use an I2C scanner)
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  */
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");}
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
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
        
      
 
     /* temperature = httpGETRequest(serverNameTemp);
      humidity = httpGETRequest(serverNameHumi);
      pressure = httpGETRequest(serverNamePres);
      Serial.println("Temperature: " + temperature + " *C - Humidity: " + humidity + " % - Pressure: " + pressure + " hPa");
      
      display.clearDisplay();
      
      // display temperature
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.print("T: ");
      display.print(temperature);
      display.print(" ");
      display.setTextSize(1);
      display.cp437(true);
      display.write(248);
      display.setTextSize(2);
      display.print("C");
      
      // display humidity
      display.setTextSize(2);
      display.setCursor(0, 25);
      display.print("H: ");
      display.print(humidity);
      display.print(" %"); 
      
      // display pressure
      display.setTextSize(2);
      display.setCursor(0, 50);
      display.print("P:");
      display.print(pressure);
      display.setTextSize(1);
      display.setCursor(110, 56);
      display.print("hPa");
           
      display.display();
      */
      // save the last HTTP GET Request
      previousMillis = currentMillis;
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }
}
