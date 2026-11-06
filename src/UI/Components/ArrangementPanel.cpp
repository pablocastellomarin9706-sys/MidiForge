#include "ArrangementPanel.h"
#include "../LookAndFeel.h"

namespace MidiForge {

ArrangementPanel::ArrangementPanel(MidiForgeAudioProcessor& p) : processor(p) {
  autoButton.setTooltip("Division automatica: la mitad para ideas de 8 o mas compases, "
                        "sin seccion B en ideas cortas.");
  autoButton.onClick = [this] {
    draggingTo = -999;
    applySplit(-1);
  };
  addAndMakeVisible(autoButton);

  allAButton.setTooltip("Toda la idea es seccion A: sin cambio de progresion ni groove.");
  allAButton.onClick = [this] {
    draggingTo = -999;
    applySplit(processor.getCurrentIdea().totalBars());
  };
  addAndMakeVisible(allAButton);

  setTooltip("Clic o arrastra sobre un compas para que ahi empiece la seccion B.");
}

void ArrangementPanel::applySplit(int split) {
  auto& idea = processor.getCurrentIdea();
  const int total = idea.totalBars();
  const int normalised = (split < 0) ? -1 : juce::jlimit(1, total, split);
  if (idea.splitBar == normalised) return;

  processor.setArrangementSplit(normalised);
  processor.updateHostDisplay();
  refresh();
  if (onApplied) onApplied();
}

juce::Rectangle<float> ArrangementPanel::stripArea() const {
  auto r = getLocalBounds().reduced(16).toFloat();
  r.removeFromTop(52);                       // title + buttons
  const float stripH = juce::jlimit(56.0f, 96.0f, r.getHeight() * 0.45f);
  return r.withHeight(stripH);
}

juce::Rectangle<float> ArrangementPanel::blockRect(int i, int total) const {
  const auto area = stripArea();
  const float gap = 3.0f;
  const float w = (area.getWidth() - gap * (float)(total - 1)) / (float)total;
  return juce::Rectangle<float>(area.getX() + (float)i * (w + gap), area.getY(),
                                w, area.getHeight());
}

int ArrangementPanel::barAt(juce::Point<float> pos) const {
  const auto& idea = processor.getCurrentIdea();
  const int total = idea.totalBars();
  const auto area = stripArea();
  if (total <= 0) return -1;
  if (pos.y < area.getY() - 4.0f || pos.y > area.getBottom() + 4.0f) return -1;
  const float gap = 3.0f;
  const float w = (area.getWidth() - gap * (float)(total - 1)) / (float)total;
  if (pos.x < area.getX() || pos.x > area.getRight()) return -1;
  const int idx = (int)((pos.x - area.getX()) / (w + gap));
  return juce::jlimit(0, total - 1, idx);
}

void ArrangementPanel::mouseDown(const juce::MouseEvent& e) {
  const int idx = barAt(e.position);
  const auto& idea = processor.getCurrentIdea();
  if (idx <= 0 || idx >= idea.totalBars()) return; // A keeps at least one bar
  draggingTo = idx;
  applySplit(idx);
}

void ArrangementPanel::mouseDrag(const juce::MouseEvent& e) {
  const int idx = barAt(e.position);
  const auto& idea = processor.getCurrentIdea();
  if (idx <= 0 || idx >= idea.totalBars()) return;
  if (idx == draggingTo) return;
  draggingTo = idx;
  applySplit(idx);
}

void ArrangementPanel::mouseUp(const juce::MouseEvent&) { draggingTo = -999; }

void ArrangementPanel::refresh() {
  auto& idea = processor.getCurrentIdea();
  const bool isAuto = (idea.splitBar < 0);
  const bool allA = (!isAuto && idea.bStart() >= idea.totalBars());
  autoButton.setToggleState(false, juce::dontSendNotification);
  allAButton.setToggleState(allA, juce::dontSendNotification);
  autoButton.setColour(juce::TextButton::buttonOnColourId,
                       Palette::accentDim.brighter(0.22f));
  allAButton.setColour(juce::TextButton::buttonOnColourId,
                       Palette::accentDim.brighter(0.22f));
  repaint();
}

void ArrangementPanel::paint(juce::Graphics& g) {
  auto r = getLocalBounds().toFloat();

  // Card background (same language as the other panels).
  juce::ColourGradient bg(Palette::panel.brighter(0.02f), r.getX(), r.getY(),
                          Palette::panel.darker(0.12f), r.getX(), r.getBottom(), false);
  g.setGradientFill(bg);
  g.fillRoundedRectangle(r.reduced(1.0f), 8.0f);
  g.setColour(Palette::border);
  g.drawRoundedRectangle(r.reduced(1.5f), 8.0f, 1.0f);

  auto& idea = processor.getCurrentIdea();
  const int total = idea.totalBars();
  const int bStart = idea.bStart();
  const bool hasB = idea.hasSectionB();

  // Title + summary.
  auto title = getLocalBounds().reduced(16).removeFromTop(22);
  g.setFont(juce::Font(juce::FontOptions(13.0f, juce::Font::bold)));
  g.setColour(Palette::accentSoft);
  g.drawText("Arreglo A/B", title, juce::Justification::centredLeft);

  juce::String summary;
  if (!hasB) {
    summary = idea.splitBar < 0 ? "Todo A (auto sin B)" : "Todo A";
  } else {
    summary = "A: c.1-" + juce::String(bStart) +
              "   B: c." + juce::String(bStart + 1) + "-" + juce::String(total);
    summary += idea.splitBar < 0 ? "   (auto)" : "";
  }
  g.setFont(juce::Font(juce::FontOptions(12.0f)));
  g.setColour(Palette::textSecondary);
  g.drawText(summary, title.withTrimmedLeft(150), juce::Justification::centredLeft);

  // Bar blocks.
  const auto area = stripArea();
  for (int i = 0; i < total; ++i) {
    const auto br = blockRect(i, total);
    const bool inB = (hasB && i >= bStart);
    const bool boundary = (hasB && i == bStart);

    if (inB) {
      juce::ColourGradient fillB(Palette::accent.withAlpha(0.34f), br.getX(), br.getY(),
                                 Palette::accentDim.withAlpha(0.45f), br.getX(),
                                 br.getBottom(), false);
      g.setGradientFill(fillB);
    } else {
      g.setColour(Palette::panelAlt);
    }
    g.fillRoundedRectangle(br, 5.0f);

    g.setColour(inB ? Palette::accent.withAlpha(0.85f) : Palette::border);
    g.drawRoundedRectangle(br.reduced(0.5f), 5.0f, 1.0f);

    g.setFont(juce::Font(juce::FontOptions(
        juce::jlimit(8.0f, 11.0f, br.getWidth() * 0.32f), juce::Font::bold)));
    g.setColour(inB ? juce::Colours::white.withAlpha(0.92f) : Palette::textSecondary);
    g.drawText(juce::String(i + 1), br, juce::Justification::centred);

    if (boundary) {
      // "B" badge hanging from the top edge of the boundary block.
      const juce::Rectangle<float> badge(br.getCentreX() - 8.0f, br.getY() - 15.0f, 16.0f, 13.0f);
      g.setColour(Palette::accent);
      g.fillRoundedRectangle(badge, 4.0f);
      g.setFont(juce::Font(juce::FontOptions(9.5f, juce::Font::bold)));
      g.setColour(juce::Colours::black.withAlpha(0.85f));
      g.drawText("B", badge, juce::Justification::centred);
    }
  }

  // Boundary line through the strip.
  if (hasB) {
    const auto bb = blockRect(bStart, total);
    const float xb = bb.getX() - 1.5f;
    g.setColour(Palette::accent.withAlpha(0.70f));
    g.drawVerticalLine((int)xb, area.getY() - 16.0f, area.getBottom() + 4.0f);
  }

  // Hint under the strip.
  auto hintInt = getLocalBounds().reduced(16);
  hintInt.removeFromTop((int)area.getBottom() - getLocalBounds().getY() + 10);
  hintInt.setHeight(40);
  g.setFont(juce::Font(juce::FontOptions(11.0f)));
  g.setColour(Palette::textSecondary.withAlpha(0.85f));
  g.drawText("Clic o arrastra un compas para que ahi empiece la seccion B "
             "(progresion, groove y motivo cambian).  BLOQ protege cada track.",
             hintInt, juce::Justification::centredLeft, true);
}

void ArrangementPanel::resized() {
  auto r = getLocalBounds().reduced(16);
  auto top = r.removeFromTop(24);
  allAButton.setBounds(top.removeFromRight(74).withSizeKeepingCentre(70, 20));
  top.removeFromRight(8);
  autoButton.setBounds(top.removeFromRight(64).withSizeKeepingCentre(60, 20));
}

} // namespace MidiForge
