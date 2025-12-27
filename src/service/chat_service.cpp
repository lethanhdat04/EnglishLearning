#include "chat_service.h"
#include "include/protocol/utils.h"

namespace english_learning {
namespace service {

ChatService::ChatService(
    repository::IChatRepository& chatRepo,
    repository::IUserRepository& userRepo)
    : chatRepo_(chatRepo)
    , userRepo_(userRepo) {}

ServiceResult<core::ChatMessage> ChatService::sendMessage(
    const std::string& senderId,
    const std::string& receiverId,
    const std::string& content) {

    // Validate sender exists
    auto senderOpt = userRepo_.findById(senderId);
    if (!senderOpt.has_value()) {
        return ServiceResult<core::ChatMessage>::error("Sender not found");
    }

    // Validate receiver exists
    auto receiverOpt = userRepo_.findById(receiverId);
    if (!receiverOpt.has_value()) {
        return ServiceResult<core::ChatMessage>::error("Receiver not found");
    }

    // Validate content
    if (content.empty()) {
        return ServiceResult<core::ChatMessage>::error("Message content is required");
    }

    core::ChatMessage message;
    message.messageId = protocol::utils::generateId("msg");
    message.senderId = senderId;
    message.recipientId = receiverId;
    message.content = content;
    message.timestamp = protocol::utils::getCurrentTimestamp();
    message.read = false;

    if (!chatRepo_.add(message)) {
        return ServiceResult<core::ChatMessage>::error("Failed to send message");
    }

    return ServiceResult<core::ChatMessage>::success(message);
}

ServiceResult<ChatHistoryResult> ChatService::getChatHistory(
    const std::string& userId1,
    const std::string& userId2,
    size_t limit) {

    auto messages = chatRepo_.findConversation(userId1, userId2);

    // Apply limit if specified
    if (limit > 0 && messages.size() > limit) {
        messages.resize(limit);
    }

    // Count unread for userId1
    size_t unreadCount = 0;
    for (const auto& msg : messages) {
        if (msg.recipientId == userId1 && !msg.read) {
            unreadCount++;
        }
    }

    ChatHistoryResult result;
    result.messages = messages;
    result.total = messages.size();
    result.unreadCount = unreadCount;

    return ServiceResult<ChatHistoryResult>::success(result);
}

ServiceResult<ChatHistoryResult> ChatService::getMessagesForUser(
    const std::string& userId) {

    auto messages = chatRepo_.findByUser(userId);
    size_t unreadCount = chatRepo_.countUnreadFor(userId);

    ChatHistoryResult result;
    result.messages = messages;
    result.total = messages.size();
    result.unreadCount = unreadCount;

    return ServiceResult<ChatHistoryResult>::success(result);
}

ServiceResult<size_t> ChatService::markMessagesAsRead(
    const std::string& recipientId,
    const std::string& senderId) {

    size_t count = chatRepo_.markConversationAsRead(recipientId, senderId);
    return ServiceResult<size_t>::success(count);
}

ServiceResult<size_t> ChatService::getUnreadCount(const std::string& userId) {
    size_t count = chatRepo_.countUnreadFor(userId);
    return ServiceResult<size_t>::success(count);
}

ServiceResult<OnlineUsersResult> ChatService::getOnlineUsers(
    const std::string& excludeUserId) {

    auto users = userRepo_.findOnlineUsers();

    OnlineUsersResult result;
    for (const auto& user : users) {
        if (user.userId != excludeUserId) {
            OnlineUsersResult::UserInfo info;
            info.userId = user.userId;
            info.fullname = user.fullname;
            info.email = user.email;
            info.role = user.role;
            result.users.push_back(info);
        }
    }
    result.total = result.users.size();

    return ServiceResult<OnlineUsersResult>::success(result);
}

VoidResult ChatService::notifyMessageDelivery(
    const std::string& receiverId,
    const core::ChatMessage& /* message */) {

    // Check if receiver is online
    auto userOpt = userRepo_.findById(receiverId);
    if (!userOpt.has_value()) {
        return VoidResult::error("Receiver not found");
    }

    const core::User& user = userOpt.value();
    if (!user.online || user.clientSocket < 0) {
        // User is offline, message will be delivered when they come online
        return VoidResult::successWithMessage("User offline, message queued");
    }

    // In the actual implementation, this would send over the socket
    // For now, we just indicate success
    return VoidResult::success();
}

} // namespace service
} // namespace english_learning
