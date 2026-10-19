#include "AutoSoundPanel.h"

#include <functional>

#include "../LookAndFeel.h"

namespace MidiForge {

namespace {
constexpr int kMelodic = 0;
constexpr int kDrums = 1;
constexpr int kMix = 2;
} // namespace

// Small chamfered drag chip: hovering shows "Arrastra a FL"; dragging starts a
// native file drag (FL copies the file into the project).
class DragChip : public juce::Component {
public:
  DragChip(juce::String labelText, std::function<juce::File()> provider)
      : text(std::move(labelText)), fileProvider(std::move(provider)) {
    setMouseCursor(juce::MouseCursor::DraggingHandCursor);
  }

  void paint(juce::Graphics& g) override {
    auto r = getLocalBounds().toFloat().reduced(0.5f);
    auto p = chamferRect(r, 6.0f);
    g.setColour(hover ? Palette::cardHover : Palette::card);
    g.fillPath(p);
    g.setColour(hover ? Palette::accent : Palette::border);
    g.strokePath(p, juce::PathStrokeType(1.0f));
    g.setColour(hover ? Palette::accentSoft : Palette::textSecondary);
    g.setFont(juce::Font(juce::FontOptions(11.0f)));
    g.drawText(hover ? "Arrastra a FL" : text, getLocalBounds(),
               juce::Justification::centred);
  }

  void mouseEnter(const juce::MouseEvent&) override { hover = true; repaint(); }
  void mouseExit(const juce::MouseEvent&) override { hover = false; repaint(); }
  void mouseDrag(const juce::MouseEvent&) override {
    if (fileProvider == nullptr) return;
    const auto f = fileProvider();
    if (!f.existsAsFile()) return;
    juce::StringArray files;
    files.add(f.getFullPathName());
    juce::DragAndDropContainer::performExternalDragDropOfFiles(files, false, this);
  }

private:
  juce::String text;
  std::function<juce::File()> fileProvider;
  bool hover{false};
};

struct AutoSoundPanel::Row {
  juce::String title;
  SoundSlot slot{SoundSlot::Melody};
  int kind{kMelodic};
  int index{0};
  juce::String trackName;
  std::unique_ptr<juce::Label> titleLabel;
  std::unique_ptr<juce::Label> nameLabel;
  std::unique_ptr<juce::TextButton> rerollButton;
  std::unique_ptr<juce::TextButton> onlineButton;
  std::unique_ptr<juce::TextButton> webButton;
  std::unique_ptr<juce::TextButton> playButton;
  std::unique_ptr<DragChip> sampleChip;
  std::unique_ptr<DragChip> loopChip;
};

AutoSoundPanel::AutoSoundPanel(MidiForgeAudioProcessor& p) : processor(p) {
  titleLabel = std::make_unique<juce::Label>();
  titleLabel->setText("AUTO-SOUND", juce::dontSendNotification);
  titleLabel->setColour(juce::Label::textColourId, Palette::accent);
  titleLabel->setFont(juce::Font(juce::FontOptions(13.0f, juce::Font::bold)));
  addAndMakeVisible(*titleLabel);

  subtitleLabel = std::make_unique<juce::Label>();
  subtitleLabel->setText(
      "Sonidos automaticos para esta idea: tus packs (y Freesound con key).",
      juce::dontSendNotification);
  subtitleLabel->setColour(juce::Label::textColourId, Palette::textSecondary);
  subtitleLabel->setFont(juce::Font(juce::FontOptions(11.0f)));
  addAndMakeVisible(*subtitleLabel);

  auto makeButton = [this](const juce::String& t) {
    auto b = std::make_unique<juce::TextButton>(t);
    b->setColour(juce::TextButton::buttonColourId, Palette::card);
    b->setColour(juce::TextButton::textColourOffId, Palette::textPrimary);
    b->setColour(juce::TextButton::textColourOnId, Palette::textPrimary);
    addAndMakeVisible(*b);
    return b;
  };

  rerollAllButton = makeButton("Variar todo");
  rerollAllButton->setColour(juce::TextButton::buttonColourId, Palette::accentDim);
  rerollAllButton->onClick = [this] {
    processor.pickAutoSoundsForIdea();
    lastStatus = "Combinacion completa nueva.";
    refresh();
  };

  rerollHintLabel = std::make_unique<juce::Label>();
  rerollHintLabel->setText("Otro cambia una capa; esto cambia todas.",
                           juce::dontSendNotification);
  rerollHintLabel->setColour(juce::Label::textColourId,
                             Palette::textSecondary.withAlpha(0.8f));
  rerollHintLabel->setFont(juce::Font(juce::FontOptions(11.0f)));
  addAndMakeVisible(*rerollHintLabel);

  keyLabel = std::make_unique<juce::Label>();
  keyLabel->setText("API key Freesound:", juce::dontSendNotification);
  keyLabel->setColour(juce::Label::textColourId, Palette::textPrimary);
  keyLabel->setFont(juce::Font(juce::FontOptions(11.0f)));
  addAndMakeVisible(*keyLabel);

  keyEditor = std::make_unique<juce::TextEditor>();
  keyEditor->setPasswordCharacter((juce::juce_wchar)0x2022);
  keyEditor->setText(processor.getFreesoundKey(), juce::dontSendNotification);
  keyEditor->setTextToShowWhenEmpty("opcional: freesound.org/apiv2/apply",
                                    Palette::textSecondary.withAlpha(0.55f));
  keyEditor->setColour(juce::TextEditor::backgroundColourId, Palette::background);
  keyEditor->setColour(juce::TextEditor::outlineColourId, Palette::border);
  keyEditor->setColour(juce::TextEditor::textColourId, Palette::textPrimary);
  keyEditor->setFont(juce::Font(juce::FontOptions(11.0f)));
  addAndMakeVisible(*keyEditor);

  saveKeyButton = makeButton("Guardar");
  saveKeyButton->onClick = [this] {
    processor.setFreesoundKey(keyEditor->getText());
    lastStatus = processor.getFreesoundKey().isEmpty()
                     ? juce::String("Key borrada.")
                     : juce::String("Key guardada. Freesound listo (solo CC0).");
    refresh();
  };

  statusLabel = std::make_unique<juce::Label>();
  statusLabel->setColour(juce::Label::textColourId, Palette::accentSoft);
  statusLabel->setFont(juce::Font(juce::FontOptions(11.0f)));
  addAndMakeVisible(*statusLabel);

  struct SeedRow {
    const char* title;
    SoundSlot slot;
    int kind;
    const char* track;
  };
  const SeedRow seeds[] = {
      { "Melodia", SoundSlot::Melody, kMelodic, "Melody" },
      { "Acordes", SoundSlot::Chords, kMelodic, "Chords" },
      { "Bajo", SoundSlot::Bass, kMelodic, "Bass" },
      { "Bateria", SoundSlot::Kick, kDrums, "Drums" },
      { "Mezcla", SoundSlot::Melody, kMix, "" },
  };

  int index = 0;
  for (const auto& sd : seeds) {
    auto row = std::make_unique<Row>();
    row->title = sd.title;
    row->slot = sd.slot;
    row->kind = sd.kind;
    row->index = index++;
    row->trackName = sd.track;

    row->titleLabel = std::make_unique<juce::Label>();
    row->titleLabel->setText(sd.title, juce::dontSendNotification);
    row->titleLabel->setColour(juce::Label::textColourId, Palette::textPrimary);
    row->titleLabel->setFont(juce::Font(juce::FontOptions(12.0f, juce::Font::bold)));
    addAndMakeVisible(*row->titleLabel);

    row->nameLabel = std::make_unique<juce::Label>();
    row->nameLabel->setColour(juce::Label::textColourId, Palette::textSecondary);
    row->nameLabel->setFont(juce::Font(juce::FontOptions(11.0f)));
    row->nameLabel->setMinimumHorizontalScale(0.6f);
    addAndMakeVisible(*row->nameLabel);

    if (sd.kind == kMelodic) {
      row->rerollButton = makeButton("Otro");
      row->onlineButton = makeButton("Freesound");
      row->webButton = makeButton("Web");
      row->playButton = makeButton("Escuchar");
      row->rerollButton->onClick = [this, rp = row.get()] { applyReroll(*rp); };
      row->onlineButton->onClick = [this, rp = row.get()] { applyOnline(*rp); };
      row->webButton->onClick = [this, rp = row.get()] { applyWeb(*rp); };
      row->playButton->onClick = [this, rp = row.get()] { applyPlay(*rp); };

      const int slotIndex = (int)sd.slot;
      row->sampleChip = std::make_unique<DragChip>(
          "Sample", [this, slotIndex]() -> juce::File {
            auto& s = processor.getAutoSounds();
            if (s.paths[slotIndex].isEmpty()) return {};
            return juce::File(s.paths[slotIndex]);
          });
      addAndMakeVisible(*row->sampleChip);
    } else if (sd.kind == kDrums) {
      row->rerollButton = makeButton("Otro");
      row->playButton = makeButton("Escuchar");
      row->rerollButton->onClick = [this, rp = row.get()] { applyReroll(*rp); };
      row->playButton->onClick = [this, rp = row.get()] { applyPlay(*rp); };
    } else {
      row->playButton = makeButton("Escuchar");
      row->playButton->onClick = [this, rp = row.get()] { applyPlay(*rp); };
    }

    row->loopChip = std::make_unique<DragChip>(
        sd.kind == kMix ? "Mezcla" : "Loop",
        [this, rp = row.get()]() -> juce::File { return loopFileFor(*rp); });
    addAndMakeVisible(*row->loopChip);

    rows.push_back(std::move(row));
  }

  refresh();
}

AutoSoundPanel::~AutoSoundPanel() = default;

void AutoSoundPanel::paint(juce::Graphics& g) {
  auto full = getLocalBounds().toFloat();
  juce::ColourGradient bg(Palette::panel.brighter(0.02f), full.getX(), full.getY(),
                          Palette::panel.darker(0.12f), full.getX(), full.getBottom(), false);
  g.setGradientFill(bg);
  g.fillRect(full);
}

void AutoSoundPanel::resized() {
  auto r = getLocalBounds().reduced(12);
  titleLabel->setBounds(r.removeFromTop(19));
  subtitleLabel->setBounds(r.removeFromTop(15));
  r.removeFromTop(8);
  {
    auto line = r.removeFromTop(26);
    rerollAllButton->setBounds(line.removeFromLeft(108));
    line.removeFromLeft(10);
    rerollHintLabel->setBounds(line);
  }
  r.removeFromTop(6);
  {
    auto line = r.removeFromTop(26);
    keyLabel->setBounds(line.removeFromLeft(104));
    saveKeyButton->setBounds(line.removeFromRight(70));
    line.removeFromRight(6);
    keyEditor->setBounds(line);
  }
  r.removeFromTop(3);
  statusLabel->setBounds(r.removeFromTop(16));
  r.removeFromTop(6);

  for (auto& row : rows) {
    auto line = r.removeFromTop(34);
    row->titleLabel->setBounds(line.removeFromLeft(66));
    line.removeFromLeft(4);

    int right = 0;
    if (row->kind == kMelodic) right = 42 + 4 + 60 + 4 + 38 + 4 + 58 + 4 + 54 + 4 + 46;
    else if (row->kind == kDrums) right = 42 + 4 + 58 + 4 + 58;
    else right = 58 + 4 + 66;
    const int nameW = juce::jmax(50, line.getWidth() - right - 8);
    row->nameLabel->setBounds(line.removeFromLeft(nameW));
    line.removeFromLeft(8);

    if (row->kind == kMelodic) {
      row->rerollButton->setBounds(line.removeFromLeft(42));
      line.removeFromLeft(4);
      row->onlineButton->setBounds(line.removeFromLeft(60));
      line.removeFromLeft(4);
      row->webButton->setBounds(line.removeFromLeft(38));
      line.removeFromLeft(4);
      row->playButton->setBounds(line.removeFromLeft(58));
      line.removeFromLeft(4);
      row->sampleChip->setBounds(line.removeFromLeft(54));
      line.removeFromLeft(4);
      row->loopChip->setBounds(line.removeFromLeft(46));
    } else if (row->kind == kDrums) {
      row->rerollButton->setBounds(line.removeFromLeft(42));
      line.removeFromLeft(4);
      row->playButton->setBounds(line.removeFromLeft(58));
      line.removeFromLeft(4);
      row->loopChip->setBounds(line.removeFromLeft(58));
    } else {
      row->playButton->setBounds(line.removeFromLeft(58));
      line.removeFromLeft(4);
      row->loopChip->setBounds(line.removeFromLeft(66));
    }
    r.removeFromTop(6);
  }
}

void AutoSoundPanel::visibilityChanged() {
  if (isVisible()) refresh();
}

void AutoSoundPanel::refresh() {
  // First real scan: fill any slot the (empty) initial pick could not fill,
  // without stomping sounds the user already fetched from Freesound.
  const int n = processor.getSampleLibrary().numItems();
  if (n == 0) {
    lazyFillDone = false;
  } else if (!lazyFillDone) {
    lazyFillDone = true;
    processor.fillEmptyAutoSounds();
  }

  for (auto& r : rows) updateRow(*r);

  const bool hasKey = processor.getFreesoundKey().isNotEmpty();
  if (lastStatus.isEmpty())
    lastStatus = hasKey
                     ? juce::String("Freesound: key guardada. Solo descarga licencias CC0.")
                     : juce::String("Freesound opcional: pega tu key gratuita y pulsa Guardar.");
  statusLabel->setText(lastStatus, juce::dontSendNotification);
  if (keyEditor->getText().isEmpty() && hasKey)
    keyEditor->setText(processor.getFreesoundKey(), juce::dontSendNotification);
  repaint();
}

void AutoSoundPanel::timerCallback() {
  stopTimer();
  if (pendingOnlineRow < 0 || pendingOnlineRow >= (int)rows.size()) return;
  auto& row = *rows[(size_t)pendingOnlineRow];
  pendingOnlineRow = -1;
  juce::String status;
  processor.fetchFreesoundForSlot(row.slot, status);
  lastStatus = status.isNotEmpty() ? status : juce::String("Sin resultados");
  refresh();
}

void AutoSoundPanel::updateRow(Row& r) {
  auto& sounds = processor.getAutoSounds();
  if (r.kind == kMelodic) {
    const int si = (int)r.slot;
    const bool has = sounds.paths[si].isNotEmpty();
    r.nameLabel->setText(has ? sounds.names[si] : juce::String("sin sonido: pulsa Otro"),
                         juce::dontSendNotification);
    r.nameLabel->setTooltip(has ? sounds.paths[si] : juce::String());
  } else if (r.kind == kDrums) {
    int count = 0;
    juce::String first;
    for (int s = (int)SoundSlot::Kick; s <= (int)SoundSlot::Crash; ++s) {
      if (sounds.paths[s].isNotEmpty()) {
        ++count;
        if (first.isEmpty()) first = sounds.names[s];
      }
    }
    r.nameLabel->setText(
        count > 0 ? (juce::String(count) + " de 6 sonidos: " + first + "...")
                  : juce::String("sin kit: pulsa Otro"),
        juce::dontSendNotification);
  } else {
    r.nameLabel->setText("loop de la idea completa (4 capas)", juce::dontSendNotification);
  }
}

void AutoSoundPanel::applyReroll(Row& r) {
  if (r.kind == kDrums) {
    for (int s = (int)SoundSlot::Kick; s <= (int)SoundSlot::Crash; ++s)
      processor.rerollAutoSound((SoundSlot)s);
    lastStatus = "Kit nuevo elegido.";
  } else if (r.kind == kMelodic) {
    processor.rerollAutoSound(r.slot);
    lastStatus = "Variacion nueva para " + r.title + ".";
  }
  refresh();
}

void AutoSoundPanel::applyOnline(Row& r) {
  if (processor.getFreesoundKey().isEmpty()) {
    lastStatus = "Pega tu API key de Freesound abajo y pulsa Guardar.";
    refresh();
    return;
  }
  pendingOnlineRow = r.index;
  lastStatus = "Buscando en Freesound...";
  refresh();
  startTimer(50);
}

void AutoSoundPanel::applyWeb(Row& r) {
  const auto term = freesoundQueryFor(r.slot, processor.getCurrentIdea().character);
  if (term.isEmpty()) return;
  juce::URL url("https://samplefocus.com/samples?search=" +
                juce::URL::addEscapeChars(term, true));
  url.launchInDefaultBrowser();
  lastStatus = "SampleFocus abierto: " + term;
  refresh();
}

void AutoSoundPanel::applyPlay(Row& r) {
  if (r.kind == kMelodic) {
    auto& s = processor.getAutoSounds();
    if (s.paths[(int)r.slot].isEmpty()) {
      lastStatus = "Sin sonido asignado: pulsa Otro.";
      refresh();
      return;
    }
    processor.previewSample(juce::File(s.paths[(int)r.slot]));
    lastStatus = "Reproduciendo " + s.names[(int)r.slot];
  } else {
    const auto f = loopFileFor(r);
    if (!f.existsAsFile()) {
      lastStatus = "Nada que renderizar todavia (faltan sonidos).";
      refresh();
      return;
    }
    processor.previewSample(f);
    lastStatus = r.kind == kMix ? juce::String("Reproduciendo la mezcla renderizada.")
                                : juce::String("Reproduciendo el kit renderizado.");
  }
  refresh();
}

juce::File AutoSoundPanel::loopFileFor(const Row& r) const {
  if (r.kind == kMix) return processor.renderMixAudio();
  return processor.renderTrackAudio(r.trackName);
}

} // namespace MidiForge
