#include "ChatPanel.h"
#include "../LookAndFeel.h"

namespace MidiForge {

ChatPanel::ChatPanel(MidiForgeAudioProcessor& p) : processor(p), engine(p) {
  titleLabel.setText("Forge Chat", juce::dontSendNotification);
  titleLabel.setFont(juce::Font(juce::FontOptions(14.0f, juce::Font::bold)));
  titleLabel.setColour(juce::Label::textColourId, Palette::accent);
  addAndMakeVisible(titleLabel);

  log.setMultiLine(true, false);
  log.setReadOnly(true);
  log.setScrollbarsShown(true);
  log.setCaretVisible(false);
  log.setPopupMenuEnabled(false);
  log.setFont(juce::Font(juce::FontOptions(12.5f)));
  log.setColour(juce::TextEditor::backgroundColourId, Palette::panelAlt.darker(0.25f));
  log.setColour(juce::TextEditor::textColourId, Palette::textPrimary);
  log.setColour(juce::TextEditor::outlineColourId, Palette::border);
  log.setColour(juce::TextEditor::focusedOutlineColourId, Palette::border);
  log.setColour(juce::TextEditor::shadowColourId, juce::Colours::transparentBlack);
  addAndMakeVisible(log);

  const char* chipTexts[] = {"Trap oscuro en F menor", "House alegre",
                             "Cambia la melodia", "Solo bateria y bajo",
                             "Mas swing", "Busca hi-hats oscuros"};
  for (const char* c : chipTexts) {
    auto b = std::make_unique<juce::TextButton>(c);
    b->setColour(juce::TextButton::buttonColourId, Palette::card);
    b->onClick = [this, c] { send(c); };
    addAndMakeVisible(*b);
    chips.push_back(std::move(b));
  }

  providerLabel.setText("Sample Scout - tienda", juce::dontSendNotification);
  providerLabel.setFont(juce::Font(juce::FontOptions(11.0f)));
  providerLabel.setColour(juce::Label::textColourId, Palette::textSecondary);
  addAndMakeVisible(providerLabel);

  providerBox.addItem("Splice", 1);
  providerBox.addItem("Freesound", 2);
  providerBox.addItem("Loopcloud", 4);
  providerBox.addItem("Samplette", 3);
  providerBox.setSelectedId(1, juce::dontSendNotification);
  addAndMakeVisible(providerBox);

  openSearchButton.setEnabled(false);
  openSearchButton.setColour(juce::TextButton::buttonColourId, Palette::card);
  openSearchButton.onClick = [this] {
    if (lastQuery.isEmpty()) return;
    auto url = ChatEngine::buildSearchUrl(lastQuery, providerBox.getSelectedId());
    juce::URL(url).launchInDefaultBrowser();
  };
  addAndMakeVisible(openSearchButton);

  sendButton.setColour(juce::TextButton::buttonColourId, Palette::accent);
  sendButton.onClick = [this] { send(input.getText()); };
  addAndMakeVisible(sendButton);

  input.setMultiLine(false, false);
  input.setFont(juce::Font(juce::FontOptions(14.0f)));
  input.setTextToShowWhenEmpty("Pide algo: 'trap oscuro en F menor'", Palette::textSecondary);
  input.setColour(juce::TextEditor::backgroundColourId, Palette::panelAlt.darker(0.25f));
  input.setColour(juce::TextEditor::textColourId, Palette::textPrimary);
  input.setColour(juce::TextEditor::outlineColourId, Palette::border);
  input.setColour(juce::TextEditor::focusedOutlineColourId, Palette::accent);
  input.setColour(juce::TextEditor::shadowColourId, juce::Colours::transparentBlack);
  input.onReturnKey = [this] { send(input.getText()); };
  addAndMakeVisible(input);

  append("FORGE", "Hola! Soy tu coproductor offline. Pideme cosas como: 'trap oscuro en F menor', "
                  "'cambia la melodia', 'mas swing', 'solo bateria y bajo' o 'busca hi-hats oscuros'.");
}

void ChatPanel::paint(juce::Graphics& g) {
  g.fillAll(Palette::panelAlt);
  g.setColour(Palette::border);
  g.drawVerticalLine(0, 0.0f, (float)getHeight());
}

void ChatPanel::resized() {
  auto r = getLocalBounds().reduced(10);
  titleLabel.setBounds(r.removeFromTop(22));
  r.removeFromTop(4);

  auto chipsArea = r.removeFromTop(60);
  for (int i = 0; i < (int)chips.size(); ++i) {
    int row = i / 2;
    int col = i % 2;
    int w = (chipsArea.getWidth() - 6) / 2;
    chips[(size_t)i]->setBounds(chipsArea.getX() + col * (w + 6),
                                chipsArea.getY() + row * 28, w, 26);
  }
  r.removeFromTop(4);

  auto bottom = r.removeFromBottom(84);
  providerLabel.setBounds(bottom.removeFromTop(14));
  auto row2 = bottom.removeFromTop(26);
  openSearchButton.setBounds(row2.removeFromRight(112));
  row2.removeFromRight(6);
  providerBox.setBounds(row2);
  bottom.removeFromTop(6);
  auto row3 = bottom.removeFromTop(28);
  sendButton.setBounds(row3.removeFromRight(70));
  row3.removeFromRight(6);
  input.setBounds(row3);

  log.setBounds(r);
}

void ChatPanel::send(const juce::String& text) {
  auto t = text.trim();
  if (t.isEmpty()) return;
  append("TU", t);
  input.clear();
  auto reply = engine.handle(t);
  append("FORGE", reply.text);
  lastQuery = reply.searchQuery;
  updateOpenSearch();
  if (reply.ideaChanged && onIdeaChanged) onIdeaChanged();
}

void ChatPanel::append(const juce::String& who, const juce::String& text) {
  log.moveCaretToEnd();
  log.insertTextAtCaret(who + ": " + text + "\n\n");
  log.moveCaretToEnd();
}

void ChatPanel::updateOpenSearch() {
  openSearchButton.setEnabled(lastQuery.isNotEmpty());
}

} // namespace MidiForge
