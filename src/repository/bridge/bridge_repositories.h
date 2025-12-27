#ifndef ENGLISH_LEARNING_BRIDGE_REPOSITORIES_H
#define ENGLISH_LEARNING_BRIDGE_REPOSITORIES_H

/**
 * Bridge repositories that wrap the existing server global data structures.
 * This allows gradual migration to the service layer while maintaining
 * backward compatibility with existing handler code.
 */

#include <map>
#include <vector>
#include <mutex>
#include <string>
#include <optional>

#include "include/core/all.h"
#include "include/repository/i_user_repository.h"
#include "include/repository/i_session_repository.h"
#include "include/repository/i_lesson_repository.h"
#include "include/repository/i_test_repository.h"
#include "include/repository/i_chat_repository.h"
#include "include/repository/i_exercise_repository.h"
#include "include/repository/i_game_repository.h"

namespace english_learning {
namespace repository {
namespace bridge {

/**
 * Bridge user repository wrapping global user maps.
 */
class BridgeUserRepository : public IUserRepository {
public:
    BridgeUserRepository(
        std::map<std::string, core::User>& users,
        std::map<std::string, core::User*>& userById,
        std::mutex& mutex)
        : users_(users), userById_(userById), mutex_(mutex) {}

    bool add(const core::User& user) override {
        std::lock_guard<std::mutex> lock(mutex_);
        if (users_.find(user.email) != users_.end()) {
            return false;
        }
        users_[user.email] = user;
        userById_[user.userId] = &users_[user.email];
        return true;
    }

    std::optional<core::User> findByEmail(const std::string& email) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = users_.find(email);
        if (it != users_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::optional<core::User> findById(const std::string& userId) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = userById_.find(userId);
        if (it != userById_.end() && it->second != nullptr) {
            return *(it->second);
        }
        return std::nullopt;
    }

    std::vector<core::User> findAll() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<core::User> result;
        for (const auto& pair : users_) {
            result.push_back(pair.second);
        }
        return result;
    }

    std::vector<core::User> findOnlineUsers() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<core::User> result;
        for (const auto& pair : users_) {
            if (pair.second.online) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

    std::vector<core::User> findByRole(const std::string& role) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<core::User> result;
        for (const auto& pair : users_) {
            if (pair.second.role == role) {
                result.push_back(pair.second);
            }
        }
        return result;
    }

    bool exists(const std::string& email) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return users_.find(email) != users_.end();
    }

    bool existsById(const std::string& userId) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return userById_.find(userId) != userById_.end();
    }

    bool update(const core::User& user) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = users_.find(user.email);
        if (it != users_.end()) {
            it->second = user;
            return true;
        }
        return false;
    }

    bool updateLevel(const std::string& userId, const std::string& level) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = userById_.find(userId);
        if (it != userById_.end() && it->second != nullptr) {
            it->second->level = level;
            return true;
        }
        return false;
    }

    bool setOnlineStatus(const std::string& userId, bool online, int socket = -1) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = userById_.find(userId);
        if (it != userById_.end() && it->second != nullptr) {
            it->second->online = online;
            it->second->clientSocket = socket;
            return true;
        }
        return false;
    }

    bool remove(const std::string& userId) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = userById_.find(userId);
        if (it != userById_.end() && it->second != nullptr) {
            std::string email = it->second->email;
            userById_.erase(it);
            users_.erase(email);
            return true;
        }
        return false;
    }

    size_t count() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return users_.size();
    }

    bool isTeacher(const std::string& userId) const override {
        auto userOpt = findById(userId);
        if (userOpt.has_value()) {
            return userOpt.value().isTeacher();
        }
        return false;
    }

    bool isAdmin(const std::string& userId) const override {
        auto userOpt = findById(userId);
        if (userOpt.has_value()) {
            return userOpt.value().isAdmin();
        }
        return false;
    }

private:
    std::map<std::string, core::User>& users_;
    std::map<std::string, core::User*>& userById_;
    std::mutex& mutex_;
};

/**
 * Bridge session repository wrapping global session maps.
 */
class BridgeSessionRepository : public ISessionRepository {
public:
    BridgeSessionRepository(
        std::map<std::string, core::Session>& sessions,
        std::map<int, std::string>& clientSessions,
        std::mutex& mutex)
        : sessions_(sessions), clientSessions_(clientSessions), mutex_(mutex) {}

    bool add(const core::Session& session) override {
        std::lock_guard<std::mutex> lock(mutex_);
        sessions_[session.sessionToken] = session;
        return true;
    }

    std::optional<core::Session> findByToken(const std::string& token) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = sessions_.find(token);
        if (it != sessions_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::optional<std::string> findTokenBySocket(int socket) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = clientSessions_.find(socket);
        if (it != clientSessions_.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::optional<std::string> validateSession(const std::string& token) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = sessions_.find(token);
        if (it != sessions_.end()) {
            // Note: We'd need to get current timestamp to validate properly
            return it->second.userId;
        }
        return std::nullopt;
    }

    bool associateSocket(int socket, const std::string& token) override {
        std::lock_guard<std::mutex> lock(mutex_);
        clientSessions_[socket] = token;
        return true;
    }

    bool extendSession(const std::string& token, int64_t newExpiry) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = sessions_.find(token);
        if (it != sessions_.end()) {
            it->second.expiresAt = newExpiry;
            return true;
        }
        return false;
    }

    bool remove(const std::string& token) override {
        std::lock_guard<std::mutex> lock(mutex_);
        // Also remove from clientSessions
        for (auto it = clientSessions_.begin(); it != clientSessions_.end(); ) {
            if (it->second == token) {
                it = clientSessions_.erase(it);
            } else {
                ++it;
            }
        }
        return sessions_.erase(token) > 0;
    }

    bool removeBySocket(int socket) override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = clientSessions_.find(socket);
        if (it != clientSessions_.end()) {
            sessions_.erase(it->second);
            clientSessions_.erase(it);
            return true;
        }
        return false;
    }

    size_t removeExpired() override {
        // Would need current timestamp - for now return 0
        return 0;
    }

    size_t count() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return sessions_.size();
    }

    bool isValid(const std::string& token) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return sessions_.find(token) != sessions_.end();
    }

private:
    std::map<std::string, core::Session>& sessions_;
    std::map<int, std::string>& clientSessions_;
    std::mutex& mutex_;
};

/**
 * Bridge chat repository wrapping global chat messages.
 */
class BridgeChatRepository : public IChatRepository {
public:
    BridgeChatRepository(
        std::vector<core::ChatMessage>& messages,
        std::mutex& mutex)
        : messages_(messages), mutex_(mutex) {}

    bool add(const core::ChatMessage& message) override {
        std::lock_guard<std::mutex> lock(mutex_);
        messages_.push_back(message);
        return true;
    }

    std::optional<core::ChatMessage> findById(const std::string& messageId) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& msg : messages_) {
            if (msg.messageId == messageId) {
                return msg;
            }
        }
        return std::nullopt;
    }

    std::vector<core::ChatMessage> findAll() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return messages_;
    }

    std::vector<core::ChatMessage> findByUser(const std::string& userId) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<core::ChatMessage> result;
        for (const auto& msg : messages_) {
            if (msg.involvesUser(userId)) {
                result.push_back(msg);
            }
        }
        return result;
    }

    std::vector<core::ChatMessage> findConversation(const std::string& user1,
                                                     const std::string& user2) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<core::ChatMessage> result;
        for (const auto& msg : messages_) {
            if (msg.isBetween(user1, user2)) {
                result.push_back(msg);
            }
        }
        return result;
    }

    std::vector<core::ChatMessage> findUnreadFor(const std::string& userId) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<core::ChatMessage> result;
        for (const auto& msg : messages_) {
            if (msg.recipientId == userId && !msg.read) {
                result.push_back(msg);
            }
        }
        return result;
    }

    size_t countUnreadFor(const std::string& userId) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t count = 0;
        for (const auto& msg : messages_) {
            if (msg.recipientId == userId && !msg.read) {
                count++;
            }
        }
        return count;
    }

    bool markAsRead(const std::string& messageId) override {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& msg : messages_) {
            if (msg.messageId == messageId) {
                msg.read = true;
                return true;
            }
        }
        return false;
    }

    size_t markConversationAsRead(const std::string& recipientId,
                                   const std::string& senderId) override {
        std::lock_guard<std::mutex> lock(mutex_);
        size_t count = 0;
        for (auto& msg : messages_) {
            if (msg.senderId == senderId && msg.recipientId == recipientId && !msg.read) {
                msg.read = true;
                count++;
            }
        }
        return count;
    }

    bool remove(const std::string& messageId) override {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto it = messages_.begin(); it != messages_.end(); ++it) {
            if (it->messageId == messageId) {
                messages_.erase(it);
                return true;
            }
        }
        return false;
    }

    size_t count() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return messages_.size();
    }

private:
    std::vector<core::ChatMessage>& messages_;
    std::mutex& mutex_;
};

} // namespace bridge
} // namespace repository
} // namespace english_learning

#endif // ENGLISH_LEARNING_BRIDGE_REPOSITORIES_H
