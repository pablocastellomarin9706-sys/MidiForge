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
  refresh();
}

const TrackIdea* TrackPage::findTrack() const {
  for(const auto& t : processor.getCurrentIdea().tracks)
    if(t.name == trackName) return &t;
  return nullptr;
}

void TrackPage::refresh() {
  const TrackIdea* t = findTrack();
  chordStrip.setTrack(t);
  pianoRoll.setTrack(t);
  drumGrid.setTrack(t);
  detailText = (t != nullptr && t->detail.isNotEmpty()) ? t->detail : juce::String();
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
  r.removeFromTop(22);
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

  samplePage = std::make_unique<SampleBrowser>(p);
  tabs.addTab("SAMPLES", Palette::panel, samplePage.get(), false);

  soundPage = std::make_unique<AutoSoundPanel>(p);
  tabs.addTab("SONIDO", Palette::panel, soundPage.get(), false);
}

void TabView::refreshAll() {
  for(auto& p : pages) p->refresh();
  if (soundPage) soundPage->refresh();
}

void TabView::resized() {
  tabs.setBounds(getLocalBounds());
}

} // namespace MidiForge
