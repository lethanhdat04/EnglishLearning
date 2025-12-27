# Runtime Behavior Documentation

This document describes the runtime behavior of the English Learning Application, including startup sequences, request flows, and component interactions.

---

## Table of Contents

1. [Application Startup Flow](#1-application-startup-flow)
2. [Login Flow](#2-login-flow)
3. [Example Feature Flow: Fetch Lessons](#3-example-feature-flow-fetch-lessons)
4. [Example Feature Flow: Send Chat Message](#4-example-feature-flow-send-chat-message)

---

## 1. Application Startup Flow

### 1.1 Server Startup

The server initializes all layers and begins accepting client connections.

#### Sequence Diagram

```
┌─────────┐    ┌──────────┐    ┌────────────┐    ┌─────────────┐    ┌────────┐
│  main() │    │  Sample  │    │   Bridge   │    │   Service   │    │ Socket │
│         │    │   Data   │    │   Repos    │    │  Container  │    │ Server │
└────┬────┘    └────┬─────┘    └─────┬──────┘    └──────┬──────┘    └────┬───┘
     │              │                │                  │                │
     │──(1)────────►│                │                  │                │
     │              │ initSampleData │                  │                │
     │◄─────────────│                │                  │                │
     │              │                │                  │                │
     │──(2)─────────────────────────►│                  │                │
     │              │                │ create bridges   │                │
     │◄──────────────────────────────│                  │                │
     │              │                │                  │                │
     │──(3)────────────────────────────────────────────►│                │
     │              │                │                  │ create services│
     │◄─────────────────────────────────────────────────│                │
     │              │                │                  │                │
     │──(4)─────────────────────────────────────────────────────────────►│
     │              │                │                  │                │ bind/listen
     │              │                │                  │                │
     │──(5)─────────────────────────────────────────────────────────────►│
     │              │                │                  │                │ accept loop
     └──────────────┴────────────────┴──────────────────┴────────────────┘
```

#### Step-by-Step

| Step | Action | Layer | Responsible Component |
|------|--------|-------|----------------------|
| 1 | Parse command-line arguments (port) | Presentation | `main()` |
| 2 | Register signal handlers (SIGINT, SIGTERM) | Presentation | `main()` |
| 3 | Initialize sample data (users, lessons, tests) | Data | `initSampleData()` |
| 4 | Create bridge repositories wrapping global data | Repository | `BridgeUserRepository`, etc. |
| 5 | Create service container with injected repos | Service | `ServiceContainer` |
| 6 | Create TCP socket | Presentation | `socket()` |
| 7 | Bind to port and start listening | Presentation | `bind()`, `listen()` |
| 8 | Print startup banner with sample accounts | Presentation | `main()` |
| 9 | Enter accept loop, spawn thread per client | Presentation | `accept()`, `std::thread` |

#### Startup Output

```
[INFO] Sample data initialized: 6 users, 7 lessons, 3 tests, 4 exercises, 4 games
[INFO] Service layer initialized
============================================
   ENGLISH LEARNING APP - SERVER
============================================
[INFO] Server started on port 8888
```

---

### 1.2 Console Client Startup

The console client connects to the server and presents a text-based menu.

#### Sequence Diagram

```
┌─────────┐    ┌────────┐    ┌─────────────┐    ┌──────────┐
│  main() │    │ Socket │    │ Recv Thread │    │ Menu UI  │
└────┬────┘    └───┬────┘    └──────┬──────┘    └────┬─────┘
     │             │                │                │
     │──(1)───────►│                │                │
     │             │ create socket  │                │
     │◄────────────│                │                │
     │             │                │                │
     │──(2)───────►│                │                │
     │             │ connect()      │                │
     │◄────────────│                │                │
     │             │                │                │
     │──(3)────────────────────────►│                │
     │             │                │ spawn thread   │
     │             │                │                │
     │──(4)───────────────────────────────────────► │
     │             │                │                │ show login menu
     │             │                │                │
     └─────────────┴────────────────┴────────────────┘
```

#### Step-by-Step

| Step | Action | Layer | Responsible Component |
|------|--------|-------|----------------------|
| 1 | Parse server IP and port from arguments | Presentation | `main()` |
| 2 | Create TCP socket | Presentation | `socket()` |
| 3 | Connect to server | Presentation | `connect()` |
| 4 | Spawn background receive thread | Presentation | `receiveThreadFunc()` |
| 5 | Display login/register menu | Presentation | Menu loop |
| 6 | Wait for user input | Presentation | `std::getline()` |

---

### 1.3 GUI Client Startup

The GTK GUI client establishes connection and renders the login window.

#### Sequence Diagram

```
┌─────────┐    ┌────────────┐    ┌─────────────┐    ┌──────────┐
│  main() │    │ Connection │    │ Recv Thread │    │   GTK    │
└────┬────┘    └─────┬──────┘    └──────┬──────┘    └────┬─────┘
     │               │                  │                │
     │──(1)─────────►│                  │                │
     │               │ connectToServer  │                │
     │◄──────────────│                  │                │
     │               │                  │                │
     │──(2)─────────────────────────────►               │
     │               │                  │ spawn detached │
     │               │                  │                │
     │──(3)─────────────────────────────────────────────►
     │               │                  │                │ gtk_init()
     │               │                  │                │
     │──(4)─────────────────────────────────────────────►
     │               │                  │                │ create window
     │               │                  │                │
     │──(5)─────────────────────────────────────────────►
     │               │                  │                │ gtk_main()
     └───────────────┴──────────────────┴────────────────┘
```

#### Step-by-Step

| Step | Action | Layer | Responsible Component |
|------|--------|-------|----------------------|
| 1 | Connect to server (127.0.0.1:8888) | Presentation | `connectToServer()` |
| 2 | Spawn detached receive thread | Presentation | `receiveThreadFunc()` |
| 3 | Initialize GTK | Presentation | `gtk_init()` |
| 4 | Create main window with login form | Presentation | Widget creation |
| 5 | Connect button signals to handlers | Presentation | `g_signal_connect()` |
| 6 | Enter GTK main loop | Presentation | `gtk_main()` |

---

## 2. Login Flow

The login flow demonstrates request/response handling across all layers.

### Sequence Diagram

```
┌────────┐   ┌────────┐   ┌─────────────┐   ┌─────────┐   ┌────────────┐   ┌────────────┐
│ Client │   │ Server │   │  Handler    │   │ Protocol│   │ Repository │   │   Data     │
│  UI    │   │ Socket │   │ (Presentn.) │   │ (JSON)  │   │ (Bridge)   │   │ (Globals)  │
└───┬────┘   └───┬────┘   └──────┬──────┘   └────┬────┘   └─────┬──────┘   └─────┬──────┘
    │            │               │               │              │                │
    │──(1)──────►│               │               │              │                │
    │  LOGIN_REQUEST             │               │              │                │
    │            │               │               │              │                │
    │            │──(2)─────────►│               │              │                │
    │            │  recv message │               │              │                │
    │            │               │               │              │                │
    │            │               │──(3)─────────►│              │                │
    │            │               │  getJsonValue │              │                │
    │            │               │◄──────────────│              │                │
    │            │               │               │              │                │
    │            │               │──(4)──────────────────────── ►               │
    │            │               │                              │ findByEmail   │
    │            │               │                              │──────────────►│
    │            │               │                              │◄──────────────│
    │            │               │◄─────────────────────────────│               │
    │            │               │               │              │                │
    │            │               │──(5)─────────►│              │                │
    │            │               │  generateToken│              │                │
    │            │               │◄──────────────│              │                │
    │            │               │               │              │                │
    │            │               │──(6)──────────────────────── ►               │
    │            │               │               │              │ add session   │
    │            │               │               │              │──────────────►│
    │            │               │◄─────────────────────────────│               │
    │            │               │               │              │                │
    │            │               │──(7)─────────►│              │                │
    │            │               │  build JSON   │              │                │
    │            │               │◄──────────────│              │                │
    │            │               │               │              │                │
    │            │◄──(8)─────────│               │              │                │
    │            │  send response│               │              │                │
    │            │               │               │              │                │
    │◄───(9)─────│               │               │              │                │
    │ LOGIN_RESPONSE             │               │              │                │
    └────────────┴───────────────┴───────────────┴──────────────┴────────────────┘
```

### Step-by-Step

| Step | Action | Layer | Responsible Component |
|------|--------|-------|----------------------|
| 1 | User enters email/password, clicks Login | Presentation | Client UI |
| 2 | Client builds JSON request, sends to server | Protocol | `sendRequest()` |
| 3 | Server receives bytes, extracts message | Presentation | `handleClient()` |
| 4 | Router dispatches to login handler | Presentation | Message type switch |
| 5 | Handler parses JSON payload | Protocol | `getJsonValue()` |
| 6 | Handler queries user by email | Repository | `users.find(email)` |
| 7 | Validate password matches | Presentation | `handleLogin()` |
| 8 | Generate session token | Protocol | `generateSessionToken()` |
| 9 | Store session and update online status | Repository | `sessions[token]`, `user.online` |
| 10 | Build JSON response | Protocol | String concatenation |
| 11 | Send response to client | Presentation | `send()` |
| 12 | Send unread messages notification | Presentation | `sendUnreadMessagesNotification()` |
| 13 | Client receives and parses response | Protocol | `receiveThreadFunc()` |
| 14 | Update UI with user info | Presentation | Client UI update |

### Request Message

```json
{
  "messageType": "LOGIN_REQUEST",
  "messageId": "msg_1_12345",
  "timestamp": 1703673600000,
  "payload": {
    "email": "student@example.com",
    "password": "student123"
  }
}
```

### Response Message (Success)

```json
{
  "messageType": "LOGIN_RESPONSE",
  "messageId": "msg_1_12345",
  "timestamp": 1703673600100,
  "payload": {
    "status": "success",
    "message": "Login successfully",
    "data": {
      "userId": "student_001",
      "fullname": "John Doe",
      "email": "student@example.com",
      "level": "intermediate",
      "sessionToken": "abc123...xyz",
      "expiresAt": 1703677200000
    }
  }
}
```

---

## 3. Example Feature Flow: Fetch Lessons

This flow demonstrates authenticated data retrieval with filtering.

### Sequence Diagram

```
┌────────┐   ┌────────┐   ┌─────────────┐   ┌──────────────┐   ┌────────────┐
│ Client │   │ Server │   │  Handler    │   │  Validate    │   │  Lessons   │
│        │   │        │   │             │   │  Session     │   │    Map     │
└───┬────┘   └───┬────┘   └──────┬──────┘   └──────┬───────┘   └─────┬──────┘
    │            │               │                 │                 │
    │──(1)──────►│               │                 │                 │
    │ GET_LESSONS_REQUEST        │                 │                 │
    │            │               │                 │                 │
    │            │──(2)─────────►│                 │                 │
    │            │               │                 │                 │
    │            │               │──(3)───────────►│                 │
    │            │               │ validateSession │                 │
    │            │               │◄────────────────│                 │
    │            │               │                 │                 │
    │            │               │──(4)────────────────────────────►│
    │            │               │                 │                 │ iterate
    │            │               │                 │                 │ & filter
    │            │               │◄─────────────────────────────────│
    │            │               │                 │                 │
    │            │◄──(5)─────────│                 │                 │
    │            │ response      │                 │                 │
    │            │               │                 │                 │
    │◄───(6)─────│               │                 │                 │
    │ GET_LESSONS_RESPONSE       │                 │                 │
    └────────────┴───────────────┴─────────────────┴─────────────────┘
```

### Step-by-Step

| Step | Action | Layer | Responsible Component |
|------|--------|-------|----------------------|
| 1 | User selects "View Lessons" from menu | Presentation | Client UI |
| 2 | Client builds request with session token and filters | Protocol | JSON building |
| 3 | Server receives and routes to handler | Presentation | `handleClient()` |
| 4 | Handler parses message and payload | Protocol | `getJsonValue()`, `getJsonObject()` |
| 5 | Validate session token, get userId | Presentation | `validateSession()` |
| 6 | If invalid, return error response | Presentation | Early return |
| 7 | Iterate through lessons map | Repository | `for (auto& pair : lessons)` |
| 8 | Apply level filter if specified | Presentation | `if (lesson.level != level)` |
| 9 | Apply topic filter if specified | Presentation | `if (lesson.topic != topic)` |
| 10 | Build JSON array of matching lessons | Protocol | `std::stringstream` |
| 11 | Build response with pagination info | Protocol | String concatenation |
| 12 | Send response to client | Presentation | `send()` |
| 13 | Client parses and displays lesson list | Presentation | UI rendering |

### Request Message

```json
{
  "messageType": "GET_LESSONS_REQUEST",
  "messageId": "msg_5_67890",
  "sessionToken": "abc123...xyz",
  "timestamp": 1703673700000,
  "payload": {
    "level": "intermediate",
    "topic": ""
  }
}
```

### Response Message

```json
{
  "messageType": "GET_LESSONS_RESPONSE",
  "messageId": "msg_5_67890",
  "timestamp": 1703673700050,
  "payload": {
    "status": "success",
    "message": "Retrieved lessons successfully",
    "data": {
      "lessons": [
        {
          "lessonId": "lesson_001",
          "title": "Past Tense Mastery",
          "description": "Learn past tense...",
          "topic": "grammar",
          "level": "intermediate",
          "duration": 30
        }
      ],
      "pagination": {
        "currentPage": 1,
        "totalPages": 1,
        "totalLessons": 1
      }
    }
  }
}
```

---

## 4. Example Feature Flow: Send Chat Message

This flow demonstrates real-time message delivery with push notification.

### Sequence Diagram

```
┌────────┐   ┌────────┐   ┌─────────────┐   ┌──────────┐   ┌───────────┐   ┌──────────┐
│ Sender │   │ Server │   │  Handler    │   │  Chat    │   │ Recipient │   │Recipient │
│ Client │   │        │   │             │   │ Storage  │   │   User    │   │  Client  │
└───┬────┘   └───┬────┘   └──────┬──────┘   └────┬─────┘   └─────┬─────┘   └────┬─────┘
    │            │               │               │               │              │
    │──(1)──────►│               │               │               │              │
    │ SEND_MESSAGE_REQUEST       │               │               │              │
    │            │               │               │               │              │
    │            │──(2)─────────►│               │               │              │
    │            │               │               │               │              │
    │            │               │──(3)─────────►│               │              │
    │            │               │ validateSession               │              │
    │            │               │◄──────────────│               │              │
    │            │               │               │               │              │
    │            │               │──(4)──────────────────────────►              │
    │            │               │               │               │ find user   │
    │            │               │◄──────────────────────────────│              │
    │            │               │               │               │              │
    │            │               │──(5)─────────►│               │              │
    │            │               │               │ store message │              │
    │            │               │◄──────────────│               │              │
    │            │               │               │               │              │
    │            │               │──(6)──────────────────────────►              │
    │            │               │               │               │ check online│
    │            │               │               │               │              │
    │            │               │──(7)─────────────────────────────────────────►
    │            │               │               │               │ RECEIVE_MSG │
    │            │               │               │               │              │
    │            │◄──(8)─────────│               │               │              │
    │            │ SEND_MESSAGE_RESPONSE         │               │              │
    │            │               │               │               │              │
    │◄───(9)─────│               │               │               │              │
    │ success    │               │               │               │              │
    └────────────┴───────────────┴───────────────┴───────────────┴──────────────┘
```

### Step-by-Step

| Step | Action | Layer | Responsible Component |
|------|--------|-------|----------------------|
| 1 | User types message and clicks Send | Presentation | Client UI |
| 2 | Client builds request with recipient and content | Protocol | JSON building |
| 3 | Server receives and routes to handler | Presentation | `handleClient()` |
| 4 | Validate sender's session token | Presentation | `validateSession()` |
| 5 | Look up recipient user by ID | Repository | `userById.find()` |
| 6 | If recipient not found, return error | Presentation | Early return |
| 7 | Create ChatMessage entity | Core | `ChatMessage` struct |
| 8 | Store message in chat storage | Repository | `chatMessages.push_back()` |
| 9 | Check if recipient is online | Repository | `recipient->online` |
| 10 | If online, build push notification | Protocol | `RECEIVE_MESSAGE` JSON |
| 11 | Send notification to recipient's socket | Presentation | `send(recipient->clientSocket)` |
| 12 | Build success response for sender | Protocol | String concatenation |
| 13 | Send response to sender | Presentation | `send()` |
| 14 | Recipient's receive thread gets notification | Presentation | Background thread |
| 15 | Recipient UI updates with new message | Presentation | UI callback |

### Request Message (Sender)

```json
{
  "messageType": "SEND_MESSAGE_REQUEST",
  "messageId": "msg_10_11111",
  "sessionToken": "sender_token_abc",
  "timestamp": 1703673800000,
  "payload": {
    "recipientId": "teacher_001",
    "messageContent": "Hello teacher, I have a question."
  }
}
```

### Push Notification (To Recipient)

```json
{
  "messageType": "RECEIVE_MESSAGE",
  "messageId": "chatmsg_54321",
  "timestamp": 1703673800050,
  "payload": {
    "messageId": "chatmsg_54321",
    "senderId": "student_001",
    "senderName": "John Doe",
    "messageContent": "Hello teacher, I have a question.",
    "sentAt": 1703673800050
  }
}
```

### Response Message (To Sender)

```json
{
  "messageType": "SEND_MESSAGE_RESPONSE",
  "messageId": "msg_10_11111",
  "timestamp": 1703673800060,
  "payload": {
    "status": "success",
    "message": "Message sent successfully",
    "data": {
      "messageId": "chatmsg_54321",
      "deliveredToRecipient": true
    }
  }
}
```

---

## Summary: Layer Responsibilities by Flow

| Flow Phase | Presentation | Protocol | Service | Repository | Data |
|------------|--------------|----------|---------|------------|------|
| **Receive Request** | Socket recv, routing | - | - | - | - |
| **Parse Request** | - | JSON parsing | - | - | - |
| **Validate Session** | Call validateSession | - | - | Session lookup | - |
| **Execute Logic** | Handler orchestration | - | Business rules | - | - |
| **Access Data** | - | - | - | CRUD operations | Storage |
| **Build Response** | - | JSON building | - | - | - |
| **Send Response** | Socket send | - | - | - | - |

---

## Connection Lifecycle

```
┌─────────────────────────────────────────────────────────────────┐
│                        CLIENT LIFECYCLE                          │
├─────────────────────────────────────────────────────────────────┤
│  1. Connect ──► 2. Login ──► 3. Use Features ──► 4. Disconnect  │
│       │              │              │                   │        │
│    socket()      session        requests           cleanup      │
│    connect()     created        processed          session      │
│                  online=true                       invalidated  │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                        SERVER LIFECYCLE                          │
├─────────────────────────────────────────────────────────────────┤
│  1. Startup ──► 2. Accept ──► 3. Handle ──► 4. Cleanup ──► 5.   │
│       │            │             │              │          │     │
│    init()      per-client    message       on disconnect  │     │
│    bind()       thread        loop         user.online    │     │
│    listen()                               session remove  │     │
│                                                       Shutdown   │
└─────────────────────────────────────────────────────────────────┘
```

---

*This document describes runtime behavior as of the current implementation.*
