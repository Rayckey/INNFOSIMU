project(INNFOS_IMU)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

add_compile_options(-O3)
add_compile_options(-std=c++14)
add_compile_options(-fPIC)
add_compile_options(-pthread)
add_compile_options(-DASIO_STANDALONE)
add_compile_options(-DASIO_HAS_STD_CHRONO)
add_compile_options(-DIMU_ENABLE)
## -DEIGEN_NO_DEBUG


include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../ActuatorController/ActuatorController
    ${CMAKE_CURRENT_SOURCE_DIR}/../ActuatorController/ActuatorController/asio
)


set (
    INNFOX_IMU_SRCS
   ${CMAKE_CURRENT_SOURCE_DIR}/innfosimu.cpp
   ${CMAKE_CURRENT_SOURCE_DIR}/crosstimer.cpp
)



set (ACTUATOR_LIBRARY
    ${CMAKE_CURRENT_SOURCE_DIR}/../ActuatorController_SDK/sdk/lib/linux_x86_64/libActuatorController.so
)


add_library(${PROJECT_NAME} SHARED
    ${INNFOX_IMU_SRCS}
)

target_link_libraries(${PROJECT_NAME}
    ${ACTUATOR_LIBRARY}
    pthread
)

add_executable(mua MUA.cpp)
target_link_libraries(mua ${PROJECT_NAME})
add_dependencies(mua ${PROJECT_NAME} )





