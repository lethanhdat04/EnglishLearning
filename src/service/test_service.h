#ifndef ENGLISH_LEARNING_SERVICE_TEST_SERVICE_H
#define ENGLISH_LEARNING_SERVICE_TEST_SERVICE_H

#include "include/service/i_test_service.h"
#include "include/repository/i_test_repository.h"
#include "include/repository/i_user_repository.h"

namespace english_learning {
namespace service {

/**
 * Implementation of test management service.
 */
class TestService : public ITestService {
public:
    TestService(
        repository::ITestRepository& testRepo,
        repository::IUserRepository& userRepo);

    ~TestService() override = default;

    ServiceResult<TestListResult> getTests(const std::string& level = "") override;

    ServiceResult<core::Test> getTestById(const std::string& testId) override;

    ServiceResult<core::Test> createTest(
        const std::string& userId,
        const std::string& title,
        const std::string& testType,
        const std::string& level,
        const std::string& topic,
        const std::vector<core::TestQuestion>& questions) override;

    ServiceResult<TestSubmissionResult> submitTest(
        const std::string& userId,
        const std::string& testId,
        const std::vector<std::string>& answers) override;

    ServiceResult<core::Test> updateTest(
        const std::string& userId,
        const std::string& testId,
        const std::string& title,
        const std::string& testType,
        const std::string& level,
        const std::string& topic,
        const std::vector<core::TestQuestion>& questions) override;

    VoidResult deleteTest(
        const std::string& userId,
        const std::string& testId) override;

    ServiceResult<size_t> getTestCount() override;

private:
    repository::ITestRepository& testRepo_;
    repository::IUserRepository& userRepo_;
};

} // namespace service
} // namespace english_learning

#endif // ENGLISH_LEARNING_SERVICE_TEST_SERVICE_H
