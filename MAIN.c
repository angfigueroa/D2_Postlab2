/*
 * File:   MAIN.c
 * Author: Angela
 *
  * Created on 20 de julio de 2023, 05:00 PM
 */


// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3/PGM pin has PGM function, low voltage programming enabled)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
#define _XTAL_FREQ 4000000
#define __delay_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))

// Definición de pines para la interfaz con la pantalla LCD
#define RS RC4
#define EN RC5
#define D0 RD0
#define D1 RD1
#define D2 RD2
#define D3 RD3
#define D4 RD4
#define D5 RB5
#define D6 RD6
#define D7 RD7

#include <xc.h>
#include <stdint.h>
#include "LCD.h" // Archivo para controlar la pantalla LCD
#include "adc.h" // Archivo para controlar el módulo ADC
#include "usart.h" // Archivo para controlar la comunicación USART
#include <stdio.h>
#include <stdlib.h>

// Variables globales
char pot, frec, tr, rc, con;
int vol1, vol2, x;
char buffer[5];
char vol1_str[5];
char cen, dec, uni;

// Función para obtener las centenas del valor del potenciómetro
char GetCentenas(int value) {
    return (value / 100);
}

// Función para transmitir datos por USART
void UART_write(unsigned char* word){
    while (*word != 0){
        TXREG = (*word);
        while(!TXSTAbits.TRMT);
        word++;
    }
    return;
}

// Función para dividir el valor del contador de USART número en centenas, decenas y unidades
void Division(char y){
    cen = (y / 100);
    dec = ((y % 100) / 10);
    uni = ((y % 100) % 10);
}

// Rutina de interrupción
void __interrupt() isr(void){
    // Rutina de interrupción del ADC
    if (ADIF){
        if (ADCON0bits.CHS == 0){
            pot = ADRESH; // Lectura del valor analógico del canal del potenciómetro
        }
        
        ADIF = 0; // Limpiar la bandera de interrupción del ADC
    }
    
    // Rutina de interrupción del USART 
    if (RCIF){
        if (RCREG == '+'){
            con++; // Incrementar el valor del contador al recibir el carácter '+'
        }
        else if (RCREG == '-'){
            con--; // Decrementar el valor del contador al recibir el carácter '-'
        }
        else if (RCREG == '*'){
            x = 1; // Activar una bandera para mostrar el valor del pot por USART
        }
        RCIF = 0; // Limpiar la bandera de interrupción del USART
    }
    
    return;
}

void main(void) {
    // Configuración de pines y periféricos
    ANSEL = 0x03; // AN0 y AN1 como entradas analógicas
    ANSELH = 0x00; // AN8-AN13 como entradas digitales
    
    TRISA = 0x03; // RA0 y RA1 como entradas
    TRISB = 0x00; // Puerto B como salidas
    TRISC = 0x80; // RC7 (RX) como entrada y el resto como salidas
    TRISD = 0x00; // Puerto D como salidas
    
    OSCCONbits.IRCF = 0B110; // Oscilador interno a 4MHz
    OSCCONbits.OSTS = 0;
    OSCCONbits.SCS = 1;
    
    ADC_CONF(4); // ADC con Vref interna y canal AN4 (RA0)
    USART_CONF(4, 1, 1); // Comunicación USART a 9600 baudios
    
    INTCONbits.PEIE = 1; // Habilitar interrupciones
    PIE1bits.ADIE = 1; // Interrupción del ADC
    PIE1bits.RCIE = 1; // Interrupción de recepción del USART
    ADCON0bits.GO = 1; 
    Lcd_Init(); // Iniciar pantalla LCD
    RCIF = 0; // Limpiar la bandera de interrupción del USART
    
    // Mensaje en la pantalla LCD y por USART
    Lcd_Clear();
    Lcd_Set(1,1);
    Lcd_Write_Str("POT:    CONT:");
    UART_write("Presione * para mostrar el pot ");
    UART_write("O presione + - para el contador");
    INTCONbits.GIE = 1; // Habilitar todas las interrupciones
    
    while(1){
        // Bucle principal
        
        ADC_IF(); // Rutina de interrupción del ADC
        PORTB = con; // Mostrar el valor del contador en el puerto B
        
        vol1 = (pot * 1.961); // Cálculo del valor del potenciómetro 1 en voltaje
        
        cen = GetCentenas(vol1); // Obtener el valor de las centenas del potenciómetro
        sprintf(vol1_str, "%d.%d", cen, (vol1 % 100)); // Convierte el valor de las centenas a cadena y agrega un punto después del primer carácter
        
        Division(con); // Dividir el valor del contador en centenas, decenas y unidades
        
        // Mostrar los valores en el LCD
        Lcd_Set(2, 1); // Valor del potenciómetro
        Lcd_Write_Str(vol1_str);
        Lcd_Set(2, 10); // Valor del contador por USART
        sprintf(buffer, "%d", cen);
        Lcd_Write_Str(buffer);
        sprintf(buffer, "%d", dec);
        Lcd_Write_Str(buffer);
        sprintf(buffer, "%d", uni);
        Lcd_Write_Str(buffer);
        
        if (x == 1){
            x = 0;
            UART_write("\r \0"); // Retornar el cursor al inicio de la línea
            sprintf(buffer, "%.2dV  ", vol1); // Convertir el valor del potenciómetro a cadena
            UART_write("POT 1: ");
            UART_write(buffer); // Enviar el valor del pot por USART
            UART_write("\r \0"); // Retornar el cursor al inicio de la línea
        }
    }
}
