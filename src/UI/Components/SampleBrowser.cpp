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

// Small rounded steel chip used for Key / BPM / format badges.
static void drawChip(juce::Graphics& g, juce::Rectangle<int> area, const juce::String& text,
                     juce::Colour colour) {
  g.setColour(colour.withAlpha(0.14f));
  g.fillRoundedRectangle(area.toFloat(), 3.5f);
  g.setColour(colour);
  g.setFont(juce::Font(juce::FontOptions(10.0f)));
  g.drawText(text, area, juce::Justification::centred);
}

void SampleBrowser::paint(juce::Graphics& g) {
  auto r = getLocalBounds().toFloat();
  juce::ColourGradient bg(Palette::panel.brighter(0.02f), r.getX(), r.getY(),
                          Palette::panel.darker(0.15f), r.getX(), r.getBottom(), false);
  g.setGradientFill(bg);
  g.fillRect(r);
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

  auto full = juce::Rectangle<int>(0, 0, width, height);
  if (selected) {
    auto rr = full.toFloat().reduced(2.0f, 1.0f);
    g.setColour(Palette::accent.withAlpha(0.16f));
    g.fillRoundedRectangle(rr, 4.0f);
    g.setColour(Palette::accent);
    g.fillRoundedRectangle(rr.withWidth(3.0f).reduced(0.0f, 4.0f), 1.5f);
  } else if (row % 2 == 1) {
    g.setColour(juce::Colours::white.withAlpha(0.02f));
    g.fillRect(full);
  }

  auto r = full.reduced(10, 0);

  // Right side: steel chips for key / bpm / format.
  auto chipArea = r.removeFromRight(juce::jmin(300, width / 2)).reduced(0, 5);
  {
    const auto ext = juce::File(it.path).getFileExtension().trimCharactersAtStart(".").toUpperCase();
    const juce::String bpmText = it.bpm > 0 ? (juce::String(it.bpm) + " BPM") : juce::String();
    const int extW = ext.isEmpty() ? 0 : 40;
    const int keyW = it.key.isNotEmpty() ? 40 : 0;
    const int bpmW = bpmText.isNotEmpty()
                         ? juce::jlimit(52, 76, 14 + (int)bpmText.length() * 6)
                         : 0;

    if (extW > 0) {
      auto c = chipArea.removeFromRight(extW);
      drawChip(g, c, ext, Palette::textSecondary);
    }
    if (bpmW > 0) {
      chipArea.removeFromRight(4);
      auto c = chipArea.removeFromRight(bpmW);
      drawChip(g, c, bpmText, Palette::accentSoft);
    }
    if (keyW > 0) {
      chipArea.removeFromRight(4);
      auto c = chipArea.removeFromRight(keyW);
      drawChip(g, c, it.key, Palette::accent);
    }
  }

  // Folder breadcrumb, right aligned before the chips.
  if (r.getWidth() > 220) {
    auto metaArea = r.removeFromRight(juce::jmin(200, r.getWidth() / 2));
    g.setColour(Palette::textSecondary.withAlpha(0.85f));
    g.setFont(juce::Font(juce::FontOptions(10.5f)));
    g.drawText(it.folder, metaArea, juce::Justification::centredRight, true);
  }

  g.setColour(selected ? Palette::textPrimary : Palette::textPrimary.withAlpha(0.92f));
  g.setFont(juce::Font(juce::FontOptions(12.5f)));
  g.drawText(it.name, r, juce::Justification::centredLeft, true);
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
