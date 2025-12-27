#include "lesson_service.h"
#include "include/protocol/utils.h"

namespace english_learning {
namespace service {

LessonService::LessonService(
    repository::ILessonRepository& lessonRepo,
    repository::IUserRepository& userRepo)
    : lessonRepo_(lessonRepo)
    , userRepo_(userRepo) {}

ServiceResult<LessonListResult> LessonService::getLessons(
    const std::string& level,
    const std::string& topic) {

    std::vector<core::Lesson> lessons;

    if (level.empty() && topic.empty()) {
        lessons = lessonRepo_.findAll();
    } else if (!level.empty() && topic.empty()) {
        lessons = lessonRepo_.findByLevel(level);
    } else if (level.empty() && !topic.empty()) {
        lessons = lessonRepo_.findByTopic(topic);
    } else {
        lessons = lessonRepo_.findByLevelAndTopic(level, topic);
    }

    LessonListResult result;
    result.lessons = lessons;
    result.total = lessons.size();

    return ServiceResult<LessonListResult>::success(result);
}

ServiceResult<core::Lesson> LessonService::getLessonById(const std::string& lessonId) {
    auto lessonOpt = lessonRepo_.findById(lessonId);
    if (!lessonOpt.has_value()) {
        return ServiceResult<core::Lesson>::error("Lesson not found");
    }
    return ServiceResult<core::Lesson>::success(lessonOpt.value());
}

ServiceResult<core::Lesson> LessonService::createLesson(
    const std::string& userId,
    const std::string& title,
    const std::string& description,
    const std::string& textContent,
    const std::string& level,
    const std::string& topic,
    int duration,
    const std::string& videoUrl,
    const std::string& audioUrl) {

    // Check if user is teacher or admin
    if (!userRepo_.isTeacher(userId)) {
        return ServiceResult<core::Lesson>::error("Only teachers can create lessons");
    }

    // Validate input
    if (title.empty()) {
        return ServiceResult<core::Lesson>::error("Title is required");
    }

    core::Lesson lesson;
    lesson.lessonId = protocol::utils::generateId("lesson");
    lesson.title = title;
    lesson.description = description;
    lesson.textContent = textContent;
    lesson.level = level;
    lesson.topic = topic;
    lesson.duration = duration;
    lesson.videoUrl = videoUrl;
    lesson.audioUrl = audioUrl;

    if (!lessonRepo_.add(lesson)) {
        return ServiceResult<core::Lesson>::error("Failed to create lesson");
    }

    return ServiceResult<core::Lesson>::success(lesson);
}

ServiceResult<core::Lesson> LessonService::updateLesson(
    const std::string& userId,
    const std::string& lessonId,
    const std::string& title,
    const std::string& description,
    const std::string& textContent,
    const std::string& level,
    const std::string& topic,
    int duration,
    const std::string& videoUrl,
    const std::string& audioUrl) {

    // Check if user is teacher or admin
    if (!userRepo_.isTeacher(userId)) {
        return ServiceResult<core::Lesson>::error("Only teachers can update lessons");
    }

    auto lessonOpt = lessonRepo_.findById(lessonId);
    if (!lessonOpt.has_value()) {
        return ServiceResult<core::Lesson>::error("Lesson not found");
    }

    core::Lesson lesson = lessonOpt.value();
    lesson.title = title;
    lesson.description = description;
    lesson.textContent = textContent;
    lesson.level = level;
    lesson.topic = topic;
    lesson.duration = duration;
    lesson.videoUrl = videoUrl;
    lesson.audioUrl = audioUrl;

    if (!lessonRepo_.update(lesson)) {
        return ServiceResult<core::Lesson>::error("Failed to update lesson");
    }

    return ServiceResult<core::Lesson>::success(lesson);
}

VoidResult LessonService::deleteLesson(
    const std::string& userId,
    const std::string& lessonId) {

    // Check if user is admin
    if (!userRepo_.isAdmin(userId)) {
        return VoidResult::error("Only admins can delete lessons");
    }

    if (!lessonRepo_.remove(lessonId)) {
        return VoidResult::error("Failed to delete lesson");
    }

    return VoidResult::success();
}

ServiceResult<size_t> LessonService::getLessonCountByLevel(
    const std::string& level) {

    size_t count = lessonRepo_.countByLevel(level);
    return ServiceResult<size_t>::success(count);
}

} // namespace service
} // namespace english_learning
