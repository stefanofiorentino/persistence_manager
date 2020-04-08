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
    ASSERT_EQ(0, db->open(db_name));
    ASSERT_EQ(0, db->close());
}

TEST(PersistenceManager, BeginTransactionCommit) {
    auto db = std::make_unique<persistence_manager>();
    ASSERT_TRUE(db);
    registry_item_t ri;
    ri.timestamp = getISOCurrentTimestamp();
    auto db_name = db->get_db_name(ri.timestamp);
    std::remove(db_name.c_str());
    ASSERT_EQ(0, db->open(db_name));
    ASSERT_EQ(0, db->begin_transaction());
    ASSERT_EQ(0, db->commit());
    ASSERT_EQ(0, db->close());
}

TEST(PersistenceManager, CreateDataTable) {
    auto db = std::make_unique<persistence_manager>();
    ASSERT_TRUE(db);
    registry_item_t ri;
    ri.timestamp = getISOCurrentTimestamp();
    auto db_name = db->get_db_name(ri.timestamp);
    std::remove(db_name.c_str());
    ASSERT_EQ(0, db->open(db_name));
    ASSERT_EQ(0, db->begin_transaction());
    ASSERT_EQ(0, db->create_data_table());
    ASSERT_EQ(0, db->commit());
    ASSERT_EQ(0, db->close());
}

TEST(PersistenceManager, InsertIntoDataTable) {
    auto db = std::make_unique<persistence_manager>();
    ASSERT_TRUE(db);
    registry_item_t ri;
    ri.timestamp = getISOCurrentTimestamp();
    auto db_name = db->get_db_name(ri.timestamp);
    std::remove(db_name.c_str());
    ASSERT_EQ(0, db->open(db_name));
    ASSERT_EQ(0, db->begin_transaction());
    ASSERT_EQ(0, db->create_data_table());
    ASSERT_EQ(0, db->insert_into_data_table(ri));
    ASSERT_EQ(0, db->commit());
    ASSERT_EQ(0, db->close());
}

TEST(PersistenceManager, SelectAllFromDataTable) {
    auto db = std::make_unique<persistence_manager>();
    ASSERT_TRUE(db);
    registry_item_t ri;
    ri.timestamp = getISOCurrentTimestamp();
    ri.sensor_type = "sensor_type";
    auto db_name = db->get_db_name(ri.timestamp);
    std::remove(db_name.c_str());
    ASSERT_EQ(0, db->open(db_name));
    ASSERT_EQ(0, db->begin_transaction());
    ASSERT_EQ(0, db->create_data_table());
    ASSERT_EQ(0, db->insert_into_data_table(ri));
    ASSERT_EQ(0, db->commit());
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
    ASSERT_EQ(0, db->close());
}

static int close_test_helper(persistence_manager &&pm) {
    return pm.close();
}

TEST(PersistenceManager, MoveOperators) {
    auto db = std::make_unique<persistence_manager>();
    ASSERT_TRUE(db);
    registry_item_t ri;
    ri.timestamp = getISOCurrentTimestamp();
    auto db_name = db->get_db_name(ri.timestamp);
    std::remove(db_name.c_str());
    ASSERT_EQ(0, db->open(db_name));
    ASSERT_EQ(0, close_test_helper(std::move(*db)));
}
