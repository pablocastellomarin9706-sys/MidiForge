#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../PluginProcessor.h"
#include "../../Chat/ChatEngine.h"

namespace MidiForge {

// Right-hand chat panel (Co-Producer style, but fully offline):
// log + input + quick chips + Sample Scout controls.
class ChatPanel : public juce::Component {
public:
  explicit ChatPanel(MidiForgeAudioProcessor& p);
  void paint(juce::Graphics&) override;
  void resized() override;

  std::function<void()> onIdeaChanged;

private:
  void send(const juce::String& text);
  void append(const juce::String& who, const juce::String& text);
  void updateOpenSearch();

  MidiForgeAudioProcessor& processor;
  ChatEngine engine;
  juce::Label titleLabel;
  juce::TextEditor log;
  juce::TextEditor input;
  juce::TextButton sendButton{"Enviar"};
  juce::TextButton openSearchButton{"Abrir busqueda"};
  juce::ComboBox providerBox;
  juce::Label providerLabel;
  juce::ComboBox quickBox;
  juce::Label quickLabel;
  juce::String lastQuery;
  std::vector<std::unique_ptr<juce::TextButton>> chips;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChatPanel)
};

} // namespace MidiForge
