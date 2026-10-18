#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../PluginProcessor.h"
#include "../../Samples/SampleLibrary.h"

namespace MidiForge {

// ListBox that starts an external OS file drag when a row is dragged out.
class SampleDragList : public juce::ListBox {
public:
  SampleDragList(const juce::String& name, juce::ListBoxModel* model)
    : juce::ListBox(name, model) {}

  std::function<void(int)> onDragRow;

  void mouseDrag(const juce::MouseEvent& e) override {
    if (onDragRow) {
      auto row = getRowContainingPosition(e.getMouseDownPosition());
      if (row >= 0) onDragRow(row);
    }
  }
};

// Local sample browser: indexes the user's folders (background thread),
// filters by text/BPM/key and drags the audio file to FL Studio.
class SampleBrowser : public juce::Component, private juce::ListBoxModel {
public:
  explicit SampleBrowser(MidiForgeAudioProcessor& p);
  ~SampleBrowser() override;

  void paint(juce::Graphics&) override;
  void resized() override;

private:
  int getNumRows() override;
  void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected) override;

  void rebuildFilter();
  void updateStatus();
  void addFolder();

  MidiForgeAudioProcessor& processor;
  juce::Label statusLabel;
  juce::TextEditor searchBox;
  juce::ComboBox bpmBox;
  juce::ComboBox keyBox;
  juce::TextButton addFolderButton{"Anadir carpeta"};
  juce::TextButton rescanButton{"Reindexar"};
  SampleDragList list;
  std::unique_ptr<juce::FileChooser> chooser;

  std::vector<SampleItem> cache;
  std::vector<int> visible;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleBrowser)
};

} // namespace MidiForge
