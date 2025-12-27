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
};

/**
 * DTO for submission list response.
 */
struct SubmissionListResult {
    std::vector<core::ExerciseSubmission> submissions;
    size_t total;
};

/**
 * Interface for exercise management services.
 */
class IExerciseService {
public:
    virtual ~IExerciseService() = default;

    /**
     * Get all exercises, optionally filtered.
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
     * Submit a response for an exercise.
     * @param userId The user submitting
     * @param exerciseId The exercise being answered
     * @param content The submission content
     * @return Created submission on success
     */
    virtual ServiceResult<core::ExerciseSubmission> submitExercise(
        const std::string& userId,
        const std::string& exerciseId,
        const std::string& content) = 0;

    /**
     * Get submissions for a user.
     * @param userId The user's ID
     * @return User's submissions
     */
    virtual ServiceResult<SubmissionListResult> getUserSubmissions(
        const std::string& userId) = 0;

    /**
     * Get pending submissions (teacher view).
     * @param teacherId The teacher's ID
     * @return Pending submissions
     */
    virtual ServiceResult<SubmissionListResult> getPendingSubmissions(
        const std::string& teacherId) = 0;

    /**
     * Review a submission (teacher only).
     * @param teacherId The teacher reviewing
     * @param submissionId The submission to review
     * @param feedback The feedback text
     * @param score The score (0-100)
     * @return Updated submission on success
     */
    virtual ServiceResult<core::ExerciseSubmission> reviewSubmission(
        const std::string& teacherId,
        const std::string& submissionId,
        const std::string& feedback,
        int score) = 0;

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
};

} // namespace service
} // namespace english_learning

#endif // ENGLISH_LEARNING_SERVICE_I_EXERCISE_SERVICE_H
