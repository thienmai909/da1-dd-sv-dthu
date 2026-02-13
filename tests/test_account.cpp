#include <gtest/gtest.h>
#include "models.hpp"

TEST(AccountTest, UsernameShouldMatch) {
    Account acc("thienmai", "123456");
    EXPECT_EQ(acc.getUsername(), "thienmai");
}

TEST(AccountTest, CorrectPassword) {
    Account acc("thienmai", "123456");
    EXPECT_TRUE(acc.verifyPassword("123456"));
}

TEST(AccountTest, WrongPassword) {
    Account acc("thienmai", "123456");
    EXPECT_FALSE(acc.verifyPassword("wrong"));
}