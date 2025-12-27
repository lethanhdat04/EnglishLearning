#include "memory_user_repository.h"

namespace english_learning {
namespace repository {
namespace memory {

bool MemoryUserRepository::add(const core::User& user) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (usersByEmail_.find(user.email) != usersByEmail_.end()) {
        return false; // Email already exists
    }
    usersByEmail_[user.email] = user;
    userIdToEmail_[user.userId] = user.email;
    return true;
}

std::optional<core::User> MemoryUserRepository::findByEmail(const std::string& email) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = usersByEmail_.find(email);
    if (it != usersByEmail_.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<core::User> MemoryUserRepository::findById(const std::string& userId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto emailIt = userIdToEmail_.find(userId);
    if (emailIt != userIdToEmail_.end()) {
        auto userIt = usersByEmail_.find(emailIt->second);
        if (userIt != usersByEmail_.end()) {
            return userIt->second;
        }
    }
    return std::nullopt;
}

std::vector<core::User> MemoryUserRepository::findAll() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<core::User> result;
    result.reserve(usersByEmail_.size());
    for (const auto& pair : usersByEmail_) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<core::User> MemoryUserRepository::findOnlineUsers() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<core::User> result;
    for (const auto& pair : usersByEmail_) {
        if (pair.second.online) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<core::User> MemoryUserRepository::findByRole(const std::string& role) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<core::User> result;
    for (const auto& pair : usersByEmail_) {
        if (pair.second.role == role) {
            result.push_back(pair.second);
        }
    }
    return result;
}

bool MemoryUserRepository::exists(const std::string& email) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return usersByEmail_.find(email) != usersByEmail_.end();
}

bool MemoryUserRepository::existsById(const std::string& userId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return userIdToEmail_.find(userId) != userIdToEmail_.end();
}

bool MemoryUserRepository::update(const core::User& user) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = usersByEmail_.find(user.email);
    if (it != usersByEmail_.end()) {
        it->second = user;
        return true;
    }
    return false;
}

bool MemoryUserRepository::updateLevel(const std::string& userId, const std::string& level) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto emailIt = userIdToEmail_.find(userId);
    if (emailIt != userIdToEmail_.end()) {
        auto userIt = usersByEmail_.find(emailIt->second);
        if (userIt != usersByEmail_.end()) {
            userIt->second.level = level;
            return true;
        }
    }
    return false;
}

bool MemoryUserRepository::setOnlineStatus(const std::string& userId, bool online, int socket) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto emailIt = userIdToEmail_.find(userId);
    if (emailIt != userIdToEmail_.end()) {
        auto userIt = usersByEmail_.find(emailIt->second);
        if (userIt != usersByEmail_.end()) {
            userIt->second.online = online;
            userIt->second.clientSocket = socket;
            return true;
        }
    }
    return false;
}

bool MemoryUserRepository::remove(const std::string& userId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto emailIt = userIdToEmail_.find(userId);
    if (emailIt != userIdToEmail_.end()) {
        usersByEmail_.erase(emailIt->second);
        userIdToEmail_.erase(emailIt);
        return true;
    }
    return false;
}

size_t MemoryUserRepository::count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return usersByEmail_.size();
}

bool MemoryUserRepository::isTeacher(const std::string& userId) const {
    auto user = findById(userId);
    if (user) {
        return user->role == "teacher" || user->role == "admin";
    }
    return false;
}

bool MemoryUserRepository::isAdmin(const std::string& userId) const {
    auto user = findById(userId);
    if (user) {
        return user->role == "admin";
    }
    return false;
}

core::User* MemoryUserRepository::getPointerById(const std::string& userId) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto emailIt = userIdToEmail_.find(userId);
    if (emailIt != userIdToEmail_.end()) {
        auto userIt = usersByEmail_.find(emailIt->second);
        if (userIt != usersByEmail_.end()) {
            return &userIt->second;
        }
    }
    return nullptr;
}

} // namespace memory
} // namespace repository
} // namespace english_learning
