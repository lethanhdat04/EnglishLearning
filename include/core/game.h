#ifndef ENGLISH_LEARNING_CORE_GAME_H
#define ENGLISH_LEARNING_CORE_GAME_H

#include <string>
#include <vector>
#include <map>
#include <utility>
#include "types.h"

namespace english_learning {
namespace core {

/**
 * Game entity representing a learning game.
 * Games include word matching, sentence matching, and picture matching.
 */
struct Game {
    std::string gameId;
    std::string gameType;       // word_match, sentence_match, picture_match
    std::string title;
    std::string description;
    std::string level;
    std::string topic;
    std::vector<std::pair<std::string, std::string>> pairs;         // For word_match: (word, meaning)
    std::vector<std::pair<std::string, std::string>> sentencePairs; // For sentence_match
    std::vector<std::pair<std::string, std::string>> picturePairs;  // For picture_match: (word, imageUrl)
    int timeLimit;              // Time limit in seconds
    int maxScore;

    Game() : timeLimit(60), maxScore(100) {}

    Game(const std::string& id, const std::string& type, const std::string& t,
         const std::string& desc, const std::string& lvl, int time, int max)
        : gameId(id), gameType(type), title(t), description(desc),
          level(lvl), timeLimit(time), maxScore(max) {}

    // Check if this is a word matching game
    bool isWordMatch() const {
        return gameType == "word_match";
    }

    // Check if this is a sentence matching game
    bool isSentenceMatch() const {
        return gameType == "sentence_match";
    }

    // Check if this is a picture matching game
    bool isPictureMatch() const {
        return gameType == "picture_match";
    }

    // Get the appropriate pairs based on game type
    const std::vector<std::pair<std::string, std::string>>& getActivePairs() const {
        if (gameType == "sentence_match") {
            return sentencePairs;
        } else if (gameType == "picture_match") {
            return picturePairs;
        }
        return pairs;
    }

    // Get number of pairs in the game
    size_t getPairCount() const {
        return getActivePairs().size();
    }

    // Check if game matches given level
    bool matchesLevel(const std::string& lvl) const {
        return level == lvl;
    }
};

/**
 * GameSession entity representing an active game session for a user.
 * Tracks game progress, answers, and final score.
 */
struct GameSession {
    std::string sessionId;
    std::string gameId;
    std::string userId;
    Timestamp startTime;
    Timestamp endTime;
    int score;
    int maxScore;
    std::map<std::string, std::string> answers;     // For tracking matches
    bool completed;

    GameSession() : startTime(0), endTime(0), score(0), maxScore(0), completed(false) {}

    GameSession(const std::string& sId, const std::string& gId,
                const std::string& uId, Timestamp start, int max)
        : sessionId(sId), gameId(gId), userId(uId),
          startTime(start), endTime(0), score(0), maxScore(max), completed(false) {}

    // Mark session as completed
    void complete(int finalScore, Timestamp end) {
        score = finalScore;
        endTime = end;
        completed = true;
    }

    // Check if session is still active
    bool isActive() const {
        return !completed;
    }

    // Get duration in seconds (only valid after completion)
    int getDurationSeconds() const {
        if (!completed) return 0;
        return static_cast<int>((endTime - startTime) / 1000);
    }

    // Calculate score percentage
    double getScorePercentage() const {
        if (maxScore == 0) return 0.0;
        return (static_cast<double>(score) / maxScore) * 100.0;
    }

    // Get grade based on score percentage
    std::string getGrade() const {
        double pct = getScorePercentage();
        if (pct >= 90) return "Excellent";
        if (pct >= 80) return "Good";
        if (pct >= 70) return "Fair";
        if (pct >= 60) return "Pass";
        return "Fail";
    }
};

} // namespace core
} // namespace english_learning

#endif // ENGLISH_LEARNING_CORE_GAME_H
