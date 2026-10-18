#include "PianoRollMini.h"
#include "../LookAndFeel.h"
#include <cmath>

namespace MidiForge {

void PianoRollMini::paint(juce::Graphics& g) {
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
    g.drawText("No notes yet", getLocalBounds(), juce::Justification::centred);
    return;
  }

  int minPitch = 127, maxPitch = 0;
  double maxBeat = 0.0;
  for(const auto& n : track->notes) {
    minPitch = juce::jmin(minPitch, n.pitch);
    maxPitch = juce::jmax(maxPitch, n.pitch);
    maxBeat = juce::jmax(maxBeat, n.start + n.length);
  }
  if(maxBeat <= 0) maxBeat = 4;
  if(maxPitch - minPitch < 6) { maxPitch += 3; minPitch -= 3; }

  auto area = r.reduced(8.0f);
  double pitchRange = (double)(maxPitch - minPitch + 1);

  // Vertical grid: 16th / beat / bar lines.
  for(double b = 0.0; b <= maxBeat + 1e-6; b += 0.25) {
    float x = (float)(area.getX() + (b / maxBeat) * area.getWidth());
    const bool bar  = std::abs(std::fmod(b, 4.0)) < 1e-6;
    const bool beat = std::abs(std::fmod(b, 1.0)) < 1e-6;
    g.setColour(Palette::border.withAlpha(bar ? 0.55f : (beat ? 0.32f : 0.13f)));
    g.drawVerticalLine((int)x, area.getY(), area.getBottom());
  }

  // Octave rows (C lines).
  for(int p = minPitch; p <= maxPitch; ++p) {
    if(p % 12 == 0) {
      float y = (float)(area.getBottom() - ((p - minPitch + 1) / pitchRange) * area.getHeight());
      g.setColour(Palette::border.withAlpha(0.30f));
      g.drawHorizontalLine((int)y, area.getX(), area.getRight());
    }
  }

  // Notes: steel ramp across pitch classes, brighter with velocity.
  for(const auto& n : track->notes) {
    float x = (float)(area.getX() + (n.start / maxBeat) * area.getWidth());
    float w = (float)juce::jmax(2.5, (n.length / maxBeat) * area.getWidth());
    float y = (float)(area.getBottom() - ((n.pitch - minPitch + 1) / pitchRange) * area.getHeight());
    float h = (float)(area.getHeight() / pitchRange);
    h = juce::jmax(h, 3.0f);

    auto c = noteColour(n.pitch, (float)n.velocity / 127.0f);
    auto nr = juce::Rectangle<float>(x, y, w, h);

    g.setColour(c.withAlpha(0.95f));
    g.fillRoundedRectangle(nr, 2.5f);

    // Cold light catching the top edge.
    g.setColour(juce::Colours::white.withAlpha(0.25f * ((float)n.velocity / 127.0f)));
    g.drawHorizontalLine((int)nr.getY() + 1, nr.getX() + 2.0f,
                         juce::jmax(nr.getX() + 2.0f, nr.getRight() - 2.0f));
  }
}

} // namespace MidiForge
