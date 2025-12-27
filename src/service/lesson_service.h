#ifndef ENGLISH_LEARNING_SERVICE_LESSON_SERVICE_H
#define ENGLISH_LEARNING_SERVICE_LESSON_SERVICE_H

#include "include/service/i_lesson_service.h"
#include "include/repository/i_lesson_repository.h"
#include "include/repository/i_user_repository.h"

namespace english_learning {
namespace service {

/**
 * Implementation of lesson management service.
 */
class LessonService : public ILessonService {
public:
    LessonService(
        repository::ILessonRepository& lessonRepo,
        repository::IUserRepository& userRepo);

    ~LessonService() override = default;

    ServiceResult<LessonListResult> getLessons(
        const std::string& level = "",
        const std::string& topic = "") override;

    ServiceResult<core::Lesson> getLessonById(const std::string& lessonId) override;

    ServiceResult<core::Lesson> createLesson(
        const std::string& userId,
        const std::string& title,
        const std::string& description,
        const std::string& textContent,
        const std::string& level,
        const std::string& topic,
        int duration,
        const std::string& videoUrl = "",
        const std::string& audioUrl = "") override;

    ServiceResult<core::Lesson> updateLesson(
        const std::string& userId,
        const std::string& lessonId,
        const std::string& title,
        const std::string& description,
        const std::string& textContent,
        const std::string& level,
        const std::string& topic,
        int duration,
        const std::string& videoUrl = "",
        const std::string& audioUrl = "") override;

    VoidResult deleteLesson(
        const std::string& userId,
        const std::string& lessonId) override;

    ServiceResult<size_t> getLessonCountByLevel(
        const std::string& level) override;

private:
    repository::ILessonRepository& lessonRepo_;
    repository::IUserRepository& userRepo_;
};

} // namespace service
} // namespace english_learning

#endif // ENGLISH_LEARNING_SERVICE_LESSON_SERVICE_H
