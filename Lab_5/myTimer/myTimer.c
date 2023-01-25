#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/rom.h"
#include "grlib/grlib.h"
#include "drivers/cfal96x64x16.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
bool led_toggle = false;
bool s_toggle = false;
bool a_toggle = false;
bool adc_display_toggle = false;
bool timer1_toggle = false;
int SRV = 0;
int req = 0;
uint32_t ADC0Value[3];
char buf[10];
int gNumCharRecv = 0;
int loopCount = 0;
int count = 0;
tRectangle sRect;
tContext g_sContext;
uint32_t g_ui32Flags;
tContext g_sContext;

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

void clearScreen() {
    sRect.i16XMin = 0;
    sRect.i16YMin = 0;
    sRect.i16XMax = GrContextDpyWidthGet(&g_sContext) - 1;
    sRect.i16YMax = GrContextDpyHeightGet(&g_sContext);
    GrContextForegroundSet(&g_sContext, ClrBlack);
    GrRectFill(&g_sContext, &sRect);
}

void Timer0IntHandler(void) {
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //Trigger adc conversion
    ADCProcessorTrigger(ADC0_BASE, 3);    
    //Read ADC Value
    ADCSequenceDataGet(ADC0_BASE, 3, ADC0Value);
    //req = ADC0Value[0];
    if (a_toggle == true) {
        req = (ADC0Value[0] * 500);
    }
    else if (a_toggle == false) {
        req = (ADC0Value[0] / 10);
    }
    TimerLoadSet(TIMER1_BASE, TIMER_A, (SysCtlClockGet()/req));
    
    sprintf(buf, "REC: %d   ", req);
    GrContextForegroundSet(&g_sContext, ClrWhite);
    GrStringDraw(&g_sContext, buf, -1,
                 1, 10, true);
    sprintf(buf, "SRV: %d   ", SRV);
    GrContextForegroundSet(&g_sContext, ClrWhite);
    GrStringDraw(&g_sContext, buf, -1,
                 1, 20, true); 

    /*Everything between IntMasterDisable() and IntMasterEnable()
      was dragged over from my Variable Frequency ISR*/
    IntMasterDisable();
    int countValue = (SysCtlClockGet() / req);
    if(adc_display_toggle == true) {
        GrStringDraw(&g_sContext, "ADC:", -1,
                     1, 50, true);
        sprintf(buf, "%d  ", ADC0Value[0]);
        GrContextForegroundSet(&g_sContext, ClrWhite);
        GrStringDraw(&g_sContext, buf, -1,
                     45, 50, true);
        
        GrStringDraw(&g_sContext, "COUNT:", -1,
                     1, 40, true);
        sprintf(buf, "%d  ", countValue);
        GrContextForegroundSet(&g_sContext, ClrWhite);
        GrStringDraw(&g_sContext, buf, -1,
                     45, 40, true);
    }
    IntMasterEnable();
    SRV = 0;
}
/*
Timer1 runs at a variable frequency set by the ADC
The Timer0 runs at 1hz, and every second,
it gets the ADC value, and uses it to change
the countdown time for timer 1. A lower countdown
time means a higher frequency. Every time timer 0 or timer 1
reaches zero, it generates an interrupt -> which means that
the interrupt handler gets called. When the interrupt handler
gets called, it clears the flag-> runs the code-> and exits
*/

void Timer1IntHandler(void) {
    //Remove all functionality except clearing flags and
    //Incrememting the SRV counter
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);   
    ++SRV;
}

void UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count) {
    //
    // Loop while there are more characters to send.
    //
    while(ui32Count--)
    {
        //
        // Write the next character to the UART.
        //
        UARTCharPut(UART0_BASE, *pui8Buffer++);
    }
}

void printMenu(){
    UARTSend((uint8_t *) "\r\nMenu Selection:", 17);
    UARTSend((uint8_t *) "\r\nM - Print this menu", 21);
    UARTSend((uint8_t *) "\r\nQ - Quit Program", 18);
    UARTSend((uint8_t *) "\r\nS - Toggle Splash Screen", 26);
    UARTSend((uint8_t *) "\r\nA - Toggle SRV Counter", 24);
    UARTSend((uint8_t *) "\r\nD - Toggle Blink", 18);
    UARTSend((uint8_t *) "\r\nC - Clear Terminal Window", 27);
}

void clearMenu() {
  for(int i = 0; i < 30; ++i) {
    UARTSend((uint8_t *) "\n", 1);
  }
  printMenu();
}

void startupScreen() {
    IntMasterDisable();
    sRect.i16XMin = 0;
    sRect.i16YMin = 0;
    
    sRect.i16XMax = GrContextDpyWidthGet(&g_sContext) - 1;
    //sRect.i16XMax = GrContextDpyWidthGet(&g_sContext) - 1;
    
    sRect.i16YMax = 9;
    GrContextForegroundSet(&g_sContext, ClrDarkBlue);
    GrRectFill(&g_sContext, &sRect);
    GrContextForegroundSet(&g_sContext, ClrWhite);
    // Put the application name in the middle of the banner.
    GrContextFontSet(&g_sContext, g_psFontFixed6x8);
    GrStringDrawCentered(&g_sContext, "Nicholas C.", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 4, 0);
    // Initialize the display and write some instructions.   
    GrStringDrawCentered(&g_sContext, "Connect a", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 20, false);
    GrStringDrawCentered(&g_sContext, "terminal", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 30, false);
    GrStringDrawCentered(&g_sContext, "to UART0.", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 40, false);
    GrStringDrawCentered(&g_sContext, "115000,N,8,1", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 50, false);
    IntMasterEnable();
}

void splash() {
    IntMasterDisable();
    clearScreen();
    sRect.i16XMin = 0;
    sRect.i16YMin = 0;
    sRect.i16XMax = GrContextDpyWidthGet(&g_sContext) - 1;
    sRect.i16YMax = 9;
    GrContextForegroundSet(&g_sContext, ClrWhite);
    GrRectFill(&g_sContext, &sRect);
    GrContextFontSet(&g_sContext, g_psFontFixed6x8);
    

    sprintf(buf, "Count: %d", SRV);
    GrContextForegroundSet(&g_sContext, ClrWhite);
    GrStringDrawCentered(&g_sContext, buf, -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 20, false);
    
    IntMasterEnable();
    SysCtlDelay(10000000);
    clearScreen();
}

void processMenu(char local_char){
    switch(local_char) {
    case 'M' :
          printMenu();
          break;
    case 'Q':
          exit(1);
          break;
    case 'A' :
       if (a_toggle == true) {
           a_toggle = false;
      }
      else if (a_toggle == false) {
               a_toggle = true;
      }
          break;
    case 'D' :
      if (led_toggle == true) {
          led_toggle = false;
      }
      else if (led_toggle == false) {
          led_toggle = true;
      }
          break;
    case 'C' :
          clearMenu();
          break;
    case 'S' :
          splash();
          break;
    case 'P' :
          if (timer1_toggle == true) {
          timer1_toggle = false;
          clearScreen();
      }
      else if (timer1_toggle == false) {
          timer1_toggle = true;
      }
          break;
     case 'O' :
          if (adc_display_toggle == true) {
          adc_display_toggle = false;
          clearScreen();
      }
      else if (adc_display_toggle == false) {
          adc_display_toggle = true;
      }
          break;
    default :
          break;
    }
}

void UARTIntHandler(void) {
    uint32_t ui32Status;

    //
    // Get the interrrupt status.
    //
    ui32Status = UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(UART0_BASE, ui32Status);

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(UARTCharsAvail(UART0_BASE))
    {
        int32_t local_char;
        local_char = UARTCharGetNonBlocking(UART0_BASE);
        if(local_char != -1) {
            gNumCharRecv++;
            processMenu(local_char);
            //clearScreen();
    }
  }
}

int
main(void) {
    //80Mhz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
                   SYSCTL_OSC_MAIN);
    //50Mhz
    //SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |
    //               SYSCTL_OSC_MAIN);
    //16Mhz
    //SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
    //                  SYSCTL_XTAL_16MHZ);
    
    volatile uint32_t ui32Loop;
        
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    FPULazyStackingEnable();
    // Initialize the display driver.
    CFAL96x64x16Init();
    
    // Initialize the graphics context and find the middle X coordinate.
    GrContextInit(&g_sContext, &g_sCFAL96x64x16);
    GrContextFontSet(&g_sContext, g_psFontFixed6x8);
    // Enable the UART and GPIO port A
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    
    // Set GPIO A0 and A1 as UART pins.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    // Configure the UART for 115,200, 8-N-1 operation.
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
    
    //Enable Interrupt for the UART
    IntEnable(INT_UART0);
    //Enable UART interrupt Source - RX = Receive Interrupt - RT = Receive Timeout interrupt
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    
    splash();
    
    
    //Enable GPIO port G and set it as an output pin
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOG)){}
    GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);
    
    
    // Enable both timer peripherals used in the example
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)){}
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet());
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER0A);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER1)){}
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER1_BASE, TIMER_A, SysCtlClockGet() / 2);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER1A);
    // Enable processor interrupts.
    IntMasterEnable();
    
    //Enable the pins associated with AIN7, 12, and 13
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD)){}
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_2);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_3);    
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_4);

    //Enable the ADC0 Peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)){}
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceDisable(ADC0_BASE, 3);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0,ADC_CTL_CH13);
    ADCSequenceEnable(ADC0_BASE, 3);
    
   
    
    //
    // Enable the timers.
    //
    TimerEnable(TIMER0_BASE, TIMER_A);
    TimerEnable(TIMER1_BASE, TIMER_A);
    

    printMenu();

    
    
    while(1){

      if(led_toggle == false) {
        GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, GPIO_PIN_2);
        for(ui32Loop = 0; ui32Loop < 500000; ui32Loop++) {}
        GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, 0);
        for(ui32Loop = 0; ui32Loop < 500000; ui32Loop++){}
      }
      ++loopCount;
    }

}