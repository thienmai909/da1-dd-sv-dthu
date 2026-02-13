#pragma once
#include "sodium.h"

#include <string>
#include <stdexcept>
#include <iomanip>
#include <chrono>
#include <sstream>

class Account {
    std::string _username;
    std::string _password_hash;

public:
    Account(const std::string& username, const std::string& raw_password);

    bool verifyPassword(const std::string& raw_password) const;
    const std::string& getUsername() const;
};

class DateTime {
    std::chrono::system_clock::time_point _tp;

public:
    DateTime();
    DateTime(int day, int month, int year, int hour = 0, int minute = 0, int second = 0);

    static DateTime now();

    int day() const;
    int month() const;
    int year() const;
    int hour() const;
    int minute() const;
    int second() const;

    std::string toString() const;

    DateTime addDays(int days) const;
    DateTime addHours(int hours) const;

    long long daysBetween(const DateTime& other) const;

    bool operator==(const DateTime& other) const;
    bool operator<(const DateTime& other) const;
    bool operator>(const DateTime& other) const;

};