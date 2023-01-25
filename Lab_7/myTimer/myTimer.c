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
#define MOTOR_OFF (0x00)
#define RPM_MAX 200

bool led_toggle = false;
bool direction_toggle = false;
bool stop = false;
bool move_toggle = true;
int rpm = 30;
int countdown = 0;

int drive_mode = 3;
int target;
int move = 0;
bool drive_arr[4];
uint32_t ADC0Value[3];
uint32_t ui32Loop;
char buf[10];
int gNumCharRecv = 0;
int loopCount = 0;
int count = 0;
tRectangle sRect;
tContext g_sContext;
uint32_t g_ui32Flags;
tContext g_sContext;
const uint8_t full_step_array[4] = {0x0C, 0x06, 0x03, 0x09};
const uint8_t wave_drive_array[4] = {0x08, 0x04, 0x02, 0x01};
uint32_t pui32ADC0Value[5];
int step = 0;
int step2 = 3;
int oldADC = 0;
int newADC = 0;

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
    UARTSend((uint8_t *) "\r\nD - Toggle Motor Direction", 28);
    UARTSend((uint8_t *) "\r\nL - Toggle LED Blinking", 25);
    UARTSend((uint8_t *) "\r\nC - Clear Terminal Window", 27);
    UARTSend((uint8_t *) "\r\nSpace - Change Drive Mode", 27);
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
    sprintf(buf, "RPM: %d", rpm);
    GrContextForegroundSet(&g_sContext, ClrWhite);
    GrStringDrawCentered(&g_sContext, buf, -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 20, false);
    
    IntMasterEnable();
    SysCtlDelay(5000000);
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
    case 'L' :
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
    case 'D' :
      if (direction_toggle == true) {
          UARTSend((uint8_t *) "\r\nGoing Backwards", 17);
          direction_toggle = false;
      }
      else if (direction_toggle == false) {
          UARTSend((uint8_t *) "\r\nGoing Forwards", 16);
          direction_toggle = true;
      }
          break;
    case ' ' :
      ++drive_mode;
      if (drive_mode == 4) {
          drive_mode = 1;
      }
      if (drive_mode == 1) {
        UARTSend((uint8_t *) "\r\nFull Step", 11);
      } else if (drive_mode == 2) {
        UARTSend((uint8_t *) "\r\nWave Step", 11);
      } else if (drive_mode == 3) {
        UARTSend((uint8_t *) "\r\nFollow Mode", 16);
      }
          break;
    case '=' :
      if (rpm < RPM_MAX);
      rpm+=10;
      countdown = ((rpm * 200) / 60);
      TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / (countdown));
      GrContextForegroundSet(&g_sContext, ClrWhite);
      GrContextFontSet(&g_sContext, g_psFontFixed6x8);
      GrStringDraw(&g_sContext, "RPM:", -1, 16, 35, 1);
      sprintf(buf,"%d      ", rpm);
      GrStringDraw(&g_sContext, buf, -1, 60, 35, 1);
          break;
    case '-' :
      if (rpm > 0) {
      rpm-=10;
      countdown = ((rpm * 200) / 60);
      TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet() / (countdown));
      }
      GrContextForegroundSet(&g_sContext, ClrWhite);
      GrContextFontSet(&g_sContext, g_psFontFixed6x8);
      GrStringDraw(&g_sContext, "RPM:", -1, 16, 35, 1);
      sprintf(buf,"%d      ", rpm);
      GrStringDraw(&g_sContext, buf, -1, 60, 35, 1);
           break;
    default  :
          break;
    case '!' :
      if (stop == 0) {
        UARTSend((uint8_t *) "\r\nStopping Motor", 16);
        stop = 1;
      } else if (stop == 1) {
        UARTSend((uint8_t *) "\r\nStarting Motor", 16);
        stop = 0;
      }
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
            gNumCharRecv++;
            processMenu(local_char);
    }
  }
}

void Timer0IntHandler(void) {
    //Clear The interrupt flag
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
//    IntMasterDisable();
    // Full-Step Foward
    if(drive_mode == 1 && direction_toggle == 1) {
     GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0 | GPIO_PIN_1 | 
                  GPIO_PIN_2 | GPIO_PIN_3, full_step_array[step]);  
     step++;
     if(step > 3) {
       step = 0;
     }
    }
    // Wave-Drive Forward
    if(drive_mode == 2 && direction_toggle == 1) {
      GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0 | GPIO_PIN_1 | 
                  GPIO_PIN_2 | GPIO_PIN_3, wave_drive_array[step]);  
     step++;
     if(step > 3) {
       step = 0;
     }
    }        
    // Full-Step Backward
    if(drive_mode == 1 && direction_toggle == 0) {
     GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0 | GPIO_PIN_1 | 
                  GPIO_PIN_2 | GPIO_PIN_3, full_step_array[step2]);  
     step2--;
     if(step2 < 0) {
       step2 = 3;
     }
    }
    // Wave-Drive Backward
    if(drive_mode == 2 && direction_toggle == 0) {
     GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0 | GPIO_PIN_1 | 
                  GPIO_PIN_2 | GPIO_PIN_3, wave_drive_array[step2]);  
     step2--;
     if(step2 < 0) {
       step2 = 3;
     }
    }
    if(drive_mode == 3) {
      
      if (move_toggle == true) {
        move_toggle = false;
        oldADC = newADC;
        // Trigger and wait for the ADC conversion.
        ADCProcessorTrigger(ADC0_BASE, 3);
        // Read ADC Value.
        ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);     
        // Matching the potentiometer steps to the motor steps 
        newADC = ((pui32ADC0Value[0] * 200) / (4095));
        
        // Forward movement for the potentiometer
        //NewADC and OldADC need to be global or else this doesnt work??? Coding Is So Evil Sometimes
      
        
      GrContextForegroundSet(&g_sContext, ClrWhite);
      GrContextFontSet(&g_sContext, g_psFontFixed6x8);
      sprintf(buf,"%d  ",pui32ADC0Value[0] );
      GrStringDraw(&g_sContext, buf, -1, 60, 35, 1);
      
        target = abs(newADC - oldADC);
      }
        
        if (move < target){
          
          if (newADC > oldADC) {
            // Writing to the motor 
            GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0 | GPIO_PIN_1 | 
                    GPIO_PIN_2 | GPIO_PIN_3, wave_drive_array[step]);
            step++;
            oldADC++;
            if(step > 3) {
              step = 0;
            }
          }
          
            // Backward movement for the potentiometer and motor 
            else if(newADC < oldADC) {
                    // Writing to the motor 
                    GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0 | GPIO_PIN_1 | 
                    GPIO_PIN_2 | GPIO_PIN_3, wave_drive_array[step]);
            step--;
            oldADC--;
            if(step < 0) {
              step = 3;
            }
          }
          step++;

        }  else {
          move = 0;
          move_toggle = true;
        }
    }
    if(stop == 1) {
     GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0 | GPIO_PIN_1 | 
                  GPIO_PIN_2 | GPIO_PIN_3, 0);  
     
    }

}

int
main(void)
{

    FPULazyStackingEnable();
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);     
    IntMasterDisable();    
    CFAL96x64x16Init(); 
    GrContextInit(&g_sContext, &g_sCFAL96x64x16);
    clearScreen();
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL)){}

    // Enable the GPIO pin for the motor. 
    GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE, GPIO_PIN_0 | GPIO_PIN_1 | 
                          GPIO_PIN_2 | GPIO_PIN_3);  
    
    // Enable the GPIO port that is used for the on-board LED.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOG)){}
    GPIOPinTypeGPIOOutput(GPIO_PORTG_BASE, GPIO_PIN_2);  

    // Enable the peripherals used by this example.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);   
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()/countdown);
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_A);

    // Enable the ADC
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    //ADC0 is used with AIN0 on port D.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_3);
    ADCSequenceDisable(ADC0_BASE, 3);
    ADCSequenceEnable(ADC0_BASE, 3);    
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH12 | 
                             ADC_CTL_END);
    ADCIntEnable(ADC0_BASE, 3);
    
   
    
    // Enable UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
                            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                             UART_CONFIG_PAR_NONE));
    IntEnable(INT_UART0);
    UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
    
    //splash();

        
    printMenu();
    IntMasterEnable();           
    

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