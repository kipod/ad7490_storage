cmake_minimum_required(VERSION 3.0.0)
# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)
project(writer VERSION 0.1.0 LANGUAGES C CXX)

# <---------- set c++ standard ------------->
# NOTE: you must build redis-plus-plus and your application code with the same standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



include(CTest)
enable_testing()

set(SOURCES
    qu.cpp
    qu.hpp
    qdata.cpp
    qdata.hpp
    main.cpp
    config.cpp
    config.hpp
    test_q.cpp
    test_q.hpp
    # ad7490.cpp
    # ad7490.hpp
    uart.cpp
    uart.hpp
    )

add_executable(writer ${SOURCES})

# <------------ add hiredis dependency --------------->
find_path(HIREDIS_HEADER hiredis)
target_include_directories(writer PUBLIC ${HIREDIS_HEADER})

find_library(HIREDIS_LIB hiredis)
target_link_libraries(writer ${HIREDIS_LIB})

# <------------ add redis-plus-plus dependency -------------->
# NOTE: this should be *sw* NOT *redis++*
find_path(REDIS_PLUS_PLUS_HEADER sw)
target_include_directories(writer PUBLIC ${REDIS_PLUS_PLUS_HEADER})

find_library(REDIS_PLUS_PLUS_LIB redis++)
target_link_libraries(writer ${REDIS_PLUS_PLUS_LIB})

# <------------ add wiringPi dependency -------------->
find_path(WIRINGPI_HEADER wiringPi)
target_include_directories(writer PUBLIC ${WIRINGPI_HEADER})

find_library(WIRINGPI_LIB wiringPi)
target_link_libraries(writer ${WIRINGPI_LIB})


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
