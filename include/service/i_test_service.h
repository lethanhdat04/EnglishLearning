#ifndef ENGLISH_LEARNING_SERVICE_I_TEST_SERVICE_H
#define ENGLISH_LEARNING_SERVICE_I_TEST_SERVICE_H

#include <string>
#include <vector>
#include "service_result.h"
#include "include/core/test.h"

namespace english_learning {
namespace service {

/**
 * DTO for test list response.
 */
struct TestListResult {
    std::vector<core::Test> tests;
    size_t total;
};

/**
 * DTO for test submission result.
 */
struct TestSubmissionResult {
    std::string testId;
    int score;
    int totalPoints;
    double percentage;
    std::string grade;
    std::vector<bool> questionResults;
};

/**
 * Interface for test management services.
 */
class ITestService {
public:
    virtual ~ITestService() = default;

    /**
     * Get all tests, optionally filtered by level.
     * @param level Filter by level (empty for all)
     * @return List of matching tests
     */
    virtual ServiceResult<TestListResult> getTests(const std::string& level = "") = 0;

    /**
     * Get a specific test by ID.
     * @param testId The test ID
     * @return Test if found, error otherwise
     */
    virtual ServiceResult<core::Test> getTestById(const std::string& testId) = 0;

    /**
     * Create a new test (teacher/admin only).
     * @param userId The user creating the test
     * @param title Test title
     * @param testType Test type
     * @param level Difficulty level
     * @param topic Test topic
     * @param questions Test questions
     * @return Created test on success
     */
    virtual ServiceResult<core::Test> createTest(
        const std::string& userId,
        const std::string& title,
        const std::string& testType,
        const std::string& level,
        const std::string& topic,
        const std::vector<core::TestQuestion>& questions) = 0;

    /**
     * Submit answers for a test.
     * @param userId The user submitting
     * @param testId The test being submitted
     * @param answers User's answers (in order)
     * @return Submission result with score
     */
    virtual ServiceResult<TestSubmissionResult> submitTest(
        const std::string& userId,
        const std::string& testId,
        const std::vector<std::string>& answers) = 0;

    /**
     * Update an existing test (teacher/admin only).
     * @param userId The user updating the test
     * @param testId The test to update
     * @param title New title
     * @param testType New type
     * @param level New level
     * @param topic New topic
     * @param questions New questions
     * @return Updated test on success
     */
    virtual ServiceResult<core::Test> updateTest(
        const std::string& userId,
        const std::string& testId,
        const std::string& title,
        const std::string& testType,
        const std::string& level,
        const std::string& topic,
        const std::vector<core::TestQuestion>& questions) = 0;

    /**
     * Delete a test (admin only).
     * @param userId The user deleting the test
     * @param testId The test to delete
     * @return Success or error
     */
    virtual VoidResult deleteTest(
        const std::string& userId,
        const std::string& testId) = 0;

    /**
     * Get total test count.
     * @return Total number of tests
     */
    virtual ServiceResult<size_t> getTestCount() = 0;
};

} // namespace service
} // namespace english_learning

#endif // ENGLISH_LEARNING_SERVICE_I_TEST_SERVICE_H
