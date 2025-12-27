#ifndef ENGLISH_LEARNING_REPOSITORY_I_LESSON_REPOSITORY_H
#define ENGLISH_LEARNING_REPOSITORY_I_LESSON_REPOSITORY_H

#include <string>
#include <vector>
#include <optional>
#include "include/core/lesson.h"

namespace english_learning {
namespace repository {

/**
 * Interface for lesson data access operations.
 * Provides CRUD operations for Lesson entities.
 */
class ILessonRepository {
public:
    virtual ~ILessonRepository() = default;

    // Create
    virtual bool add(const core::Lesson& lesson) = 0;

    // Read
    virtual std::optional<core::Lesson> findById(const std::string& lessonId) const = 0;
    virtual std::vector<core::Lesson> findAll() const = 0;
    virtual std::vector<core::Lesson> findByLevel(const std::string& level) const = 0;
    virtual std::vector<core::Lesson> findByTopic(const std::string& topic) const = 0;
    virtual std::vector<core::Lesson> findByLevelAndTopic(const std::string& level,
                                                          const std::string& topic) const = 0;
    virtual bool exists(const std::string& lessonId) const = 0;

    // Update
    virtual bool update(const core::Lesson& lesson) = 0;

    // Delete
    virtual bool remove(const std::string& lessonId) = 0;

    // Utility
    virtual size_t count() const = 0;
    virtual size_t countByLevel(const std::string& level) const = 0;
};

} // namespace repository
} // namespace english_learning

#endif // ENGLISH_LEARNING_REPOSITORY_I_LESSON_REPOSITORY_H
