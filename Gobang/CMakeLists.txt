cmake_minimum_required(VERSION 3.5)
project(GomokuQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)   # 或 17（若你使用结构化绑定）
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt5 COMPONENTS Widgets REQUIRED)
set(QT_LIBS Qt5::Widgets)

add_executable(GomokuQt main.cpp)

# 自动运行 moc / uic
set_target_properties(GomokuQt PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

target_link_libraries(GomokuQt PRIVATE ${QT_LIBS})
