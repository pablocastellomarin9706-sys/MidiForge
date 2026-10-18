# Co-Producer (Output) — Analisis de referencia vs MidiForge

Documento companion de `REFERENCES.md`. Resume que es realmente Co-Producer,
en que se diferencia de MidiForge y que flujos merece la pena adaptar (offline).

Estado del instalador local: `C:\Users\Pablo\Music\co-producer-win-1.6.1.10884.exe`
— 152 MB, firma digital valida de Output, Inc. Corresponde a la ultima linea
publicada (1.6.1 / GUI 1.6.5 con Re-imagine 2.0, sep 2026). No instalado aun.

## 1. Que es Co-Producer (info publica oficial)

Plugin (VST/AU/AAX) de **curacion de samples session-aware**:

- Escucha el audio de tu sesion (armonia, ritmo, tempo, complejidad) y
  recomienda samples royalty-free de la nube de Output sincronizados a tu
  key y BPM.
- Flujos clave: busqueda Audio+Texto, busqueda solo Audio, Similar Sounds,
  drag & drop directo al DAW sin exportar.
- Re-imagine (variaciones de un sample, con slider de "Creativity";
  v2 desde sep 2026).
- Generate Kit: trocea un sample en un kit interpretable por cortes.
- Auto key detection (1.6.1): detecta la tonalidad de tu proyecto.
- Custom Collections (1.5): guarda samples en colecciones y ordenalas por
  key/BPM de la sesion actual.
- Integracion con Arcade (mandar samples entre plugins) y Pack Generator web.
- Modelo: suscripcion Output One; catalogo en nube (cuenta + conexion).

**Matiz clave: Co-Producer NO genera MIDI.** Es un buscador/curador de
samples. Lo "co-producer" es el flujo: sugiere material que encaja con la
sesion y tu decides. MidiForge cubre la otra mitad: ideas MIDI completas
(chords/melodia/bajo/bateria) 100% offline.

## 2. Comparativa directa

| Area | Co-Producer | MidiForge hoy |
| --- | --- | --- |
| Genera MIDI | No | Si (offline, seeded, personajes Detroit/Plugg/Rage/TrapLatino...) |
| Samples | Nube Output (suscripcion) | Tu disco (indexado) + enlaces de busqueda legales |
| Session-aware | Analiza el audio de la sesion | Lee el BPM del host; conoce su idea (key/barras) |
| Audicion de samples | Si, integrada | Si (doble clic, salida del plugin) |
| Organizacion | Colecciones, orden por key/BPM | Favoritos + chip FIT + similares + filtros BPM/Key |
| IA | Nube | 100% offline, algoritmica |
| Coste | Suscripcion mensual | Gratis, personal |

## 3. Flujos que si merece la pena adaptar (offline y legales)

1. **BPM del host**: leer el tempo del proyecto (`getPlayHead`) y alinear la
   idea al abrir. Es el 80% de la sensacion "encaja con tu sesion", aplicada
   a MIDI.
2. **Fit score en el SampleBrowser**: ordenar/etiquetar samples locales por
   cercania de BPM al proyecto + match de key (ya parseamos BPM/key de los
   nombres). Chip de color "FIT" cuando encaja.
3. **Favoritos / Colecciones**: estrella en la fila, persistido en el estado
   del plugin, lista de favoritos. Inspirado en Custom Collections.
4. **Buscar similares**: accion en la fila (mismo folder + base name +
   BPM parecido) al estilo "Similar Sounds".
5. **Slider de Creatividad**: control "cuanto se aleja la variacion"
   (humanizacion determinista extra: velocity, micro-timing, octavas).
   Inspirado en Re-imagine. Hecho como slider **Var** (0 = neutral).
6. **Audicion dentro del plugin**: reproducir el archivo seleccionado por la
   salida del plugin (AudioFormatManager + AudioTransportSource en
   processBlock). Hecho: doble clic en la fila o menu contextual.

## 4. Limites que nos autoimponemos

- No extraer ni reutilizar assets/codigo del instalador (EULA).
- No pedir cuenta/suscripcion de Output.
- No copiar su interfaz al detalle; MidiForge tiene ya su tema propio (Acero).

## 5. Estado del plan

- [x] 1 BPM host (cache del playhead + "Proyecto: X BPM" en el navegador)
- [x] 2 FIT (chip verde + filtro + orden por cercania de tempo y key)
- [x] 3 Favoritos (estrella + filtro + persistidos en el estado, v0.5.0)
- [x] 4 Similares (menu contextual; mismo nombre/BPM/folder, orden por score)
- [x] 5 Slider Var (variacion determinista; 0 = neutral)
- [x] 6 Audicion (doble clic; audio por la salida del plugin)
- [ ] Sesion A/B en FL cuando instales Co-Producer (capturas como referencia)
- [ ] Descarga legal via Freesound API (bloqueado: falta API key gratuita)
