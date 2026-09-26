# Índice de Tests — Codexion

Suite modular de pruebas para el binario `coders/codexion`. Cada entrada describe
el **argumento** que se ejecuta y **qué se valida** en ese test concreto.

Todos los comandos asumen el binario en `coders/codexion`. La firma es:

```
./codexion <num_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <compiles_requeridas> <dongle_cooldown> <scheduler>
```

**Reglas transversales** que se comprueban en los tests viables:

* **C1** — Formato estricto por línea: `<timestamp> <coder_id> <mensaje>`
* **C2** — Mensajes válidos: `has taken a dongle | is compiling | is debugging | is refactoring | burned out`
* **C3** — Dos tomas de dongle antes de cada `is compiling` (dos dongles simultáneos)
* **C4** — Timestamps monótonos (global y por coder)
* **C5** — Todos los coders `1..N` aparecen y completan las compilaciones requeridas
* **C6** — `burned out` aparece exactamente una vez y como **última línea**
* **C7** — Ningún coder puede emitir un evento propio (toma de dongle, compile, debug o refactor) después de su deadline teórico `last_compile_start + time_to_burnout` (o `time_to_burnout` desde el inicio si aún no compiló) y antes del `burned out`. Detecta deaths enmascaradas, como el caso reportado del coder que recompila en el ms ~120 cuando su deadline era 110.
* **C8** — `timeout`(124)=No Definido; `139`/`134` o log vacío = FAIL

---

## [TEST 01] 2 coders, FIFO, cooldown 0

* **Comando:** `./codexion 2 800 200 100 100 3 0 fifo`
* **Propósito:** Caso base viable con 2 coders, planificador `fifo` y cooldown de dongle nulo: ambas personas deben completar las 3 compilaciones obligatorias sin agotarse.
* **Comportamiento Esperado:**
  * **Exit Code:** `0`
  * **Salida Log:** Formato C1-C5: 2 tomas de dongle por cada `is compiling`, timestamps monótonos, y `>= 3` compilaciones por coder.

## [TEST 02] 2 coders, EDF con cooldown 50

* **Comando:** `./codexion 2 800 200 100 100 3 50 edf`
* **Propósito:** Verificar el planificador `edf` (Earliest Deadline First) con un cooldown de 50ms entre liberación y nueva toma de dongle; los parámetros son viables, así que EDF debe completar la simulación `exit 0`.
* **Comportamiento Esperado:**
  * **Exit Code:** `0`
  * **Salida Log:** Formato C1-C5 con `>= 3` compilaciones por coder, respetando el cooldown y con mensajes no mezclados (log serializado).

## [TEST 03] 3 coders, FIFO, mesa circular

* **Comando:** `./codexion 3 800 150 100 100 4 0 fifo`
* **Propósito:** Tres personas en hub circular: cada coder comparte sus dos dongles con los vecinos. Valida el arbitraje `fifo` cuando 3 personas compiten por dongles contiguos.
* **Comportamiento Esperado:**
  * **Exit Code:** `0`
  * **Salida Log:** Formato C1-C5, `>= 4` compilaciones por coder, sin agotamiento.

## [TEST 04] 4 coders, EDF con cooldown 100

* **Comando:** `./codexion 4 900 200 150 150 5 100 edf`
* **Propósito:** Cuatro coders con cooldown más exigente (100ms) bajo `edf`; valida que el planificador por deadline evita el agotamiento con presupuesto acotado.
* **Comportamiento Esperado:**
  * **Exit Code:** `0`
  * **Salida Log:** Formato C1-C5, `>= 5` compilaciones por coder, timestamps monótonos.

## [TEST 05] 5 coders, FIFO regimen ajustado

* **Comando:** `./codexion 5 800 200 200 200 5 0 fifo`
* **Propósito:** Régimen ajustado: ciclo por coder (200+200+200=600ms) pegado al margen de agotamiento (800ms). Valida que `fifo` reparte el recurso sin inanición ni burnout con margen justo.
* **Comportamiento Esperado:**
  * **Exit Code:** `0`
  * **Salida Log:** Formato C1-C5, `>= 5` compilaciones por coder.

## [TEST 06] 8 coders, EDF cooldown 50

* **Comando:** `./codexion 8 1500 300 200 200 4 50 edf`
* **Propósito:** Ocho coders compitiendo por dongles compartidos con `edf` y cooldown de 50ms; valida viabilidad en mesa con más contendientes.
* **Comportamiento Esperado:**
  * **Exit Code:** `0`
  * **Salida Log:** Formato C1-C5, `>= 4` compilaciones por coder (los 8 coders aparecen).

## [TEST 07] 10 coders, FIFO baja contencion

* **Comando:** `./codexion 10 2000 200 100 100 3 0 fifo`
* **Propósito:** Diez coders con ciclo corto (400ms) frente a un margen amplio (2000ms): baja contención. Valida `fifo` a mayor escala sin agotamiento.
* **Comportamiento Esperado:**
  * **Exit Code:** `0`
  * **Salida Log:** Formato C1-C5, `>= 3` compilaciones para los 10 coders.

---

## [TEST 08] 50 coders, EDF cooldown 10

* **Comando:** `./codexion 50 2000 200 100 100 5 10 edf`
* **Propósito:** Gran escala: 50 coders bajo `edf` con cooldown de 10ms. Valida rendimiento y correcta planificación sin agotarse ni colgarse.
* **Comportamiento Esperado:**
  * **Exit Code:** `0`
  * **Salida Log:** Formato C1-C5, `>= 5` compilaciones por coder (los 50 coders completan).

## [TEST 09] 100 coders, FIFO

* **Comando:** `./codexion 100 3000 300 150 150 3 0 fifo`
* **Propósito:** Cien coders en mesa circular con `fifo`; valida la cola de prioridad FIFO a escala y que todos completen sus compilaciones.
* **Comportamiento Esperado:**
  * **Exit Code:** `0`
  * **Salida Log:** Formato C1-C5, `>= 3` compilaciones por coder.

## [TEST 10] 200 coders, EDF stress

* **Comando:** `./codexion 200 3000 300 200 200 3 0 edf`
* **Propósito:** Estrés: 200 coders bajo `edf`. Valida la cola de prioridad (heap) con deadlines en competencia masiva.
* **Comportamiento Esperado:**
  * **Exit Code:** `0`
  * **Salida Log:** Formato C1-C5, `>= 3` compilaciones por coder.

## [TEST 11] 500 coders, FIFO escala maxima

* **Comando:** `./codexion 500 5000 400 200 200 2 0 fifo`
* **Propósito:** Escala máxima: 500 coders con el requisito mínimo (2 compilaciones) y margen amplio; valida estabilidad del arbitraje `fifo`.
* **Comportamiento Esperado:**
  * **Exit Code:** `0`
  * **Salida Log:** Formato C1-C5, `>= 2` compilaciones por coder (los 500 completan).

## [TEST 12] Estres en rafaga: 10 corridas de 30 coders (2s c/u)

* **Comando:** `./codexion 30 1500 200 100 100 3 0 fifo` (repetido 10 veces con timeout de 2s)
* **Propósito:** Robustez ante ejecuciones repetidas y rápidas: el binario no debe crash-ar (segfault `139`/abort `134`) ni producir logs vacíos en ninguna corrida.
* **Comportamiento Esperado:**
  * **Exit Code:** No aplica por corrida; se cuentan crashes y logs vacíos.
  * **Salida Log:** `0` crashes (`139`/`134`) y `0` logs vacíos en las 10 corridas.

---

## [TEST 13] 2 coders inviables: ciclo > burnout (FIFO)

* **Comando:** `./codexion 2 300 200 200 200 5 0 fifo`
* **Propósito:** Configuración inviable: el ciclo completo (200+200+200=600ms) supera el tiempo de agotamiento (300ms). El monitor debe detectar el burnout y detener la simulación.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0` (terminación temprana; no `0` ni timeout `124`)
  * **Salida Log:** `burned out` exactamente una vez y como **última línea** (Regla C6); además no deben existir eventos del coder tras su deadline de 300ms (Regla C7).

## [TEST 14] 1 coder, dongle único

* **Comando:** `./codexion 1 300 500 100 100 5 0 fifo`
* **Propósito:** Validar la salida temprana cuando un solo coder intenta operar sin la cantidad mínima de dongles requerida (necesita 2 y con una sola persona solo hay 1 dongle en la mesa).
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0` (no debe terminar con `0` ni dar *timeout* `124`)
  * **Salida Log:** El coder 1 debe registrar `burned out` como la **última línea** de salida (Regla C6); sin eventos propios tras su deadline de 300ms (Regla C7).

## [TEST 15] 4 coders, inanicion FIFO, margen nulo

* **Comando:** `./codexion 4 400 200 100 100 10 0 fifo`
* **Propósito:** Ciclo por coder (400ms) igual al tiempo de agotamiento (margen nulo). Con `fifo` puede aparecer inanición: el test acepta **o** la finalización completa **o** el burnout correctamente detectado.
* **Comportamiento Esperado:**
  * **Exit Code:** `0` (completó las 10 compiles) o `!= 0` (burnout detectado)
  * **Salida Log:** Si hay burnout, `burned out` única vez y como última línea (C6).

## [TEST 16] EDF tambien detecta burnout (inviable)

* **Comando:** `./codexion 3 300 300 200 200 5 0 edf`
* **Propósito:** Parámetros inviables (ciclo 700ms > burnout 300ms) con planificador `edf`: el monitor debe detectar el agotamiento también bajo EDF.
* **Comportamiento Esperado:**
  * **Exit Code:** `0` (si con margen completó) o `!= 0` (burnout con `edf`)
  * **Salida Log:** Si hay burnout, `burned out` única vez y como última línea (C6).

## [TEST 17] Precision del monitor: delay del log de burnout

* **Comando:** `./codexion 2 300 200 200 200 5 0 fifo`
* **Propósito:** Mide la precisión del monitor: `delay = ts_burnout - último "is compiling" - time_to_burnout`. La spec exige medir dentro de los ≤10ms reales (se tolera hasta 25ms).
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0` (burnout)
  * **Salida Log:** delay acotado: `<= 10ms` (ideal, spec) o `<= 25ms` (aceptable por poll + tolerancia).

## [TEST 39] Caso reportado: verificacion regresiva del bug del monitor

* **Comando:** `./codexion 2 110 59 30 30 2 1 fifo`
* **Propósito:** Regresión del caso reportado por el evaluador: en el ms ~120 el coder 1 seguía compilando cuando debería haber muerto en el ms 110. El ciclo (59+30+30=119ms) supera el burnout (110ms), así que la simulación **debe** terminar siempre por agotamiento, nunca con exit `0`.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0` (burnout; nunca `0` ni timeout `124`)
  * **Salida Log:** `burned out` exactamente una vez y como **última línea** (Regla C6); sin eventos del coder tras su deadline de 110ms (Regla C7, falla el binario buggy si recompila en el ms 120); delay del log ≤ 10ms.

## [TEST 40] Banda critica +10ms: borde del agotamiento

* **Comando:** `./codexion 2 110 60 30 30 2 1 fifo`
* **Propósito:** Variante de regresión en el borde de la banda crítica: ciclo (60+30+30=120ms) = burnout (110ms) + 10ms (la ventana exacta del poll). El monitor debe ganar la carrera contra el reseteo del coder y terminar por agotamiento.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0` (burnout; el coder coincide con el último momento viable para recompilar)
  * **Salida Log:** `burned out` única vez y como última línea (Regla C6); sin eventos tras el deadline de 110ms (Regla C7).

---

## [TEST 18] Sin argumentos

* **Comando:** `./codexion`
* **Propósito:** Todos los argumentos son obligatorios; sin ellos el parsing debe rechazar la entrada.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0`
  * **Salida Log (stderr):** Mensaje de error de parsing.

## [TEST 19] Argumentos faltantes

* **Comando:** `./codexion 4 800 200 100 100`
* **Propósito:** Faltan `compiles_requeridas`, `cooldown` y `scheduler`; el parser debe detectar aridad incompleta.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0`
  * **Salida Log (stderr):** Mensaje de error.

## [TEST 20] Argumentos de mas

* **Comando:** `./codexion 4 800 200 100 100 5 10 fifo extra`
* **Propósito:** Sobran argumentos; el parser debe rechazar la entrada con error.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0`
  * **Salida Log (stderr):** Mensaje de error.

## [TEST 21] Numero de coders negativo

* **Comando:** `./codexion -5 800 200 100 100 5 10 fifo`
* **Propósito:** `numero de coders` negativo es inválido (números negativos rechazados).
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0`
  * **Salida Log (stderr):** Mensaje de error.

## [TEST 22] Tiempo negativo

* **Comando:** `./codexion 4 -800 200 100 100 5 10 fifo`
* **Propósito:** Tiempos (en ms) negativos son inválidos.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0`
  * **Salida Log (stderr):** Mensaje de error.

## [TEST 23] Cooldown negativo

* **Comando:** `./codexion 4 800 200 100 100 5 -10 fifo`
* **Propósito:** `dongle_cooldown` negativo es inválido.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0`
  * **Salida Log (stderr):** Mensaje de error.

## [TEST 24] Cero en numero de coders

* **Comando:** `./codexion 0 800 200 100 100 5 10 fifo`
* **Propósito:** `numero de coders = 0` es inválido (no hay simulador que orquestar).
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0`
  * **Salida Log (stderr):** Mensaje de error.

## [TEST 25] Cero en tiempos (3 variantes)

* **Comando:** `./codexion 4 0 200 100 100 5 10 fifo` / `./codexion 4 800 0 100 100 5 10 fifo` / `./codexion 4 800 200 0 100 5 10 fifo`
* **Propósito:** Tiempos en cero inválidos (burnout, compile, debug): se rechazan las 3 variantes.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0` en las 3 variantes
  * **Salida Log (stderr):** Mensaje de error por variante.

## [TEST 26] Entrada no numerica

* **Comando:** `./codexion 4 800 abc 100 100 5 10 edf`
* **Propósito:** Un valor no numérico (`abc`) en una posición de tiempo debe rechazarse.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0`
  * **Salida Log (stderr):** Mensaje de error.

## [TEST 27] Valores no enteros (flotantes)

* **Comando:** `./codexion 4 800 1.5 100 100 5 10 edf` / `./codexion 4 3.14 200 100 100 5 10 fifo`
* **Propósito:** Los valores deben ser enteros; los flotantes (`1.5`, `3.14`) se rechazan.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0` en las 2 variantes
  * **Salida Log (stderr):** Mensaje de error.

## [TEST 28] Sufijo / mezcla basura

* **Comando:** `./codexion 4 800 200ms 100 100 5 10 edf` / `./codexion 4 800 200 100 100 5 10a edf`
* **Propósito:** Sufijos (`200ms`) o basura tras un número (`10a`) invalidan la entrada.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0` en las 2 variantes
  * **Salida Log (stderr):** Mensaje de error.

## [TEST 29] Espacios, signo y cadena vacia

* **Comando:** `"./codexion " 4 800 200 100 100 5 10 fifo` / `./codexion +4 800 200 100 100 5 10 fifo` / `./codexion "" 800 200 100 100 5 10 fifo`
* **Propósito:** Espacios iniciales, signo `+` explícito y cadena vacía son entradas inválidas (3 variantes).
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0` en las 3 variantes
  * **Salida Log (stderr):** Mensaje de error.

## [TEST 30] Overflow (mas alla de INT_MAX)

* **Comando:** `./codexion 2147483648 800 200 100 100 5 10 fifo` / `./codexion 999999999999 800 200 100 100 5 10 fifo`
* **Propósito:** Números que desbordan `INT_MAX` deben rechazarse (no truncarse).
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0` en las 2 variantes
  * **Salida Log (stderr):** Mensaje de error.

## [TEST 31] Scheduler invalido

* **Comando:** `./codexion 4 800 200 100 100 5 10 round_robin`
* **Propósito:** El scheduler solo admite `fifo` o `edf`; cualquier otro valor (`round_robin`) se rechaza.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0`
  * **Salida Log (stderr):** Mensaje de error.

## [TEST 32] Scheduler case / parcial / con espacios

* **Comando:** `./codexion 4 800 200 100 100 5 10 FIFO` / `./codexion 4 800 200 100 100 5 10 fif` / `./codexion 4 800 200 100 100 5 10 "edf "`
* **Propósito:** Mayúsculas (`FIFO`), scheduler parcial (`fif`) o con espacio (`edf `) son inválidos: el valor debe ser exactamente `fifo` o `edf`.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0` en las 3 variantes
  * **Salida Log (stderr):** Mensaje de error.

## [TEST 33] Scheduler numerico o vacio

* **Comando:** `./codexion 4 800 200 100 100 5 10 1` / `./codexion 4 800 200 100 100 5 10 ""`
* **Propósito:** Scheduler numérico (`1`) o vacío (`""`) se rechazan.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0` en las 2 variantes
  * **Salida Log (stderr):** Mensaje de error.

## [TEST 34] Frontera valida: cooldown=0 se acepta (no es error de parseo)

* **Comando:** `./codexion 2 800 200 100 100 3 0 fifo`
* **Propósito:** Contraejemplo: `cooldown = 0` **es válido** y la simulación debe arrancar (exit `!= 1`). Verifica que el parser no confunde `0` en el cooldown con error.
* **Comportamiento Esperado:**
  * **Exit Code:** `0` (simulación válida C1-C5) **o** `124` (timeout: arrancó y el test la cortó — parseo OK)
  * **Salida Log:** Si `exit 0`, formato C1-C5 válido.

---

## [TEST 35] Valgrind: ejecucion normal (viable)

* **Comando:** `./codexion 4 800 200 100 100 3 10 edf` (bajo `valgrind --leak-check=full -s`)
* **Propósito:** La ejecución viable completa no debe tener errores de memoria ni fugas (`definitely lost`).
* **Comportamiento Esperado:**
  * **Exit Code:** `0`
  * **Memoria:** `ERROR SUMMARY: 0 errors` y `definitely lost: 0 bytes`.

## [TEST 36] Valgrind: salida temprana por burnout

* **Comando:** `./codexion 2 300 500 100 100 5 0 fifo` (bajo `valgrind --leak-check=full -s`)
* **Propósito:** Incluso con terminación temprana por burnout, el proceso debe limpiar toda su memoria.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0` (burnout)
  * **Memoria:** `ERROR SUMMARY: 0 errors` y `definitely lost: 0 bytes`.

## [TEST 37] Valgrind: gran escala (50 coders EDF)

* **Comando:** `./codexion 50 2000 200 100 100 5 10 edf` (bajo `valgrind --leak-check=full -s`)
* **Propósito:** A gran escala (50 coders + heap de planificación) no debe haber fugas ni errores de memoria.
* **Comportamiento Esperado:**
  * **Exit Code:** `0`
  * **Memoria:** `ERROR SUMMARY: 0 errors` y `definitely lost: 0 bytes`.

## [TEST 38] Valgrind: ruta de error de parsing

* **Comando:** `./codexion 4 800 200 100 100` (bajo `valgrind --leak-check=full -s`)
* **Propósito:** La ruta de rechazo por parsing también debe liberar memoria de forma limpia.
* **Comportamiento Esperado:**
  * **Exit Code:** `!= 0` (error esperado)
  * **Memoria:** `ERROR SUMMARY: 0 errors` y `definitely lost: 0 bytes`.