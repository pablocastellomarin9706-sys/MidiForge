cmake_minimum_required(VERSION 3.22)

project(MidiForge
  VERSION 0.1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(JUCE_VERSION "8.0.8")

include(FetchContent)

FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG ${JUCE_VERSION}
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(JUCE)

juce_add_plugin(MidiForge
  COMPANY_NAME "MidiForge"
  PLUGIN_NAME "MidiForge"
  DESCRIPTION "MIDI Generator inspired by Co-Producer - 100% offline"
  VERSION "${CMAKE_PROJECT_VERSION}"
  BUNDLE_ID "com.midiforge.plugin"
  IS_SYNTH TRUE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  EDITOR_WANTS_KEYBOARD_FOCUS FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE
  VST3_CATEGORIES "Instrument"
  PLUGIN_MANUFACTURER_CODE MFGE
  PLUGIN_CODE MFGE
  FORMATS VST3
  PRODUCT_NAME "MidiForge"
)

target_sources(MidiForge
  PRIVATE
    src/PluginProcessor.cpp
    src/PluginProcessor.h
    src/PluginEditor.cpp
    src/PluginEditor.h
    src/Core/Types.h
    src/Core/MusicTheory.cpp
    src/Core/MusicTheory.h
    src/Core/RandomGen.h
    src/Generator/ChordGenerator.cpp
    src/Generator/ChordGenerator.h
    src/Generator/MelodyGenerator.cpp
    src/Generator/MelodyGenerator.h
    src/Generator/BassGenerator.cpp
    src/Generator/BassGenerator.h
    src/Generator/DrumGenerator.cpp
    src/Generator/DrumGenerator.h
    src/Generator/IdeaGenerator.cpp
    src/Generator/IdeaGenerator.h
    src/MIDI/MidiBuilder.cpp
    src/MIDI/MidiBuilder.h
    src/MIDI/MidiDrag.cpp
    src/MIDI/MidiDrag.h
    src/MIDI/MidiUtils.cpp
    src/MIDI/MidiUtils.h
    src/UI/LookAndFeel.cpp
    src/UI/LookAndFeel.h
    src/UI/MainComponent.cpp
    src/UI/MainComponent.h
    src/UI/Components/HeaderBar.cpp
    src/UI/Components/HeaderBar.h
    src/UI/Components/Sidebar.cpp
    src/UI/Components/Sidebar.h
    src/UI/Components/TabView.cpp
    src/UI/Components/TabView.h
    src/UI/Components/ChordStrip.cpp
    src/UI/Components/ChordStrip.h
    src/UI/Components/PianoRollMini.cpp
    src/UI/Components/PianoRollMini.h
    src/UI/Components/DrumGrid.cpp
    src/UI/Components/DrumGrid.h
    src/UI/Components/DragHandle.cpp
    src/UI/Components/DragHandle.h
)

target_compile_definitions(MidiForge
  PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(MidiForge
  PRIVATE
    juce::juce_audio_processors
    juce::juce_gui_extra
    juce::juce_gui_basics
    juce::juce_graphics
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_warning_flags
)

if (MSVC)
  target_compile_options(MidiForge PRIVATE /W4 /permissive-)
endif()
