#include "MidiDrag.h"

namespace MidiForge {

bool MidiDrag::startDraggingMidi(juce::Component* source, const juce::MidiFile& mf,
                                 const juce::String& clipName) {
  // Persist the drag payload as a real .mid file: the Windows shell drag that
  // JUCE performs transfers a file list (CF_HDROP), so the receiver (FL Studio)
  // needs an actual file on disk, not a memory buffer.
  auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                 .getChildFile("MidiForge");
  dir.createDirectory();

  auto safeName = juce::File::createLegalFileName(clipName);
  if (safeName.isEmpty()) safeName = "clip";
  auto file = dir.getChildFile(safeName + ".mid");

  // Replace any previous same-named file.
  file.deleteFile();

  if (auto stream = file.createOutputStream()) {
    if (!mf.writeTo(*stream)) return false;
    stream->flush();
  } else {
    return false;
  }

  juce::StringArray files;
  files.add(file.getFullPathName());

  // canMoveFiles = false -> receiver makes a copy, keeps our temp file usable.
  return juce::DragAndDropContainer::performExternalDragDropOfFiles(files, false, source);
}

} // namespace MidiForge
