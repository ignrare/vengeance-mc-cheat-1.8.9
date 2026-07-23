cmake_minimum_required(VERSION 3.10)
project(MinecraftCheat)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Esp)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GlDraw)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/GlText)

# ImGui sources
set(IMGUI_SOURCES
    imgui.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
)

set(IMGUI_HEADERS
    imgui.h
    imgui_internal.h
    imgui_draw.h
    imgui_tables.h
    imgui_widgets.h
    imstb_rectpack.h
    imstb_textedit.h
    imconfig.h
)

# ImGui backend sources
set(IMGUI_BACKEND_SOURCES
    imgui_impl_win32.cpp
    imgui_impl_opengl2.cpp
)

set(IMGUI_BACKEND_HEADERS
    imgui_impl_win32.h
    imgui_impl_opengl2.h
)

# Core sources
set(CORE_SOURCES
    Core/Core.cpp
)

set(CORE_HEADERS
    Core/Core.h
)

# Individual Module Files
set(COMBAT_SOURCES
    "Modules/Combat/Aimbot.cpp"
    "Modules/Combat/AutoClicker.cpp"
    "Modules/Combat/Blockhit.cpp"
)

set(VISUALS_SOURCES
    "Modules/Visuals/ESP.cpp"
    "Modules/Visuals/Tracer.cpp"
    "Modules/Visuals/XRay.cpp"
    "Modules/Visuals/ChestESP.cpp"
)

set(MODULE_HEADERS
    Modules/Combat/Aimbot.h
    Modules/Combat/AutoClicker.h
    Modules/Combat/Blockhit.h
    Modules/Visuals/ESP.h
    Modules/Visuals/Tracer.h
    Modules/Visuals/XRay.h
    Modules/Visuals/ChestESP.h
)

# UI sources
set(UI_SOURCES
    UI/UI.cpp
    UI/ModernMenu.cpp
)

set(UI_HEADERS
    UI/UI.h
)

# Esp sources
set(ESP_SOURCES
    Esp/Position.cpp
    Esp/Radius.cpp
    Esp/Color.cpp
)

set(ESP_HEADERS
    Esp/Position.h
    Esp/Radius.h
    Esp/Color.h
)

# GlDraw sources
set(GLDRAW_SOURCES
    GlDraw/glDraw.cpp
)

set(GLDRAW_HEADERS
    GlDraw/glDraw.h
)

# Main source
set(MAIN_SOURCES
    main_new.cpp
)

# Collect all sources
set(ALL_SOURCES
    ${MAIN_SOURCES}
    ${CORE_SOURCES}
    ${COMBAT_SOURCES}
    ${VISUALS_SOURCES}
    ${UI_SOURCES}
    ${ESP_SOURCES}
    ${GLDRAW_SOURCES}
    ${IMGUI_SOURCES}
    ${IMGUI_BACKEND_SOURCES}
)

# Collect all headers
set(ALL_HEADERS
    ${CORE_HEADERS}
    ${MODULE_HEADERS}
    ${UI_HEADERS}
    ${ESP_HEADERS}
    ${GLDRAW_HEADERS}
    ${IMGUI_HEADERS}
    ${IMGUI_BACKEND_HEADERS}
    includes.h
)

# Create executable
add_executable(${PROJECT_NAME} ${ALL_SOURCES} ${ALL_HEADERS})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    opengl32
    gdi32
    user32
    kernel32
)

# Windows specific
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        _CRT_SECURE_NO_WARNINGS
        WIN32_LEAN_AND_MEAN
    )
endif()

# Compiler-specific options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W3
        /MP
    )
endif()

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
