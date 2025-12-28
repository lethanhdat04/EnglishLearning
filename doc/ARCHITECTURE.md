# English Learning Application - Architecture Documentation

## Overview

This document describes the layered architecture implemented through a 6-phase refactoring of the English Learning Application. The architecture follows Clean Architecture principles, enabling testability, maintainability, and gradual migration.

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           PRESENTATION LAYER                                 │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │  server.cpp handlers: handleLogin, handleGetLessons, handleChat, etc.   ││
│  │  client.cpp: CLI interface                                               ││
│  │  gui_main.cpp: GTK GUI interface                                         ││
│  └─────────────────────────────────────────────────────────────────────────┘│
└───────────────────────────────────┬─────────────────────────────────────────┘
                                    │ uses
                                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                             SERVICE LAYER                                    │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │  ServiceContainer: Dependency injection container                        ││
│  │  ├── AuthService: Login, register, session management                   ││
│  │  ├── LessonService: Lesson CRUD, filtering                              ││
│  │  ├── TestService: Test management, submissions                          ││
│  │  ├── ChatService: Messaging, online users                               ││
│  │  ├── ExerciseService: Exercises, teacher review, feedback viewer        ││
│  │  ├── GameService: Games, sessions, picture matching                     ││
│  │  └── VoiceCallService: Voice calls (initiate, accept, reject, end)      ││
│  └─────────────────────────────────────────────────────────────────────────┘│
└───────────────────────────────────┬─────────────────────────────────────────┘
                                    │ depends on
                                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                           REPOSITORY LAYER                                   │
│  ┌───────────────────────────────┐ ┌───────────────────────────────────────┐│
│  │     Interface (Contracts)      │ │       Implementations                 ││
│  │  ├── IUserRepository          │ │  ├── BridgeUserRepository (globals)   ││
│  │  ├── ISessionRepository       │ │  ├── BridgeSessionRepository          ││
│  │  ├── ILessonRepository        │ │  ├── BridgeLessonRepository           ││
│  │  ├── ITestRepository          │ │  ├── BridgeVoiceCallRepository        ││
│  │  ├── IChatRepository          │ │  ├── MemoryUserRepository (standalone)││
│  │  ├── IExerciseRepository      │ │  └── ... more implementations         ││
│  │  ├── IGameRepository          │ │                                       ││
│  │  └── IVoiceCallRepository     │ │                                       ││
│  └───────────────────────────────┘ └───────────────────────────────────────┘│
└───────────────────────────────────┬─────────────────────────────────────────┘
                                    │ uses
                                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                           PROTOCOL LAYER                                     │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │  MessageType: Protocol message type constants                            ││
│  │  JsonParser: JSON parsing utilities                                      ││
│  │  JsonBuilder: JSON construction utilities                                ││
│  │  Utils: ID generation, timestamps, session tokens                        ││
│  └─────────────────────────────────────────────────────────────────────────┘│
└───────────────────────────────────┬─────────────────────────────────────────┘
                                    │ uses
                                    ▼
┌─────────────────────────────────────────────────────────────────────────────┐
│                              CORE LAYER                                      │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │  Domain Models (POD structs with helper methods)                         ││
│  │  ├── User: userId, email, password, role, level, online status          ││
│  │  ├── Session: sessionToken, userId, expiresAt                           ││
│  │  ├── Lesson: lessonId, title, content, level, topic                     ││
│  │  ├── Test/TestQuestion: testId, questions, answers                      ││
│  │  ├── ChatMessage: messageId, senderId, recipientId, content             ││
│  │  ├── Exercise/ExerciseSubmission: exercises with teacher feedback       ││
│  │  ├── Game/GameSession: matching games with picture support              ││
│  │  └── VoiceCall: callId, callerId, calleeId, status, timestamps          ││
│  └─────────────────────────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## Phase 1: Core Domain Models

**Goal**: Extract domain models from server.cpp into reusable, well-documented header files.

### Files Created

| File | Description |
|------|-------------|
| `include/core/types.h` | Common type definitions and constants |
| `include/core/user.h` | User struct with role/level helpers |
| `include/core/session.h` | Session struct with validation |
| `include/core/lesson.h` | Lesson struct |
| `include/core/test.h` | Test and TestQuestion structs |
| `include/core/chat_message.h` | ChatMessage struct |
| `include/core/exercise.h` | Exercise and ExerciseSubmission structs |
| `include/core/game.h` | Game and GameSession structs with PicturePair |
| `include/core/voice_call.h` | VoiceCall struct with status tracking |
| `include/core/all.h` | Convenience header including all models |

### Key Design Decisions

1. **POD-like structs**: Models are simple data containers with helper methods
2. **No dependencies**: Core layer has no external dependencies
3. **Backward compatibility**: Type aliases in server.cpp maintain compatibility

### Example: User Model

```cpp
namespace english_learning::core {

struct User {
    std::string userId;
    std::string fullname;
    std::string email;
    std::string password;
    std::string role;      // "student", "teacher", "admin"
    std::string level;     // "beginner", "intermediate", "advanced"
    int64_t createdAt;
    bool online;
    int clientSocket;

    // Helper methods
    bool isTeacher() const { return role == "teacher" || role == "admin"; }
    bool isAdmin() const { return role == "admin"; }
    bool isStudent() const { return role == "student"; }
};

} // namespace english_learning::core
```

---

## Phase 2: Protocol Layer

**Goal**: Extract JSON parsing, message types, and utility functions into a dedicated layer.

### Files Created

| File | Description |
|------|-------------|
| `include/protocol/message_types.h` | Protocol message type constants |
| `include/protocol/json_parser.h` | JSON parsing utilities |
| `include/protocol/json_builder.h` | JSON construction utilities |
| `include/protocol/utils.h` | ID generation, timestamps |
| `include/protocol/all.h` | Convenience header |
| `src/protocol/json_parser.cpp` | Implementation |

### Key Features

1. **Message Types**: Centralized constants for all protocol messages
2. **JSON Parser**: Lightweight custom JSON parsing (no external deps)
3. **Utilities**: Thread-safe ID and token generation

### Example: Message Types

```cpp
namespace english_learning::protocol::MessageType {
    constexpr const char* LOGIN_REQUEST = "LOGIN_REQUEST";
    constexpr const char* LOGIN_RESPONSE = "LOGIN_RESPONSE";
    constexpr const char* GET_LESSONS_REQUEST = "GET_LESSONS_REQUEST";
    // ... more message types
}
```

---

## Phase 3: Repository Layer

**Goal**: Define data access interfaces following the Repository pattern.

### Files Created

| File | Description |
|------|-------------|
| `include/repository/i_user_repository.h` | User data access interface |
| `include/repository/i_session_repository.h` | Session data access interface |
| `include/repository/i_lesson_repository.h` | Lesson data access interface |
| `include/repository/i_test_repository.h` | Test data access interface |
| `include/repository/i_chat_repository.h` | Chat message data access interface |
| `include/repository/i_exercise_repository.h` | Exercise data access interface |
| `include/repository/i_game_repository.h` | Game data access interface |
| `include/repository/i_voice_call_repository.h` | Voice call data access interface |
| `include/repository/all.h` | Convenience header |
| `src/repository/memory/*.cpp` | In-memory implementations |

### Key Design Decisions

1. **Interface segregation**: Each entity has its own repository interface
2. **Optional returns**: Uses `std::optional<T>` for nullable results
3. **Thread-safe contracts**: Implementations expected to handle concurrency

### Example: IUserRepository

```cpp
class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    virtual bool add(const core::User& user) = 0;
    virtual std::optional<core::User> findByEmail(const std::string& email) const = 0;
    virtual std::optional<core::User> findById(const std::string& userId) const = 0;
    virtual std::vector<core::User> findOnlineUsers() const = 0;
    virtual bool update(const core::User& user) = 0;
    virtual bool setOnlineStatus(const std::string& userId, bool online, int socket = -1) = 0;
    // ... more methods
};
```

---

## Phase 4: Service Layer

**Goal**: Create business logic services that encapsulate operations and use repositories.

### Files Created

| File | Description |
|------|-------------|
| `include/service/service_result.h` | Generic result type for operations |
| `include/service/i_auth_service.h` | Authentication service interface |
| `include/service/i_lesson_service.h` | Lesson service interface |
| `include/service/i_test_service.h` | Test service interface |
| `include/service/i_chat_service.h` | Chat service interface |
| `include/service/i_exercise_service.h` | Exercise service interface |
| `include/service/i_game_service.h` | Game service interface |
| `include/service/i_voice_call_service.h` | Voice call service interface |
| `include/service/all.h` | Convenience header |
| `src/service/*.cpp` | Service implementations |
| `src/service/voice_call_service.cpp` | Voice call service implementation |
| `src/service/service_container.h` | Dependency injection container |

### ServiceResult Pattern

```cpp
template<typename T>
class ServiceResult {
public:
    static ServiceResult success(const T& data);
    static ServiceResult error(const std::string& message);

    bool isSuccess() const;
    const T& getData() const;
    const std::string& getMessage() const;
};
```

### Example: AuthService

```cpp
class AuthService : public IAuthService {
public:
    AuthService(IUserRepository& userRepo,
                ISessionRepository& sessionRepo,
                IChatRepository& chatRepo);

    ServiceResult<LoginResult> login(
        const std::string& email,
        const std::string& password,
        int clientSocket) override;

    // Business logic encapsulated:
    // - Validates credentials
    // - Creates session
    // - Updates online status
    // - Returns structured result
};
```

---

## Phase 5: Integration (Bridge Pattern)

**Goal**: Integrate service layer with existing server.cpp using bridge repositories.

### Files Created

| File | Description |
|------|-------------|
| `src/repository/bridge/bridge_repositories.h` | User, Session, Chat bridges |
| `src/repository/bridge/bridge_repositories_ext.h` | Lesson, Test, Exercise, Game, VoiceCall bridges |

### Bridge Pattern

Bridge repositories wrap existing global data structures, allowing gradual migration:

```cpp
class BridgeUserRepository : public IUserRepository {
public:
    BridgeUserRepository(
        std::map<std::string, core::User>& users,    // Wraps global
        std::map<std::string, core::User*>& userById,
        std::mutex& mutex)
        : users_(users), userById_(userById), mutex_(mutex) {}

    // Implements IUserRepository by delegating to wrapped globals
    std::optional<core::User> findByEmail(const std::string& email) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = users_.find(email);
        return it != users_.end() ? std::optional(it->second) : std::nullopt;
    }
};
```

### Server Integration

```cpp
// In main():
static bridge::BridgeUserRepository userRepo(users, userById, usersMutex);
static bridge::BridgeSessionRepository sessionRepo(sessions, clientSessions, sessionsMutex);
static bridge::BridgeVoiceCallRepository voiceCallRepo(voiceCalls, voiceCallsMutex);
// ... more bridge repos

serviceContainer = std::make_unique<service::ServiceContainer>(
    userRepo, sessionRepo, lessonRepo, testRepo, chatRepo, exerciseRepo, gameRepo, voiceCallRepo);
```

---

## Phase 6: Handler Migration Demo

**Goal**: Demonstrate how handlers can be migrated to use the service layer.

### Example: handleLoginV2

```cpp
// OLD: Direct global access
std::string handleLogin(const std::string& json, int clientSocket) {
    std::lock_guard<std::mutex> lock(usersMutex);
    auto it = users.find(email);
    // ... manual session creation, user updates
}

// NEW: Service layer usage
std::string handleLoginV2(const std::string& json, int clientSocket) {
    auto result = serviceContainer->auth().login(email, password, clientSocket);

    if (!result.isSuccess()) {
        // Build error response from result.getMessage()
    } else {
        // Build success response from result.getData()
    }
}
```

### Benefits of Service Layer

1. **Encapsulated business logic**: Auth rules in AuthService, not handlers
2. **Testability**: Services can be unit tested with mock repositories
3. **Consistency**: All handlers use same patterns
4. **Thread safety**: Handled by bridge repositories

---

## Directory Structure

```
EnglishLearning/
├── include/
│   ├── core/                    # Phase 1: Domain models
│   │   ├── types.h
│   │   ├── user.h
│   │   ├── session.h
│   │   ├── lesson.h
│   │   ├── test.h
│   │   ├── chat_message.h
│   │   ├── exercise.h
│   │   ├── game.h              # Includes PicturePair, ImageSourceType
│   │   ├── voice_call.h
│   │   └── all.h
│   ├── protocol/                # Phase 2: Protocol layer
│   │   ├── message_types.h
│   │   ├── json_parser.h
│   │   ├── json_builder.h
│   │   ├── utils.h
│   │   └── all.h
│   ├── repository/              # Phase 3: Repository interfaces
│   │   ├── i_user_repository.h
│   │   ├── i_session_repository.h
│   │   ├── i_lesson_repository.h
│   │   ├── i_test_repository.h
│   │   ├── i_chat_repository.h
│   │   ├── i_exercise_repository.h
│   │   ├── i_game_repository.h
│   │   ├── i_voice_call_repository.h
│   │   └── all.h
│   └── service/                 # Phase 4: Service interfaces
│       ├── service_result.h
│       ├── i_auth_service.h
│       ├── i_lesson_service.h
│       ├── i_test_service.h
│       ├── i_chat_service.h
│       ├── i_exercise_service.h
│       ├── i_game_service.h
│       ├── i_voice_call_service.h
│       └── all.h
├── src/
│   ├── protocol/                # Phase 2: Implementations
│   │   └── json_parser.cpp
│   ├── repository/
│   │   ├── memory/              # Phase 3: Memory implementations
│   │   │   ├── memory_user_repository.cpp
│   │   │   ├── memory_session_repository.cpp
│   │   │   └── memory_repositories.cpp
│   │   └── bridge/              # Phase 5: Bridge repositories
│   │       ├── bridge_repositories.h
│   │       └── bridge_repositories_ext.h
│   └── service/                 # Phase 4: Service implementations
│       ├── auth_service.h
│       ├── auth_service.cpp
│       ├── lesson_service.h
│       ├── lesson_service.cpp
│       ├── test_service.h
│       ├── test_service.cpp
│       ├── chat_service.h
│       ├── chat_service.cpp
│       ├── exercise_service.h
│       ├── exercise_service.cpp
│       ├── game_service.h
│       ├── game_service.cpp
│       ├── voice_call_service.h
│       ├── voice_call_service.cpp
│       ├── service_container.h
│       └── all.h
├── doc/
│   └── ARCHITECTURE.md          # This document
├── server.cpp                   # Server with service integration
├── client.cpp                   # CLI client
├── gui_main.cpp                 # GTK GUI client
└── Makefile                     # Build configuration
```

---

## Migration Guide

### Migrating a Handler to Service Layer

1. **Identify the service**: Which service handles this operation?
2. **Parse request**: Extract data from JSON payload
3. **Call service**: Use `serviceContainer->serviceName().method()`
4. **Handle result**: Check `isSuccess()`, use `getData()` or `getMessage()`
5. **Build response**: Format JSON response from service result

### Example Migration

```cpp
// Before: ~50 lines with locks and direct data access
std::string handleSomeRequest(const std::string& json) {
    std::lock_guard<std::mutex> lock(someMutex);
    // ... manual data manipulation
}

// After: ~20 lines, cleaner separation
std::string handleSomeRequestV2(const std::string& json) {
    auto result = serviceContainer->someService().someMethod(params);
    return result.isSuccess()
        ? buildSuccessResponse(result.getData())
        : buildErrorResponse(result.getMessage());
}
```

---

## Future Improvements

1. **Complete handler migration**: Migrate all handlers to use services
2. **Database persistence**: Implement database-backed repositories
3. **Unit tests**: Add tests for services with mock repositories
4. **JSON builder**: Use JsonBuilder for response construction
5. **Async operations**: Add async variants for I/O-bound operations
6. **Configuration**: External configuration for timeouts, limits

---

## Build Instructions

```bash
# Build all (server, client, GUI)
make all

# Build individually
make server
make client
make gui

# Run
./server 8888
./client 127.0.0.1 8888
./gui_app

# Clean
make clean
```

---

## Dependencies

- **C++17**: Required for `std::optional`, structured bindings
- **POSIX sockets**: Linux/WSL networking
- **pthreads**: Multi-threading
- **GTK+ 3.0**: GUI (optional)

---

*Document generated for English Learning Application refactoring project.*
