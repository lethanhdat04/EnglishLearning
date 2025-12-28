# English Learning Application Protocol Specification

Version: 1.1
Last Updated: 2025-12-28

## Table of Contents

1. [Transport Layer](#1-transport-layer)
2. [Message Format](#2-message-format)
3. [Message Types](#3-message-types)
   - [Authentication](#31-authentication)
   - [Lessons](#32-lessons)
   - [Tests](#33-tests)
   - [Exercises](#34-exercises)
   - [Games](#35-games)
   - [Chat](#36-chat)
   - [Voice Call](#37-voice-call)
   - [Teacher Feedback](#38-teacher-feedback)
   - [Error Handling](#39-error-handling)

---

## 1. Transport Layer

### 1.1 Protocol

- **Transport**: TCP
- **Default Port**: Application-defined (typically 8080)
- **Connection**: Persistent socket connection per client
- **Encoding**: UTF-8

### 1.2 Message Framing

Messages are framed using a **length-prefixed** strategy:

```
+----------------+------------------+
| Length (4 bytes) | JSON Payload     |
+----------------+------------------+
```

| Field | Size | Description |
|-------|------|-------------|
| Length | 4 bytes | Big-endian unsigned integer representing payload size in bytes |
| Payload | Variable | UTF-8 encoded JSON message |

**Reading a Message:**
1. Read 4 bytes to get message length
2. Read exactly `length` bytes for the JSON payload
3. Parse JSON payload

**Writing a Message:**
1. Serialize message to JSON string
2. Calculate byte length of JSON string
3. Write 4-byte length prefix (big-endian)
4. Write JSON payload bytes

### 1.3 Connection Lifecycle

1. **Connect**: Client establishes TCP connection
2. **Authenticate**: Client sends `LOGIN_REQUEST` or `REGISTER_REQUEST`
3. **Session**: Server maintains session token for authenticated requests
4. **Keep-Alive**: Connection remains open for push notifications
5. **Disconnect**: Client closes socket or session expires

---

## 2. Message Format

### 2.1 JSON Structure

All messages follow a standard envelope format:

```json
{
  "messageType": "<MESSAGE_TYPE_CONSTANT>",
  "messageId": "<unique_identifier>",
  "timestamp": <milliseconds_since_epoch>,
  "payload": { }
}
```

### 2.2 Required Fields

| Field | Type | Description |
|-------|------|-------------|
| `messageType` | string | Message type constant (see Section 3) |
| `messageId` | string | Unique identifier for request-response correlation |
| `timestamp` | number | Unix timestamp in milliseconds |
| `payload` | object | Message-specific data |

### 2.3 Message ID Format

```
msg_<counter>_<timestamp_mod_100000>
```

Example: `msg_42_85321`

### 2.4 Response Payload Structure

**Success with Data:**
```json
{
  "status": "success",
  "data": { }
}
```

**Success with Message:**
```json
{
  "status": "success",
  "message": "Operation completed successfully"
}
```

**Error:**
```json
{
  "status": "error",
  "message": "Error description"
}
```

### 2.5 Common Data Types

| Type | JSON Representation | Example |
|------|---------------------|---------|
| Timestamp | number (ms since epoch) | `1703721600000` |
| UserId | string | `"user_12345"` |
| Level | string enum | `"beginner"`, `"intermediate"`, `"advanced"` |
| Topic | string enum | `"grammar"`, `"vocabulary"`, `"listening"`, `"speaking"`, `"reading"`, `"writing"` |
| UserRole | string enum | `"student"`, `"teacher"`, `"admin"` |
| Boolean | boolean | `true`, `false` |

---

## 3. Message Types

### 3.1 Authentication

#### 3.1.1 Register

**Purpose**: Create a new user account.

**Request** (`REGISTER_REQUEST`):
```json
{
  "messageType": "REGISTER_REQUEST",
  "messageId": "msg_1_12345",
  "timestamp": 1703721600000,
  "payload": {
    "fullname": "John Doe",
    "email": "john@example.com",
    "password": "securepassword123",
    "role": "student"
  }
}
```

| Field | Required | Description |
|-------|----------|-------------|
| fullname | Yes | User's full name |
| email | Yes | Unique email address |
| password | Yes | User password |
| role | Yes | `"student"`, `"teacher"`, or `"admin"` |

**Response** (`REGISTER_RESPONSE`):
```json
{
  "messageType": "REGISTER_RESPONSE",
  "messageId": "msg_1_12345",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "data": {
      "userId": "user_54321",
      "sessionToken": "a1b2c3d4e5f6...64chars...",
      "expiresAt": 1703725200000
    }
  }
}
```

**Error Cases:**
- Email already exists
- Invalid email format
- Missing required fields
- Invalid role value

---

#### 3.1.2 Login

**Purpose**: Authenticate an existing user and obtain a session token.

**Request** (`LOGIN_REQUEST`):
```json
{
  "messageType": "LOGIN_REQUEST",
  "messageId": "msg_2_12346",
  "timestamp": 1703721600000,
  "payload": {
    "email": "john@example.com",
    "password": "securepassword123"
  }
}
```

| Field | Required | Description |
|-------|----------|-------------|
| email | Yes | User's email address |
| password | Yes | User's password |

**Response** (`LOGIN_RESPONSE`):
```json
{
  "messageType": "LOGIN_RESPONSE",
  "messageId": "msg_2_12346",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "data": {
      "userId": "user_54321",
      "fullname": "John Doe",
      "email": "john@example.com",
      "level": "beginner",
      "role": "student",
      "sessionToken": "a1b2c3d4e5f6...64chars...",
      "expiresAt": 1703725200000
    }
  }
}
```

**Error Cases:**
- Invalid credentials
- User not found
- Account disabled

---

#### 3.1.3 Set Level

**Purpose**: Update user's proficiency level.

**Request** (`SET_LEVEL_REQUEST`):
```json
{
  "messageType": "SET_LEVEL_REQUEST",
  "messageId": "msg_3_12347",
  "timestamp": 1703721600000,
  "payload": {
    "sessionToken": "a1b2c3d4e5f6...64chars...",
    "level": "intermediate"
  }
}
```

**Response** (`SET_LEVEL_RESPONSE`):
```json
{
  "messageType": "SET_LEVEL_RESPONSE",
  "messageId": "msg_3_12347",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "message": "Level updated successfully"
  }
}
```

**Error Cases:**
- Invalid session token
- Session expired
- Invalid level value

---

### 3.2 Lessons

#### 3.2.1 Get Lessons List

**Purpose**: Retrieve a filtered list of available lessons.

**Request** (`GET_LESSONS_REQUEST`):
```json
{
  "messageType": "GET_LESSONS_REQUEST",
  "messageId": "msg_10_12350",
  "timestamp": 1703721600000,
  "payload": {
    "sessionToken": "a1b2c3d4e5f6...64chars...",
    "level": "beginner",
    "topic": "grammar"
  }
}
```

| Field | Required | Description |
|-------|----------|-------------|
| sessionToken | Yes | Valid session token |
| level | No | Filter by level |
| topic | No | Filter by topic |

**Response** (`GET_LESSONS_RESPONSE`):
```json
{
  "messageType": "GET_LESSONS_RESPONSE",
  "messageId": "msg_10_12350",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "data": {
      "lessons": [
        {
          "lessonId": "lesson_001",
          "title": "Introduction to English Grammar",
          "description": "Learn the basics of English grammar",
          "topic": "grammar",
          "level": "beginner",
          "duration": 30
        },
        {
          "lessonId": "lesson_002",
          "title": "Basic Vocabulary",
          "description": "Essential words for beginners",
          "topic": "vocabulary",
          "level": "beginner",
          "duration": 25
        }
      ]
    }
  }
}
```

**Error Cases:**
- Invalid session token
- Session expired

---

#### 3.2.2 Get Lesson Detail

**Purpose**: Retrieve full content of a specific lesson.

**Request** (`GET_LESSON_DETAIL_REQUEST`):
```json
{
  "messageType": "GET_LESSON_DETAIL_REQUEST",
  "messageId": "msg_11_12351",
  "timestamp": 1703721600000,
  "payload": {
    "sessionToken": "a1b2c3d4e5f6...64chars...",
    "lessonId": "lesson_001"
  }
}
```

**Response** (`GET_LESSON_DETAIL_RESPONSE`):
```json
{
  "messageType": "GET_LESSON_DETAIL_RESPONSE",
  "messageId": "msg_11_12351",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "data": {
      "lessonId": "lesson_001",
      "title": "Introduction to English Grammar",
      "description": "Learn the basics of English grammar",
      "textContent": "Grammar is the foundation of any language...",
      "topic": "grammar",
      "level": "beginner",
      "duration": 30,
      "videoUrl": "https://example.com/videos/lesson001.mp4",
      "audioUrl": ""
    }
  }
}
```

| Response Field | Type | Description |
|----------------|------|-------------|
| lessonId | string | Unique lesson identifier |
| title | string | Lesson title |
| description | string | Brief description |
| textContent | string | Full lesson text content |
| topic | string | Topic category |
| level | string | Difficulty level |
| duration | number | Duration in minutes |
| videoUrl | string | Video URL (empty if none) |
| audioUrl | string | Audio URL (empty if none) |

**Error Cases:**
- Invalid session token
- Lesson not found

---

### 3.3 Tests

#### 3.3.1 Get Test

**Purpose**: Retrieve a test with questions.

**Request** (`GET_TEST_REQUEST`):
```json
{
  "messageType": "GET_TEST_REQUEST",
  "messageId": "msg_20_12360",
  "timestamp": 1703721600000,
  "payload": {
    "sessionToken": "a1b2c3d4e5f6...64chars...",
    "testId": "test_001"
  }
}
```

**Response** (`GET_TEST_RESPONSE`):
```json
{
  "messageType": "GET_TEST_RESPONSE",
  "messageId": "msg_20_12360",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "data": {
      "testId": "test_001",
      "title": "Grammar Basics Quiz",
      "testType": "quiz",
      "level": "beginner",
      "topic": "grammar",
      "questions": [
        {
          "questionId": "q_001",
          "type": "multiple_choice",
          "question": "Which is the correct form?",
          "options": ["He go", "He goes", "He going", "He goed"],
          "points": 10
        },
        {
          "questionId": "q_002",
          "type": "fill_blank",
          "question": "She ___ to school every day.",
          "points": 10
        },
        {
          "questionId": "q_003",
          "type": "sentence_order",
          "question": "Arrange the words to form a correct sentence:",
          "words": ["the", "cat", "sat", "on", "mat", "the"],
          "points": 15
        }
      ]
    }
  }
}
```

**Question Types:**

| Type | Description | Additional Fields |
|------|-------------|-------------------|
| `multiple_choice` | Select one correct answer | `options`: array of choices |
| `fill_blank` | Fill in the blank | None |
| `sentence_order` | Arrange words correctly | `words`: array of words to arrange |

**Error Cases:**
- Invalid session token
- Test not found

---

#### 3.3.2 Submit Test

**Purpose**: Submit test answers for grading.

**Request** (`SUBMIT_TEST_REQUEST`):
```json
{
  "messageType": "SUBMIT_TEST_REQUEST",
  "messageId": "msg_21_12361",
  "timestamp": 1703721600000,
  "payload": {
    "sessionToken": "a1b2c3d4e5f6...64chars...",
    "testId": "test_001",
    "answers": [
      {
        "questionId": "q_001",
        "answer": "He goes"
      },
      {
        "questionId": "q_002",
        "answer": "goes"
      },
      {
        "questionId": "q_003",
        "answer": "the cat sat on the mat"
      }
    ]
  }
}
```

**Response** (`SUBMIT_TEST_RESPONSE`):
```json
{
  "messageType": "SUBMIT_TEST_RESPONSE",
  "messageId": "msg_21_12361",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "data": {
      "testId": "test_001",
      "score": 25,
      "maxScore": 35,
      "percentage": 71.4,
      "results": [
        {
          "questionId": "q_001",
          "correct": true,
          "pointsEarned": 10
        },
        {
          "questionId": "q_002",
          "correct": true,
          "pointsEarned": 10
        },
        {
          "questionId": "q_003",
          "correct": false,
          "pointsEarned": 0,
          "correctAnswer": "the cat sat on the mat"
        }
      ]
    }
  }
}
```

**Error Cases:**
- Invalid session token
- Test not found
- Missing answers
- Invalid question ID

---

### 3.4 Exercises

#### 3.4.1 Get Exercise

**Purpose**: Retrieve an exercise assignment.

**Request** (`GET_EXERCISE_REQUEST`):
```json
{
  "messageType": "GET_EXERCISE_REQUEST",
  "messageId": "msg_30_12370",
  "timestamp": 1703721600000,
  "payload": {
    "sessionToken": "a1b2c3d4e5f6...64chars...",
    "exerciseId": "exercise_001"
  }
}
```

**Response** (`GET_EXERCISE_RESPONSE`):
```json
{
  "messageType": "GET_EXERCISE_RESPONSE",
  "messageId": "msg_30_12370",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "data": {
      "exerciseId": "exercise_001",
      "exerciseType": "paragraph_writing",
      "title": "Describe Your Daily Routine",
      "description": "Write a paragraph about your daily activities",
      "instructions": "Write 100-150 words describing your typical day",
      "level": "intermediate",
      "topic": "writing",
      "duration": 20,
      "requirements": [
        "Use present simple tense",
        "Include at least 5 different verbs",
        "Use time expressions"
      ]
    }
  }
}
```

**Exercise Types:**

| Type | Description | Additional Fields |
|------|-------------|-------------------|
| `sentence_rewrite` | Rewrite sentences | `prompts`: array of sentences |
| `paragraph_writing` | Write a paragraph | `topicDescription`, `requirements` |
| `topic_speaking` | Speaking practice | `topicDescription`, audio submission |

**Error Cases:**
- Invalid session token
- Exercise not found

---

#### 3.4.2 Submit Exercise

**Purpose**: Submit exercise work for teacher review.

**Request** (`SUBMIT_EXERCISE_REQUEST`):
```json
{
  "messageType": "SUBMIT_EXERCISE_REQUEST",
  "messageId": "msg_31_12371",
  "timestamp": 1703721600000,
  "payload": {
    "sessionToken": "a1b2c3d4e5f6...64chars...",
    "exerciseId": "exercise_001",
    "content": "Every day I wake up at 7 AM. First, I brush my teeth and take a shower..."
  }
}
```

**Response** (`SUBMIT_EXERCISE_RESPONSE`):
```json
{
  "messageType": "SUBMIT_EXERCISE_RESPONSE",
  "messageId": "msg_31_12371",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "data": {
      "submissionId": "sub_001",
      "exerciseId": "exercise_001",
      "status": "pending",
      "submittedAt": 1703721600100
    }
  }
}
```

**Error Cases:**
- Invalid session token
- Exercise not found
- Empty content

---

### 3.5 Games

#### 3.5.1 Get Game List

**Purpose**: Retrieve available learning games.

**Request** (`GET_GAME_LIST_REQUEST`):
```json
{
  "messageType": "GET_GAME_LIST_REQUEST",
  "messageId": "msg_40_12380",
  "timestamp": 1703721600000,
  "payload": {
    "sessionToken": "a1b2c3d4e5f6...64chars...",
    "level": "beginner"
  }
}
```

**Response** (`GET_GAME_LIST_RESPONSE`):
```json
{
  "messageType": "GET_GAME_LIST_RESPONSE",
  "messageId": "msg_40_12380",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "data": {
      "games": [
        {
          "gameId": "game_001",
          "gameType": "word_match",
          "title": "Vocabulary Match",
          "description": "Match words with their meanings",
          "level": "beginner",
          "topic": "vocabulary",
          "timeLimit": 120,
          "maxScore": 100
        },
        {
          "gameId": "game_002",
          "gameType": "sentence_match",
          "title": "Sentence Completion",
          "description": "Match sentence halves",
          "level": "beginner",
          "topic": "grammar",
          "timeLimit": 180,
          "maxScore": 100
        }
      ]
    }
  }
}
```

**Game Types:**

| Type | Description |
|------|-------------|
| `word_match` | Match words with definitions |
| `sentence_match` | Match sentence parts |
| `picture_match` | Match words with images |

---

#### 3.5.2 Start Game

**Purpose**: Start a game session.

**Request** (`START_GAME_REQUEST`):
```json
{
  "messageType": "START_GAME_REQUEST",
  "messageId": "msg_41_12381",
  "timestamp": 1703721600000,
  "payload": {
    "sessionToken": "a1b2c3d4e5f6...64chars...",
    "gameId": "game_001"
  }
}
```

**Response** (`START_GAME_RESPONSE`):
```json
{
  "messageType": "START_GAME_RESPONSE",
  "messageId": "msg_41_12381",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "data": {
      "gameSessionId": "gsession_001",
      "gameId": "game_001",
      "gameType": "word_match",
      "startTime": 1703721600100,
      "timeLimit": 120,
      "pairs": [
        {"word": "happy", "meaning": "feeling joy"},
        {"word": "sad", "meaning": "feeling sorrow"},
        {"word": "angry", "meaning": "feeling rage"}
      ]
    }
  }
}
```

---

#### 3.5.3 Submit Game Result

**Purpose**: Submit completed game results.

**Request** (`SUBMIT_GAME_RESULT_REQUEST`):
```json
{
  "messageType": "SUBMIT_GAME_RESULT_REQUEST",
  "messageId": "msg_42_12382",
  "timestamp": 1703721600000,
  "payload": {
    "sessionToken": "a1b2c3d4e5f6...64chars...",
    "gameSessionId": "gsession_001",
    "score": 85,
    "completedAt": 1703721720000
  }
}
```

**Response** (`SUBMIT_GAME_RESULT_RESPONSE`):
```json
{
  "messageType": "SUBMIT_GAME_RESULT_RESPONSE",
  "messageId": "msg_42_12382",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "data": {
      "gameSessionId": "gsession_001",
      "score": 85,
      "maxScore": 100,
      "percentage": 85.0,
      "duration": 120,
      "grade": "B"
    }
  }
}
```

---

### 3.6 Chat

#### 3.6.1 Get Contact List

**Purpose**: Retrieve list of users available for chat.

**Request** (`GET_CONTACT_LIST_REQUEST`):
```json
{
  "messageType": "GET_CONTACT_LIST_REQUEST",
  "messageId": "msg_50_12390",
  "timestamp": 1703721600000,
  "payload": {
    "sessionToken": "a1b2c3d4e5f6...64chars..."
  }
}
```

**Response** (`GET_CONTACT_LIST_RESPONSE`):
```json
{
  "messageType": "GET_CONTACT_LIST_RESPONSE",
  "messageId": "msg_50_12390",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "data": {
      "contacts": [
        {
          "userId": "user_002",
          "fullname": "Jane Smith",
          "role": "teacher",
          "online": true
        },
        {
          "userId": "user_003",
          "fullname": "Bob Johnson",
          "role": "student",
          "online": false
        }
      ]
    }
  }
}
```

---

#### 3.6.2 Send Message

**Purpose**: Send a chat message to another user.

**Request** (`SEND_MESSAGE_REQUEST`):
```json
{
  "messageType": "SEND_MESSAGE_REQUEST",
  "messageId": "msg_51_12391",
  "timestamp": 1703721600000,
  "payload": {
    "sessionToken": "a1b2c3d4e5f6...64chars...",
    "recipientId": "user_002",
    "content": "Hello, I have a question about the grammar lesson."
  }
}
```

**Response** (`SEND_MESSAGE_RESPONSE`):
```json
{
  "messageType": "SEND_MESSAGE_RESPONSE",
  "messageId": "msg_51_12391",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "data": {
      "chatMessageId": "chat_001",
      "senderId": "user_001",
      "recipientId": "user_002",
      "content": "Hello, I have a question about the grammar lesson.",
      "timestamp": 1703721600100,
      "read": false
    }
  }
}
```

**Error Cases:**
- Invalid session token
- Recipient not found
- Empty message content

---

#### 3.6.3 Get Chat History

**Purpose**: Retrieve message history with a specific user.

**Request** (`GET_CHAT_HISTORY_REQUEST`):
```json
{
  "messageType": "GET_CHAT_HISTORY_REQUEST",
  "messageId": "msg_52_12392",
  "timestamp": 1703721600000,
  "payload": {
    "sessionToken": "a1b2c3d4e5f6...64chars...",
    "otherUserId": "user_002",
    "limit": 50,
    "beforeTimestamp": 1703721600000
  }
}
```

| Field | Required | Description |
|-------|----------|-------------|
| sessionToken | Yes | Valid session token |
| otherUserId | Yes | User to get chat history with |
| limit | No | Max messages to return (default: 50) |
| beforeTimestamp | No | Pagination: get messages before this time |

**Response** (`GET_CHAT_HISTORY_RESPONSE`):
```json
{
  "messageType": "GET_CHAT_HISTORY_RESPONSE",
  "messageId": "msg_52_12392",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "data": {
      "messages": [
        {
          "messageId": "chat_001",
          "senderId": "user_001",
          "recipientId": "user_002",
          "content": "Hello, I have a question about the grammar lesson.",
          "timestamp": 1703721500000,
          "read": true
        },
        {
          "messageId": "chat_002",
          "senderId": "user_002",
          "recipientId": "user_001",
          "content": "Sure, what would you like to know?",
          "timestamp": 1703721550000,
          "read": true
        }
      ]
    }
  }
}
```

---

#### 3.6.4 Mark Messages Read

**Purpose**: Mark messages from a user as read.

**Request** (`MARK_MESSAGES_READ_REQUEST`):
```json
{
  "messageType": "MARK_MESSAGES_READ_REQUEST",
  "messageId": "msg_53_12393",
  "timestamp": 1703721600000,
  "payload": {
    "sessionToken": "a1b2c3d4e5f6...64chars...",
    "senderId": "user_002"
  }
}
```

**Response** (`MARK_MESSAGES_READ_RESPONSE`):
```json
{
  "messageType": "MARK_MESSAGES_READ_RESPONSE",
  "messageId": "msg_53_12393",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "message": "Messages marked as read"
  }
}
```

---

#### 3.6.5 Push Notifications (Server to Client)

**Receive Message** (`RECEIVE_MESSAGE`):

Pushed when a new message arrives for the connected user.

```json
{
  "messageType": "RECEIVE_MESSAGE",
  "messageId": "msg_push_001",
  "timestamp": 1703721600000,
  "payload": {
    "messageId": "chat_003",
    "senderId": "user_002",
    "senderName": "Jane Smith",
    "content": "Don't forget about tomorrow's lesson!",
    "timestamp": 1703721600000
  }
}
```

**Unread Messages Notification** (`UNREAD_MESSAGES_NOTIFICATION`):

Pushed to notify user of unread message count.

```json
{
  "messageType": "UNREAD_MESSAGES_NOTIFICATION",
  "messageId": "msg_push_002",
  "timestamp": 1703721600000,
  "payload": {
    "unreadCount": 3,
    "fromUsers": [
      {"userId": "user_002", "count": 2},
      {"userId": "user_003", "count": 1}
    ]
  }
}
```

---

### 3.7 Voice Call

#### 3.7.1 Initiate Voice Call

**Purpose**: Start a voice call with another user.

**Request** (`VOICE_CALL_INITIATE_REQUEST`):
```json
{
  "messageType": "VOICE_CALL_INITIATE_REQUEST",
  "messageId": "msg_60_12400",
  "sessionToken": "a1b2c3d4e5f6...64chars...",
  "timestamp": 1703721600000,
  "payload": {
    "calleeId": "user_002"
  }
}
```

**Response** (`VOICE_CALL_INITIATE_RESPONSE`):
```json
{
  "messageType": "VOICE_CALL_INITIATE_RESPONSE",
  "messageId": "msg_60_12400",
  "timestamp": 1703721600100,
  "payload": {
    "status": "success",
    "data": {
      "callId": "call_001",
      "callerId": "user_001",
      "calleeId": "user_002",
      "status": "ringing",
      "startTime": 1703721600100
    }
  }
}
```

**Push to Callee** (`VOICE_CALL_INCOMING`):
```json
{
  "messageType": "VOICE_CALL_INCOMING",
  "timestamp": 1703721600100,
  "payload": {
    "callId": "call_001",
    "callerId": "user_001",
    "callerName": "John Doe"
  }
}
```

---

#### 3.7.2 Accept Voice Call

**Request** (`VOICE_CALL_ACCEPT_REQUEST`):
```json
{
  "messageType": "VOICE_CALL_ACCEPT_REQUEST",
  "sessionToken": "callee_token...",
  "payload": {
    "callId": "call_001"
  }
}
```

**Push to Caller** (`VOICE_CALL_ACCEPTED`):
```json
{
  "messageType": "VOICE_CALL_ACCEPTED",
  "payload": {
    "callId": "call_001",
    "acceptedAt": 1703721610000
  }
}
```

---

#### 3.7.3 Reject Voice Call

**Request** (`VOICE_CALL_REJECT_REQUEST`):
```json
{
  "messageType": "VOICE_CALL_REJECT_REQUEST",
  "sessionToken": "callee_token...",
  "payload": {
    "callId": "call_001"
  }
}
```

**Push to Caller** (`VOICE_CALL_REJECTED`):
```json
{
  "messageType": "VOICE_CALL_REJECTED",
  "payload": {
    "callId": "call_001",
    "rejectedAt": 1703721615000
  }
}
```

---

#### 3.7.4 End Voice Call

**Request** (`VOICE_CALL_END_REQUEST`):
```json
{
  "messageType": "VOICE_CALL_END_REQUEST",
  "sessionToken": "user_token...",
  "payload": {
    "callId": "call_001"
  }
}
```

**Push to Other Party** (`VOICE_CALL_ENDED`):
```json
{
  "messageType": "VOICE_CALL_ENDED",
  "payload": {
    "callId": "call_001",
    "endedAt": 1703721700000,
    "duration": 90
  }
}
```

---

### 3.8 Teacher Feedback

#### 3.8.1 Get User Submissions

**Purpose**: Retrieve all exercise submissions for a user to view teacher feedback.

**Request** (`GET_USER_SUBMISSIONS_REQUEST`):
```json
{
  "messageType": "GET_USER_SUBMISSIONS_REQUEST",
  "sessionToken": "student_token...",
  "payload": {}
}
```

**Response** (`GET_USER_SUBMISSIONS_RESPONSE`):
```json
{
  "messageType": "GET_USER_SUBMISSIONS_RESPONSE",
  "payload": {
    "status": "success",
    "data": {
      "submissions": [
        {
          "submissionId": "sub_001",
          "exerciseId": "exercise_001",
          "exerciseTitle": "Daily Routine Writing",
          "content": "Every day I wake up...",
          "status": "reviewed",
          "submittedAt": 1703721600000,
          "feedback": "Great work! Consider using more varied vocabulary.",
          "score": 85
        }
      ]
    }
  }
}
```

---

### 3.9 Error Handling

#### 3.9.1 Error Response Format

**Message Type**: `ERROR_RESPONSE`

All errors follow a consistent format:

```json
{
  "messageType": "ERROR_RESPONSE",
  "messageId": "msg_original_id",
  "timestamp": 1703721600100,
  "payload": {
    "status": "error",
    "message": "Human-readable error description",
    "code": "ERROR_CODE"
  }
}
```

#### 3.7.2 Common Error Codes

| Code | Description |
|------|-------------|
| `INVALID_SESSION` | Session token is invalid or missing |
| `SESSION_EXPIRED` | Session has expired (1 hour timeout) |
| `USER_NOT_FOUND` | Referenced user does not exist |
| `INVALID_CREDENTIALS` | Login email/password incorrect |
| `RESOURCE_NOT_FOUND` | Requested lesson/test/exercise not found |
| `PERMISSION_DENIED` | User lacks permission for action |
| `VALIDATION_ERROR` | Request payload validation failed |
| `DUPLICATE_EMAIL` | Email already registered |
| `INTERNAL_ERROR` | Server-side error |

#### 3.7.3 Error Examples

**Invalid Session:**
```json
{
  "messageType": "ERROR_RESPONSE",
  "messageId": "msg_99_12399",
  "timestamp": 1703721600100,
  "payload": {
    "status": "error",
    "message": "Session token is invalid or has expired",
    "code": "INVALID_SESSION"
  }
}
```

**Resource Not Found:**
```json
{
  "messageType": "ERROR_RESPONSE",
  "messageId": "msg_100_12400",
  "timestamp": 1703721600100,
  "payload": {
    "status": "error",
    "message": "Lesson with ID 'lesson_999' not found",
    "code": "RESOURCE_NOT_FOUND"
  }
}
```

---

## Appendix A: Message Type Constants

```
# Authentication
REGISTER_REQUEST / REGISTER_RESPONSE
LOGIN_REQUEST / LOGIN_RESPONSE
SET_LEVEL_REQUEST / SET_LEVEL_RESPONSE

# Lessons
GET_LESSONS_REQUEST / GET_LESSONS_RESPONSE
GET_LESSON_DETAIL_REQUEST / GET_LESSON_DETAIL_RESPONSE

# Tests
GET_TEST_REQUEST / GET_TEST_RESPONSE
SUBMIT_TEST_REQUEST / SUBMIT_TEST_RESPONSE

# Exercises
GET_EXERCISE_REQUEST / GET_EXERCISE_RESPONSE
SUBMIT_EXERCISE_REQUEST / SUBMIT_EXERCISE_RESPONSE
GET_PENDING_REVIEWS_REQUEST / GET_PENDING_REVIEWS_RESPONSE
REVIEW_EXERCISE_REQUEST / REVIEW_EXERCISE_RESPONSE
GET_FEEDBACK_REQUEST / GET_FEEDBACK_RESPONSE

# Games
GET_GAME_LIST_REQUEST / GET_GAME_LIST_RESPONSE
START_GAME_REQUEST / START_GAME_RESPONSE
SUBMIT_GAME_RESULT_REQUEST / SUBMIT_GAME_RESULT_RESPONSE
ADD_GAME_REQUEST / ADD_GAME_RESPONSE
UPDATE_GAME_REQUEST / UPDATE_GAME_RESPONSE
DELETE_GAME_REQUEST / DELETE_GAME_RESPONSE
GET_ADMIN_GAMES_REQUEST / GET_ADMIN_GAMES_RESPONSE

# Chat
GET_CONTACT_LIST_REQUEST / GET_CONTACT_LIST_RESPONSE
SEND_MESSAGE_REQUEST / SEND_MESSAGE_RESPONSE
GET_CHAT_HISTORY_REQUEST / GET_CHAT_HISTORY_RESPONSE
MARK_MESSAGES_READ_REQUEST / MARK_MESSAGES_READ_RESPONSE

# Voice Call
VOICE_CALL_INITIATE_REQUEST / VOICE_CALL_INITIATE_RESPONSE
VOICE_CALL_ACCEPT_REQUEST / VOICE_CALL_ACCEPT_RESPONSE
VOICE_CALL_REJECT_REQUEST / VOICE_CALL_REJECT_RESPONSE
VOICE_CALL_END_REQUEST / VOICE_CALL_END_RESPONSE
VOICE_CALL_GET_STATUS_REQUEST / VOICE_CALL_GET_STATUS_RESPONSE

# Voice Call Push Notifications
VOICE_CALL_INCOMING
VOICE_CALL_ACCEPTED
VOICE_CALL_REJECTED
VOICE_CALL_ENDED

# Teacher Feedback
GET_USER_SUBMISSIONS_REQUEST / GET_USER_SUBMISSIONS_RESPONSE

# Push Notifications
RECEIVE_MESSAGE
UNREAD_MESSAGES_NOTIFICATION
EXERCISE_FEEDBACK_NOTIFICATION

# Error
ERROR_RESPONSE
```

---

## Appendix B: Session Token

- **Length**: 64 characters
- **Character Set**: Alphanumeric (a-z, A-Z, 0-9)
- **Expiration**: 1 hour from creation
- **Validation**: Required for all requests except `LOGIN_REQUEST` and `REGISTER_REQUEST`

---

## Appendix C: Timestamp Format

- **Type**: 64-bit signed integer
- **Unit**: Milliseconds since Unix epoch (January 1, 1970 00:00:00 UTC)
- **Display Format**: "YYYY-MM-DD HH:MM:SS" (for logging/debugging)
