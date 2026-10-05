# Configuración de origen y compatibilidad

Los ejemplos usan nombres de variables presentes en el [inventario extraído de CS2](https://github.com/SteamDatabase/GameTracking-CS2/blob/master/DumpSource2/convars.txt). Los valores del ejemplo están dentro de los rangos publicados en ese inventario.

| Variable de CS2 | Rango | Exportación |
| --- | --- | --- |
| `viewmodel_fov` | 60 a 68 | `source_only_settings` |
| `viewmodel_offset_x` | -2 a 2.5 | `source_only_settings` |
| `viewmodel_offset_y` | -2 a 2 | `source_only_settings` |
| `viewmodel_offset_z` | -2 a 2 | `source_only_settings` |

Estos comandos de CS no se convierten en instrucciones ejecutables de Valorant. CrossSync conserva sus valores como parte del inventario del origen.

Los binds `buy ak47` y `buy awp` hacen referencia a compras y armas de Counter-Strike. Se conservan como texto en `source_bindings` para documentar la configuración.

La mira admite los parámetros actuales `cl_crosshair_length`, `cl_crosshair_gap` y `cl_crosshair_thickness`, además de nombres anteriores que puedan aparecer en CFG existentes. Cada nombre y valor se conserva sin equiparar sus unidades entre versiones o juegos.

Valorant permite introducir un color hexadecimal de seis dígitos en su menú de mira. El valor exportado puede consultarse allí sin el prefijo `#`, según las [notas oficiales de la versión 5.04](https://playvalorant.com/es-es/news/game-updates/valorant-patch-notes-5-04/).
