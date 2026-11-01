#include "DrumGrid.h"
#include "../LookAndFeel.h"
#include <cmath>

namespace MidiForge {

namespace {

constexpr int kNumLanes = 10;
constexpr int kSteps = 16;
constexpr float kLabelW = 52.0f;

struct LaneDef {
  int pitch;
  const char* label;
  int velocity;
  double length;
  juce::Colour colour;
};

const LaneDef& laneDef(int i) {
  static const LaneDef lanes[kNumLanes] = {
      {36, "KICK", 118, 0.12, Palette::accent},
      {38, "SNR", 112, 0.12, Palette::hot},
      {39, "CLAP", 106, 0.12, Palette::hot.brighter(0.18f)},
      {37, "RIM", 64, 0.10, Palette::textSecondary},
      {42, "HH", 82, 0.10, Palette::cool},
      {46, "OH", 86, 0.28, Palette::cool.brighter(0.18f)},
      {49, "CRSH", 104, 0.45, Palette::star},
      {45, "T1", 100, 0.20, Palette::accentSoft},
      {47, "T2", 102, 0.20, Palette::accentSoft.brighter(0.10f)},
      {50, "T3", 104, 0.20, Palette::accentSoft.brighter(0.18f)}};
  return lanes[juce::jlimit(0, kNumLanes - 1, i)];
}

} // namespace

juce::Rectangle<float> DrumGrid::tabsArea() const {
  auto r = getLocalBounds().toFloat().reduced(8.0f);
  return r.removeFromTop(18.0f);
}

juce::Rectangle<float> DrumGrid::gridArea() const {
  auto r = getLocalBounds().toFloat().reduced(8.0f);
  r.removeFromTop(26.0f);
  const float rowH = juce::jlimit(16.0f, 32.0f, r.getHeight() / (float)kNumLanes);
  r.setHeight(rowH * (float)kNumLanes);
  return r;
}

void DrumGrid::setTrack(TrackIdea* t, int barCount, bool canEdit) {
  if (t != track) undoStack.clear();
  track = t;
  bars = juce::jmax(1, barCount);
  editable = canEdit;
  currentBar = juce::jlimit(0, bars - 1, currentBar);
  setWantsKeyboardFocus(editable);
  setMouseCursor(editable ? juce::MouseCursor::PointingHandCursor
                          : juce::MouseCursor::NormalCursor);
  setTooltip(editable ? "Clic o arrastra para activar/quitar golpes. Ctrl+Z deshace. "
                        "Las pestanas numeradas cambian de compas."
                      : "");
  painting = false;
  repaint();
}

void DrumGrid::paint(juce::Graphics& g) {
  auto r = getLocalBounds().toFloat();
  const float radius = 8.0f;

  // Steel card.
  juce::ColourGradient card(Palette::panelAlt.brighter(0.05f), r.getX(), r.getY(),
                            Palette::panelAlt.darker(0.25f), r.getX(), r.getBottom(), false);
  g.setGradientFill(card);
  g.fillRoundedRectangle(r, radius);
  g.setColour(Palette::border);
  g.drawRoundedRectangle(r.reduced(0.5f), radius, 1.0f);

  const auto area = gridArea();
  const auto tabs = tabsArea();

  // Bar tabs (only when there is more than one bar).
  if (bars > 1) {
    float x = tabs.getX();
    for (int b = 0; b < bars; ++b) {
      auto tr = juce::Rectangle<float>(x, tabs.getY(), 24.0f, tabs.getHeight());
      const bool sel = (b == currentBar);
      const bool bSection = (bars >= 8 && b >= bars / 2);
      g.setColour(sel ? Palette::accent.withAlpha(0.90f)
                      : (bSection ? Palette::accentDim.withAlpha(0.55f) : Palette::panelAlt));
      g.fillRoundedRectangle(tr, 4.0f);
      g.setColour(sel ? juce::Colours::white.withAlpha(0.90f) : Palette::textSecondary);
      g.setFont(juce::Font(juce::FontOptions(10.0f, juce::Font::bold)));
      g.drawText(juce::String(b + 1), tr, juce::Justification::centred);
      x += 26.0f;
    }
    if (x + 8.0f < tabs.getRight() - 110.0f) {
      g.setColour(Palette::textSecondary.withAlpha(0.65f));
      g.setFont(juce::Font(juce::FontOptions(10.0f)));
      g.drawText("clic: activar / quitar",
                 tabs.withTrimmedLeft(x - tabs.getX() + 8.0f),
                 juce::Justification::centredRight);
    }
  } else {
    g.setColour(Palette::textSecondary.withAlpha(0.65f));
    g.setFont(juce::Font(juce::FontOptions(10.0f)));
    g.drawText("clic: activar / quitar", tabs, juce::Justification::centredLeft);
  }

  const float cellW = (area.getWidth() - kLabelW) / (float)kSteps;
  const float rowH = area.getHeight() / (float)kNumLanes;

  // Lane rows and empty cells.
  for (int lane = 0; lane < kNumLanes; ++lane) {
    const auto& L = laneDef(lane);
    auto row = juce::Rectangle<float>(area.getX(), area.getY() + (float)lane * rowH,
                                      area.getWidth(), rowH);
    if (lane % 2 == 1) {
      g.setColour(Palette::panelAlt.withAlpha(0.28f));
      g.fillRect(row);
    }

    auto lab = row.removeFromLeft(kLabelW);
    g.setColour(L.colour.withAlpha(0.85f));
    g.setFont(juce::Font(juce::FontOptions(10.5f, juce::Font::bold)));
    g.drawText(L.label, lab.withTrimmedLeft(6.0f), juce::Justification::centredLeft);

    for (int s = 0; s < kSteps; ++s) {
      auto cell = juce::Rectangle<float>(row.getX() + (float)s * cellW + 1.5f,
                                         row.getY() + 2.0f, cellW - 3.0f, rowH - 4.0f);
      const bool onBeat = (s % 4 == 0);
      g.setColour(Palette::panel.withAlpha(onBeat ? 0.90f : 0.55f));
      g.fillRoundedRectangle(cell, 2.5f);
      g.setColour(Palette::border.withAlpha(onBeat ? 0.55f : 0.28f));
      g.drawRoundedRectangle(cell, 2.5f, 0.8f);
    }
  }

  if (track == nullptr) {
    g.setColour(Palette::textSecondary);
    g.setFont(13.0f);
    g.drawText("No drums yet", getLocalBounds(), juce::Justification::centred);
    return;
  }

  // Hits of the visible bar.
  for (const auto& n : track->notes) {
    if (n.start < 0.0 || n.start >= (double)bars * 4.0) continue;
    const int bar = (int)std::floor(n.start / 4.0);
    if (bar != currentBar) continue;

    int lane = -1;
    for (int i = 0; i < kNumLanes; ++i)
      if (laneDef(i).pitch == n.pitch) { lane = i; break; }
    if (lane < 0) continue;

    const double local = n.start - (double)currentBar * 4.0;
    int step = (int)std::floor(local / 0.25 + 0.5);
    step = juce::jlimit(0, kSteps - 1, step);

    const float x = area.getX() + kLabelW + (float)step * cellW + 1.5f;
    const float y = area.getY() + (float)lane * rowH + 2.0f;
    auto cell = juce::Rectangle<float>(x, y, cellW - 3.0f, rowH - 4.0f);

    const auto& L = laneDef(lane);
    g.setColour(L.colour.withAlpha(0.35f + 0.60f * ((float)n.velocity / 127.0f)));
    g.fillRoundedRectangle(cell, 2.5f);
    g.setColour(juce::Colours::white.withAlpha(0.28f));
    g.drawRoundedRectangle(cell.reduced(0.6f), 2.5f, 0.8f);
  }
}

int DrumGrid::barTabAt(juce::Point<float> pos) const {
  if (bars <= 1) return -1;
  const auto tabs = tabsArea();
  for (int b = 0; b < bars; ++b) {
    auto tr = juce::Rectangle<float>(tabs.getX() + (float)b * 26.0f, tabs.getY(),
                                     24.0f, tabs.getHeight());
    if (tr.contains(pos)) return b;
  }
  return -1;
}

bool DrumGrid::cellAt(juce::Point<float> pos, int& lane, int& step) const {
  const auto area = gridArea();
  if (!area.contains(pos)) return false;
  if (pos.x < area.getX() + kLabelW) return false;

  const float cellW = (area.getWidth() - kLabelW) / (float)kSteps;
  const float rowH = area.getHeight() / (float)kNumLanes;
  const int s = (int)((pos.x - (area.getX() + kLabelW)) / cellW);
  const int l = (int)((pos.y - area.getY()) / rowH);
  if (s < 0 || s >= kSteps || l < 0 || l >= kNumLanes) return false;
  lane = l;
  step = s;
  return true;
}

void DrumGrid::pushUndo() {
  if (track == nullptr) return;
  undoStack.push_back(track->notes);
  if (undoStack.size() > 24) undoStack.erase(undoStack.begin());
}

void DrumGrid::undo() {
  if (track == nullptr || undoStack.empty()) return;
  track->notes = undoStack.back();
  undoStack.pop_back();
  track->edited = true;
  if (onEdited) onEdited();
  repaint();
}

bool DrumGrid::cellHasHit(int lane, int step) const {
  if (track == nullptr) return false;
  const auto& L = laneDef(lane);
  const double barStart = (double)currentBar * 4.0;
  for (const auto& n : track->notes) {
    if (n.pitch != L.pitch || n.start < barStart || n.start >= barStart + 4.0) continue;
    int s = (int)std::floor((n.start - barStart) / 0.25 + 0.5);
    s = juce::jlimit(0, kSteps - 1, s);
    if (s == step) return true;
  }
  return false;
}

bool DrumGrid::applyCell(int lane, int step, bool add) {
  if (track == nullptr) return false;
  const auto& L = laneDef(lane);
  const double barStart = (double)currentBar * 4.0;
  auto& notes = track->notes;

  if (!add) {
    bool removed = false;
    for (auto it = notes.begin(); it != notes.end(); ) {
      if (it->pitch == L.pitch && it->start >= barStart && it->start < barStart + 4.0) {
        int s = (int)std::floor((it->start - barStart) / 0.25 + 0.5);
        s = juce::jlimit(0, kSteps - 1, s);
        if (s == step) {
          it = notes.erase(it);
          removed = true;
          continue;
        }
      }
      ++it;
    }
    return removed;
  }

  if (cellHasHit(lane, step)) return false;

  Note n{};
  n.pitch = L.pitch;
  n.start = barStart + (double)step * 0.25;
  n.length = L.length;
  n.velocity = L.velocity;
  n.channel = track->channel;
  notes.push_back(n);
  return true;
}

void DrumGrid::mouseDown(const juce::MouseEvent& e) {
  if (track == nullptr) return;

  const int bar = barTabAt(e.position);
  if (bar >= 0) {
    currentBar = bar;
    repaint();
    return;
  }

  if (!editable) return;

  int lane = 0, step = 0;
  if (!cellAt(e.position, lane, step)) return;

  grabKeyboardFocus();
  pushUndo();
  paintAdd = !cellHasHit(lane, step);
  painting = true;
  dragChanged = false;
  lastLane = lane;
  lastStep = step;

  if (applyCell(lane, step, paintAdd)) {
    dragChanged = true;
    track->edited = true;
    repaint();
  }
}

void DrumGrid::mouseDrag(const juce::MouseEvent& e) {
  if (!painting || track == nullptr) return;

  int lane = 0, step = 0;
  if (!cellAt(e.position, lane, step)) return;
  if (lane == lastLane && step == lastStep) return;

  lastLane = lane;
  lastStep = step;
  if (applyCell(lane, step, paintAdd)) {
    dragChanged = true;
    track->edited = true;
    repaint();
  }
}

void DrumGrid::mouseUp(const juce::MouseEvent&) {
  if (!painting) return;
  painting = false;
  lastLane = -1;
  lastStep = -1;
  if (dragChanged) {
    if (onEdited) onEdited();
  } else if (!undoStack.empty()) {
    undoStack.pop_back(); // nothing actually changed: drop the snapshot
  }
  dragChanged = false;
}

bool DrumGrid::keyPressed(const juce::KeyPress& k) {
  if (k.getModifiers().isCommandDown()) {
    const auto ch = k.getTextCharacter();
    if (ch == 'z' || ch == 'Z') {
      undo();
      return true;
    }
  }
  return false;
}

} // namespace MidiForge
