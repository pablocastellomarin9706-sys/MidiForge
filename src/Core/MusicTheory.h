#pragma once

#include "Types.h"
#include <vector>

namespace MidiForge::MusicTheory {

std::vector<int> getScale(ScaleType type, int root);
std::vector<int> getChordPitches(int root, const std::vector<int>& intervals, const std::vector<int>& scale);
std::vector<ChordDef> getCommonProgressions();

// ---- Fase 2: curated harmonic content ----
int degreeToSemitone(ScaleType scale, int degree); // degree 1-7
std::vector<int> triadIntervals(ScaleType scale, int degree,
                                const juce::String& qualityOverride, bool seventh);
bool isMinorFamily(ScaleType type);
juce::String pitchClassName(int midi);
juce::String chordSymbol(int rootMidi, const std::vector<int>& intervals);
// Voice a chord (inversions + range) close to prevPitches for smooth voice leading.
std::vector<int> realizeVoicing(int rootMidi, const std::vector<int>& intervals,
                                const std::vector<int>& prevPitches);

} // namespace MidiForge::MusicTheory
