# Append custom CMake modules.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/functions)

include(FetchContentGoogleTest)
