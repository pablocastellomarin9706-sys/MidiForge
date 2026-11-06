# Changelog

## 1.7.0 - 2026-11-06

### Added
- **Editor de arreglo A/B (pestana ARREGLO)**: la idea se dibuja como una tira de
  compases; clic o arrastre sobre un compas fija ahi el inicio de la seccion B.
  Botones **AUTO** (mitad para 8+ compases, como antes) y **TODO A** (sin B).
- El limite es de verdad: progresion, groove de bateria y motivo de melodia
  cambian en la barra marcada, no siempre a la mitad. B puede empezar en el
  compas 2 de una idea de 4 si te apetece un drop pronto.
- **Regeneracion en caliente**: al mover el limite se rehacen las notas con los
  mismos picks y la misma semilla; solo se mueve la frontera. BLOQ sigue
  protegiendo cada track y el host marca el cambio como undo.
- La marca B del piano roll, las pestanas ambar del grid y el detalle del
  ChordStrip siguen ahora el limite elegido. Estado v6: el limite viaja con el
  proyecto (los proyectos viejos siguen con mitad automatica).

## 1.6.0 - 2026-11-01

### Added
- **Stems WAV + MIDI**: boton en la pestana SONIDO que renderiza los cuatro stems
  con los sonidos Auto-Sound actuales y guarda el MIDI completo en
  `Musica/MidiForge/Export_fecha_hora`, listo para compartir o subir.
- **Marca de seccion B en el piano roll**: linea y etiqueta B a la mitad de las
  ideas de 8+ compases, a juego con la progresion y el groove propios de la B.

## 1.5.0 - 2026-11-01

### Added
- **Progresion propia para la seccion B**: en ideas de 8+ compases la segunda
  mitad usa otra progresion de la misma familia (elegida sin alterar el resto de
  picks), asi que acordes, melodia y bajo cambian de verdad en la B. El detalle
  del track muestra "B: ...".
- **Pintar arrastrando en el grid de bateria**: el primer clic decide si el
  trazo activa o quita golpes; al arrastrar se aplica a todas las celdas que
  pasas. **Ctrl+Z** deshace el trazo completo (24 niveles).
- El grid muestra el compas actual con pestanas y marca en ambar la seccion B.

## 1.4.0 - 2026-11-01

### Added
- **Grid de bateria editable**: 16 pasos por compas y 10 lanes GM (kick, snare,
  clap, rim, hat, open hat, crash y 3 toms). Clic en una celda para activar o
  quitar el golpe; las pestanas numeradas cambian de compas y la seccion B se
  marca en ambar. Se guarda con el proyecto y muestra "editado a mano".
- **Modo DIATONICO** (boton en la cabecera): acordes y bajo construidos apilando
  terceras de la escala elegida, sin color prestado ni raices fuera de escala.
  Con pentatonica/blues la armonia usa su escala madre de 7 notas (la melodia
  sigue en la pentatonica). El bajo sanea sus b7 cromaticos.
- **Bloqueo por track (BLOQ)**: GENERATE, Shuffle, Replace y los cambios de
  parametros dejan intacto el track bloqueado, incluidas sus ediciones. El estado
  de bloqueo se guarda con el proyecto.

### Fixed
- El detalle del track de acordes avisa cuando suena en modo estricto.

## 1.3.0 - 2026-11-01

### Added
- **Piano roll editable** en Melody y Bass: arrastra una nota para moverla, tira del
  borde derecho para alargarla, doble clic en el hueco para anadir, clic derecho (o
  Alt+clic) para borrar y **Ctrl+Z** para deshacer. Rejilla de 1/16 y nota
  seleccionada resaltada.
- **Las ediciones se guardan con el proyecto**: los tracks tocados a mano se
  serializan nota a nota en el estado del plugin. Antes, al reabrir, todo se
  regeneraba desde la semilla. El track muestra el chip EDITADO y la etiqueta
  "editado a mano".
- **Seccion B en ideas de 8+ compases**: la segunda mitad usa otro groove de la
  misma familia, con crash de entrada, y la melodia cambia de celulas y sube un
  poco de intensidad.
- **Fills cada 4 compases** (ademas del final): turnarounds de trap, builds de
  house/EDM, toms de rock... asi un loop de 8/16 compases no suena copiado.
- **Glide real de 808**: en HipHop las notas de bajo se solapan ligeramente con la
  siguiente para que un 808 mono con portamento haga el slide (estilo FL), sin
  tocar dos veces la misma nota.

### Fixed
- Al abrir el editor se regeneraba la idea y se habrian perdido las ediciones
  manuales; ahora solo se genera si no habia ninguna idea restaurada.

## 1.2.0 - 2026-11-01

### Fixed
- **Melodias fuera de escala**: las notas de frase se enganchaban a cualquier nota
  del acorde, incluidos los colores prestados (bVII en mayor, V mayor en menor,
  novenas con Juice alto), asi que el lead podia salirse de la escala elegida.
  Ahora el enganche solo acepta notas del acorde que ademas estan en la escala, y
  hay una red final que devuelve cualquier nota suelta a su grado mas cercano.
- **Swing parecia roto**: el slider guardaba el valor pero no regeneraba la idea
  (habia que pulsar Shuffle para oirlo). Ahora se aplica al soltar.

### Added
- **Variacion real por genero en bateria y bajo**: cada estilo tenia un unico
  patron, asi que Shuffle no cambiaba nada. Ahora hay 3-4 grooves de bateria y
  3-4 lineas de bajo por estilo (Pop, LoFi, House, HipHop, EDM, Rock).
- **Bateria que respira**: los compases impares anaden toques del vocabulario del
  genero (ghost snares, kicks sincopados, open hats, rims) y el ultimo compas trae
  un fill propio (roll de trap, build de house/EDM, toms de rock, rim de lofi).
  Las duraciones ahora tienen sentido (open hat y crash suenan, hats cortos).
- **Bajo con movimiento**: saltos de octava del 808, bombeos de house, septimas
  de lofi, quintas de rock y notas de aproximacion caminando al siguiente acorde.
- **Motivos melodicos por estilo**: cada genero usa su propio conjunto de celulas
  ritmicas, y la melodia recorre el conjunto compas a compas.
- **Mascota animada de la forja** en la cabecera: yunque con martillo que golpea
  solo y lanza chispas. Es interactiva: un clic forja una idea nueva y reacciona
  a cada regeneracion.
- **Tooltips** en los controles de cabecera, SONIDO, SAMPLES y el chat.

### Removed
- **Samplette** del selector de tiendas del chat: abria su web sin llevarse la
  busqueda, asi que era un ajuste enganoso.

### Notes
- Los acordes pueden seguir usando color prestado de forma intencionada (bVII en
  mayor, V mayor en menor): es armonia correcta. La melodia, sin embargo, ya no
  se sale nunca de la escala que eliges.

## 1.1.0 - 2026-10-19

### Added
- **Auto-Sound (pestana SONIDO)**: cada idea elige automaticamente un sonido por
  capa desde tus carpetas de samples (bell/pluck para plugg y jerk, piano/rhodes
  para boom bap, leads para rage, 808 para el bajo, kick/snare/clap/hats/crash
  para la bateria). En melodia/acordes/bajo prefiere muestras cuya tonalidad
  coincide con la idea (pick armonico).
- **Render WAV por capa y mezcla**: "Escuchar" renderiza la idea con los sonidos
  elegidos (pitch-shift afinado por muestra); los chips Sample / Loop / Mezcla
  se arrastran directamente a FL (el nombre incluye el BPM del proyecto).
- **Otro / Variar todo**: variaciones ilimitadas de sonido; Replace de una capa
  tambien trae un sonido nuevo para esa capa.
- **Freesound integrado (opcional)**: boton Freesound en melodia, acordes y bajo.
  Con tu API key gratuita (freesound.org/apiv2/apply) busca y descarga previews
  CC0 a `Music\MidiForge_Cache` (se indexan tambien en SAMPLES). Sin key, el
  plugin sigue 100 % local.
- Boton **Web** por capa melodica: abre SampleFocus con la busqueda del genero.

### Notes
- La bateria sale siempre de tus packs locales; Freesound cubre melodia,
  acordes y bajo, y solo se consulta cuando pulsas su boton (nunca automatico).

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
- **SampleFocus + busquedas rapidas** en el Sample Scout: nueva tienda
  (samplefocus.com) y selector de busquedas rapidas por genero (JERK, Detroit,
  Trap, Plugg, Hip-Hop, UK, Synth, Chords, Melodies) que abre la busqueda en la
  tienda elegida.

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
