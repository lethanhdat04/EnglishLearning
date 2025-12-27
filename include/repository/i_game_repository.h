#ifndef ENGLISH_LEARNING_REPOSITORY_I_GAME_REPOSITORY_H
#define ENGLISH_LEARNING_REPOSITORY_I_GAME_REPOSITORY_H

#include <string>
#include <vector>
#include <optional>
#include "include/core/game.h"

namespace english_learning {
namespace repository {

/**
 * Interface for game data access operations.
 * Provides CRUD operations for Game and GameSession entities.
 */
class IGameRepository {
public:
    virtual ~IGameRepository() = default;

    // ===== Game operations =====

    // Create
    virtual bool addGame(const core::Game& game) = 0;

    // Read
    virtual std::optional<core::Game> findGameById(const std::string& gameId) const = 0;
    virtual std::vector<core::Game> findAllGames() const = 0;
    virtual std::vector<core::Game> findGamesByLevel(const std::string& level) const = 0;
    virtual std::vector<core::Game> findGamesByType(const std::string& gameType) const = 0;
    virtual std::vector<core::Game> findGamesByLevelAndType(const std::string& level,
                                                             const std::string& gameType) const = 0;
    virtual bool gameExists(const std::string& gameId) const = 0;

    // Update
    virtual bool updateGame(const core::Game& game) = 0;

    // Delete
    virtual bool removeGame(const std::string& gameId) = 0;

    // ===== GameSession operations =====

    // Create
    virtual bool addSession(const core::GameSession& session) = 0;

    // Read
    virtual std::optional<core::GameSession> findSessionById(
        const std::string& sessionId) const = 0;
    virtual std::vector<core::GameSession> findSessionsByUser(
        const std::string& userId) const = 0;
    virtual std::vector<core::GameSession> findSessionsByGame(
        const std::string& gameId) const = 0;
    virtual std::vector<core::GameSession> findActiveSessionsByUser(
        const std::string& userId) const = 0;

    // Update
    virtual bool updateSession(const core::GameSession& session) = 0;
    virtual bool completeSession(const std::string& sessionId,
                                 int score,
                                 int64_t endTime) = 0;

    // Utility
    virtual size_t countGames() const = 0;
    virtual size_t countSessions() const = 0;
};

} // namespace repository
} // namespace english_learning

#endif // ENGLISH_LEARNING_REPOSITORY_I_GAME_REPOSITORY_H
