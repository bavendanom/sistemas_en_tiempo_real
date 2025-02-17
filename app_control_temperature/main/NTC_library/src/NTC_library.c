#include "NTC_library.h"

// Funcion para calcular la resistencia de la NTC .
float res_ntc(float voltage_out, float res_fija, float vcc){
    return (voltage_out * res_fija) / (vcc - voltage_out);
}

// Función para calcular la temperatura en Celsius usando la fórmula de Beta
float tem_cel(float R_ntc){
    float ln_R_ratio = log(R_ntc / R_0); 
    float inv_T = (1.0 / T_0) + (1.0 / BETA) * ln_R_ratio; 
    float T = 1.0 / inv_T; // Temperatura en Kelvin 
        
    // Convertir la temperatura a Celsius 
    return (T - 273.15);
}