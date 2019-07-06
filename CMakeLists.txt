cmake_minimum_required(VERSION 3.0.0)
project(NDNS VERSION 0.1.0)

include(CTest)
enable_testing()

aux_source_directory(Sources SRC)

add_executable(NDNS ${SRC})
target_link_libraries(NDNS PUBLIC -lboost_system -lpthread -lsfml-audio -lsfml-system) 

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
