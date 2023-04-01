/*
 * File:   main.c
 * Author: Oscar Escriba
 *
 * Created on 30 de marzo de 2023, 09:41 PM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

#define _XTAL_FREQ 8000000 

void __interrupt() isr (void) { 
    if (PIR1bits.ADIF) { 
         int numero =0; 
     if (PIR1bits.ADIF) { 
         //interrupcion  
         numero = ADRESH; 
         CCPR1L=numero; 
         CCPR2L=numero; 
         
         PIR1bits.ADIF=0;  
     } 
    }
}
void setup(void) { 
    //configuracion de entradas y salidas  
    ANSEL = 0b00000011; 
    ANSELH =0; 
    TRISAbits.TRISA0=1; 
    TRISAbits.TRISA1=1; 
    
    //CONFIGURACION DEL OSCILADOR
    OSCCONbits.IRCF=0b0111; //configurado a 8Mhz 
    OSCCONbits.SCS=1;  
    
    //configuracion del adc 
    
    ADCON1bits.ADFM=0; //justificacion a la izquierda 
    ADCON1bits.VCFG1=0; 
    ADCON1bits.VCFG0=0;  
    ADCON0bits.ADCS=0b10; //FOSC/32 
    ADCON0bits.CHS=0; 
    ADCON0bits.CHS=1; 
    ADCON0bits.ADON=1; //prender el modulo de adc 
    __delay_ms(50); 
    
    //configuracion de las interrupciones 
    PIR1bits.ADIF=0; 
    PIE1bits.ADIE=1; 
    INTCONbits.PEIE=1; 
    INTCONbits.GIE=1;  
    
    // CONFIGURACION PWM, SIGUIENDO LOS PASOS DEL MANUAL 
    TRISCbits.TRISC2=0; //RC2/CCP1 COMO ENTRADA 
    TRISCbits.TRISC1=0;
    PR2 =249; //CONFIGURACION DEL PERIODO
    CCP1CONbits.P1M=0; //CONFIG MODO PWM 
    CCP1CONbits.CCP1M=0b1100;  //CONFIGURAR CONTROL DEL PWM  
    CCP2CONbits.CCP2M=0b1100;  
  
    CCPR1L =0x0f;  //ciclo de trabajo inicial. 
    CCP1CONbits.DC1B=0;  
    CCP2CONbits.DC2B0=0; 
    CCP2CONbits.DC2B1=1; 
    
    PIR1bits.TMR2IF=0;  //apagar la bandera 
    T2CONbits.T2CKPS=0b11; //prescaler 1:16 
    T2CONbits.TMR2ON=1; 
    //segun los pasos, esperar un ciclo del tmr2 
    while (PIR1bits.TMR2IF==0); 
    PIR1bits.TMR2IF=0;
    
    TRISCbits.TRISC2=0; //SALIDA DEL PWM 
    return; 
}

void main(void) { 
     setup(); 
  while (1) { 
      if (ADCON0bits.GO==0) {  
          if (ADCON0bits.CHS==0) { 
              ADCON0bits.CHS=1; 
          } else { 
              ADCON0bits.CHS=0; 
         __delay_ms(50);  
         ADCON0bits.GO=1;  
          }
      }
  }

} 
