#include "PianoRollMini.h"
#include "../LookAndFeel.h"
#include <cmath>

namespace MidiForge {

namespace {
constexpr double kSnap = 0.25; // one 16th note

struct RollLayout {
  juce::Rectangle<float> area;
  int minPitch{60};
  int maxPitch{84};
  double maxBeat{16.0};

  int rows() const { return juce::jmax(1, maxPitch - minPitch + 1); }
  float rowH() const { return area.getHeight() / (float)rows(); }

  float xForBeat(double b) const {
    return area.getX() + (float)(b / maxBeat) * area.getWidth();
  }

  double beatForX(float x) const {
    if (area.getWidth() <= 0.0f) return 0.0;
    const double b = (double)((x - area.getX()) / area.getWidth()) * maxBeat;
    return juce::jlimit(0.0, maxBeat, b);
  }

  float yForPitch(int p) const {
    return area.getBottom() - (float)(p - minPitch + 1) * rowH();
  }

  int pitchForY(float y) const {
    if (area.getHeight() <= 0.0f) return minPitch;
    const int idx = (int)std::floor((area.getBottom() - y) / area.getHeight() * (float)rows());
    return juce::jlimit(minPitch, maxPitch, minPitch + idx);
  }

  juce::Rectangle<float> rectFor(const Note& n) const {
    const float x = xForBeat(n.start);
    const float w = juce::jmax(2.5f, xForBeat(n.start + n.length) - x);
    const float h = juce::jmax(3.0f, rowH() - 1.0f);
    return {x, yForPitch(n.pitch), w, h};
  }
};

RollLayout makeLayoutFor(juce::Rectangle<float> bounds, int minPitch, int maxPitch, double maxBeat) {
  RollLayout L;
  L.area = bounds.reduced(8.0f);
  L.minPitch = minPitch;
  L.maxPitch = juce::jmax(minPitch + 1, maxPitch);
  L.maxBeat = juce::jmax(4.0, maxBeat);
  return L;
}

} // namespace

void PianoRollMini::setTrack(TrackIdea* t, int barCount, bool canEdit) {
  const bool sameTrack = (t == track) && (barCount == bars);
  track = t;
  bars = juce::jmax(1, barCount);
  maxBeat = (double)bars * 4.0;
  editable = canEdit;

  // Keep the vertical mapping stable while the user drags. It is only rebuilt
  // when a different track arrives (generation / Replace), never on refresh.
  if (!sameTrack) {
    int lo = 127, hi = 0;
    if (track != nullptr) {
      for (const auto& n : track->notes) {
        lo = juce::jmin(lo, n.pitch);
        hi = juce::jmax(hi, n.pitch);
      }
    }
    if (hi < lo) { lo = 60; hi = 72; }
    lo -= 3;
    hi += 3;
    if (hi - lo < 16) {
      const int mid = (lo + hi) / 2;
      lo = mid - 8;
      hi = mid + 8;
    }
    minPitch = juce::jlimit(0, 127, lo);
    maxPitch = juce::jlimit(0, 127, hi);
    selected = -1;
    dragIndex = -1;
    undoStack.clear();
  }

  setWantsKeyboardFocus(editable);
  setMouseCursor(editable ? juce::MouseCursor::DraggingHandCursor
                          : juce::MouseCursor::NormalCursor);
  setTooltip(editable
                 ? "Arrastra una nota para moverla, tira del borde derecho para alargarla, "
                   "doble clic para anadir, clic derecho para borrar. Ctrl+Z deshace."
                 : "");
  repaint();
}

int PianoRollMini::noteAt(juce::Point<float> pos) const {
  if (track == nullptr) return -1;
  const auto L = makeLayoutFor(getLocalBounds().toFloat(), minPitch, maxPitch, maxBeat);
  for (int i = (int)track->notes.size() - 1; i >= 0; --i) {
    if (L.rectFor(track->notes[(size_t)i]).expanded(1.0f).contains(pos)) return i;
  }
  return -1;
}

void PianoRollMini::pushUndo() {
  if (track == nullptr) return;
  undoStack.push_back(track->notes);
  if (undoStack.size() > 24) undoStack.erase(undoStack.begin());
}

void PianoRollMini::undo() {
  if (track == nullptr || undoStack.empty()) return;
  track->notes = undoStack.back();
  undoStack.pop_back();
  track->edited = true;
  selected = -1;
  dragIndex = -1;
  if (onEdited) onEdited();
  repaint();
}

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

  if (editable) {
    const bool edited = (track != nullptr && track->edited);
    auto pill = juce::Rectangle<float>(r.getRight() - 82.0f, r.getY() + 7.0f, 74.0f, 16.0f);
    g.setColour(Palette::accent.withAlpha(edited ? 0.30f : 0.11f));
    g.fillRoundedRectangle(pill, 8.0f);
    g.setColour(edited ? Palette::accentSoft : Palette::accentDim);
    g.setFont(juce::Font(juce::FontOptions(9.5f, juce::Font::bold)));
    g.drawText(edited ? "EDITADO" : "EDITABLE", pill, juce::Justification::centred);
  }

  const auto L = makeLayoutFor(getLocalBounds().toFloat(), minPitch, maxPitch, maxBeat);
  const auto area = L.area;
  const double gridEnd = juce::jmax(4.0, maxBeat);

  // Vertical grid: 16th / beat / bar lines.
  for (double b = 0.0; b <= gridEnd + 1e-6; b += 0.25) {
    const float x = (float)(area.getX() + (b / gridEnd) * area.getWidth());
    const bool bar  = std::abs(std::fmod(b, 4.0)) < 1e-6;
    const bool beat = std::abs(std::fmod(b, 1.0)) < 1e-6;
    g.setColour(Palette::border.withAlpha(bar ? 0.55f : (beat ? 0.32f : 0.13f)));
    g.drawVerticalLine((int)x, area.getY(), area.getBottom());
  }

  // Octave rows (C lines).
  for (int p = minPitch; p <= maxPitch; ++p) {
    if (p % 12 == 0) {
      const float y = L.yForPitch(p);
      g.setColour(Palette::border.withAlpha(0.30f));
      g.drawHorizontalLine((int)y, area.getX(), area.getRight());
    }
  }

  if (track == nullptr || track->notes.empty()) {
    g.setColour(Palette::textSecondary);
    g.setFont(13.0f);
    g.drawText(editable ? "Sin notas: doble clic para anadir"
                        : "No notes yet",
               getLocalBounds(), juce::Justification::centred);
    return;
  }

  // Notes: steel ramp across pitch classes, brighter with velocity.
  for (size_t i = 0; i < track->notes.size(); ++i) {
    const auto& n = track->notes[i];
    auto nr = L.rectFor(n);
    auto c = noteColour(n.pitch, (float)n.velocity / 127.0f);

    g.setColour(c.withAlpha(0.95f));
    g.fillRoundedRectangle(nr, 2.5f);

    // Cold light catching the top edge.
    g.setColour(juce::Colours::white.withAlpha(0.25f * ((float)n.velocity / 127.0f)));
    g.drawHorizontalLine((int)nr.getY() + 1, nr.getX() + 2.0f,
                         juce::jmax(nr.getX() + 2.0f, nr.getRight() - 2.0f));

    if ((int)i == selected) {
      g.setColour(juce::Colours::white.withAlpha(0.90f));
      g.drawRoundedRectangle(nr.expanded(0.8f), 2.5f, 1.2f);
    }
  }
}

void PianoRollMini::mouseDown(const juce::MouseEvent& e) {
  if (!editable || track == nullptr) return;
  grabKeyboardFocus();

  const int idx = noteAt(e.position);

  if (e.mods.isPopupMenu() || e.mods.isAltDown()) {
    if (idx >= 0) {
      pushUndo();
      track->notes.erase(track->notes.begin() + idx);
      track->edited = true;
      selected = -1;
      dragIndex = -1;
      if (onEdited) onEdited();
      repaint();
    }
    return;
  }

  if (idx < 0) {
    selected = -1;
    dragIndex = -1;
    repaint();
    return;
  }

  selected = idx;
  dragIndex = idx;
  undoPushed = false;
  moved = false;

  const auto L = makeLayoutFor(getLocalBounds().toFloat(), minPitch, maxPitch, maxBeat);
  const auto& n = track->notes[(size_t)idx];
  const auto nr = L.rectFor(n);
  resizing = e.position.x > nr.getRight() - 6.0f;
  pressBeat = L.beatForX(e.position.x);
  pressPitch = L.pitchForY(e.position.y);
  origStart = n.start;
  origLength = n.length;
  origPitch = n.pitch;
  repaint();
}

void PianoRollMini::mouseDrag(const juce::MouseEvent& e) {
  if (dragIndex < 0 || track == nullptr || dragIndex >= (int)track->notes.size()) return;
  auto& n = track->notes[(size_t)dragIndex];
  const auto L = makeLayoutFor(getLocalBounds().toFloat(), minPitch, maxPitch, maxBeat);

  if (!undoPushed) {
    // Snapshot only when the note actually starts changing, so a plain click
    // does not flood the undo history.
    pushUndo();
    undoPushed = true;
  }

  if (resizing) {
    const double endBeat = std::round(L.beatForX(e.position.x) / kSnap) * kSnap;
    const double maxLen = juce::jmax(kSnap, L.maxBeat - n.start);
    n.length = juce::jlimit(kSnap, maxLen, endBeat - n.start);
  } else {
    const int pitch = L.pitchForY(e.position.y);
    n.pitch = juce::jlimit(0, 127, origPitch + (pitch - pressPitch));

    const double want = origStart + (L.beatForX(e.position.x) - pressBeat);
    const double snapped = std::round(want / kSnap) * kSnap;
    const double maxStart = juce::jmax(0.0, L.maxBeat - n.length);
    n.start = juce::jlimit(0.0, maxStart, snapped);
  }

  moved = true;
  repaint();
}

void PianoRollMini::mouseUp(const juce::MouseEvent&) {
  if (dragIndex >= 0 && moved && track != nullptr) {
    track->edited = true;
    if (onEdited) onEdited();
  }
  dragIndex = -1;
  moved = false;
  resizing = false;
  undoPushed = false;
  repaint();
}

void PianoRollMini::mouseDoubleClick(const juce::MouseEvent& e) {
  if (!editable || track == nullptr) return;
  const auto L = makeLayoutFor(getLocalBounds().toFloat(), minPitch, maxPitch, maxBeat);
  if (!L.area.contains(e.position)) return;
  if (noteAt(e.position) >= 0) return;

  pushUndo();
  Note n{};
  n.pitch = L.pitchForY(e.position.y);
  n.start = std::floor(L.beatForX(e.position.x) / kSnap) * kSnap;
  n.length = kSnap;
  n.velocity = 100;
  n.channel = track->channel;
  if (n.start + n.length > L.maxBeat)
    n.start = juce::jmax(0.0, L.maxBeat - n.length);

  track->notes.push_back(n);
  track->edited = true;
  selected = (int)track->notes.size() - 1;
  if (onEdited) onEdited();
  repaint();
}

bool PianoRollMini::keyPressed(const juce::KeyPress& k) {
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
