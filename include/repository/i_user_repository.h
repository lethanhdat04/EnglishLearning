#ifndef ENGLISH_LEARNING_REPOSITORY_I_USER_REPOSITORY_H
#define ENGLISH_LEARNING_REPOSITORY_I_USER_REPOSITORY_H

#include <string>
#include <vector>
#include <optional>
#include "include/core/user.h"

namespace english_learning {
namespace repository {

/**
 * Interface for user data access operations.
 * Provides CRUD operations for User entities.
 */
class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    // Create
    virtual bool add(const core::User& user) = 0;

    // Read
    virtual std::optional<core::User> findByEmail(const std::string& email) const = 0;
    virtual std::optional<core::User> findById(const std::string& userId) const = 0;
    virtual std::vector<core::User> findAll() const = 0;
    virtual std::vector<core::User> findOnlineUsers() const = 0;
    virtual std::vector<core::User> findByRole(const std::string& role) const = 0;
    virtual bool exists(const std::string& email) const = 0;
    virtual bool existsById(const std::string& userId) const = 0;

    // Update
    virtual bool update(const core::User& user) = 0;
    virtual bool updateLevel(const std::string& userId, const std::string& level) = 0;
    virtual bool setOnlineStatus(const std::string& userId, bool online, int socket = -1) = 0;

    // Delete
    virtual bool remove(const std::string& userId) = 0;

    // Utility
    virtual size_t count() const = 0;
    virtual bool isTeacher(const std::string& userId) const = 0;
    virtual bool isAdmin(const std::string& userId) const = 0;
};

} // namespace repository
} // namespace english_learning

#endif // ENGLISH_LEARNING_REPOSITORY_I_USER_REPOSITORY_H
