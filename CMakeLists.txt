cmake_minimum_required(VERSION 3.12)

project(PPDA VERSION 3.6.0 LANGUAGES C CXX)

set(EXECUTABLE main)
set(CLIENT client)
set(SERVER server)
set(MASK mask)
# Executable will be in ../bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PPDA_SOURCE_DIR}/bin)

include(CheckCCompilerFlag)
function(check_c_compiler_flag_and_add flag result)
  check_c_compiler_flag("${flag}" ${result})
  if(${result})
    add_compile_options("${flag}")
  endif()
endfunction()

check_c_compiler_flag_and_add(-pedantic CC_SUPPORTS_PEDANTIC)
check_c_compiler_flag_and_add(-Wall CC_SUPPORTS_WALL)
check_c_compiler_flag_and_add(-Wextra CC_SUPPORTS_WEXTRA)
check_c_compiler_flag_and_add(-Wshadow CC_SUPPORTS_WSHADOW)
check_c_compiler_flag_and_add(-Werror=vla CC_SUPPORTS_ERRORVLA)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMP_CLIENT_PLAIN ON)

# configure keccak
add_subdirectory(keccak)

add_executable(${EXECUTABLE} main.cpp)
target_sources(${EXECUTABLE}
  PRIVATE
    Client.cpp
    Server.cpp
    uniformDouble.cpp
    shake.cpp
)

add_executable(${CLIENT} client_standalone.cpp)

if(CMP_CLIENT_PLAIN)
  target_sources(${CLIENT}
    PRIVATE
      Client.cpp
      Server.cpp # just for plain compuation
      uniformDouble.cpp
      shake.cpp
  )
  add_definitions("-DCMP_CLIENT_PLAIN")
else()
  target_sources(${CLIENT}
  PRIVATE
    Client.cpp
  )
endif()

add_executable(${SERVER} server_standalone.cpp )
target_sources(${SERVER}
  PRIVATE
    Server.cpp
    uniformDouble.cpp
    shake.cpp
)

add_executable(${MASK} mask_demo.cpp)
target_sources(${MASK}
  PRIVATE
    Client.cpp
    Server.cpp
    uniformDouble.cpp
    shake.cpp
)

# Import Microsoft SEAL
find_package(SEAL 3.6 REQUIRED
        # Optionaly providing a path so this can be built without installing SEAL
        PATHS ${PPDA_SOURCE_DIR}/SEAL/build/cmake
)

target_link_libraries(${EXECUTABLE} SEAL::seal keccak)
if(CMP_CLIENT_PLAIN)
  target_link_libraries(${CLIENT} SEAL::seal keccak)
else()
 target_link_libraries(${CLIENT} SEAL::seal)
endif()
target_link_libraries(${SERVER} SEAL::seal keccak)
target_link_libraries(${MASK} SEAL::seal keccak)

add_custom_target(run
  COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${EXECUTABLE}
  DEPENDS ${EXECUTABLE}
)
