include_directories(".")

set(SOURCES main.cpp)

add_executable(${PROJECT_NAME}-test ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})

SOURCE_GROUP("" FILES ${SOURCES})
SOURCE_GROUP("${PROJECT_NAME}" FILES ${HEADERS_GENERAL})
SOURCE_GROUP("${PROJECT_NAME}\\policy" FILES ${HEADERS_POLICY})
SOURCE_GROUP("${PROJECT_NAME}\\storage" FILES ${HEADERS_STORAGE})

add_test(NAME ${PROJECT_NAME}-test
         COMMAND ${PROJECT_NAME}-test --success)