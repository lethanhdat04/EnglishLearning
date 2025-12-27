#ifndef ENGLISH_LEARNING_CORE_EXERCISE_H
#define ENGLISH_LEARNING_CORE_EXERCISE_H

#include <string>
#include <vector>
#include "types.h"

namespace english_learning {
namespace core {

/**
 * Exercise entity representing a practice exercise.
 * Exercises can be sentence rewriting, paragraph writing, or speaking topics.
 */
struct Exercise {
    std::string exerciseId;
    std::string exerciseType;       // sentence_rewrite, paragraph_writing, topic_speaking
    std::string title;
    std::string description;
    std::string instructions;
    std::string level;
    std::string topic;
    std::vector<std::string> prompts;           // For sentence_rewrite: original sentences
    std::string topicDescription;               // For topic_speaking and paragraph_writing
    std::vector<std::string> requirements;      // For paragraph_writing: requirements list
    int duration;                               // Duration in minutes

    Exercise() : duration(0) {}

    Exercise(const std::string& id, const std::string& type, const std::string& t,
             const std::string& desc, const std::string& lvl, int dur)
        : exerciseId(id), exerciseType(type), title(t), description(desc),
          level(lvl), duration(dur) {}

    // Check if this is a sentence rewrite exercise
    bool isSentenceRewrite() const {
        return exerciseType == "sentence_rewrite";
    }

    // Check if this is a paragraph writing exercise
    bool isParagraphWriting() const {
        return exerciseType == "paragraph_writing";
    }

    // Check if this is a topic speaking exercise
    bool isTopicSpeaking() const {
        return exerciseType == "topic_speaking";
    }

    // Check if exercise matches given level
    bool matchesLevel(const std::string& lvl) const {
        return level == lvl;
    }
};

/**
 * ExerciseSubmission entity representing a student's submission for an exercise.
 * Submissions can be reviewed by teachers who provide feedback and scores.
 */
struct ExerciseSubmission {
    std::string submissionId;
    std::string exerciseId;
    std::string userId;
    std::string exerciseType;
    std::string content;            // Answers or written text or audio URL
    std::string status;             // pending, reviewed
    Timestamp submittedAt;
    std::string teacherId;          // Who reviewed it
    std::string teacherFeedback;
    int teacherScore;               // 0-100
    Timestamp reviewedAt;

    ExerciseSubmission()
        : submittedAt(0), teacherScore(0), reviewedAt(0) {}

    ExerciseSubmission(const std::string& subId, const std::string& exId,
                       const std::string& uid, const std::string& type,
                       const std::string& cont, Timestamp ts)
        : submissionId(subId), exerciseId(exId), userId(uid),
          exerciseType(type), content(cont), status("pending"),
          submittedAt(ts), teacherScore(0), reviewedAt(0) {}

    // Check if submission is pending review
    bool isPending() const {
        return status == "pending";
    }

    // Check if submission has been reviewed
    bool isReviewed() const {
        return status == "reviewed";
    }

    // Set review data
    void setReview(const std::string& teacher, const std::string& feedback,
                   int score, Timestamp ts) {
        teacherId = teacher;
        teacherFeedback = feedback;
        teacherScore = score;
        reviewedAt = ts;
        status = "reviewed";
    }

    // Check if submission belongs to a specific user
    bool belongsTo(const std::string& uid) const {
        return userId == uid;
    }
};

} // namespace core
} // namespace english_learning

#endif // ENGLISH_LEARNING_CORE_EXERCISE_H
