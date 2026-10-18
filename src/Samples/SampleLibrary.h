#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <vector>

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>

namespace MidiForge {

struct SampleItem {
  juce::String path;
  juce::String name;
  juce::String folder;
  int bpm{0};         // 0 = unknown
  juce::String key;   // "" = unknown, e.g. "Fm", "C#", "Am"
};

// Background indexer for local audio files. Scans the configured folders,
// extracts filename metadata (BPM/key) and exposes a thread-safe snapshot.
class SampleLibrary : public juce::Thread {
public:
  SampleLibrary();
  ~SampleLibrary() override;

  void setFolders(const juce::StringArray& newFolders);
  juce::StringArray getFolders() const;
  void rescan();

  bool isScanning() const { return scanning.load(); }
  int numItems() const;
  std::vector<SampleItem> snapshot() const;

  // Favourites (message thread only; persisted with the plugin state).
  juce::StringArray getFavorites() const;
  void setFavorites(const juce::StringArray& newFavorites);
  bool isFavorite(const juce::String& path) const;
  bool toggleFavorite(const juce::String& path);

  // Called on the message thread whenever a scan makes progress / finishes.
  std::function<void()> onScanChanged;

  void run() override;

  static bool isAudioFile(const juce::File& f);
  static int parseBpm(const juce::String& fileName);
  static juce::String parseKey(const juce::String& fileName);

private:
  void scanOnce();
  void notifyChanged();

  mutable juce::CriticalSection lock;
  std::vector<SampleItem> items;
  juce::StringArray folders;
  mutable juce::CriticalSection favLock;
  juce::StringArray favorites;
  std::atomic<bool> scanning{false};
  std::atomic<bool> dirty{false};
  std::shared_ptr<std::atomic<bool>> aliveFlag;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleLibrary)
};

} // namespace MidiForge
