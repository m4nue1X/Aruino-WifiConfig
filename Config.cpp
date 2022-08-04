#include "Config.h"

ConfigEntry::ConfigEntry(ConfigEntryType entry_type, const std::string& entry_id, const std::string& entry_name, const std::string& entry_default_value, const OptionList& entry_options) :
    type(entry_type), id(entry_id), name(entry_name), default_value(entry_default_value), value(entry_default_value), options(entry_options) { }


ConfigGroup::ConfigGroup(const std::string& group_name, const EntryList& config_entries) : name(group_name), entries(config_entries) { }