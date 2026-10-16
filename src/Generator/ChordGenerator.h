#pragma once

#include "../Core/Types.h"
#include "../Core/RandomGen.h"

namespace MidiForge {

class ChordGenerator {
public:
  ChordGenerator() = default;
  void generate(SessionIdea& idea, RandomGen& rng);
};

} // namespace MidiForge
