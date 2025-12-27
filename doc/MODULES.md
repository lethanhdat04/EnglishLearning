# Module Documentation

This document provides detailed documentation for each module in the English Learning Application. Each section describes the module's responsibility, key classes, interactions, and explicit boundaries.

---

## Table of Contents

1. [Core Module](#1-core-module-includecore)
2. [Protocol Module](#2-protocol-module-includeprotocol)
3. [Repository Module](#3-repository-module-includerepository)
4. [Repository Implementations](#4-repository-implementations-srcrepository)
5. [Service Module](#5-service-module-includeservice)
6. [Service Implementations](#6-service-implementations-srcservice)
7. [Presentation Layer](#7-presentation-layer-servercpp-clientcpp-gui_maincpp)

---

## 1. Core Module (`include/core/`)

### Responsibility

The Core module defines **domain entities** and **value types** that represent the business concepts of the application. It is the innermost layer with zero external dependencies.

### Key Classes and Roles

| Class | File | Role |
|-------|------|------|
| `User` | `user.h` | Represents a registered user with credentials, role, and online status |
| `Session` | `session.h` | Authentication session with token and expiry validation |
| `Lesson` | `lesson.h` | Learning content organized by level and topic |
| `Test` | `test.h` | Assessment containing multiple `TestQuestion` items |
| `TestQuestion` | `test.h` | Individual question with options and correct answer |
| `ChatMessage` | `chat_message.h` | Message between two users with read status |
| `Exercise` | `exercise.h` | Writing exercise requiring teacher review |
| `ExerciseSubmission` | `exercise.h` | Student submission with teacher feedback |
| `Game` | `game.h` | Interactive matching game with word/sentence pairs |
| `GameSession` | `game.h` | Player's game attempt with score tracking |

**Type Definitions** (`types.h`):
- Enums: `UserRole`, `Level`, `Topic`, `QuestionType`, `ExerciseType`, `GameType`, `SubmissionStatus`
- Type alias: `Timestamp` (int64_t milliseconds)
- String conversion helpers: `levelToString()`, `stringToLevel()`, etc.

### Interactions with Other Modules

```
Core Module
    ▲
    │ used by (read-only dependency)
    │
    ├── Repository Module (entities in interfaces)
    ├── Service Module (entities in business logic)
    └── Protocol Module (entities in serialization)
```

The Core module is **used by all other modules** but **depends on nothing**.

### Explicit Boundaries (Must NOT Do)

| Forbidden | Reason |
|-----------|--------|
| Include any headers outside `<std*>` | Must remain dependency-free |
| Contain business logic | Logic belongs in Service layer |
| Know about persistence | Repositories handle storage |
| Know about serialization | Protocol handles JSON |
| Use I/O operations | No file, network, or console access |
| Manage memory for other modules | Only self-contained data |

---

## 2. Protocol Module (`include/protocol/`)

### Responsibility

The Protocol module handles **message serialization**, **JSON parsing**, and **protocol utilities**. It defines the communication contract between client and server.

### Key Classes and Roles

| Class/Namespace | File | Role |
|-----------------|------|------|
| `MessageType` | `message_types.h` | Constants for all 40+ protocol message types |
| `JsonParser` | `json_parser.h` | Lightweight JSON parsing without external libraries |
| `JsonBuilder` | `json_builder.h` | Fluent API for constructing JSON responses |
| `utils` | `utils.h` | Timestamp, ID generation, session token utilities |

**Key Functions**:

```cpp
// JSON parsing
std::string getJsonValue(json, key);    // Extract string/primitive
std::string getJsonObject(json, key);   // Extract nested object
std::vector<std::string> parseJsonArray(arrayStr);

// Utilities
long long getCurrentTimestamp();        // Milliseconds since epoch
std::string generateId("prefix");       // e.g., "user_12345"
std::string generateSessionToken();     // 64-char alphanumeric
```

### Interactions with Other Modules

```
Protocol Module
    │
    ├── depends on ──► Core Module (for Timestamp type)
    │
    └── used by ◄──── Presentation Layer (handlers parse/build JSON)
                      Service Layer (for ID/timestamp generation)
```

### Explicit Boundaries (Must NOT Do)

| Forbidden | Reason |
|-----------|--------|
| Contain business logic | Protocol is data format, not rules |
| Access repositories | No data persistence |
| Make network calls | Presentation layer handles sockets |
| Store state | Stateless utility functions only |
| Validate business rules | Services validate semantics |
| Know about specific handlers | Generic message handling only |

---

## 3. Repository Module (`include/repository/`)

### Responsibility

The Repository module defines **data access interfaces** (contracts) for all entities. It abstracts storage operations without specifying implementation details.

### Key Classes and Roles

| Interface | File | Role |
|-----------|------|------|
| `IUserRepository` | `i_user_repository.h` | CRUD for users, online status, role queries |
| `ISessionRepository` | `i_session_repository.h` | Session creation, validation, socket mapping |
| `ILessonRepository` | `i_lesson_repository.h` | Lesson queries by level, topic |
| `ITestRepository` | `i_test_repository.h` | Test retrieval by level, type |
| `IChatRepository` | `i_chat_repository.h` | Message storage, conversation queries, read status |
| `IExerciseRepository` | `i_exercise_repository.h` | Exercise and submission management |
| `IGameRepository` | `i_game_repository.h` | Game and session tracking |

**Common Interface Pattern**:

```cpp
class IEntityRepository {
public:
    virtual ~IEntityRepository() = default;

    // Create
    virtual bool add(const Entity& entity) = 0;

    // Read
    virtual std::optional<Entity> findById(const std::string& id) const = 0;
    virtual std::vector<Entity> findAll() const = 0;
    virtual bool exists(const std::string& id) const = 0;

    // Update
    virtual bool update(const Entity& entity) = 0;

    // Delete
    virtual bool remove(const std::string& id) = 0;

    // Count
    virtual size_t count() const = 0;
};
```

### Interactions with Other Modules

```
Repository Module (Interfaces)
    │
    ├── depends on ──► Core Module (entity types in signatures)
    │
    ├── implemented by ◄── Repository Implementations (bridge, memory)
    │
    └── used by ◄──── Service Module (injected via constructor)
```

### Explicit Boundaries (Must NOT Do)

| Forbidden | Reason |
|-----------|--------|
| Contain implementation code | Interfaces only, implementations elsewhere |
| Include storage-specific headers | No SQL, file, or data structure includes |
| Define business rules | Only data operations |
| Handle serialization | Returns entities, not JSON |
| Manage transactions | Simple CRUD operations |
| Know about services | Lower layer, no upward dependencies |

---

## 4. Repository Implementations (`src/repository/`)

### Responsibility

Provides **concrete implementations** of repository interfaces. Two implementation strategies exist:

1. **Bridge Repositories**: Wrap existing global data structures for gradual migration
2. **Memory Repositories**: Standalone in-memory implementations for testing

### Key Classes and Roles

#### Bridge Repositories (`src/repository/bridge/`)

| Class | File | Role |
|-------|------|------|
| `BridgeUserRepository` | `bridge_repositories.h` | Wraps `std::map<string, User>` globals |
| `BridgeSessionRepository` | `bridge_repositories.h` | Wraps session and socket maps |
| `BridgeChatRepository` | `bridge_repositories.h` | Wraps `std::vector<ChatMessage>` |
| `BridgeLessonRepository` | `bridge_repositories_ext.h` | Wraps lessons map |
| `BridgeTestRepository` | `bridge_repositories_ext.h` | Wraps tests map |
| `BridgeExerciseRepository` | `bridge_repositories_ext.h` | Wraps exercises and submissions |
| `BridgeGameRepository` | `bridge_repositories_ext.h` | Wraps games and sessions |

**Bridge Pattern Example**:

```cpp
class BridgeUserRepository : public IUserRepository {
public:
    BridgeUserRepository(
        std::map<std::string, User>& users,      // Reference to global
        std::map<std::string, User*>& userById,
        std::mutex& mutex)
        : users_(users), userById_(userById), mutex_(mutex) {}

    std::optional<User> findByEmail(const std::string& email) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = users_.find(email);
        return it != users_.end() ? std::optional(it->second) : std::nullopt;
    }
private:
    std::map<std::string, User>& users_;
    std::mutex& mutex_;
};
```

#### Memory Repositories (`src/repository/memory/`)

| Class | File | Role |
|-------|------|------|
| `MemoryUserRepository` | `memory_user_repository.h` | Self-contained user storage |
| `MemorySessionRepository` | `memory_session_repository.h` | Self-contained session storage |

### Interactions with Other Modules

```
Repository Implementations
    │
    ├── implements ──► Repository Interfaces
    │
    ├── depends on ──► Core Module (entity types)
    │
    └── injected into ◄── ServiceContainer (at startup)
```

### Explicit Boundaries (Must NOT Do)

| Forbidden | Reason |
|-----------|--------|
| Contain business logic | Only data operations |
| Know about services | Lower layer |
| Parse or build JSON | Repositories work with entities |
| Make network calls | Data layer, not I/O |
| Log to console | Infrastructure concern |
| Throw business exceptions | Return bool/optional instead |

---

## 5. Service Module (`include/service/`)

### Responsibility

Defines **service interfaces** and **result types** for business operations. Services encapsulate use cases and business rules.

### Key Classes and Roles

| Class | File | Role |
|-------|------|------|
| `ServiceResult<T>` | `service_result.h` | Generic success/error wrapper for operations |
| `VoidResult` | `service_result.h` | Result type for operations with no return data |
| `IAuthService` | `i_auth_service.h` | Login, registration, session management |
| `ILessonService` | `i_lesson_service.h` | Lesson retrieval and filtering |
| `ITestService` | `i_test_service.h` | Test management and grading |
| `IChatService` | `i_chat_service.h` | Messaging and online users |
| `IExerciseService` | `i_exercise_service.h` | Exercise workflow with teacher review |
| `IGameService` | `i_game_service.h` | Game sessions and leaderboards |

**Data Transfer Objects (DTOs)**:

| DTO | Service | Purpose |
|-----|---------|---------|
| `LoginResult` | AuthService | Session token, user info, unread count |
| `RegisterResult` | AuthService | New user ID and email |
| `LessonListResult` | LessonService | Paginated lesson list |
| `TestSubmissionResult` | TestService | Score and correct answers |
| `ChatHistoryResult` | ChatService | Messages with unread count |
| `OnlineUsersResult` | ChatService | List of online users |

**ServiceResult Pattern**:

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

### Interactions with Other Modules

```
Service Module (Interfaces)
    │
    ├── depends on ──► Core Module (entities in signatures)
    │                  Repository Module (interfaces for DI)
    │
    ├── implemented by ◄── Service Implementations
    │
    └── used by ◄──── Presentation Layer (handlers call services)
```

### Explicit Boundaries (Must NOT Do)

| Forbidden | Reason |
|-----------|--------|
| Include implementation details | Interfaces only |
| Know about JSON/protocol | Services work with entities and DTOs |
| Access sockets or I/O | Presentation layer handles I/O |
| Manage threads directly | Concurrency via repository layer |
| Include specific repository implementations | Only interfaces |

---

## 6. Service Implementations (`src/service/`)

### Responsibility

Provides **concrete implementations** of service interfaces. Contains all **business logic** and **validation rules**.

### Key Classes and Roles

| Class | File | Role |
|-------|------|------|
| `AuthService` | `auth_service.h/cpp` | User authentication, session lifecycle |
| `LessonService` | `lesson_service.h/cpp` | Lesson queries with level filtering |
| `TestService` | `test_service.h/cpp` | Test retrieval and automatic grading |
| `ChatService` | `chat_service.h/cpp` | Message sending, read status |
| `ExerciseService` | `exercise_service.h/cpp` | Submission and teacher review workflow |
| `GameService` | `game_service.h/cpp` | Game session management, scoring |
| `ServiceContainer` | `service_container.h` | Dependency injection container |

**ServiceContainer** (Composition Root):

```cpp
class ServiceContainer {
public:
    ServiceContainer(
        IUserRepository& userRepo,
        ISessionRepository& sessionRepo,
        ILessonRepository& lessonRepo,
        ITestRepository& testRepo,
        IChatRepository& chatRepo,
        IExerciseRepository& exerciseRepo,
        IGameRepository& gameRepo);

    IAuthService& auth();
    ILessonService& lessons();
    ITestService& tests();
    IChatService& chat();
    IExerciseService& exercises();
    IGameService& games();
};
```

### Interactions with Other Modules

```
Service Implementations
    │
    ├── implements ──► Service Interfaces
    │
    ├── depends on ──► Core Module (entities)
    │                  Repository Interfaces (via DI)
    │                  Protocol Utils (timestamps, IDs)
    │
    └── used by ◄──── Presentation Layer (via ServiceContainer)
```

### Explicit Boundaries (Must NOT Do)

| Forbidden | Reason |
|-----------|--------|
| Parse or build JSON | Protocol layer responsibility |
| Access sockets | No network I/O |
| Access global variables | Use injected repositories |
| Log to files | Infrastructure concern |
| Know about specific handlers | Services are handler-agnostic |
| Manage HTTP/socket connections | Presentation layer concern |

---

## 7. Presentation Layer (`server.cpp`, `client.cpp`, `gui_main.cpp`)

### Responsibility

Handles **user interaction**, **network I/O**, and **request routing**. This is the outermost layer that coordinates all others.

### Key Components and Roles

#### Server (`server.cpp`)

| Component | Role |
|-----------|------|
| `main()` | Initialize services, start socket listener |
| `handleClient()` | Per-connection thread, message routing |
| `handleLogin()` | Parse request, call AuthService, build response |
| `handleGetLessons()` | Parse request, call LessonService, build response |
| `handleSendMessage()` | Parse request, call ChatService, push to recipient |
| Global data structures | Legacy storage (wrapped by bridge repos) |

#### Client (`client.cpp`)

| Component | Role |
|-----------|------|
| `main()` | Connect to server, run input loop |
| `sendRequest()` | Build and send JSON messages |
| `receiveResponse()` | Parse server responses |
| Menu functions | CLI user interface |

#### GUI (`gui_main.cpp`)

| Component | Role |
|-----------|------|
| `main()` | Initialize GTK, create windows |
| Widget callbacks | Handle button clicks, form submissions |
| `NetworkThread` | Background communication with server |

### Interactions with Other Modules

```
Presentation Layer
    │
    ├── uses ──► ServiceContainer (calls services)
    │            Protocol Module (JSON parsing/building)
    │            Core Module (entity types)
    │
    └── initializes ──► Repository Implementations (bridge repos)
                        Service Implementations (via container)
```

### Explicit Boundaries (Must NOT Do)

| Forbidden | Reason |
|-----------|--------|
| Contain business logic | Delegate to services |
| Access data structures directly | Use services (not global maps) |
| Validate business rules | Services handle validation |
| Define new entity types | Core module responsibility |
| Implement data access | Repository responsibility |

---

## Module Dependency Summary

```
┌─────────────────────────────────────────────────────────────┐
│                    PRESENTATION LAYER                        │
│              server.cpp, client.cpp, gui_main.cpp           │
└──────────────────────────┬──────────────────────────────────┘
                           │ uses
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                  SERVICE IMPLEMENTATIONS                     │
│                      src/service/                            │
└──────────────────────────┬──────────────────────────────────┘
                           │ implements
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                   SERVICE INTERFACES                         │
│                    include/service/                          │
└──────────────────────────┬──────────────────────────────────┘
                           │ depends on
                           ▼
┌──────────────────────────┴──────────────────────────────────┐
│                                                              │
│  ┌────────────────────┐      ┌────────────────────────────┐ │
│  │ REPOSITORY IMPLS   │      │   REPOSITORY INTERFACES    │ │
│  │  src/repository/   │─────►│    include/repository/     │ │
│  └────────────────────┘      └─────────────┬──────────────┘ │
│                                            │                 │
└────────────────────────────────────────────┼─────────────────┘
                                             │ depends on
                                             ▼
┌─────────────────────────────────────────────────────────────┐
│                     PROTOCOL MODULE                          │
│                    include/protocol/                         │
└──────────────────────────┬──────────────────────────────────┘
                           │ depends on
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                      CORE MODULE                             │
│                     include/core/                            │
│                   (NO DEPENDENCIES)                          │
└─────────────────────────────────────────────────────────────┘
```

---

## Quick Reference: What Goes Where

| I need to... | Module |
|--------------|--------|
| Add a new entity field | `include/core/` |
| Add a new message type | `include/protocol/message_types.h` |
| Add a new query method | `include/repository/i_*_repository.h` |
| Add business validation | `src/service/*_service.cpp` |
| Add a new API endpoint | `server.cpp` (handler function) |
| Add a new DTO | `include/service/i_*_service.h` |
| Change JSON format | `include/protocol/json_*.h` |
| Add database support | `src/repository/` (new implementation) |

---

*This document should be updated when modules are added or responsibilities change.*
