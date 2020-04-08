set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

function(ADD_SM1CMA_TEST TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})

    target_link_libraries(
            ${TEST_NAME}
            PRIVATE
            GTest::Main
            sqlite3
    )

    target_include_directories(${TEST_NAME} PRIVATE ${PROJECT_SOURCE_DIR})
    add_test(NAME pm_${TEST_NAME} COMMAND $<TARGET_FILE:${TEST_NAME}>)
endfunction()

set(PERSISTENCE_MANAGER_SOURCES cma/persistence_manager.cpp ${PROJECT_SOURCE_DIR}/src/persistence_manager.cpp)
ADD_SM1CMA_TEST(persistence_manager "${PERSISTENCE_MANAGER_SOURCES}")
