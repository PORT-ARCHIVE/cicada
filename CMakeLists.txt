cmake_minimum_required(VERSION 2.8)

project(MyProject CXX)

if( CMAKE_SYSTEM_NAME MATCHES "Darwin" )
  set(CMAKE_CXX_FLAGS "-std=c++11")
else()
  set(CMAKE_CXX_FLAGS "-std=c++11 -pthread")
  set(CMAKE_CXX_COMPILER /opt/rh/devtoolset-3/root/usr/bin/g++)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

execute_process (
	COMMAND mecab-config --libs
	COMMAND awk "{ print $1 }"
	COMMAND perl -pe "s/-L//"
	OUTPUT_VARIABLE
	MECAB_LIB_PATH
	OUTPUT_STRIP_TRAILING_WHITESPACE
	)

include_directories(../src/spdlog/include)
include_directories(/usr/local/include)

add_executable(semicrf Main.cpp SemiCrf.cpp SemiCrfData.cpp FeatureFunction.cpp Logger.cpp MultiByteTokenizer.cpp Error.cpp Optimizer.cpp W2V.cpp ujson.cpp double-conversion.cc JsonIO.cpp Signal.cpp)
add_executable(gen Generator.cpp Logger.cpp Error.cpp ujson.cpp double-conversion.cc)
add_executable(bd2c	bd2c.cpp Logger.cpp MultiByteTokenizer.cpp Error.cpp W2V.cpp ujson.cpp double-conversion.cc JsonIO.cpp)
add_executable(bdc2e bdc2e.cpp SemiCrfData.cpp Logger.cpp MultiByteTokenizer.cpp Error.cpp W2V.cpp ujson.cpp double-conversion.cc JsonIO.cpp)
add_executable(jcmp jcmp.cpp Logger.cpp Error.cpp ujson.cpp double-conversion.cc JsonIO.cpp)
find_library(MECAB_LIB mecab HINTS ${MECAB_LIB_PATH})

target_link_libraries(semicrf ${MECAB_LIB})
target_link_libraries(semicrf stdc++)
target_link_libraries(bd2c ${MECAB_LIB})
target_link_libraries(bd2c stdc++)
target_link_libraries(gen stdc++)
target_link_libraries(bdc2e stdc++)
target_link_libraries(jcmp stdc++)

message(STATUS ${MECAB_LIB_PATH})
message(STATUS ${MECAB_LIB})
message(STATUS ${CMAKE_SYSTEM})
message(STATUS ${CMAKE_CXX_COMPILER})
