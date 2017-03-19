cmake_minimum_required (VERSION 2.8.8)

project (ScreepsApi-test)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -ggdb")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# JSON
include_directories(external/json/src)

add_subdirectory ( external/ScreepsApi )

# ScreepsApi
include_directories(external/ScreepsApi/include)

# Web
include_directories(external/Simple-Web-Server)

set(BOOST_COMPONENTS system thread filesystem date_time coroutine context)
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
        set(BOOST_COMPONENTS ${BOOST_COMPONENTS} regex)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_BOOST_REGEX")
    endif()
endif()
find_package(Boost 1.53.0 COMPONENTS ${BOOST_COMPONENTS} REQUIRED)
include_directories(SYSTEM ${Boost_INCLUDE_DIR})
if(APPLE)
  set(OPENSSL_ROOT_DIR "/usr/local/opt/openssl")
endif()
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
message ( status "boost libs : ${Boost_LIBRARIES}" )

set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
 
set(PROJECT_SRCS 
    #
    ${PROJECT_SOURCE_DIR}/main.cpp
)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_INCLUDE_DIR}")

add_executable ( ${PROJECT_NAME} ${PROJECT_SRCS} )

target_link_libraries ( ${PROJECT_NAME} ScreepsApi )

target_link_libraries ( ${PROJECT_NAME} ${Boost_LIBRARIES} )
target_link_libraries ( ${PROJECT_NAME} ${OPENSSL_CRYPTO_LIBRARY} )
target_link_libraries ( ${PROJECT_NAME} ${OPENSSL_LIBRARIES})
