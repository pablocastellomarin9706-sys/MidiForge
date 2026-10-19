#include "AutoSound.h"

#include <algorithm>
#include <cmath>

namespace MidiForge {

namespace {

juce::String lower(const juce::String& s) { return s.toLowerCase(); }

bool containsAny(const juce::String& text, const std::vector<juce::String>& keys) {
  for (const auto& k : keys)
    if (text.contains(k)) return true;
  return false;
}

// Full loops/stems are not one-shots: keep them out of every pick.
bool isUsableAudio(const SampleItem& it) {
  const auto n = lower(it.name);
  return !n.contains("loop") && !n.contains("break") && !n.contains("stem");
}

// "Fm" -> 5, "C#" -> 1, "Bbm" -> 10, "" -> -1.
int pitchClassFromKey(const juce::String& key) {
  if (key.isEmpty()) return -1;
  const auto s = lower(key);
  int pc = -1;
  switch (s[0]) {
    case 'c': pc = 0; break;
    case 'd': pc = 2; break;
    case 'e': pc = 4; break;
    case 'f': pc = 5; break;
    case 'g': pc = 7; break;
    case 'a': pc = 9; break;
    case 'b': pc = 11; break;
    default: break;
  }
  if (pc < 0) return -1;
  if (s.length() > 1) {
    if (s[1] == '#') pc = (pc + 1) % 12;
    else if (s[1] == 'b') pc = (pc + 11) % 12;
  }
  return pc;
}

bool isPitchedSlot(SoundSlot s) {
  return s == SoundSlot::Melody || s == SoundSlot::Chords || s == SoundSlot::Bass;
}

// Search keywords per slot, in priority tiers: the first tier with matches
// wins. Character tilts the words towards the genre (bells for plugg, piano
// for boom bap, leads for rage...).
std::vector<std::vector<juce::String>> keywordTiers(SoundSlot slot, Character c) {
  using SL = SoundSlot;
  switch (slot) {
    case SL::Melody:
      switch (c) {
        case Character::Plugg:
          return {{"bell", "flute", "guitar", "harp"}, {"pluck", "mallet"}, {"melody", "arp"}};
        case Character::NuJerk:
          return {{"pluck", "bell", "triangle"}, {"flute", "arp"}, {"melody"}};
        case Character::Detroit:
          return {{"detroit", "bell", "piano"}, {"pluck", "strings"}, {"melody", "arp"}};
        case Character::TrapDark:
          return {{"dark", "bell", "pluck"}, {"flute", "melody"}, {"arp"}};
        case Character::BoomBap:
          return {{"piano", "rhodes", "guitar", "organ", "keys", "wurli"}, {"melody", "soul"}, {"arp"}};
        case Character::Rage:
          return {{"lead", "supersaw", "saw", "synth"}, {"melody"}, {"arp"}};
        case Character::TrapLatino:
          return {{"pluck", "guitar", "bell"}, {"flute", "marimba"}, {"melody", "arp"}};
        case Character::None:
        default:
          return {{"bell", "pluck", "lead", "flute"}, {"melody", "key"}, {"arp", "synth"}};
      }
    case SL::Chords:
      switch (c) {
        case Character::BoomBap:
          return {{"rhodes", "piano", "wurli", "organ"}, {"keys", "chord"}, {"pad"}};
        case Character::Rage:
          return {{"pad", "synth", "chord"}, {"lead", "keys"}, {"piano"}};
        case Character::Detroit:
          return {{"piano", "keys", "pad", "rhodes"}, {"chord", "strings"}, {"organ"}};
        default:
          return {{"rhodes", "keys", "chord", "pad"}, {"piano", "organ", "string", "guitar"}, {"synth"}};
      }
    case SL::Bass:
      return {{"808"}, {"sub"}};
    case SL::Kick:
      return {{"kick", "bassdrum", "bass_drum", "bd", "boom"}};
    case SL::Snare:
      return {{"snare", "sd", "snap"}, {"clap"}};
    case SL::Clap:
      return {{"clap", "snap"}, {"snare"}};
    case SL::Hat:
      return {{"closedhat", "closed_hat", "closed", "chh", "hh", "hat"}, {"cym", "perc"}};
    case SL::OpenHat:
      return {{"openhat", "open_hat", "open", "ohh", "oh_"}, {"hat", "hh"}, {"cym", "perc"}};
    case SL::Crash:
      return {{"crash", "cymbal", "cym"}, {"splash", "ride"}, {"hat", "perc"}};
    default:
      return {};
  }
}

std::vector<int> collectCandidates(const std::vector<SampleItem>& lib, SoundSlot slot,
                                   Character c, bool keyPriority, int keyRoot) {
  const auto tiers = keywordTiers(slot, c);
  for (const auto& tier : tiers) {
    std::vector<int> matches;
    std::vector<int> keyMatches;
    for (int i = 0; i < (int)lib.size(); ++i) {
      const auto& it = lib[(size_t)i];
      if (!isUsableAudio(it)) continue;
      const auto n = lower(it.name);
      if (slot == SoundSlot::Hat && n.contains("open")) continue;
      if (!containsAny(n, tier)) continue;
      matches.push_back(i);
      if (keyPriority) {
        const int pc = pitchClassFromKey(it.key);
        if (pc >= 0 && pc == keyRoot) keyMatches.push_back(i);
      }
    }
    if (matches.empty()) continue;
    if (keyPriority && !keyMatches.empty()) return keyMatches;
    return matches;
  }
  return {};
}

int randomPick(const std::vector<int>& cands, int seed) {
  if (cands.empty()) return -1;
  juce::Random rng(seed);
  return cands[(size_t)rng.nextInt((int)cands.size())];
}

float slotGain(SoundSlot s) {
  switch (s) {
    case SoundSlot::Kick: return 0.90f;
    case SoundSlot::Snare: return 0.72f;
    case SoundSlot::Clap: return 0.72f;
    case SoundSlot::Hat: return 0.40f;
    case SoundSlot::OpenHat: return 0.42f;
    case SoundSlot::Crash: return 0.55f;
    case SoundSlot::Bass: return 0.90f;
    case SoundSlot::Chords: return 0.62f;
    case SoundSlot::Melody:
    default: return 0.68f;
  }
}

} // namespace

juce::String soundSlotName(SoundSlot s) {
  switch (s) {
    case SoundSlot::Melody: return "Melodia";
    case SoundSlot::Chords: return "Acordes";
    case SoundSlot::Bass: return "Bajo";
    case SoundSlot::Kick: return "Kick";
    case SoundSlot::Snare: return "Snare";
    case SoundSlot::Clap: return "Clap";
    case SoundSlot::Hat: return "Hat";
    case SoundSlot::OpenHat: return "OpenHat";
    case SoundSlot::Crash: return "Crash";
    default: return "Sonido";
  }
}

SoundSlot slotForDrumPitch(int pitch) {
  switch (pitch) {
    case 36: return SoundSlot::Kick;
    case 37: case 39: return SoundSlot::Clap;
    case 38: case 40: return SoundSlot::Snare;
    case 42: case 44: return SoundSlot::Hat;
    case 46: return SoundSlot::OpenHat;
    case 49: case 57: return SoundSlot::Crash;
    default: break;
  }
  if (pitch <= 36) return SoundSlot::Kick;
  if (pitch <= 40) return SoundSlot::Snare;
  return SoundSlot::Hat;
}

AutoSoundSet pickAutoSounds(const std::vector<SampleItem>& library, Character character,
                            int keyRoot, int seed) {
  AutoSoundSet set;
  for (int s = 0; s < kNumSoundSlots; ++s) {
    const auto slot = (SoundSlot)s;
    auto cands = collectCandidates(library, slot, character, isPitchedSlot(slot), keyRoot);
    const int idx = randomPick(cands, seed + s * 10007);
    if (idx >= 0) {
      set.paths[s] = library[(size_t)idx].path;
      set.names[s] = library[(size_t)idx].name;
    }
  }
  return set;
}

bool pickOneSlot(AutoSoundSet& set, const std::vector<SampleItem>& library,
                 Character character, int keyRoot, SoundSlot slot, int seed,
                 const juce::String& avoidPath) {
  auto cands = collectCandidates(library, slot, character, isPitchedSlot(slot), keyRoot);
  if (cands.size() > 1 && avoidPath.isNotEmpty()) {
    std::vector<int> filtered;
    filtered.reserve(cands.size());
    for (int i : cands)
      if (library[(size_t)i].path != avoidPath) filtered.push_back(i);
    if (!filtered.empty()) cands.swap(filtered);
  }
  const int idx = randomPick(cands, seed);
  if (idx < 0) return false;
  set.paths[(int)slot] = library[(size_t)idx].path;
  set.names[(int)slot] = library[(size_t)idx].name;
  return true;
}

juce::File renderIdeaToWav(const SessionIdea& idea, const juce::String& trackName,
                           const AutoSoundSet& sounds, double bpm, bool mixAll) {
  if (bpm < 20.0 || bpm > 400.0) bpm = 120.0;
  const double sr = 44100.0;
  const double secPerBeat = 60.0 / bpm;

  struct PendingNote {
    const Note* note;
    int slot;
    bool pitched;
  };

  // Pass 1: what will sound (drums via GM pitch, melodic layers via channel).
  std::vector<PendingNote> pending;
  std::vector<int> slotPitches[kNumSoundSlots];
  double lastBeat = 0.0;
  for (const auto& t : idea.tracks) {
    if (t.mute) continue;
    if (!mixAll && t.name != trackName) continue;
    for (const auto& n : t.notes) {
      int slot = (int)SoundSlot::Melody;
      bool pitched = true;
      if (n.channel == 10) {
        slot = (int)slotForDrumPitch(n.pitch);
        pitched = false;
      } else if (n.channel == 1) {
        slot = (int)SoundSlot::Chords;
      } else if (n.channel == 3) {
        slot = (int)SoundSlot::Bass;
      }
      pending.push_back({ &n, slot, pitched });
      if (pitched) slotPitches[slot].push_back(n.pitch);
      lastBeat = juce::jmax(lastBeat, n.start + n.length);
    }
  }
  if (pending.empty()) return {};

  // Pass 2: load the assigned files. Root pitch comes from the sample's key
  // (when the name carries one) with the octave taken from the notes it has to
  // cover, so the pitch-shift stays small and harmonic.
  juce::AudioFormatManager fm;
  fm.registerBasicFormats();
  juce::AudioBuffer<float> slotBuf[kNumSoundSlots];
  bool slotOk[kNumSoundSlots] = {};
  double slotRate[kNumSoundSlots] = {};
  int slotRoot[kNumSoundSlots];
  for (int s = 0; s < kNumSoundSlots; ++s) slotRoot[s] = 60;

  for (int s = 0; s < kNumSoundSlots; ++s) {
    if (sounds.paths[s].isEmpty()) continue;
    juce::File f(sounds.paths[s]);
    if (!f.existsAsFile()) continue;
    std::unique_ptr<juce::AudioFormatReader> reader(fm.createReaderFor(f));
    if (reader == nullptr || reader->lengthInSamples <= 0) continue;
    const int len = (int)juce::jmin<juce::int64>(reader->lengthInSamples,
                                                 (juce::int64)(sr * 30.0));
    const int numCh = juce::jmax(1, (int)reader->numChannels);
    juce::AudioBuffer<float> buf(numCh, len);
    reader->read(&buf, 0, len, 0, true, true);
    slotBuf[s] = std::move(buf);
    slotOk[s] = true;
    slotRate[s] = reader->sampleRate > 0.0 ? reader->sampleRate / sr : 1.0;

    if (isPitchedSlot((SoundSlot)s) && !slotPitches[s].empty()) {
      auto pitches = slotPitches[s];
      std::sort(pitches.begin(), pitches.end());
      const int median = pitches[pitches.size() / 2];
      const int pc = pitchClassFromKey(SampleLibrary::parseKey(f.getFileName()));
      if (pc >= 0) {
        const int oct = juce::jlimit(1, 7, (int)std::lround((median - pc) / 12.0));
        slotRoot[s] = pc + 12 * oct;
      } else {
        slotRoot[s] = juce::jlimit(12, 96, median);
      }
    }
  }

  // Pass 3: render (simple linear resample + envelope + velocities).
  const double totalSec = juce::jmin(240.0, lastBeat * secPerBeat + 1.5);
  const int totalSamples = juce::jmax((int)(0.25 * sr), (int)(totalSec * sr));
  juce::AudioBuffer<float> out(2, totalSamples);
  out.clear();

  for (const auto& pn : pending) {
    const auto& n = *pn.note;
    if (!slotOk[pn.slot]) continue;
    const auto& buf = slotBuf[pn.slot];
    const int ns = buf.getNumSamples();
    if (ns < 2) continue;
    const int nc = buf.getNumChannels();
    const float* c0 = buf.getReadPointer(0);
    const float* c1 = buf.getReadPointer(nc > 1 ? 1 : 0);

    double rate = slotRate[pn.slot];
    if (pn.pitched)
      rate *= std::pow(2.0, (n.pitch - slotRoot[pn.slot]) / 12.0);
    if (rate <= 0.0) rate = 1.0;

    const int startSample = (int)(n.start * secPerBeat * sr);
    if (startSample >= totalSamples) continue;

    const double durSec = juce::jmax(0.05, n.length * secPerBeat);
    const float gain = slotGain((SoundSlot)pn.slot) *
                       (0.30f + 0.78f * (float)juce::jlimit(1, 127, n.velocity) / 127.0f);

    double pos = 0.0;
    for (int i = startSample; i < totalSamples && pos + 1.0 < (double)ns;
         ++i, pos += rate) {
      float env = 1.0f;
      if (pn.pitched) {
        const double t = (i - startSample) / sr;
        if (t < 0.004) env = (float)(t / 0.004);
        else if (t > durSec) env = (float)juce::jmax(0.0, 1.0 - (t - durSec) / 0.03);
        if (env <= 0.0f) break;
      }
      const int p0 = (int)pos;
      const int p1 = (p0 + 1 < ns) ? p0 + 1 : ns - 1;
      const float fr = (float)(pos - p0);
      const float sL = c0[p0] + (c0[p1] - c0[p0]) * fr;
      const float sR = c1[p0] + (c1[p1] - c1[p0]) * fr;
      const float inc = gain * env;
      out.addSample(0, i, sL * inc);
      out.addSample(1, i, sR * inc);
    }
  }

  // Only scale down when the blend clips; otherwise keep the natural balance.
  float peak = 0.0f;
  for (int ch = 0; ch < 2; ++ch)
    peak = juce::jmax(peak, out.getMagnitude(ch, 0, totalSamples));
  if (peak > 0.95f) out.applyGain(0.95f / peak);

  // Write into the temp folder; the BPM in the name helps FL when dragging
  // the loop into the Playlist.
  const juce::String prefix = (mixAll || trackName.isEmpty()) ? juce::String("Idea") : trackName;
  const int bpmInt = (int)std::lround(bpm);
  auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("MidiForge");
  dir.createDirectory();
  auto file = dir.getChildFile("MidiForge_" + prefix + "_sonido_" +
                               juce::String(bpmInt) + "BPM.wav");
  file.deleteFile();
  auto stream = std::unique_ptr<juce::FileOutputStream>(file.createOutputStream());
  if (stream == nullptr) return {};
  stream->setPosition(0);
  stream->truncate();
  juce::WavAudioFormat wav;
  std::unique_ptr<juce::AudioFormatWriter> writer(
      wav.createWriterFor(stream.get(), sr, 2, 24, {}, 0));
  if (writer == nullptr) {
    file.deleteFile();
    return {};
  }
  stream.release();
  writer->writeFromAudioSampleBuffer(out, 0, totalSamples);
  writer.reset(); // flush + close
  return file;
}

juce::String freesoundQueryFor(SoundSlot slot, Character c) {
  switch (slot) {
    case SoundSlot::Melody:
      switch (c) {
        case Character::Plugg: return "plugg bell";
        case Character::NuJerk: return "bell pluck";
        case Character::Detroit: return "detroit bell";
        case Character::TrapDark: return "dark bell";
        case Character::BoomBap: return "soul piano";
        case Character::Rage: return "synth lead";
        case Character::TrapLatino: return "pluck";
        case Character::None:
        default: return "melody bell";
      }
    case SoundSlot::Chords:
      switch (c) {
        case Character::BoomBap: return "rhodes chord";
        case Character::Rage: return "synth chord";
        case Character::Detroit: return "piano chord";
        case Character::None:
        default: return "rhodes chord";
      }
    case SoundSlot::Bass:
      return "808 bass";
    default:
      return {};
  }
}

juce::File fetchFreesoundSound(const juce::String& apiKey, SoundSlot slot,
                               Character character, const juce::File& cacheDir,
                               juce::String& status) {
  const auto query = freesoundQueryFor(slot, character);
  if (query.isEmpty()) {
    status = "Esta capa usa solo tus packs";
    return {};
  }
  const auto key = apiKey.trim();
  if (key.isEmpty()) {
    status = "Falta la API key de Freesound";
    return {};
  }

  const juce::String filter = "license:\"Creative Commons 0\" duration:[0 TO 12]";
  const juce::String url =
      "https://freesound.org/apiv2/search/text/?query=" +
      juce::URL::addEscapeChars(query, true) + "&filter=" +
      juce::URL::addEscapeChars(filter, true) + "&fields=" +
      juce::URL::addEscapeChars("id,name,previews,license,duration", true) +
      "&page_size=8&token=" + key;

  status = "Buscando en Freesound...";
  auto stream = juce::URL(url).createInputStream(nullptr, 12000, {}, 10000);
  if (stream == nullptr) {
    status = "Sin conexion con Freesound (revisa la key)";
    return {};
  }
  const auto text = stream->readEntireStreamAsString();
  stream.reset();

  auto json = juce::JSON::parse(text);
  auto* obj = json.getDynamicObject();
  if (obj == nullptr) {
    status = "Respuesta no valida de Freesound";
    return {};
  }
  auto* results = obj->getProperty("results").getArray();
  if (results == nullptr) {
    const auto detail = obj->getProperty("detail").toString();
    status = detail.isNotEmpty() ? ("Freesound: " + detail) : juce::String("Error de Freesound");
    return {};
  }
  if (results->isEmpty()) {
    status = "Sin resultados CC0 para \"" + query + "\"";
    return {};
  }

  cacheDir.createDirectory();
  for (const auto& v : *results) {
    auto* o = v.getDynamicObject();
    if (o == nullptr) continue;
    auto* previews = o->getProperty("previews").getDynamicObject();
    if (previews == nullptr) continue;
    auto hq = previews->getProperty("preview-hq-mp3").toString();
    if (hq.isEmpty()) hq = previews->getProperty("preview-lq-mp3").toString();
    if (hq.isEmpty()) continue;
    if (!hq.contains("token="))
      hq += (hq.containsChar('?') ? "&" : "?") + juce::String("token=") + key;

    const auto soundName = o->getProperty("name").toString();
    const int id = (int)o->getProperty("id");
    auto file = cacheDir.getChildFile(
        "freesound_" + juce::String((int)slot) + "_" +
        juce::File::createLegalFileName(soundName) + "_" + juce::String(id) + ".mp3");
    file.deleteFile();

    auto dl = juce::URL(hq).createInputStream(nullptr, 20000, {}, 12000);
    if (dl == nullptr) continue;
    juce::MemoryBlock mb;
    dl->readIntoMemoryBlock(mb);
    dl.reset();
    if (mb.getSize() > 1000 && file.replaceWithData(mb.getData(), mb.getSize())) {
      status = "Descargado: " + soundName;
      return file;
    }
  }
  status = "No se pudo descargar ningun preview";
  return {};
}

} // namespace MidiForge
