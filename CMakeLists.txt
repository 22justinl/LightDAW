cmake_minimum_required(VERSION 3.22)
set(CMAKE_EXPORT_COMPILE_COMMANDS yes)

project(LightDAW
    VERSION 0.0.1
)

add_subdirectory(external/JUCE)
add_subdirectory(external/doctest)


juce_add_gui_app(LightDAW
    PRODUCT_NAME "LightDAW"
)
set_target_properties(LightDAW PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
target_sources(LightDAW
    PRIVATE
        src/main.cpp
        src/MainComponent.cpp

        src/audio/AudioEngine.cpp
        src/audio/DAWIOCallback.cpp
        src/audio/Track.cpp

        src/gui/AudioSettingsComponent.cpp
        src/gui/TrackComponent.cpp
        src/gui/TrackListComponent.cpp
)
target_link_libraries(LightDAW
    PRIVATE
        juce::juce_gui_extra
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
target_include_directories(LightDAW
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_definitions(LightDAW
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:LightDAW,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:LightDAW,JUCE_VERSION>"
)


add_executable(tests)
target_sources(tests
    PRIVATE
        tests/test.cpp
)
target_link_libraries(tests
    PRIVATE
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
target_include_directories(tests
    PRIVATE
        external/doctest
)
target_compile_definitions(tests
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:LightDAW,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:LightDAW,JUCE_VERSION>"
)
