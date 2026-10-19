# MidiForge

Plugin **VST3** de generación de ideas MIDI para **FL Studio** (Windows), inspirado en
Output *Co-Producer*. Genera **acordes, melodía, bajo y batería** de forma
**100 % offline** (algorítmico, sin APIs ni internet), permite **arrastrar los clips
MIDI** al Piano Roll y **arrastrar samples de tu disco** a la Playlist.

## Estado

- [x] Estructura CMake + JUCE
- [x] Generador offline (Chords / Melody / Bass / Drums) con seed determinista
- [x] UI "Acero azul minimal" (Header + Sidebar + Tabs) + panel de chat
- [x] Librería curada de progresiones y patrones por estilo
- [x] Juice (brillo) + Swing + Humanize + Replace por track
- [x] **Forge Chat** offline (ES/EN): ideas, replace, mute/solo, variaciones, parámetros
- [x] **Character presets** (7): Detroit / Trap Oscuro / Plugg / Nu-Jerk / Boom Bap /
      Rage / Trap Latino — swing, jitter humano, velocity, ghost hats, 808 glides
      y aire melodico; calibrados con tus referencias (`REFERENCES.md`)
- [x] **Sample Scout**: búsquedas en Splice / Freesound / Loopcloud / Samplette /
      YouTube / SoundCloud (abre el navegador; el plugin nunca accede a internet)
- [x] **Navegador de samples local**: indexa tus carpetas en segundo plano,
      detecta BPM/key por nombre, filtros y **drag de audio directo a FL**
- [x] **Co-Producer style (offline)**: BPM del proyecto + chip **FIT**, favoritos
      persistidos, **buscar similares**, slider **Var** (variacion) y **audicion**
      de samples dentro del plugin (doble clic)
- [x] Drag & drop de MIDI (temp `.mid` → drag nativo de Windows)
- [x] **Fuentes de samples**: Freesound / SampleFocus (cuentas gratuitas) → descarga
      manual a tus carpetas vigiladas; el plugin las indexa y audiciona (100 % offline)
- [x] **Contenido entregado (v1.0)**: JERK, PLUGG y Boom Bap con patrones, 808 y
      progresiones propios (mismos grooves que los .mid de `MidiForge_Entregas`)
- [ ] Integracion directa Freesound API (opcional; requiere API key gratuita)
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
2. Ajusta **Key**, **Scale**, **Style**, **Bars**, **Seed**, **Juice** y **Var**, o pide ideas
   en el **Forge Chat** (por ejemplo: *"trap oscuro en F menor"*).
3. Elige **Caracter** en la barra lateral (Detroit, Trap Oscuro, Plugg, Nu-Jerk,
   Boom Bap, Rage, Trap Latino) para el "sabor" del beat: swing, timing humano,
   ghost hats, 808 glides.
4. Ve a la pestaña **SAMPLES** para navegar tu carpeta de musica (indexado en
   segundo plano, filtros de BPM/key) y **arrastra el archivo a la Playlist de FL**.
   - **FIT**: chip verde + filtro para los samples que cuadran con el BPM del
     proyecto (dale al play en FL una vez para que el plugin lea el tempo).
   - **Estrella** (columna izquierda): favoritos persistentes; filtro **Favoritos**.
   - **Clic derecho** en una fila: *Buscar similares*, escuchar/parar, anadir a
     favoritos o revelar en el explorador.
   - **Doble clic** en una fila: audicion dentro del plugin (otra vez, para).
5. Pulsa **Replace** en cualquier track para obtener otra variante de esa capa.
6. Arrastra desde **"Drag MIDI → Track"** hasta el **Piano Roll** de FL.

### Forge Chat (offline) — ejemplos

| Escribes | Resultado |
| --- | --- |
| `trap oscuro en F menor` | Nuevo beat: estilo, carácter, tonalidad y brillo |
| `hazme algo detroit` / `boom bap oscuro` | Cambia el Carácter (swing + humanize + ghost hats) |
| `un beat latino` / `trap latino oscuro` | Carácter Trap Latino (menor armónica + 808 glides) |
| `algo rage` / `estilo yeat` | Carácter Rage (lead oscuro, 808 duro) |
| `hazme house alegre de 8 compases` | Nueva idea House en mayor, 8 compases |
| `cambia la melodia` / `otra bateria` | Replace solo de esa capa |
| `solo bateria y bajo` / `quita los acordes` | Silencia capas (persiste al regenerar) |
| `mas swing` / `mas brillo` / `mas oscuro` | Ajusta swing o Juice |
| `otra variacion` / `nueva idea` | Nueva toma con la misma seed / idea nueva |
| `busca hi-hats oscuros` | **Sample Scout**: abre búsqueda en la fuente elegida |

## Estructura

```
src/
  Chat/        Forge Chat: parser offline ES/EN + Sample Scout
  Content/     Progresiones, patrones y presets de Carácter
  Core/        Tipos, teoría musical, RNG
  Generator/   Generadores por track + orquestador (DNA del carácter)
  MIDI/        Construcción de MidiFile + drag externo
  Samples/     Indexador local de samples (hilo de fondo, BPM/key)
  UI/          LookAndFeel, MainComponent y componentes
  PluginProcessor / PluginEditor
```

## Notas técnicas

- El drag usa `performExternalDragDropOfFiles`: `.mid` temporal para clips MIDI y
  el archivo de audio original para samples (FL lo copia al proyecto).
- El generador es determinista respecto a la `seed`; el Carácter aplica un
  post-proceso de humanización (jitter, velocity, longitudes, ghost hats).
- Las carpetas del navegador y los **favoritos** se guardan con el estado del plugin.
- **Var** (variacion) por defecto 0 (neutral): suma humanizacion determinista
  (velocity, micro-timing, octavas) sobre la idea generada sin tocar la seed.
- La **audicion** reproduce el archivo por la salida del plugin (doble clic);
  formatos: wav/aif/aiff/mp3/ogg/flac (lectores basicos de JUCE).
- El chat es un parser de intenciones 100 % local (sin red).
- Sample Scout solo **abre el navegador** con la búsqueda preparada.
- Límite de indexado: 40.000 archivos de audio (wav/aif/aiff/mp3/ogg/flac).

## Fuentes de samples (Freesound / SampleFocus)

El plugin **no descarga nada** (100 % offline). Tu flujo: descarga el sample a una
carpeta vigilada del navegador (por ejemplo `C:\Users\Pablo\Music\FL\Sample Packs`)
y el plugin lo indexa solo (BPM/key por nombre, filtros, audicion, drag a FL).

**Freesound** (freesound.org) — cuenta gratuita:
1. Crea la cuenta y entra (Join, arriba a la derecha).
2. Descarga desde la web (boton *Download* en cada sonido); cada ficha indica su
   licencia (CC0 no exige atribucion; CC-BY si requiere mencion).
3. (Opcional, para automatizar mas adelante) **API key**: en tu cuenta ve al
   formulario de aplicaciones de la API (`freesound.org/apiv2/apply`),
   *Request new access credentials*, rellena nombre y descripcion, acepta las
   condiciones; la key aparece al momento. Con key se puede buscar en la API;
   descargar originales requiere OAuth2 (previews con la key simple).

**SampleFocus** (samplefocus.com) — cuenta gratuita:
1. Sign up (email o Google); confirma el correo si te lo pide.
2. Descarga: icono amarillo en la tarjeta, o boton *Download* en la ficha.
3. Plan free: 3 creditos de descarga cada lunes + extras por subir samples
   (4 por upload aprobado). Todos los sonidos son royalty-free sin atribucion
   (Standard License).

## Licencia / aviso

Proyecto de aprendizaje inspirado funcionalmente en Co-Producer (Output). No incluye
ni redistribuye assets, sonidos ni marcas de Output. JUCE se usa bajo su licencia
correspondiente (AGPLv3 o comercial).
