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
#include "driverlib/comp.h"
bool led_toggle = false;
bool s_toggle = false;
bool z_toggle = false;
bool b_toggle = false;
char buf[64];
int loopCount = 0;
int button_count = 0;
int ref_volt_cross = 0;
int comparator = 0;
tRectangle sRect;
tContext g_sContext;
uint32_t g_ui32Flags;
uint8_t button_location;

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
    UARTSend((uint8_t *) "\r\nD - Toggle Blink", 18);
    UARTSend((uint8_t *) "\r\nD - Toggle Button", 19);
    UARTSend((uint8_t *) "\r\nZ - Toggle COMP", 17);
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
    GrContextForegroundSet(&g_sContext, ClrGreen);
    GrRectFill(&g_sContext, &sRect);
    GrContextForegroundSet(&g_sContext, ClrRed);
    GrContextFontSet(&g_sContext, g_psFontFixed6x8);
    GrStringDrawCentered(&g_sContext, "Nicholas C.", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 4, 0); 
    GrStringDrawCentered(&g_sContext, "This Is", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 20, false);
    GrStringDrawCentered(&g_sContext, "Your Personal", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 30, false);
    GrStringDrawCentered(&g_sContext, "Splash Screen.", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 40, false);
    GrContextForegroundSet(&g_sContext, ClrGreen);
    GrStringDrawCentered(&g_sContext, "Merry Christmas", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 50, false);
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
    case 'Z' :
          if (z_toggle == true) {
              z_toggle = false;
              clearScreen();
          }
          else if (z_toggle == false) {
              z_toggle = true;
          }
    case 'B' :
        if (b_toggle == true) {
              b_toggle = false;
              clearScreen();
          }
          else if (b_toggle == false) {
              b_toggle = true;
          }
    default :
          break;
    }
}

void UARTIntHandler(void) {
    uint32_t ui32Status;
    // Get the interrrupt status.
    ui32Status = UARTIntStatus(UART0_BASE, true);
    // Clear the asserted interrupts.
    UARTIntClear(UART0_BASE, ui32Status);
    // Loop while there are characters in the receive FIFO.
    while(UARTCharsAvail(UART0_BASE))
    {
        int32_t local_char;
        local_char = UARTCharGetNonBlocking(UART0_BASE);
        if(local_char != -1) {
            processMenu(local_char);
            //clearScreen();
    }
  }
}

void GPIOIntHandler(void) {
    GPIOIntClear(GPIO_PORTM_BASE, GPIO_PIN_0 | GPIO_PIN_1 |
                 GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4);
    if(b_toggle == true) {
        sprintf(buf, "Presses: %d   ", button_count);
        GrContextForegroundSet(&g_sContext, ClrWhite);
        GrStringDraw(&g_sContext, buf, -1,
                    1, 10, true);
        if (GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_0) == 0) {
            ++button_count;
            GrContextForegroundSet(&g_sContext, ClrWhite);
            GrStringDraw(&g_sContext, "UP    ", -1,
                        1, 20, true);
        } else if (GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_1) == 0) {
            ++button_count;
            GrContextForegroundSet(&g_sContext, ClrWhite);
            GrStringDraw(&g_sContext, "DOWN  ", -1,
                        1, 20, true);
        } else if (GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_2) == 0) {
            ++button_count;
            GrContextForegroundSet(&g_sContext, ClrWhite);
            GrStringDraw(&g_sContext, "LEFT  ", -1,
                        1, 20, true);
        } else if (GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_3) == 0) {
            ++button_count;
            GrContextForegroundSet(&g_sContext, ClrWhite);
            GrStringDraw(&g_sContext, "RIGHT ", -1,
                        1, 20, true);
        } else if (GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_4) == 0) {
            ++button_count;
            GrContextForegroundSet(&g_sContext, ClrWhite);
            GrStringDraw(&g_sContext, "SELECT", -1,
                        1, 20, true);
        }
    }

}

void COMPIntHandler(void) {
    ComparatorIntClear(COMP_BASE, 0);
    comparator = ComparatorValueGet(COMP_BASE, 0);
     GrContextForegroundSet(&g_sContext, ClrWhite);
    IntMasterDisable();
    if (z_toggle == 1){
      if(comparator == false){
        sprintf(buf, "Count = %d  ", ref_volt_cross);
        GrStringDraw(&g_sContext, buf, -1, 10, 40, 1);
        GrStringDraw(&g_sContext, "Light", -1, 10, 50, 1);
        ++ref_volt_cross;
      } else if (comparator == true){
        sprintf(buf, "Count = %d  ", ref_volt_cross);
        GrStringDraw(&g_sContext, buf, -1, 10, 40, 1);
        GrStringDraw(&g_sContext, "Dark", -1, 10, 50, 1);
        ++ref_volt_cross;
      }
    }
    IntMasterEnable();
}

void main(void)
{
    volatile uint32_t ui32Loop;
    FPULazyStackingEnable();
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
    
    
    // Enable processor interrupts.
    IntMasterDisable();
    
    
    CFAL96x64x16Init();
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
    
    
    
    //Put up the splash screen to the OLED
    splash();
    
    
    
    //Enable GPIO port G and set it as an output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOG)){}
    GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);
    IntRegister(INT_UART0, UARTIntHandler);
    
    
    
    
    
    //Enable Port M on the board
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)){}
    //Enable each button as an input
    GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_0, 
                         GPIO_STRENGTH_2MA, 
                         GPIO_PIN_TYPE_STD_WPU);
    GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_1);
    GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_1, 
                         GPIO_STRENGTH_2MA, 
                         GPIO_PIN_TYPE_STD_WPU);
    GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_2);
    GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_2, 
                         GPIO_STRENGTH_2MA, 
                         GPIO_PIN_TYPE_STD_WPU);
    GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_3);
    GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_3, 
                         GPIO_STRENGTH_2MA, 
                         GPIO_PIN_TYPE_STD_WPU);
    GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_4, 
                         GPIO_STRENGTH_2MA, 
                         GPIO_PIN_TYPE_STD_WPU);
    //Enable GPIO Interrupts
    GPIOIntTypeSet(GPIO_PORTM_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |
                   GPIO_PIN_3 | GPIO_PIN_4, GPIO_FALLING_EDGE);
   
    //IntRegister(GPIO_PORTM_BASE, GPIOIntHandler);
    IntEnable(INT_GPIOM);
    GPIOIntEnable(GPIO_PORTM_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1
                  | GPIO_INT_PIN_2 | GPIO_INT_PIN_3 | GPIO_INT_PIN_4);
   
    
    
    
    
    //Enable the analog comparator
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC)){}

    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_COMP0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_COMP0)){}
    
    ComparatorConfigure(COMP_BASE, 0, COMP_TRIG_NONE |
                        COMP_INT_BOTH | COMP_ASRCP_PIN0 | COMP_OUTPUT_NORMAL);
    IntEnable(INT_COMP0);
    ComparatorIntEnable(COMP_BASE, 0);
    //ComparatorIntRegister(COMP_BASE, 0, COMPIntHandler);
    
    
    
    
    
    IntMasterEnable();
    printMenu();
    while(1){
      if(led_toggle == false) {
        GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, GPIO_PIN_2);
        for(ui32Loop = 0; ui32Loop < 200000; ui32Loop++) {}
        GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, 0);
        for(ui32Loop = 0; ui32Loop < 1000000; ui32Loop++){}
      }
      ++loopCount;
    }

}