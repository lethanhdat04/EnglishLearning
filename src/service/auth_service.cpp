#include "auth_service.h"
#include "include/protocol/utils.h"

namespace english_learning {
namespace service {

AuthService::AuthService(
    repository::IUserRepository& userRepo,
    repository::ISessionRepository& sessionRepo,
    repository::IChatRepository& chatRepo)
    : userRepo_(userRepo)
    , sessionRepo_(sessionRepo)
    , chatRepo_(chatRepo) {}

ServiceResult<RegisterResult> AuthService::registerUser(
    const std::string& fullname,
    const std::string& email,
    const std::string& password,
    const std::string& role) {

    // Validate input
    if (fullname.empty()) {
        return ServiceResult<RegisterResult>::error("Full name is required");
    }
    if (email.empty()) {
        return ServiceResult<RegisterResult>::error("Email is required");
    }
    if (password.empty()) {
        return ServiceResult<RegisterResult>::error("Password is required");
    }

    // Check if email already exists
    if (userRepo_.findByEmail(email).has_value()) {
        return ServiceResult<RegisterResult>::error("Email already registered");
    }

    // Create new user
    core::User user;
    user.userId = protocol::utils::generateId("user");
    user.fullname = fullname;
    user.email = email;
    user.password = password;  // TODO: Hash password in production
    user.role = role.empty() ? "student" : role;
    user.level = "beginner";
    user.createdAt = protocol::utils::getCurrentTimestamp();
    user.online = false;
    user.clientSocket = -1;

    if (!userRepo_.add(user)) {
        return ServiceResult<RegisterResult>::error("Failed to create user");
    }

    RegisterResult result;
    result.userId = user.userId;
    result.email = user.email;
    result.fullname = user.fullname;

    return ServiceResult<RegisterResult>::success(result);
}

ServiceResult<LoginResult> AuthService::login(
    const std::string& email,
    const std::string& password,
    int clientSocket) {

    // Find user by email
    auto userOpt = userRepo_.findByEmail(email);
    if (!userOpt.has_value()) {
        return ServiceResult<LoginResult>::error("Invalid email or password");
    }

    core::User user = userOpt.value();

    // Verify password
    if (user.password != password) {  // TODO: Compare hashed passwords
        return ServiceResult<LoginResult>::error("Invalid email or password");
    }

    // Create session
    long long now = protocol::utils::getCurrentTimestamp();
    long long expiresAt = now + (24 * 60 * 60 * 1000);  // 24 hours

    core::Session session;
    session.sessionToken = protocol::utils::generateSessionToken();
    session.userId = user.userId;
    session.expiresAt = expiresAt;

    if (!sessionRepo_.add(session)) {
        return ServiceResult<LoginResult>::error("Failed to create session");
    }

    // Associate socket with session
    sessionRepo_.associateSocket(clientSocket, session.sessionToken);

    // Update user online status
    userRepo_.setOnlineStatus(user.userId, true, clientSocket);

    // Get unread message count
    size_t unreadCount = chatRepo_.countUnreadFor(user.userId);

    LoginResult result;
    result.sessionToken = session.sessionToken;
    result.userId = user.userId;
    result.fullname = user.fullname;
    result.email = user.email;
    result.level = user.level;
    result.role = user.role;
    result.expiresAt = session.expiresAt;
    result.unreadMessages = unreadCount;

    return ServiceResult<LoginResult>::success(result);
}

ServiceResult<std::string> AuthService::validateSession(const std::string& sessionToken) {
    auto sessionOpt = sessionRepo_.findByToken(sessionToken);
    if (!sessionOpt.has_value()) {
        return ServiceResult<std::string>::error("Invalid session token");
    }

    const core::Session& session = sessionOpt.value();
    long long now = protocol::utils::getCurrentTimestamp();
    if (!session.isValid(now)) {
        return ServiceResult<std::string>::error("Session expired or inactive");
    }

    return ServiceResult<std::string>::success(session.userId);
}

VoidResult AuthService::logout(const std::string& sessionToken) {
    auto sessionOpt = sessionRepo_.findByToken(sessionToken);
    if (!sessionOpt.has_value()) {
        return VoidResult::error("Invalid session token");
    }

    const core::Session& session = sessionOpt.value();

    // Update user online status
    userRepo_.setOnlineStatus(session.userId, false);

    // Remove session
    if (!sessionRepo_.remove(sessionToken)) {
        return VoidResult::error("Failed to invalidate session");
    }

    return VoidResult::success();
}

VoidResult AuthService::setUserLevel(const std::string& userId, const std::string& level) {
    auto userOpt = userRepo_.findById(userId);
    if (!userOpt.has_value()) {
        return VoidResult::error("User not found");
    }

    if (!userRepo_.updateLevel(userId, level)) {
        return VoidResult::error("Failed to update user level");
    }

    return VoidResult::success();
}

VoidResult AuthService::handleDisconnect(int clientSocket) {
    // Find session by socket and remove it
    auto tokenOpt = sessionRepo_.findTokenBySocket(clientSocket);
    if (tokenOpt.has_value()) {
        auto sessionOpt = sessionRepo_.findByToken(tokenOpt.value());
        if (sessionOpt.has_value()) {
            userRepo_.setOnlineStatus(sessionOpt.value().userId, false);
        }
        sessionRepo_.removeBySocket(clientSocket);
    }

    return VoidResult::success();
}

bool AuthService::isTeacher(const std::string& userId) {
    return userRepo_.isTeacher(userId);
}

bool AuthService::isAdmin(const std::string& userId) {
    return userRepo_.isAdmin(userId);
}

ServiceResult<core::User> AuthService::getUserById(const std::string& userId) {
    auto userOpt = userRepo_.findById(userId);
    if (!userOpt.has_value()) {
        return ServiceResult<core::User>::error("User not found");
    }
    return ServiceResult<core::User>::success(userOpt.value());
}

} // namespace service
} // namespace english_learning
