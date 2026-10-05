# Formato del perfil

`crosssync.profile/v1` es el esquema de exportación de CrossSync para organizar una transición de configuración.

```json
{
  "schema": "crosssync.profile/v1",
  "generator": "CrossSync 1.1.0",
  "source": "Counter-Strike 2",
  "target": "VALORANT",
  "workflow": "manual_setup",
  "mouse": {
    "source_sensitivity": 1.6
  },
  "crosshair": {
    "color": "#32FFA0",
    "source_parameters": {
      "cl_crosshair_gap": 4,
      "cl_crosshair_length": 8,
      "cl_crosshair_thickness": 2
    }
  },
  "source_only_settings": {
    "viewmodel_fov": 68,
    "viewmodel_offset_x": 2.5,
    "viewmodel_offset_y": 2,
    "viewmodel_offset_z": -2
  },
  "source_bindings": {
    "MOUSE1": "+attack",
    "SPACE": "+jump"
  },
  "diagnostics": {
    "supported_commands": 13,
    "ignored_commands": 0
  }
}
```

La sensibilidad y los parámetros de mira conservan sus valores y unidades de origen. El color se serializa en hexadecimal RGB. `source_only_settings` agrupa las variables del modelo del arma de CS; los binds mantienen su texto original.

`manual_setup` identifica el flujo de aplicación: consultar el perfil y completar los ajustes en los menús del destino. Este JSON pertenece a CrossSync. Para compartir miras dentro de Valorant, el juego utiliza sus [códigos de importación y exportación](https://playvalorant.com/en-us/news/game-updates/valorant-patch-notes-4-05/).

La salida omite la ruta de entrada y cualquier contenido ajeno al perfil reconocido. Los CFG personales y los perfiles exportados se excluyen del repositorio por defecto.
