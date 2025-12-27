#include "game_service.h"
#include "include/protocol/utils.h"
#include <algorithm>

namespace english_learning {
namespace service {

GameService::GameService(
    repository::IGameRepository& gameRepo,
    repository::IUserRepository& userRepo)
    : gameRepo_(gameRepo)
    , userRepo_(userRepo) {}

ServiceResult<GameListResult> GameService::getGames(const std::string& level) {
    std::vector<core::Game> games;

    if (level.empty()) {
        games = gameRepo_.findAllGames();
    } else {
        games = gameRepo_.findGamesByLevel(level);
    }

    GameListResult result;
    result.games = games;
    result.total = games.size();

    return ServiceResult<GameListResult>::success(result);
}

ServiceResult<core::Game> GameService::getGameById(const std::string& gameId) {
    auto gameOpt = gameRepo_.findGameById(gameId);
    if (!gameOpt.has_value()) {
        return ServiceResult<core::Game>::error("Game not found");
    }
    return ServiceResult<core::Game>::success(gameOpt.value());
}

ServiceResult<GameStartResult> GameService::startGame(
    const std::string& userId,
    const std::string& gameId) {

    auto gameOpt = gameRepo_.findGameById(gameId);
    if (!gameOpt.has_value()) {
        return ServiceResult<GameStartResult>::error("Game not found");
    }

    const core::Game& game = gameOpt.value();

    // Create a new game session
    core::GameSession session;
    session.sessionId = protocol::utils::generateId("gsess");
    session.gameId = gameId;
    session.userId = userId;
    session.startTime = protocol::utils::getCurrentTimestamp();
    session.endTime = 0;
    session.score = 0;
    session.maxScore = game.maxScore;
    session.completed = false;

    if (!gameRepo_.addSession(session)) {
        return ServiceResult<GameStartResult>::error("Failed to start game session");
    }

    GameStartResult result;
    result.sessionId = session.sessionId;
    result.gameId = gameId;
    result.gameType = game.gameType;
    result.pairs = game.getActivePairs();
    result.timeLimit = game.timeLimit;
    result.maxScore = game.maxScore;

    return ServiceResult<GameStartResult>::success(result);
}

ServiceResult<GameSessionResult> GameService::submitGameMatches(
    const std::string& userId,
    const std::string& sessionId,
    const std::map<std::string, std::string>& matches) {

    auto sessionOpt = gameRepo_.findSessionById(sessionId);
    if (!sessionOpt.has_value()) {
        return ServiceResult<GameSessionResult>::error("Game session not found");
    }

    core::GameSession session = sessionOpt.value();

    if (session.userId != userId) {
        return ServiceResult<GameSessionResult>::error("Session does not belong to user");
    }

    if (session.completed) {
        return ServiceResult<GameSessionResult>::error("Session already completed");
    }

    auto gameOpt = gameRepo_.findGameById(session.gameId);
    if (!gameOpt.has_value()) {
        return ServiceResult<GameSessionResult>::error("Game not found");
    }

    const core::Game& game = gameOpt.value();
    const auto& correctPairs = game.getActivePairs();

    // Calculate score based on correct matches
    int correctCount = 0;
    for (const auto& pair : correctPairs) {
        auto it = matches.find(pair.first);
        if (it != matches.end() && it->second == pair.second) {
            correctCount++;
        }
    }

    int score = correctPairs.empty() ? 0 :
        (correctCount * game.maxScore) / static_cast<int>(correctPairs.size());

    long long endTime = protocol::utils::getCurrentTimestamp();

    // Complete the session
    if (!gameRepo_.completeSession(sessionId, score, endTime)) {
        return ServiceResult<GameSessionResult>::error("Failed to complete session");
    }

    double percentage = game.maxScore == 0 ? 0.0 :
        (static_cast<double>(score) / game.maxScore) * 100.0;

    std::string grade;
    if (percentage >= 90) grade = "Excellent";
    else if (percentage >= 80) grade = "Good";
    else if (percentage >= 70) grade = "Fair";
    else if (percentage >= 60) grade = "Pass";
    else grade = "Fail";

    GameSessionResult result;
    result.sessionId = sessionId;
    result.gameId = session.gameId;
    result.oderId = userId;
    result.score = score;
    result.maxScore = game.maxScore;
    result.percentage = percentage;
    result.grade = grade;
    result.durationSeconds = static_cast<int>((endTime - session.startTime) / 1000);

    return ServiceResult<GameSessionResult>::success(result);
}

ServiceResult<std::vector<core::GameSession>> GameService::getUserGameHistory(
    const std::string& userId,
    const std::string& gameId) {

    std::vector<core::GameSession> sessions;

    if (gameId.empty()) {
        sessions = gameRepo_.findSessionsByUser(userId);
    } else {
        auto allSessions = gameRepo_.findSessionsByUser(userId);
        for (const auto& s : allSessions) {
            if (s.gameId == gameId) {
                sessions.push_back(s);
            }
        }
    }

    return ServiceResult<std::vector<core::GameSession>>::success(sessions);
}

ServiceResult<LeaderboardResult> GameService::getLeaderboard(
    const std::string& gameId,
    size_t limit) {

    auto sessions = gameRepo_.findSessionsByGame(gameId);

    // Group by user and find high scores
    std::map<std::string, int> highScores;
    std::map<std::string, int> gamesPlayed;
    std::map<std::string, int> totalScores;

    for (const auto& session : sessions) {
        if (session.completed) {
            gamesPlayed[session.userId]++;
            totalScores[session.userId] += session.score;
            if (highScores.find(session.userId) == highScores.end() ||
                session.score > highScores[session.userId]) {
                highScores[session.userId] = session.score;
            }
        }
    }

    // Build leaderboard entries
    std::vector<LeaderboardEntry> entries;
    for (const auto& [userId, highScore] : highScores) {
        auto userOpt = userRepo_.findById(userId);
        if (userOpt.has_value()) {
            LeaderboardEntry entry;
            entry.userId = userId;
            entry.fullname = userOpt.value().fullname;
            entry.highScore = highScore;
            entry.gamesPlayed = gamesPlayed[userId];
            entry.averageScore = static_cast<double>(totalScores[userId]) / gamesPlayed[userId];
            entries.push_back(entry);
        }
    }

    // Sort by high score descending
    std::sort(entries.begin(), entries.end(), [](const auto& a, const auto& b) {
        return a.highScore > b.highScore;
    });

    // Apply limit
    if (limit > 0 && entries.size() > limit) {
        entries.resize(limit);
    }

    LeaderboardResult result;
    result.gameId = gameId;
    result.entries = entries;
    result.total = entries.size();

    return ServiceResult<LeaderboardResult>::success(result);
}

ServiceResult<core::Game> GameService::createGame(
    const std::string& userId,
    const core::Game& game) {

    if (!userRepo_.isAdmin(userId)) {
        return ServiceResult<core::Game>::error("Only admins can create games");
    }

    if (game.title.empty()) {
        return ServiceResult<core::Game>::error("Title is required");
    }

    core::Game newGame = game;
    newGame.gameId = protocol::utils::generateId("game");

    if (!gameRepo_.addGame(newGame)) {
        return ServiceResult<core::Game>::error("Failed to create game");
    }

    return ServiceResult<core::Game>::success(newGame);
}

VoidResult GameService::deleteGame(
    const std::string& userId,
    const std::string& gameId) {

    if (!userRepo_.isAdmin(userId)) {
        return VoidResult::error("Only admins can delete games");
    }

    if (!gameRepo_.removeGame(gameId)) {
        return VoidResult::error("Failed to delete game");
    }

    return VoidResult::success();
}

} // namespace service
} // namespace english_learning
