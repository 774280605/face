cmake_minimum_required(VERSION 3.17)
project(zbk_service)

set(CMAKE_CXX_STANDARD  11)
include_directories(${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}/include/3rd_inc/libjpeg
                    ${PROJECT_SOURCE_DIR}/common/include)

#pack
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions(-DENABLE_DEBUG)
endif(CMAKE_BUILD_TYPE STREQUAL Debug)



include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_DIRECTORY ${PROJECT_SOURCE_DIR}/pack)
set(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")
set(PROJECT_VERSION_FULL ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
set(CPACK_SOURCE_IGNORE_FILES
        ${PROJECT_SOURCE_DIR}/cmake-build-debug
        ${PROJECT_SOURCE_DIR}/pack
        ${PROJECT_SOURCE_DIR}/models
        ${PROJECT_SOURCE_DIR}/.git
        ${PROJECT_SOURCE_DIR}/.idea
        )

set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_PACKAGE_FILE_NAME zbk_service-${PROJECT_VERSION_FULL})
include(CPack)
#pack



add_definitions(-w)



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)


file(GLOB SRC_LIST "${PROJECT_SOURCE_DIR}/common/src/*.c"
                    "${PROJECT_SOURCE_DIR}/src/*")
link_directories(${PROJECT_SOURCE_DIR}/lib
                ${PROJECT_SOURCE_DIR}/lib/3rd_lib)

add_executable(zbk_service src/face_quality.cpp ${SRC_LIST})

target_link_libraries(zbk_service
        libjpeg.a
        libturbojpeg.a
        libmegface.so
        libmeglayer1.so
        libevent.a
        libjemalloc.so
        pthread)



#pack
