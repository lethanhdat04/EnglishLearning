#include "include/protocol/json_parser.h"

namespace english_learning {
namespace protocol {

std::string JsonParser::getValue(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    if (keyPos == std::string::npos) return "";

    size_t colonPos = json.find(':', keyPos);
    if (colonPos == std::string::npos) return "";

    size_t valueStart = colonPos + 1;
    while (valueStart < json.length() &&
           (json[valueStart] == ' ' || json[valueStart] == '\t' || json[valueStart] == '\n')) {
        valueStart++;
    }

    if (valueStart >= json.length()) return "";

    if (json[valueStart] == '"') {
        size_t valueEnd = valueStart + 1;
        while (valueEnd < json.length()) {
            if (json[valueEnd] == '"' && json[valueEnd - 1] != '\\') {
                break;
            }
            valueEnd++;
        }
        if (valueEnd < json.length()) {
            return json.substr(valueStart + 1, valueEnd - valueStart - 1);
        }
    } else {
        size_t valueEnd = json.find_first_of(",}\n]", valueStart);
        if (valueEnd != std::string::npos) {
            std::string value = json.substr(valueStart, valueEnd - valueStart);
            // Trim whitespace
            size_t start = value.find_first_not_of(" \t\n\r");
            if (start == std::string::npos) return "";
            size_t end = value.find_last_not_of(" \t\n\r");
            return value.substr(start, end - start + 1);
        }
    }

    return "";
}

std::string JsonParser::getObject(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    if (keyPos == std::string::npos) return "";

    size_t colonPos = json.find(':', keyPos);
    if (colonPos == std::string::npos) return "";

    size_t bracePos = json.find('{', colonPos);
    if (bracePos == std::string::npos) return "";

    int braceCount = 1;
    size_t endPos = bracePos + 1;
    while (endPos < json.length() && braceCount > 0) {
        if (json[endPos] == '{') braceCount++;
        else if (json[endPos] == '}') braceCount--;
        endPos++;
    }

    return json.substr(bracePos, endPos - bracePos);
}

std::string JsonParser::getArray(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t keyPos = json.find(searchKey);
    if (keyPos == std::string::npos) return "";

    size_t colonPos = json.find(':', keyPos);
    if (colonPos == std::string::npos) return "";

    size_t bracketPos = json.find('[', colonPos);
    if (bracketPos == std::string::npos) return "";

    int bracketCount = 1;
    size_t endPos = bracketPos + 1;
    while (endPos < json.length() && bracketCount > 0) {
        if (json[endPos] == '[') bracketCount++;
        else if (json[endPos] == ']') bracketCount--;
        endPos++;
    }

    return json.substr(bracketPos, endPos - bracketPos);
}

std::vector<std::string> JsonParser::parseArray(const std::string& arrayStr) {
    std::vector<std::string> result;
    if (arrayStr.empty() || arrayStr[0] != '[') return result;

    size_t pos = 1;
    while (pos < arrayStr.length()) {
        // Skip whitespace and commas
        while (pos < arrayStr.length() &&
               (arrayStr[pos] == ' ' || arrayStr[pos] == '\n' ||
                arrayStr[pos] == '\t' || arrayStr[pos] == ',')) {
            pos++;
        }

        if (pos >= arrayStr.length() || arrayStr[pos] == ']') break;

        if (arrayStr[pos] == '{') {
            int braceCount = 1;
            size_t start = pos;
            pos++;
            while (pos < arrayStr.length() && braceCount > 0) {
                if (arrayStr[pos] == '{') braceCount++;
                else if (arrayStr[pos] == '}') braceCount--;
                pos++;
            }
            result.push_back(arrayStr.substr(start, pos - start));
        } else if (arrayStr[pos] == '"') {
            // Handle string array elements
            size_t start = pos + 1;
            pos++;
            while (pos < arrayStr.length() &&
                   !(arrayStr[pos] == '"' && arrayStr[pos - 1] != '\\')) {
                pos++;
            }
            if (pos < arrayStr.length()) {
                result.push_back(arrayStr.substr(start, pos - start));
                pos++;
            }
        } else {
            pos++;
        }
    }
    return result;
}

std::string JsonParser::escape(const std::string& str) {
    std::string result;
    result.reserve(str.size() + 10);  // Pre-allocate for efficiency

    for (char c : str) {
        switch (c) {
            case '"':  result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            default:   result += c; break;
        }
    }
    return result;
}

std::string JsonParser::unescape(const std::string& str) {
    std::string result;
    result.reserve(str.size());

    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case 'n':  result += '\n'; i++; break;
                case 'r':  result += '\r'; i++; break;
                case 't':  result += '\t'; i++; break;
                case '"':  result += '"'; i++; break;
                case '\\': result += '\\'; i++; break;
                case 'b':  result += '\b'; i++; break;
                case 'f':  result += '\f'; i++; break;
                default:   result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

} // namespace protocol
} // namespace english_learning
