#include "ChatEngine.h"
#include "../Core/MusicTheory.h"

#include <initializer_list>
#include <utility>
#include <vector>

namespace MidiForge {

namespace {

// ---------------------------------------------------------------- helpers

juce::String normalize(juce::String s) {
  s = s.toLowerCase();
  const std::pair<const char*, const char*> reps[] = {
      {"á","a"},{"à","a"},{"ä","a"},{"â","a"},
      {"é","e"},{"è","e"},{"ë","e"},
      {"í","i"},{"ì","i"},{"ï","i"},
      {"ó","o"},{"ò","o"},{"ö","o"},{"ô","o"},
      {"ú","u"},{"ù","u"},{"ü","u"},{"û","u"},
      {"ñ","n"},{"ç","c"}};
  for (const auto& r : reps) s = s.replace(r.first, r.second);
  while (s.contains("  ")) s = s.replace("  ", " ");
  return s.trim();
}

bool hasAny(const juce::String& s, std::initializer_list<const char*> words) {
  for (const char* w : words) if (s.contains(w)) return true;
  return false;
}

juce::StringArray tokensOf(const juce::String& s) {
  auto raw = juce::StringArray::fromTokens(s, " ,.;:!?()[]{}\"'/\\|-+*@&=", "");
  juce::StringArray out;
  for (const auto& t : raw) if (t.isNotEmpty()) out.add(t);
  return out;
}

bool hasToken(const juce::StringArray& toks, const char* w) {
  for (const auto& t : toks) if (t == w) return true;
  return false;
}

bool containsString(const std::vector<juce::String>& v, const juce::String& s) {
  for (const auto& x : v) if (x == s) return true;
  return false;
}

juce::String joinList(const std::vector<juce::String>& v) {
  juce::String out;
  for (size_t i = 0; i < v.size(); ++i) {
    if (i > 0) out += (i + 1 == v.size() ? " y " : ", ");
    out += v[i];
  }
  return out;
}

std::vector<juce::String> findAllLayers(const juce::String& s) {
  std::vector<juce::String> out;
  if (hasAny(s, {"melodia","melody","lead","voz","voces"})) out.push_back("Melody");
  if (hasAny(s, {"bajo","bass"})) out.push_back("Bass");
  if (hasAny(s, {"bateria","drums","drum","percusion"})) out.push_back("Drums");
  if (hasAny(s, {"acordes","chords","armonia","progresion","pads"})) out.push_back("Chords");
  return out;
}

// ---------------------------------------------------------------- labels

juce::String scaleLabel(ScaleType t) {
  switch (t) {
    case ScaleType::Major: return "Major";
    case ScaleType::MinorNatural: return "Minor";
    case ScaleType::MinorHarmonic: return "Minor harm.";
    case ScaleType::MinorMelodic: return "Minor mel.";
    case ScaleType::Dorian: return "Dorian";
    case ScaleType::Phrygian: return "Phrygian";
    case ScaleType::Lydian: return "Lydian";
    case ScaleType::Mixolydian: return "Mixolydian";
    case ScaleType::Aeolian: return "Aeolian";
    case ScaleType::Locrian: return "Locrian";
    case ScaleType::PentatonicMajor: return "Pent. mayor";
    case ScaleType::PentatonicMinor: return "Pent. menor";
    case ScaleType::Blues: return "Blues";
    case ScaleType::Chromatic: return "Chromatic";
  }
  return "Major";
}

juce::String keyLabelOf(const SessionIdea& idea) {
  static const char* notes[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
  int pc = ((idea.keyRoot % 12) + 12) % 12;
  return juce::String(notes[pc]) + (MusicTheory::isMinorFamily(idea.scale) ? " menor" : " mayor");
}

juce::String describeIdea(const SessionIdea& idea) {
  juce::String out = "Ahora suena:";
  if (idea.progressionName.isNotEmpty())
    out += "\n- Progresion: " + idea.progressionName + " (" + idea.progressionDisplay + ")";
  for (const auto& t : idea.tracks)
    out += "\n- " + t.name + ": " + (t.detail.isNotEmpty() ? t.detail : juce::String("listo"));
  return out;
}

juce::String trackDetailOf(const SessionIdea& idea, const juce::String& name) {
  for (const auto& t : idea.tracks) if (t.name == name) return t.detail;
  return {};
}

// ---------------------------------------------------------------- parsing

int letterPc(char c) {
  switch (c) {
    case 'c': return 0;
    case 'd': return 2;
    case 'e': return 4;
    case 'f': return 5;
    case 'g': return 7;
    case 'a': return 9;
    case 'b': return 11;
    default: return -1;
  }
}

// Returns pitch class 0-11 for a note token ("f#", "bb", "do", "mib") or -1.
int parseNoteToken(juce::String t) {
  if (t.isEmpty()) return -1;

  struct Sol { const char* w; int pc; };
  static const Sol sol[] = {{"do",0},{"re",2},{"mi",4},{"fa",5},{"sol",7},{"la",9},{"si",11}};
  for (const auto& s : sol) {
    juce::String w(s.w);
    if (t.startsWith(w)) {
      juce::String rest = t.substring(w.length());
      int pc = s.pc;
      for (auto ch : rest) {
        if (ch == '#') pc = (pc + 1) % 12;
        else if (ch == 'b') pc = (pc + 11) % 12;
        else return -1;
      }
      return pc;
    }
  }

  int pc = letterPc((char)t[0]);
  if (pc < 0) return -1;
  for (int i = 1; i < t.length(); ++i) {
    auto ch = t[i];
    if (ch == '#') pc = (pc + 1) % 12;
    else if (ch == 'b') pc = (pc + 11) % 12;
    else return -1;
  }
  return pc;
}

bool findKey(const juce::String& s, int& pc, bool* minorHint = nullptr) {
  auto toks = tokensOf(s);
  auto isMode = [](const juce::String& w) {
    return w == "menor" || w == "mayor" || w == "minor" || w == "major" ||
           w == "min" || w == "maj" || w == "dorico" || w == "dorian" ||
           w == "mixolidio" || w == "mixolydian" || w == "blues" ||
           w == "pentatonica" || w == "pentatonic";
  };
  for (int i = 0; i < toks.size(); ++i) {
    auto tok = toks[i];

    // Composite tokens like "fmin" / "c#maj".
    juce::String stem;
    if (tok.endsWith("min") && tok.length() > 3) stem = tok.substring(0, tok.length() - 3);
    else if (tok.endsWith("maj") && tok.length() > 3) stem = tok.substring(0, tok.length() - 3);
    if (stem.isNotEmpty()) {
      int npc = parseNoteToken(stem);
      if (npc >= 0) {
        pc = npc;
        if (minorHint != nullptr && tok.endsWith("min")) *minorHint = true;
        return true;
      }
    }

    int npc = parseNoteToken(tok);
    if (npc < 0) continue;
    bool nextMode = (i + 1 < toks.size()) && isMode(toks[i + 1]);
    bool prevEn = (i > 0) && (toks[i - 1] == "en" || toks[i - 1] == "in");
    bool accidental = tok.containsChar('#') || tok.substring(1).containsChar('b');
    if (nextMode || (prevEn && accidental)) { pc = npc; return true; }
  }
  return false;
}

bool findBars(const juce::String& s, int& bars) {
  auto toks = tokensOf(s);
  for (int i = 0; i < toks.size(); ++i) {
    auto w = toks[i];
    if (w == "compas" || w == "compases" || w == "bar" || w == "bars") {
      if (i > 0) {
        int n = toks[i - 1].getIntValue();
        if (n >= 1 && n <= 32) { bars = n; return true; }
      }
      if (i + 1 < toks.size()) {
        int n = toks[i + 1].getIntValue();
        if (n >= 1 && n <= 32) { bars = n; return true; }
      }
    }
  }
  return false;
}

int findBpm(const juce::String& s) {
  auto toks = tokensOf(s);
  for (int i = 0; i < toks.size(); ++i) {
    auto w = toks[i];
    if (w.contains("bpm")) {
      int inlineN = w.replace("bpm", "").getIntValue();
      if (inlineN >= 40 && inlineN <= 300) return inlineN;
      if (i > 0) { int n = toks[i - 1].getIntValue(); if (n >= 40 && n <= 300) return n; }
      if (i + 1 < toks.size()) { int n = toks[i + 1].getIntValue(); if (n >= 40 && n <= 300) return n; }
    }
  }
  return 0;
}

bool matchStyle(const juce::String& s, Style& out) {
  if (hasAny(s, {"boom bap","boombap","hip hop","hiphop","hip-hop","trap","drill",
                 "phonk","reggaeton","urbano","urban","chillhop","lofi","lo-fi","lo fi",
                 "chill","jazzy","jazz hop","jazzhop"})) {
    out = hasAny(s, {"lofi","lo-fi","lo fi","chill","jazzy","jazz hop","jazzhop","chillhop"})
              ? Style::LoFi : Style::HipHop;
    return true;
  }
  if (hasAny(s, {"house","techno","garage","groove","deep"})) { out = Style::House; return true; }
  if (hasAny(s, {"edm","festival","trance","electro","dance","big room","rave","club"})) { out = Style::EDM; return true; }
  if (hasAny(s, {"rock","indie","metal","punk","guitarra","guitar"})) { out = Style::Rock; return true; }
  if (hasAny(s, {"pop","comercial","radio"})) { out = Style::Pop; return true; }
  return false;
}

bool matchScale(const juce::StringArray& toks, ScaleType& out) {
  for (const auto& t : toks) {
    if (t == "dorico" || t == "dorian") { out = ScaleType::Dorian; return true; }
    if (t == "mixolidio" || t == "mixolydian") { out = ScaleType::Mixolydian; return true; }
    if (t == "blues") { out = ScaleType::Blues; return true; }
    if (t == "pentatonica" || t == "pentatonic") {
      out = (hasToken(toks, "mayor") || hasToken(toks, "major"))
                ? ScaleType::PentatonicMajor : ScaleType::PentatonicMinor;
      return true;
    }
  }
  for (const auto& t : toks) {
    if (t == "menor" || t == "minor" || t == "min") { out = ScaleType::MinorNatural; return true; }
    if (t == "mayor" || t == "major" || t == "maj") { out = ScaleType::Major; return true; }
  }
  return false;
}

// -1 = dark, +1 = bright, 0 = neutral.
int moodSign(const juce::String& s) {
  if (hasAny(s, {"oscuro","oscura","oscuras","oscuros","dark","triste","tristes","sad",
                 "melancolic","sombri","tenebros","nocturn","misterios","serio","seria"})) return -1;
  if (hasAny(s, {"alegre","alegres","feliz","happy","brillante","brillantes","bright",
                 "luminos","verano","sunny","vivo","energetic","energico","epico","epic",
                 "fiesta","party","radiante"})) return 1;
  return 0;
}

// ---------------------------------------------------------------- scout

bool isScout(const juce::String& s) {
  return hasAny(s, {"busca","encuentra","encontra","sample","loop","sonido","one shot",
                    "oneshot","acapella","chop","freesound","splice","samplette",
                    "loopcloud","pack"});
}

juce::String styleWord(Style st) {
  switch (st) {
    case Style::Pop: return "pop";
    case Style::LoFi: return "lo-fi";
    case Style::House: return "house";
    case Style::HipHop: return "hip hop";
    case Style::EDM: return "edm";
    case Style::Rock: return "rock";
  }
  return "pop";
}

juce::String buildScoutQuery(const juce::String& s, const SessionIdea& idea) {
  juce::String q = s;
  for (auto ch : juce::String(",.;:!?()[]{}\"'")) q = q.replaceCharacter(ch, ' ');
  q = q.replace("one shot", "oneshot");
  q = q.replace("hi hat", "hihat");

  const std::pair<const char*, const char*> dict[] = {
      {"oscuro","dark"},{"oscura","dark"},{"oscuros","dark"},{"oscuras","dark"},
      {"brillante","bright"},{"brillantes","bright"},
      {"calido","warm"},{"calida","warm"},{"calidos","warm"},{"calidas","warm"},
      {"suave","soft"},{"suaves","soft"},{"rapido","fast"},{"rapida","fast"},
      {"rapidos","fast"},{"rapidas","fast"},{"lento","slow"},{"lenta","slow"},
      {"profundo","deep"},{"profunda","deep"},{"triste","sad"},{"tristes","sad"},
      {"melancolico","melancholic"},{"melancolica","melancholic"},
      {"energico","energetic"},{"energica","energetic"},{"hipnotico","hypnotic"},
      {"etereo","ethereal"},{"cristalino","glassy"},{"analogico","analog"},
      {"vintage","vintage"},{"bonito","beautiful"},{"bonita","beautiful"},
      {"voz","vocal"},{"voces","vocals"},{"vocal","vocal"},{"coro","choir"},
      {"cuerda","strings"},{"cuerdas","strings"},{"piano","piano"},{"guitarra","guitar"},
      {"guitarras","guitar"},{"trompeta","trumpet"},{"saxo","sax"},{"flauta","flute"},
      {"organo","organ"},{"teclado","keys"},{"arpegio","arp"},
      {"campana","bell"},{"campanas","bells"},{"caja","snare"},{"bombo","kick"},
      {"charles","hi-hat"},{"hihat","hi-hat"},{"hi-hats","hi-hats"},{"platillos","cymbals"},
      {"percu","percussion"},{"percusion","percussion"},{"shaker","shaker"},
      {"congas","congas"},{"clap","clap"},{"claps","claps"},{"bateria","drums"},
      {"bajo","bass"},{"melodia","melody"},{"acordes","chords"},
      {"vinilo","vinyl"},{"ruido","noise"},{"ambiente","ambient"},{"textura","texture"},
      {"golpe","hit"},{"golpes","hits"},{"sonido","sound"},{"sonidos","sounds"}};

  const char* drop[] = {"busca","buscar","buscame","encuentrame","encuentra","encontra",
                        "quiero","dame","me","un","una","unos","unas","el","la","los","las",
                        "de","del","al","a","o","u","y","con","sin","para","que","por","favor",
                        "algo","como","tipo","estilo","rollo","vibe","vibra","muy","mas",
                        "sample","samples","loop","loops","oneshot","oneshots","chop","chops",
                        "acapella","freesound","splice","samplette","loopcloud","pack","packs","porfa"};

  juce::StringArray rebuilt;
  for (const auto& t : tokensOf(q)) {
    bool dropped = false;
    for (const char* d : drop) if (t == d) { dropped = true; break; }
    if (dropped) continue;
    juce::String mapped = t;
    for (const auto& p : dict) if (t == p.first) { mapped = p.second; break; }
    rebuilt.add(mapped);
  }

  juce::String out = rebuilt.joinIntoString(" ");
  auto sw = styleWord(idea.style);
  if (!out.contains(sw)) out = (out + " " + sw).trim();
  if (moodSign(s) < 0 && !out.contains("dark")) out = (out + " dark").trim();
  if (out.isEmpty()) out = "loop";
  return out;
}

} // namespace

// ---------------------------------------------------------------- engine

ChatReply ChatEngine::handle(const juce::String& userText) {
  ChatReply reply;
  auto raw = userText.trim();
  if (raw.isEmpty()) return reply;

  auto s = normalize(raw);
  auto& idea = processor.getCurrentIdea();
  auto toks = tokensOf(s);

  // 1) Sample Scout: prepare a web search, don't touch the idea.
  if (isScout(s)) {
    auto q = buildScoutQuery(s, idea);
    reply.searchQuery = q;
    reply.text = "He preparado la búsqueda: '" + q + "'\n"
                 "Pulsa 'Abrir búsqueda' y elige tienda (Splice, Freesound, Loopcloud o Samplette).\n"
                 "Nota Samplette: es un shuffle con filtros; usa Key = " + keyLabelOf(idea) +
                 " y estilo " + styleName(idea.style) + ".";
    return reply;
  }

  auto layers = findAllLayers(s);

  // 2) Replace one or more layers with a different curated option.
  bool replaceVerb = hasAny(s, {"cambia","cambiale","cambiamela","cambiamelo","reemplaza",
                                "replace","regenera","sustituye"});
  bool otraLayer = hasAny(s, {"otra melodia","otro bajo","otra bateria","otros acordes",
                              "otra armonia","otra progresion"});
  if ((replaceVerb || otraLayer) && !layers.empty()) {
    juce::StringArray done;
    for (const auto& l : layers) { processor.regenerateTrack(l); done.add(l); }
    reply.ideaChanged = true;
    reply.text = "Reemplazada: " + done.joinIntoString(" + ") + ".\n";
    for (const auto& l : layers) reply.text += "- " + l + ": " + trackDetailOf(idea, l) + "\n";
    return reply;
  }

  // 3) Mute / solo / reactivate layers.
  bool soloAsk = hasToken(toks, "solo") || hasToken(toks, "solamente");
  bool muteAsk = hasAny(s, {"quita","quitale","mutea","mute","silencia","apaga","elimina",
                            "desactiva","fuera"}) || hasToken(toks, "sin");
  bool unmuteAsk = hasAny(s, {"activa","activale","desmutea","unmute","enciende","reactiva",
                              "recupera","anade"});
  if ((soloAsk || muteAsk || unmuteAsk) && !layers.empty()) {
    if (soloAsk) {
      for (auto& t : idea.tracks) {
        bool on = containsString(layers, t.name);
        t.solo = on;
        t.mute = !on;
      }
      reply.text = "Solo suenan: " + joinList(layers) + ".";
    } else if (unmuteAsk) {
      for (auto& t : idea.tracks)
        if (containsString(layers, t.name)) { t.mute = false; t.solo = false; }
      reply.text = "Reactivado: " + joinList(layers) + ".";
    } else {
      for (auto& t : idea.tracks)
        if (containsString(layers, t.name)) { t.mute = true; t.solo = false; }
      reply.text = "Silenciado: " + joinList(layers) + ". (Di 'activa ...' para recuperarlo.)";
    }
    reply.ideaChanged = true;
    return reply;
  }

  // 4) Musical parameters + variation / new idea.
  bool wantVariation = hasAny(s, {"otra variacion","variacion","variaciones","otra toma",
                                  "otra version","shuffle","otra vuelta","cambia todo"});
  bool wantNew = hasAny(s, {"nueva idea","nuevo beat","nueva cancion","otra idea","otro beat",
                            "otra cancion","random","aleatorio","sorprendeme","hazme algo",
                            "algo nuevo","genera","generate","crea"})
                 || hasToken(toks, "hazme") || hasToken(toks, "haz") || hasToken(toks, "dame");

  Style st = idea.style;
  bool styleMatched = matchStyle(s, st);
  int keyPc = 0;
  bool minorHint = false;
  bool keyPcFound = findKey(s, keyPc, &minorHint);
  ScaleType sc;
  bool scaleWord = matchScale(toks, sc);
  int mood = moodSign(s);
  bool swingAsk = s.contains("swing") || s.contains("balanceo");
  int newBars = 0;
  bool barsAsk = findBars(s, newBars);
  int bpm = findBpm(s);

  if (wantVariation || wantNew || styleMatched || keyPcFound || scaleWord || mood != 0 || swingAsk || barsAsk) {
    if (styleMatched) idea.style = st;
    if (keyPcFound) idea.keyRoot = keyPc;
    if (scaleWord) idea.scale = sc;
    else if (minorHint) idea.scale = ScaleType::MinorNatural;

    if (mood != 0) {
      idea.brightness = juce::jlimit(0.0, 1.0, idea.brightness + (mood > 0 ? 0.15 : -0.15));
      if (!scaleWord && !minorHint && (styleMatched || keyPcFound))
        idea.scale = (mood < 0) ? ScaleType::MinorNatural : ScaleType::Major;
    }

    if (swingAsk) {
      if (hasAny(s, {"menos","quita","sin","recto","reduce","baja"}))
        idea.swing = juce::jlimit(0.0, 100.0, idea.swing - 15.0);
      else
        idea.swing = juce::jlimit(0.0, 100.0, idea.swing + 15.0);
    }
    if (barsAsk) idea.bars = newBars;

    bool compositional = styleMatched || keyPcFound || scaleWord || minorHint || wantNew;
    if (compositional) {
      idea.seed = juce::Random::getSystemRandom().nextInt(1000000) + 1;
      idea.take = 0;
    } else if (wantVariation) {
      idea.take += 1;
    }

    processor.generateFull();
    reply.ideaChanged = true;

    juce::String head;
    if (wantNew) head = "Idea nueva (seed " + juce::String(idea.seed) + ").";
    else if (wantVariation) head = "Nueva variación (toma " + juce::String(idea.take) + ").";
    else head = "Hecho:";

    juce::StringArray changes;
    if (styleMatched) changes.add("Estilo: " + styleName(idea.style));
    if (keyPcFound || scaleWord || minorHint)
      changes.add("Tonalidad: " + keyLabelOf(idea) + " - " + scaleLabel(idea.scale));
    if (mood != 0) changes.add("Juice: " + juce::String(juce::roundToInt(idea.brightness * 100.0)) + "%");
    if (swingAsk) changes.add("Swing: " + juce::String(juce::roundToInt(idea.swing)) + "%");
    if (barsAsk) changes.add("Compases: " + juce::String(idea.bars));

    reply.text = head;
    if (!changes.isEmpty()) reply.text += "\n- " + changes.joinIntoString("\n- ");
    reply.text += "\n\n" + describeIdea(idea);
    if (bpm > 0)
      reply.text += "\n(Referencia: " + juce::String(bpm) + " BPM; el tempo real lo fija FL Studio.)";
    return reply;
  }

  // 5) Fallback help.
  reply.text = "No te he pillado del todo. Prueba con cosas como:\n"
               "- 'trap oscuro en F menor'\n"
               "- 'hazme house alegre de 8 compases'\n"
               "- 'cambia la melodia' / 'cambia la bateria'\n"
               "- 'solo bateria y bajo' / 'quita los acordes'\n"
               "- 'mas swing', 'mas brillo', 'mas oscuro'\n"
               "- 'otra variacion' / 'nueva idea'\n"
               "- 'busca hi-hats oscuros' (Sample Scout)";
  return reply;
}

juce::String ChatEngine::buildSearchUrl(const juce::String& query, int provider) {
  auto escaped = juce::URL::addEscapeChars(query.trim(), true);
  if (provider == 1) return "https://splice.com/sounds/search?q=" + escaped;
  if (provider == 2) return "https://freesound.org/search/?q=" + escaped;
  if (provider == 4) return "https://sounds.loopcloud.com/search?q=" + escaped;
  return "https://samplette.io/";
}

} // namespace MidiForge
