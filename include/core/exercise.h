#ifndef ENGLISH_LEARNING_CORE_EXERCISE_H
#define ENGLISH_LEARNING_CORE_EXERCISE_H

#include <string>
#include <vector>
#include <map>
#include "types.h"

namespace english_learning {
namespace core {

/**
 * Exercise type constants
 */
namespace ExerciseTypes {
    constexpr const char* SENTENCE_REWRITE = "sentence_rewrite";
    constexpr const char* PARAGRAPH_WRITING = "paragraph_writing";
    constexpr const char* TOPIC_SPEAKING = "topic_speaking";
}

/**
 * Score breakdown for detailed feedback
 */
struct ScoreBreakdown {
    int grammar;        // Grammar accuracy (0-100)
    int vocabulary;     // Vocabulary usage (0-100)
    int coherence;      // Logical flow and coherence (0-100)
    int pronunciation;  // For speaking exercises (0-100)
    int overall;        // Overall score (0-100)

    ScoreBreakdown() : grammar(0), vocabulary(0), coherence(0), pronunciation(0), overall(0) {}

    // Calculate weighted average
    int calculateOverall(bool isSpeaking = false) const {
        if (isSpeaking) {
            return (grammar * 25 + vocabulary * 25 + coherence * 25 + pronunciation * 25) / 100;
        }
        return (grammar * 35 + vocabulary * 35 + coherence * 30) / 100;
    }
};

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
    int minWordCount;                           // Minimum word count for writing
    int maxWordCount;                           // Maximum word count for writing
    std::string rubric;                         // Grading rubric description
    std::string createdBy;                      // Teacher who created it

    Exercise() : duration(0), minWordCount(0), maxWordCount(0) {}

    Exercise(const std::string& id, const std::string& type, const std::string& t,
             const std::string& desc, const std::string& lvl, int dur)
        : exerciseId(id), exerciseType(type), title(t), description(desc),
          level(lvl), duration(dur), minWordCount(0), maxWordCount(0) {}

    // Check if this is a sentence rewrite exercise
    bool isSentenceRewrite() const {
        return exerciseType == ExerciseTypes::SENTENCE_REWRITE;
    }

    // Check if this is a paragraph writing exercise
    bool isParagraphWriting() const {
        return exerciseType == ExerciseTypes::PARAGRAPH_WRITING;
    }

    // Check if this is a topic speaking exercise
    bool isTopicSpeaking() const {
        return exerciseType == ExerciseTypes::TOPIC_SPEAKING;
    }

    // Check if exercise requires audio submission
    bool requiresAudio() const {
        return isTopicSpeaking();
    }

    // Check if exercise matches given level
    bool matchesLevel(const std::string& lvl) const {
        return level == lvl;
    }

    // Check if exercise matches given type
    bool matchesType(const std::string& type) const {
        return exerciseType == type;
    }
};

/**
 * ExerciseSubmission entity representing a student's submission for an exercise.
 * Submissions can be reviewed by teachers who provide feedback and scores.
 *
 * Workflow states:
 * - draft: Student is working on it, not yet submitted
 * - pending_review: Submitted, waiting for teacher review
 * - reviewed: Teacher has provided feedback
 */
struct ExerciseSubmission {
    std::string submissionId;
    std::string exerciseId;
    std::string userId;
    std::string exerciseType;
    std::string content;            // Written text for writing exercises
    std::string audioUrl;           // Audio file path/URL for speaking exercises
    std::string status;             // draft, pending_review, reviewed
    Timestamp createdAt;            // When draft was first created
    Timestamp submittedAt;          // When submitted for review
    std::string teacherId;          // Who reviewed it
    std::string teacherFeedback;    // Overall feedback comment
    ScoreBreakdown scores;          // Detailed score breakdown
    int teacherScore;               // Overall score 0-100 (legacy compatibility)
    Timestamp reviewedAt;
    int wordCount;                  // Word count of submission
    int attemptNumber;              // Which attempt this is (1, 2, 3...)

    ExerciseSubmission()
        : createdAt(0), submittedAt(0), teacherScore(0), reviewedAt(0),
          wordCount(0), attemptNumber(1) {}

    ExerciseSubmission(const std::string& subId, const std::string& exId,
                       const std::string& uid, const std::string& type,
                       const std::string& cont, Timestamp ts)
        : submissionId(subId), exerciseId(exId), userId(uid),
          exerciseType(type), content(cont), status(SubmissionStatusStr::PENDING_REVIEW),
          createdAt(ts), submittedAt(ts), teacherScore(0), reviewedAt(0),
          wordCount(0), attemptNumber(1) {}

    // Check if submission is a draft
    bool isDraft() const {
        return status == SubmissionStatusStr::DRAFT;
    }

    // Check if submission is pending review
    bool isPendingReview() const {
        return status == SubmissionStatusStr::PENDING_REVIEW;
    }

    // Check if submission has been reviewed
    bool isReviewed() const {
        return status == SubmissionStatusStr::REVIEWED;
    }

    // Legacy compatibility
    bool isPending() const {
        return isPendingReview() || status == "pending";
    }

    // Submit draft for review
    void submit(Timestamp ts) {
        status = SubmissionStatusStr::PENDING_REVIEW;
        submittedAt = ts;
    }

    // Save as draft
    void saveDraft(const std::string& newContent, Timestamp ts) {
        content = newContent;
        status = SubmissionStatusStr::DRAFT;
        if (createdAt == 0) createdAt = ts;
    }

    // Set review data with detailed scores
    void setReview(const std::string& teacher, const std::string& feedback,
                   const ScoreBreakdown& scoreBreakdown, Timestamp ts) {
        teacherId = teacher;
        teacherFeedback = feedback;
        scores = scoreBreakdown;
        teacherScore = scoreBreakdown.overall;
        reviewedAt = ts;
        status = SubmissionStatusStr::REVIEWED;
    }

    // Legacy setReview for compatibility
    void setReview(const std::string& teacher, const std::string& feedback,
                   int score, Timestamp ts) {
        teacherId = teacher;
        teacherFeedback = feedback;
        teacherScore = score;
        scores.overall = score;
        reviewedAt = ts;
        status = SubmissionStatusStr::REVIEWED;
    }

    // Check if submission belongs to a specific user
    bool belongsTo(const std::string& uid) const {
        return userId == uid;
    }

    // Check if this is a speaking submission
    bool isSpeakingSubmission() const {
        return exerciseType == ExerciseTypes::TOPIC_SPEAKING;
    }

    // Get display status string
    std::string getDisplayStatus() const {
        if (status == SubmissionStatusStr::DRAFT) return "Draft";
        if (status == SubmissionStatusStr::PENDING_REVIEW || status == "pending") return "Under Review";
        if (status == SubmissionStatusStr::REVIEWED || status == "reviewed") return "Feedback Received";
        return status;
    }
};

/**
 * Notification entity for exercise-related notifications
 */
struct ExerciseNotification {
    std::string notificationId;
    std::string userId;             // Recipient
    std::string type;               // new_submission, feedback_received
    std::string submissionId;
    std::string message;
    Timestamp createdAt;
    bool read;

    ExerciseNotification() : createdAt(0), read(false) {}
};

} // namespace core
} // namespace english_learning

#endif // ENGLISH_LEARNING_CORE_EXERCISE_H
