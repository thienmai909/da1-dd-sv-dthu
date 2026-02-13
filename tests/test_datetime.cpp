#include <gtest/gtest.h>
#include "models.hpp"

TEST(DateTimeTest, ValidConstruction) {
    DateTime dt(1, 5, 2025, 12, 30, 45);

    EXPECT_EQ(dt.day(), 1);
    EXPECT_EQ(dt.month(), 5);
    EXPECT_EQ(dt.year(), 2025);
    EXPECT_EQ(dt.hour(), 12);
    EXPECT_EQ(dt.minute(), 30);
    EXPECT_EQ(dt.second(), 45);
}

TEST(DateTimeTest, InvalidDateThrows) {
    EXPECT_THROW(DateTime(31, 2, 2025), std::invalid_argument);
}

TEST(DateTimeTest, InvalidTimeThrows) {
    EXPECT_THROW(DateTime(1, 1, 2025, 25, 0, 0), std::invalid_argument);
}

TEST(DateTimeTest, NowDoesNotThrow) {
    EXPECT_NO_THROW(DateTime::now());
}

TEST(DateTimeTest, AddDays) {
    DateTime dt(1, 1, 2025);
    DateTime future = dt.addDays(30);

    EXPECT_EQ(future.day(), 31);
    EXPECT_EQ(future.month(), 1);
}

TEST(DateTimeTest, AddDaysAcrossMonth) {
    DateTime dt(31, 1, 2025);
    DateTime future = dt.addDays(1);

    EXPECT_EQ(future.day(), 1);
    EXPECT_EQ(future.month(), 2);
}

TEST(DateTimeTest, AddDaysAcrossYear) {
    DateTime dt(31, 12, 2025);
    DateTime future = dt.addDays(1);

    EXPECT_EQ(future.day(), 1);
    EXPECT_EQ(future.month(), 1);
    EXPECT_EQ(future.year(), 2026);
}

TEST(DateTimeTest, DaysBetween) {
    DateTime d1(1, 1, 2025);
    DateTime d2(11, 1, 2025);

    EXPECT_EQ(d1.daysBetween(d2), 10);
}

TEST(DateTimeTest, ComparisonOperators) {
    DateTime d1(1, 1, 2025);
    DateTime d2(2, 1, 2025);

    EXPECT_TRUE(d1 < d2);
    EXPECT_FALSE(d1 > d2);
    EXPECT_FALSE(d2 < d1);
    EXPECT_TRUE(d2 > d1);
    EXPECT_FALSE(d1 == d2);
}
