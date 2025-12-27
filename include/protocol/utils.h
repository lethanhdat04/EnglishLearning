#ifndef ENGLISH_LEARNING_PROTOCOL_UTILS_H
#define ENGLISH_LEARNING_PROTOCOL_UTILS_H

#include <string>
#include <chrono>
#include <random>
#include <sstream>
#include <iomanip>

namespace english_learning {
namespace protocol {

/**
 * Utility functions for the protocol layer.
 */
namespace utils {

/**
 * Get current timestamp in milliseconds since epoch.
 */
inline long long getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

/**
 * Generate a random ID with a given prefix.
 * Format: prefix_XXXXX (where X is a random digit)
 */
inline std::string generateId(const std::string& prefix) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(10000, 99999);
    return prefix + "_" + std::to_string(dis(gen));
}

/**
 * Generate a session token (64 random alphanumeric characters).
 */
inline std::string generateSessionToken() {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);

    std::string token;
    token.reserve(64);
    for (int i = 0; i < 64; ++i) {
        token += alphanum[dis(gen)];
    }
    return token;
}

/**
 * Generate a message ID for protocol messages.
 */
inline std::string generateMessageId() {
    static int counter = 0;
    return "msg_" + std::to_string(++counter) + "_" +
           std::to_string(getCurrentTimestamp() % 100000);
}

/**
 * Format a timestamp to a human-readable string.
 */
inline std::string formatTimestamp(long long timestamp) {
    auto tp = std::chrono::system_clock::time_point(
        std::chrono::milliseconds(timestamp)
    );
    auto time = std::chrono::system_clock::to_time_t(tp);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

/**
 * Calculate session expiry time (1 hour from now).
 */
inline long long calculateSessionExpiry() {
    return getCurrentTimestamp() + (60 * 60 * 1000); // 1 hour in milliseconds
}

/**
 * Check if a session has expired.
 */
inline bool isSessionExpired(long long expiresAt) {
    return getCurrentTimestamp() > expiresAt;
}

} // namespace utils
} // namespace protocol
} // namespace english_learning

#endif // ENGLISH_LEARNING_PROTOCOL_UTILS_H
