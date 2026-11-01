#include "ForgeMascot.h"
#include "../LookAndFeel.h"

#include <cmath>

namespace MidiForge {

namespace {

// The mascot is drawn in a 30x30 design space and scaled to its bounds, so it
// stays crisp at any size the header gives it.
constexpr float kDesign = 30.0f;
constexpr float kHammerRest = 0.10f;    // head hovering above the anvil
constexpr float kHammerRaised = 0.38f;  // anticipation
constexpr float kHammerStrike = -0.24f; // impact on the anvil face
constexpr double kImpactAt = 0.28;      // seconds into the swing
constexpr double kIdleStrike = 3.4;     // seconds between autonomous strikes
constexpr float kGravity = 240.0f;

juce::Rectangle<float> badgeAreaFor(juce::Rectangle<float> bounds) {
  const float size = juce::jmin(34.0f, juce::jmin(bounds.getWidth(), bounds.getHeight()) - 6.0f);
  return juce::Rectangle<float>(size, size).withCentre(bounds.getCentre());
}

juce::AffineTransform designTransformFor(juce::Rectangle<float> badge) {
  return juce::AffineTransform::translation(badge.getX(), badge.getY())
      .scaled(badge.getWidth() / kDesign, badge.getHeight() / kDesign);
}

juce::Path anvilPath() {
  juce::Path p;
  p.startNewSubPath(6.5f, 15.0f);
  p.lineTo(9.0f, 13.6f);
  p.lineTo(23.5f, 13.6f);
  p.lineTo(25.0f, 15.0f);
  p.lineTo(25.0f, 17.0f);
  p.lineTo(19.0f, 17.0f);
  p.lineTo(19.0f, 19.8f);
  p.lineTo(23.0f, 19.8f);
  p.lineTo(23.0f, 24.0f);
  p.lineTo(7.0f, 24.0f);
  p.lineTo(7.0f, 19.8f);
  p.lineTo(11.5f, 19.8f);
  p.lineTo(11.5f, 17.0f);
  p.lineTo(6.5f, 17.0f);
  p.closeSubPath();
  return p;
}

} // namespace

ForgeMascot::ForgeMascot() {
  setName("Forge: idea nueva");
  setMouseCursor(juce::MouseCursor::PointingHandCursor);
  setTooltip("Forja una idea nueva (igual que GENERATE)");
  random.setSeed((juce::int64)juce::Time::currentTimeMillis());
  startTimerHz(30);
}

void ForgeMascot::strike() {
  sinceStrike = 0.0;
  impactDone = false;
  repaint();
}

void ForgeMascot::burst(int count) {
  const auto badge = badgeAreaFor(getLocalBounds().toFloat());
  const auto xf = designTransformFor(badge);
  const auto origin = juce::Point<float>(10.0f, 14.0f).transformedBy(xf);

  for (int i = 0; i < count; ++i) {
    Spark sp;
    sp.pos = origin + juce::Point<float>(random.nextFloat() * 4.0f - 2.0f,
                                         random.nextFloat() * 2.0f - 1.0f);
    const float angle = -juce::MathConstants<float>::halfPi + (random.nextFloat() - 0.5f) * 2.0f;
    const float speed = 24.0f + random.nextFloat() * 68.0f;
    sp.vel = juce::Point<float>(std::cos(angle) * speed, std::sin(angle) * speed);
    sp.maxLife = 0.30f + random.nextFloat() * 0.40f;
    sp.life = sp.maxLife;
    sp.size = 1.0f + random.nextFloat() * 1.4f;
    const float pick = random.nextFloat();
    sp.colour = pick < 0.25f ? juce::Colours::white
                             : (pick < 0.60f ? Palette::accentSoft : Palette::accent);
    sparks.push_back(sp);
  }
  while (sparks.size() > 80) sparks.erase(sparks.begin());
}

float ForgeMascot::hammerAngle(double t) const {
  auto mix = [](float a, float b, float p) { return a + (b - a) * juce::jlimit(0.0f, 1.0f, p); };
  if (t < 0.16) {
    const float p = (float)(t / 0.16);
    return mix(kHammerRest, kHammerRaised, p * p);           // raise
  }
  if (t < kImpactAt) {
    const float p = (float)((t - 0.16) / (kImpactAt - 0.16));
    return mix(kHammerRaised, kHammerStrike, p * p);         // slam
  }
  if (t < 0.85) {
    const float p = (float)((t - kImpactAt) / (0.85 - kImpactAt));
    const float eased = 1.0f - (1.0f - p) * (1.0f - p);
    return mix(kHammerStrike, kHammerRest, eased);           // recover
  }
  return kHammerRest;
}

void ForgeMascot::timerCallback() {
  const double now = juce::Time::getMillisecondCounterHiRes() * 0.001;
  const float dt = lastTick > 0.0 ? (float)juce::jlimit(0.0, 0.12, now - lastTick) : (1.0f / 30.0f);
  lastTick = now;
  if (!isShowing()) return; // nothing to animate while the editor is hidden

  glowPhase += dt * 2.1f;
  if (glowPhase > juce::MathConstants<float>::twoPi)
    glowPhase -= juce::MathConstants<float>::twoPi;

  sinceStrike += dt;
  if (sinceStrike >= kIdleStrike) {
    sinceStrike = 0.0;
    impactDone = false;
  }
  if (!impactDone && sinceStrike >= kImpactAt) {
    impactDone = true;
    flash = 1.0f;
    burst(9);
  }
  flash = juce::jmax(0.0f, flash - dt * 3.4f);

  for (auto& sp : sparks) {
    sp.life -= dt;
    sp.pos += sp.vel * dt;
    sp.vel.y += kGravity * dt;
  }
  for (int i = (int)sparks.size() - 1; i >= 0; --i)
    if (sparks[(size_t)i].life <= 0.0f) sparks.erase(sparks.begin() + i);

  repaint();
}

void ForgeMascot::paint(juce::Graphics& g) {
  const auto bounds = getLocalBounds().toFloat();
  const auto badge = badgeAreaFor(bounds);
  const auto xf = designTransformFor(badge);
  const float s = badge.getWidth() / kDesign;

  // Warm halo: breathes gently, brighter on hover.
  const float breathe = 0.5f + 0.5f * std::sin(glowPhase);
  const auto halo = badge.expanded(3.0f);
  juce::ColourGradient haloGrad(
      Palette::accent.withAlpha(0.18f + 0.10f * breathe + (hover ? 0.10f : 0.0f)),
      halo.getCentreX(), halo.getCentreY(), Palette::accent.withAlpha(0.0f), halo.getCentreX(),
      halo.getY(), true);
  g.setGradientFill(haloGrad);
  g.fillEllipse(halo);

  // Molten badge.
  const auto badgePath = chamferRect(badge, 7.0f * s);
  g.setGradientFill(juce::ColourGradient(Palette::accentSoft, badge.getX(), badge.getY(),
                                         Palette::hot, badge.getRight(), badge.getBottom(), false));
  g.fillPath(badgePath);
  g.setColour(juce::Colours::white.withAlpha(0.26f));
  g.strokePath(badgePath, juce::PathStrokeType(1.0f));
  if (hover) {
    g.setColour(Palette::accentSoft.withAlpha(0.90f));
    g.strokePath(chamferRect(badge.expanded(1.6f), 8.2f * s), juce::PathStrokeType(1.2f));
  }

  // Hot top glint.
  g.setColour(juce::Colours::white.withAlpha(0.38f));
  g.drawLine(badge.getX() + 7.0f * s, badge.getY() + 2.0f * s, badge.getRight() - 7.0f * s,
             badge.getY() + 2.0f * s, 1.0f);

  // Anvil.
  auto anvil = anvilPath();
  anvil.applyTransform(xf);
  g.setColour(Palette::background);
  g.fillPath(anvil);
  g.setColour(Palette::textSecondary.withAlpha(0.22f));
  g.drawLine(juce::Line<float>(juce::Point<float>(9.2f, 14.1f).transformedBy(xf),
                               juce::Point<float>(23.2f, 14.1f).transformedBy(xf)),
             1.0f * s);

  // Hammer: handle + head swing around the grip.
  const float angle = hammerAngle(sinceStrike);
  const auto rot = juce::AffineTransform::rotation(angle, 25.5f, 24.0f);
  const auto headCentre = juce::Point<float>(12.0f, 10.0f).transformedBy(rot).transformedBy(xf);
  const auto grip = juce::Point<float>(25.5f, 24.0f).transformedBy(rot).transformedBy(xf);
  g.setColour(Palette::background.withAlpha(0.85f));
  g.drawLine(juce::Line<float>(grip, headCentre), 3.8f * s);
  g.setColour(Palette::textSecondary);
  g.drawLine(juce::Line<float>(grip, headCentre), 2.0f * s);

  juce::Path head;
  head.addRoundedRectangle(12.0f - 4.5f, 10.0f - 3.0f, 9.0f, 6.0f, 1.6f);
  head.applyTransform(rot);
  head.applyTransform(xf);
  g.setColour(Palette::textPrimary.withAlpha(0.92f));
  g.fillPath(head);
  g.setColour(Palette::background.withAlpha(0.55f));
  g.strokePath(head, juce::PathStrokeType(0.9f * s));

  // Impact flash on the anvil face.
  if (flash > 0.01f) {
    const auto centre = juce::Point<float>(10.0f, 14.0f).transformedBy(xf);
    const float radius = (1.6f + (1.0f - flash) * 7.0f) * s;
    g.setColour(Palette::accentSoft.withAlpha(0.65f * flash));
    g.drawEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f, 1.2f * s);
  }

  // Sparks.
  for (const auto& sp : sparks) {
    const float alpha = juce::jlimit(0.0f, 1.0f, sp.life / sp.maxLife);
    g.setColour(sp.colour.withAlpha(alpha));
    g.fillEllipse(sp.pos.x - sp.size * 0.5f, sp.pos.y - sp.size * 0.5f, sp.size, sp.size);
  }
}

void ForgeMascot::mouseEnter(const juce::MouseEvent&) { hover = true; repaint(); }
void ForgeMascot::mouseExit(const juce::MouseEvent&) { hover = false; repaint(); }

void ForgeMascot::mouseDown(const juce::MouseEvent&) {
  strike();
  if (onClick) onClick();
}

} // namespace MidiForge
