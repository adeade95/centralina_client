//DOVREMO fsre un megs bettore char dove inserire indirizzi IP che andremo a integrare con i comandi
//e un mega vettore dove gestire risposte e stati

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
unsigned long previousMillis = 0;
const long interval = 5000; 


//ledpin falsh è il 4
#define ledpin 4

//Your IP address or domain name with URL path //va bene anche se non sono const(vedo che compils non provsto DA CANCELLARE APPENA HAI FINITO LE PROVE
//char* serverNameState = "http://192.168.0.21/state";  //è un array non farti ingannare da come è stato dichiarato
//char* serverNameReset = "http://192.168.0.21/reset";  //è un array non farti ingannare da come è stato dichiarato


//spiegazione array https://www.html.it/pag/15507/cenni-sugli-array-multidimensionali/ ora proviano a mettere in un array multidimensionale tutti gli indirizzi
//char serveraddress [2][20]={'http://192.168.0.21/http://192.168.0.20/'};  //così sembra compilare, ora avremo un indirizzo ogni 20 celle
//forse conviene fare l'array con indirizzo e comando così eviti di aggiungerlo dopo, ma prima devi verificare che tu sappia richiamare la parte di array corretta
//numero totale schede da interrogare
const unsigned int ntotbsensor = 3;
//indirizzi schede
char* indirizzilista [ntotbsensor] ={"http://192.168.0.22/state", "http://192.168.0.21/state", "http://192.168.0.20/state"}; //sembra andare ma non provato nella funzione di ade ask
// numero pin ingresso digitale schede
const unsigned int ntotbinputsensor = 12;

// array in cui salviamo gli stati di riposo a gruppi di
char rip [ntotbinputsensor*ntotbsensor];
//array in cui salviamo gli stati trigger che interrogheremo
char triggerint [ntotbinputsensor*ntotbsensor]; 
//stringa in cui abbiamo il trigger a 0 che non ha rilevato niente
//String triggerok; 

//funzione per copiare elementi char da un array a un altra
//restituisce il numero della casella in cui ha notato l'ultima differenza
int copyarraychar(int nelements, char* arrayorig, char* arraydest){
  int diverso= -1;
  for(int i=0;i< nelements; i++){
    if(arraydest[i]!=arrayorig[i])
      arraydest[i]=arrayorig[i];
      diverso = i;
  }
  return diverso;
}

String httpGETRequest(const char* serverName) { //funzione per chiedere info alle altre schede, passare come parametro l'indirizzo della scheda
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
    String payload = "ERRORE"; 
  }
  // Free resources
  http.end();

  return payload;
}

//funzione per interrogare lo stato trigger o altre cose delle schede
//al momento settare   const unsigned int convlenght = 12; //numero di caratteri da convertire da string a char
//parametri: totale schede a cui chiedere, vettore indirizzi da chiedere, vettore dove salvo risposte, lunghezza dei caratteri da acquisire
int askinf(const unsigned int totask, char** addressask, char* answer, const unsigned int convlenght){
  int diverso=-1;  //ci serve per sapere se notiamo stati trigger diversi rispetto a quelli letti in passato
  Serial.print("Interrogo ");
  Serial.print(totask);
  Serial.println(" schede");
  for(int i=0; i<totask; i++){  //ciclo per interrogare tutte le schede
    Serial.print("Interrogo ");
    Serial.println(addressask[i]);
    char bufconv [convlenght];  //buffer temporaneo dove salverò i dati in char della string buf
    String buf= httpGETRequest(addressask[i]);//acquisisco dato da scheda server
    Serial.print(" ricevuto ");
    Serial.println(buf);
    if(buf=="ERRORE") //comunichiamo su seriale se ci sono problemi di comunicazione con la scheda sensori 
      Serial.println("Errori di comunicazione con la scheda sensori");
    buf.toCharArray(bufconv, convlenght); //conversione da string a char di alcuni caatteri
    for(int e=0; e<convlenght;e++){
      if(answer[i*convlenght+e] != bufconv[e]){
      answer[i*convlenght+e]= bufconv[e]; //salvo il dato nel mega array
      diverso =1;  //quindi abbiamo notato stati diversi e lo notifichiamo
      }
    }
  }
  return diverso;
}


void ConnectToWiFi()  //funzione per connettersi al wifi
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
      Serial.println("WIFI connesso");
      int returnaskinf = askinf(ntotbsensor, &indirizzilista[0], &rip[0], ntotbinputsensor);
      
     /* String inputstate= httpGETRequest(indirizzilista[0]);//acquisisco dato da scheda server QUESTO PEZZO SARò DA CANCELLARE
      Serial.print("Interrogazione di ");
      Serial.println(indirizzilista[2]);
      Serial.println(indirizzilista[1]);
      Serial.println(indirizzilista[0]);
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
      if(inputstate!="0" && inputstate!="1" || inputstate=="ERRORE") // probabilmente in futuro lasceremo solo errore
        Serial.println("possibili problemi di connessione ocn la scheda");          //non ho capito perché questo if dà errore*/



      // save the last HTTP GET Request
      previousMillis = currentMillis;
    }
    else {
      Serial.println("WiFi Disconnected");
    }
  }
}
