// Incluir las librerías
#include <xc.h>
#include "usart.h"

void USART_CONF(int frec, int tr, int rc) {
    // Configuración de los registros para la comunicación USART
    RCSTAbits.SPEN = 1; // Habilita USART

    TXSTAbits.SYNC = 0; // Modo asíncrono 
    TXSTAbits.TX9 = 0; // Transmisión de 8 bits
    RCSTAbits.RX9 = 0; // Recepción de 8 bits

    switch (frec) {
        case 4:
            SPBRG = 25; // Baud rate de 9600
            TXSTAbits.BRGH = 1; // Alta velocidad en el modo asincrónico
            BAUDCTLbits.BRG16 = 0; // Baud rate de 8 bits
            break;
        case 8:
            SPBRG = 12; // Baud Rade de 9600
            TXSTAbits.BRGH = 0; // Baja velocidad en el modo asincrónico
            BAUDCTLbits.BRG16 = 0; // Baud rate de 8 bits
            break;
    }

    // Configuración de la transmisión y recepción según los parámetros tr y rc
    if (tr == 1) {
        TXEN = 1; // Habilita la transmisión USART
    } else {
        TXEN = 0; // Deshabilita la transmisión USART
    }

    if (rc == 1) {
        RCSTAbits.CREN = 1; // Habilita la recepción USART
    } else {
        RCSTAbits.CREN = 0; // Deshabilita la recepción USART
    }
}

