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

ServiceResult<core::ExerciseSubmission> ExerciseService::saveDraft(
    const std::string& userId,
    const std::string& exerciseId,
    const std::string& content,
    const std::string& audioUrl) {

    auto exerciseOpt = exerciseRepo_.findExerciseById(exerciseId);
    if (!exerciseOpt.has_value()) {
        return ServiceResult<core::ExerciseSubmission>::error("Exercise not found");
    }

    const core::Exercise& exercise = exerciseOpt.value();
    int64_t now = protocol::utils::getCurrentTimestamp();

    // Check if draft already exists
    auto existingDraft = exerciseRepo_.findDraftByUserAndExercise(userId, exerciseId);
    if (existingDraft.has_value()) {
        // Update existing draft
        core::ExerciseSubmission draft = existingDraft.value();
        draft.content = content;
        draft.audioUrl = audioUrl;
        if (!exerciseRepo_.updateSubmission(draft)) {
            return ServiceResult<core::ExerciseSubmission>::error("Failed to update draft");
        }
        return ServiceResult<core::ExerciseSubmission>::success(draft);
    }

    // Create new draft
    core::ExerciseSubmission submission;
    submission.submissionId = protocol::utils::generateId("sub");
    submission.exerciseId = exerciseId;
    submission.userId = userId;
    submission.exerciseType = exercise.exerciseType;
    submission.content = content;
    submission.audioUrl = audioUrl;
    submission.status = core::SubmissionStatusStr::DRAFT;
    submission.createdAt = now;
    submission.submittedAt = 0;
    submission.teacherScore = 0;
    submission.reviewedAt = 0;
    submission.attemptNumber = exerciseRepo_.getAttemptCount(userId, exerciseId) + 1;

    if (!exerciseRepo_.addSubmission(submission)) {
        return ServiceResult<core::ExerciseSubmission>::error("Failed to save draft");
    }

    return ServiceResult<core::ExerciseSubmission>::success(submission);
}

ServiceResult<core::ExerciseSubmission> ExerciseService::submitExercise(
    const std::string& userId,
    const std::string& exerciseId,
    const std::string& content,
    const std::string& audioUrl) {

    auto exerciseOpt = exerciseRepo_.findExerciseById(exerciseId);
    if (!exerciseOpt.has_value()) {
        return ServiceResult<core::ExerciseSubmission>::error("Exercise not found");
    }

    const core::Exercise& exercise = exerciseOpt.value();
    int64_t now = protocol::utils::getCurrentTimestamp();

    // Check if draft exists and submit it
    auto existingDraft = exerciseRepo_.findDraftByUserAndExercise(userId, exerciseId);
    if (existingDraft.has_value()) {
        core::ExerciseSubmission submission = existingDraft.value();
        submission.content = content;
        submission.audioUrl = audioUrl;
        submission.submit(now);
        if (!exerciseRepo_.updateSubmission(submission)) {
            return ServiceResult<core::ExerciseSubmission>::error("Failed to submit exercise");
        }
        return ServiceResult<core::ExerciseSubmission>::success(submission);
    }

    // Create new submission directly
    core::ExerciseSubmission submission;
    submission.submissionId = protocol::utils::generateId("sub");
    submission.exerciseId = exerciseId;
    submission.userId = userId;
    submission.exerciseType = exercise.exerciseType;
    submission.content = content;
    submission.audioUrl = audioUrl;
    submission.status = core::SubmissionStatusStr::PENDING_REVIEW;
    submission.createdAt = now;
    submission.submittedAt = now;
    submission.teacherScore = 0;
    submission.reviewedAt = 0;
    submission.attemptNumber = exerciseRepo_.getAttemptCount(userId, exerciseId) + 1;

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

    // Count by status
    for (const auto& sub : submissions) {
        if (sub.isDraft()) {
            result.draftCount++;
        } else if (sub.isPendingReview()) {
            result.pendingCount++;
        } else if (sub.isReviewed()) {
            result.reviewedCount++;
        }
    }

    return ServiceResult<SubmissionListResult>::success(result);
}

ServiceResult<SubmissionListResult> ExerciseService::getUserDrafts(
    const std::string& userId) {

    auto drafts = exerciseRepo_.findDraftsByUser(userId);

    SubmissionListResult result;
    result.submissions = drafts;
    result.total = drafts.size();
    result.draftCount = drafts.size();

    return ServiceResult<SubmissionListResult>::success(result);
}

ServiceResult<core::ExerciseSubmission> ExerciseService::getFeedback(
    const std::string& userId,
    const std::string& submissionId) {

    auto submissionOpt = exerciseRepo_.findSubmissionById(submissionId);
    if (!submissionOpt.has_value()) {
        return ServiceResult<core::ExerciseSubmission>::error("Submission not found");
    }

    core::ExerciseSubmission submission = submissionOpt.value();

    // Verify ownership
    if (submission.userId != userId) {
        return ServiceResult<core::ExerciseSubmission>::error("Unauthorized: Not your submission");
    }

    if (!submission.isReviewed()) {
        return ServiceResult<core::ExerciseSubmission>::error("Feedback not yet available");
    }

    return ServiceResult<core::ExerciseSubmission>::success(submission);
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

ServiceResult<SubmissionDetailResult> ExerciseService::getSubmissionDetail(
    const std::string& teacherId,
    const std::string& submissionId) {

    if (!userRepo_.isTeacher(teacherId)) {
        return ServiceResult<SubmissionDetailResult>::error("Only teachers can view submission details");
    }

    auto submissionOpt = exerciseRepo_.findSubmissionById(submissionId);
    if (!submissionOpt.has_value()) {
        return ServiceResult<SubmissionDetailResult>::error("Submission not found");
    }

    core::ExerciseSubmission submission = submissionOpt.value();

    auto exerciseOpt = exerciseRepo_.findExerciseById(submission.exerciseId);
    if (!exerciseOpt.has_value()) {
        return ServiceResult<SubmissionDetailResult>::error("Exercise not found");
    }

    auto userOpt = userRepo_.findById(submission.userId);

    SubmissionDetailResult result;
    result.submission = submission;
    result.exercise = exerciseOpt.value();
    result.attemptNumber = submission.attemptNumber;

    if (userOpt.has_value()) {
        result.studentName = userOpt.value().fullname;
        result.studentEmail = userOpt.value().email;
    }

    return ServiceResult<SubmissionDetailResult>::success(result);
}

ServiceResult<core::ExerciseSubmission> ExerciseService::reviewSubmissionWithScores(
    const std::string& teacherId,
    const std::string& submissionId,
    const std::string& feedback,
    const core::ScoreBreakdown& scores) {

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
    if (!exerciseRepo_.reviewSubmissionWithScores(submissionId, teacherId, feedback, scores, reviewedAt)) {
        return ServiceResult<core::ExerciseSubmission>::error("Failed to review submission");
    }

    submission.setReview(teacherId, feedback, scores, reviewedAt);

    return ServiceResult<core::ExerciseSubmission>::success(submission);
}

ServiceResult<ReviewStatistics> ExerciseService::getReviewStatistics(
    const std::string& teacherId) {

    if (!userRepo_.isTeacher(teacherId)) {
        return ServiceResult<ReviewStatistics>::error("Only teachers can view review statistics");
    }

    auto allSubmissions = exerciseRepo_.findAllSubmissions();

    ReviewStatistics stats;
    stats.totalPending = 0;
    stats.totalReviewed = 0;
    stats.reviewedToday = 0;
    stats.reviewedThisWeek = 0;
    stats.averageScore = 0.0;

    int64_t now = protocol::utils::getCurrentTimestamp();
    int64_t dayStart = now - (now % 86400000);  // Start of today (ms)
    int64_t weekStart = now - (7 * 86400000);   // 7 days ago

    int totalScore = 0;
    int reviewedCount = 0;

    for (const auto& sub : allSubmissions) {
        if (sub.isPendingReview()) {
            stats.totalPending++;
        } else if (sub.isReviewed()) {
            stats.totalReviewed++;
            if (sub.teacherId == teacherId) {
                if (sub.reviewedAt >= dayStart) {
                    stats.reviewedToday++;
                }
                if (sub.reviewedAt >= weekStart) {
                    stats.reviewedThisWeek++;
                }
                totalScore += sub.teacherScore;
                reviewedCount++;
            }
        }
    }

    if (reviewedCount > 0) {
        stats.averageScore = static_cast<double>(totalScore) / reviewedCount;
    }

    return ServiceResult<ReviewStatistics>::success(stats);
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
