#ifndef SM1CMA_PERSISTENCE_MANAGER_H
#define SM1CMA_PERSISTENCE_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <type_traits>

#include <sqlite3.h>

#include "registry_item_t.h"

enum class persistence_manager_status {
    STATUS_OK = SQLITE_OK, STATUS_ROW = SQLITE_ROW, STATUS_DONE = SQLITE_DONE
};

bool operator!=(int lhs, persistence_manager_status rhs);

bool operator==(persistence_manager_status lhs, int rhs);

struct persistence_manager_open_close_interface {
    virtual int open(std::string const &) noexcept = 0;

    virtual int close() noexcept = 0;
};

struct persistence_manager_begin_transaction_commit_interface {
    virtual int begin_transaction() noexcept = 0;

    virtual int commit() noexcept = 0;
};

struct persistence_manager_insertable_interface {
    virtual int insert_into_data_table(registry_item_t const &ri) noexcept = 0;
};

struct persistence_manager_selectable_interface {
    [[nodiscard]] virtual std::vector<registry_item_t> select_all_from_data_table() noexcept = 0;
};

struct persistence_manager_interface
        : public persistence_manager_open_close_interface,
          public persistence_manager_begin_transaction_commit_interface,
          public persistence_manager_insertable_interface,
          public persistence_manager_selectable_interface {

    [[nodiscard]] virtual std::string get_db_name(std::string const &) const noexcept = 0;

    virtual int create_data_table() noexcept = 0;

    virtual ~persistence_manager_interface() noexcept = 0;
};

inline persistence_manager_interface::~persistence_manager_interface() noexcept = default;

struct persistence_manager : public persistence_manager_interface {
    persistence_manager();

    persistence_manager(persistence_manager &&rhs) noexcept;

    persistence_manager &operator=(persistence_manager &&rhs) noexcept;

    ~persistence_manager() override;

    [[nodiscard]] std::string get_db_name(std::string const &timestamp) const noexcept override {
        auto db_name = timestamp.substr(0, 13);
        db_name += ".db";
        return db_name;
    }

    [[nodiscard]] int open(std::string const &db_name) noexcept override;

    [[nodiscard]] int close() noexcept override;

    [[nodiscard]] int begin_transaction() noexcept override;

    [[nodiscard]] int commit() noexcept override;

    [[nodiscard]] int create_data_table() noexcept override;

    [[nodiscard]] int insert_into_data_table(registry_item_t const &ri) noexcept override;

    [[nodiscard]] std::vector<registry_item_t> select_all_from_data_table() noexcept override;

protected:
    [[nodiscard]] int performSQLiteGetQuery(const std::string &sql_string) noexcept;

    [[nodiscard]] std::string getSQLiteColumnText(int col) const noexcept;

    [[nodiscard]] std::map<int, std::vector<std::string>>
    performSQLiteSelectQuery(const std::string &sql_string) noexcept;
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif //SM1CMA_PERSISTENCE_MANAGER_H
