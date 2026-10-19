#pragma once

#include <vector>

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_core/juce_core.h>

#include "../Core/Types.h"
#include "SampleLibrary.h"

namespace MidiForge {

// One sound per layer of the idea. The Auto-Sound engine picks them from the
// user's watched folders (and, optionally, from Freesound) so every generated
// MIDI arrives with a matching sound.
enum class SoundSlot {
  Melody = 0,
  Chords,
  Bass,
  Kick,
  Snare,
  Clap,
  Hat,
  OpenHat,
  Crash,
  NumSlots
};

inline constexpr int kNumSoundSlots = (int)SoundSlot::NumSlots;

// Row / cache label for a slot ("Melodia", "Kick"...). ASCII only.
juce::String soundSlotName(SoundSlot s);

// GM drum pitch -> slot (36 kick, 38/40 snare, 39/37 clap, 42/44 closed hat,
// 46 open hat, 49/57 crash).
SoundSlot slotForDrumPitch(int pitch);

struct AutoSoundSet {
  juce::String paths[kNumSoundSlots];
  juce::String names[kNumSoundSlots];

  bool any() const {
    for (const auto& p : paths)
      if (p.isNotEmpty()) return true;
    return false;
  }
  bool has(SoundSlot s) const { return paths[(int)s].isNotEmpty(); }
  void clear() {
    for (auto& p : paths) p = {};
    for (auto& n : names) n = {};
  }
};

// Picks one sound per slot. Deterministic for a given seed. For the melodic
// slots (melody/chords/bass) files whose detected key matches the idea key are
// preferred when available (harmonic pick).
AutoSoundSet pickAutoSounds(const std::vector<SampleItem>& library, Character character,
                            int keyRoot, int seed);

// Re-picks a single slot, avoiding the current file when possible.
bool pickOneSlot(AutoSoundSet& set, const std::vector<SampleItem>& library,
                 Character character, int keyRoot, SoundSlot slot, int seed,
                 const juce::String& avoidPath);

// Renders one element (mixAll = full mix) to a 44.1 kHz stereo WAV using the
// assigned samples, pitch-aligned to the idea key. Returns the written file
// (a null File when nothing could be rendered).
juce::File renderIdeaToWav(const SessionIdea& idea, const juce::String& trackName,
                           const AutoSoundSet& sounds, double bpm, bool mixAll);

// --- Freesound (optional, requires the user's free API key) -----------------

// Default search query for a slot. Empty = that slot never goes online
// (drums are always local).
juce::String freesoundQueryFor(SoundSlot slot, Character character);

// Searches Freesound (CC0 only) and downloads the best preview into cacheDir.
// Returns the downloaded file; status always receives a short ASCII message.
juce::File fetchFreesoundSound(const juce::String& apiKey, SoundSlot slot,
                               Character character, const juce::File& cacheDir,
                               juce::String& status);

} // namespace MidiForge
