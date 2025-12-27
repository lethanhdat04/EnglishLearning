#ifndef ENGLISH_LEARNING_SERVICE_I_GAME_SERVICE_H
#define ENGLISH_LEARNING_SERVICE_I_GAME_SERVICE_H

#include <string>
#include <vector>
#include <map>
#include "service_result.h"
#include "include/core/game.h"

namespace english_learning {
namespace service {

/**
 * DTO for game list response.
 */
struct GameListResult {
    std::vector<core::Game> games;
    size_t total;
};

/**
 * DTO for starting a game session.
 */
struct GameStartResult {
    std::string sessionId;
    std::string gameId;
    std::string gameType;
    std::vector<std::pair<std::string, std::string>> pairs;
    int timeLimit;
    int maxScore;
};

/**
 * DTO for game session result.
 */
struct GameSessionResult {
    std::string sessionId;
    std::string gameId;
    std::string oderId;
    int score;
    int maxScore;
    double percentage;
    std::string grade;
    int durationSeconds;
};

/**
 * DTO for leaderboard entry.
 */
struct LeaderboardEntry {
    std::string userId;
    std::string fullname;
    int highScore;
    int gamesPlayed;
    double averageScore;
};

/**
 * DTO for leaderboard response.
 */
struct LeaderboardResult {
    std::string gameId;
    std::vector<LeaderboardEntry> entries;
    size_t total;
};

/**
 * Interface for game management services.
 */
class IGameService {
public:
    virtual ~IGameService() = default;

    /**
     * Get all available games.
     * @param level Filter by level (empty for all)
     * @return List of games
     */
    virtual ServiceResult<GameListResult> getGames(
        const std::string& level = "") = 0;

    /**
     * Get a specific game by ID.
     * @param gameId The game ID
     * @return Game if found, error otherwise
     */
    virtual ServiceResult<core::Game> getGameById(const std::string& gameId) = 0;

    /**
     * Start a new game session.
     * @param userId The user starting the game
     * @param gameId The game to start
     * @return Game session start data
     */
    virtual ServiceResult<GameStartResult> startGame(
        const std::string& userId,
        const std::string& gameId) = 0;

    /**
     * Submit matches for a game session.
     * @param userId The user submitting
     * @param sessionId The game session ID
     * @param matches User's matches (key -> value pairs)
     * @return Game session result with score
     */
    virtual ServiceResult<GameSessionResult> submitGameMatches(
        const std::string& userId,
        const std::string& sessionId,
        const std::map<std::string, std::string>& matches) = 0;

    /**
     * Get user's game history.
     * @param userId The user's ID
     * @param gameId Optional filter by game (empty for all)
     * @return List of game sessions
     */
    virtual ServiceResult<std::vector<core::GameSession>> getUserGameHistory(
        const std::string& userId,
        const std::string& gameId = "") = 0;

    /**
     * Get leaderboard for a game.
     * @param gameId The game ID
     * @param limit Maximum entries to return
     * @return Leaderboard entries
     */
    virtual ServiceResult<LeaderboardResult> getLeaderboard(
        const std::string& gameId,
        size_t limit = 10) = 0;

    /**
     * Create a new game (admin only).
     * @param userId The user creating the game
     * @param game The game to create
     * @return Created game on success
     */
    virtual ServiceResult<core::Game> createGame(
        const std::string& userId,
        const core::Game& game) = 0;

    /**
     * Delete a game (admin only).
     * @param userId The user deleting the game
     * @param gameId The game to delete
     * @return Success or error
     */
    virtual VoidResult deleteGame(
        const std::string& userId,
        const std::string& gameId) = 0;
};

} // namespace service
} // namespace english_learning

#endif // ENGLISH_LEARNING_SERVICE_I_GAME_SERVICE_H
