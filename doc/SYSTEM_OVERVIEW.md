# English Learning Application - System Overview

## 1. Purpose of the System

### Problem Statement

Learning English effectively requires consistent practice across multiple skill areas: vocabulary, grammar, reading comprehension, and communication. Traditional learning methods often lack:

- **Personalized progression** based on skill level
- **Immediate feedback** on exercises and tests
- **Interactive practice** through games and real-time chat
- **Teacher oversight** for writing exercises

The English Learning App addresses these gaps by providing a comprehensive, interactive platform for English language education.

### Target Users

| User Role | Description | Key Capabilities |
|-----------|-------------|------------------|
| **Students** | Language learners at any level | Access lessons, take tests, submit exercises, play learning games, chat with teachers |
| **Teachers** | Language instructors | All student capabilities plus: review and grade student submissions, provide feedback |
| **Administrators** | System operators | All teacher capabilities plus: manage content, user administration |

### Core Features

- **Lesson Management**: Structured lessons organized by level (beginner/intermediate/advanced) and topic
- **Testing System**: Multiple-choice tests with automatic grading
- **Exercise Submission**: Writing exercises with teacher review workflow
- **Learning Games**: Interactive matching games for vocabulary practice
- **Real-time Chat**: Direct messaging between students and teachers
- **Progress Tracking**: Level-based content filtering and progression

---

## 2. High-Level Architecture

The system follows a **layered architecture** with clear separation of concerns. Each layer has a single responsibility and depends only on layers below it.

### Layer Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    PRESENTATION LAYER                        │
│         (User interfaces: GUI, Console, Handlers)           │
├─────────────────────────────────────────────────────────────┤
│                      PROTOCOL LAYER                          │
│           (Message types, JSON serialization)                │
├─────────────────────────────────────────────────────────────┤
│                      SERVICE LAYER                           │
│              (Business logic, use cases)                     │
├─────────────────────────────────────────────────────────────┤
│                     REPOSITORY LAYER                         │
│            (Data access abstraction, interfaces)             │
├─────────────────────────────────────────────────────────────┤
│                   DATA / PERSISTENCE                         │
│         (In-memory storage, bridge to global state)          │
├─────────────────────────────────────────────────────────────┤
│                       CORE LAYER                             │
│                (Domain models, entities)                     │
└─────────────────────────────────────────────────────────────┘
```

### Layer Responsibilities

#### Presentation Layer
**Location**: `server.cpp` (handlers), `client.cpp`, `gui_main.cpp`

| Responsibility | Description |
|----------------|-------------|
| User interaction | Handle user input from CLI or GUI |
| Request routing | Map incoming messages to appropriate handlers |
| Response formatting | Build JSON responses for clients |
| Socket management | Accept connections, send/receive data |

**Key Components**:
- `handleLogin()`, `handleGetLessons()`, `handleSendMessage()`, etc.
- GTK+ GUI widgets and event handlers
- Console menu system

#### Protocol Layer
**Location**: `include/protocol/`

| Responsibility | Description |
|----------------|-------------|
| Message definition | Define all request/response message types |
| JSON parsing | Parse incoming JSON messages |
| JSON building | Construct outgoing JSON responses |
| Utilities | ID generation, timestamps, session tokens |

**Key Components**:
- `MessageType` namespace with 40+ message constants
- `JsonParser` for extracting values from JSON strings
- `JsonBuilder` for constructing JSON responses
- `utils` for cryptographic and time utilities

#### Service Layer
**Location**: `include/service/`, `src/service/`

| Responsibility | Description |
|----------------|-------------|
| Business logic | Implement use cases and business rules |
| Validation | Validate inputs before processing |
| Orchestration | Coordinate repository operations |
| Result handling | Return structured success/error results |

**Key Components**:
- `AuthService`: Login, registration, session management
- `LessonService`: Lesson retrieval and filtering
- `TestService`: Test management and submission grading
- `ChatService`: Messaging and online user tracking
- `ExerciseService`: Exercise CRUD and teacher review workflow
- `GameService`: Game sessions and leaderboards
- `ServiceContainer`: Dependency injection container

#### Repository Layer
**Location**: `include/repository/`, `src/repository/`

| Responsibility | Description |
|----------------|-------------|
| Data abstraction | Define contracts for data access |
| CRUD operations | Create, read, update, delete entities |
| Query methods | Find entities by various criteria |
| Thread safety | Ensure concurrent access is safe |

**Key Components**:
- `IUserRepository`: User data access interface
- `ISessionRepository`: Session management interface
- `ILessonRepository`: Lesson data access interface
- `ITestRepository`: Test data access interface
- `IChatRepository`: Chat message interface
- `IExerciseRepository`: Exercise and submission interface
- `IGameRepository`: Game and session interface

#### Data / Persistence Layer
**Location**: `src/repository/bridge/`, `src/repository/memory/`

| Responsibility | Description |
|----------------|-------------|
| Storage implementation | Implement repository interfaces |
| State management | Maintain in-memory data structures |
| Bridge adapters | Wrap legacy global state |

**Key Components**:
- `BridgeUserRepository`: Wraps `std::map<string, User>` globals
- `BridgeSessionRepository`: Wraps session maps
- `MemoryUserRepository`: Standalone in-memory implementation

#### Core Layer
**Location**: `include/core/`

| Responsibility | Description |
|----------------|-------------|
| Domain models | Define business entities as data structures |
| Entity behavior | Provide helper methods on entities |
| Type definitions | Common types used across layers |

**Key Components**:
- `User`: User entity with role/level helpers
- `Session`: Authentication session with expiry validation
- `Lesson`: Learning content with level and topic
- `Test`, `TestQuestion`: Assessment content
- `ChatMessage`: Messaging entity
- `Exercise`, `ExerciseSubmission`: Practice content with review workflow
- `Game`, `GameSession`: Interactive game content

---

## 3. Architecture Diagrams

### Client-Server Interaction

```
┌─────────────────┐                           ┌─────────────────┐
│     CLIENT      │                           │     SERVER      │
│                 │                           │                 │
│  ┌───────────┐  │      TCP/IP Socket        │  ┌───────────┐  │
│  │    GUI    │  │   (Length-prefixed JSON)  │  │  Handler  │  │
│  │    or     │◄─┼──────────────────────────►┼──│  Router   │  │
│  │  Console  │  │                           │  └─────┬─────┘  │
│  └───────────┘  │                           │        │        │
│                 │                           │        ▼        │
│  ┌───────────┐  │                           │  ┌───────────┐  │
│  │ Protocol  │  │                           │  │  Service  │  │
│  │  Parser   │  │                           │  │ Container │  │
│  └───────────┘  │                           │  └─────┬─────┘  │
│                 │                           │        │        │
└─────────────────┘                           │        ▼        │
                                              │  ┌───────────┐  │
                                              │  │Repository │  │
                                              │  │  Layer    │  │
                                              │  └─────┬─────┘  │
                                              │        │        │
                                              │        ▼        │
                                              │  ┌───────────┐  │
                                              │  │   Data    │  │
                                              │  │  Storage  │  │
                                              │  └───────────┘  │
                                              └─────────────────┘
```

### Message Flow Example (Login)

```
Client                    Server
  │                         │
  │  LOGIN_REQUEST          │
  │  {email, password}      │
  │────────────────────────►│
  │                         │
  │                         ├──► handleLogin()
  │                         │         │
  │                         │         ▼
  │                         │    AuthService.login()
  │                         │         │
  │                         │         ▼
  │                         │    UserRepository.findByEmail()
  │                         │    SessionRepository.add()
  │                         │         │
  │                         │◄────────┘
  │                         │
  │  LOGIN_RESPONSE         │
  │  {userId, sessionToken} │
  │◄────────────────────────│
  │                         │
```

### Dependency Direction

```
                    ┌─────────────────┐
                    │   Presentation  │
                    │   (Handlers)    │
                    └────────┬────────┘
                             │ depends on
                             ▼
                    ┌─────────────────┐
                    │    Protocol     │
                    │ (MessageTypes)  │
                    └────────┬────────┘
                             │ depends on
                             ▼
┌─────────────────┐ ┌─────────────────┐
│    Service      │ │   Repository    │
│ (Impl classes)  │─│  (Interfaces)   │
└────────┬────────┘ └────────┬────────┘
         │                   │
         │ depends on        │ implemented by
         ▼                   ▼
┌─────────────────┐ ┌─────────────────┐
│      Core       │ │  Bridge/Memory  │
│ (Domain Models) │ │    (Impls)      │
└─────────────────┘ └─────────────────┘

         ▲
         │
   NO DEPENDENCIES
   (Pure domain)
```

---

## 4. Key Design Principles

### Separation of Concerns

Each layer has a **single, well-defined responsibility**:

| Principle | Implementation |
|-----------|----------------|
| **Handlers don't contain business logic** | They parse JSON, call services, format responses |
| **Services don't know about sockets** | They receive parameters, return `ServiceResult<T>` |
| **Repositories don't know about JSON** | They work with domain entities only |
| **Core has no dependencies** | Pure data structures with helper methods |

**Example**: Login flow
```cpp
// Handler: JSON parsing + response building
std::string handleLogin(const std::string& json, int socket) {
    std::string email = getJsonValue(payload, "email");
    auto result = serviceContainer->auth().login(email, password, socket);
    return result.isSuccess() ? buildSuccess(result.getData()) : buildError(result.getMessage());
}

// Service: Business logic
ServiceResult<LoginResult> AuthService::login(email, password, socket) {
    auto user = userRepo_.findByEmail(email);  // Repository call
    if (!user || user->password != password) return error("Invalid credentials");
    // Create session, update status...
    return success(loginResult);
}
```

### Dependency Direction

Dependencies flow **inward toward the core**:

- **Outer layers depend on inner layers** (never the reverse)
- **Interfaces live in inner layers**, implementations in outer layers
- **The core layer has zero external dependencies**

```
Presentation → Protocol → Service → Repository → Core
                              ↓
                         Data/Persistence
```

This enables:
- Swapping implementations without changing interfaces
- Testing with mock repositories
- Adding new persistence backends (database) without touching services

### Testability

The architecture enables testing at each layer:

| Layer | Testing Approach |
|-------|------------------|
| **Core** | Unit test entity methods directly |
| **Repository** | Test with in-memory implementations |
| **Service** | Inject mock repositories, test business logic |
| **Protocol** | Test JSON parsing/building functions |
| **Presentation** | Integration tests with real/mock services |

**Example**: Testing AuthService
```cpp
// Create mock repository
MockUserRepository mockUsers;
mockUsers.addUser(testUser);

// Inject into service
AuthService authService(mockUsers, mockSessions, mockChat);

// Test business logic
auto result = authService.login("test@example.com", "password", -1);
ASSERT(result.isSuccess());
ASSERT(result.getData().userId == testUser.userId);
```

### Extensibility

The layered design supports common extension scenarios:

| Extension | Implementation Approach |
|-----------|-------------------------|
| **New feature (e.g., Vocabulary Lists)** | Add `VocabularyService`, `IVocabularyRepository`, `Vocabulary` entity |
| **Database persistence** | Implement `SqlUserRepository` conforming to `IUserRepository` |
| **REST API** | Add HTTP handlers calling existing services |
| **Mobile client** | Use same JSON protocol over sockets |
| **New game type** | Extend `Game` entity, add logic to `GameService` |

---

## 5. Non-Goals / Future Extensions

### Current Non-Goals

These are explicitly **not** in the current scope:

| Non-Goal | Rationale |
|----------|-----------|
| **Database persistence** | In-memory storage sufficient for demonstration |
| **Password hashing** | Simplified for development; marked with TODO |
| **HTTPS/TLS** | Plain TCP for simplicity; add TLS wrapper for production |
| **Horizontal scaling** | Single-server architecture; no distributed state |
| **Rich media content** | Text-only lessons; no audio/video support |
| **Internationalization** | English-only interface |

### Planned Future Extensions

| Extension | Priority | Description |
|-----------|----------|-------------|
| **SQLite persistence** | High | Replace in-memory with SQLite for data durability |
| **Password security** | High | Add bcrypt hashing for production deployment |
| **Complete handler migration** | Medium | Migrate all handlers to use service layer |
| **Unit test suite** | Medium | Comprehensive tests for all services |
| **Async operations** | Low | Non-blocking I/O for better scalability |
| **REST API** | Low | HTTP endpoints for web client support |
| **Audio lessons** | Low | Support for listening comprehension |

### Migration Path

To migrate to a production-ready system:

1. **Database**: Implement `SqliteUserRepository`, `SqliteSessionRepository`, etc.
2. **Security**: Add password hashing in `AuthService`, TLS wrapper for sockets
3. **Config**: Externalize port, timeouts, database path to config file
4. **Logging**: Replace `std::cout` with structured logging framework
5. **Monitoring**: Add health check endpoint, metrics collection

---

## Quick Reference

### Building

```bash
make all          # Build server, client, GUI
make server       # Build server only
make client       # Build CLI client only
make gui          # Build GTK GUI only
make clean        # Remove binaries
```

### Running

```bash
./server 8888              # Start server on port 8888
./client 127.0.0.1 8888    # Connect CLI client
./gui_app                  # Launch GUI client
```

### Default Accounts

| Email | Password | Role |
|-------|----------|------|
| student@example.com | student123 | student |
| student2@example.com | student123 | student |
| sarah@example.com | teacher123 | teacher |

### Directory Structure

```
EnglishLearning/
├── include/
│   ├── core/           # Domain models
│   ├── protocol/       # Message types, JSON utilities
│   ├── repository/     # Data access interfaces
│   └── service/        # Service interfaces
├── src/
│   ├── protocol/       # Protocol implementations
│   ├── repository/     # Repository implementations
│   └── service/        # Service implementations
├── doc/                # Documentation
├── server.cpp          # Server application
├── client.cpp          # CLI client
├── gui_main.cpp        # GTK GUI client
└── Makefile            # Build configuration
```

---

*This document describes the system as of the current refactoring phase. It should be updated as the system evolves.*
