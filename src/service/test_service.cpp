#include "test_service.h"
#include "include/protocol/utils.h"

namespace english_learning {
namespace service {

TestService::TestService(
    repository::ITestRepository& testRepo,
    repository::IUserRepository& userRepo)
    : testRepo_(testRepo)
    , userRepo_(userRepo) {}

ServiceResult<TestListResult> TestService::getTests(const std::string& level) {
    std::vector<core::Test> tests;

    if (level.empty()) {
        tests = testRepo_.findAll();
    } else {
        tests = testRepo_.findByLevel(level);
    }

    TestListResult result;
    result.tests = tests;
    result.total = tests.size();

    return ServiceResult<TestListResult>::success(result);
}

ServiceResult<core::Test> TestService::getTestById(const std::string& testId) {
    auto testOpt = testRepo_.findById(testId);
    if (!testOpt.has_value()) {
        return ServiceResult<core::Test>::error("Test not found");
    }
    return ServiceResult<core::Test>::success(testOpt.value());
}

ServiceResult<core::Test> TestService::createTest(
    const std::string& userId,
    const std::string& title,
    const std::string& testType,
    const std::string& level,
    const std::string& topic,
    const std::vector<core::TestQuestion>& questions) {

    if (!userRepo_.isTeacher(userId)) {
        return ServiceResult<core::Test>::error("Only teachers can create tests");
    }

    if (title.empty()) {
        return ServiceResult<core::Test>::error("Title is required");
    }

    if (questions.empty()) {
        return ServiceResult<core::Test>::error("At least one question is required");
    }

    core::Test test;
    test.testId = protocol::utils::generateId("test");
    test.title = title;
    test.testType = testType;
    test.level = level;
    test.topic = topic;
    test.questions = questions;

    if (!testRepo_.add(test)) {
        return ServiceResult<core::Test>::error("Failed to create test");
    }

    return ServiceResult<core::Test>::success(test);
}

ServiceResult<TestSubmissionResult> TestService::submitTest(
    const std::string& /* userId */,
    const std::string& testId,
    const std::vector<std::string>& answers) {

    auto testOpt = testRepo_.findById(testId);
    if (!testOpt.has_value()) {
        return ServiceResult<TestSubmissionResult>::error("Test not found");
    }

    const core::Test& test = testOpt.value();

    if (answers.size() != test.questions.size()) {
        return ServiceResult<TestSubmissionResult>::error("Number of answers doesn't match questions");
    }

    int score = 0;
    std::vector<bool> questionResults;

    for (size_t i = 0; i < test.questions.size(); i++) {
        bool correct = test.questions[i].checkAnswer(answers[i]);
        questionResults.push_back(correct);
        if (correct) {
            score += test.questions[i].points;
        }
    }

    int totalPoints = test.getTotalPoints();
    double percentage = totalPoints == 0 ? 0.0 :
        (static_cast<double>(score) / totalPoints) * 100.0;

    std::string grade;
    if (percentage >= 90) grade = "A";
    else if (percentage >= 80) grade = "B";
    else if (percentage >= 70) grade = "C";
    else if (percentage >= 60) grade = "D";
    else grade = "F";

    TestSubmissionResult result;
    result.testId = testId;
    result.score = score;
    result.totalPoints = totalPoints;
    result.percentage = percentage;
    result.grade = grade;
    result.questionResults = questionResults;

    return ServiceResult<TestSubmissionResult>::success(result);
}

ServiceResult<core::Test> TestService::updateTest(
    const std::string& userId,
    const std::string& testId,
    const std::string& title,
    const std::string& testType,
    const std::string& level,
    const std::string& topic,
    const std::vector<core::TestQuestion>& questions) {

    if (!userRepo_.isTeacher(userId)) {
        return ServiceResult<core::Test>::error("Only teachers can update tests");
    }

    auto testOpt = testRepo_.findById(testId);
    if (!testOpt.has_value()) {
        return ServiceResult<core::Test>::error("Test not found");
    }

    core::Test test = testOpt.value();
    test.title = title;
    test.testType = testType;
    test.level = level;
    test.topic = topic;
    test.questions = questions;

    if (!testRepo_.update(test)) {
        return ServiceResult<core::Test>::error("Failed to update test");
    }

    return ServiceResult<core::Test>::success(test);
}

VoidResult TestService::deleteTest(
    const std::string& userId,
    const std::string& testId) {

    if (!userRepo_.isAdmin(userId)) {
        return VoidResult::error("Only admins can delete tests");
    }

    if (!testRepo_.remove(testId)) {
        return VoidResult::error("Failed to delete test");
    }

    return VoidResult::success();
}

ServiceResult<size_t> TestService::getTestCount() {
    size_t count = testRepo_.count();
    return ServiceResult<size_t>::success(count);
}

} // namespace service
} // namespace english_learning
