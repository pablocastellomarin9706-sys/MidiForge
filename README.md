# MidiForge

Plugin **VST3** de generación de ideas MIDI para **FL Studio** (Windows), inspirado en
Output *Co-Producer*. Genera **acordes, melodía, bajo y batería** de forma
**100 % offline** (algorítmico, sin APIs ni internet) y permite **arrastrar los clips
MIDI** directamente al Piano Roll / Channel Rack.

## Estado

- [x] Estructura CMake + JUCE
- [x] Generador offline (Chords / Melody / Bass / Drums) con seed determinista
- [x] UI oscura (Header + Sidebar + Tabs por track) + panel de chat
- [x] Librería curada de progresiones y patrones por estilo (Pop/LoFi/House/HipHop/EDM/Rock)
- [x] **Juice** (brillo: octavas, 9ªs, velocity) y **Swing**
- [x] **Replace** por track (cambia una sola capa por otra opción curada)
- [x] **Forge Chat** offline (español/inglés) para pedir ideas y cambios
- [x] **Sample Scout**: prepara búsquedas en Splice / Freesound / Loopcloud / Samplette
- [x] Drag & drop de MIDI (`.mid` temporal → drag nativo de Windows)
- [ ] Navegador de samples local con drag de audio (fase siguiente)
- [ ] Mini editor de piano roll editable (fase siguiente)

## Requisitos (para compilar)

- **Windows 10/11 x64**
- **Visual Studio 2022** con "Desktop development with C++" (o Build Tools)
- **CMake 3.22+**
- Conexión a internet la primera vez (CMake descarga JUCE 8.0.8 vía `FetchContent`)

## Compilar (rápido)

```powershell
# Desde la raíz del proyecto
cmake -B build -A x64
cmake --build build --config Release
```

El VST3 resultante queda en:

```
build/MidiForge_artefacts/Release/VST3/MidiForge.vst3
```

## Instalar en FL Studio

1. Copia (o haz un enlace) la carpeta `MidiForge.vst3` a:

   ```
   C:\Program Files\Common Files\VST3\
   ```

   (Necesita permisos de administrador.)

2. En FL Studio: **Options → Manage plugins → Find more plugins**.
3. Aparecerá como **MidiForge** (categoría *Instrument*).

## Uso

1. Abre MidiForge en el Channel Rack o en un slot de mezcla.
2. Ajusta **Key**, **Scale**, **Style**, **Bars**, **Seed** y **Juice**, o pide ideas en el
   **Forge Chat** (por ejemplo: *"trap oscuro en F menor"*).
3. Ve a la pestaña del track que quieras (Chords / Melody / Bass / Drums).
4. Arrastra desde **"Drag MIDI → Track"** hasta el **Piano Roll** o el **Channel Rack** de FL.
5. Pulsa **Replace** en cualquier track para obtener otra variante de esa capa.

### Forge Chat (offline)

Ejemplos de comandos:

| Escribes | Resultado |
| --- | --- |
| `trap oscuro en F menor` | Nuevo beat: estilo, tonalidad y brillo ajustados |
| `hazme house alegre de 8 compases` | Nueva idea House en mayor, 8 compases |
| `cambia la melodia` / `otra bateria` | Replace solo de esa capa |
| `solo bateria y bajo` / `quita los acordes` | Silencia capas (persiste al regenerar) |
| `mas swing` / `mas brillo` / `mas oscuro` | Ajusta swing o Juice |
| `otra variacion` / `nueva idea` | Nueva toma con la misma seed / idea nueva |
| `busca hi-hats oscuros` | **Sample Scout**: abre búsqueda en la tienda elegida |

## Estructura

```
src/
  Chat/        Forge Chat: parser offline ES/EN + Sample Scout
  Content/     Librería curada de progresiones y patrones
  Core/        Tipos, teoría musical, RNG
  Generator/   Generadores por track + orquestador
  MIDI/        Construcción de MidiFile + drag externo
  UI/          LookAndFeel, MainComponent y componentes
  PluginProcessor / PluginEditor
```

## Notas técnicas

- El drag usa `juce::DragAndDropContainer::performExternalDragDropOfFiles` con un
  `.mid` temporal en `%TEMP%\MidiForge\`. Es el mecanismo que FL Studio acepta al
  soltar sobre el Piano Roll.
- El generador es determinista respecto a la `seed`: la misma seed produce la misma idea.
- El chat es un parser de intenciones 100 % local (sin red): diccionario de géneros,
  moods, tonalidades y acciones en español e inglés.
- Sample Scout solo **abre el navegador** con la búsqueda preparada; el plugin nunca
  accede a internet.

## Licencia / aviso

Proyecto de aprendizaje inspirado funcionalmente en Co-Producer (Output). No incluye
ni redistribuye assets, sonidos ni marcas de Output. JUCE se usa bajo su licencia
correspondiente (AGPLv3 o comercial).
