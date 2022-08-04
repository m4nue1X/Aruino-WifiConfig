
#ifndef __WIFICONFIG_CONFIG
#define __WIFICONFIG_CONFIG

#include <string>
#include <vector>

enum class ConfigEntryType {
    kText,
    kPassword,
    kCheckBox,
    kRadioGroup,
    kDropDown
};

class ConfigEntry {
public:
    using OptionType = std::pair<std::string, std::string>;
    using OptionList = std::vector<OptionType>;

    ConfigEntry(ConfigEntryType entry_type, const std::string& entry_id, const std::string& entry_name, const std::string& entry_default_value, const OptionList& entry_options = OptionList());

    ConfigEntryType type;
    std::string id;
    std::string name;
    std::string default_value;
    std::string value;
    OptionList options;
};

class ConfigGroup {
public:
    using EntryList = std::vector<ConfigEntry>;

    ConfigGroup(const std::string& group_name, const EntryList& config_entries);

    std::string name;
    EntryList entries;
};

#endif // __WIFICONFIG_CONFIG