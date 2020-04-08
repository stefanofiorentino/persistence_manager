#include "include/persistence_manager.h"

bool operator!=(int lhs, persistence_manager_status rhs) {
    return lhs != static_cast<int>(rhs);
}

bool operator==(persistence_manager_status lhs, int rhs) {
    return static_cast<int>(lhs) == rhs;
}

struct persistence_manager::Impl {
    sqlite3 *persistentDB;
    sqlite3_stmt *stmt;
};

persistence_manager::persistence_manager() :
        pImpl(std::make_unique<Impl>()) {
}

persistence_manager::~persistence_manager() = default;

persistence_manager::persistence_manager(persistence_manager &&rhs) noexcept = default;

persistence_manager &persistence_manager::operator=(persistence_manager &&rhs) noexcept = default;

int persistence_manager::open(std::string const &db_name) noexcept {
    return sqlite3_open(db_name.c_str(), &pImpl->persistentDB);
}

int persistence_manager::close() noexcept {
    return sqlite3_close_v2(pImpl->persistentDB);
}

int persistence_manager::begin_transaction() noexcept {
    int rc = sqlite3_prepare_v2(pImpl->persistentDB, "BEGIN TRANSACTION", -1, &pImpl->stmt, nullptr);
    if(rc != persistence_manager_status::STATUS_OK) {
        // TODO: gestione errori
        auto s = sqlite3_errmsg(pImpl->persistentDB);
        return rc;
    }
    rc = sqlite3_step(pImpl->stmt);
    if(rc != persistence_manager_status::STATUS_DONE) {
        // TODO: gestione errori
        auto s = sqlite3_errmsg(pImpl->persistentDB);
        return rc;
    }
    return sqlite3_finalize(pImpl->stmt);
}

int persistence_manager::commit() noexcept {
    int rc = sqlite3_prepare_v2(pImpl->persistentDB, "COMMIT", -1, &pImpl->stmt, nullptr);
    if(rc != persistence_manager_status::STATUS_OK) {
        // TODO: gestione errori
        auto s = sqlite3_errmsg(pImpl->persistentDB);
        return rc;
    }
    rc = sqlite3_step(pImpl->stmt);
    if(rc != persistence_manager_status::STATUS_DONE) {
        // TODO: gestione errori
        auto s = sqlite3_errmsg(pImpl->persistentDB);
        return rc;
    }
    return sqlite3_finalize(pImpl->stmt);
}

int persistence_manager::create_data_table() noexcept {
    // creazione tabella principale
    char const *sql_string = R"(
CREATE TABLE IF NOT EXISTS `data_table` (
	`id`	INTEGER PRIMARY KEY AUTOINCREMENT,
	`timestamp`	TEXT NOT NULL,
	`sensor_type`	TEXT NOT NULL
))";
    int rc = sqlite3_prepare_v2(pImpl->persistentDB, sql_string, -1, &pImpl->stmt, nullptr);
    if(rc != persistence_manager_status::STATUS_OK) {
        // TODO: gestione errori
        auto s = sqlite3_errmsg(pImpl->persistentDB);
        return rc;
    }
    rc = sqlite3_step(pImpl->stmt);
    if(rc != persistence_manager_status::STATUS_DONE) {
        // TODO: gestione errori
        auto s = sqlite3_errmsg(pImpl->persistentDB);
        return rc;
    }
    return sqlite3_finalize(pImpl->stmt);
}

int persistence_manager::insert_into_data_table(registry_item_t const &ri) noexcept {
    // inserimento dati nel db aperto
    char const *sql_string = R"(
INSERT INTO `data_table`(
    `timestamp`,
    `sensor_type`) VALUES (?, ?))";
    int rc = sqlite3_prepare_v2(pImpl->persistentDB, sql_string, -1, &pImpl->stmt, nullptr);
    if(rc != persistence_manager_status::STATUS_OK) {
        // TODO: gestione errori
        auto s = sqlite3_errmsg(pImpl->persistentDB);
        return rc;
    }
    rc = sqlite3_bind_text(pImpl->stmt, 1, ri.timestamp.c_str(), -1, SQLITE_STATIC);
    if(rc != persistence_manager_status::STATUS_OK) {
        // TODO: gestione errori
        auto s = sqlite3_errmsg(pImpl->persistentDB);
        return rc;
    }
    rc = sqlite3_bind_text(pImpl->stmt, 2, ri.sensor_type.c_str(), -1, SQLITE_STATIC);
    if(rc != persistence_manager_status::STATUS_OK) {
        // TODO: gestione errori
        auto s = sqlite3_errmsg(pImpl->persistentDB);
        return rc;
    }
    rc = sqlite3_step(pImpl->stmt);
    if(rc != persistence_manager_status::STATUS_DONE) {
        // TODO: gestione errori
        auto s = sqlite3_errmsg(pImpl->persistentDB);
        return rc;
    }
    return sqlite3_finalize(pImpl->stmt);
}

int persistence_manager::performSQLiteGetQuery(const std::string &sql_string) noexcept {
    int rc = sqlite3_prepare_v2(pImpl->persistentDB, sql_string.c_str(), -1, &pImpl->stmt, nullptr);
    if(rc != persistence_manager_status::STATUS_OK) {
        // TODO: gestione errori
        auto s = sqlite3_errmsg(pImpl->persistentDB);
        return rc;
    }
    return sqlite3_step(pImpl->stmt);
}

std::map<int, std::vector<std::string>>
persistence_manager::performSQLiteSelectQuery(const std::string &sql_string) noexcept {
    std::map<int, std::vector<std::string>> returnMapVectorString;
    auto rc = performSQLiteGetQuery(sql_string);
    if(rc != persistence_manager_status::STATUS_ROW && rc != persistence_manager_status::STATUS_DONE) {
        return returnMapVectorString;
    }
    int row = 0;
    while(persistence_manager_status::STATUS_ROW == rc) {
        const auto cols = sqlite3_column_count(pImpl->stmt);
        std::vector<std::string> returnVectorString;
        for(int col = 0; col < cols; ++col) {
            returnVectorString.push_back(getSQLiteColumnText(col));
        }
        returnMapVectorString[row] = returnVectorString;
        rc = sqlite3_step(pImpl->stmt);
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
