#include "under_the_hood.h"
#include "under_the_hood_cfg.h"
#include "support.h"


static void (*OsTickCallBack)(void) = NULL;
static void (*OsSchedullingCallBack)(void) = NULL;


void SysTick_Handler(void) {
#if UNDER_THE_HOOD_DEBUG == 1U
    GPIOA_AHB->DATA_Bits[TEST_PIN] = TEST_PIN;
#endif
    (*OsTickCallBack)();

    __disable_irq();
    (*OsSchedullingCallBack)();
    __enable_irq();
#if UNDER_THE_HOOD_DEBUG == 1U
    GPIOA_AHB->DATA_Bits[TEST_PIN] = 0U;
#endif
}

void System_Init(void) {
    SYSCTL->GPIOHBCTL |= (1U << 0); /* enable AHB for GPIOA */
    SYSCTL->RCGCGPIO  |= (1U << 0); /* enable Run Mode for GPIOA */

		//Digital Output enables
    GPIOA_AHB->DIR |= (TEST_PIN | TEST_PIN_1 | TEST_PIN_2 | TEST_PIN_3 | TEST_PIN_4 | TEST_PIN_5);
    GPIOA_AHB->DEN |= (TEST_PIN | TEST_PIN_1 | TEST_PIN_2 | TEST_PIN_3 | TEST_PIN_4 | TEST_PIN_5);
	
}

void OS_onStartup(void) {
	
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / BSP_TICKS_PER_SEC);

    /* set the SysTick interrupt priority (highest) */
    NVIC_SetPriority(SysTick_IRQn, 0U);
}


void assert_failed();

void assert_failed() {
    while(1){
		}
}
void ASSERT(uint8_t condition){
	if(condition){
	
	}else{
		assert_failed();
	}
}


void Set_SysticHandlerCallbacks(void (*ptr1)(void) , void (*ptr2)(void)){
    OsTickCallBack = ptr1;
    OsSchedullingCallBack = ptr2;

}

