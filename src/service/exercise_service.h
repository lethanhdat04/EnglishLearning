#ifndef ENGLISH_LEARNING_SERVICE_EXERCISE_SERVICE_H
#define ENGLISH_LEARNING_SERVICE_EXERCISE_SERVICE_H

#include "include/service/i_exercise_service.h"
#include "include/repository/i_exercise_repository.h"
#include "include/repository/i_user_repository.h"

namespace english_learning {
namespace service {

/**
 * Implementation of exercise management service.
 */
class ExerciseService : public IExerciseService {
public:
    ExerciseService(
        repository::IExerciseRepository& exerciseRepo,
        repository::IUserRepository& userRepo);

    ~ExerciseService() override = default;

    ServiceResult<ExerciseListResult> getExercises(
        const std::string& level = "",
        const std::string& type = "") override;

    ServiceResult<core::Exercise> getExerciseById(
        const std::string& exerciseId) override;

    ServiceResult<core::Exercise> createExercise(
        const std::string& userId,
        const core::Exercise& exercise) override;

    ServiceResult<core::ExerciseSubmission> submitExercise(
        const std::string& userId,
        const std::string& exerciseId,
        const std::string& content) override;

    ServiceResult<SubmissionListResult> getUserSubmissions(
        const std::string& userId) override;

    ServiceResult<SubmissionListResult> getPendingSubmissions(
        const std::string& teacherId) override;

    ServiceResult<core::ExerciseSubmission> reviewSubmission(
        const std::string& teacherId,
        const std::string& submissionId,
        const std::string& feedback,
        int score) override;

    ServiceResult<core::Exercise> updateExercise(
        const std::string& userId,
        const core::Exercise& exercise) override;

    VoidResult deleteExercise(
        const std::string& userId,
        const std::string& exerciseId) override;

private:
    repository::IExerciseRepository& exerciseRepo_;
    repository::IUserRepository& userRepo_;
};

} // namespace service
} // namespace english_learning

#endif // ENGLISH_LEARNING_SERVICE_EXERCISE_SERVICE_H
