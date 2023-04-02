cmake_minimum_required(VERSION 3.24)
project(math_matters)

set(CMAKE_CXX_STANDARD 17)

add_executable(math_matters src/main.cpp)
add_executable(tests src/tests.cpp)
include_directories(src)

# Download and unpack Catch2
Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.0.1
)

# Fetch FTXUI

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    )

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

FetchContent_MakeAvailable(Catch2)


target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(math_matters
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component # Not needed for this example.
    )