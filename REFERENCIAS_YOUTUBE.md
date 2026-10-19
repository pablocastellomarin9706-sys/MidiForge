# Referencias de YouTube (analisis de contenido)

Base de conocimiento derivada de los enlaces aportados por el usuario para
**perfeccionar el plugin**. Sintesis **honesta del metodo**: se hizo con
herramientas (metadatos, capitulos, transcripciones y medicion espectral de
audio con `librosa`), **no** con escucha humana ni juicio de oido. Los numeros
sirven como orientacion, no como verdad absoluta.

Restriccion respetada: **no se descargo nada al equipo del usuario**. Todo el
material intermedio (audio, video, miniaturas, subtitulos) se elimino; solo se
conservaron notas de texto y las conclusiones que alimentan el plugin.

---

## 1. Que aporta cada referencia

| Referencia | Canal | Tipo | Lo que aporta al plugin |
| --- | --- | --- | --- |
| `ZLEOd8LJNAE` (fakemink jerk) | mayze | tutorial | DNA Nu-Jerk: 184 BPM, Cm; pad drone (notas sueltas), capa duplicada una octava abajo, snare, hood-trap, **nota fantasma de baja velocity**, 808 simple, *beat cuts* |
| `z0s9njCatwk` | mayze | beat | 92.29 BPM (~half-time), D#m; cortes de graves en el arrangement |
| `3DcobO7snJE` (nettspend) | mayze | beat | 143.55 BPM, D#m, centroide 649 Hz (oscuro), densidad creciente |
| `5uLs9SEzV-g` (yung fazo) | mayze | beat | ~191 BPM half-time, Cm, flatness alto (textura "deep-fried") |
| `1OTlRPeytVg` | Insulation (OGX Beats x Jouta) | sesion | **Detroit real**: melodies primero, dobladas/distorsionadas; menor armonica para oscuridad; Humanize; duplicar + **reversa**; el silencio importa; 808 raiz -4 semitonos; **soft clipper** (no mezcla); ARP 1/4 + multiply 50% + cinta; 6-10 beats en 3 h |
| `_hZstZn-CNs` | ServidaMusic | tutorial | **6/8**: ajustar proyecto a 6/8 (o 3/4) y rejilla 1/3-1/6; R&B 120-130 con swing + ghost kicks; cuerdas pizzicato/tremolo; acorde + flourish + turnaround; segundo beat 4/4 en tresillos ~105 BPM |
| `RfXqVI5R65c` ("going for moods") | Nick Mira | stream | **mood = piano roll**; energy = BPM + key; el mismo BPM/efectos en todo hace que los beats suenen iguales |
| `7_6czDqDVJc` | Rio Leyva | beat | 143.55 BPM, Am; densidad que sube |
| `Sm1wSNupKTA` | Oscar Calmaestra | sesion | centroide 1368 Hz: mezcla cargada de graves, densidad de low-end |
| `ziw8k8FouAw` | Lucid Beats | remake | 89.10 BPM, Dm: tempo lento tipo plugg/etereo |
| `6y_JqVR_bUo` | PolBeats | short | "las melodias no necesitan mas notas" (aire/confianza) |

(El inventario completo de 66 videos, playlists y busquedas vive en el
material de analisis; aqui solo lo que cambia el plugin.)

## 2. Lectura musical consolidada

- **Nu-Jerk**: half-time rapido (visto como ~184 o ~89-95 BPM segun como se
  cuente), menor, melodias/pads **dronales** con muchisimo aire, notas
  **staccato**, una **nota fantasma** de baja velocity en la bateria y 808
  simple. Color "hazy" aunque sea agresivo.
- **Detroit**: ~130 BPM, menor (con color de **menor armonica**), todo
  construido **desde la melodia**, melodias dobladas y saturadas, uso del
  **silencio** como elemento, 808 con raiz desplazada (-4 semitonos en la
  referencia), final con **soft clipper** en vez de mezcla.
- **Trap Oscuro**: rollings de 808, hats densos, centroide bajo (oscuro),
  densidad que crece dentro del compas.
- **Plugg**: tempo lento (~89), bells/pads, **mucho espacio**, 808 con glide,
  melodia +1 octava, densidad baja.
- **Rage**: energetico, notas cortas, hats densos, 808 duro.
- **Trap Latino**: base menor, V mayor (color de menor armonica), pulso
  bailable dembow.
- **Trap generico / BoomBap**: sin cambios estructurales; BoomBap conserva su
  swing amplio.

## 3. Lo que se instalo en el plugin (esta version)

1. **Contenido consciente del `Character`** (`src/Content/Patterns.*`,
   `src/Content/Progressions.*`): antes Detroit, jerk y plugg compartian los
   mismos patrones genericos de HipHop. Ahora cada personaje tiene **2 patrones
   de bateria, 2 de 808 y 2 progresiones** propios, con *fallback* a los
   genericos cuando no aplica.
2. **Escala por defecto del personaje** (`src/Content/Characters.h`,
   `defaultScaleFor`): al elegir un personaje de familia menor con la idea en
   Mayor, se ajusta a su modo (Detroit/TrapDark/Plugg/NuJerk/Rage -> menor
   natural; TrapLatino -> menor armonica). Se conserva el comportamiento previo
   de Trap Latino.
3. **Recalibracion Nu-Jerk** (`Characters.h`): notas mas staccato (0.65),
   ghost 0.20, densidad 0.85, swing 6% — alineado con el jerk de mayze.
4. Bateria: anadidos `kRim` y `kCrash` para acentos.

## 4. Pendiente (siguientes pasos)

- [ ] **6/8** (Servida): soportar compas 6/8 (o 3/4) y rejilla 1/3-1/6.
- [ ] **Tresillos / shuffle** explicitos (R&B 105 BPM).
- [ ] Melodia **doblada una octava abajo** como opcion (Detroit).
- [ ] "Reversa + hueco" como recurso de motivo (Detroit / Silencio).
- [ ] Ajuste de **tuning** del 808 (-4 semitonos) como parametro de textura,
      no de armonia (no forzar la raiz).
- [ ] Groove "deep-fried" (flatness alta) como preset de saturacion del sample.
- [ ] Revisar que el fallback de progresiones por familia no introduzca el V
      mayor bajo melodia de menor natural de forma molesta (hoy es intencional).

---

_Nota de metodo: medidas de BPM/tonalidad/centroide/flatness hechas con
`librosa` sobre extractos; transcripciones con `youtube-transcript-api`.
No sustituyen a la escucha humana._
