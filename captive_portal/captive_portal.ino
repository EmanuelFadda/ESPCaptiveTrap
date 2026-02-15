void setup() {
  Serial.begin(115200);
  Serial.println("START");
  setupBLE("ESPSERVER");
  

  commandHandler();
  //setupESPLocalNetwork(ssid_esp,psw_esp,IPAddress(8,8,8,8), IPAddress(8,8,8,8), IPAddress(255,255,255,0));
  Serial.println("Done");
}

void loop() {
  //dnsServer.processNextRequest();
  delay(20);
}
