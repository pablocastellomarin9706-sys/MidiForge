#include "SampleBrowser.h"
#include "../LookAndFeel.h"

namespace MidiForge {

// --- helpers ---------------------------------------------------------------

// Pitch class of a parsed key string ("Fm" -> 5, "C#" -> 1...). -1 = unknown.
static int keyPitchClass(const juce::String& key) {
  if (key.isEmpty()) return -1;
  int pc = -1;
  switch (juce::CharacterFunctions::toUpperCase(key[0])) {
    case 'C': pc = 0; break;
    case 'D': pc = 2; break;
    case 'E': pc = 4; break;
    case 'F': pc = 5; break;
    case 'G': pc = 7; break;
    case 'A': pc = 9; break;
    case 'B': pc = 11; break;
    default: return -1;
  }
  if (key.length() > 1) {
    if (key[1] == '#') pc = (pc + 1) % 12;
    else if (key[1] == 'b') pc = (pc + 11) % 12;
  }
  return pc;
}

static bool scaleContainsPitch(ScaleType scale, int root, int pc) {
  static const int major[7]     = {0, 2, 4, 5, 7, 9, 11};
  static const int minorNat[7]  = {0, 2, 3, 5, 7, 8, 10};
  static const int minorHarm[7] = {0, 2, 3, 5, 7, 8, 11};
  static const int minorMel[7]  = {0, 2, 3, 5, 7, 9, 11};
  static const int dorian[7]    = {0, 2, 3, 5, 7, 9, 10};
  static const int phrygian[7]  = {0, 1, 3, 5, 7, 8, 10};
  static const int lydian[7]    = {0, 2, 4, 6, 7, 9, 11};
  static const int mixo[7]      = {0, 2, 4, 5, 7, 9, 10};
  static const int locrian[7]   = {0, 1, 3, 5, 6, 8, 10};
  static const int pentMaj[5]   = {0, 2, 4, 7, 9};
  static const int pentMin[5]   = {0, 3, 5, 7, 10};
  static const int blues[6]     = {0, 3, 5, 6, 7, 10};

  const int* steps = nullptr;
  int n = 0;
  switch (scale) {
    case ScaleType::Major: steps = major; n = 7; break;
    case ScaleType::MinorNatural:
    case ScaleType::Aeolian: steps = minorNat; n = 7; break;
    case ScaleType::MinorHarmonic: steps = minorHarm; n = 7; break;
    case ScaleType::MinorMelodic: steps = minorMel; n = 7; break;
    case ScaleType::Dorian: steps = dorian; n = 7; break;
    case ScaleType::Phrygian: steps = phrygian; n = 7; break;
    case ScaleType::Lydian: steps = lydian; n = 7; break;
    case ScaleType::Mixolydian: steps = mixo; n = 7; break;
    case ScaleType::Locrian: steps = locrian; n = 7; break;
    case ScaleType::PentatonicMajor: steps = pentMaj; n = 5; break;
    case ScaleType::PentatonicMinor: steps = pentMin; n = 5; break;
    case ScaleType::Blues: steps = blues; n = 6; break;
    case ScaleType::Chromatic: return true;
  }

  const int rel = ((pc - root) % 12 + 12) % 12;
  for (int i = 0; i < n; ++i)
    if (steps[i] == rel) return true;
  return false;
}

// Five point star used for the favourite column.
static void drawStar(juce::Graphics& g, juce::Rectangle<int> area, bool filled) {
  juce::Path p;
  const float cx = (float)area.getCentreX();
  const float cy = (float)area.getCentreY();
  const float rout = 6.5f;
  const float rin = 2.7f;
  for (int i = 0; i < 10; ++i) {
    const float r = (i % 2 == 0) ? rout : rin;
    const float a = -juce::MathConstants<float>::halfPi +
                    (float)i * juce::MathConstants<float>::pi / 5.0f;
    const float x = cx + r * std::cos(a);
    const float y = cy + r * std::sin(a);
    if (i == 0) p.startNewSubPath(x, y);
    else p.lineTo(x, y);
  }
  p.closeSubPath();
  if (filled) {
    g.setColour(Palette::star);
    g.fillPath(p);
  } else {
    g.setColour(Palette::border.brighter(0.30f));
    g.strokePath(p, juce::PathStrokeType(1.2f, juce::PathStrokeType::curved,
                                         juce::PathStrokeType::rounded));
  }
}

// "kick_detroit_140.wav" -> {"kick", "detroit"} (numbers and short tokens out).
static juce::StringArray nameTokens(const juce::String& name) {
  juce::StringArray out;
  juce::String tok;
  auto flush = [&] {
    if (tok.length() >= 3 && !tok.containsOnly("0123456789"))
      out.addIfNotAlreadyThere(tok.toLowerCase());
    tok.clear();
  };
  for (int i = 0; i < name.length(); ++i) {
    const auto c = name[i];
    if (juce::CharacterFunctions::isLetterOrDigit(c))
      tok << juce::String::charToString(c);
    else
      flush();
  }
  flush();
  return out;
}

static int nameTokenOverlap(const juce::String& a, const juce::String& b) {
  const auto ta = nameTokens(a);
  if (ta.isEmpty()) return 0;
  const auto tb = nameTokens(b);
  int n = 0;
  for (int i = 0; i < ta.size(); ++i)
    if (tb.contains(ta[i])) ++n;
  return n;
}

static bool isSimilarSample(const SampleItem& a, const SampleItem& b) {
  if (a.path == b.path) return false;
  if (nameTokenOverlap(a.name, b.name) <= 0) return false;
  if (a.bpm > 0 && b.bpm > 0 && std::abs(a.bpm - b.bpm) > 8) return false;
  return true;
}

static int similarityScore(const SampleItem& anchor, const SampleItem& it) {
  int s = nameTokenOverlap(anchor.name, it.name) * 100;
  if (anchor.bpm > 0 && it.bpm > 0) s -= juce::jmin(99, std::abs(anchor.bpm - it.bpm));
  if (anchor.key.isNotEmpty() && anchor.key == it.key) s += 30;
  if (anchor.folder.isNotEmpty() && anchor.folder == it.folder) s += 10;
  return s;
}

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

  fitButton.setClickingTogglesState(true);
  fitButton.setColour(juce::TextButton::buttonOnColourId, Palette::accent);
  fitButton.onClick = [this] {
    fitMode = fitButton.getToggleState();
    rebuildFilter();
  };
  addAndMakeVisible(fitButton);

  favButton.setClickingTogglesState(true);
  favButton.setColour(juce::TextButton::buttonOnColourId, Palette::star.darker(0.30f));
  favButton.onClick = [this] {
    favMode = favButton.getToggleState();
    rebuildFilter();
  };
  addAndMakeVisible(favButton);

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
  list.onRowClicked = [this](int row, const juce::MouseEvent& e) { handleRowClick(row, e); };
  list.onRowDoubleClicked = [this](int row) { handleRowDoubleClick(row); };

  auto& lib = processor.getSampleLibrary();
  lib.onScanChanged = [this] { rebuildFilter(); };

  if (lib.getFolders().isEmpty()) {
    juce::StringArray def;
    def.add(juce::File::getSpecialLocation(juce::File::userMusicDirectory).getFullPathName());
    lib.setFolders(def);
  } else {
    lib.rescan();
  }

  startTimer(1500);
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

  addFolderButton.setBounds(top.removeFromLeft(104));
  top.removeFromLeft(6);
  rescanButton.setBounds(top.removeFromLeft(84));
  top.removeFromLeft(6);
  fitButton.setBounds(top.removeFromLeft(52));
  top.removeFromLeft(6);
  favButton.setBounds(top.removeFromLeft(86));
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
  const int cacheIndex = visible[(size_t)row];
  const auto& it = cache[(size_t)cacheIndex];

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

  // Favourite star (click to toggle).
  auto starArea = r.removeFromLeft(20);
  drawStar(g, starArea, processor.getSampleLibrary().isFavorite(it.path));

  // Audition indicator.
  if (processor.isPreviewing() && previewCacheIndex == cacheIndex) {
    g.setColour(Palette::fit);
    g.fillEllipse((float)r.getX(), (float)full.getCentreY() - 2.5f, 5.0f, 5.0f);
    r.removeFromLeft(10);
  }

  // Right side: steel chips for fit / key / bpm / format.
  auto chipArea = r.removeFromRight(juce::jmin(300, width / 2)).reduced(0, 5);
  {
    const auto ext = juce::File(it.path).getFileExtension().trimCharactersAtStart(".").toUpperCase();
    const juce::String bpmText = it.bpm > 0 ? (juce::String(it.bpm) + " BPM") : juce::String();
    const int extW = ext.isEmpty() ? 0 : 40;
    const int keyW = it.key.isNotEmpty() ? 40 : 0;
    const int bpmW = bpmText.isNotEmpty()
                         ? juce::jlimit(52, 76, 14 + (int)bpmText.length() * 6)
                         : 0;

    if (isFit(it)) {
      drawChip(g, chipArea.removeFromLeft(34), "FIT", Palette::fit);
      chipArea.removeFromLeft(4);
    }
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

  if (similarAnchor >= (int)cache.size()) similarAnchor = -1;
  const SampleItem* anchor = (similarAnchor >= 0) ? &cache[(size_t)similarAnchor] : nullptr;

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
      if (keyPitchClass(it.key) != keyId - 2) continue;
    }

    if (favMode && !processor.getSampleLibrary().isFavorite(it.path)) continue;
    if (fitMode && !isFit(it)) continue;
    if (anchor != nullptr && !isSimilarSample(*anchor, it)) continue;

    visible.push_back(i);
  }

  // Smart ordering: similar results by closeness, FIT by tempo distance.
  if (anchor != nullptr) {
    const SampleItem a = *anchor;
    std::sort(visible.begin(), visible.end(), [this, &a](int l, int r) {
      return similarityScore(a, cache[(size_t)l]) > similarityScore(a, cache[(size_t)r]);
    });
  } else if (fitMode) {
    const double hb = processor.getHostBpm();
    std::sort(visible.begin(), visible.end(), [this, hb](int l, int r) {
      return std::abs((double)cache[(size_t)l].bpm - hb) <
             std::abs((double)cache[(size_t)r].bpm - hb);
    });
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

  const double hb = processor.getHostBpm();
  if (hb > 1.0)
    t << "   |   Proyecto: " << juce::String(hb, 1) << " BPM";
  else if (fitMode)
    t << "   |   FIT: dale al play en FL para leer el BPM";

  if (similarAnchor >= 0 && similarAnchor < (int)cache.size())
    t << "   |   Similares a: " << cache[(size_t)similarAnchor].name;

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

bool SampleBrowser::isFit(const SampleItem& it) const {
  const double hostBpm = processor.getHostBpm();
  if (hostBpm <= 1.0) return false;
  if (it.bpm <= 0) return false;

  const double d = std::abs((double)it.bpm - hostBpm);
  if (d > juce::jmax(2.0, hostBpm * 0.02)) return false;

  const int pc = keyPitchClass(it.key);
  if (pc < 0) return true; // key unknown: tempo fit is enough
  const auto& idea = processor.getCurrentIdea();
  return scaleContainsPitch(idea.scale, idea.keyRoot, pc);
}

void SampleBrowser::handleRowClick(int row, const juce::MouseEvent& e) {
  if (row < 0 || row >= (int)visible.size()) return;
  const int cacheIndex = visible[(size_t)row];

  if (e.mods.isPopupMenu()) {
    showRowMenu(cacheIndex);
    return;
  }

  // Star zone on the left toggles the favourite.
  if (e.x < 34) {
    processor.getSampleLibrary().toggleFavorite(cache[(size_t)cacheIndex].path);
    list.repaint();
    if (favMode) rebuildFilter();
  }
}

void SampleBrowser::handleRowDoubleClick(int row) {
  if (row < 0 || row >= (int)visible.size()) return;
  const int cacheIndex = visible[(size_t)row];

  if (processor.isPreviewing() && previewCacheIndex == cacheIndex) {
    processor.stopPreview();
    previewCacheIndex = -1;
  } else {
    const juce::File f(cache[(size_t)cacheIndex].path);
    if (f.existsAsFile() && processor.previewSample(f))
      previewCacheIndex = cacheIndex;
  }
  list.repaint();
}

void SampleBrowser::showRowMenu(int cacheIndex) {
  if (cacheIndex < 0 || cacheIndex >= (int)cache.size()) return;
  const auto& it = cache[(size_t)cacheIndex];

  juce::PopupMenu menu;
  menu.addSectionHeader(it.name);
  const bool playing = processor.isPreviewing() && previewCacheIndex == cacheIndex;
  menu.addItem(5, playing ? "Parar" : "Escuchar");
  menu.addItem(1, "Buscar similares");
  if (similarAnchor >= 0) menu.addItem(4, "Quitar filtro de similares");
  const bool fav = processor.getSampleLibrary().isFavorite(it.path);
  menu.addItem(2, fav ? "Quitar de favoritos" : "Anadir a favoritos");
  menu.addItem(3, "Revelar en el explorador");

  menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&list),
                     [this, cacheIndex](int result) {
    if (cacheIndex >= (int)cache.size()) return;
    const auto& item = cache[(size_t)cacheIndex];
    if (result == 1) {
      similarAnchor = cacheIndex;
      rebuildFilter();
    } else if (result == 2) {
      processor.getSampleLibrary().toggleFavorite(item.path);
      list.repaint();
      if (favMode) rebuildFilter();
    } else if (result == 3) {
      juce::File(item.path).revealToUser();
    } else if (result == 4) {
      similarAnchor = -1;
      rebuildFilter();
    } else if (result == 5) {
      if (processor.isPreviewing() && previewCacheIndex == cacheIndex) {
        processor.stopPreview();
        previewCacheIndex = -1;
      } else {
        const juce::File f(item.path);
        if (f.existsAsFile() && processor.previewSample(f)) previewCacheIndex = cacheIndex;
      }
      list.repaint();
    }
  });
}

void SampleBrowser::timerCallback() {
  const double hb = processor.getHostBpm();
  if (std::abs(hb - lastHostBpm) > 0.01) {
    lastHostBpm = hb;
    list.repaint();
    updateStatus();
  }

  // Clear the audition indicator when the preview ends by itself.
  if (previewCacheIndex >= 0 && !processor.isPreviewing()) {
    previewCacheIndex = -1;
    list.repaint();
  }
}

} // namespace MidiForge
