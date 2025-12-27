#include "memory_session_repository.h"

namespace english_learning {
namespace repository {
namespace memory {

bool MemorySessionRepository::add(const core::Session& session) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (sessions_.find(session.sessionToken) != sessions_.end()) {
        return false; // Token already exists
    }
    sessions_[session.sessionToken] = session;
    return true;
}

std::optional<core::Session> MemorySessionRepository::findByToken(const std::string& token) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessions_.find(token);
    if (it != sessions_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::string> MemorySessionRepository::findTokenBySocket(int socket) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = clientSessions_.find(socket);
    if (it != clientSessions_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<std::string> MemorySessionRepository::validateSession(const std::string& token) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessions_.find(token);
    if (it != sessions_.end()) {
        int64_t now = protocol::utils::getCurrentTimestamp();
        if (now <= it->second.expiresAt) {
            return it->second.userId;
        }
    }
    return std::nullopt;
}

bool MemorySessionRepository::associateSocket(int socket, const std::string& token) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (sessions_.find(token) == sessions_.end()) {
        return false; // Token doesn't exist
    }
    clientSessions_[socket] = token;
    return true;
}

bool MemorySessionRepository::extendSession(const std::string& token, int64_t newExpiry) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessions_.find(token);
    if (it != sessions_.end()) {
        it->second.expiresAt = newExpiry;
        return true;
    }
    return false;
}

bool MemorySessionRepository::remove(const std::string& token) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = sessions_.find(token);
    if (it != sessions_.end()) {
        // Also remove from client sessions
        for (auto csIt = clientSessions_.begin(); csIt != clientSessions_.end(); ) {
            if (csIt->second == token) {
                csIt = clientSessions_.erase(csIt);
            } else {
                ++csIt;
            }
        }
        sessions_.erase(it);
        return true;
    }
    return false;
}

bool MemorySessionRepository::removeBySocket(int socket) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = clientSessions_.find(socket);
    if (it != clientSessions_.end()) {
        clientSessions_.erase(it);
        return true;
    }
    return false;
}

size_t MemorySessionRepository::removeExpired() {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t removed = 0;
    int64_t now = protocol::utils::getCurrentTimestamp();

    for (auto it = sessions_.begin(); it != sessions_.end(); ) {
        if (now > it->second.expiresAt) {
            // Remove associated client sessions
            for (auto csIt = clientSessions_.begin(); csIt != clientSessions_.end(); ) {
                if (csIt->second == it->first) {
                    csIt = clientSessions_.erase(csIt);
                } else {
                    ++csIt;
                }
            }
            it = sessions_.erase(it);
            ++removed;
        } else {
            ++it;
        }
    }
    return removed;
}

size_t MemorySessionRepository::count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return sessions_.size();
}

bool MemorySessionRepository::isValid(const std::string& token) const {
    return validateSession(token).has_value();
}

} // namespace memory
} // namespace repository
} // namespace english_learning
