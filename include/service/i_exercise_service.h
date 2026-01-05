#ifndef ENGLISH_LEARNING_SERVICE_I_EXERCISE_SERVICE_H
#define ENGLISH_LEARNING_SERVICE_I_EXERCISE_SERVICE_H

#include <string>
#include <vector>
#include "service_result.h"
#include "include/core/exercise.h"

namespace english_learning {
namespace service {

/**
 * DTO for exercise list response.
 */
struct ExerciseListResult {
    std::vector<core::Exercise> exercises;
    size_t total;
    std::string filterLevel;
    std::string filterType;
};

/**
 * DTO for submission list response.
 */
struct SubmissionListResult {
    std::vector<core::ExerciseSubmission> submissions;
    size_t total;
    size_t pendingCount;
    size_t reviewedCount;
    size_t draftCount;
};

/**
 * DTO for submission detail response (includes exercise info).
 */
struct SubmissionDetailResult {
    core::ExerciseSubmission submission;
    core::Exercise exercise;
    std::string studentName;
    std::string studentEmail;
    int attemptNumber;
};

/**
 * DTO for review statistics.
 */
struct ReviewStatistics {
    size_t totalPending;
    size_t totalReviewed;
    size_t reviewedToday;
    size_t reviewedThisWeek;
    double averageScore;
};

/**
 * Interface for exercise management services.
 * Supports the complete homework/practice workflow for students and teachers.
 */
class IExerciseService {
public:
    virtual ~IExerciseService() = default;

    // ==================== Exercise Management ====================

    /**
     * Get all exercises, optionally filtered by level and type.
     * @param level Filter by level (empty for all)
     * @param type Filter by type (empty for all)
     * @return List of matching exercises
     */
    virtual ServiceResult<ExerciseListResult> getExercises(
        const std::string& level = "",
        const std::string& type = "") = 0;

    /**
     * Get a specific exercise by ID.
     * @param exerciseId The exercise ID
     * @return Exercise if found, error otherwise
     */
    virtual ServiceResult<core::Exercise> getExerciseById(
        const std::string& exerciseId) = 0;

    /**
     * Create a new exercise (teacher/admin only).
     * @param userId The user creating the exercise
     * @param exercise The exercise to create
     * @return Created exercise on success
     */
    virtual ServiceResult<core::Exercise> createExercise(
        const std::string& userId,
        const core::Exercise& exercise) = 0;

    /**
     * Update an existing exercise (teacher/admin only).
     * @param userId The user updating the exercise
     * @param exercise The updated exercise
     * @return Updated exercise on success
     */
    virtual ServiceResult<core::Exercise> updateExercise(
        const std::string& userId,
        const core::Exercise& exercise) = 0;

    /**
     * Delete an exercise (admin only).
     * @param userId The user deleting the exercise
     * @param exerciseId The exercise to delete
     * @return Success or error
     */
    virtual VoidResult deleteExercise(
        const std::string& userId,
        const std::string& exerciseId) = 0;

    // ==================== Student Workflow ====================

    /**
     * Save a draft submission (work in progress).
     * Creates a new draft or updates existing one.
     * @param userId The student's ID
     * @param exerciseId The exercise ID
     * @param content The text content (for writing exercises)
     * @param audioUrl The audio file URL (for speaking exercises)
     * @return Created/updated draft submission
     */
    virtual ServiceResult<core::ExerciseSubmission> saveDraft(
        const std::string& userId,
        const std::string& exerciseId,
        const std::string& content,
        const std::string& audioUrl = "") = 0;

    /**
     * Submit an exercise for review.
     * Can submit a new response or submit an existing draft.
     * @param userId The user submitting
     * @param exerciseId The exercise being answered
     * @param content The submission content
     * @param audioUrl The audio URL for speaking exercises
     * @return Created submission on success
     */
    virtual ServiceResult<core::ExerciseSubmission> submitExercise(
        const std::string& userId,
        const std::string& exerciseId,
        const std::string& content,
        const std::string& audioUrl = "") = 0;

    /**
     * Get all submissions for a user (including drafts).
     * @param userId The user's ID
     * @return User's submissions with status counts
     */
    virtual ServiceResult<SubmissionListResult> getUserSubmissions(
        const std::string& userId) = 0;

    /**
     * Get user's draft submissions.
     * @param userId The user's ID
     * @return User's draft submissions
     */
    virtual ServiceResult<SubmissionListResult> getUserDrafts(
        const std::string& userId) = 0;

    /**
     * Get feedback for a specific submission.
     * @param userId The user requesting (must own the submission)
     * @param submissionId The submission ID
     * @return Submission with feedback details
     */
    virtual ServiceResult<core::ExerciseSubmission> getFeedback(
        const std::string& userId,
        const std::string& submissionId) = 0;

    // ==================== Teacher Workflow ====================

    /**
     * Get pending submissions for review (teacher view).
     * @param teacherId The teacher's ID
     * @return Pending submissions queue
     */
    virtual ServiceResult<SubmissionListResult> getPendingSubmissions(
        const std::string& teacherId) = 0;

    /**
     * Get detailed submission information (teacher view).
     * Includes exercise details, student info, and submission content.
     * @param teacherId The teacher's ID
     * @param submissionId The submission to view
     * @return Full submission details
     */
    virtual ServiceResult<SubmissionDetailResult> getSubmissionDetail(
        const std::string& teacherId,
        const std::string& submissionId) = 0;

    /**
     * Review a submission with simple scoring (teacher only).
     * @param teacherId The teacher reviewing
     * @param submissionId The submission to review
     * @param feedback The feedback text
     * @param score The overall score (0-100)
     * @return Updated submission on success
     */
    virtual ServiceResult<core::ExerciseSubmission> reviewSubmission(
        const std::string& teacherId,
        const std::string& submissionId,
        const std::string& feedback,
        int score) = 0;

    /**
     * Review a submission with detailed scoring (teacher only).
     * @param teacherId The teacher reviewing
     * @param submissionId The submission to review
     * @param feedback The feedback text
     * @param scores Detailed score breakdown
     * @return Updated submission on success
     */
    virtual ServiceResult<core::ExerciseSubmission> reviewSubmissionWithScores(
        const std::string& teacherId,
        const std::string& submissionId,
        const std::string& feedback,
        const core::ScoreBreakdown& scores) = 0;

    /**
     * Get review statistics for a teacher.
     * @param teacherId The teacher's ID
     * @return Review statistics
     */
    virtual ServiceResult<ReviewStatistics> getReviewStatistics(
        const std::string& teacherId) = 0;
};

} // namespace service
} // namespace english_learning

#endif // ENGLISH_LEARNING_SERVICE_I_EXERCISE_SERVICE_H
