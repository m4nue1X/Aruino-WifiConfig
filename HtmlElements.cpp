#include "HtmlElements.h"

#include <iostream>
#include <algorithm>

void HtmlElements::htmlDocumentPart1(const std::string& title, std::ostream& out) {
  out << "<!DOCTYPE html>" << std::endl
      << "<head>" << std::endl
      << "  <title>" << title << "</title>" << std::endl
      << "  <style>" << std::endl
      << "  body {" << std::endl
      << "    font-family: sans-serif;" << std::endl
      << "  }" << std::endl
      << "  </style>" << std::endl
      << "  <script>" << std::endl
      << "  function getElementValue(id) {" << std::endl
      << "    element = document.getElementById(id);" << std::endl
      << "    if(element != null) {" << std::endl
      << "      if(element.type.toLowerCase() == 'checkbox') {" << std::endl
      << "        return (element.checked ? 'true' : 'false');" << std::endl
      << "      }" << std::endl
      << "      return element.value;" << std::endl
      << "    } else {" << std::endl
      << "      elements = document.getElementsByName(id);" << std::endl
      << "      for(i=0; i<elements.length; ++i) {" << std::endl
      << "        if(elements[i].tagName.toLowerCase() == 'input' && elements[i].type.toLowerCase() == 'radio' && elements[i].checked) {" << std::endl
      << "          return elements[i].value;" << std::endl
      << "        }" << std::endl
      << "      }" << std::endl
      << "      return null;" << std::endl
      << "    }" << std::endl
      << "  }" << std::endl
      << "  function save() {" << std::endl
      << "    data = {" << std::endl;
}

void HtmlElements::htmlSubmitScriptEntry(const std::string& config_entry_id, std::ostream& out) {
  out << "      " << config_entry_id << ": getElementValue('" << config_entry_id << "')," << std::endl;
}

void HtmlElements::htmlDocumentPart2(const std::string& title, std::ostream& out) {
  out << "    }" << std::endl
      << "    fetch('/save', { method: 'POST', body: JSON.stringify(data) }).then(response => { " << std::endl
      << "      response.text().then(text => { " << std::endl
      << "        handleResponse(response.ok, text);" << std::endl
      << "      })" << std::endl
      << "    });" << std::endl
      << "  }" << std::endl
      << "  function handleResponse(ok, text) {" << std::endl
      << "    result = 'error';" << std::endl
      << "    if(ok) {" << std::endl
      << "      result = JSON.parse(text)['result'];" << std::endl
      << "    }" << std::endl
      << "    result_span = document.getElementById('status');" << std::endl
      << "    result_span.innerText = result;" << std::endl
      << "    if(result.toLowerCase() == 'success') {" << std::endl
      << "      result_span.style.color = 'green';" << std::endl
      << "    } else {" << std::endl
      << "      result_span.style.color = 'red';" << std::endl
      << "    }" << std::endl
      << "    window.setTimeout(resetResult, 3000);" << std::endl
      << "  }" << std::endl
      << "  function resetResult() {" << std::endl
      << "    result_span = document.getElementById('status');" << std::endl
      << "    result_span.innerText = '';" << std::endl
      << "    result_span.style.color = '';" << std::endl
      << "  }" << std::endl
      << "  </script>" << std::endl
      << "</head>" << std::endl
      << "<body>" << std::endl
      << "    <h1>" << title << "</h1>" << std::endl
      << "    <table>" << std::endl;
}

void HtmlElements::htmlConfigGroupHeader(const std::string& config_group_name, std::ostream& out) {
  out << "      <tr><td colspan='3'><h2>" << config_group_name << "</h2></td><tr>" << std::endl;
}

void HtmlElements::htmlConfigEntryText(const std::string& entry_name, const std::string& entry_id, const std::string& entry_value, std::ostream& out) {
  out << "      <tr><td>" << entry_name << "</td><td><input type='text' id='" << entry_id << "' value='" << entry_value << "'/></td><td></td></tr>" << std::endl;
}

void HtmlElements::htmlConfigEntryPassword(const std::string& entry_name, const std::string& entry_id, const std::string& entry_value, std::ostream& out) {
  out << "      <tr><td>" << entry_name << "</td><td><input type='password' id='" << entry_id << "' value='" << entry_value << "'/></td><td></td></tr>" << std::endl;
}

void HtmlElements::htmlConfigEntryCheckBox(const std::string& entry_name, const std::string& entry_id, bool entry_checked, std::ostream& out) {
  out << "      <tr><td>" << entry_name << "</td><td><input type='checkbox' id='" << entry_id << "'" << (entry_checked ? " checked='on'" : "") << "/></td><td></td></tr>" << std::endl;
}

void HtmlElements::htmlConfigEntryRadioGroupBegin(const std::string& entry_name, std::ostream& out) {
  out << "      <tr><td>" << entry_name << "</td><td>";
}

void HtmlElements::htmlConfigEntryRadioGroupEntry(const std::string& group_name, const std::string& group_value, const std::string& entry_value, const std::string& entry_name, std::ostream& out) {
  out << "<input type='radio' name='" << group_name << "' value='" << entry_value << "'" << ((group_value == entry_value) ? " checked='on'" : "") << "/>" << entry_name << "  ";
}

void HtmlElements::htmlConfigEntryRadioGroupEnd(std::ostream& out) {
  out << "</td><td></td></tr>" << std::endl;
}

void HtmlElements::htmlConfigEntryDropDownBegin(const std::string& entry_name, const std::string& entry_id, std::ostream& out) {
  out << "      <tr><td>" << entry_name << "</td><td><select id='" << entry_id << "'>";
}

void HtmlElements::htmlConfigEntryDropDownEntry(const std::string& group_value, const std::string& entry_value, const std::string& entry_name, std::ostream& out) {
  out << "<option value='" << entry_value << "'" << ((group_value == entry_value) ? " selected='on'" : "") << "/>" << entry_name << "</option>";
}

void HtmlElements::htmlConfigEntryDropDownEnd(std::ostream& out) {
  out << "</select></td><td></td></tr>" << std::endl;
}

void HtmlElements::htmlDocumentPart3(std::ostream& out) {
  out << "      <tr><td/><td align='right'><span id='status'></span>  <button onclick='save()'>save</button></td><td></td></tr>" << std::endl
      << "    </table>" << std::endl
      << "</body>" << std::endl
      << "</html>" << std::endl;
}

void HtmlElements::streamDocument(const std::string& title, const std::vector<ConfigGroup>& config_groups, std::ostream& out) {
  htmlDocumentPart1(title, out);
  generateSubmitScript(config_groups, out);
  htmlDocumentPart2(title, out); 
  generateConfigGroups(config_groups, out);
  htmlDocumentPart3(out);
  out.flush();
}

void HtmlElements::generateSubmitScript(const std::vector<ConfigGroup>& config_groups, std::ostream& out) {
  for(const ConfigGroup& config_group : config_groups) {
    for(const ConfigEntry& config_entry : config_group.entries) {
      htmlSubmitScriptEntry(config_entry.id, out);
    }
  }
}

void HtmlElements::generateConfigGroups(const std::vector<ConfigGroup>& config_groups, std::ostream& out) {
  for(const ConfigGroup& config_group : config_groups) {
    generateConfigGroup(config_group, out);
  }
}

void HtmlElements::generateConfigGroup(const ConfigGroup& config_group, std::ostream& out) {
  auto get_bool_value = [&](const std::string& value) -> bool {
    std::string mvalue = value;
    std::transform(mvalue.cbegin(), mvalue.cend(), mvalue.begin(), [](unsigned char c) { return std::tolower(c); });
    return !(mvalue == "false" || mvalue == "0" || mvalue == "no" || mvalue == "off");
  };

  htmlConfigGroupHeader(config_group.name, out);
  for(const ConfigEntry& entry : config_group.entries) {
    switch(entry.type) {
      case ConfigEntryType::kText:
        htmlConfigEntryText(entry.name, entry.id, entry.value, out);
        break;
      case ConfigEntryType::kPassword:
        htmlConfigEntryPassword(entry.name, entry.id, std::string(entry.value.length(), ' '), out); // use spaces instead of actual password
        break;
      case ConfigEntryType::kCheckBox:
        htmlConfigEntryCheckBox(entry.name, entry.id, get_bool_value(entry.value), out);
        break;
      case ConfigEntryType::kRadioGroup:
        htmlConfigEntryRadioGroupBegin(entry.name, out);
        for(const std::pair<std::string, std::string>& item : entry.options) {
          htmlConfigEntryRadioGroupEntry(entry.id, entry.value, item.first, item.second, out);
        }
        htmlConfigEntryRadioGroupEnd(out);
        break;
      case ConfigEntryType::kDropDown:
        htmlConfigEntryDropDownBegin(entry.name, entry.id, out);
        for(const std::pair<std::string, std::string>& item : entry.options) {
          htmlConfigEntryDropDownEntry(entry.value, item.first, item.second, out);
        }
        htmlConfigEntryDropDownEnd(out);
        break;
    }
  }
}
