# WifiConfig

The WifiConfig library allows you to configure the Wifi SSID and passphrase for your ESP8266 devices on the fly. It introduces a config mode which you can enter by pressing a button within 5 seconds after the device is turned on. In this mode the library configures the ESP8266 as access point with a setup SSID and passowrd. If you join the ESP's wifi network and go to http://192.168.0.1 it allows you to enter SSID and passphrase for the wifi network you want the device to join. The data is saved in a json file in the ESP's flash memory. Every time your device boots in normal mode, it will use those to sign into your wifi network.

WifiConfig also allows you to add custom configuration options. Configuration options you register via addConfigGroup() will show up on the configuration page and the library will store the data you enter. You can retrieve the information via getParameter().

```cpp
#include <ESP8266WiFi.h>
#include <WifiConfig.h>
#include <LittleFS.h>

WifiConfig wifi_config;

const int config_pin = 12;

void setup() {
  Serial.begin(115200);
  delay(100);

  LittleFS.begin();
  wifi_config.begin("Title", LittleFS, "setup_SSID", "setup_Password", config_pin, true);
  wifi_config.setup();

  // if in config mode, don't setup the rest
  if (wifi_config.isSetupMode()) {
    return; // end setup here
  }

  // normal setup code
}

void loop() {
  // if in config mode, only run config loop
  if (wifi_config.isSetupMode()) {
    wifi_config.loop();
    return;
  }

  // normal loop code
}
```
