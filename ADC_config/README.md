# Control de LED RGB con ADC y GPIO en ESP32

Este proyecto demuestra cómo controlar un LED RGB utilizando un microcontrolador ESP32. La intensidad del color del LED se cambia mediante la lectura de valores de un ADC (Convertidor Analógico-Digital) y la interacción con botones conectados a GPIOs permite el cambio de color. El proyecto utiliza el controlador LEDC (LED Control) para generar señales PWM y controlar el brillo de cada color del LED RGB.

## Características principales

- Control de un LED RGB mediante PWM.
- Lectura de valores analógicos utilizando el ADC del ESP32.
- Cambio de color del LED mediante interrupciones de GPIO.
- Calibración del ADC para obtener lecturas precisas de voltaje.

## Hardware requerido

- Placa de desarrollo ESP32.
- LED RGB de cátodo común o ánodo común.
- Resistencias adecuadas para los LEDs (generalmente 220Ω).
- Potenciómetros o sensores analógicos para el ADC.
- Botones para la interacción con GPIOs.

## Configuración del hardware

1. **LED RGB**:
   - Conecta el pin del LED rojo al GPIO 21.
   - Conecta el pin del LED verde al GPIO 19.
   - Conecta el pin del LED azul al GPIO 18.
   - Conecta el cátodo común (o ánodo común) del LED RGB a GND (o VCC, dependiendo del tipo de LED).

2. **ADC**:
   - Conecta un potenciómetro o sensor analógico al canal 4 (GPIO 32) del ADC1.
   - Conecta otro potenciómetro o sensor analógico al canal 5 (GPIO 33) del ADC1.

3. **Botones**:
   - Conecta un botón entre el GPIO 0 y GND. (Se usa el integrado en la placa de desarrollo)
   - Conecta otro botón entre el GPIO 5 y GND.

## Configuración del software

### Dependencias

- ESP-IDF (Espressif IoT Development Framework).
- Librerías de ADC y LEDC de ESP-IDF.

### Compilación y carga

1. Clona este repositorio en tu entorno de desarrollo.
```bash
   git clone https://github.com/bavendanom/sistemas_en_tiempo_real
   cd ADC_config
   
```
2. Abre el proyecto en un entorno de desarrollo compatible con ESP-IDF (como VS Code con la extensión ESP-IDF).
3. Configura el proyecto para tu placa ESP32.
4. Compila y carga el firmware en la placa ESP32.



## Estructura el proyecto


```bash
├── main/
│   ├── CMakeLists.txt                     # Definición cómo se deben compilar y enlazar los archivos de código fuente
│   ├── oneshot_read_main.c                # Código principal
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

![ESP32](https://github.com/bavendanom/sistemas_en_tiempo_real/blob/main/ADC_config/conexion_pot_rgb.png?raw=true)

## Ejemplo de salida

Ejecutando este ejemplo, verá la siguiente salida de registro en el monitor serie:

```
I (304) ADC_ONESHOT: calibration scheme version is Curve Fitting
I (304) ADC_ONESHOT: calibration scheme version is Curve Fitting
I (314) ADC_ONESHOT: ADC1 Channel[2] Raw Data: 0
I (314) ADC_ONESHOT: ADC1 Channel[2] Cali Voltage: 0 mV
I (1324) ADC_ONESHOT: ADC1 Channel[3] Raw Data: 664
I (1324) ADC_ONESHOT: ADC1 Channel[3] Cali Voltage: 559 mV
I (2324) ADC_ONESHOT: ADC2 Channel[0] Raw Data: 580
I (2324) ADC_ONESHOT: ADC2 Channel[0] Cali Voltage: 498 mV
I (3324) ADC_ONESHOT: ADC1 Channel[2] Raw Data: 0
I (3324) ADC_ONESHOT: ADC1 Channel[2] Cali Voltage: 0 mV
I (4324) ADC_ONESHOT: ADC1 Channel[3] Raw Data: 666
I (4324) ADC_ONESHOT: ADC1 Channel[3] Cali Voltage: 561 mV
I (5324) ADC_ONESHOT: ADC2 Channel[0] Raw Data: 575
I (5324) ADC_ONESHOT: ADC2 Channel[0] Cali Voltage: 495 mV
...
```

## Authors

**Brayan Avendaño Mesa**
- [@bavendanom](https://www.github.com/bavendanom)


