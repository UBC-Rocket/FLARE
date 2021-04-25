#include "gtest/gtest.h"

#include <cstring>
#include <limits>

#include "log.hpp"

TEST(LogOutput, Example) {
    std::stringstream out_stream;
    rktlog::OstreamOutput out(out_stream);
    rktlog::Logger::setOutput(out);

    // WARNING - LINE NUMBERS MATTER HERE
#line 100 "Filename"
    // undefined whether #line changes itself or the line following it
    int line_num = __LINE__;
    LOG_ERROR("Error Message " << 123);  // line_num + 1
    LOG_WARN("Warning Message " << 456); // line_num + 2
    LOG_INFO("Info Message " << -123);   // line_num + 3
    LOG_DEBUG("Debug Message " << 0);    // line_num + 4

    std::stringstream expect_stream;
    expect_stream << "ERRO  Filename[" << line_num + 1
                  << "]: Error Message 123\n";

    expect_stream << "WARN  Filename[" << line_num + 2
                  << "]: Warning Message 456\n";

    expect_stream << "INFO  Filename[" << line_num + 3
                  << "]: Info Message -123\n";

    expect_stream << "DEBG  Filename[" << line_num + 4
                  << "]: Debug Message 0\n";

    // std::cout << out_stream.str(); // visualize result
    EXPECT_EQ(out_stream.str(), expect_stream.str());
}
