cmake_minimum_required(VERSION 3.5)

project(sanjuuni-ui VERSION 0.1 LANGUAGES CXX)

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
find_package(Poco CONFIG REQUIRED COMPONENTS Net NetSSL Util Foundation)
find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)
find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h)
find_library(AVDEVICE_LIBRARY avdevice)
find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)
find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h)
find_library(AVUTIL_LIBRARY avutil)
find_path(SWRESAMPLE_INCLUDE_DIR libswresample/swresample.h)
find_library(SWRESAMPLE_LIBRARY swresample)
find_path(SWSCALE_INCLUDE_DIR libswscale/swscale.h)
find_library(SWSCALE_LIBRARY swscale)
find_path(ZLIB_INCLUDE_DIR zlib.h)
find_library(ZLIB_LIBRARY z)

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
endif()

target_include_directories(sanjuuni-ui PRIVATE ${AVCODEC_INCLUDE_DIR} ${AVDEVICE_INCLUDE_DIR} ${AVFORMAT_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${SWRESAMPLE_INCLUDE_DIR} ${SWSCALE_INCLUDE_DIR})
target_include_directories(sanjuuni-ui PRIVATE ${ZLIB_INCLUDE_DIR})

target_link_libraries(sanjuuni-ui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(sanjuuni-ui PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(sanjuuni-ui PRIVATE OpenCL::OpenCL)
target_link_libraries(sanjuuni-ui PRIVATE Poco::Net Poco::NetSSL Poco::Util)
target_link_libraries(sanjuuni-ui PRIVATE ${AVCODEC_LIBRARY} ${AVDEVICE_LIBRARY} ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY} ${SWRESAMPLE_LIBRARY} ${SWSCALE_LIBRARY})
target_link_libraries(sanjuuni-ui PRIVATE ${ZLIB_LIBRARY})

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
