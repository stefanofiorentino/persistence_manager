#ifndef SM1CMA_ISODATE_HPP
#define SM1CMA_ISODATE_HPP

#include "ext/date.h"

template<class Precision = std::chrono::seconds>
inline std::string getISOCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    return date::format("%FT%TZ", date::floor<Precision>(now));
}

template<class Precision = std::chrono::seconds>
inline std::string getISOTimestampToKeep() {
    auto now = std::chrono::system_clock::now() - std::chrono::seconds(3*30*24*60*60);
    return date::format("%FT%TZ", date::floor<Precision>(now));
}

#endif //SM1CMA_ISODATE_HPP
