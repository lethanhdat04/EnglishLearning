#ifndef ENGLISH_LEARNING_SERVICE_GAME_SERVICE_H
#define ENGLISH_LEARNING_SERVICE_GAME_SERVICE_H

#include "include/service/i_game_service.h"
#include "include/repository/i_game_repository.h"
#include "include/repository/i_user_repository.h"

namespace english_learning {
namespace service {

/**
 * Implementation of game management service.
 */
class GameService : public IGameService {
public:
    GameService(
        repository::IGameRepository& gameRepo,
        repository::IUserRepository& userRepo);

    ~GameService() override = default;

    ServiceResult<GameListResult> getGames(
        const std::string& level = "") override;

    ServiceResult<core::Game> getGameById(const std::string& gameId) override;

    ServiceResult<GameStartResult> startGame(
        const std::string& userId,
        const std::string& gameId) override;

    ServiceResult<GameSessionResult> submitGameMatches(
        const std::string& userId,
        const std::string& sessionId,
        const std::map<std::string, std::string>& matches) override;

    ServiceResult<std::vector<core::GameSession>> getUserGameHistory(
        const std::string& userId,
        const std::string& gameId = "") override;

    ServiceResult<LeaderboardResult> getLeaderboard(
        const std::string& gameId,
        size_t limit = 10) override;

    ServiceResult<core::Game> createGame(
        const std::string& userId,
        const core::Game& game) override;

    VoidResult deleteGame(
        const std::string& userId,
        const std::string& gameId) override;

private:
    repository::IGameRepository& gameRepo_;
    repository::IUserRepository& userRepo_;
};

} // namespace service
} // namespace english_learning

#endif // ENGLISH_LEARNING_SERVICE_GAME_SERVICE_H
