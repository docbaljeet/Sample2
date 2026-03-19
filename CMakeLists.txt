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
    src_tls/bindings.cpp
)

target_include_directories(ActuarialModelTLS PRIVATE src_tls)
target_link_libraries(ActuarialModelTLS PRIVATE pybind11::embed)

# Copy per-script Python files next to the TLS executable.
foreach(_script mortality_script lapse_script eia_credit_script)
    add_custom_command(TARGET ActuarialModelTLS POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/scripts/${_script}.py"
            "$<TARGET_FILE_DIR:ActuarialModelTLS>/${_script}.py"
        COMMENT "Copying ${_script}.py for ActuarialModelTLS"
    )
endforeach()

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

# ── Fourth executable: vectorized TLS approach (scripts called once) ──
add_executable(ActuarialModelTLSVec
    src_tls_vec/main.cpp
    src_tls_vec/projection_engine.cpp
    src_tls_vec/bindings.cpp
)

target_include_directories(ActuarialModelTLSVec PRIVATE src_tls_vec)
target_link_libraries(ActuarialModelTLSVec PRIVATE pybind11::embed)

# Copy vectorized Python scripts next to the executable.
foreach(_script mortality_script_vec lapse_script_vec eia_credit_script_vec)
    add_custom_command(TARGET ActuarialModelTLSVec POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/scripts/${_script}.py"
            "$<TARGET_FILE_DIR:ActuarialModelTLSVec>/${_script}.py"
        COMMENT "Copying ${_script}.py for ActuarialModelTLSVec"
    )
endforeach()

# ── Fifth executable: generic data-driven bindings ────────────────────
add_executable(ActuarialModelGeneric
    src_generic/main.cpp
    src_generic/projection_engine.cpp
    src_generic/bindings.cpp
)

target_include_directories(ActuarialModelGeneric PRIVATE src_generic)
target_link_libraries(ActuarialModelGeneric PRIVATE pybind11::embed)

# Reuses the same vectorized Python scripts — the API is identical.
foreach(_script mortality_script_vec lapse_script_vec eia_credit_script_vec)
    add_custom_command(TARGET ActuarialModelGeneric POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/scripts/${_script}.py"
            "$<TARGET_FILE_DIR:ActuarialModelGeneric>/${_script}.py"
        COMMENT "Copying ${_script}.py for ActuarialModelGeneric"
    )
endforeach()

# ── Sixth executable: code-generated bindings from registry.json ──────
# Step 1: Run codegen.py whenever registry.json changes.
set(_CODEGEN_DIR "${CMAKE_SOURCE_DIR}/src_codegen")
set(_GENERATED_DIR "${_CODEGEN_DIR}/generated")
add_custom_command(
    OUTPUT
        "${_GENERATED_DIR}/script_contexts.h"
        "${_GENERATED_DIR}/runtime_context.h"
        "${_GENERATED_DIR}/bindings.cpp"
    COMMAND ${Python3_EXECUTABLE}
        "${_CODEGEN_DIR}/codegen.py"
        --registry "${_CODEGEN_DIR}/registry.json"
        --outdir   "${_GENERATED_DIR}"
    DEPENDS
        "${_CODEGEN_DIR}/registry.json"
        "${_CODEGEN_DIR}/codegen.py"
    COMMENT "Generating C++ from registry.json"
)

# Step 2: Build the executable using the generated files.
add_executable(ActuarialModelCodegen
    src_codegen/main.cpp
    src_codegen/projection_engine.cpp
    "${_GENERATED_DIR}/bindings.cpp"
)

target_include_directories(ActuarialModelCodegen PRIVATE
    src_codegen
    "${_GENERATED_DIR}"
)
target_link_libraries(ActuarialModelCodegen PRIVATE pybind11::embed)

# Reuses the same vectorized Python scripts.
foreach(_script mortality_script_vec lapse_script_vec eia_credit_script_vec)
    add_custom_command(TARGET ActuarialModelCodegen POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/scripts/${_script}.py"
            "$<TARGET_FILE_DIR:ActuarialModelCodegen>/${_script}.py"
        COMMENT "Copying ${_script}.py for ActuarialModelCodegen"
    )
endforeach()

# ── Seventh executable: dict marshalling (no embedded module) ─────────
add_executable(ActuarialModelDict
    src_dict/main.cpp
    src_dict/projection_engine.cpp
)

target_include_directories(ActuarialModelDict PRIVATE src_dict)
target_link_libraries(ActuarialModelDict PRIVATE pybind11::embed)

foreach(_script mortality_script_dict lapse_script_dict eia_credit_script_dict restricted_dict)
    add_custom_command(TARGET ActuarialModelDict POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/scripts/${_script}.py"
            "$<TARGET_FILE_DIR:ActuarialModelDict>/${_script}.py"
        COMMENT "Copying ${_script}.py for ActuarialModelDict"
    )
endforeach()

# ── Eighth executable: struct passing via pybind11 class ──────────────
add_executable(ActuarialModelStruct
    src_struct/main.cpp
    src_struct/projection_engine.cpp
    src_struct/bindings.cpp
)

target_include_directories(ActuarialModelStruct PRIVATE src_struct)
target_link_libraries(ActuarialModelStruct PRIVATE pybind11::embed)

# Pass the Python installation directory so the executable can set
# PYTHONHOME at runtime — required for embedded Python on Windows/pyenv.
get_filename_component(_py3_home "${Python3_EXECUTABLE}" DIRECTORY)
target_compile_definitions(ActuarialModelStruct PRIVATE
    PYTHON_HOME_PATH="${_py3_home}")

foreach(_script mortality_script_struct lapse_script_struct eia_credit_script_struct assumptions actuarial)
    add_custom_command(TARGET ActuarialModelStruct POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/scripts/${_script}.py"
            "$<TARGET_FILE_DIR:ActuarialModelStruct>/${_script}.py"
        COMMENT "Copying ${_script}.py for ActuarialModelStruct"
    )
endforeach()

add_custom_command(TARGET ActuarialModelStruct POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/scripts/hooks_config.json"
        "$<TARGET_FILE_DIR:ActuarialModelStruct>/hooks_config.json"
    COMMENT "Copying hooks_config.json for ActuarialModelStruct"
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
    foreach(_target ActuarialModel ActuarialModelTLS ActuarialModelLambda ActuarialModelTLSVec ActuarialModelGeneric ActuarialModelCodegen ActuarialModelDict ActuarialModelStruct)
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
