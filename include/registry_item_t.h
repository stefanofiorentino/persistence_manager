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


#endif //SM1CMA_REGISTRY_ITEM_T_H
