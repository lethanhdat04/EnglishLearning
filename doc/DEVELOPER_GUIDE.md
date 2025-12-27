# English Learning Application - Developer Guide

This guide covers building, running, and troubleshooting the English Learning application.

## Table of Contents

1. [Build Requirements](#1-build-requirements)
2. [How to Build](#2-how-to-build)
3. [How to Run](#3-how-to-run)
4. [Common Issues](#4-common-issues)

---

## 1. Build Requirements

### 1.1 Compiler

| Requirement | Minimum | Recommended |
|-------------|---------|-------------|
| C++ Standard | C++17 | C++17 or later |
| GCC | 7.0+ | 9.0+ |
| Clang | 5.0+ | 10.0+ |

The project uses C++17 features including:
- `std::string_view`
- Structured bindings
- `std::optional`
- Inline variables

### 1.2 Libraries

| Library | Purpose | Required For |
|---------|---------|--------------|
| POSIX Threads | Multithreading | Server, Client, GUI |
| POSIX Sockets | Network communication | Server, Client, GUI |
| GTK+ 3.0 | Graphical interface | GUI client only |

**GTK+ 3.0 Installation:**

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install libgtk-3-dev pkg-config

# Fedora/RHEL
sudo dnf install gtk3-devel pkgconfig

# Arch Linux
sudo pacman -S gtk3 pkgconf

# macOS (with Homebrew)
brew install gtk+3 pkg-config
```

**Verify GTK installation:**

```bash
pkg-config --modversion gtk+-3.0
# Expected output: 3.24.x (or similar)
```

### 1.3 Build System

The project uses **GNU Make** (not CMake).

```bash
# Verify make is installed
make --version

# Install if needed (Ubuntu/Debian)
sudo apt-get install build-essential
```

### 1.4 Platform Support

| Platform | Status | Notes |
|----------|--------|-------|
| Linux | Fully supported | Primary development platform |
| WSL/WSL2 | Fully supported | Requires WSLg for GUI |
| macOS | Supported | May need minor adjustments |
| Windows (native) | Not supported | Use WSL instead |

---

## 2. How to Build

### 2.1 Quick Start

```bash
# Clone/navigate to project directory
cd /path/to/EnglishLearning

# Build everything (server, console client, GUI)
make all
```

### 2.2 Build Targets

#### Build Server Only

```bash
make server
```

**Output:** `./server`

**What it compiles:**
- `server.cpp` - Main server code
- `src/protocol/*.cpp` - JSON parser
- `src/repository/memory/*.cpp` - In-memory data storage
- `src/service/*.cpp` - Business logic services

#### Build Console Client Only

```bash
make client
```

**Output:** `./client`

**What it compiles:**
- `client.cpp` - Console client with interactive menu
- `src/protocol/json_parser.cpp` - JSON parsing utilities

#### Build GUI Client Only

```bash
make gui
```

**Output:** `./gui_app`

**What it compiles:**
- `gui_main.cpp` - GTK+ graphical interface
- `client.cpp` - Network layer (with `CLIENT_SKIP_MAIN` defined)
- `src/protocol/json_parser.cpp` - JSON parsing utilities

**Note:** Requires GTK+ 3.0 development libraries.

### 2.3 Clean Build

```bash
# Remove all compiled binaries
make clean

# Full rebuild
make clean && make all
```

### 2.4 Build Flags

The Makefile uses these compiler flags:

| Flag | Purpose |
|------|---------|
| `-std=c++17` | Enable C++17 standard |
| `-Wall` | Enable all warnings |
| `-Wextra` | Enable extra warnings |
| `-pthread` | Enable POSIX threads |
| `-O2` | Optimization level 2 |
| `-I.` | Include project root for headers |

### 2.5 Verbose Build

To see exact commands being executed:

```bash
make VERBOSE=1 server
```

Or manually:

```bash
g++ -std=c++17 -Wall -Wextra -pthread -O2 -I. \
    -o server server.cpp \
    src/protocol/json_parser.cpp \
    src/repository/memory/memory_user_repository.cpp \
    src/repository/memory/memory_session_repository.cpp \
    src/repository/memory/memory_repositories.cpp \
    src/service/auth_service.cpp \
    src/service/lesson_service.cpp \
    src/service/test_service.cpp \
    src/service/chat_service.cpp \
    src/service/exercise_service.cpp \
    src/service/game_service.cpp
```

---

## 3. How to Run

### 3.1 Start Server

**Using Make:**

```bash
make run-server
```

**Direct execution:**

```bash
# Default port (8888)
./server

# Custom port
./server 9000
```

**Expected output:**

```
╔══════════════════════════════════════════╗
║     ENGLISH LEARNING SERVER              ║
╚══════════════════════════════════════════╝
[INFO] Starting server on port 8888...
[INFO] Server is running. Waiting for connections...
[INFO] Initialized 3 users, 6 lessons, 3 tests, 3 exercises, 3 games
```

### 3.2 Start Console Client

**Using Make:**

```bash
make run-client
```

**Direct execution:**

```bash
# Connect to localhost:8888
./client

# Connect to specific server
./client 192.168.1.100 8888

# Connect to custom port
./client 127.0.0.1 9000
```

**Expected output:**

```
╔══════════════════════════════════════════╗
║       ENGLISH LEARNING APP               ║
╚══════════════════════════════════════════╝
Connecting to server 127.0.0.1:8888...
[SUCCESS] Connected to server!
```

### 3.3 Start GUI Client

**Using Make:**

```bash
make run-gui
```

**Direct execution:**

```bash
./gui_app
```

**Note for WSL users:** Requires WSLg (Windows 11) or an X server (Windows 10).

```bash
# For Windows 10 with X server (e.g., VcXsrv)
export DISPLAY=:0
./gui_app
```

### 3.4 Configuration Options

#### Server Configuration

| Option | Default | Environment Variable | Description |
|--------|---------|---------------------|-------------|
| Port | 8888 | - | TCP port to listen on |
| Max Clients | 100 | - | Maximum concurrent connections |
| Buffer Size | 65536 | - | Message buffer size in bytes |

```bash
# Start with custom port
./server 9000
```

#### Client Configuration

| Option | Default | Description |
|--------|---------|-------------|
| Server IP | 127.0.0.1 | Target server address |
| Port | 8888 | Target server port |
| Buffer Size | 65536 | Message buffer size |

```bash
# Connect with custom settings
./client <server_ip> <port>
./client 192.168.1.50 9000
```

### 3.5 Test Accounts

The server initializes with sample data:

| Email | Password | Role | Level |
|-------|----------|------|-------|
| `student@example.com` | `student123` | Student | Beginner |
| `sarah@example.com` | `teacher123` | Teacher | Advanced |
| `john@example.com` | `teacher123` | Teacher | Advanced |

### 3.6 Running in Background

**Server:**

```bash
# Run server in background with log
nohup ./server 8888 > server_output.log 2>&1 &

# Check if running
ps aux | grep server

# Stop server
pkill -f "./server"
```

**Using systemd (production):**

```bash
# Create service file
sudo nano /etc/systemd/system/english-learning.service
```

```ini
[Unit]
Description=English Learning Server
After=network.target

[Service]
Type=simple
User=youruser
WorkingDirectory=/path/to/EnglishLearning
ExecStart=/path/to/EnglishLearning/server 8888
Restart=on-failure
RestartSec=5

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl enable english-learning
sudo systemctl start english-learning
sudo systemctl status english-learning
```

---

## 4. Common Issues

### 4.1 GTK Not Found

**Error:**

```
Package gtk+-3.0 was not found in the pkg-config search path.
```

**Solution:**

```bash
# Install GTK development package
sudo apt-get install libgtk-3-dev

# Verify installation
pkg-config --exists gtk+-3.0 && echo "GTK found" || echo "GTK not found"

# Check pkg-config path
pkg-config --variable pc_path pkg-config
```

**Alternative (build without GUI):**

```bash
# Build only server and console client
make server client
```

### 4.2 Port Already in Use

**Error:**

```
[ERROR] Cannot bind to port 8888: Address already in use
```

**Solution:**

```bash
# Find process using the port
sudo lsof -i :8888
# or
sudo netstat -tlnp | grep 8888

# Kill the process
sudo kill -9 <PID>

# Or use a different port
./server 9000
./client 127.0.0.1 9000
```

**Prevent issue:**

```bash
# Wait for socket to release (TIME_WAIT state)
# This typically takes 60-120 seconds

# Or enable SO_REUSEADDR in code (already implemented)
```

### 4.3 JSON Parse Errors

**Error:**

```
[ERROR] Failed to parse JSON response
```

**Common causes and solutions:**

1. **Malformed JSON from server:**
   ```bash
   # Check server logs
   tail -f server.log
   ```

2. **Message truncation:**
   - Increase `BUFFER_SIZE` in both `server.cpp` and `client.cpp`
   - Default: 65536 bytes

3. **Network issues:**
   ```bash
   # Test connectivity
   nc -zv 127.0.0.1 8888
   ```

4. **Character encoding:**
   - Ensure UTF-8 encoding throughout
   - Special characters in user input may cause issues

**Debug JSON parsing:**

```cpp
// Add debug output in client.cpp
std::cout << "Raw response: " << response << std::endl;
```

### 4.4 Connection Refused

**Error:**

```
[ERROR] Cannot connect to server
```

**Solutions:**

1. **Verify server is running:**
   ```bash
   ps aux | grep "./server"
   ```

2. **Check firewall:**
   ```bash
   # Ubuntu
   sudo ufw allow 8888/tcp

   # CentOS/RHEL
   sudo firewall-cmd --add-port=8888/tcp --permanent
   sudo firewall-cmd --reload
   ```

3. **Verify correct IP/port:**
   ```bash
   # Server should show listening
   netstat -tlnp | grep 8888
   ```

### 4.5 Compilation Errors

**Error: C++17 features not supported**

```
error: 'optional' is not a member of 'std'
```

**Solution:**

```bash
# Check GCC version
g++ --version

# Install newer GCC (Ubuntu)
sudo apt-get install g++-9
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 90
```

**Error: Missing headers**

```
fatal error: gtk/gtk.h: No such file or directory
```

**Solution:**

```bash
sudo apt-get install libgtk-3-dev
```

### 4.6 WSL-Specific Issues

**GUI not displaying:**

```bash
# Check DISPLAY variable
echo $DISPLAY

# For WSL2 with WSLg (Windows 11)
# Should work automatically

# For WSL1 or Windows 10 with X server
export DISPLAY=:0
# or
export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0
```

**Permission denied on Windows filesystem:**

```bash
# Move project to WSL filesystem for better performance
cp -r /mnt/d/EnglishLearning ~/EnglishLearning
cd ~/EnglishLearning
make clean && make all
```

### 4.7 Memory Issues

**Error: Segmentation fault**

**Debug steps:**

```bash
# Compile with debug symbols
g++ -std=c++17 -g -O0 -pthread -I. -o server_debug server.cpp ...

# Run with GDB
gdb ./server_debug
(gdb) run
(gdb) bt  # backtrace when crash occurs

# Run with Valgrind
valgrind --leak-check=full ./server 8888
```

### 4.8 Thread Issues

**Error: Deadlock or unresponsive server**

**Debug steps:**

```bash
# Check thread status
ps -eLf | grep server

# Attach GDB to running process
gdb -p <PID>
(gdb) info threads
(gdb) thread apply all bt
```

---

## Appendix A: Project Structure

```
EnglishLearning/
├── Makefile                 # Build configuration
├── server.cpp               # Server main file
├── client.cpp               # Console client
├── gui_main.cpp             # GTK+ GUI client
├── client_bridge.h          # Shared client declarations
├── include/
│   ├── core/                # Domain models
│   │   ├── user.h
│   │   ├── lesson.h
│   │   ├── test.h
│   │   └── ...
│   ├── protocol/            # Protocol definitions
│   │   ├── message_types.h
│   │   ├── json_parser.h
│   │   └── json_builder.h
│   ├── repository/          # Repository interfaces
│   └── service/             # Service interfaces
├── src/
│   ├── protocol/            # Protocol implementations
│   ├── repository/          # Repository implementations
│   └── service/             # Service implementations
└── doc/                     # Documentation
```

---

## Appendix B: Quick Reference

### Build Commands

```bash
make all          # Build everything
make server       # Build server only
make client       # Build console client
make gui          # Build GUI client
make clean        # Remove binaries
```

### Run Commands

```bash
./server [port]                    # Start server
./client [server_ip] [port]        # Start console client
./gui_app                          # Start GUI client
```

### Debug Commands

```bash
tail -f server.log                 # Watch server logs
netstat -tlnp | grep 8888          # Check port status
ps aux | grep server               # Check running processes
```
