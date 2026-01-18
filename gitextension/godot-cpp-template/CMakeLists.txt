# Using the same minimum as the godot-cpp project
cmake_minimum_required(VERSION 3.17)

# Silence unused variable warning when specified from toolchain
if(CMAKE_C_COMPILER)
endif()

set(LIBNAME "EXTENSION-NAME" CACHE STRING "The name of the library")
set(GODOT_PROJECT_DIR "demo" CACHE STRING "The directory of a Godot project folder")

# Make sure all the dependencies are satisfied
find_package(Python3 3.4 REQUIRED)
find_program(GIT git REQUIRED)

# Ensure godot-cpp submodule has been updated
if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/godot-cpp/src")
    message(NOTICE "godot-cpp bindings source not found")
    message(NOTICE "initializing/updating the godot-cpp submodule...")

    # update the c++ bindings submodule to populate it with the necessary source for the library
    execute_process(
        COMMAND git submodule update --init godot-cpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()
add_subdirectory(godot-cpp SYSTEM)

# Add godot-cpp's module path and include the exported functions.
# This is made available for documentation generation
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${godot-cpp_SOURCE_DIR}/cmake")
include(GodotCPPModule)

# The godot-cpp target has some of useful properties attached that can be retrieved like so.
get_target_property(GODOTCPP_SUFFIX godot::cpp GODOTCPP_SUFFIX)
get_target_property(GODOTCPP_PLATFORM godot::cpp GODOTCPP_PLATFORM)

# Now we can specify our own project which will inherit any global cmake properties or variables that have been defined.
project(godot-cpp-template
    VERSION 1.0
    DESCRIPTION "This repository serves as a quickstart template for GDExtension development with Godot 4.0+."
    HOMEPAGE_URL "https://github.com/enetheru/godot-cpp-template/tree/main"
    LANGUAGES CXX
)

add_library(${LIBNAME} SHARED)

target_sources(${LIBNAME}
    PRIVATE
    src/example_class.cpp
    src/example_class.h
    src/register_types.cpp
    src/register_types.h
    src/remc2/engine/Animation.cpp
    src/remc2/engine/Animation.h
    src/remc2/engine/Basic.cpp
    src/remc2/engine/Basic.h
    src/remc2/engine/CommandLineParser.cpp
    src/remc2/engine/CommandLineParser.h
    src/remc2/engine/Compare.cpp
    src/remc2/engine/Compare.h
    src/remc2/engine/Events.cpp
    src/remc2/engine/Events.h
    src/remc2/engine/GameBitmap.cpp
    src/remc2/engine/GameBitmap.h
    src/remc2/engine/GameBitmapIndexes.h
    src/remc2/engine/GameRenderGL.cpp
    src/remc2/engine/GameRenderGL.h
    src/remc2/engine/GameRenderHD.cpp
    src/remc2/engine/GameRenderHD.h
    src/remc2/engine/GameRenderInterface.h
    src/remc2/engine/GameRenderNG.cpp
    src/remc2/engine/GameRenderNG.h
    src/remc2/engine/GameRenderOriginal.cpp
    src/remc2/engine/GameRenderOriginal.h
    src/remc2/engine/GameUI.cpp
    src/remc2/engine/GameUI.h
    src/remc2/engine/GameUiConstants.h
    src/remc2/engine/global_types.h
    src/remc2/engine/Graphics.cpp
    src/remc2/engine/Graphics.h
    src/remc2/engine/LangTextIndexes.h
    src/remc2/engine/Level.cpp
    src/remc2/engine/Level.h
    src/remc2/engine/LevelStructs.cpp
    src/remc2/engine/LevelStructs.h
    src/remc2/engine/Map.h
    src/remc2/engine/MenusAndIntros.cpp
    src/remc2/engine/MenusAndIntros.h
    src/remc2/engine/Network.cpp
    src/remc2/engine/Network.h
    src/remc2/engine/Player.cpp
    src/remc2/engine/Player.h
    src/remc2/engine/PlayerInput.cpp
    src/remc2/engine/PlayerInput.h
    src/remc2/engine/ProjectionPolygon.cpp
    src/remc2/engine/ProjectionPolygon.h
    src/remc2/engine/RenderThread.cpp
    src/remc2/engine/RenderThread.h
    src/remc2/engine/Sound.cpp
    src/remc2/engine/Sound.h
    src/remc2/engine/Terrain.cpp
    src/remc2/engine/Terrain.h
    src/remc2/engine/TextureMaps.cpp
    src/remc2/engine/TextureMaps.h
    src/remc2/engine/Type_D404C.cpp
    src/remc2/engine/Type_D404C.h
    src/remc2/engine/Type_D93C0_Bldgprmbuffer.cpp
    src/remc2/engine/Type_D93C0_Bldgprmbuffer.h
    src/remc2/engine/Type_D94F0_Bldgprmbuffer.cpp
    src/remc2/engine/Type_D94F0_Bldgprmbuffer.h
    src/remc2/engine/Type_MapScreenPortals_E17CC.cpp
    src/remc2/engine/Type_MapScreenPortals_E17CC.h
    src/remc2/engine/Type_SecretMapScreenPortals_E2970.cpp
    src/remc2/engine/Type_SecretMapScreenPortals_E2970.h
    src/remc2/engine/Type_WORD_D951C.cpp
    src/remc2/engine/Type_WORD_D951C.h
    src/remc2/engine/Unk_D47E0.cpp
    src/remc2/engine/Unk_D47E0.h
    src/remc2/engine/Unk_D4A30.cpp
    src/remc2/engine/Unk_D4A30.h
    src/remc2/engine/ViewPort.cpp
    src/remc2/engine/ViewPort.h
    src/remc2/engine/ail_sound.h
    src/remc2/engine/defs.h
    src/remc2/engine/engine_support.cpp
    src/remc2/engine/engine_support.h
    src/remc2/engine/engine_support_converts.cpp
    src/remc2/engine/engine_support_converts.h
    src/remc2/engine/read_config.cpp
    src/remc2/engine/read_config.h
    src/remc2/engine/sub_main_mouse.h
    src/remc2/sub_main.cpp
    src/remc2/sub_main.h
    src/remc2/utilities/BitmapIO.cpp
    src/remc2/utilities/BitmapIO.h
    src/remc2/utilities/DataFileIO.cpp
    src/remc2/utilities/DataFileIO.h
    src/remc2/utilities/Maths.cpp
    src/remc2/utilities/Maths.h
    src/remc2/utilities/RendererTests.cpp
    src/remc2/utilities/RendererTests.h
    src/remc2/utilities/StateMonitor.cpp
    src/remc2/utilities/StateMonitor.h
    src/remc2/build_config.h
    src/remc2/engine/BasicMini.cpp
    src/remc2/engine/BasicMini.h
    src/remc2/engine/BasicTerrain.cpp
    src/remc2/engine/BasicTerrain.h
    src/remc2/engine/CompareMaps.cpp
    src/remc2/engine/CompareMaps.h
    src/remc2/engine/ConvertMapInfo.cpp
    src/remc2/engine/ConvertMapInfo.h
    src/remc2/engine/DatTabIndexes.cpp
    src/remc2/engine/DatTabIndexes.h
    src/remc2/engine/EventsFunctions.cpp
    src/remc2/engine/EventsFunctions.h
    src/remc2/engine/LevelInit.cpp
    src/remc2/engine/LevelInit.h
    src/remc2/engine/MouseOnOff.cpp
    src/remc2/engine/MouseOnOff.h
    src/remc2/engine/PrintMessages.cpp
    src/remc2/engine/PrintMessages.h
    src/remc2/engine/ReadAndDecompress.cpp
    src/remc2/engine/ReadAndDecompress.h
    src/remc2/engine/ScreenBuff.cpp
    src/remc2/engine/ScreenBuff.h
    src/remc2/engine/Spells.cpp
    src/remc2/engine/Spells.h
    src/remc2/engine/UVTable_D4350.cpp
    src/remc2/engine/UVTable_D4350.h
    src/remc2/utilities/DataFileRNC.cpp
    src/remc2/utilities/DataFileRNC.h
    src/remc2/portability/fcaseopen.cpp
    src/remc2/portability/fcaseopen.h
    src/remc2/portability/port_filesystem.cpp
    src/remc2/portability/port_filesystem.h
    src/remc2/portability/port_time.cpp
    src/remc2/portability/port_time.h
    src/remc2/portability/port_sdl_vga_mouse.cpp
    src/remc2/portability/port_sdl_vga_mouse.h
    src/remc2/portability/port_show_perifery.cpp
    src/remc2/portability/port_show_perifery.h
    src/remc2/portability/port_sdl_joystick.cpp
    src/remc2/portability/port_sdl_joystick.h
    src/remc2/portability/port_net.cpp
    src/remc2/portability/port_net.h
    src/remc2/portability/port_outputs.cpp
    src/remc2/portability/port_outputs.h
    src/remc2/portability/port_sdl_sound.cpp
    src/remc2/portability/port_sdl_sound.h
    src/MBEX/convertData.cpp
    src/MBEX/convertData.h
)

# Fetch a list of the xml files to use for documentation and add to our target
file(GLOB_RECURSE DOC_XML LIST_DIRECTORIES NO CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/doc_classes/*.xml")

# conditionally add doc data to compile output
if(DOC_XML)
    if(GODOTCPP_TARGET MATCHES "editor|template_debug")
        target_doc_sources(${LIBNAME} ${DOC_XML})
    endif()
endif()

target_link_libraries(${LIBNAME} PRIVATE godot-cpp)

# Require at least C++17 for this target
set_property(TARGET ${LIBNAME} PROPERTY CXX_STANDARD 17)

set_target_properties(${LIBNAME}
    PROPERTIES
    # The generator expression here prevents msvc from adding a Debug or Release subdir.
    RUNTIME_OUTPUT_DIRECTORY "$<1:${PROJECT_SOURCE_DIR}/bin/${GODOTCPP_PLATFORM}>"

    PREFIX ""
    OUTPUT_NAME "${LIBNAME}${GODOTCPP_SUFFIX}"
)

set(GODOT_PROJECT_BINARY_DIR "${PROJECT_SOURCE_DIR}/${GODOT_PROJECT_DIR}/bin/${GODOTCPP_PLATFORM}")

add_custom_command(TARGET ${LIBNAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${LIBNAME}>" "${GODOT_PROJECT_BINARY_DIR}/$<TARGET_FILE_NAME:${LIBNAME}>"
)
