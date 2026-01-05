#ifndef ENGLISH_LEARNING_REPOSITORY_I_EXERCISE_REPOSITORY_H
#define ENGLISH_LEARNING_REPOSITORY_I_EXERCISE_REPOSITORY_H

#include <string>
#include <vector>
#include <optional>
#include "include/core/exercise.h"

namespace english_learning {
namespace repository {

/**
 * Interface for exercise data access operations.
 * Provides CRUD operations for Exercise and ExerciseSubmission entities.
 */
class IExerciseRepository {
public:
    virtual ~IExerciseRepository() = default;

    // ===== Exercise operations =====

    // Create
    virtual bool addExercise(const core::Exercise& exercise) = 0;

    // Read
    virtual std::optional<core::Exercise> findExerciseById(const std::string& exerciseId) const = 0;
    virtual std::vector<core::Exercise> findAllExercises() const = 0;
    virtual std::vector<core::Exercise> findExercisesByLevel(const std::string& level) const = 0;
    virtual std::vector<core::Exercise> findExercisesByType(const std::string& type) const = 0;
    virtual std::vector<core::Exercise> findExercisesByLevelAndType(
        const std::string& level, const std::string& type) const = 0;
    virtual bool exerciseExists(const std::string& exerciseId) const = 0;

    // Update
    virtual bool updateExercise(const core::Exercise& exercise) = 0;

    // Delete
    virtual bool removeExercise(const std::string& exerciseId) = 0;

    // ===== Submission operations =====

    // Create
    virtual bool addSubmission(const core::ExerciseSubmission& submission) = 0;

    // Read
    virtual std::optional<core::ExerciseSubmission> findSubmissionById(
        const std::string& submissionId) const = 0;
    virtual std::vector<core::ExerciseSubmission> findAllSubmissions() const = 0;
    virtual std::vector<core::ExerciseSubmission> findSubmissionsByUser(
        const std::string& userId) const = 0;
    virtual std::vector<core::ExerciseSubmission> findSubmissionsByExercise(
        const std::string& exerciseId) const = 0;
    virtual std::vector<core::ExerciseSubmission> findPendingSubmissions() const = 0;
    virtual std::vector<core::ExerciseSubmission> findReviewedSubmissions(
        const std::string& userId) const = 0;
    virtual std::vector<core::ExerciseSubmission> findDraftsByUser(
        const std::string& userId) const = 0;
    virtual std::optional<core::ExerciseSubmission> findDraftByUserAndExercise(
        const std::string& userId, const std::string& exerciseId) const = 0;

    // Update
    virtual bool updateSubmission(const core::ExerciseSubmission& submission) = 0;
    virtual bool reviewSubmission(const std::string& submissionId,
                                  const std::string& teacherId,
                                  const std::string& feedback,
                                  int score,
                                  int64_t reviewedAt) = 0;
    virtual bool reviewSubmissionWithScores(const std::string& submissionId,
                                            const std::string& teacherId,
                                            const std::string& feedback,
                                            const core::ScoreBreakdown& scores,
                                            int64_t reviewedAt) = 0;
    virtual bool saveDraft(const std::string& submissionId,
                           const std::string& content,
                           const std::string& audioUrl,
                           int64_t updatedAt) = 0;
    virtual bool submitForReview(const std::string& submissionId,
                                 int64_t submittedAt) = 0;

    // Delete
    virtual bool removeSubmission(const std::string& submissionId) = 0;

    // Utility
    virtual size_t countExercises() const = 0;
    virtual size_t countSubmissions() const = 0;
    virtual size_t countPendingSubmissions() const = 0;
    virtual size_t countDrafts(const std::string& userId) const = 0;
    virtual int getAttemptCount(const std::string& userId, const std::string& exerciseId) const = 0;
};

} // namespace repository
} // namespace english_learning

#endif // ENGLISH_LEARNING_REPOSITORY_I_EXERCISE_REPOSITORY_H
