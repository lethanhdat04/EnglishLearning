#ifndef ENGLISH_LEARNING_REPOSITORY_MEMORY_USER_REPOSITORY_H
#define ENGLISH_LEARNING_REPOSITORY_MEMORY_USER_REPOSITORY_H

#include <map>
#include <mutex>
#include "include/repository/i_user_repository.h"

namespace english_learning {
namespace repository {
namespace memory {

/**
 * In-memory implementation of IUserRepository.
 * Thread-safe with mutex protection.
 */
class MemoryUserRepository : public IUserRepository {
public:
    MemoryUserRepository() = default;
    ~MemoryUserRepository() override = default;

    // Create
    bool add(const core::User& user) override;

    // Read
    std::optional<core::User> findByEmail(const std::string& email) const override;
    std::optional<core::User> findById(const std::string& userId) const override;
    std::vector<core::User> findAll() const override;
    std::vector<core::User> findOnlineUsers() const override;
    std::vector<core::User> findByRole(const std::string& role) const override;
    bool exists(const std::string& email) const override;
    bool existsById(const std::string& userId) const override;

    // Update
    bool update(const core::User& user) override;
    bool updateLevel(const std::string& userId, const std::string& level) override;
    bool setOnlineStatus(const std::string& userId, bool online, int socket = -1) override;

    // Delete
    bool remove(const std::string& userId) override;

    // Utility
    size_t count() const override;
    bool isTeacher(const std::string& userId) const override;
    bool isAdmin(const std::string& userId) const override;

    // Direct access for backward compatibility (during migration)
    core::User* getPointerById(const std::string& userId);
    const std::map<std::string, core::User>& getUsersByEmail() const { return usersByEmail_; }

private:
    mutable std::mutex mutex_;
    std::map<std::string, core::User> usersByEmail_;  // email -> User
    std::map<std::string, std::string> userIdToEmail_; // userId -> email (for fast lookup)
};

} // namespace memory
} // namespace repository
} // namespace english_learning

#endif // ENGLISH_LEARNING_REPOSITORY_MEMORY_USER_REPOSITORY_H
