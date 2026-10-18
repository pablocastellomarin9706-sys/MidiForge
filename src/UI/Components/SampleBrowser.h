#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../PluginProcessor.h"
#include "../../Samples/SampleLibrary.h"

namespace MidiForge {

// ListBox that starts an external OS file drag when a row is dragged out,
// and forwards row clicks (star zone / popup menu / double click).
class SampleDragList : public juce::ListBox {
public:
  SampleDragList(const juce::String& name, juce::ListBoxModel* model)
    : juce::ListBox(name, model) {}

  std::function<void(int)> onDragRow;
  std::function<void(int, const juce::MouseEvent&)> onRowClicked;
  std::function<void(int)> onRowDoubleClicked;

  void mouseDown(const juce::MouseEvent& e) override {
    juce::ListBox::mouseDown(e);
    auto row = getRowContainingPosition(e.x, e.y);
    if (onRowClicked) onRowClicked(row, e);
  }

  void mouseDoubleClick(const juce::MouseEvent& e) override {
    juce::ListBox::mouseDoubleClick(e);
    auto row = getRowContainingPosition(e.x, e.y);
    if (onRowDoubleClicked) onRowDoubleClicked(row);
  }

  void mouseDrag(const juce::MouseEvent& e) override {
    if (onDragRow) {
      auto p = e.getMouseDownPosition();
      auto row = getRowContainingPosition(p.x, p.y);
      if (row >= 0) onDragRow(row);
    }
  }
};

// Local sample browser: indexes the user's folders (background thread),
// filters by text/BPM/key, fits to the host tempo ("FIT"), favourites,
// find-similar, audition (double click) and drags files to FL Studio.
class SampleBrowser : public juce::Component, private juce::ListBoxModel, private juce::Timer {
public:
  explicit SampleBrowser(MidiForgeAudioProcessor& p);
  ~SampleBrowser() override;

  void paint(juce::Graphics&) override;
  void resized() override;

private:
  int getNumRows() override;
  void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected) override;
  void timerCallback() override;

  void rebuildFilter();
  void updateStatus();
  void addFolder();
  void handleRowClick(int row, const juce::MouseEvent& e);
  void handleRowDoubleClick(int row);
  void showRowMenu(int cacheIndex);
  bool isFit(const SampleItem& it) const;

  MidiForgeAudioProcessor& processor;
  juce::Label statusLabel;
  juce::TextEditor searchBox;
  juce::ComboBox bpmBox;
  juce::ComboBox keyBox;
  juce::TextButton addFolderButton{"Anadir carpeta"};
  juce::TextButton rescanButton{"Reindexar"};
  juce::TextButton fitButton{"FIT"};
  juce::TextButton favButton{"Favoritos"};
  SampleDragList list;
  std::unique_ptr<juce::FileChooser> chooser;

  std::vector<SampleItem> cache;
  std::vector<int> visible;
  bool fitMode{false};
  bool favMode{false};
  int similarAnchor{-1};     // cache index of the "find similar" anchor (-1 = off)
  int previewCacheIndex{-1}; // cache index currently auditioning (-1 = none)
  double lastHostBpm{0.0};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleBrowser)
};

} // namespace MidiForge
