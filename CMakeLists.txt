cmake_minimum_required(VERSION 3.22.1)
project(umrt_geiger_interface)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ********** Setup CMake **********

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # enables building a static library but later link it into a dynamic library
    add_compile_options(-fPIC)
endif()
if(NOT WIN32)
    # About -Wno-sign-conversion: With Clang, -Wconversion implies -Wsign-conversion. There are a number of
    # implicit sign conversions in gtest.cc, see https://ci.ros2.org/job/ci_osx/9381/clang/.
    # Hence disabling -Wsign-conversion for now until all those have eventually been fixed.
    # (from https://github.com/ros2/rcutils/pull/263#issuecomment-663252537)
    add_compile_options(-Wall -Wextra -Wconversion -Wno-sign-conversion -Wpedantic)
endif()

# Setup Boost
find_package(Boost REQUIRED COMPONENTS log log_setup thread system filesystem)
include_directories(${Boost_INCLUDE_DIRS})


# ********** Setup umrt_imu_interface_lib library **********
set(lib_target ${PROJECT_NAME})

add_library(${lib_target})

set(public_headers
        include/umrt_geiger_interface/geiger.h
)

set_property(TARGET ${lib_targe} PROPERTY PUBLIC_HEADER ${public_headers})
target_include_directories(${lib_target} PUBLIC 
                      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}>"
                      "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/${PROJECT_NAME}>"
                      "$<INSTALL_INTERFACE:include/>"
)

target_link_libraries(${lib_target} PUBLIC
                 ${Boost_LIBRARIES}
) 

# ********** Setup geiger_dump executable **********

set(geiger_dump_target geiger_dump)
add_executable(${geiger_dump_target})
target_sources(${geiger_dump_target} PRIVATE src/geiger.cpp src/main.cpp)
target_link_libraries(${geiger_dump_target} PRIVATE 
#not sure if there is need to include this here if it is made public in lib_target#        
${Boost_LIBRARIES} 
        ${lib_target}
)


# ********** Setup packaging **********

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

file(READ "version" version_input)
string(REGEX MATCH "([0-9]+)\.([0-9]+)\.([0-9]+)" _ ${version_input})


configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.in.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}COnfig.cmake
    @ONLY
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${version_input}
        COMPATIBILITY AnyNewerVersion
)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
        )

install(
        TARGETS ${lib_target} 
        EXPORT ${PROJECT_NAME}Targets
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
)

install(
        EXPORT ${PROJECT_NAME}Targets
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}
)
message(STATUS "Components to pack: ${CPACK_COMPONENTS_ALL}")
include(cpack.cmake)

