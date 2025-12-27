#ifndef ENGLISH_LEARNING_CORE_USER_H
#define ENGLISH_LEARNING_CORE_USER_H

#include <string>
#include "types.h"

namespace english_learning {
namespace core {

/**
 * User entity representing a registered user in the system.
 * Users can be students, teachers, or administrators.
 */
struct User {
    std::string userId;
    std::string fullname;
    std::string email;
    std::string password;       // TODO: Should be hashed in production
    std::string level;          // beginner, intermediate, advanced
    std::string role;           // student, teacher, admin
    Timestamp createdAt;
    bool online;
    int clientSocket;           // Active socket descriptor (-1 if not connected)

    User() : createdAt(0), online(false), clientSocket(-1) {}

    User(const std::string& id, const std::string& name, const std::string& mail,
         const std::string& pwd, const std::string& lvl, const std::string& r)
        : userId(id), fullname(name), email(mail), password(pwd),
          level(lvl), role(r), createdAt(0), online(false), clientSocket(-1) {}

    // Check if user is a teacher
    bool isTeacher() const {
        return role == "teacher" || role == "admin";
    }

    // Check if user is an admin
    bool isAdmin() const {
        return role == "admin";
    }

    // Check if user is a student
    bool isStudent() const {
        return role == "student";
    }
};

} // namespace core
} // namespace english_learning

#endif // ENGLISH_LEARNING_CORE_USER_H
