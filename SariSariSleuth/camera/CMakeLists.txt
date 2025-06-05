cmake_minimum_required(VERSION 3.16)

project(qt6project VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(TARGET_NAME appqt6project)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Multimedia)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appqt6project
    main.cpp
)

qt_add_qml_module(appqt6project
    URI qt6project
    VERSION 1.0
    QML_FILES Main.qml
    SOURCES include/DynamsoftBarcodeReader.h include/DynamsoftCommon.h
    SOURCES FrameProcessor.cpp FrameProcessor.h
    SOURCES ImageProcessor.h ImageProcessor.cpp
)

set_target_properties(appqt6project PROPERTIES
    WIN32_EXECUTABLE TRUE
)

include_directories("${PROJECT_SOURCE_DIR}/include/")

target_link_libraries(${TARGET_NAME} PRIVATE Qt6::Quick Qt::Multimedia)

# Windows-specific configuration
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(${TARGET_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/libs/windows/DynamsoftBarcodeReaderx64.dll")
else() # Assuming MSVC or other non-GNU compilers
    target_link_libraries(${TARGET_NAME} PRIVATE "DBRx64")
endif()

# Copy DLLs to build directory after build
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/libs/windows/"
    $<TARGET_FILE_DIR:${TARGET_NAME}>)

include(GNUInstallDirs)
install(TARGETS appqt6project
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
