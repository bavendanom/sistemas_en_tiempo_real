## Control de LED RGB con ESP32 y LEDC

Este proyecto demuestra cómo controlar un LED RGB utilizando el controlador LEDC (LED Controller) en un microcontrolador ESP32. El código permite ajustar el color del LED RGB variando el ciclo de trabajo (duty cycle) de los canales PWM asociados a los LEDs rojo, verde y azul.

## Características

- Control de un LED RGB mediante PWM.
- Configuración de la frecuencia y resolución del PWM.
- Transición suave de colores mediante ajuste del duty cycle.
- Uso del controlador LEDC del ESP32 para generar señales PWM.

## Requisitos

### Hardware
- ESP32 (cualquier placa compatible).
- LED RGB de ánodo común o cátodo común.
- Resistencias limitadoras de corriente (si es necesario).
- Conexiones GPIO según la configuración del código.

### Software
- ESP-IDF (Espressif IoT Development Framework).
- Herramientas de compilación y flasheo para ESP32.

## Configuración

### Conexiones
- **LED Rojo**: Conectado al GPIO 21.
- **LED Verde**: Conectado al GPIO 19.
- **LED Azul**: Conectado al GPIO 18.
- **GND**: Conectado al cátodo del LED RGB (si es de cátodo común) o al ánodo, **VCC** (si es de ánodo común).

### Configuración del Código
- El código está configurado para usar el temporizador `LEDC_TIMER_0` y los canales `LEDC_CHANNEL_0`, `LEDC_CHANNEL_1` y `LEDC_CHANNEL_2` para los LEDs rojo, verde y azul, respectivamente.
- La frecuencia PWM está configurada a 4 kHz con una resolución de 13 bits.

## Uso

1. Clona este repositorio en tu entorno de desarrollo.
   ```bash
   git clone https://github.com/bavendanom/sistemas_en_tiempo_real
   cd led_RGB
## Estructura el proyecto


```bash
├── main/
│   ├── CMakeLists.txt                     # Definición cómo se deben compilar y enlazar los archivos de código fuente
│   ├── ledc_basic_example_main.c          # Código principal
│   ├──led_RGB_LIBRARY
│   │   ├──include
│   │   │   ├── led_RGB_library.h          # Definición de la librería LED RGB
│   │   ├──src
│   │   │   ├── led_RGB_library.c          # Implementación de la librería LED RGB
│   ├── CMakeLists.txt                     # Configuración de CMake
├── build/                                 # Carpeta generada tras la compilación
├── sdkconfig                              # Archivo de configuración de ESP-IDF
└── README.md                              # Documentación del proyecto

```
## Conexion a la placa

![ESP32](https://github.com/bavendanom/sistemas_en_tiempo_real/blob/main/led_RGB/conexion_LED_RGB?raw=true)


## Authors

**Brayan Avendaño Mesa**
- [@bavendanom](https://www.github.com/bavendanom)


