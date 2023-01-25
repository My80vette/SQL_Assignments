#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
  name timer_one_interrupt
  section .text:CODE
  extern SRV
  public isr_asm_start
isr_asm_start:
  push (r7,lr)
  MOVS r1, #1
  LDR r0, [pc, #0x3e4]
  BL TimerIntClear
  
  LDR r0, [pc, #0x3d8]
  LDR r1, [r0, #0x38]
  ADDS r1, r1, #1
  STR r1, [r0, #0x38]







pop {r0,pc} ; return
end