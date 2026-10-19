# Changelog

## 1.0.0 - 2026-10-19

### Added
- **BoomBap content** (the character had none): dedicated drum pattern
  ("BoomBap Backbeat": kick 0/10, backbeat 4/12, ghost snare at 14, swung hats),
  808 root pattern ("BoomBap Root") and the "BoomBap Soul" progression
  (im7 - ivm7 - VImaj7 - IIImaj7). The character also defaults to a minor scale
  now instead of quietly falling back to Major.
- **Delivered-set content**: the JERK / PLUGG / HIP-HOP MIDI entregas are now
  part of the plugin's DNA — "Jerk Bounce" drums + "Jerk 808 Bounce" + "Jerk
  Hook" progression (im7-ivm7-VImaj7-V7); "Plugg Hats 16" drums + "Plugg Long"
  808 + "Plugg Bells" progression (im7-VImaj7-IIImaj7-VII); BoomBap as above.

### Notes
- First release (`1.0.0`): full QA pass (0.6.1) + delivered-set content + docs.
- The plugin stays **100 % offline** by design. The sample workflow (Freesound /
  SampleFocus into your watched folders) is documented in the README.

## 0.6.1 - 2026-10-19

### Fixed
- **Melody transposition**: the generator added the key root twice
  (`getScale` already folds it in), so every non&#8209;C key played the melody
  shifted. Melodies now land exactly in the chosen key, centred at C5.
- **Audio-thread race**: the preview transport in `processBlock` was touched
  by `previewSample`/`stopPreview` (message thread) with no lock. Both sides
  now share a `juce::SpinLock` (short holds; no callbacks under the lock).
- Fresh "seed 12345" stomped a restored idea on editor open; regeneration now
  deterministically reproduces the saved idea instead.
- `RandomGen::getSeed()` never stored the seed (always returned 12345).
- `degreeToSemitone` used the natural-minor table for Dorian/Phrygian/etc.;
  each mode now resolves its exact degrees.

### Added
- **Idea persistence** (state v3): seed/take/bars/key/scale/style/character/
  swing/brightness/creativity plus curated progression/drums/bass/motif picks
  are saved with the plugin state and deterministically rebuilt on reload.
- **Live history** in the sidebar: every generated idea is recorded; clicking
  a row restores the full idea (all musical settings + picks).
- **Multi&#8209;track MIDI drag**: one named MIDI track per instrument, each on
  its own channel (chords 1, melody 2, bass 3, drums 10).
- "Chords" Replace re-picks the bass pattern when style/character changed, so
  the 808 always fits the current selection.
- Defensive guards: melody motif index range, restored-index validation with
  automatic fallback to a fresh pick.

## 0.6.0 - 2026-10-19

### Changed
- New "Forge" visual identity: warm charcoal, molten orange/magenta accents,
  chamfered (anvil-like) geometry, ember-particle background, animated sheen,
  pitch-class rainbow note colours brightened by velocity.

### Added
- Detroit character deepening: octave-down melody thickening layer and the
  per-bar "reverse + gap" retrograde motif (melodyReverse probability).

## 0.5.0 - 2026-10-18

### Added
- Co-producer offline flows (chat parsing, quick chips, Sample Scout search
  opens the provider's site), sample audition through the plugin's output.
- Host BPM read into the sample "fit" filter.
- Character-aware content: dedicated drum patterns, 808s and progressions per
  character, with generic fallbacks; `defaultScaleFor` when picking a minor
  family character from a major idea.
