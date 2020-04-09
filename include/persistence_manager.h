#ifndef SM1CMA_PERSISTENCE_MANAGER_H
#define SM1CMA_PERSISTENCE_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <type_traits>

#include "registry_item_t.h"

enum class persistence_manager_status {
    KO = -1, OK, ROW, DONE
};

persistence_manager_status sqlite_rc_to_persistence_manager_status(int rc);

struct persistence_manager_open_close_interface {
    virtual persistence_manager_status open(std::string const &) noexcept = 0;

    virtual persistence_manager_status close() noexcept = 0;

    virtual ~persistence_manager_open_close_interface() = 0;
};

inline persistence_manager_open_close_interface::~persistence_manager_open_close_interface() = default;

struct persistence_manager_begin_transaction_commit_interface {
    virtual persistence_manager_status begin_transaction() noexcept = 0;

    virtual persistence_manager_status commit() noexcept = 0;

    virtual persistence_manager_status rollback() noexcept = 0;

    virtual ~persistence_manager_begin_transaction_commit_interface() = 0;
};

inline persistence_manager_begin_transaction_commit_interface::~persistence_manager_begin_transaction_commit_interface() = default;

struct persistence_manager_insertable_interface {
    virtual persistence_manager_status insert_into_data_table(registry_item_t const &ri) noexcept = 0;

    virtual ~persistence_manager_insertable_interface() = 0;
};

inline persistence_manager_insertable_interface::~persistence_manager_insertable_interface() = default;

struct persistence_manager_selectable_interface {
    [[nodiscard]] virtual std::vector<registry_item_t> select_all_from_data_table() noexcept = 0;

    virtual ~persistence_manager_selectable_interface() = 0;
};

inline persistence_manager_selectable_interface::~persistence_manager_selectable_interface() = default;

struct persistence_manager_interface
        : public persistence_manager_open_close_interface,
          public persistence_manager_begin_transaction_commit_interface,
          public persistence_manager_insertable_interface,
          public persistence_manager_selectable_interface {

    [[nodiscard]] virtual std::string get_db_name(std::string const &) const noexcept = 0;

    virtual persistence_manager_status create_data_table() noexcept = 0;

    ~persistence_manager_interface() noexcept override = 0;
};

inline persistence_manager_interface::~persistence_manager_interface() noexcept = default;

struct persistence_manager : public persistence_manager_interface {
    persistence_manager();

    persistence_manager(persistence_manager &&rhs) noexcept;

    persistence_manager &operator=(persistence_manager &&rhs) noexcept;

    ~persistence_manager() noexcept override;

    [[nodiscard]] std::string get_db_name(std::string const &timestamp) const noexcept override {
        auto db_name = timestamp.substr(0, 13);
        db_name += ".db";
        return db_name;
    }

    [[nodiscard]] persistence_manager_status open(std::string const &db_name) noexcept override;

    [[nodiscard]] persistence_manager_status close() noexcept override;

    [[nodiscard]] persistence_manager_status begin_transaction() noexcept override;

    [[nodiscard]] persistence_manager_status commit() noexcept override;

    [[nodiscard]] persistence_manager_status rollback() noexcept final;

    [[nodiscard]] persistence_manager_status create_data_table() noexcept override;

    [[nodiscard]] persistence_manager_status insert_into_data_table(registry_item_t const &ri) noexcept override;

    [[nodiscard]] std::vector<registry_item_t> select_all_from_data_table() noexcept override;

protected:
    [[nodiscard]] persistence_manager_status performSQLiteGetQuery(const std::string &sql_string) noexcept;

    [[nodiscard]] std::string getSQLiteColumnText(int col) const noexcept;

    [[nodiscard]] std::map<int, std::vector<std::string>>
    performSQLiteSelectQuery(const std::string &sql_string) noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

#endif //SM1CMA_PERSISTENCE_MANAGER_H
