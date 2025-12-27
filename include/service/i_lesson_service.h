#ifndef ENGLISH_LEARNING_SERVICE_I_LESSON_SERVICE_H
#define ENGLISH_LEARNING_SERVICE_I_LESSON_SERVICE_H

#include <string>
#include <vector>
#include "service_result.h"
#include "include/core/lesson.h"

namespace english_learning {
namespace service {

/**
 * DTO for lesson list response.
 */
struct LessonListResult {
    std::vector<core::Lesson> lessons;
    size_t total;
};

/**
 * Interface for lesson management services.
 */
class ILessonService {
public:
    virtual ~ILessonService() = default;

    /**
     * Get all lessons, optionally filtered by level and topic.
     * @param level Filter by level (empty for all)
     * @param topic Filter by topic (empty for all)
     * @return List of matching lessons
     */
    virtual ServiceResult<LessonListResult> getLessons(
        const std::string& level = "",
        const std::string& topic = "") = 0;

    /**
     * Get a specific lesson by ID.
     * @param lessonId The lesson ID
     * @return Lesson if found, error otherwise
     */
    virtual ServiceResult<core::Lesson> getLessonById(const std::string& lessonId) = 0;

    /**
     * Create a new lesson (teacher/admin only).
     * @param userId The user creating the lesson
     * @param title Lesson title
     * @param description Lesson description
     * @param textContent Lesson text content
     * @param level Difficulty level
     * @param topic Lesson topic
     * @param duration Duration in minutes
     * @param videoUrl Optional video URL
     * @param audioUrl Optional audio URL
     * @return Created lesson on success
     */
    virtual ServiceResult<core::Lesson> createLesson(
        const std::string& userId,
        const std::string& title,
        const std::string& description,
        const std::string& textContent,
        const std::string& level,
        const std::string& topic,
        int duration,
        const std::string& videoUrl = "",
        const std::string& audioUrl = "") = 0;

    /**
     * Update an existing lesson (teacher/admin only).
     * @param userId The user updating the lesson
     * @param lessonId The lesson to update
     * @param title New title
     * @param description New description
     * @param textContent New content
     * @param level New level
     * @param topic New topic
     * @param duration New duration
     * @param videoUrl New video URL
     * @param audioUrl New audio URL
     * @return Updated lesson on success
     */
    virtual ServiceResult<core::Lesson> updateLesson(
        const std::string& userId,
        const std::string& lessonId,
        const std::string& title,
        const std::string& description,
        const std::string& textContent,
        const std::string& level,
        const std::string& topic,
        int duration,
        const std::string& videoUrl = "",
        const std::string& audioUrl = "") = 0;

    /**
     * Delete a lesson (admin only).
     * @param userId The user deleting the lesson
     * @param lessonId The lesson to delete
     * @return Success or error
     */
    virtual VoidResult deleteLesson(
        const std::string& userId,
        const std::string& lessonId) = 0;

    /**
     * Get lessons count by level.
     * @param level The level to count
     * @return Count of lessons at that level
     */
    virtual ServiceResult<size_t> getLessonCountByLevel(
        const std::string& level) = 0;
};

} // namespace service
} // namespace english_learning

#endif // ENGLISH_LEARNING_SERVICE_I_LESSON_SERVICE_H
