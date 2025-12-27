#ifndef ENGLISH_LEARNING_SERVICE_I_CHAT_SERVICE_H
#define ENGLISH_LEARNING_SERVICE_I_CHAT_SERVICE_H

#include <string>
#include <vector>
#include "service_result.h"
#include "include/core/chat_message.h"

namespace english_learning {
namespace service {

/**
 * DTO for chat history response.
 */
struct ChatHistoryResult {
    std::vector<core::ChatMessage> messages;
    size_t total;
    size_t unreadCount;
};

/**
 * DTO for online users response.
 */
struct OnlineUsersResult {
    struct UserInfo {
        std::string userId;
        std::string fullname;
        std::string email;
        std::string role;
    };
    std::vector<UserInfo> users;
    size_t total;
};

/**
 * Interface for chat messaging services.
 */
class IChatService {
public:
    virtual ~IChatService() = default;

    /**
     * Send a private message to another user.
     * @param senderId The sender's user ID
     * @param receiverId The receiver's user ID
     * @param content Message content
     * @return Created message on success
     */
    virtual ServiceResult<core::ChatMessage> sendMessage(
        const std::string& senderId,
        const std::string& receiverId,
        const std::string& content) = 0;

    /**
     * Get chat history between two users.
     * @param userId1 First user ID
     * @param userId2 Second user ID
     * @param limit Maximum messages to return (0 for all)
     * @return List of messages between the users
     */
    virtual ServiceResult<ChatHistoryResult> getChatHistory(
        const std::string& userId1,
        const std::string& userId2,
        size_t limit = 50) = 0;

    /**
     * Get all messages for a user.
     * @param userId The user's ID
     * @return All messages involving this user
     */
    virtual ServiceResult<ChatHistoryResult> getMessagesForUser(
        const std::string& userId) = 0;

    /**
     * Mark messages as read.
     * @param recipientId The user marking messages as read
     * @param senderId The sender whose messages are being marked
     * @return Number of messages marked as read
     */
    virtual ServiceResult<size_t> markMessagesAsRead(
        const std::string& recipientId,
        const std::string& senderId) = 0;

    /**
     * Get count of unread messages for a user.
     * @param userId The user's ID
     * @return Count of unread messages
     */
    virtual ServiceResult<size_t> getUnreadCount(const std::string& userId) = 0;

    /**
     * Get list of online users.
     * @param excludeUserId User ID to exclude from list (typically the requester)
     * @return List of online users
     */
    virtual ServiceResult<OnlineUsersResult> getOnlineUsers(
        const std::string& excludeUserId = "") = 0;

    /**
     * Notify that a message was delivered in real-time.
     * @param receiverId The receiver's user ID
     * @param message The message to deliver
     * @return Success if receiver is online and message sent
     */
    virtual VoidResult notifyMessageDelivery(
        const std::string& receiverId,
        const core::ChatMessage& message) = 0;
};

} // namespace service
} // namespace english_learning

#endif // ENGLISH_LEARNING_SERVICE_I_CHAT_SERVICE_H
