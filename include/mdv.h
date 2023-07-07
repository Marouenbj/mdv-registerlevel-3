#pragma once
#ifndef MDV_LIBRARY_H
#define MDV_LIBRARY_H

#ifdef __CLANG_ATOMICS
#define __CLANG_ATOMICS __GNUC_ATOMICS
#endif

extern void lock_mutex (void *mutex);
extern void unlock_mutex (void *mutex);

extern void sem_inc (void *semaphore);
extern void sem_dec (void *semaphore);

#include "stdio.h"
#include "stdlib.h"

#include "stddef.h"
#include "stdint.h"
#include "string.h"

#include "stdbool.h"

#include "stdatomic.h"

#include "cmsis_gcc.h"

#include "core_periph.h"
#include "periph.h"

#include "ledctrl.h"

#include "filter.h"

#include "fsm.h"

#include "command.h"

// https://github.com/ARM-software/CMSIS_6/blob/main/CMSIS/Core/Include/core_cm4.h
/**
  \brief   System Reset
  \details Initiates a system reset request to reset the
  MCU.
 */
__NO_RETURN __STATIC_INLINE void
__NVIC_SystemReset (void)
{
  __DSB (); /* Ensure all outstanding memory accesses
               included buffered write are completed
               before reset */
  SCB->AIRCR
      = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos)
                   | (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk)
                   | SCB_AIRCR_SYSRESETREQ_Msk); /* Keep
                                                    priority
                                                    group
                                                    unchanged
                                                  */
  __DSB (); /* Ensure completion of memory access */

  for (;;) /* wait until reset */
    {
      __NOP ();
    }
}

#endif