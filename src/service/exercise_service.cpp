#include "exercise_service.h"
#include "include/protocol/utils.h"

namespace english_learning {
namespace service {

ExerciseService::ExerciseService(
    repository::IExerciseRepository& exerciseRepo,
    repository::IUserRepository& userRepo)
    : exerciseRepo_(exerciseRepo)
    , userRepo_(userRepo) {}

ServiceResult<ExerciseListResult> ExerciseService::getExercises(
    const std::string& level,
    const std::string& type) {

    std::vector<core::Exercise> exercises;

    if (level.empty() && type.empty()) {
        exercises = exerciseRepo_.findAllExercises();
    } else if (!level.empty() && type.empty()) {
        exercises = exerciseRepo_.findExercisesByLevel(level);
    } else if (level.empty() && !type.empty()) {
        exercises = exerciseRepo_.findExercisesByType(type);
    } else {
        // Filter by both level and type
        auto byLevel = exerciseRepo_.findExercisesByLevel(level);
        for (const auto& ex : byLevel) {
            if (ex.exerciseType == type) {
                exercises.push_back(ex);
            }
        }
    }

    ExerciseListResult result;
    result.exercises = exercises;
    result.total = exercises.size();

    return ServiceResult<ExerciseListResult>::success(result);
}

ServiceResult<core::Exercise> ExerciseService::getExerciseById(
    const std::string& exerciseId) {

    auto exerciseOpt = exerciseRepo_.findExerciseById(exerciseId);
    if (!exerciseOpt.has_value()) {
        return ServiceResult<core::Exercise>::error("Exercise not found");
    }
    return ServiceResult<core::Exercise>::success(exerciseOpt.value());
}

ServiceResult<core::Exercise> ExerciseService::createExercise(
    const std::string& userId,
    const core::Exercise& exercise) {

    if (!userRepo_.isTeacher(userId)) {
        return ServiceResult<core::Exercise>::error("Only teachers can create exercises");
    }

    if (exercise.title.empty()) {
        return ServiceResult<core::Exercise>::error("Title is required");
    }

    core::Exercise newExercise = exercise;
    newExercise.exerciseId = protocol::utils::generateId("exercise");

    if (!exerciseRepo_.addExercise(newExercise)) {
        return ServiceResult<core::Exercise>::error("Failed to create exercise");
    }

    return ServiceResult<core::Exercise>::success(newExercise);
}

ServiceResult<core::ExerciseSubmission> ExerciseService::submitExercise(
    const std::string& userId,
    const std::string& exerciseId,
    const std::string& content) {

    auto exerciseOpt = exerciseRepo_.findExerciseById(exerciseId);
    if (!exerciseOpt.has_value()) {
        return ServiceResult<core::ExerciseSubmission>::error("Exercise not found");
    }

    const core::Exercise& exercise = exerciseOpt.value();

    core::ExerciseSubmission submission;
    submission.submissionId = protocol::utils::generateId("sub");
    submission.exerciseId = exerciseId;
    submission.userId = userId;
    submission.exerciseType = exercise.exerciseType;
    submission.content = content;
    submission.status = "pending";
    submission.submittedAt = protocol::utils::getCurrentTimestamp();
    submission.teacherScore = 0;
    submission.reviewedAt = 0;

    if (!exerciseRepo_.addSubmission(submission)) {
        return ServiceResult<core::ExerciseSubmission>::error("Failed to submit exercise");
    }

    return ServiceResult<core::ExerciseSubmission>::success(submission);
}

ServiceResult<SubmissionListResult> ExerciseService::getUserSubmissions(
    const std::string& userId) {

    auto submissions = exerciseRepo_.findSubmissionsByUser(userId);

    SubmissionListResult result;
    result.submissions = submissions;
    result.total = submissions.size();

    return ServiceResult<SubmissionListResult>::success(result);
}

ServiceResult<SubmissionListResult> ExerciseService::getPendingSubmissions(
    const std::string& teacherId) {

    if (!userRepo_.isTeacher(teacherId)) {
        return ServiceResult<SubmissionListResult>::error("Only teachers can view pending submissions");
    }

    auto submissions = exerciseRepo_.findPendingSubmissions();

    SubmissionListResult result;
    result.submissions = submissions;
    result.total = submissions.size();

    return ServiceResult<SubmissionListResult>::success(result);
}

ServiceResult<core::ExerciseSubmission> ExerciseService::reviewSubmission(
    const std::string& teacherId,
    const std::string& submissionId,
    const std::string& feedback,
    int score) {

    if (!userRepo_.isTeacher(teacherId)) {
        return ServiceResult<core::ExerciseSubmission>::error("Only teachers can review submissions");
    }

    auto submissionOpt = exerciseRepo_.findSubmissionById(submissionId);
    if (!submissionOpt.has_value()) {
        return ServiceResult<core::ExerciseSubmission>::error("Submission not found");
    }

    core::ExerciseSubmission submission = submissionOpt.value();

    if (submission.isReviewed()) {
        return ServiceResult<core::ExerciseSubmission>::error("Submission already reviewed");
    }

    long long reviewedAt = protocol::utils::getCurrentTimestamp();
    if (!exerciseRepo_.reviewSubmission(submissionId, teacherId, feedback, score, reviewedAt)) {
        return ServiceResult<core::ExerciseSubmission>::error("Failed to review submission");
    }

    // Update the local copy with review data
    submission.setReview(teacherId, feedback, score, reviewedAt);

    return ServiceResult<core::ExerciseSubmission>::success(submission);
}

ServiceResult<core::Exercise> ExerciseService::updateExercise(
    const std::string& userId,
    const core::Exercise& exercise) {

    if (!userRepo_.isTeacher(userId)) {
        return ServiceResult<core::Exercise>::error("Only teachers can update exercises");
    }

    auto existingOpt = exerciseRepo_.findExerciseById(exercise.exerciseId);
    if (!existingOpt.has_value()) {
        return ServiceResult<core::Exercise>::error("Exercise not found");
    }

    if (!exerciseRepo_.updateExercise(exercise)) {
        return ServiceResult<core::Exercise>::error("Failed to update exercise");
    }

    return ServiceResult<core::Exercise>::success(exercise);
}

VoidResult ExerciseService::deleteExercise(
    const std::string& userId,
    const std::string& exerciseId) {

    if (!userRepo_.isAdmin(userId)) {
        return VoidResult::error("Only admins can delete exercises");
    }

    if (!exerciseRepo_.removeExercise(exerciseId)) {
        return VoidResult::error("Failed to delete exercise");
    }

    return VoidResult::success();
}

} // namespace service
} // namespace english_learning
