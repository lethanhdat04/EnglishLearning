#ifndef ENGLISH_LEARNING_CORE_CHAT_MESSAGE_H
#define ENGLISH_LEARNING_CORE_CHAT_MESSAGE_H

#include <string>
#include "types.h"

namespace english_learning {
namespace core {

/**
 * ChatMessage entity representing a message between two users.
 * Messages track sender, recipient, content, timestamp, and read status.
 */
struct ChatMessage {
    std::string messageId;
    std::string senderId;
    std::string recipientId;
    std::string content;
    Timestamp timestamp;
    bool read;

    ChatMessage() : timestamp(0), read(false) {}

    ChatMessage(const std::string& id, const std::string& sender,
                const std::string& recipient, const std::string& msg,
                Timestamp ts)
        : messageId(id), senderId(sender), recipientId(recipient),
          content(msg), timestamp(ts), read(false) {}

    // Check if message involves a specific user (as sender or recipient)
    bool involvesUser(const std::string& userId) const {
        return senderId == userId || recipientId == userId;
    }

    // Check if message is between two specific users
    bool isBetween(const std::string& user1, const std::string& user2) const {
        return (senderId == user1 && recipientId == user2) ||
               (senderId == user2 && recipientId == user1);
    }

    // Check if message is from a specific sender to a specific recipient
    bool isFromTo(const std::string& from, const std::string& to) const {
        return senderId == from && recipientId == to;
    }

    // Mark message as read
    void markAsRead() {
        read = true;
    }

    // Check if message is unread
    bool isUnread() const {
        return !read;
    }
};

} // namespace core
} // namespace english_learning

#endif // ENGLISH_LEARNING_CORE_CHAT_MESSAGE_H
