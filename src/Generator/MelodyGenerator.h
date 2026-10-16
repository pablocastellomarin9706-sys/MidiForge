#pragma once

#include "../Core/Types.h"
#include "../Core/RandomGen.h"

namespace MidiForge {

class MelodyGenerator {
public:
  void generate(SessionIdea& idea, RandomGen& rng);
};

} // namespace MidiForge
