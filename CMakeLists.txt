cmake_minimum_required(VERSION 3.5)

project(sanjuuni-ui VERSION 0.5 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(OpenSSL REQUIRED)
find_package(OpenCL)
find_package(Poco CONFIG REQUIRED COMPONENTS Foundation XML JSON Util Net NetSSL)
find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(AVCODEC libavcodec)
    if(NOT AVCODEC_FOUND)
        find_path(AVCODEC_INCLUDE_DIRS libavcodec/avcodec.h REQUIRED)
        find_library(AVCODEC_LIBRARIES avcodec REQUIRED)
    endif()
    pkg_check_modules(AVDEVICE libavdevice)
    if(NOT AVDEVICE_FOUND)
        find_path(AVDEVICE_INCLUDE_DIRS libavdevice/avdevice.h REQUIRED)
        find_library(AVDEVICE_LIBRARIES avdevice REQUIRED)
    endif()
    pkg_check_modules(AVFILTER libavfilter)
    if(NOT AVFILTER_FOUND)
        find_path(AVFILTER_INCLUDE_DIRS libavfilter/avfilter.h REQUIRED)
        find_library(AVFILTER_LIBRARIES avfilter REQUIRED)
    endif()
    pkg_check_modules(AVFORMAT libavformat)
    if(NOT AVFORMAT_FOUND)
        find_path(AVFORMAT_INCLUDE_DIRS libavformat/avformat.h REQUIRED)
        find_library(AVFORMAT_LIBRARIES avformat REQUIRED)
    endif()
    pkg_check_modules(AVUTIL libavutil)
    if(NOT AVUTIL_FOUND)
        find_path(AVUTIL_INCLUDE_DIRS libavutil/avutil.h REQUIRED)
        find_library(AVUTIL_LIBRARIES avutil REQUIRED)
    endif()
    pkg_check_modules(SWRESAMPLE libswresample)
    if(NOT SWRESAMPLE_FOUND)
        find_path(SWRESAMPLE_INCLUDE_DIRS libswresample/swresample.h REQUIRED)
        find_library(SWRESAMPLE_LIBRARIES swresample REQUIRED)
    endif()
    pkg_check_modules(SWSCALE libswscale)
    if(NOT SWSCALE_FOUND)
        find_path(SWSCALE_INCLUDE_DIRS libswscale/swscale.h REQUIRED)
        find_library(SWSCALE_LIBRARIES swscale REQUIRED)
    endif()
    pkg_check_modules(ZLIB zlib)
    if(NOT ZLIB_FOUND)
        find_path(ZLIB_INCLUDE_DIRS zlib.h REQUIRED)
        find_library(ZLIB_LIBRARIES z REQUIRED)
    endif()
else()
    find_path(AVCODEC_INCLUDE_DIRS libavcodec/avcodec.h REQUIRED)
    find_library(AVCODEC_LIBRARIES avcodec REQUIRED)
    find_path(AVDEVICE_INCLUDE_DIRS libavdevice/avdevice.h REQUIRED)
    find_library(AVDEVICE_LIBRARIES avdevice REQUIRED)
    find_path(AVFILTER_INCLUDE_DIRS libavfilter/avfilter.h REQUIRED)
    find_library(AVFILTER_LIBRARIES avfilter REQUIRED)
    find_path(AVFORMAT_INCLUDE_DIRS libavformat/avformat.h REQUIRED)
    find_library(AVFORMAT_LIBRARIES avformat REQUIRED)
    find_path(AVUTIL_INCLUDE_DIRS libavutil/avutil.h REQUIRED)
    find_library(AVUTIL_LIBRARIES avutil REQUIRED)
    find_path(SWRESAMPLE_INCLUDE_DIRS libswresample/swresample.h REQUIRED)
    find_library(SWRESAMPLE_LIBRARIES swresample REQUIRED)
    find_path(SWSCALE_INCLUDE_DIRS libswscale/swscale.h REQUIRED)
    find_library(SWSCALE_LIBRARIES swscale REQUIRED)
    find_path(ZLIB_INCLUDE_DIRS zlib.h REQUIRED)
    find_library(ZLIB_LIBRARIES z REQUIRED)
endif()

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        resizelabel.h resizelabel.cpp
        advanceddialog.h advanceddialog.cpp advanceddialog.ui
        resource.qrc
        sanjuuni/src/cc-pixel.cpp sanjuuni/src/cc-pixel-cl.cpp sanjuuni/src/generator.cpp sanjuuni/src/octree.cpp sanjuuni/src/quantize.cpp sanjuuni/src/sanjuuni.hpp sanjuuni/src/sanjuuni.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(sanjuuni-ui
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET sanjuuni-ui APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(sanjuuni-ui SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(sanjuuni-ui
            ${PROJECT_SOURCES}
        )
    endif()
endif()

add_compile_definitions(main=sanjuuni_main)
add_compile_definitions(STATUS_FUNCTION=ui_update_status)
if(OpenCL_FOUND)
    add_compile_definitions(HAS_OPENCL=1)
    target_link_libraries(sanjuuni-ui PRIVATE OpenCL::OpenCL)
endif()

target_include_directories(sanjuuni-ui PRIVATE ${AVCODEC_INCLUDE_DIRS} ${AVDEVICE_INCLUDE_DIRS} ${AVFILTER_INCLUDE_DIRS} ${AVFORMAT_INCLUDE_DIRS} ${AVUTIL_INCLUDE_DIRS} ${SWRESAMPLE_INCLUDE_DIRS} ${SWSCALE_INCLUDE_DIRS})
target_include_directories(sanjuuni-ui PRIVATE ${ZLIB_INCLUDE_DIRS})

target_link_libraries(sanjuuni-ui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(sanjuuni-ui PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(sanjuuni-ui PRIVATE Poco::Net Poco::NetSSL Poco::Util)
target_link_libraries(sanjuuni-ui PRIVATE ${AVCODEC_LIBRARIES} ${AVDEVICE_LIBRARIES} ${AVFILTER_LIBRARIES} ${AVFORMAT_LIBRARIES} ${AVUTIL_LIBRARIES} ${SWRESAMPLE_LIBRARIES} ${SWSCALE_LIBRARIES})
target_link_libraries(sanjuuni-ui PRIVATE ${ZLIB_LIBRARIES})

if(WIN32)
    string(REGEX MATCH "^[A-Z]+" DRIVE_LETTER ${PROJECT_SOURCE_DIR})
    string(TOLOWER ${DRIVE_LETTER} DRIVE_LETTER_LOWER)
    string(REGEX REPLACE "^([A-Z]+):" "/mnt/${DRIVE_LETTER_LOWER}" BASH_SOURCE_DIR ${PROJECT_SOURCE_DIR})
else()
    set(BASH_SOURCE_DIR ${PROJECT_SOURCE_DIR})
endif()

add_custom_command(
    OUTPUT ${PROJECT_SOURCE_DIR}/sanjuuni/src/cc-pixel-cl.cpp
    DEPENDS ${PROJECT_SOURCE_DIR}/sanjuuni/src/cc-pixel.cpp
    COMMAND bash ARGS ${BASH_SOURCE_DIR}/makecl.sh ${BASH_SOURCE_DIR}/sanjuuni/src/cc-pixel.cpp ${BASH_SOURCE_DIR}/sanjuuni/src/cc-pixel-cl.cpp)

set_target_properties(sanjuuni-ui PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER cc.craftos-pc.sanjuuni
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS sanjuuni-ui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(sanjuuni-ui)
endif()
