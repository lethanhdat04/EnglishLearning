#ifndef ENGLISH_LEARNING_REPOSITORY_I_SESSION_REPOSITORY_H
#define ENGLISH_LEARNING_REPOSITORY_I_SESSION_REPOSITORY_H

#include <string>
#include <optional>
#include "include/core/session.h"

namespace english_learning {
namespace repository {

/**
 * Interface for session data access operations.
 * Manages user authentication sessions.
 */
class ISessionRepository {
public:
    virtual ~ISessionRepository() = default;

    // Create
    virtual bool add(const core::Session& session) = 0;

    // Read
    virtual std::optional<core::Session> findByToken(const std::string& token) const = 0;
    virtual std::optional<std::string> findTokenBySocket(int socket) const = 0;
    virtual std::optional<std::string> validateSession(const std::string& token) const = 0;

    // Update
    virtual bool associateSocket(int socket, const std::string& token) = 0;
    virtual bool extendSession(const std::string& token, int64_t newExpiry) = 0;

    // Delete
    virtual bool remove(const std::string& token) = 0;
    virtual bool removeBySocket(int socket) = 0;
    virtual size_t removeExpired() = 0;

    // Utility
    virtual size_t count() const = 0;
    virtual bool isValid(const std::string& token) const = 0;
};

} // namespace repository
} // namespace english_learning

#endif // ENGLISH_LEARNING_REPOSITORY_I_SESSION_REPOSITORY_H
