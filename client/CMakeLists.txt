cmake_minimum_required(VERSION 3.15)

set(TARGET_NAME server)

file(GLOB ${PROJECT_NAME}_SRC
    "../depends/src/*.cpp"
    "main.cpp"
)

list (SORT ${PROJECT_NAME}_SRC)
add_executable(${TARGET_NAME} ${${PROJECT_NAME}_SRC} )

find_package(Threads REQUIRED)
find_package(glog REQUIRED)
target_link_libraries(${TARGET_NAME}
    PRIVATE
      Threads::Threads
      glog::glog
)

# 只给这个 target 加额外警告选项
target_compile_options(${TARGET_NAME}
    PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Wpedantic>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Wpedantic>
)

# 安装规则也写在这里
install(TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

