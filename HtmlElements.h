
#ifndef __WIFICONFIG_HTMLELEMENTS
#define __WIFICONFIG_HTMLELEMENTS

#include <string>
#include <vector>
#include <map>
#include <Config.h>

class HtmlElements {
public:
  static void streamDocument(const std::string& title, const std::vector<ConfigGroup>& config_groups, std::ostream& out);
private:
  static void htmlDocumentPart1(const std::string& title, std::ostream& out);
  static void htmlSubmitScriptEntry(const std::string& config_entry_id, std::ostream& out);
  static void htmlDocumentPart2(const std::string& title, std::ostream& out);
  static void htmlConfigGroupHeader(const std::string& config_group_name, std::ostream& out);
  static void htmlConfigEntryText(const std::string& entry_name, const std::string& entry_id, const std::string& entry_value, std::ostream& out);
  static void htmlConfigEntryPassword(const std::string& entry_name, const std::string& entry_id, const std::string& entry_value, std::ostream& out);
  static void htmlConfigEntryCheckBox(const std::string& entry_name, const std::string& entry_id, bool entry_checked, std::ostream& out);
  static void htmlConfigEntryRadioGroupBegin(const std::string& entry_name, std::ostream& out);
  static void htmlConfigEntryRadioGroupEntry(const std::string& group_name, const std::string& group_value, const std::string& entry_value, const std::string& entry_name, std::ostream& out);
  static void htmlConfigEntryRadioGroupEnd(std::ostream& out);
  static void htmlConfigEntryDropDownBegin(const std::string& entry_name, const std::string& entry_id, std::ostream& out);
  static void htmlConfigEntryDropDownEntry(const std::string& group_value, const std::string& entry_value, const std::string& entry_name, std::ostream& out);
  static void htmlConfigEntryDropDownEnd(std::ostream& out);
  static void htmlDocumentPart3(std::ostream& out);
  static void generateSubmitScript(const std::vector<ConfigGroup>& config_groups, std::ostream& out);
  static void generateConfigGroups(const std::vector<ConfigGroup>& config_groups, std::ostream& out);
  static void generateConfigGroup(const ConfigGroup& config_group, std::ostream& out);
};

#endif // __WIFICONFIG_HTMLELEMENTS