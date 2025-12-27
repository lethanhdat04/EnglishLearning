#ifndef ENGLISH_LEARNING_PROTOCOL_JSON_PARSER_H
#define ENGLISH_LEARNING_PROTOCOL_JSON_PARSER_H

#include <string>
#include <vector>

namespace english_learning {
namespace protocol {

/**
 * Simple JSON parser for the application protocol.
 * Provides functions to extract values from JSON strings without
 * requiring a full JSON library.
 *
 * Note: This is a lightweight parser suitable for the protocol's needs.
 * For complex JSON operations, consider using a proper JSON library.
 */
class JsonParser {
public:
    /**
     * Extract a string or primitive value from JSON by key.
     * @param json The JSON string to parse
     * @param key The key to look for
     * @return The value as a string, or empty string if not found
     */
    static std::string getValue(const std::string& json, const std::string& key);

    /**
     * Extract a nested JSON object by key.
     * @param json The JSON string to parse
     * @param key The key to look for
     * @return The nested object as a string (including braces), or empty string if not found
     */
    static std::string getObject(const std::string& json, const std::string& key);

    /**
     * Extract a JSON array by key.
     * @param json The JSON string to parse
     * @param key The key to look for
     * @return The array as a string (including brackets), or empty string if not found
     */
    static std::string getArray(const std::string& json, const std::string& key);

    /**
     * Parse a JSON array string into a vector of object strings.
     * @param arrayStr The JSON array string (must start with '[')
     * @return Vector of individual object strings
     */
    static std::vector<std::string> parseArray(const std::string& arrayStr);

    /**
     * Escape special characters in a string for JSON encoding.
     * @param str The string to escape
     * @return The escaped string safe for JSON
     */
    static std::string escape(const std::string& str);

    /**
     * Unescape JSON-encoded special characters.
     * @param str The escaped string
     * @return The unescaped original string
     */
    static std::string unescape(const std::string& str);
};

// Convenience functions (for backward compatibility and ease of use)
inline std::string getJsonValue(const std::string& json, const std::string& key) {
    return JsonParser::getValue(json, key);
}

inline std::string getJsonObject(const std::string& json, const std::string& key) {
    return JsonParser::getObject(json, key);
}

inline std::string getJsonArray(const std::string& json, const std::string& key) {
    return JsonParser::getArray(json, key);
}

inline std::vector<std::string> parseJsonArray(const std::string& arrayStr) {
    return JsonParser::parseArray(arrayStr);
}

inline std::string escapeJson(const std::string& str) {
    return JsonParser::escape(str);
}

inline std::string unescapeJson(const std::string& str) {
    return JsonParser::unescape(str);
}

} // namespace protocol
} // namespace english_learning

#endif // ENGLISH_LEARNING_PROTOCOL_JSON_PARSER_H
