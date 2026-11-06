#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../PluginProcessor.h"

namespace MidiForge {

// A/B arrangement editor. The bars of the idea are drawn as a strip of
// blocks: click (or drag across) a block to make section B start there.
// AUTO restores the halfway split for 8+ bar ideas, TODO A removes section B.
// Moving the boundary replays the generators with the same curated picks, so
// only the arrangement changes, never the theme of the idea.
class ArrangementPanel : public juce::Component,
                         public juce::SettableTooltipClient {
public:
  explicit ArrangementPanel(MidiForgeAudioProcessor& p);
  void paint(juce::Graphics&) override;
  void resized() override;
  void refresh();

  // Called after a boundary change so the rest of the UI (piano roll marks,
  // drum grid tabs, chord strip) can repaint with the new split.
  std::function<void()> onApplied;

private:
  void applySplit(int split);
  juce::Rectangle<float> stripArea() const;
  juce::Rectangle<float> blockRect(int i, int total) const;
  int barAt(juce::Point<float> pos) const;

  void mouseDown(const juce::MouseEvent&) override;
  void mouseDrag(const juce::MouseEvent&) override;
  void mouseUp(const juce::MouseEvent&) override;

  MidiForgeAudioProcessor& processor;
  juce::TextButton autoButton{"AUTO"};
  juce::TextButton allAButton{"TODO A"};
  int draggingTo{-999}; // pending split while dragging (applied live)

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ArrangementPanel)
};

} // namespace MidiForge
