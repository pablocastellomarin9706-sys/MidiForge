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
  g.setColour(Palette::card);
  g.fillRoundedRectangle(r, 5.0f);
  g.setColour(Palette::border);
  g.drawRoundedRectangle(r, 5.0f, 1.0f);

  regenArea = getLocalBounds().removeFromLeft(100);
  g.setColour(Palette::card.brighter(0.1f));
  g.fillRoundedRectangle(regenArea.toFloat().reduced(2.0f), 4.0f);
  g.setColour(Palette::textPrimary);
  g.setFont(12.0f);
  g.drawText("Regenerate", regenArea, juce::Justification::centred);

  auto dragArea = getLocalBounds().withTrimmedLeft(108);
  g.setColour(dragging ? Palette::accent : Palette::panelAlt);
  g.fillRoundedRectangle(dragArea.toFloat().reduced(2.0f), 4.0f);
  g.setColour(Palette::textPrimary);
  g.setFont(12.0f);
  g.drawText(dragging ? "Drop in FL..." : "Drag MIDI  ->  " + trackName, dragArea, juce::Justification::centred);
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
