cmake_minimum_required(VERSION 3.17)
project(ZBK_SERVICE)

set(CMAKE_CXX_STANDARD  11)
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}/include/3rd_inc/libjpeg
                    ${PROJECT_SOURCE_DIR}/common/include)

message(${PROJECT_SOURCE_DIR})


file(GLOB SRC_LIST "${PROJECT_SOURCE_DIR}/common/src/*.c")
link_directories(${PROJECT_SOURCE_DIR}/lib
                ${PROJECT_SOURCE_DIR}/lib/3rd_lib)

add_executable(ZBK_SERVICE face_quality.cpp ${SRC_LIST} Service.cpp Service.h)

target_link_libraries(ZBK_SERVICE
        libjpeg.a
        libturbojpeg.a
        libmegface.so
        libmeglayer1.so
        libevent.a
        libjemalloc.so
        pthread)