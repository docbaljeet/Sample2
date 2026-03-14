cmake_minimum_required(VERSION 3.20)
project(ActuarialModel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Find Python (must come before pybind11) ──────────────────────────
# Tell pybind11 to use CMake's FindPython instead of its own script,
# which avoids issues with pyenv shims on Windows.
set(PYBIND11_FINDPYTHON ON)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# ── Fetch pybind11 ───────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG        v2.13.6
)
FetchContent_MakeAvailable(pybind11)

# ── Main executable ──────────────────────────────────────────────────
add_executable(ActuarialModel
    src/main.cpp
    src/projection_engine.cpp
    src/actuarial_functions.cpp
    src/bindings.cpp
)

target_include_directories(ActuarialModel PRIVATE src)
target_link_libraries(ActuarialModel PRIVATE pybind11::embed)

# ── Post-build: copy user script and Python DLL next to executable ───
add_custom_command(TARGET ActuarialModel POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/scripts/user_script.py"
        "$<TARGET_FILE_DIR:ActuarialModel>/user_script.py"
    COMMENT "Copying user_script.py to output directory"
)

# ── Second executable: thread_local approach (no singleton) ──────────
add_executable(ActuarialModelTLS
    src_tls/main.cpp
    src_tls/projection_engine.cpp
    src/actuarial_functions.cpp
    src_tls/bindings.cpp
)

# src_tls for its own headers, src for shared headers
target_include_directories(ActuarialModelTLS PRIVATE src_tls src)
target_link_libraries(ActuarialModelTLS PRIVATE pybind11::embed)

add_custom_command(TARGET ActuarialModelTLS POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/scripts/user_script.py"
        "$<TARGET_FILE_DIR:ActuarialModelTLS>/user_script.py"
    COMMENT "Copying user_script.py to TLS output directory"
)

# ── Third executable: lambda-capture approach (no globals at all) ────
add_executable(ActuarialModelLambda
    src_lambda/main.cpp
    src_lambda/projection_engine.cpp
    src_lambda/register_module.cpp
    src/actuarial_functions.cpp
)

target_include_directories(ActuarialModelLambda PRIVATE src_lambda src)
target_link_libraries(ActuarialModelLambda PRIVATE pybind11::embed)

add_custom_command(TARGET ActuarialModelLambda POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/scripts/user_script.py"
        "$<TARGET_FILE_DIR:ActuarialModelLambda>/user_script.py"
    COMMENT "Copying user_script.py to Lambda output directory"
)

# Copy python3XX.dll so the executable can find it without PATH tricks.
get_target_property(_py3_loc Python3::Python IMPORTED_LOCATION_RELEASE)
if(NOT _py3_loc)
    get_target_property(_py3_loc Python3::Python IMPORTED_LOCATION)
endif()
if(_py3_loc)
    # The .lib is the import library; the DLL is next to the interpreter.
    get_filename_component(_py3_dir "${Python3_EXECUTABLE}" DIRECTORY)
    file(GLOB _py3_dlls "${_py3_dir}/python3*.dll")
    foreach(_target ActuarialModel ActuarialModelTLS ActuarialModelLambda)
        foreach(_dll ${_py3_dlls})
            add_custom_command(TARGET ${_target} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${_dll}"
                    "$<TARGET_FILE_DIR:${_target}>/"
                COMMENT "Copying ${_dll} for ${_target}"
            )
        endforeach()
    endforeach()
endif()
