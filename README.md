# ESPCaptiveTrap
Emulation of fake captive portal on ESP


## What the project is about?
The first goal of this project is emulating how attackers can steal your data using fake public (and private why not) WIFI connections with an ESP32.<br>

> [!WARNING]  
> This project is ONLY for educational purpose! 

### How this type of attack works?
When the ESP32 is turned on, it starts setting a WIFI connection in AP mode (Access point) with a fake captive portal. The device, which is connected to the ESP WIFI will detect automatically the captive portal and the application/device will redirect to the malicious form. 

The ESP32 has a default configuration stored in memory but the attacker can personalize the attack with different configuration through the multiplatform Flask application (work in progess)  
## What is a captive portal:


### How autocaptive portal works






