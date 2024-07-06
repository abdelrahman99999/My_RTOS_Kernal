#ifndef __UNDER_THE_HOOD_H__
#define __UNDER_THE_HOOD_H__
#include "TM4C123GH6PM.h" /* the TM4C MCU Peripheral Access Layer (TI) */
#include <stdint.h> 

void System_Init(void);

#define DIO_Set(pin) GPIOA_AHB->DATA_Bits[pin] = pin

#define DIO_Reset(pin) GPIOA_AHB->DATA_Bits[pin] = 0U

void ASSERT(uint8_t condition);

void Set_SysticHandlerCallbacks(void (*ptr1)(void) , void (*ptr2)(void));

#endif /* __UNDER_THE_HOOD_H__ */
