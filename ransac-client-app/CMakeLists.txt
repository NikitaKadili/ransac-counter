cmake_minimum_required(VERSION 3.5)

project(ransac-client-app VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network REQUIRED)
find_package(QT NAMES Qt6 Qt5 COMPONENTS PrintSupport REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS PrintSupport REQUIRED)
# Protobuf
find_package(Protobuf REQUIRED)
find_package(Threads REQUIRED)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS
    src/data_message.proto
)

set(PROJECT_SOURCES
        src/main.cpp

        src/ransacclientapp.h
        src/ransacclientapp.cpp
        src/ransacclientapp.ui

        src/client.h
        src/client.cpp

        src/qcustomplot.h
        src/qcustomplot.cpp

        src/editportdialog.h
        src/editportdialog.cpp
        src/editportdialog.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ransac-client-app
        MANUAL_FINALIZATION
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ransac-client-app APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ransac-client-app SHARED
            ${PROTO_SRCS}
            ${PROTO_HDRS}
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ransac-client-app
            ${PROTO_SRCS}
            ${PROTO_HDRS}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ransac-client-app PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(ransac-client-app PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(ransac-client-app PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)
# Protobuf
target_link_directories(ransac-client-app PRIVATE ${Protobuf_INCLUDE_DIRS})
target_link_directories(ransac-client-app PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(ransac-client-app PRIVATE ${Protobuf_LIBRARY} Threads::Threads)

set_target_properties(ransac-client-app PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ransac-client-app)
endif()
