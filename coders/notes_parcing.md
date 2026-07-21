# Codexion — Notas de diseño y decisiones tomadas

## Estado actual del parsing

Los argumentos se reciben como strings, se **validan** por separado y luego se **convierten** y almacenan en `t_config`. La cadena completa es:

```
main(&cfg)
 └─ parse_args(argc, argv, &cfg)
     ├─ check_numeric_args()          ← valida strings (argv[1..7])
     ├─ ft_is_valid_scheduler()       ← valida "fifo" / "edf"
     └─ fill_config(argv, &cfg)       ← convierte y rellena t_config
```

---

## 1. Sistema de mensajes de error: códigos en vez de printf directo

**Problema del enfoque inicial:** las funciones de validación imprimían su propio error con `ft_putendl`, mezclando lógica de validación con presentación.

**Decisión tomada:** las funciones de validación devuelven un `t_error` (enum definido en `codexion.h`), y un único punto (`print_error()`) centraliza los mensajes en una tabla indexada por código.

**Por qué:** permite tener mensajes distintos por tipo de fallo (numérico, overflow, cero, scheduler...) sin acoplar cada función a un texto fijo. Si en la defensa piden cambiar un mensaje, se toca una sola línea, no varias funciones. Además, facilita testear la lógica sin capturar stdout.

**Archivos implicados:**
- `includes/codexion.h:10-19` — definición de `t_error`
- `srcs/parcing/parcing.c:3-14` — `print_error()` con tabla de mensajes

---

## 2. t_config: campos de tiempo en long long

**Decisión:** todos los campos de tiempo (`time_to_burnout`, `time_to_compile`, `time_to_debug`, `time_to_refactor`, `dongle_cooldown`) son `long long`.

**Por qué:** evita tener que ampliar tipos más adelante cuando se hagan cálculos de timestamps (`gettimeofday` + sumas repetidas a lo largo de la simulación). Un `long` podría desbordar con operaciones acumuladas en milisegundos; `long long` da margen de sobra.

**Archivos implicados:**
- `includes/codexion.h:31-41` — definición de `t_config`

---

## 3. ft_atoi: reescritura como función segura (string-to-int)

**Problema original:** `ft_atoi` acumulaba en `int res`, lo cual produce comportamiento indefinido (UB) cuando el string representa un valor mayor a `INT_MAX`. Por ejemplo, `"9999999999"` desborda `int` *durante* la multiplicación `res * 10`, antes de poder comprobar nada.

**Decisión:** reescribir `ft_atoi` con la siguiente firma:

```c
int  ft_atoi(const char *str, int *out);  // devuelve 0 = OK, -1 = fallo
```

**Algoritmo implementado:**

1. Rechaza string vacío (`NULL` o `""`).
2. Salta whitespace (espacios y tabulaciones, como el `ft_atoi` original de libft).
3. Rechaza `-`; acepta opcionalmente `+`.
4. Exige al menos un dígito después del signo (sin esto, `"+"` parseaba como 0).
5. Acumula en `long long res` para evitar UB durante el bucle.
6. Comprueba `res > INT_MAX` en cada paso — si se supera, rechaza.
7. Comprueba que no queden caracteres después de los dígitos.
8. Escribe el resultado en `*out` (cast seguro `long long → int` porque ya sabemos que cabe).

**Por qué se acumula en `long long`:** la comprobación de overflow se hace *antes* de escribir en `*out`. Si acumuláramos en `int`, la propia operación `res * 10 + digit` sería UB para valores grandes, haciendo que la comprobación llegara demasiado tarde.

**Por qué se rechaza el `-`:** el enunciado pide un parser de números positivos. Los negativos no son válidos de todas formas, y rechazarlos antes simplifica la lógica.

**Problema encontrado durante testing:** la versión inicial no comprobaba si había al menos un dígito después de `+`. El string `"+"` pasaba como válido con valor 0, porque el bucle de dígitos no se ejecutaba pero tampoco fallaba. Se corrigió añadiendo una comprobación explícita `if (*str < '0' || *str > '9') return (-1)` justo después de consumir el `+`.

**Archivos implicados:**
- `srcs/auxiliary_libft/ft_atoi.c` — reescritura completa
- `includes/codexion.h:78` — declaración actualizada

---

## 4. El enfoque rechazado: cast de punteros `(int *)&cfg->time_to_burnout`

**Problema detectado:** el plan original proponía escribir directamente en los campos `long long` de `t_config` usando `ft_atoi` (que solo escribe `int`, 4 bytes) mediante un cast de puntero:

```c
ft_atoi(argv[2], (int *)&cfg->time_to_burnout)
```

**Por qué se rechazó:** `ft_atoi` solo rellena los 4 bytes bajos de un `long long` (que ocupa 8 bytes). Si `t_config` no estaba inicializada a cero, los 4 bytes altos quedaban con basura de la pila, dando lugar a un valor final incorrecto — un bug real, no cosmético, y difícil de detectar en pruebas simples.

---

## 5. ft_atoll: wrapper que reutiliza ft_atoi

**Decisión:** crear `ft_atoll` que internamente llama a `ft_atoi` y amplía el resultado:

```c
int  ft_atoll(const char *str, long long *out)
{
    int  tmp;

    if (ft_atoi(str, &tmp) == -1)
        return (-1);
    *out = (long long)tmp;
    return (0);
}
```

**Por qué esta opción y no otras:**

| Opción descartada | Razón |
|---|---|
| Reescribir `ft_atoi` para devolver `long long` | Afectaría a más código del necesario; la mayoría de campos en `t_config` son `int` o no necesitan `long long` en el parseo |
| Guardar todo como `int` en `t_config` | Volvería a mezclar tipos entre el parseo y el resto del programa |
| Duplicar la lógica de parseo en `ft_atoll` | Si se corrige un bug en el parseo de dígitos, habría que acordarse de corregirlo en dos sitios |

**Límite de overflow:** `ft_atoll` mantiene el límite en `INT_MAX`, no en `LLONG_MAX`. La validación previa (`ft_check_overflow`) ya rechaza cualquier valor mayor — mantener el mismo límite en validación y conversión evita inconsistencias. Si en el futuro se necesitan valores mayores a `INT_MAX`, habría que cambiar tanto la validación como la conversión juntas.

**Archivos implicados:**
- `srcs/auxiliary_libft/ft_atoll.c` — nuevo
- `includes/codexion.h:79` — declaración

---

## 6. fill_config: función que conecta validación con almacenamiento

**Qué hace:** recorre `argv[1]` a `argv[8]` y rellena `t_config` usando `ft_atoi` (para los campos `int`: `number_of_coders`, `number_of_compiles_required`) y `ft_atoll` (para los campos `long long`), y resuelve el scheduler a partir del string ya validado.

**Por qué es `static`:** solo se llama desde `parse_args` en el mismo archivo. No tiene por qué ser visible en otros archivos.

**Scheduler con `else if` explícito:** en vez de un `else` al final, se usa `else if (ft_strncmp(argv[8], "edf", 3) == 0)` para no depender silenciosamente de que la validación previa ya descartó cualquier otro valor. Es defensivo: si alguien cambia la lógica de validación en el futuro, el `else if` no asignará un valor basura por accidente.

```c
if (ft_strncmp(argv[8], "fifo", 4) == 0)
    cfg->scheduler = SCHEDULER_FIFO;
else if (ft_strncmp(argv[8], "edf", 3) == 0)
    cfg->scheduler = SCHEDULER_EDF;
```

**Archivos implicados:**
- `srcs/parcing/parcing.c:35-56` — `fill_config`

---

## 7. Cambio de firma de parse_args

**Antes:** `int parse_args(int argc, char **argv)` — devolvía `0/1`, sin forma de devolver los valores parseados.

**Después:** `int parse_args(int argc, char **argv, t_config *cfg)` — el caller pasa un puntero a `t_config` donde `fill_config` escribe los valores.

**Por qué:** `parse_args` ya no solo valida, sino que también rellena la configuración. Separar la firma del struct significaría tener que devolver el struct por otra vía (global, return...), y pasar el puntero es el patrón más limpio en C.

**Archivos implicados:**
- `includes/codexion.h:95` — declaración actualizada
- `srcs/parcing/parcing.c:58` — implementación actualizada
- `srcs/codexion.c` — `main` ahora declara `t_config cfg` y la pasa

---

## 8. Funciones sin uso en el proyecto

Análisis de uso de las funciones en `srcs/auxiliary_libft/`:

| Función | Estado |
|---|---|
| `ft_atoi` | Ahora usada por `fill_config` |
| `ft_atoll` | Ahora usada por `fill_config` |
| `ft_bzero` | Solo usada internamente por `ft_calloc` (que a su vez no se usa) |
| `ft_calloc` | Sin uso en el proyecto |
| `ft_memset` | Sin uso en el proyecto |
| `ft_putchar` | Solo usada internamente por `ft_putendl` |
| `ft_putendl` | Usada por `print_error` y `main` |
| `ft_putstr` | Solo usada internamente por `ft_putendl` |
| `ft_strlen` | Usada por `parching_utils.c` |
| `ft_strncmp` | Usada por `parching_utils.c` |
| `ft_validnumber` | Usada por `parching_utils.c` |

`ft_bzero`, `ft_calloc` y `ft_memset` son código muerto actualmente. Podrían eliminarse si no se prevé usarlas en el futuro de la simulación.

---

## Resumen de archivos modificados

| Archivo | Cambios |
|---|---|
| `includes/codexion.h` | `#include <limits.h>`, `ft_atoi` nueva firma, `ft_atoll` nueva declaración, `parse_args` nueva firma |
| `srcs/auxiliary_libft/ft_atoi.c` | Reescritura completa: `long long` accumulation, overflow check, firma con `int *out` |
| `srcs/auxiliary_libft/ft_atoll.c` | **Nuevo** — wrapper de `ft_atoi` que amplía a `long long` |
| `srcs/parcing/parcing.c` | `fill_config` nueva (estática), `parse_args` ahora recibe `t_config *cfg` y llama a `fill_config` |
| `srcs/codexion.c` | `main` declara `t_config cfg` y la pasa a `parse_args` |
| `Makefile` | `ft_atoll.c` añadido a SRC |


¿Volvere a leerme el notes?
