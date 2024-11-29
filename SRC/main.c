#include <stdint.h>
#include "RTOS_Kernal.h"
#include "bsp.h"

uint32_t stack_blinky1[40];
OSThread blinky1;
void main_blinky1() {
    while (1) {
        uint32_t volatile i;
        for (i = 1500U; i != 0U; --i) {
            BSP_ledGreenOn();
            BSP_ledGreenOff();
        }
        OS_delay(3U); /* block for 1 tick */
    }
}

uint32_t stack_blinky2[40];
OSThread blinky2;
void main_blinky2() {
    while (1) {
        uint32_t volatile i;
        for (i = 9*1500U; i != 0U; --i) {
            BSP_ledBlueOn();
            BSP_ledBlueOff();
        }
        OS_delay(50U); /* block for 50 ticks */
    }
}


uint32_t stack_idleThread[40];

int main() {
    BSP_init();
    OS_init(stack_idleThread, sizeof(stack_idleThread));

    /* start blinky1 thread */
    OSThread_start(&blinky1,
                   5U, /* priority */
                   &main_blinky1,
                   stack_blinky1, sizeof(stack_blinky1));

    /* start blinky2 thread */
    OSThread_start(&blinky2,
                   6U, /* priority */
                   &main_blinky2,
                   stack_blinky2, sizeof(stack_blinky2));


    /* transfer control to the RTOS to run the threads */
    OS_run();

    //return 0;
}
