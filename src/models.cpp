#include "models.hpp"

// ================ Account ================
Account::Account(const std::string& username, const std::string& raw_password) : _username(username) {
    char hash[crypto_pwhash_STRBYTES];

    if (crypto_pwhash_str(
        hash,
        raw_password.c_str(),
        raw_password.size(),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,
        crypto_pwhash_MEMLIMIT_INTERACTIVE
    ) != 0) {
        throw std::runtime_error("Hashing failed");
    }

    _password_hash = hash;
}

bool Account::verifyPassword(const std::string& raw_password) const {
    return crypto_pwhash_str_verify(
        _password_hash.c_str(),
        raw_password.c_str(),
        raw_password.size()
    ) == 0;
}

const std::string& Account::getUsername() const {
    return _username;
}

// ================ Date ================

DateTime::DateTime() : _tp{std::chrono::system_clock::now()} {}

DateTime::DateTime(int day, int month, int year, int hour, int minute, int second) {
    std::chrono::year_month_day ymd {
        std::chrono::year {year},
        std::chrono::month {static_cast<unsigned int>(month)},
        std::chrono::day {static_cast<unsigned int>(day)}
    };
    
    if (!ymd.ok())
        throw std::invalid_argument("Invalid date");
    
    if (hour < 0 || hour > 23 ||
        minute < 0 || minute > 59 ||
        second < 0 || second > 59)
        throw std::invalid_argument("Invalid time");

    using namespace std::chrono;
    sys_days days {ymd};
    _tp = days + hours { hour } + minutes { minute} +   seconds {second};
}

DateTime DateTime::now()
{
    return DateTime();
}

int DateTime::day() const
{
    auto dp = floor<std::chrono::days>(_tp);
    return unsigned(std::chrono::year_month_day{dp}.day());
}

int DateTime::month() const
{
    auto dp = floor<std::chrono::days>(_tp);
    return unsigned(std::chrono::year_month_day{dp}.month());
}

int DateTime::year() const
{
    auto dp = floor<std::chrono::days>(_tp);
    return int(std::chrono::year_month_day{dp}.year());
}

int DateTime::hour() const
{
    auto dp = floor<std::chrono::days>(_tp);
    std::chrono::hh_mm_ss hms{std::chrono::duration_cast<std::chrono::seconds>(_tp - dp)};
    return hms.hours().count();
}

int DateTime::minute() const
{
    auto dp = floor<std::chrono::days>(_tp);
    std::chrono::hh_mm_ss hms{std::chrono::duration_cast<std::chrono::seconds>(_tp - dp)};
    return hms.minutes().count();
}

int DateTime::second() const
{
    auto dp = floor<std::chrono::days>(_tp);
    std::chrono::hh_mm_ss hms{std::chrono::duration_cast<std::chrono::seconds>(_tp - dp)};
    return hms.seconds().count();
}

std::string DateTime::toString() const
{
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << day() << "/"
        << std::setw(2) << std::setfill('0') << month() << "/"
        << year() << " "
        << std::setw(2) << std::setfill('0') << hour() << ":"
        << std::setw(2) << std::setfill('0') << minute() << ":"
        << std::setw(2) << std::setfill('0') << second();
    return oss.str();
}

DateTime DateTime::addDays(int days) const
{
    DateTime result;
    result._tp = _tp + std::chrono::days { days };
    return result;
}

DateTime DateTime::addHours(int hours) const
{
    DateTime result;
    result._tp = _tp + std::chrono::hours { hours };
    return result;
}

long long DateTime::daysBetween(const DateTime &other) const
{
    auto d1 = floor<std::chrono::days>(_tp);
    auto d2 = floor<std::chrono::days>(other._tp);
    return (d2 - d1).count();
}

bool DateTime::operator==(const DateTime &other) const
{
    return _tp == other._tp;
}

bool DateTime::operator<(const DateTime &other) const
{
    return _tp < other._tp;
}

bool DateTime::operator>(const DateTime &other) const
{
    return _tp > other._tp;
}
