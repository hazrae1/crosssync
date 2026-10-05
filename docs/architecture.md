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
    inventario de ajustes
              |
              v
       *.json / consola
```

## Responsabilidades

| Archivo | Responsabilidad |
| --- | --- |
| `include/crosssync/profile.hpp` | Estructura del perfil y contrato del núcleo. |
| `src/profile.cpp` | Tokenización, validación, serialización JSON y muestra incorporada. |
| `src/main.cpp` | Argumentos, presentación, lectura del archivo indicado y escritura del reporte. |
| `tests/profile_tests.cpp` | Casos de parser, valores inválidos, comentarios, comillas y JSON. |

El parser acepta valores entre comillas, comentarios `//` fuera de comillas y comandos separados por `;`. Las entradas repetidas usan el último valor. Interpreta sensibilidad, tres medidas de mira y tres canales RGB, además de `bind` y cuatro variables `viewmodel_*`. Conserva el nombre original de cada parámetro de mira, tanto con la sintaxis actual como con la anterior.

La entrada está limitada a 1 MiB y requiere sensibilidad positiva. Los canales RGB deben ser enteros entre 0 y 255. Los parámetros actuales de mira y modelo del arma se validan con los rangos de la [referencia de comandos](compatibility.md). Los comandos desconocidos cuentan como ignorados.

## Presentación

Las seis etapas muestran el avance del flujo de importación y exportación. `--fast` omite las pausas de presentación; conserva el mismo perfil. `--dry-run` realiza la lectura y la vista previa sin crear una salida. Los colores se habilitan únicamente en una terminal compatible.

El programa escribe perfiles cuyo nombre termina en `.json` y rechaza que la entrada y salida sean el mismo archivo. El flujo de configuración del destino se realiza desde sus menús, usando el perfil exportado como referencia.
