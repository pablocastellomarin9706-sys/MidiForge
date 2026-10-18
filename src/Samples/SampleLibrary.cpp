#include "SampleLibrary.h"

namespace MidiForge {

SampleLibrary::SampleLibrary()
  : juce::Thread("MidiForgeSampleScan"),
    aliveFlag(std::make_shared<std::atomic<bool>>(true)) {
  startThread();
}

SampleLibrary::~SampleLibrary() {
  aliveFlag->store(false);
  signalThreadShouldExit();
  notify();
  stopThread(2000);
}

void SampleLibrary::setFolders(const juce::StringArray& newFolders) {
  {
    const juce::ScopedLock sl(lock);
    folders = newFolders;
  }
  rescan();
}

juce::StringArray SampleLibrary::getFolders() const {
  const juce::ScopedLock sl(lock);
  return folders;
}

void SampleLibrary::rescan() {
  dirty = true;
  notify();
}

int SampleLibrary::numItems() const {
  const juce::ScopedLock sl(lock);
  return (int)items.size();
}

std::vector<SampleItem> SampleLibrary::snapshot() const {
  const juce::ScopedLock sl(lock);
  return items;
}

void SampleLibrary::notifyChanged() {
  auto alive = aliveFlag;
  juce::MessageManager::callAsync([this, alive] {
    if (!alive->load()) return;
    if (onScanChanged) onScanChanged();
  });
}

void SampleLibrary::run() {
  while (!threadShouldExit()) {
    if (!dirty.load()) {
      wait(400);
      continue;
    }
    dirty = false;
    scanning = true;
    scanOnce();
    scanning = false;
    notifyChanged();
  }
}

void SampleLibrary::scanOnce() {
  juce::StringArray dirs;
  {
    const juce::ScopedLock sl(lock);
    dirs = folders;
  }

  std::vector<SampleItem> found;
  const int maxFiles = 40000;

  for (int d = 0; d < dirs.size() && !threadShouldExit(); ++d) {
    juce::File root(dirs[d]);
    if (!root.isDirectory()) continue;

    juce::Array<juce::File> files;
    root.findChildFiles(files, juce::File::findFiles, true,
                        "*.wav;*.aif;*.aiff;*.mp3;*.ogg;*.flac");

    for (const auto& f : files) {
      if ((int)found.size() >= maxFiles) break;
      SampleItem it;
      it.path = f.getFullPathName();
      it.name = f.getFileName();
      it.folder = f.getParentDirectory().getFileName();
      it.bpm = parseBpm(it.name);
      it.key = parseKey(it.name);
      found.push_back(std::move(it));
    }

    if (threadShouldExit()) break;
    {
      const juce::ScopedLock sl(lock);
      items = found;
    }
    notifyChanged();
  }

  {
    const juce::ScopedLock sl(lock);
    items = found;
  }
}

bool SampleLibrary::isAudioFile(const juce::File& f) {
  auto e = f.getFileExtension().toLowerCase();
  return e == ".wav" || e == ".aif" || e == ".aiff" || e == ".mp3" ||
         e == ".ogg" || e == ".flac";
}

int SampleLibrary::parseBpm(const juce::String& fileName) {
  auto s = fileName.toLowerCase();
  const int i = s.indexOf("bpm");
  if (i > 0) {
    const int n = s.substring(0, i).getTrailingIntValue();
    if (n >= 40 && n <= 300) return n;
  }
  auto toks = juce::StringArray::fromTokens(s, " _-.,()[]", "");
  const int lim = juce::jmin(3, toks.size());
  for (int k = 0; k < lim; ++k) {
    const auto t = toks[k];
    if (t.length() >= 2 && t.length() <= 3 && t.containsOnly("0123456789")) {
      const int n = t.getIntValue();
      if (n >= 60 && n <= 200) return n;
    }
  }
  return 0;
}

juce::String SampleLibrary::parseKey(const juce::String& fileName) {
  auto toks = juce::StringArray::fromTokens(fileName, " _-.,()[]{}", "");
  for (int k = 0; k < toks.size(); ++k) {
    auto t = toks[k].toLowerCase();
    if (t.isEmpty() || t.length() > 6) continue;
    const juce_wchar c0 = t[0];
    if (c0 < 'a' || c0 > 'g') continue;

    juce::String acc;
    int pos = 1;
    if (pos < t.length() && (t[pos] == '#' || t[pos] == 'b')) {
      acc = juce::String::charToString(t[pos]);
      ++pos;
    }
    auto rest = t.substring(pos);
    auto base = juce::String::charToString(c0).toUpperCase() + acc;

    if (rest.isEmpty()) {
      if (acc.isEmpty()) {
        if (k + 1 < toks.size()) {
          auto nxt = toks[k + 1].toLowerCase();
          if (nxt == "m" || nxt == "min" || nxt == "minor") return base + "m";
          if (nxt == "maj" || nxt == "major") return base;
        }
        continue;
      }
      return base;
    }
    if (rest == "m" || rest == "min" || rest == "minor") return base + "m";
    if (rest == "maj" || rest == "major") return base;
  }
  return {};
}

} // namespace MidiForge
