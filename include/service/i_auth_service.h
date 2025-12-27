#ifndef ENGLISH_LEARNING_SERVICE_I_AUTH_SERVICE_H
#define ENGLISH_LEARNING_SERVICE_I_AUTH_SERVICE_H

#include <string>
#include "service_result.h"
#include "include/core/user.h"

namespace english_learning {
namespace service {

/**
 * Data transfer object for login response.
 */
struct LoginResult {
    std::string sessionToken;
    std::string userId;
    std::string fullname;
    std::string email;
    std::string level;
    std::string role;
    int64_t expiresAt;
    size_t unreadMessages;
};

/**
 * Data transfer object for registration response.
 */
struct RegisterResult {
    std::string userId;
    std::string email;
    std::string fullname;
};

/**
 * Interface for authentication and user management services.
 */
class IAuthService {
public:
    virtual ~IAuthService() = default;

    /**
     * Register a new user.
     * @param fullname User's full name
     * @param email User's email (must be unique)
     * @param password User's password
     * @param role User's role (student, teacher, admin)
     * @return RegisterResult on success, error message on failure
     */
    virtual ServiceResult<RegisterResult> registerUser(
        const std::string& fullname,
        const std::string& email,
        const std::string& password,
        const std::string& role = "student") = 0;

    /**
     * Authenticate a user and create a session.
     * @param email User's email
     * @param password User's password
     * @param clientSocket Socket for the client connection
     * @return LoginResult on success, error message on failure
     */
    virtual ServiceResult<LoginResult> login(
        const std::string& email,
        const std::string& password,
        int clientSocket) = 0;

    /**
     * Validate a session token and return the associated user ID.
     * @param sessionToken The token to validate
     * @return User ID if valid, error if invalid or expired
     */
    virtual ServiceResult<std::string> validateSession(const std::string& sessionToken) = 0;

    /**
     * Logout a user and invalidate their session.
     * @param sessionToken The session to invalidate
     * @return Success or error
     */
    virtual VoidResult logout(const std::string& sessionToken) = 0;

    /**
     * Update user's learning level.
     * @param userId The user's ID
     * @param level New level (beginner, intermediate, advanced)
     * @return Success or error
     */
    virtual VoidResult setUserLevel(const std::string& userId, const std::string& level) = 0;

    /**
     * Handle client disconnection cleanup.
     * @param clientSocket The socket that disconnected
     * @return Success or error
     */
    virtual VoidResult handleDisconnect(int clientSocket) = 0;

    /**
     * Check if a user is a teacher or admin.
     * @param userId The user's ID
     * @return true if teacher or admin
     */
    virtual bool isTeacher(const std::string& userId) = 0;

    /**
     * Check if a user is an admin.
     * @param userId The user's ID
     * @return true if admin
     */
    virtual bool isAdmin(const std::string& userId) = 0;

    /**
     * Get user by ID.
     * @param userId The user's ID
     * @return User if found, error otherwise
     */
    virtual ServiceResult<core::User> getUserById(const std::string& userId) = 0;
};

} // namespace service
} // namespace english_learning

#endif // ENGLISH_LEARNING_SERVICE_I_AUTH_SERVICE_H
