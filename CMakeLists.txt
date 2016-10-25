cmake_minimum_required(VERSION 2.8)

project(HomeClimateControl C)

file(GLOB srcs src/*.c)
file(GLOB hdrs src/*.h)

add_executable(homeclimatecontrol ${srcs} ${hdrs})
target_link_libraries(homeclimatecontrol letmecreate_click letmecreate_core)
install(TARGETS homeclimatecontrol RUNTIME DESTINATION bin)
