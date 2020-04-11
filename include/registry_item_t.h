#ifndef SM1CMA_REGISTRY_ITEM_T_H
#define SM1CMA_REGISTRY_ITEM_T_H

#include <string>
#include <vector>
#include <algorithm>

struct registry_item_t final {
    std::string timestamp;
    std::string sensor_type;

    bool operator==(registry_item_t const &rhs) const {
        bool res{true};
        res &= timestamp == rhs.timestamp;
        return res;
    }

    explicit operator bool() {
        return !timestamp.empty() && !sensor_type.empty();
    }
};

inline std::string getNumericTimestamp(const std::string &timestamp) {
    return timestamp.substr(0, 4) + timestamp.substr(5, 2) + timestamp.substr(8, 2) + timestamp.substr(11, 2) +
           timestamp.substr(14, 2) + timestamp.substr(17, 2) + "000";
}

inline std::tuple<std::string, std::string> timestamp_to_isodate_and_date(registry_item_t const &item) {
    auto isodate = item.timestamp;
    auto date = getNumericTimestamp(isodate);
    return {date, isodate};
}

#endif //SM1CMA_REGISTRY_ITEM_T_H
