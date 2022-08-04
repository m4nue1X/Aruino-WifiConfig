#include "WifiConfig.h"

#define ARDUINOJSON_ENABLE_ARDUINO_STREAM 1
#define ARDUINOJSON_ENABLE_STD_STRING 1

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <sstream>
#include <algorithm>
#include <HtmlElements.h>

WifiConfig::WifiConfig() { }

WifiConfig::~WifiConfig() { }

bool WifiConfig::addConfigGroup(const ConfigGroup& group) {
    if(!initialized_) {
       return false; 
    }

    // Add the group at first since we need the poitners to the entries
    config_groups_.push_back(group);
    std::vector<std::pair<std::string, ConfigEntry*>> entries;
    entries.reserve(group.entries.size());
    for(ConfigEntry& entry : config_groups_.back().entries) {
        if(config_values_.find(entry.id) != config_values_.end()) {
            // another config entry with this id already exists -> can't add config gorup
            config_groups_.pop_back();
            return false;
        }
        entries.push_back(std::make_pair(entry.id, &entry));
    }
    config_values_.insert(entries.begin(), entries.end());
}

bool WifiConfig::getParameter(const std::string& id, std::string& out_value) {
    auto it = config_values_.find(id);
    if(it != config_values_.end()) {
        out_value = it->second->value;
        return true;
    }
    return false;
}

bool WifiConfig::getParameter(const std::string& id, bool& out_value) {
    std::string value;
    if(getParameter(id, value)) {
        std::transform(value.cbegin(), value.cend(), value.begin(), [](unsigned char c) { return std::tolower(c); });
        out_value = !(value == "false" || value == "0" || value == "no" || value == "off");
        return true;
    }
    return false;
}

bool WifiConfig::getParameter(const std::string& id, int& out_value) {
    std::string value;
    if(getParameter(id, value)) {
        out_value = std::atoi(value.c_str());
        return true;
    }
    return false;
}

bool WifiConfig::hasParameter(const std::string& id) {
    return (config_values_.find(id) != config_values_.end());
}

bool WifiConfig::begin(const char* title, FS& file_system, const char* setup_ssid, const char* setup_password, int config_pin, bool auto_connect) {
    if(initialized_) {
       return false; 
    }

    reset();
    title_ = title;
    config_pin_ = config_pin;
    auto_connect_ = auto_connect;
    file_system_ = &file_system;
    setup_ssid_ = setup_ssid;
    setup_password_ = setup_password;
    initialized_ = true;
    addConfigGroup({ "Wifi Config", { { ConfigEntryType::kText, SSID_PAREMETER_ID, "SSID", "" }, { ConfigEntryType::kPassword, PASSPHRASE_PAREMETER_ID, "Passphrase", "" } } });    
    return true;
}

void WifiConfig::end() {
    if(initialized_) {
        reset();    
    }
}

bool WifiConfig::isSetupMode() {
    return setup_mode_;
}

void WifiConfig::setup() {
    if(!initialized_) {
        return;
    }

    // blink for up to SETUP_MODE_TIMEOUT ms
    // if the user presses the button and
    // - releases within FACTORY_RESET_TIMEOUT, enter setup mode
    // - otherwise perform a factory reset
    long end = millis() + SETUP_MODE_TIMEOUT;
    int mode = 0; // 0: normal boot, 1: setup mode, 2: factory reset
    long end_factory_reset = 0;
    while(millis() < end && mode == 0) {
        // turn on LED for 100ms
        pinMode(config_pin_, OUTPUT);
        digitalWrite(config_pin_, LOW);
        delay(200);
        // switch over to input to turn off LED and read pin
        pinMode(config_pin_, INPUT);
        long end2 = millis()+200;
        while(millis() < end2) {
            if(digitalRead(config_pin_) == LOW) {
                end_factory_reset = millis() + FACTORY_RESET_TIMEOUT;
                mode = 1;
                break;
            }
            delay(20);
        }
    }
    while(digitalRead(config_pin_) == LOW) {
        if(millis() >= end_factory_reset) {
            mode = 2;
        }
        delay(50);
    }

    // perform factory reset by deleting config.json
    if(mode == 2) {
        Serial.println("storeCodetupnfig(): Deleting config file");
        file_system_->remove("/config.json");
        // flicker for a second to signal factory reset
        long end_flicker = millis() + 1000;
        while(millis() < end_flicker) {
            pinMode(config_pin_, OUTPUT);
            digitalWrite(config_pin_, LOW);
            delay(30);
            pinMode(config_pin_, INPUT);
            delay(20);
        }
        mode = 1;
    }

    // load the config if possible and if not possible enter setup mode
    if(!loadConfig() || !hasParameter(SSID_PAREMETER_ID) || !hasParameter(PASSPHRASE_PAREMETER_ID)) {
        mode = 1;
    }

    if(mode == 1) { // enter setup mode
        setup_mode_ = true;
        // keep LED on and activate the AP
        pinMode(config_pin_, OUTPUT);
        digitalWrite(config_pin_, LOW);
        WiFi.hostname("wificonfig.local");
        WiFi.softAPConfig(IPAddress(192, 168, 0, 1), IPAddress(192, 168, 0, 1), IPAddress(255, 255, 255, 0));
        if(WiFi.softAP(setup_ssid_.c_str(), setup_password_.c_str())) {
            MDNS.begin(setup_ssid_.c_str());

            // use lambda functions to be able to call methods
            std::function<void()> handle_root_lambda = [this]() {
                this->handleRoot();
            };
            std::function<void()> handle_save_lambda = [this]() {
                this->handleSave();
            };
            std::function<void()> handle_not_found_lambda = [this]() {
                this->handleNotFound();
            };
            web_server_.on("/", handle_root_lambda);
            web_server_.on("/save", handle_save_lambda);
            web_server_.onNotFound(handle_not_found_lambda);
            web_server_.begin();
        }
    } else { // button not pressed and config found -> normal boot mode
        setup_mode_ = false;
        // turn off LED
        pinMode(config_pin_, INPUT);
        if(auto_connect_) {
            connect();
        }
    }
}

void WifiConfig::loop() {
    if(!initialized_) {
        return;
    }
    MDNS.update();
    web_server_.handleClient();
}

bool WifiConfig::connect() {
    std::string wifi_ssid, wifi_passphrase;
    if(!getParameter(SSID_PAREMETER_ID, wifi_ssid) || !getParameter(PASSPHRASE_PAREMETER_ID, wifi_passphrase)) {
        return false;
    }

    WiFi.mode(WIFI_STA);
    wifi_set_sleep_type(NONE_SLEEP_T); // make sure light sleep is enabled
    wifi_station_set_reconnect_policy(true);
    WiFi.begin(wifi_ssid.c_str(), wifi_passphrase.c_str());

    Serial.println();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

bool WifiConfig::loadConfig() {
    if(file_system_->exists("/config.json")) {
        Serial.println("loadConfig(): Config file exists");
        File config_file = file_system_->open("/config.json", "r");
        if(!config_file) {
            Serial.println("loadConfig(): Faild to open config file");
            return false;
        }
        StaticJsonDocument<WIFICONFIG_JSONBUFFER_SIZE> json_doc;
        DeserializationError json_err = deserializeJson(json_doc, config_file);
        config_file.close();
        if(json_err) {
            Serial.printf("loadConfig(): Faild to parse config file: %d\n", json_err);
            return false;
        }
        
        JsonObject json_obj = json_doc.as<JsonObject>();
        for(const auto& entry : json_obj) {
            auto it = config_values_.find(entry.key().c_str());
            if(it != config_values_.end()) {
                it->second->value = entry.value().as<std::string>();
            }
        }
        Serial.println("loadConfig(): Config file successfully loaded");
        return true;
    }
    Serial.println("loadConfig(): Config file does not exist");
    return false;
}

bool WifiConfig::storeConfig() {
    File config_file = file_system_->open("/config.json", "w");
    if(!config_file) {
        Serial.println("storeConfig(): Unable to open/create config file");
        return false;
    }
    StaticJsonDocument<WIFICONFIG_JSONBUFFER_SIZE> json_doc;
    for(const auto& entry : config_values_) {
        json_doc[entry.first.c_str()] = entry.second->value.c_str();
    }
    serializeJsonPretty(json_doc, config_file);
    config_file.close();
    Serial.println("storeConfig(): Config file written");
    return true;
}

bool WifiConfig::updateConfig(const char* json_data) {
    StaticJsonDocument<WIFICONFIG_JSONBUFFER_SIZE> json_doc;
    DeserializationError json_err = deserializeJson(json_doc, json_data);
    if(json_err) {
        return false;
    }
    bool success = true;
    for(auto& entry : config_values_) {
        if(json_doc.containsKey(entry.first.c_str())) {
            std::string value = json_doc[entry.first.c_str()].as<std::string>();
            // ignore all space passwords (since this means no change)
            if(entry.second->type != ConfigEntryType::kPassword || value.find_first_not_of(' ') != std::string::npos) {
                entry.second->value = value;
            }
        } else {
            success = false;
        }
    }
    return success;
}

void WifiConfig::reset() {
    web_server_.close();
    MDNS.end();
    setup_mode_ = false;
    file_system_ = nullptr;
    config_pin_ = -1;
    auto_connect_ = true;
    initialized_ = false;
    setup_ssid_ = "";
    setup_password_ = "";

    title_ = "";
    config_groups_.clear();
    config_values_.clear();
}

void WifiConfig::handleNotFound() {
    if(!initialized_) {
        return;
    }
    web_server_.send(404, "text/plain", "Not found!");
}

void WifiConfig::handleRoot() {
    if(!initialized_) {
        return;
    }
    
    unsigned long start = millis();
    // NOTE: tried streaming the content, but for some reason the performance is much worse (300+ms with yields, 3+s without)
    // than sending the entire document at once
    std::stringstream stream;
    HtmlElements::streamDocument(title_, config_groups_, stream);
    web_server_.sendHeader("Cache-Control", "no-cache"); // this adds the header rather than sending it
    web_server_.send(200, "text/html", stream.str().c_str());
    web_server_.client().stop();
    unsigned long end = millis();
    Serial.printf("Request took %dms\n", end-start);
}

void WifiConfig::handleSave() {
    if(!initialized_) {
        return;
    }
    if(web_server_.args() > 0) {
        bool success = (updateConfig(web_server_.arg(0).c_str()) && storeConfig());
        web_server_.sendHeader("Cache-Control", "no-cache");
        web_server_.send(200, "text/plain", (success) ? "{ \"result\": \"success\" }" : "{ \"result\": \"falied\" }");
    } else {
        web_server_.sendHeader("Cache-Control", "no-cache");
        web_server_.send(200, "text/plain", "{ \"result\": \"error\" }");
    }
}