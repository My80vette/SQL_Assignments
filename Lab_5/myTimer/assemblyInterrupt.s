#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
  name isr_asm
  section .text:CODE
  extern SRV
  public isr_asm_start
isr_asm_start:

  MOVS          R1, #1                  ;Register 1 has the value 1
  MOV32         R0, #0x40031000         ;Register 0 has the value of the system clock
  
  STR           R1, [R0, #0x24]         ;Store the value of R1 at the address
                                        ;stored in R0, with an offset of 24 bits
 
  MOV32         R1, SRV                 ;store the address of SRV in R1
  LDR           R2, [R1]                ;Load Into R2, the value at the address of R1 
                                        ;(The value of SRV)
  ADD           R2, R2, #1              ;Add 1 to the value stored in R2
  STR           R2, [R1]                ;Store the value of R2 at the address
                                        ;pointed to by R1 
                                        ;(replace SRV value with whatever is in R2)
  BX            LR                      ;Branch to address of the Link Register


  end
