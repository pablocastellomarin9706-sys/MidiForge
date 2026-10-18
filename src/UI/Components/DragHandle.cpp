#include "DragHandle.h"
#include "../LookAndFeel.h"
#include "../../MIDI/MidiBuilder.h"
#include "../../MIDI/MidiDrag.h"

namespace MidiForge {

DragHandle::DragHandle(MidiForgeAudioProcessor& p, juce::String name)
  : processor(p), trackName(std::move(name)) {
  setMouseCursor(juce::MouseCursor::DraggingHandCursor);
}

void DragHandle::paint(juce::Graphics& g) {
  auto r = getLocalBounds().toFloat();
  const float radius = 8.0f;

  // Card.
  juce::ColourGradient card(Palette::card.brighter(0.05f), r.getX(), r.getY(),
                            Palette::card.darker(0.20f), r.getX(), r.getBottom(), false);
  g.setGradientFill(card);
  g.fillRoundedRectangle(r, radius);
  g.setColour(Palette::border);
  g.drawRoundedRectangle(r.reduced(0.5f), radius, 1.0f);

  // Replace zone.
  regenArea = getLocalBounds().removeFromLeft(100);
  auto rz = regenArea.toFloat().reduced(3.0f);
  g.setColour(Palette::panelAlt.brighter(0.04f));
  g.fillRoundedRectangle(rz, 6.0f);
  g.setColour(Palette::border.brighter(0.10f));
  g.drawRoundedRectangle(rz, 6.0f, 1.0f);
  g.setColour(Palette::textSecondary);
  g.setFont(juce::Font(juce::FontOptions(11.5f, juce::Font::bold)));
  g.drawText("REPLACE", regenArea, juce::Justification::centred);

  // Drag zone: dashed steel frame that lights up while dragging.
  auto dragArea = getLocalBounds().withTrimmedLeft(108).toFloat().reduced(3.0f);
  {
    juce::Path frame;
    frame.addRoundedRectangle(dragArea, 6.0f);
    const float dashes[] = {5.0f, 4.0f};
    juce::Path dashed;
    juce::PathStrokeType(1.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded)
        .createDashedStroke(dashed, frame, dashes, 2);
    if(dragging) {
      g.setColour(Palette::accent.withAlpha(0.16f));
      g.fillRoundedRectangle(dragArea, 6.0f);
    }
    g.setColour(dragging ? Palette::accent : Palette::accentDim);
    g.fillPath(dashed);
  }

  // Arrow glyph + label.
  auto textArea = dragArea;
  auto glyphArea = textArea.removeFromLeft(26.0f);
  g.setColour(dragging ? Palette::accentSoft : Palette::accent);
  {
    juce::Path arrow;
    const float cx = glyphArea.getCentreX(), cy = glyphArea.getCentreY();
    arrow.startNewSubPath(cx - 7.0f, cy);
    arrow.lineTo(cx + 7.0f, cy);
    arrow.startNewSubPath(cx + 2.0f, cy - 5.0f);
    arrow.lineTo(cx + 7.0f, cy);
    arrow.lineTo(cx + 2.0f, cy + 5.0f);
    g.strokePath(arrow, juce::PathStrokeType(1.8f, juce::PathStrokeType::curved,
                                             juce::PathStrokeType::rounded));
  }
  g.setColour(dragging ? Palette::textPrimary : Palette::textSecondary);
  g.setFont(juce::Font(juce::FontOptions(11.5f)));
  g.drawText(dragging ? "Drop in FL..." : ("Drag " + trackName + " MIDI"),
             textArea, juce::Justification::centredLeft);
}

void DragHandle::mouseDown(const juce::MouseEvent& e) {
  if(regenArea.contains(e.getPosition())) {
    if(onRegenerate) onRegenerate();
  }
}

void DragHandle::mouseDrag(const juce::MouseEvent& e) {
  if(regenArea.contains(e.getMouseDownPosition())) return;
  if(!dragging && e.getDistanceFromDragStart() > 6) {
    dragging = true;
    repaint();
    if(onDrag) onDrag();
    auto& idea = processor.getCurrentIdea();
    SessionIdea solo = idea;
    solo.tracks.clear();
    for(const auto& t : idea.tracks) if(t.name == trackName) solo.tracks.push_back(t);
    if(solo.tracks.empty()) solo = idea;
    auto mf = MidiBuilder::build(solo);
    MidiDrag::startDraggingMidi(this, mf, "MidiForge_" + trackName);
    dragging = false;
    repaint();
  }
}

} // namespace MidiForge
