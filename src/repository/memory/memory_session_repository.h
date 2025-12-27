#ifndef ENGLISH_LEARNING_REPOSITORY_MEMORY_SESSION_REPOSITORY_H
#define ENGLISH_LEARNING_REPOSITORY_MEMORY_SESSION_REPOSITORY_H

#include <map>
#include <mutex>
#include "include/repository/i_session_repository.h"
#include "include/protocol/utils.h"

namespace english_learning {
namespace repository {
namespace memory {

/**
 * In-memory implementation of ISessionRepository.
 * Thread-safe with mutex protection.
 */
class MemorySessionRepository : public ISessionRepository {
public:
    MemorySessionRepository() = default;
    ~MemorySessionRepository() override = default;

    // Create
    bool add(const core::Session& session) override;

    // Read
    std::optional<core::Session> findByToken(const std::string& token) const override;
    std::optional<std::string> findTokenBySocket(int socket) const override;
    std::optional<std::string> validateSession(const std::string& token) const override;

    // Update
    bool associateSocket(int socket, const std::string& token) override;
    bool extendSession(const std::string& token, int64_t newExpiry) override;

    // Delete
    bool remove(const std::string& token) override;
    bool removeBySocket(int socket) override;
    size_t removeExpired() override;

    // Utility
    size_t count() const override;
    bool isValid(const std::string& token) const override;

    // Direct access for backward compatibility
    const std::map<std::string, core::Session>& getSessions() const { return sessions_; }
    const std::map<int, std::string>& getClientSessions() const { return clientSessions_; }

private:
    mutable std::mutex mutex_;
    std::map<std::string, core::Session> sessions_;      // token -> Session
    std::map<int, std::string> clientSessions_;          // socket -> token
};

} // namespace memory
} // namespace repository
} // namespace english_learning

#endif // ENGLISH_LEARNING_REPOSITORY_MEMORY_SESSION_REPOSITORY_H
