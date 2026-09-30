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
        src/audio/AudioFileStream.cpp
        src/audio/AudioRingBuffer.cpp
        src/audio/DAWIOCallback.cpp
        src/audio/PlaybackSource.cpp
        src/audio/StreamManager.cpp
        src/audio/Track.cpp
        src/audio/TrackAudioSource.cpp
        src/audio/Transport.cpp

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


add_executable(unit_tests)
target_sources(unit_tests
    PRIVATE
        tests/unit/test_main.cpp
        tests/unit/test.cpp
        tests/unit/test_SPSCQueue.cpp
        tests/unit/test_AudioFileStream.cpp

        src/audio/AudioEngine.cpp
        src/audio/AudioFileStream.cpp
        src/audio/AudioRingBuffer.cpp
        src/audio/DAWIOCallback.cpp
        src/audio/PlaybackSource.cpp
        src/audio/StreamManager.cpp
        src/audio/Track.cpp
        src/audio/TrackAudioSource.cpp
        src/audio/Transport.cpp
)
target_include_directories(unit_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(unit_tests
    PRIVATE
        juce::juce_gui_extra
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
target_include_directories(unit_tests
    PRIVATE
        external/doctest
)
target_compile_definitions(unit_tests
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:LightDAW,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:LightDAW,JUCE_VERSION>"
)


add_executable(streaming_test)
set_target_properties(streaming_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
target_sources(streaming_test
    PRIVATE
        tests/streaming/main.cpp

        src/audio/AudioEngine.cpp
        src/audio/AudioFileStream.cpp
        src/audio/AudioRingBuffer.cpp
        src/audio/DAWIOCallback.cpp
        src/audio/PlaybackSource.cpp
        src/audio/StreamManager.cpp
        src/audio/Track.cpp
        src/audio/TrackAudioSource.cpp
        src/audio/Transport.cpp
)
target_link_libraries(streaming_test
    PRIVATE
        juce::juce_gui_extra
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
target_include_directories(streaming_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_compile_definitions(streaming_test
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:LightDAW,JUCE_PRODUCT_NAME>"
        JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:LightDAW,JUCE_VERSION>"
)
