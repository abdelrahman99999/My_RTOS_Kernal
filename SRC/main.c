#include <stdint.h>
#include "RTOS_Kernel.h"
#include "under_the_hood.h"
#include "under_the_hood_cfg.h"

uint32_t stack_Thread1[40];
OSThread Thread1;
void main_Thread1() {
    while (1) {
        uint32_t volatile i;
        for (i = 5000U; i != 0U; --i) {
            DIO_Set(TEST_PIN_2);
            DIO_Reset(TEST_PIN_2);

        }
        OS_delay(1U); /* block for 1 tick */
    }
}

uint32_t stack_Thread2[40];
OSThread Thread2;
void main_Thread2() {
    while (1) {
        uint32_t volatile i;
        for (i = 6000U; i != 0U; --i) {
            DIO_Set(TEST_PIN_3);
            DIO_Reset(TEST_PIN_3);
        }
        OS_delay(2U); /* block for 2 ticks */
    }
}

uint32_t stack_Thread3[40];
OSThread Thread3;
void main_Thread3() {
    while (1) {
         uint32_t volatile i;
        for (i = 15000U; i != 0U; --i) {
            DIO_Set(TEST_PIN_4);
            DIO_Reset(TEST_PIN_4);

        }
        OS_delay(4U); /* block for 4 ticks */
    }
}

uint32_t stack_Thread4[40];
OSThread Thread4;
void main_Thread4() {
    while (1) {
        uint32_t volatile i;
        for (i = 1500U; i != 0U; --i) {
            DIO_Set(TEST_PIN_5);
            DIO_Reset(TEST_PIN_5);

        }
        OS_delay(5U); /* block for 5 tick */
    }
}




int main() {
    System_Init();
    OS_init();

    /* start Thread1  */
    OSThread_start(&Thread1,
                   1U, /* priority */
                   &main_Thread1,
                   stack_Thread1, sizeof(stack_Thread1));

    /* start Thread2  */
    OSThread_start(&Thread2,
                   2U, /* priority */
                   &main_Thread2,
                   stack_Thread2, sizeof(stack_Thread2));

    /* start Thread3  */
    OSThread_start(&Thread3,
                   3U, /* priority */
                   &main_Thread3,
                   stack_Thread3, sizeof(stack_Thread3));
		
		    /* start Thread4  */
    OSThread_start(&Thread4,
                   4U, /* priority */
                   &main_Thread4,
                   stack_Thread4, sizeof(stack_Thread4));
									 
    /* transfer control to the RTOS to run the threads */
    OS_run();

}
