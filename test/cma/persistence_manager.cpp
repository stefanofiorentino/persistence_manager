#include <gtest/gtest.h>
#include <include/isodate.hpp>
#include <include/persistence_manager.h>

TEST(PersistenceManager, OpenClose) {
    auto db = std::make_unique<persistence_manager>();
    ASSERT_TRUE(db);
    registry_item_t ri;
    ri.timestamp = getISOCurrentTimestamp();
    auto db_name = db->get_db_name(ri.timestamp);
    std::remove(db_name.c_str());
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->open(db_name));
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->close());
}

TEST(PersistenceManager, BeginTransactionCommit) {
    auto db = std::make_unique<persistence_manager>();
    ASSERT_TRUE(db);
    registry_item_t ri;
    ri.timestamp = getISOCurrentTimestamp();
    auto db_name = db->get_db_name(ri.timestamp);
    std::remove(db_name.c_str());
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->open(db_name));
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->begin_transaction());
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->commit());
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->close());
}

TEST(PersistenceManager, CreateDataTable) {
    auto db = std::make_unique<persistence_manager>();
    ASSERT_TRUE(db);
    registry_item_t ri;
    ri.timestamp = getISOCurrentTimestamp();
    auto db_name = db->get_db_name(ri.timestamp);
    std::remove(db_name.c_str());
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->open(db_name));
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->begin_transaction());
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->create_data_table());
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->commit());
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->close());
}

TEST(PersistenceManager, InsertIntoDataTable) {
    auto db = std::make_unique<persistence_manager>();
    ASSERT_TRUE(db);
    registry_item_t ri;
    ri.timestamp = getISOCurrentTimestamp();
    auto db_name = db->get_db_name(ri.timestamp);
    std::remove(db_name.c_str());
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->open(db_name));
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->begin_transaction());
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->create_data_table());
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->insert_into_data_table(ri));
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->commit());
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->close());
}

TEST(PersistenceManager, SelectAllFromDataTable) {
    auto db = std::make_unique<persistence_manager>();
    ASSERT_TRUE(db);
    registry_item_t ri;
    ri.timestamp = getISOCurrentTimestamp();
    ri.sensor_type = "sensor_type";
    auto db_name = db->get_db_name(ri.timestamp);
    std::remove(db_name.c_str());
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->open(db_name));
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->begin_transaction());
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->create_data_table());
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->insert_into_data_table(ri));
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->commit());
    auto items = db->select_all_from_data_table();
    ASSERT_FALSE(items.empty());
    auto string = items.at(0).timestamp;
    auto basicString = ri.timestamp;
    bool found{false};
    for(auto const &item : items) {
        if(ri.timestamp == item.timestamp) {
            found = true;
            ASSERT_EQ("sensor_type", item.sensor_type);
        }
    }
    ASSERT_TRUE(found);
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->close());
}

static persistence_manager_status close_test_helper(persistence_manager &&pm) {
    return pm.close();
}

TEST(PersistenceManager, MoveOperators) {
    auto db = std::make_unique<persistence_manager>();
    ASSERT_TRUE(db);
    registry_item_t ri;
    ri.timestamp = getISOCurrentTimestamp();
    auto db_name = db->get_db_name(ri.timestamp);
    std::remove(db_name.c_str());
    ASSERT_EQ(persistence_manager_status::STATUS_OK, db->open(db_name));
    ASSERT_EQ(persistence_manager_status::STATUS_OK, close_test_helper(std::move(*db)));
}
