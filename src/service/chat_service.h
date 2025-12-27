#ifndef ENGLISH_LEARNING_SERVICE_CHAT_SERVICE_H
#define ENGLISH_LEARNING_SERVICE_CHAT_SERVICE_H

#include "include/service/i_chat_service.h"
#include "include/repository/i_chat_repository.h"
#include "include/repository/i_user_repository.h"

namespace english_learning {
namespace service {

/**
 * Implementation of chat messaging service.
 */
class ChatService : public IChatService {
public:
    ChatService(
        repository::IChatRepository& chatRepo,
        repository::IUserRepository& userRepo);

    ~ChatService() override = default;

    ServiceResult<core::ChatMessage> sendMessage(
        const std::string& senderId,
        const std::string& receiverId,
        const std::string& content) override;

    ServiceResult<ChatHistoryResult> getChatHistory(
        const std::string& userId1,
        const std::string& userId2,
        size_t limit = 50) override;

    ServiceResult<ChatHistoryResult> getMessagesForUser(
        const std::string& userId) override;

    ServiceResult<size_t> markMessagesAsRead(
        const std::string& recipientId,
        const std::string& senderId) override;

    ServiceResult<size_t> getUnreadCount(const std::string& userId) override;

    ServiceResult<OnlineUsersResult> getOnlineUsers(
        const std::string& excludeUserId = "") override;

    VoidResult notifyMessageDelivery(
        const std::string& receiverId,
        const core::ChatMessage& message) override;

private:
    repository::IChatRepository& chatRepo_;
    repository::IUserRepository& userRepo_;
};

} // namespace service
} // namespace english_learning

#endif // ENGLISH_LEARNING_SERVICE_CHAT_SERVICE_H
