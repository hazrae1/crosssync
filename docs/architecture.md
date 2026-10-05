# Arquitectura

CrossSync mantiene el parser separado de la presentación de consola. El núcleo usa únicamente la biblioteca estándar de C++17; la única llamada específica de Windows habilita el color de la terminal.

```text
CFG de ejemplo o archivo indicado
              |
              v
      tokenizer / validación
              |
              v
        Profile de origen
              |
              v
     vista previa de demo
              |
              v
      *.demo.json / consola
```

## Responsabilidades

| Archivo | Responsabilidad |
| --- | --- |
| `include/crosssync/profile.hpp` | Estructura del perfil y contrato del núcleo. |
| `src/profile.cpp` | Tokenización, validación, serialización JSON y muestra incorporada. |
| `src/main.cpp` | Argumentos, presentación, lectura del archivo indicado y escritura del reporte. |
| `tests/profile_tests.cpp` | Casos de parser, valores inválidos, comentarios, comillas y JSON. |

El parser acepta valores entre comillas, comentarios `//` fuera de comillas y comandos separados por `;`. Las entradas repetidas usan el último valor. Solo interpreta sensibilidad, tres medidas de mira y tres canales RGB, además de `bind`. Los comandos desconocidos cuentan como ignorados y se conservan únicamente los binds como metadatos de texto.

La entrada está limitada a 1 MiB y requiere sensibilidad positiva. Los canales RGB deben ser enteros entre 0 y 255. Las medidas restantes se validan dentro de los rangos definidos para esta demo.

## Presentación

Las seis etapas de consola ilustran una migración ficticia. `--fast` omite sus pausas; no altera el contenido del reporte. `--dry-run` realiza la lectura y la vista previa sin crear una salida. Los colores se habilitan únicamente en una terminal compatible.

El programa escribe solo reportes cuyo nombre termina en `.demo.json`. No tiene integración con procesos, cuentas, instaladores ni archivos internos de juegos.
