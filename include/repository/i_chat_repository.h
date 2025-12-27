#ifndef ENGLISH_LEARNING_REPOSITORY_I_CHAT_REPOSITORY_H
#define ENGLISH_LEARNING_REPOSITORY_I_CHAT_REPOSITORY_H

#include <string>
#include <vector>
#include <optional>
#include "include/core/chat_message.h"

namespace english_learning {
namespace repository {

/**
 * Interface for chat message data access operations.
 * Provides operations for ChatMessage entities.
 */
class IChatRepository {
public:
    virtual ~IChatRepository() = default;

    // Create
    virtual bool add(const core::ChatMessage& message) = 0;

    // Read
    virtual std::optional<core::ChatMessage> findById(const std::string& messageId) const = 0;
    virtual std::vector<core::ChatMessage> findAll() const = 0;
    virtual std::vector<core::ChatMessage> findByUser(const std::string& userId) const = 0;
    virtual std::vector<core::ChatMessage> findConversation(const std::string& user1,
                                                             const std::string& user2) const = 0;
    virtual std::vector<core::ChatMessage> findUnreadFor(const std::string& userId) const = 0;
    virtual size_t countUnreadFor(const std::string& userId) const = 0;

    // Update
    virtual bool markAsRead(const std::string& messageId) = 0;
    virtual size_t markConversationAsRead(const std::string& recipientId,
                                          const std::string& senderId) = 0;

    // Delete
    virtual bool remove(const std::string& messageId) = 0;

    // Utility
    virtual size_t count() const = 0;
};

} // namespace repository
} // namespace english_learning

#endif // ENGLISH_LEARNING_REPOSITORY_I_CHAT_REPOSITORY_H
