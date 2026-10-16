#pragma once

#include "Types.h"
#include <vector>

namespace MidiForge::MusicTheory {

std::vector<int> getScale(ScaleType type, int root);
std::vector<int> getChordPitches(int root, const std::vector<int>& intervals, const std::vector<int>& scale);
std::vector<ChordDef> getCommonProgressions();

} // namespace MidiForge::MusicTheory
