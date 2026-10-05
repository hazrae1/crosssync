# Formato del reporte

El formato `crosssync.demo/v1` pertenece a este simulador. No es un archivo de configuración de Valorant ni un perfil importable por el juego.

```json
{
  "schema": "crosssync.demo/v1",
  "generator": "CrossSync 1.0.0",
  "simulation": true,
  "applied_to_game": false,
  "source": "Counter-Strike CFG",
  "target": "VALORANT mock profile",
  "notice": "Joke project. Fictional calibration; not a VALORANT import format.",
  "mouse": {
    "source_sensitivity": 1.6,
    "preview_sensitivity": 0.5,
    "demo_divisor": 3.2
  },
  "crosshair_preview": {
    "color": "#32FFA0",
    "source_size": 2.5,
    "source_gap": -2.0,
    "source_thickness": 0.5
  },
  "source_bindings": {
    "MOUSE1": "+attack",
    "SPACE": "+jump"
  },
  "diagnostics": {
    "supported_commands": 9,
    "ignored_commands": 0
  }
}
```

`preview_sensitivity` divide la sensibilidad de origen entre `3.2`, una constante ficticia elegida para la presentación. Las medidas de mira y los binds son valores de origen, conservados como metadatos; no se traducen a opciones reales de Valorant.

La salida omite la ruta de entrada y cualquier contenido ajeno al perfil reconocido. Si decides usar un CFG personal, el reporte puede contener tus binds. Los reportes y CFG personales se excluyen del repositorio por defecto.
