#ifndef CLIENT_BRIDGE_H
#define CLIENT_BRIDGE_H

#include <string>
#include <iostream>
#include "include/protocol/json_parser.h"

// --- BIẾN DÙNG CHUNG (Shared Variables) ---
// Khai báo extern để gui_main.cpp hiểu là các biến này đang nằm bên client.cpp
extern std::string sessionToken; // Token đăng nhập
extern std::string currentLevel; // Level hiện tại (beginner/intermediate...)
extern std::string currentRole;  // Role: student, teacher, admin
extern bool running;             // Trạng thái chương trình

// --- CÁC HÀM DÙNG CHUNG (Shared Functions) ---
bool connectToServer(const char* ip, int port);
bool sendMessage(const std::string& message);
std::string waitForResponse(int timeoutMs = 3000);
void receiveThreadFunc();

// Use namespace-qualified JSON functions from protocol library
using english_learning::protocol::getJsonValue;
using english_learning::protocol::getJsonObject;
using english_learning::protocol::getJsonArray;
using english_learning::protocol::parseJsonArray;
using english_learning::protocol::escapeJson;

#endif