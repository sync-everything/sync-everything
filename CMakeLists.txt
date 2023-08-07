cmake_minimum_required(VERSION 3.8)

project(sync-everything LANGUAGES C CXX VERSION 0.0.1)

add_executable(sync-everything "")

target_sources(sync-everything
        PRIVATE
        "src/main.cpp")
target_include_directories(sync-everything
        PRIVATE
        "src")