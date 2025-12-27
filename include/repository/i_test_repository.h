#ifndef ENGLISH_LEARNING_REPOSITORY_I_TEST_REPOSITORY_H
#define ENGLISH_LEARNING_REPOSITORY_I_TEST_REPOSITORY_H

#include <string>
#include <vector>
#include <optional>
#include "include/core/test.h"

namespace english_learning {
namespace repository {

/**
 * Interface for test data access operations.
 * Provides CRUD operations for Test entities.
 */
class ITestRepository {
public:
    virtual ~ITestRepository() = default;

    // Create
    virtual bool add(const core::Test& test) = 0;

    // Read
    virtual std::optional<core::Test> findById(const std::string& testId) const = 0;
    virtual std::vector<core::Test> findAll() const = 0;
    virtual std::vector<core::Test> findByLevel(const std::string& level) const = 0;
    virtual std::vector<core::Test> findByType(const std::string& testType) const = 0;
    virtual std::vector<core::Test> findByLevelAndType(const std::string& level,
                                                        const std::string& testType) const = 0;
    virtual bool exists(const std::string& testId) const = 0;

    // Update
    virtual bool update(const core::Test& test) = 0;

    // Delete
    virtual bool remove(const std::string& testId) = 0;

    // Utility
    virtual size_t count() const = 0;
};

} // namespace repository
} // namespace english_learning

#endif // ENGLISH_LEARNING_REPOSITORY_I_TEST_REPOSITORY_H
