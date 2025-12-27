#ifndef ENGLISH_LEARNING_CORE_LESSON_H
#define ENGLISH_LEARNING_CORE_LESSON_H

#include <string>
#include "types.h"

namespace english_learning {
namespace core {

/**
 * Lesson entity representing a learning lesson.
 * Lessons contain educational content on various topics and levels.
 */
struct Lesson {
    std::string lessonId;
    std::string title;
    std::string description;
    std::string topic;          // grammar, vocabulary, listening, speaking, reading, writing
    std::string level;          // beginner, intermediate, advanced
    int duration;               // Duration in minutes
    std::string textContent;
    std::string videoUrl;
    std::string audioUrl;

    Lesson() : duration(0) {}

    Lesson(const std::string& id, const std::string& t, const std::string& desc,
           const std::string& top, const std::string& lvl, int dur)
        : lessonId(id), title(t), description(desc),
          topic(top), level(lvl), duration(dur) {}

    // Check if lesson has video content
    bool hasVideo() const {
        return !videoUrl.empty();
    }

    // Check if lesson has audio content
    bool hasAudio() const {
        return !audioUrl.empty();
    }

    // Check if lesson matches given level
    bool matchesLevel(const std::string& lvl) const {
        return level == lvl;
    }

    // Check if lesson matches given topic
    bool matchesTopic(const std::string& top) const {
        return topic.empty() || top.empty() || topic == top;
    }
};

} // namespace core
} // namespace english_learning

#endif // ENGLISH_LEARNING_CORE_LESSON_H
