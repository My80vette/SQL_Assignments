//*****************************************************************************
//
// uart_echo.c - Example for reading data from and writing data to the UART in
//               an interrupt driven fashion.
//
// Copyright (c) 2011-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the DK-TM4C123G Firmware Package.
//
//*****************************************************************************
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"
#include "grlib/grlib.h"
#include "drivers/cfal96x64x16.h"
#include "utils/uartstdio.h"
bool f_toggle = false;
bool l_toggle = false;
bool u_toggle = false;
bool led_toggle = false;
bool b_toggle = false;
bool s_toggle = false;
bool a_toggle = false;
bool bar_toggle = false;
tRectangle sRect;
tContext sContext;
int gNumCharRecv = 0;
int loopCount = 0;

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>UART Echo (uart_echo)</h1>
//!
//! This example application utilizes the UART to echo text.  The first UART
//! (connected to the USB debug virtual serial port on the evaluation board)
//! will be configured in 115,200 baud, 8-n-1 mode.  All characters received on
//! the UART are transmitted back to the UART.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


//*****************************************************************************
//
// The UART interrupt handler.
//
//*****************************************************************************
void
UARTIntHandler(void)
{
    uint32_t ui32Status;

    //
    // Get the interrrupt status.
    //
    ui32Status = ROM_UARTIntStatus(UART0_BASE, true);

    //
    // Clear the asserted interrupts.
    //
    ROM_UARTIntClear(UART0_BASE, ui32Status);

    //
    // Loop while there are characters in the receive FIFO.
    //
    while(ROM_UARTCharsAvail(UART0_BASE))
    {
        //
        // Read the next character from the UART and write it back to the UART.
        //
        ROM_UARTCharPutNonBlocking(UART0_BASE,
                                   ROM_UARTCharGetNonBlocking(UART0_BASE));
    }
}


//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void
UARTSend(const uint8_t *pui8Buffer, uint32_t ui32Count)
{
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

//*****************************************************************************
//
// This example demonstrates how to send a string of data to the UART.
//
//*****************************************************************************

 //  UARTCharPut(UART0_BASE, local_char);
 //}

void printMenu(){
    UARTSend((uint8_t *) "\r\nMenu Selection:", 17);
    UARTSend((uint8_t *) "\r\nM - Print this menu", 21);
    UARTSend((uint8_t *) "\r\nB - Toggle Button Display", 27);
    UARTSend((uint8_t *) "\r\nQ - Quit Program", 18);
    UARTSend((uint8_t *) "\r\nL - Toggle Loop Count Display", 31);
    UARTSend((uint8_t *) "\r\nU - Toggle UART Recv/Xmit Display", 35);
    UARTSend((uint8_t *) "\r\nF - Toggle Flood Character Enable", 35);
    UARTSend((uint8_t *) "\r\nD - Toggle Flashing LED", 25);
    UARTSend((uint8_t *) "\r\nC - Clear Terminal Window", 27);
}

void clearMenu() {
  for(int i = 0; i < 30; ++i) {
    UARTSend((uint8_t *) "\n", 1);
  }
  printMenu();
}

void clearScreen() {
    sRect.i16XMin = 0;
    sRect.i16YMin = 0;
    sRect.i16XMax = GrContextDpyWidthGet(&sContext) - 1;
    sRect.i16YMax = GrContextDpyHeightGet(&sContext);
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);
}

void printNums(int num){
    clearScreen();
    char buf[10];
    sprintf(buf, "%d", num);
    GrContextForegroundSet(&sContext, ClrWhite);
    GrStringDrawCentered(&sContext, buf, -1,
                         GrContextDpyWidthGet(&sContext) / 2, 20, false);
}

void startupSplash() {
sRect.i16XMin = 0;
    sRect.i16YMin = 0;
    sRect.i16XMax = GrContextDpyWidthGet(&sContext) - 1;
    sRect.i16YMax = 9;
    GrContextForegroundSet(&sContext, ClrDarkBlue);
    GrRectFill(&sContext, &sRect);


    // Change foreground for white text.
    GrContextForegroundSet(&sContext, ClrWhite);


    // Put the application name in the middle of the banner.
    GrContextFontSet(&sContext, g_psFontFixed6x8);
    GrStringDrawCentered(&sContext, "Nicholas C.", -1,
                         GrContextDpyWidthGet(&sContext) / 2, 4, 0);


    // Initialize the display and write some instructions.   
    GrStringDrawCentered(&sContext, "Connect a", -1,
                         GrContextDpyWidthGet(&sContext) / 2, 20, false);
    GrStringDrawCentered(&sContext, "terminal", -1,
                         GrContextDpyWidthGet(&sContext) / 2, 30, false);
    GrStringDrawCentered(&sContext, "to UART0.", -1,
                         GrContextDpyWidthGet(&sContext) / 2, 40, false);
    GrStringDrawCentered(&sContext, "115000,N,8,1", -1,
                         GrContextDpyWidthGet(&sContext) / 2, 50, false);
}

void splash() {
    clearScreen();
    sRect.i16XMin = 0;
    sRect.i16YMin = 0;
    sRect.i16XMax = GrContextDpyWidthGet(&sContext) - 1;
    sRect.i16YMax = 9;
    GrContextForegroundSet(&sContext, ClrWhite);
    GrRectFill(&sContext, &sRect);
    GrStringDrawCentered(&sContext, "This Is", -1,
                         GrContextDpyWidthGet(&sContext) / 2, 20, false);
    GrStringDrawCentered(&sContext, "A", -1,
                         GrContextDpyWidthGet(&sContext) / 2, 30, false);
    GrStringDrawCentered(&sContext, "Custom.", -1,
                         GrContextDpyWidthGet(&sContext) / 2, 40, false);
    GrStringDrawCentered(&sContext, "Splash Screen", -1,
                         GrContextDpyWidthGet(&sContext) / 2, 50, false);
    SysCtlDelay(15000000);
    clearScreen();
    startupSplash();
}



void processMenu(char local_char){
    switch(local_char) {
    case 'M' :
          printMenu();
          break;
    case 'Q':
          exit(1);
          break;
    case 'L' :
      if (l_toggle == true) {
         l_toggle = false;
      }
      else if (l_toggle == false) {
         l_toggle = true;
      } 
          break;
    case 'U' :
      if (u_toggle == true) {
          u_toggle = false;
      }
      else if (u_toggle == false) {
          u_toggle = true;
          clearScreen();
      } 
   
          break;
    case 'F' :
      if (f_toggle == true) {
          f_toggle = false;
      }
      else if (f_toggle == false) {
          f_toggle = true;
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
    case 'B' :
      if (b_toggle == true) {
            b_toggle = false;
      }
      else if (b_toggle == false) {
               b_toggle = true;
      }
          break;
    case 'C' :
          clearMenu();
          break;
    default :
          break;
    case 'S' :
      if (s_toggle == true) {
            s_toggle = false;
      }
      else if (s_toggle == false) {
               s_toggle = true;
      }
          break;
    case 'A' :
      if (a_toggle == true) {
            a_toggle = false;
      }
      else if (a_toggle == false) {
               a_toggle = true;
      }
          break;
    case 'P' :
      if (bar_toggle == true) {
            bar_toggle = false;
      }
      else if (bar_toggle == false) {
               bar_toggle = true;
      }
      break;
  }
}
int main(void) {
    volatile uint32_t ui32Loop;
    uint32_t ADC0Value[3];

    

    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    // FPULazyStackingEnable();


    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);
    
    // Initialize the display driver.
    CFAL96x64x16Init();

    // Initialize the graphics context.
    GrContextInit(&sContext, &g_sCFAL96x64x16);
    startupSplash();    

    // Enable the peripherals used by this example.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
       
    
      
    // Enable processor interrupts.
    //IntMasterEnable();

    // Set GPIO A0 and A1 as UART pins.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    
    // Configure the UART for 115,200, 8-N-1 operation.
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOG)){}
    GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);


    //Enable the pins associated with AIN7, 12, and 13
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD)){}
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_2);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_3);    
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_4);

    //Enable the ADC0 Peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);  
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceDisable(ADC0_BASE, 1);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0,ADC_CTL_CH13);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1,ADC_CTL_CH12);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2,ADC_CTL_CH7 | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 1);

   

//Trigger adc conversion
//ADCProcessorTrigger(ADC0_BASE, 1);
//Read ADC Value
// ADCSequenceDataGet(ADC0_BASE, 1, ADC0Value);
    
    //Beneath this line is the code for menu functions
    printMenu();
    while(1)
    {   
        if(UARTCharsAvail(UART0_BASE)) {
        int32_t local_char;
        local_char = UARTCharGetNonBlocking(UART0_BASE);
        if(local_char != -1) {
            gNumCharRecv++;
            processMenu(local_char);
            clearScreen();
        }
        
      }
      
      if(f_toggle == true) {
          UARTCharPutNonBlocking(UART0_BASE, 'L');
          SysCtlDelay(800000);
      }
      if(l_toggle == true) {
          printNums(loopCount);
      }
      if(led_toggle == true) {
          GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, GPIO_PIN_2);
          for(ui32Loop = 0; ui32Loop < 500000; ui32Loop++) {}
          GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, 0);
          for(ui32Loop = 0; ui32Loop < 500000; ui32Loop++) {}
      }
      if(s_toggle == true) {
          splash();
          s_toggle = false;
      }
      if(b_toggle == true) {
          clearScreen();
          SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM);
          while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOM)) {}
          GPIOPinTypeGPIOInput(GPIO_PORTM_BASE, GPIO_PIN_0);
          GPIOPadConfigSet(GPIO_PORTM_BASE, GPIO_PIN_0, 
                           GPIO_STRENGTH_2MA, 
                           GPIO_PIN_TYPE_STD_WPU);
          if(GPIOPinRead(GPIO_PORTM_BASE, GPIO_PIN_0) == 0) {
             clearScreen();
             printNums(gNumCharRecv);
             b_toggle = false;
          }
      }
      if(a_toggle == true) {
          clearScreen();
          //Trigger adc conversion
          ADCProcessorTrigger(ADC0_BASE, 1);
          //Read ADC Value
          ADCSequenceDataGet(ADC0_BASE, 1, ADC0Value);
          char myArr[20];
          GrContextForegroundSet(&sContext, ClrWhite);
          //Take the values from the ADC0Value buffer
          //and make them Chars in the myArr Buffer
          //This allows the use of GtStringDrawCentered
          sprintf(myArr,"AIN0 = %d", ADC0Value[0]);
          GrStringDrawCentered(&sContext, myArr, -1,
                                GrContextDpyWidthGet(&sContext) / 2, 10, true);
          sprintf(myArr,"AIN1 = %d", ADC0Value[1]);
          GrStringDrawCentered(&sContext, myArr, -1,
                                GrContextDpyWidthGet(&sContext) / 2, 20, true); 
          sprintf(myArr,"AIN2 = %d", ADC0Value[2]);
          GrStringDrawCentered(&sContext, myArr, -1,
                                GrContextDpyWidthGet(&sContext) / 2, 30, true);
      }
      
      if(bar_toggle == true) {
          clearScreen();
          int screenWidth = GrContextDpyWidthGet(&sContext);
           //Trigger adc conversion
          ADCProcessorTrigger(ADC0_BASE, 1);
          //Read ADC Value
          ADCSequenceDataGet(ADC0_BASE, 1, ADC0Value);
          tRectangle rectLine1;
          rectLine1.i16XMin = 0;
          rectLine1.i16YMin = 0;
          rectLine1.i16XMax = (ADC0Value[0] / screenWidth) * 2;
          rectLine1.i16YMax = 10;
          
          tRectangle rectLine2;
          rectLine2.i16XMin = 0;
          rectLine2.i16YMin = 11;
          rectLine2.i16XMax = (ADC0Value[1] / screenWidth) * 2;
          rectLine2.i16YMax = 21;
          
          tRectangle rectLine3;
          rectLine3.i16XMin = 0;
          rectLine3.i16YMin = 22;
          rectLine3.i16XMax = (ADC0Value[2] / screenWidth) * 2;
          rectLine3.i16YMax = 32;
          
          
          GrContextForegroundSet(&sContext, ClrWhite);
          GrRectFill(&sContext, &rectLine1);
          GrContextForegroundSet(&sContext, ClrBlue);
          GrRectFill(&sContext, &rectLine2);
          GrContextForegroundSet(&sContext, ClrRed);
          GrRectFill(&sContext, &rectLine3);
          
      }  
      loopCount++;
    }

}
 