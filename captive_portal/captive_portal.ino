# include "active_mode.h"
# include "setting_mode.h"

TaskHandle_t taskCore0Handle=NULL;
TaskHandle_t taskCore1Handle=NULL;



void setup() {
  Serial.begin(115200);
  Serial.println("START SETUP");
  
  
  Serial.println("STARTING CORE...");
  xTaskCreatePinnedToCore(
    loopCore0,          
    "loopCore0",        
    8192,               
    NULL,               
    1,                  
    &taskCore0Handle,   
    0                    
  );

  // Creazione task su core 1
  xTaskCreatePinnedToCore(
    loopCore1,
    "loopCore1",
    2048,
    NULL,
    1,
    &taskCore1Handle,
    1                   // core 1
  );

  Serial.println("DONE");
}


void loop() {}




