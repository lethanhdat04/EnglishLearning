#ifndef ENGLISH_LEARNING_CORE_TEST_H
#define ENGLISH_LEARNING_CORE_TEST_H

#include <string>
#include <vector>
#include "types.h"

namespace english_learning {
namespace core {

/**
 * TestQuestion entity representing a single question in a test.
 * Supports multiple question types: multiple choice, fill-in-the-blank, sentence ordering.
 */
struct TestQuestion {
    std::string questionId;
    std::string type;           // multiple_choice, fill_blank, sentence_order
    std::string question;
    std::vector<std::string> options;
    std::string correctAnswer;
    std::vector<std::string> words;  // For sentence_order type
    int points;

    TestQuestion() : points(10) {}

    TestQuestion(const std::string& id, const std::string& t, const std::string& q,
                 const std::string& correct, int pts = 10)
        : questionId(id), type(t), question(q), correctAnswer(correct), points(pts) {}

    // Check if answer is correct
    bool checkAnswer(const std::string& answer) const {
        if (type == "multiple_choice") {
            return answer == correctAnswer;
        } else if (type == "fill_blank") {
            // Case-insensitive comparison for fill blank
            std::string lowerAnswer = answer;
            std::string lowerCorrect = correctAnswer;
            for (char& c : lowerAnswer) c = std::tolower(c);
            for (char& c : lowerCorrect) c = std::tolower(c);
            return lowerAnswer == lowerCorrect;
        } else if (type == "sentence_order") {
            // For sentence order, compare the full sentence
            return answer == correctAnswer;
        }
        return false;
    }

    // Check if this is a multiple choice question
    bool isMultipleChoice() const {
        return type == "multiple_choice";
    }

    // Check if this is a fill-in-the-blank question
    bool isFillBlank() const {
        return type == "fill_blank";
    }

    // Check if this is a sentence ordering question
    bool isSentenceOrder() const {
        return type == "sentence_order";
    }
};

/**
 * Test entity representing a collection of questions.
 * Tests have a specific type, level, and topic.
 */
struct Test {
    std::string testId;
    std::string testType;
    std::string level;
    std::string topic;
    std::string title;
    std::vector<TestQuestion> questions;

    Test() = default;

    Test(const std::string& id, const std::string& type, const std::string& lvl,
         const std::string& top, const std::string& t)
        : testId(id), testType(type), level(lvl), topic(top), title(t) {}

    // Add a question to the test
    void addQuestion(const TestQuestion& q) {
        questions.push_back(q);
    }

    // Get total possible points
    int getTotalPoints() const {
        int total = 0;
        for (const auto& q : questions) {
            total += q.points;
        }
        return total;
    }

    // Get number of questions
    size_t getQuestionCount() const {
        return questions.size();
    }

    // Check if test matches given level
    bool matchesLevel(const std::string& lvl) const {
        return level == lvl;
    }
};

} // namespace core
} // namespace english_learning

#endif // ENGLISH_LEARNING_CORE_TEST_H
