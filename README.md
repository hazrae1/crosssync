<p align="center">
  <img src="assets/banner.svg" alt="CrossSync — CS2 a VALORANT" width="100%" />
</p>

<p align="center">
  <a href="https://github.com/hazrae1/crosssync/actions/workflows/build.yml"><img src="https://github.com/hazrae1/crosssync/actions/workflows/build.yml/badge.svg" alt="Build &amp; verify" /></a>
  <img src="https://img.shields.io/badge/C%2B%2B-17-48eab5?style=flat-square&amp;labelColor=121e27" alt="C++17" />
  <img src="https://img.shields.io/badge/platform-Windows%20%7C%20Linux-a3b0bd?style=flat-square&amp;labelColor=121e27" alt="Windows y Linux" />
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-ff5265?style=flat-square&amp;labelColor=121e27" alt="Licencia MIT" /></a>
</p>

<p align="center"><strong>Tu configuración, lista para el siguiente juego.</strong></p>
<p align="center">Importación de CFG, inventario de ajustes y exportación de perfiles en C++17.</p>

CrossSync organiza tu configuración de Counter-Strike 2 en un perfil de transición a Valorant. Lee sensibilidad, color de mira, parámetros de origen y binds; además conserva las opciones propias de CS en una sección dedicada del JSON.

## Vista de la consola

```text
   +------------------------------------------------------+
   |  C R O S S S Y N C                         v1.1.0    |
   |  CS2  ->  VALORANT          CONFIGURATION BRIDGE     |
   +------------------------------------------------------+

  SESSION   LOCAL / CONFIGURATION EXPORT
  SOURCE    examples/autoexec.cfg
  TARGET    output/valorant-profile.json

  [==..................]  10% Initialize migration workspace
  [======..............]  30% Parse source configuration
  [===========.........]  55% Collect input and crosshair parameters
  [===============.....]  75% Index source-specific settings and keybinds
  [==================..]  90% Serialize configuration profile
  [====================] 100% Configuration profile exported

  MIGRATION PREVIEW
  --------------------------------------------------------
  Source sensitivity   1.6000
  Crosshair color      #32FFA0
  Source keybinds      7 staged
  CS-specific options  4 preserved
  CFG commands         18 read / 1 ignored

  Configuration session complete.
```

## Qué incluye

| Módulo | Función |
| --- | --- |
| CFG reader | Lee sensibilidad, parámetros de mira y binds de un archivo indicado. |
| Input profile | Conserva la sensibilidad de origen con su valor exacto. |
| Crosshair staging | Genera el color hexadecimal y conserva las medidas de origen. |
| Keybind staging | Guarda teclas y comandos, incluidos binds de compra de CS. |
| Source compatibility | Reconoce `viewmodel_fov` y los tres offsets del arma. |
| Profile export | Escribe un JSON con el esquema `crosssync.profile/v1`. |
| Dry run | Muestra el flujo sin crear archivos. |

## Inicio rápido

Descarga el paquete de tu plataforma en [Releases](https://github.com/hazrae1/crosssync/releases/latest), extráelo y abre una terminal en esa carpeta.

**Windows / PowerShell**

```powershell
.\crosssync.exe
.\crosssync.exe --input examples/autoexec.cfg --output output/valorant-profile.json
.\crosssync.exe --input examples/cs-only.cfg
.\crosssync.exe --dry-run --fast
```

**Linux**

```bash
chmod +x crosssync
./crosssync --input examples/autoexec.cfg
./crosssync --dry-run --fast
```

La ejecución sin argumentos usa un perfil incorporado. El reporte predeterminado se escribe en `output/valorant-profile.json`; repetir la ejecución reemplaza ese reporte.

## Opciones propias de Counter-Strike

El repositorio incluye dos CFG con nombres de comandos de CS2 y valores dentro de sus rangos. `examples/autoexec.cfg` contiene el perfil completo; `examples/cs-only.cfg` reúne los ajustes del modelo del arma y binds de compra.

```cfg
viewmodel_fov "68"
viewmodel_offset_x "2.5"
viewmodel_offset_y "2"
viewmodel_offset_z "-2"
bind "F5" "buy ak47"
bind "F6" "buy awp"
```

Los parámetros `viewmodel_*` se guardan en `source_only_settings`; los comandos de compra permanecen en `source_bindings`. Consulta [la referencia de compatibilidad](docs/compatibility.md).

## Compilar desde el código

Necesitas CMake 3.16 o posterior y un compilador con C++17. En Windows puedes usar Visual Studio con las herramientas de C++.

```bash
git clone https://github.com/hazrae1/crosssync.git
cd crosssync
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

Ejecuta `build/Release/crosssync.exe` con Visual Studio, o `build/crosssync` con un generador de una sola configuración en Linux.

## Opciones

| Opción | Uso |
| --- | --- |
| `--input <file.cfg>` | Lee el archivo indicado; por defecto usa la muestra incorporada. |
| `--output <file.json>` | Elige la ruta del perfil exportado. |
| `--dry-run` | Ejecuta la vista previa sin escribir el reporte. |
| `--fast` | Quita las pausas de presentación. |
| `--no-color` | Desactiva los colores ANSI. También respeta `NO_COLOR`. |
| `--help` | Muestra ayuda. |
| `--version` | Muestra la versión. |

## Uso del perfil

El perfil sirve como referencia para completar los ajustes desde el menú de Valorant. La sensibilidad y las medidas se conservan en unidades de origen; los parámetros propios de CS se archivan para consulta. La aplicación al juego es manual.

Los archivos se procesan localmente. El ejecutable no solicita cuentas, no accede a carpetas de juegos y no hace peticiones de red. Solo lee la entrada que indiques y escribe el reporte solicitado. Los comandos `exec`, `alias` y demás comandos ajenos a la muestra se ignoran; ningún comando del CFG se ejecuta.

Para el detalle del flujo y del formato, consulta [la arquitectura](docs/architecture.md) y [el esquema del perfil](docs/profile-format.md).

## Desarrollo

GitHub Actions compila y prueba el parser y la CLI en Windows y Linux. Cada ejecución genera un paquete portable. Los CFG personales y los perfiles generados quedan fuera de Git mediante `.gitignore`; únicamente se incluyen los dos CFG de `examples/`.

Licencia [MIT](LICENSE). Proyecto independiente, sin afiliación con Valve ni Riot Games.
