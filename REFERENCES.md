# Referencias A/B (calibracion)

Referencias aportadas por el usuario para calibrar los Character presets.
Mapeadas a rasgos MIDI concretos (swing, jitter, densidad, extensiones, registros).

## 1. Plugg / Rage / Underground US-UK

| Artista | Rasgos | Implicacion |
| --- | --- | --- |
| Pi'erre Bourne | plugg clasico: bells detuneadas, 808 glides, mucho espacio | Plugg: swing ~9%, bajo legato (glide), melodia +1 octava, baja densidad |
| Yeat + AlanTheChemist | rage: leads oscuros, 808 duro, staccato energetico | Nuevo preset Rage |
| Fakemink | UK, jerk "mellowed", cloud rap | Nu-Jerk v2: menos agresivo, mas haze |
| Payme Skrilla | Philly (Kensington), trap crudo | Trap Oscuro seco, pocos elementos |

## 2. Boom bap / sampling

| Artista | Rasgos | Implicacion |
| --- | --- | --- |
| Lord Finesse | DITC, boom bap 90s sampleado | Swing 20-24%, extensiones jazz 7/9/11 |
| Cookin Soul | flips soul/funk | Sincopa de chop, velocities amplias |
| Gloosito | scam rap ES + sampleo | BoomBap con groove sucio |
| Dano | rap ES, lirico | Progresiones calidas (menor natural/Dorian) |

## 3. Trap Latino / ES

| Artista | Rasgos | Implicacion |
| --- | --- | --- |
| JC Reyes | trap/drill ES | Nuevo preset Trap Latino: 808 glides, 140-150 |
| 8Belial + Enzocerobulto | DISOBEY Society (Madrid), trap/electronica | Melodias oscuras con pulso bailable |
| El Bugg | neoperreo/cloud (Madrid) | 100-110 BPM, dembow suave |
| Huan 62 | trap crudo RD | Hats densos, 808 sostenidos |
| Fanta Rosario | urbano PR (trap+reggaeton) | Dembow + trap hibrido |
| El Americano 4KT | trap Colombia (Buenaventura) | Menor simple, actitud cruda |
| NTG | Bronx, spanglish trap latino | Melodias tristes + flow agresivo |
| Cero* | canal ES underground | Justifica preset latino para escena local |

## 4. Calibracion pendiente

- [x] Trap Latino/ES: swing 5%, hats fantasma 45%, 808 glides (bassLenScale 1.7),
      densidad 0.9, menor armonica por defecto (desde Mayor).
- [x] Rage: swing 2%, velSigma 9, notas cortas (0.70), 808 glides 1.5, densidad 0.85.
- [x] Plugg v2: swing 9%, bajo legato (1.6), densidad 0.75 (aire), +1 octava.
- [x] Nu-Jerk v2 (Fakemink): relajado (swing 4%, len 0.75, ghosts 0.18, densidad 0.80).
- [ ] BoomBap: +sincopa de chop (base swing 22% ya implementada; menos prioritario).
- [x] Character base (Detroit / Trap Oscuro / Plugg / Nu-Jerk / Boom Bap)
      implementado en la Fase 4.

Nota: los tempos son orientativos (FL Studio es dueno del tempo). El plugin
solo fija el "feel" (swing, jitter, longitudes) y el contenido (notas).
