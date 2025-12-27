#ifndef ENGLISH_LEARNING_PROTOCOL_JSON_BUILDER_H
#define ENGLISH_LEARNING_PROTOCOL_JSON_BUILDER_H

#include <string>
#include <sstream>
#include <vector>
#include "json_parser.h"

namespace english_learning {
namespace protocol {

/**
 * Fluent JSON builder for constructing JSON responses.
 * Provides a type-safe way to build JSON without manual string concatenation.
 */
class JsonBuilder {
public:
    JsonBuilder() : first_(true), hasContent_(false) {
        ss_ << "{";
    }

    // Add a string field
    JsonBuilder& addString(const std::string& key, const std::string& value) {
        addComma();
        ss_ << "\"" << key << "\":\"" << JsonParser::escape(value) << "\"";
        return *this;
    }

    // Add an integer field
    JsonBuilder& addInt(const std::string& key, int value) {
        addComma();
        ss_ << "\"" << key << "\":" << value;
        return *this;
    }

    // Add a long long field
    JsonBuilder& addLong(const std::string& key, long long value) {
        addComma();
        ss_ << "\"" << key << "\":" << value;
        return *this;
    }

    // Add a double field
    JsonBuilder& addDouble(const std::string& key, double value) {
        addComma();
        ss_ << "\"" << key << "\":" << value;
        return *this;
    }

    // Add a boolean field
    JsonBuilder& addBool(const std::string& key, bool value) {
        addComma();
        ss_ << "\"" << key << "\":" << (value ? "true" : "false");
        return *this;
    }

    // Add a raw JSON value (object or array) - no escaping
    JsonBuilder& addRaw(const std::string& key, const std::string& rawJson) {
        addComma();
        ss_ << "\"" << key << "\":" << rawJson;
        return *this;
    }

    // Add a null field
    JsonBuilder& addNull(const std::string& key) {
        addComma();
        ss_ << "\"" << key << "\":null";
        return *this;
    }

    // Add a nested object using another builder
    JsonBuilder& addObject(const std::string& key, const JsonBuilder& nested) {
        addComma();
        ss_ << "\"" << key << "\":" << nested.build();
        return *this;
    }

    // Add a string array
    JsonBuilder& addStringArray(const std::string& key, const std::vector<std::string>& values) {
        addComma();
        ss_ << "\"" << key << "\":[";
        bool firstItem = true;
        for (const auto& v : values) {
            if (!firstItem) ss_ << ",";
            ss_ << "\"" << JsonParser::escape(v) << "\"";
            firstItem = false;
        }
        ss_ << "]";
        return *this;
    }

    // Build the final JSON string
    std::string build() const {
        return ss_.str() + "}";
    }

    // Reset the builder for reuse
    void reset() {
        ss_.str("");
        ss_.clear();
        ss_ << "{";
        first_ = true;
        hasContent_ = false;
    }

private:
    void addComma() {
        if (!first_) {
            ss_ << ",";
        }
        first_ = false;
        hasContent_ = true;
    }

    std::stringstream ss_;
    bool first_;
    bool hasContent_;
};

/**
 * Helper class for building standard protocol responses.
 */
class ResponseBuilder {
public:
    /**
     * Build a success response with data.
     */
    static std::string success(const std::string& messageType,
                               const std::string& messageId,
                               long long timestamp,
                               const std::string& dataJson = "{}") {
        JsonBuilder builder;
        builder.addString("messageType", messageType)
               .addString("messageId", messageId)
               .addLong("timestamp", timestamp);

        JsonBuilder payload;
        payload.addString("status", "success")
               .addRaw("data", dataJson);

        builder.addObject("payload", payload);
        return builder.build();
    }

    /**
     * Build a success response with a message.
     */
    static std::string successMessage(const std::string& messageType,
                                      const std::string& messageId,
                                      long long timestamp,
                                      const std::string& message) {
        JsonBuilder builder;
        builder.addString("messageType", messageType)
               .addString("messageId", messageId)
               .addLong("timestamp", timestamp);

        JsonBuilder payload;
        payload.addString("status", "success")
               .addString("message", message);

        builder.addObject("payload", payload);
        return builder.build();
    }

    /**
     * Build an error response.
     */
    static std::string error(const std::string& messageType,
                             const std::string& messageId,
                             long long timestamp,
                             const std::string& errorMessage) {
        JsonBuilder builder;
        builder.addString("messageType", messageType)
               .addString("messageId", messageId)
               .addLong("timestamp", timestamp);

        JsonBuilder payload;
        payload.addString("status", "error")
               .addString("message", errorMessage);

        builder.addObject("payload", payload);
        return builder.build();
    }
};

/**
 * Helper class for building JSON arrays.
 */
class JsonArrayBuilder {
public:
    JsonArrayBuilder() : first_(true) {
        ss_ << "[";
    }

    // Add a raw JSON element (object or value)
    JsonArrayBuilder& addRaw(const std::string& rawJson) {
        if (!first_) ss_ << ",";
        ss_ << rawJson;
        first_ = false;
        return *this;
    }

    // Add a string element
    JsonArrayBuilder& addString(const std::string& value) {
        if (!first_) ss_ << ",";
        ss_ << "\"" << JsonParser::escape(value) << "\"";
        first_ = false;
        return *this;
    }

    // Add an object from a builder
    JsonArrayBuilder& addObject(const JsonBuilder& obj) {
        if (!first_) ss_ << ",";
        ss_ << obj.build();
        first_ = false;
        return *this;
    }

    // Build the final array string
    std::string build() const {
        return ss_.str() + "]";
    }

    // Check if array is empty
    bool isEmpty() const {
        return first_;
    }

    // Reset the builder
    void reset() {
        ss_.str("");
        ss_.clear();
        ss_ << "[";
        first_ = true;
    }

private:
    std::stringstream ss_;
    bool first_;
};

} // namespace protocol
} // namespace english_learning

#endif // ENGLISH_LEARNING_PROTOCOL_JSON_BUILDER_H
