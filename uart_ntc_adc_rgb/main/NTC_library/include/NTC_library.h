#include <math.h>

// Constantes específicas del NTC
#define R_0 47.0 // Resistencia a 25°C (en ohmios)
#define T_0 298.15 // Temperatura de referencia en Kelvin (25°C)
#define BETA 3200.0 // Coeficiente Beta del NTC

// Funciones para cálculo
float res_ntc(float voltage_out, float R_fija, float V_cc);
float tem_cel(float R_NTC);