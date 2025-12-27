#ifndef ENGLISH_LEARNING_CORE_SESSION_H
#define ENGLISH_LEARNING_CORE_SESSION_H

#include <string>
#include "types.h"

namespace english_learning {
namespace core {

/**
 * Session entity representing an authenticated user session.
 * Sessions are time-limited and tied to a specific user.
 */
struct Session {
    std::string sessionToken;
    std::string userId;
    Timestamp expiresAt;

    Session() : expiresAt(0) {}

    Session(const std::string& token, const std::string& uid, Timestamp expires)
        : sessionToken(token), userId(uid), expiresAt(expires) {}

    // Check if session has expired
    bool isExpired(Timestamp currentTime) const {
        return currentTime > expiresAt;
    }

    // Check if session is valid
    bool isValid(Timestamp currentTime) const {
        return !sessionToken.empty() && !userId.empty() && !isExpired(currentTime);
    }
};

} // namespace core
} // namespace english_learning

#endif // ENGLISH_LEARNING_CORE_SESSION_H
