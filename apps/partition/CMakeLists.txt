# -----------------------------------------------------------------------------
# yagkp_partition: разбиение графа только средствами YAGkP
# -----------------------------------------------------------------------------
add_executable(yagkp_partition main.cpp)
target_link_libraries(yagkp_partition PRIVATE yagkp)