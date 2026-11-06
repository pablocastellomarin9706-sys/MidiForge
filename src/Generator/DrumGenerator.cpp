#include "DrumGenerator.h"
#include "../Content/Patterns.h"

#include <algorithm>

namespace MidiForge {

namespace {

// One-shots: closed hats are tight, open hats / crashes / toms ring out.
double drumLen(int pitch) {
  switch (pitch) {
    case Content::kCrash:   return 0.50;
    case Content::kOpenHat: return 0.30;
    case Content::kTomLow:
    case Content::kTomMid:
    case Content::kTomHigh: return 0.22;
    default:                return 0.12;
  }
}

double swungStart(const SessionIdea& idea, double base, int step) {
  double start = base + (double)step * 0.25;
  if (idea.swing > 0.0 && (step % 2 == 1)) start += (idea.swing / 100.0) * 0.15;
  return start;
}

void pushHit(TrackIdea& t, const SessionIdea& idea, double base, int step, int pitch, int vel,
             RandomGen& rng) {
  t.notes.push_back({pitch, swungStart(idea, base, step), drumLen(pitch),
                     juce::jlimit(1, 127, vel + rng.nextInt(-5, 5)), 10});
}

// Small per-bar touches so the loop breathes instead of repeating identically.
// Each style gets its own vocabulary (ghosts, syncopated kicks, open hats).
void addBarVariation(TrackIdea& t, const SessionIdea& idea, double base, RandomGen& rng) {
  const double p = rng.nextDouble();
  switch (idea.style) {
    case Style::House:
    case Style::EDM:
      if (p < 0.45)      pushHit(t, idea, base, 14, Content::kOpenHat, 86, rng);
      else if (p < 0.75) { pushHit(t, idea, base, 7, Content::kHat, 64, rng);
                           pushHit(t, idea, base, 15, Content::kHat, 70, rng); }
      else               pushHit(t, idea, base, 15, Content::kClap, 60, rng);
      break;
    case Style::HipHop:
      if (p < 0.35)      pushHit(t, idea, base, 15, Content::kSnare, 44, rng);
      else if (p < 0.65) pushHit(t, idea, base, 11, Content::kKick, 92, rng);
      else if (p < 0.85) pushHit(t, idea, base, 14, Content::kRim, 54, rng);
      else               pushHit(t, idea, base, 3, Content::kOpenHat, 70, rng);
      break;
    case Style::LoFi:
      if (p < 0.45)      pushHit(t, idea, base, 15, Content::kRim, 46, rng);
      else if (p < 0.75) pushHit(t, idea, base, 7, Content::kKick, 84, rng);
      else               pushHit(t, idea, base, 6, Content::kOpenHat, 58, rng);
      break;
    case Style::Rock:
      if (p < 0.40)      pushHit(t, idea, base, 6, Content::kKick, 96, rng);
      else if (p < 0.70) pushHit(t, idea, base, 15, Content::kHat, 78, rng);
      else               pushHit(t, idea, base, 10, Content::kRim, 64, rng);
      break;
    case Style::Pop:
    default:
      if (p < 0.35)      pushHit(t, idea, base, 7, Content::kHat, 70, rng);
      else if (p < 0.60) pushHit(t, idea, base, 10, Content::kRim, 58, rng);
      else if (p < 0.85) pushHit(t, idea, base, 14, Content::kOpenHat, 78, rng);
      else               pushHit(t, idea, base, 15, Content::kSnare, 48, rng);
      break;
  }
}

// Fill into the loop point, written in the language of the genre: trap snare
// roll, lofi rim ghost, house/EDM build, rock tom fill, pop tom + backbeat.
void addFill(TrackIdea& t, const SessionIdea& idea, double base, RandomGen& rng) {
  switch (idea.style) {
    case Style::HipHop:
      pushHit(t, idea, base, 12, Content::kSnare, 66, rng);
      pushHit(t, idea, base, 13, Content::kSnare, 80, rng);
      pushHit(t, idea, base, 14, Content::kSnare, 96, rng);
      pushHit(t, idea, base, 15, Content::kSnare, 112, rng);
      pushHit(t, idea, base, 15, Content::kRim, 58, rng);
      break;
    case Style::LoFi:
      pushHit(t, idea, base, 13, Content::kRim, 52, rng);
      pushHit(t, idea, base, 14, Content::kSnare, 72, rng);
      pushHit(t, idea, base, 14, Content::kOpenHat, 60, rng);
      pushHit(t, idea, base, 15, Content::kRim, 44, rng);
      break;
    case Style::House:
    case Style::EDM:
      pushHit(t, idea, base, 8,  Content::kSnare, 58, rng);
      pushHit(t, idea, base, 10, Content::kSnare, 70, rng);
      pushHit(t, idea, base, 12, Content::kSnare, 84, rng);
      pushHit(t, idea, base, 13, Content::kSnare, 92, rng);
      pushHit(t, idea, base, 14, Content::kSnare, 104, rng);
      pushHit(t, idea, base, 15, Content::kSnare, 116, rng);
      pushHit(t, idea, base, 15, Content::kOpenHat, 88, rng);
      break;
    case Style::Rock:
      pushHit(t, idea, base, 12, Content::kTomHigh, 96, rng);
      pushHit(t, idea, base, 13, Content::kTomMid, 100, rng);
      pushHit(t, idea, base, 14, Content::kTomLow, 106, rng);
      pushHit(t, idea, base, 15, Content::kSnare, 118, rng);
      break;
    case Style::Pop:
    default:
      pushHit(t, idea, base, 12, Content::kTomHigh, 84, rng);
      pushHit(t, idea, base, 13, Content::kTomMid, 90, rng);
      pushHit(t, idea, base, 14, Content::kSnare, 104, rng);
      pushHit(t, idea, base, 15, Content::kSnare, 114, rng);
      break;
  }
}

} // namespace

void DrumGenerator::generate(SessionIdea& idea, RandomGen& rng) {
  const auto& pats = Content::allDrumPatterns();
  if (idea.drumPatternIdx < 0 || idea.drumPatternIdx >= (int)pats.size()) return;
  const auto& pat = pats[(size_t)idea.drumPatternIdx];

  TrackIdea t;
  t.name = "Drums";
  t.channel = 10;
  t.colour = juce::Colour(255, 124, 92);
  t.detail = pat.name;

  int bars = idea.bars > 0 ? idea.bars : 4;

  // Section B (from the arrangement editor or the auto half) gets a second
  // groove, so a long idea is not the same four bars pasted over and over.
  int secondIdx = -1;
  const int secondStart = idea.bStart();
  if (idea.hasSectionB()) {
    auto cands = Content::drumIndicesFor(idea.style, idea.character);
    if (cands.size() > 1) {
      const int start = rng.nextInt(0, (int)cands.size() - 1);
      for (int k = 1; k <= (int)cands.size(); ++k) {
        const int cand = cands[(size_t)((start + k) % (int)cands.size())];
        if (cand != idea.drumPatternIdx) { secondIdx = cand; break; }
      }
    }
  }

  if (secondIdx >= 0)
    t.detail = pat.name + "  /  " + pats[(size_t)secondIdx].name;

  for (int bar = 0; bar < bars; ++bar) {
    double base = (double)bar * 4.0;
    const bool lastBar = (bar == bars - 1);
    const bool inB = secondIdx >= 0 && bar >= secondStart;
    const auto& active = inB ? pats[(size_t)secondIdx] : pat;

    for (const auto& h : active.hits)
      pushHit(t, idea, base, h.step, h.pitch, h.velocity, rng);

    // Crash into the B section.
    if (bar == secondStart && secondIdx >= 0)
      pushHit(t, idea, base, 0, Content::kCrash, 112, rng);

    // Odd bars get a stylistic touch, so bar 2 is not a clone of bar 1.
    if (bars > 1 && !lastBar && (bar % 2 == 1))
      addBarVariation(t, idea, base, rng);

    // Every 4th bar (and the final bar) gets a fill: 8/16-bar ideas breathe
    // instead of looping a single bar of drums.
    const bool phraseEnd = (bars > 1) && ((bar + 1) % 4 == 0);
    if (bars > 1 && (phraseEnd || lastBar)) {
      // Crash on the downbeat for the styles that use one, then the fill.
      if (lastBar && idea.style != Style::HipHop && idea.style != Style::LoFi)
        pushHit(t, idea, base, 0, Content::kCrash, 108, rng);
      addFill(t, idea, base, rng);
    }
  }

  std::stable_sort(t.notes.begin(), t.notes.end(),
                   [](const Note& a, const Note& b) { return a.start < b.start; });
  idea.tracks.push_back(std::move(t));
}

} // namespace MidiForge
