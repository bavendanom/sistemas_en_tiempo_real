# Ejemplo de GPIO con ESP32

Este es un ejemplo de cómo configurar y utilizar interrupciones de GPIO en un ESP32. El código muestra cómo configurar pines GPIO como entradas y salidas, y cómo manejar interrupciones en los flancos de subida (rising edge) y bajada (falling edge).

## Descripción
El proyecto configura dos pines GPIO como salidas y dos pines como entradas con interrupciones. Cuando se detecta un flanco de subida en las entradas, se activa una interrupción que cambia el estado de un LED conectado a una de las salidas.

### Configuración de pines
- **Salidas**:
  - `GPIO_OUTPUT_IO_0`: Pin de salida (configurable, por defecto GPIO 2).
  - `GPIO_OUTPUT_IO_1`: Pin de salida (configurable, por defecto GPIO 18).
- **Entradas**:
  - `GPIO_INPUT_IO_0`: Pin de entrada con interrupción en flanco de subida y bajada (configurable, por defecto GPIO 0).
  - `GPIO_INPUT_IO_1`: Pin de entrada con interrupción en flanco de subida (configurable, por defecto GPIO 4).

## Nota 
Se utilza el led y el boton incorporados en la placa de desarrollo

## Requisitos
- **ESP32**: Placa de desarrollo compatible con ESP-IDF.
- **Cables y resistencias**: Para conectar los pines de entrada y salida.
- **LED**: Opcional, para visualizar el estado de la salida.
- **ESP-IDF**: Framework de desarrollo para ESP32.

## Instalación
1. Clona este repositorio o descarga el código.
   ```bash
   git clone https://github.com/bavendanom/sistemas_en_tiempo_real
![ESP32](https://github.com/bavendanom/sistemas_en_tiempo_real/blob/main/toogle/ESP32.png?raw=true)


## Authors

**Brayan Avendaño Mesa**
- [@bavendanom](https://www.github.com/bavendanom)


