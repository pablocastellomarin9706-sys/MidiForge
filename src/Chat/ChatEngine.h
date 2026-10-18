#pragma once

#include <juce_core/juce_core.h>
#include "../PluginProcessor.h"

namespace MidiForge {

// Result of one chat message handled by the offline engine.
struct ChatReply {
  juce::String text;         // assistant message shown in the chat log
  juce::String searchQuery;  // non-empty => Sample Scout search was prepared
  bool ideaChanged{false};   // UI should refresh after this message
};

// Offline natural-language-ish command engine. No network, no cloud:
// keyword + intent parsing in Spanish/English that drives the curated
// generator the same way the header controls do.
class ChatEngine {
public:
  explicit ChatEngine(MidiForgeAudioProcessor& processorToUse);

  ChatReply handle(const juce::String& userText);

  // Builds a sample search URL. provider: 1=Splice, 2=Freesound, 3=Samplette, 4=Loopcloud.
  static juce::String buildSearchUrl(const juce::String& query, int provider);

private:
  MidiForgeAudioProcessor& processor;
};

} // namespace MidiForge
