
#ifndef WIFI_CONFIG_H_
#define WIFI_CONFIG_H_

#include <string>
#include <map>
#include <vector>
#include <ESP8266WebServer.h>
#include <Config.h>

#ifndef WIFICONFIG_JSONBUFFER_SIZE
#define WIFICONFIG_JSONBUFFER_SIZE 512
#endif

class WifiConfig {
public:
    static constexpr long SETUP_MODE_TIMEOUT = 5000;
    static constexpr long FACTORY_RESET_TIMEOUT = 5000;
    static constexpr const char* SSID_PAREMETER_ID = "wifi_ssid";
    static constexpr const char* PASSPHRASE_PAREMETER_ID = "wifi_passphrase";

    WifiConfig();
    virtual ~WifiConfig();

    bool addConfigGroup(const ConfigGroup& group);
    bool getParameter(const std::string& id, std::string& out_value);
    bool getParameter(const std::string& id, bool& out_value);
    bool getParameter(const std::string& id, int& out_value);
    bool hasParameter(const std::string& id);

    bool begin(const char* title, FS& file_system, const char* setup_ssid, const char* setup_password, int config_pin, bool auto_connect=true);
    void end();

    bool isSetupMode();

    void setup();
    void loop();

    bool connect();

private:
    bool setup_mode_{false};
    ESP8266WebServer web_server_{80};

    FS* file_system_{nullptr};
    int config_pin_{-1};
    bool auto_connect_{true};
    bool initialized_{false};
    std::string setup_ssid_;
    std::string setup_password_;

    std::string title_;
    std::vector<ConfigGroup> config_groups_;
    std::map<std::string, ConfigEntry*> config_values_;

    bool loadConfig();
    bool updateConfig(const char* json_data);
    bool storeConfig();

    void reset();

    void handleNotFound();
    void handleRoot();
    void handleSave();
};

#endif WIFI_CONFIG_H_