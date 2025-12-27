#ifndef ENGLISH_LEARNING_SERVICE_RESULT_H
#define ENGLISH_LEARNING_SERVICE_RESULT_H

#include <string>
#include <optional>

namespace english_learning {
namespace service {

/**
 * Generic result type for service operations.
 * Encapsulates success/failure status with optional data and error message.
 */
template<typename T>
class ServiceResult {
public:
    // Success result with data
    static ServiceResult success(const T& data) {
        ServiceResult result;
        result.success_ = true;
        result.data_ = data;
        return result;
    }

    // Success result with message
    static ServiceResult successWithMessage(const T& data, const std::string& message) {
        ServiceResult result;
        result.success_ = true;
        result.data_ = data;
        result.message_ = message;
        return result;
    }

    // Error result
    static ServiceResult error(const std::string& errorMessage) {
        ServiceResult result;
        result.success_ = false;
        result.message_ = errorMessage;
        return result;
    }

    bool isSuccess() const { return success_; }
    bool isError() const { return !success_; }
    const T& getData() const { return data_.value(); }
    bool hasData() const { return data_.has_value(); }
    const std::string& getMessage() const { return message_; }

private:
    ServiceResult() : success_(false) {}

    bool success_;
    std::optional<T> data_;
    std::string message_;
};

/**
 * Specialized result for void operations (no return data).
 */
class VoidResult {
public:
    static VoidResult success() {
        VoidResult result;
        result.success_ = true;
        return result;
    }

    static VoidResult successWithMessage(const std::string& message) {
        VoidResult result;
        result.success_ = true;
        result.message_ = message;
        return result;
    }

    static VoidResult error(const std::string& errorMessage) {
        VoidResult result;
        result.success_ = false;
        result.message_ = errorMessage;
        return result;
    }

    bool isSuccess() const { return success_; }
    bool isError() const { return !success_; }
    const std::string& getMessage() const { return message_; }

private:
    VoidResult() : success_(false) {}

    bool success_;
    std::string message_;
};

} // namespace service
} // namespace english_learning

#endif // ENGLISH_LEARNING_SERVICE_RESULT_H
