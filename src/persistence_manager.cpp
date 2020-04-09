#include "include/persistence_manager.h"

#include <sqlite3.h>

persistence_manager_status sqlite_rc_to_persistence_manager_status(int rc) {
    switch(rc) {
        case SQLITE_DONE:
            return persistence_manager_status::DONE;
        case SQLITE_OK:
            return persistence_manager_status::OK;
        case SQLITE_ROW:
            return persistence_manager_status::ROW;
        default:
            return persistence_manager_status::KO;
    }
}

struct persistence_manager::Impl {
    sqlite3 *persistentDB;
    sqlite3_stmt *stmt;
    bool transaction_is_active{false};
};

persistence_manager::persistence_manager() :
        pImpl(std::make_unique<Impl>()) {
}

persistence_manager::~persistence_manager() noexcept {
    if(pImpl && pImpl->transaction_is_active) {
        auto rc = rollback();
        if(persistence_manager_status::KO == rc) {
            return; // just swallow error status
        }
    }
}

persistence_manager::persistence_manager(persistence_manager &&rhs) noexcept = default;

persistence_manager &persistence_manager::operator=(persistence_manager &&rhs) noexcept = default;

persistence_manager_status persistence_manager::open(std::string const &db_name) noexcept {
    return sqlite_rc_to_persistence_manager_status(sqlite3_open(db_name.c_str(), &pImpl->persistentDB));
}

persistence_manager_status persistence_manager::close() noexcept {
    return sqlite_rc_to_persistence_manager_status(sqlite3_close_v2(pImpl->persistentDB));
}

persistence_manager_status persistence_manager::begin_transaction() noexcept {
    if(pImpl && pImpl->transaction_is_active) {
        return persistence_manager_status::KO;
    }
    int rc = sqlite3_prepare_v2(pImpl->persistentDB, "BEGIN TRANSACTION", -1, &pImpl->stmt, nullptr);
    if(sqlite_rc_to_persistence_manager_status(rc) != persistence_manager_status::OK) {
        return sqlite_rc_to_persistence_manager_status(rc);
    }
    rc = sqlite3_step(pImpl->stmt);
    if(sqlite_rc_to_persistence_manager_status(rc) != persistence_manager_status::DONE) {
        return sqlite_rc_to_persistence_manager_status(rc);
    }
    pImpl->transaction_is_active = true;
    return sqlite_rc_to_persistence_manager_status(sqlite3_finalize(pImpl->stmt));
}

persistence_manager_status persistence_manager::commit() noexcept {
    if(!pImpl || !pImpl->transaction_is_active) {
        return persistence_manager_status::KO;
    }
    int rc = sqlite3_prepare_v2(pImpl->persistentDB, "COMMIT", -1, &pImpl->stmt, nullptr);
    if(sqlite_rc_to_persistence_manager_status(rc) != persistence_manager_status::OK) {
        return sqlite_rc_to_persistence_manager_status(rc);
    }
    rc = sqlite3_step(pImpl->stmt);
    if(sqlite_rc_to_persistence_manager_status(rc) != persistence_manager_status::DONE) {
        return sqlite_rc_to_persistence_manager_status(rc);
    }
    if(pImpl->transaction_is_active) {
        pImpl->transaction_is_active = false;
    }
    return sqlite_rc_to_persistence_manager_status(sqlite3_finalize(pImpl->stmt));
}

persistence_manager_status persistence_manager::rollback() noexcept {
    int rc = sqlite3_prepare_v2(pImpl->persistentDB, "ROLLBACK", -1, &pImpl->stmt, nullptr);
    if(sqlite_rc_to_persistence_manager_status(rc) != persistence_manager_status::OK) {
        return sqlite_rc_to_persistence_manager_status(rc);
    }
    rc = sqlite3_step(pImpl->stmt);
    if(sqlite_rc_to_persistence_manager_status(rc) != persistence_manager_status::DONE) {
        return sqlite_rc_to_persistence_manager_status(rc);
    }
    if(pImpl->transaction_is_active) {
        pImpl->transaction_is_active = false;
    }
    return sqlite_rc_to_persistence_manager_status(sqlite3_finalize(pImpl->stmt));
}

persistence_manager_status persistence_manager::create_data_table() noexcept {
    // creazione tabella principale
    char const *sql_string = R"(
CREATE TABLE IF NOT EXISTS `data_table` (
	`id`	INTEGER PRIMARY KEY AUTOINCREMENT,
	`timestamp`	TEXT NOT NULL,
	`sensor_type`	TEXT NOT NULL
))";
    int rc = sqlite3_prepare_v2(pImpl->persistentDB, sql_string, -1, &pImpl->stmt, nullptr);
    if(sqlite_rc_to_persistence_manager_status(rc) != persistence_manager_status::OK) {
        return sqlite_rc_to_persistence_manager_status(rc);
    }
    rc = sqlite3_step(pImpl->stmt);
    if(sqlite_rc_to_persistence_manager_status(rc) != persistence_manager_status::DONE) {
        return sqlite_rc_to_persistence_manager_status(rc);
    }
    return sqlite_rc_to_persistence_manager_status(sqlite3_finalize(pImpl->stmt));
}

persistence_manager_status persistence_manager::insert_into_data_table(registry_item_t const &ri) noexcept {
    // inserimento dati nel db aperto
    char const *sql_string = R"(
INSERT INTO `data_table`(
    `timestamp`,
    `sensor_type`) VALUES (?, ?))";
    int rc = sqlite3_prepare_v2(pImpl->persistentDB, sql_string, -1, &pImpl->stmt, nullptr);
    if(sqlite_rc_to_persistence_manager_status(rc) != persistence_manager_status::OK) {
        return sqlite_rc_to_persistence_manager_status(rc);
    }
    rc = sqlite3_bind_text(pImpl->stmt, 1, ri.timestamp.c_str(), -1, SQLITE_STATIC);
    if(sqlite_rc_to_persistence_manager_status(rc) != persistence_manager_status::OK) {
        return sqlite_rc_to_persistence_manager_status(rc);
    }
    rc = sqlite3_bind_text(pImpl->stmt, 2, ri.sensor_type.c_str(), -1, SQLITE_STATIC);
    if(sqlite_rc_to_persistence_manager_status(rc) != persistence_manager_status::OK) {
        return sqlite_rc_to_persistence_manager_status(rc);
    }
    rc = sqlite3_step(pImpl->stmt);
    if(sqlite_rc_to_persistence_manager_status(rc) != persistence_manager_status::DONE) {
        return sqlite_rc_to_persistence_manager_status(rc);
    }
    return sqlite_rc_to_persistence_manager_status(sqlite3_finalize(pImpl->stmt));
}

persistence_manager_status persistence_manager::performSQLiteGetQuery(const std::string &sql_string) noexcept {
    int rc = sqlite3_prepare_v2(pImpl->persistentDB, sql_string.c_str(), -1, &pImpl->stmt, nullptr);
    if(sqlite_rc_to_persistence_manager_status(rc) != persistence_manager_status::OK) {
        return sqlite_rc_to_persistence_manager_status(rc);
    }
    return sqlite_rc_to_persistence_manager_status(sqlite3_step(pImpl->stmt));
}

std::map<int, std::vector<std::string>>
persistence_manager::performSQLiteSelectQuery(const std::string &sql_string) noexcept {
    std::map<int, std::vector<std::string>> returnMapVectorString;
    auto rc = performSQLiteGetQuery(sql_string);
    if(rc != persistence_manager_status::ROW && rc != persistence_manager_status::DONE) {
        return returnMapVectorString;
    }
    int row = 0;
    while(persistence_manager_status::ROW == rc) {
        const auto cols = sqlite3_column_count(pImpl->stmt);
        std::vector<std::string> returnVectorString;
        returnVectorString.reserve(cols);
        for(int col = 0; col < cols; ++col) {
            returnVectorString.push_back(getSQLiteColumnText(col));
        }
        returnMapVectorString[row] = returnVectorString;
        rc = sqlite_rc_to_persistence_manager_status(sqlite3_step(pImpl->stmt));
        row++;
    }
    sqlite3_finalize(pImpl->stmt);
    return returnMapVectorString;
}

std::vector<registry_item_t> persistence_manager::select_all_from_data_table() noexcept {
    std::vector<registry_item_t> res;
    auto rows = performSQLiteSelectQuery("SELECT * FROM `data_table`");
    for(auto const &pair: rows) {
        auto const &values = pair.second;
        registry_item_t ri;
        ri.timestamp = values.at(1);
        ri.sensor_type = values.at(2);
        res.emplace_back(ri);
    }
    return res;
}

std::string persistence_manager::getSQLiteColumnText(int col) const noexcept {
    const auto string = sqlite3_column_text(pImpl->stmt, col);
    const auto string1 = reinterpret_cast<const char *>(string);
    return (string1 != nullptr) ? std::string(string1) : "";
}
