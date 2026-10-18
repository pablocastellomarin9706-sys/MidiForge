#include "SampleBrowser.h"
#include "../LookAndFeel.h"

namespace MidiForge {

SampleBrowser::SampleBrowser(MidiForgeAudioProcessor& p)
  : processor(p), list("samples", this) {
  statusLabel.setFont(juce::Font(juce::FontOptions(12.0f)));
  statusLabel.setColour(juce::Label::textColourId, Palette::textSecondary);
  addAndMakeVisible(statusLabel);

  searchBox.setTextToShowWhenEmpty("Buscar por nombre, carpeta, key...", Palette::textSecondary);
  searchBox.setFont(juce::Font(juce::FontOptions(13.0f)));
  searchBox.setColour(juce::TextEditor::backgroundColourId, Palette::panelAlt.darker(0.25f));
  searchBox.setColour(juce::TextEditor::textColourId, Palette::textPrimary);
  searchBox.setColour(juce::TextEditor::outlineColourId, Palette::border);
  searchBox.setColour(juce::TextEditor::focusedOutlineColourId, Palette::accent);
  searchBox.setColour(juce::TextEditor::shadowColourId, juce::Colours::transparentBlack);
  searchBox.onTextChange = [this] { rebuildFilter(); };
  addAndMakeVisible(searchBox);

  bpmBox.addItem("BPM: all", 1);
  bpmBox.addItem("60-90", 2);
  bpmBox.addItem("90-110", 3);
  bpmBox.addItem("110-130", 4);
  bpmBox.addItem("130-160", 5);
  bpmBox.addItem("160+", 6);
  bpmBox.setSelectedId(1, juce::dontSendNotification);
  bpmBox.onChange = [this] { rebuildFilter(); };
  addAndMakeVisible(bpmBox);

  keyBox.addItem("Key: all", 1);
  static const char* keyNames[] = {"C", "C#", "D", "D#", "E", "F",
                                   "F#", "G", "G#", "A", "A#", "B"};
  for (int i = 0; i < 12; ++i) keyBox.addItem(keyNames[i], i + 2);
  keyBox.setSelectedId(1, juce::dontSendNotification);
  keyBox.onChange = [this] { rebuildFilter(); };
  addAndMakeVisible(keyBox);

  addFolderButton.setColour(juce::TextButton::buttonColourId, Palette::card);
  addFolderButton.onClick = [this] { addFolder(); };
  addAndMakeVisible(addFolderButton);

  rescanButton.setColour(juce::TextButton::buttonColourId, Palette::card);
  rescanButton.onClick = [this] { processor.getSampleLibrary().rescan(); };
  addAndMakeVisible(rescanButton);

  list.setRowHeight(26);
  list.setColour(juce::ListBox::backgroundColourId, Palette::panelAlt.darker(0.15f));
  list.setColour(juce::ListBox::outlineColourId, Palette::border);
  list.setOutlineThickness(1);
  addAndMakeVisible(list);
  list.onDragRow = [this](int row) {
    if (row < 0 || row >= (int)visible.size()) return;
    const auto& it = cache[(size_t)visible[(size_t)row]];
    juce::File f(it.path);
    if (f.existsAsFile())
      juce::DragAndDropContainer::performExternalDragDropOfFiles({f.getFullPathName()}, false, this);
  };

  auto& lib = processor.getSampleLibrary();
  lib.onScanChanged = [this] { rebuildFilter(); };

  if (lib.getFolders().isEmpty()) {
    juce::StringArray def;
    def.add(juce::File::getSpecialLocation(juce::File::userMusicDirectory).getFullPathName());
    lib.setFolders(def);
  } else {
    lib.rescan();
  }

  rebuildFilter();
}

SampleBrowser::~SampleBrowser() {
  processor.getSampleLibrary().onScanChanged = nullptr;
}

void SampleBrowser::paint(juce::Graphics& g) {
  g.fillAll(Palette::panel);
}

void SampleBrowser::resized() {
  auto r = getLocalBounds().reduced(12);
  statusLabel.setBounds(r.removeFromTop(16));
  r.removeFromTop(6);

  auto top = r.removeFromTop(26);

  addFolderButton.setBounds(top.removeFromLeft(110));
  top.removeFromLeft(6);
  rescanButton.setBounds(top.removeFromLeft(90));
  top.removeFromLeft(6);
  bpmBox.setBounds(top.removeFromRight(110));
  top.removeFromRight(6);
  keyBox.setBounds(top.removeFromRight(110));
  top.removeFromRight(6);
  searchBox.setBounds(top);

  r.removeFromTop(8);
  list.setBounds(r);
}

int SampleBrowser::getNumRows() {
  return (int)visible.size();
}

void SampleBrowser::paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected) {
  if (row < 0 || row >= (int)visible.size()) return;
  const auto& it = cache[(size_t)visible[(size_t)row]];

  if (selected) {
    g.setColour(Palette::accent.withAlpha(0.25f));
    g.fillRect(0, 0, width, height);
  } else if (row % 2 == 1) {
    g.setColour(juce::Colours::white.withAlpha(0.02f));
    g.fillRect(0, 0, width, height);
  }

  auto r = juce::Rectangle<int>(0, 0, width, height).reduced(8, 0);

  g.setColour(Palette::textPrimary);
  g.setFont(juce::Font(juce::FontOptions(12.5f)));
  auto right = r.removeFromRight(juce::jmin(280, width / 2));
  g.drawText(it.name, r, juce::Justification::centredLeft, true);

  juce::String meta = it.folder;
  if (it.bpm > 0) meta += "  " + juce::String(it.bpm) + " BPM";
  if (it.key.isNotEmpty()) meta += "  " + it.key;
  g.setColour(Palette::textSecondary);
  g.setFont(juce::Font(juce::FontOptions(11.0f)));
  g.drawText(meta, right, juce::Justification::centredRight, true);
}

void SampleBrowser::rebuildFilter() {
  cache = processor.getSampleLibrary().snapshot();

  const auto q = searchBox.getText().trim().toLowerCase();
  const int bpmId = bpmBox.getSelectedId();
  const int keyId = keyBox.getSelectedId();

  static const char* keyNames[] = {"C", "C#", "D", "D#", "E", "F",
                                   "F#", "G", "G#", "A", "A#", "B"};

  visible.clear();
  for (int i = 0; i < (int)cache.size(); ++i) {
    const auto& it = cache[(size_t)i];

    if (q.isNotEmpty()) {
      auto hay = (it.name + " " + it.folder + " " + it.key).toLowerCase();
      if (!hay.contains(q)) continue;
    }

    if (bpmId > 1) {
      if (it.bpm == 0) continue;
      bool ok = false;
      switch (bpmId) {
        case 2: ok = it.bpm >= 60 && it.bpm < 90; break;
        case 3: ok = it.bpm >= 90 && it.bpm < 110; break;
        case 4: ok = it.bpm >= 110 && it.bpm < 130; break;
        case 5: ok = it.bpm >= 130 && it.bpm < 160; break;
        case 6: ok = it.bpm >= 160; break;
        default: ok = true; break;
      }
      if (!ok) continue;
    }

    if (keyId > 1) {
      if (it.key.isEmpty()) continue;
      if (!it.key.startsWith(keyNames[keyId - 2])) continue;
    }

    visible.push_back(i);
  }

  list.updateContent();
  list.repaint();
  updateStatus();
}

void SampleBrowser::updateStatus() {
  auto& lib = processor.getSampleLibrary();
  juce::String t;
  if (lib.isScanning()) t << "Escaneando... ";
  t << lib.numItems() << " samples";
  if (!lib.isScanning() && (int)visible.size() != lib.numItems())
    t << " (" << (int)visible.size() << " mostrados)";
  statusLabel.setText(t, juce::dontSendNotification);
}

void SampleBrowser::addFolder() {
  chooser = std::make_unique<juce::FileChooser>(
      "Anade una carpeta de samples",
      juce::File::getSpecialLocation(juce::File::userMusicDirectory));
  chooser->launchAsync(juce::FileBrowserComponent::openMode |
                       juce::FileBrowserComponent::canSelectDirectories,
      [this](const juce::FileChooser& fc) {
        auto dir = fc.getResult();
        if (!dir.isDirectory()) return;
        auto& lib = processor.getSampleLibrary();
        auto folders = lib.getFolders();
        if (!folders.contains(dir.getFullPathName()))
          folders.add(dir.getFullPathName());
        lib.setFolders(folders);
      });
}

} // namespace MidiForge
