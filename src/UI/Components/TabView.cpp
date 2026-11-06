#include "TabView.h"
#include "../LookAndFeel.h"

namespace MidiForge {

TrackPage::TrackPage(MidiForgeAudioProcessor& p, juce::String name, int previewKind)
  : processor(p), trackName(std::move(name)), kind(previewKind), drag(p, trackName) {
  if(kind == 0) addAndMakeVisible(chordStrip);
  else if(kind == 2) addAndMakeVisible(drumGrid);
  else addAndMakeVisible(pianoRoll);

  addAndMakeVisible(drag);
  drag.onRegenerate = [this] {
    processor.regenerateTrack(trackName);
    refresh();
  };
  // Manual note edits mark the host state dirty and refresh the preview.
  pianoRoll.onEdited = [this] {
    processor.updateHostDisplay();
    refresh();
  };
  drumGrid.onEdited = [this] {
    processor.updateHostDisplay();
    refresh();
  };

  lockButton.setClickingTogglesState(true);
  lockButton.setColour(juce::TextButton::buttonOnColourId, Palette::accentDim.brighter(0.22f));
  lockButton.setTooltip(
      "Bloquear este track: GENERATE, Shuffle, Replace y los cambios de "
      "parametros no lo regeneran. Se guarda con el proyecto.");
  lockButton.onClick = [this] {
    if (auto* t = trackMutable()) t->locked = lockButton.getToggleState();
    processor.updateHostDisplay();
    refresh();
  };
  addAndMakeVisible(lockButton);

  refresh();
}

const TrackIdea* TrackPage::findTrack() const {
  for(const auto& t : processor.getCurrentIdea().tracks)
    if(t.name == trackName) return &t;
  return nullptr;
}

TrackIdea* TrackPage::trackMutable() {
  for(auto& t : processor.getCurrentIdea().tracks)
    if(t.name == trackName) return &t;
  return nullptr;
}

void TrackPage::refresh() {
  TrackIdea* t = trackMutable();
  auto& idea = processor.getCurrentIdea();
  const int bStart = idea.hasSectionB() ? idea.bStart() : -1;
  chordStrip.setTrack(t);
  pianoRoll.setTrack(t, idea.bars, kind == 1, bStart);
  drumGrid.setTrack(t, idea.bars, kind == 2, bStart);
  lockButton.setToggleState(t != nullptr && t->locked, juce::dontSendNotification);
  detailText = (t != nullptr && t->detail.isNotEmpty()) ? t->detail : juce::String();
  if (t != nullptr && t->locked)
    detailText += "  ·  bloqueado";
  if (t != nullptr && t->edited)
    detailText += "  ·  editado a mano";
  repaint();
}

void TrackPage::paint(juce::Graphics& g) {
  auto full = getLocalBounds().toFloat();
  juce::ColourGradient bg(Palette::panel.brighter(0.02f), full.getX(), full.getY(),
                          Palette::panel.darker(0.12f), full.getX(), full.getBottom(), false);
  g.setGradientFill(bg);
  g.fillRect(full);

  auto r = getLocalBounds().reduced(12).removeFromTop(18);
  auto nameArea = r.removeFromLeft(90);
  r.removeFromRight(70); // room for the BLOQ button
  g.setColour(Palette::accent);
  g.fillEllipse((float)nameArea.getX(), (float)nameArea.getCentreY() - 2.5f, 5.0f, 5.0f);
  g.setFont(juce::Font(juce::FontOptions(12.0f, juce::Font::bold)));
  g.setColour(Palette::textPrimary);
  g.drawText(trackName, nameArea.withTrimmedLeft(10), juce::Justification::centredLeft);
  if (detailText.isNotEmpty()) {
    g.setColour(Palette::accentSoft.withAlpha(0.75f));
    g.setFont(juce::Font(juce::FontOptions(12.0f)));
    g.drawText(detailText, r, juce::Justification::centredLeft);
  }
}

void TrackPage::resized() {
  auto r = getLocalBounds().reduced(12);
  auto header = r.removeFromTop(22);
  lockButton.setBounds(header.removeFromRight(62).withSizeKeepingCentre(62, 18));
  drag.setBounds(r.removeFromTop(34));
  r.removeFromTop(10);
  if(kind == 0) chordStrip.setBounds(r);
  else if(kind == 2) drumGrid.setBounds(r);
  else pianoRoll.setBounds(r);
}

TabView::TabView(MidiForgeAudioProcessor& p) : tabs(juce::TabbedButtonBar::TabsAtTop) {
  addAndMakeVisible(tabs);
  auto makePage = [&](const juce::String& name, int kind) {
    auto page = std::make_unique<TrackPage>(p, name, kind);
    tabs.addTab(name, Palette::panel, page.get(), false);
    pages.push_back(std::move(page));
  };
  makePage("Chords", 0);
  makePage("Melody", 1);
  makePage("Bass", 1);
  makePage("Drums", 2);

  arrangementPage = std::make_unique<ArrangementPanel>(p);
  arrangementPage->onApplied = [this] {
    // The boundary moved: repaint the roll/grid/chord marks of every track.
    for (auto& pg : pages) pg->refresh();
  };
  tabs.addTab("ARREGLO", Palette::panel, arrangementPage.get(), false);
  arrangementPage->refresh();

  samplePage = std::make_unique<SampleBrowser>(p);
  tabs.addTab("SAMPLES", Palette::panel, samplePage.get(), false);

  soundPage = std::make_unique<AutoSoundPanel>(p);
  tabs.addTab("SONIDO", Palette::panel, soundPage.get(), false);
}

void TabView::refreshAll() {
  for(auto& p : pages) p->refresh();
  if (arrangementPage) arrangementPage->refresh();
  if (soundPage) soundPage->refresh();
}

void TabView::resized() {
  tabs.setBounds(getLocalBounds());
}

} // namespace MidiForge
