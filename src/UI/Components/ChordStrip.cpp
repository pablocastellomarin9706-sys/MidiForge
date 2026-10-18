#include "ChordStrip.h"
#include "../LookAndFeel.h"
#include <cmath>

namespace MidiForge {

void ChordStrip::paint(juce::Graphics& g) {
  auto r = getLocalBounds().toFloat();
  const float radius = 8.0f;

  // Steel card.
  juce::ColourGradient card(Palette::panelAlt.brighter(0.05f), r.getX(), r.getY(),
                            Palette::panelAlt.darker(0.25f), r.getX(), r.getBottom(), false);
  g.setGradientFill(card);
  g.fillRoundedRectangle(r, radius);
  g.setColour(Palette::border);
  g.drawRoundedRectangle(r.reduced(0.5f), radius, 1.0f);

  if(track == nullptr || track->notes.empty()) {
    g.setColour(Palette::textSecondary);
    g.setFont(13.0f);
    g.drawText("No chords yet", getLocalBounds(), juce::Justification::centred);
    return;
  }

  // Group notes by start time -> chord blocks.
  double maxBeat = 0.0;
  for(const auto& n : track->notes) maxBeat = juce::jmax(maxBeat, n.start + n.length);
  if(maxBeat <= 0) maxBeat = 4;

  // Header: track name.
  g.setColour(Palette::textSecondary);
  g.setFont(11.0f);
  g.drawText(track->name, getLocalBounds().reduced(10).removeFromTop(16),
             juce::Justification::topLeft);

  const float blockH = juce::jmax(10.0f, (float)(getHeight() - 46));
  int count = 0;
  for(size_t i = 0; i < track->notes.size(); ++i) {
    if(track->notes[i].start < 0.0) continue;
    // Detect new chord by start change.
    if(i > 0 && std::abs(track->notes[i].start - track->notes[i - 1].start) < 1e-6) continue;

    float x = (float)((track->notes[i].start / maxBeat) * (getWidth() - 16) + 8);
    float w = (float)std::max(40.0, (4.0 / maxBeat) * (getWidth() - 16));
    auto cr = juce::Rectangle<float>(x, 34.0f, w - 8.0f, blockH);
    if(cr.getWidth() < 4.0f) { ++count; continue; }

    // Each chord gets its own steel tint (track colour harmonised towards blue).
    const float t = (float)((count * 37) % 100) / 100.0f;
    auto base = track->colour.interpolatedWith(Palette::accent, 0.55f)
                    .withMultipliedBrightness(0.80f + 0.45f * t);

    juce::ColourGradient grad(base.brighter(0.10f), cr.getX(), cr.getY(),
                              base.darker(0.28f), cr.getX(), cr.getBottom(), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(cr, 6.0f);

    // Top light + soft cold edge.
    g.setColour(juce::Colours::white.withAlpha(0.18f));
    g.drawHorizontalLine((int)cr.getY() + 1, cr.getX() + 5.0f,
                         juce::jmax(cr.getX() + 5.0f, cr.getRight() - 5.0f));
    g.setColour(Palette::accentSoft.withAlpha(0.30f));
    g.drawRoundedRectangle(cr.reduced(0.5f), 6.0f, 1.0f);

    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(juce::FontOptions(12.5f, juce::Font::bold)));
    juce::String sym = ((size_t)count < track->chordSymbols.size())
                           ? track->chordSymbols[(size_t)count]
                           : ("Chord " + juce::String(count + 1));
    g.drawText(sym, (int)cr.getX(), (int)cr.getY(), (int)cr.getWidth(), (int)cr.getHeight(),
               juce::Justification::centred);
    ++count;
  }
}

} // namespace MidiForge
