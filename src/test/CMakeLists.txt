file(GLOB SOURCES *.cpp)

add_executable(unittest channel_test.cpp)

target_include_directories(unittest PRIVATE ${GTEST_INCLUDE} ${SYNCPP_INCLUDE})

target_link_libraries(unittest ${GTEST_LIB} ${GTEST_MAIN_LIB} pthread)