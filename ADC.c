// Frecuencia del microcontrolador para utilizar en las funciones de retardo
#define _XTAL_FREQ 4000000
#define __delay_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))

// Incluir las librerías
#include <xc.h>
#include "adc.h"

// Configuración del ADC
void ADC_CONF(int frec){
    
    ADCON0bits.ADON = 1; // Habilitar ADC

    ADCON0bits.CHS = 0; // Selecciona el canal para conversión

    ADCON1bits.ADFM = 0; // Justificación a la izquierda
    ADCON1bits.VCFG0 = 0; // Vref- se conecta a VSS
    ADCON1bits.VCFG1 = 0; // Vref+ se conecta a VDD

    ADCON0bits.GO = 0; // Limpia el bit de inicio

    // Configura el tiempo de adquisición del ADC según la frecuencia
    switch(frec){
        case 1:
            ADCON0bits.ADCS = 0B00; // Fosc/2 
            break;
        case 4:
            ADCON0bits.ADCS = 0B01; // Fosc/8
            break;
        case 8:
            ADCON0bits.ADCS = 0B10; // Fosc/32 
            break;
    }
}

// Función para realizar una conversión ADC en modo de barrido continuo
void ADC_IF(){
    if (ADCON0bits.GO == 0){ // Si el ADC no está en proceso de conversión
            if(ADCON0bits.CHS == 0){ // Si el canal seleccionado es 0
                ADCON0bits.CHS = 1; // Cambiar al canal 1
            }
            else{
                ADCON0bits.CHS = 0; // Si el canal seleccionado es 1, cambiar al canal 0
            }
            __delay_us(50); // Pequeño retardo para estabilizar la señal
            ADCON0bits.GO = 1; // Iniciar una nueva conversión
        }
}

