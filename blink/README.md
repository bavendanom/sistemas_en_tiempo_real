## Proyecto: Blink con ESP32

Este es un ejemplo simple de cómo hacer parpadear un LED conectado a un pin GPIO en un ESP32 utilizando el framework ESP-IDF.

## Descripción
El proyecto demuestra cómo configurar un pin GPIO como salida y hacer parpadear un LED conectado a dicho pin. El estado del LED (encendido o apagado) se alterna en un intervalo de tiempo definido.

## Requisitos
- **ESP32**: Placa de desarrollo compatible con ESP-IDF.
- **LED**: Un LED conectado al pin GPIO configurado (por defecto, GPIO 5).
- **Resistencia**: Una resistencia adecuada para limitar la corriente del LED.
- **ESP-IDF**: Framework de desarrollo para ESP32.

## Instalación
1. Clona este repositorio o descarga el código.
   ```bash
   git clone https://github.com/bavendanom/sistemas_en_tiempo_real
   
![ESP32](".\blink\ESP32.png")


## Salida

```text
I (315) example: Example configured to blink addressable LED!
I (325) example: Turning the LED OFF!
I (1325) example: Turning the LED ON!
I (2325) example: Turning the LED OFF!
I (3325) example: Turning the LED ON!
I (4325) example: Turning the LED OFF!
I (5325) example: Turning the LED ON!
I (6325) example: Turning the LED OFF!
I (7325) example: Turning the LED ON!
I (8325) example: Turning the LED OFF!
```

## Authors

**Brayan Avendaño Mesa**
- [@bavendanom](https://www.github.com/bavendanom)


