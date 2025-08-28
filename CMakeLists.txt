cmake_minimum_required(VERSION 3.10)

project(ContactForm VERSION 1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(contact_form main.c)

# Platform-specific crap
if(WIN32)
    # Windows specific settings
    target_compile_definitions(contact_form PRIVATE _WIN32)

    if(MSVC)
        set_target_properties(contact_form PROPERTIES
                WIN32_EXECUTABLE FALSE
        )
    endif()

elseif(UNIX AND NOT APPLE)
    # Linux specific settings
    target_compile_definitions(contact_form PRIVATE __linux__)

    # Link math library if needed
    target_link_libraries(contact_form m)

elseif(APPLE)
    # macOS specific settings
    target_compile_definitions(contact_form PRIVATE __APPLE__)
endif()

# Compiler-specific flags
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(contact_form PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -O2
    )
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(contact_form PRIVATE
            /W3
            /O2
    )
endif()

set_target_properties(contact_form PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

install(TARGETS contact_form
        RUNTIME DESTINATION bin
)

message(STATUS "Building Contact Form Application")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Target Platform: ${CMAKE_SYSTEM_NAME}")

if(WIN32)
    message(STATUS "Windows build")
else()
    message(STATUS "Unix")
endif()