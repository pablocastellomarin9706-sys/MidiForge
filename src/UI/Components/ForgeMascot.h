#pragma once

#include <functional>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

namespace MidiForge {

// Animated forge mascot: a molten badge with an anvil and a hammer that strikes
// on its own and throws sparks. Clicking it forges a new idea (same as
// GENERATE). It only repaints its own small area, so the animation is cheap.
class ForgeMascot : public juce::Component, private juce::Timer {
public:
  ForgeMascot();
  ~ForgeMascot() override = default;

  void paint(juce::Graphics&) override;
  void mouseEnter(const juce::MouseEvent&) override;
  void mouseExit(const juce::MouseEvent&) override;
  void mouseDown(const juce::MouseEvent&) override;

  // Replays the strike animation (sparks included). Called when the idea is
  // regenerated from another control so the mascot reacts to it.
  void strike();

  std::function<void()> onClick;

private:
  struct Spark {
    juce::Point<float> pos;
    juce::Point<float> vel;
    float life{0.0f};
    float maxLife{1.0f};
    float size{1.0f};
    juce::Colour colour;
  };

  void timerCallback() override;
  void burst(int count);
  float hammerAngle(double t) const;

  std::vector<Spark> sparks;
  juce::Random random;
  double lastTick{0.0};
  double sinceStrike{10.0};
  bool impactDone{true};
  bool hover{false};
  float flash{0.0f};
  float glowPhase{0.0f};

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ForgeMascot)
};

} // namespace MidiForge
