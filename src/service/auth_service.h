#ifndef ENGLISH_LEARNING_SERVICE_AUTH_SERVICE_H
#define ENGLISH_LEARNING_SERVICE_AUTH_SERVICE_H

#include "include/service/i_auth_service.h"
#include "include/repository/i_user_repository.h"
#include "include/repository/i_session_repository.h"
#include "include/repository/i_chat_repository.h"

namespace english_learning {
namespace service {

/**
 * Implementation of authentication service.
 * Handles user registration, login, session management.
 */
class AuthService : public IAuthService {
public:
    /**
     * Constructor with dependency injection.
     * @param userRepo User repository
     * @param sessionRepo Session repository
     * @param chatRepo Chat repository (for unread count)
     */
    AuthService(
        repository::IUserRepository& userRepo,
        repository::ISessionRepository& sessionRepo,
        repository::IChatRepository& chatRepo);

    ~AuthService() override = default;

    ServiceResult<RegisterResult> registerUser(
        const std::string& fullname,
        const std::string& email,
        const std::string& password,
        const std::string& role = "student") override;

    ServiceResult<LoginResult> login(
        const std::string& email,
        const std::string& password,
        int clientSocket) override;

    ServiceResult<std::string> validateSession(const std::string& sessionToken) override;

    VoidResult logout(const std::string& sessionToken) override;

    VoidResult setUserLevel(const std::string& userId, const std::string& level) override;

    VoidResult handleDisconnect(int clientSocket) override;

    bool isTeacher(const std::string& userId) override;

    bool isAdmin(const std::string& userId) override;

    ServiceResult<core::User> getUserById(const std::string& userId) override;

private:
    repository::IUserRepository& userRepo_;
    repository::ISessionRepository& sessionRepo_;
    repository::IChatRepository& chatRepo_;
};

} // namespace service
} // namespace english_learning

#endif // ENGLISH_LEARNING_SERVICE_AUTH_SERVICE_H
