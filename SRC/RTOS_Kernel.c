#include <stdint.h>
#include "RTOS_Kernel.h"
#include "support.h"
#include "under_the_hood.h"
#include "under_the_hood_cfg.h"
#include "RTOS_Kernel_cfg.h"
//#include "TM4C123GH6PM.h"

static OSThread * volatile OS_Current_Thread; /* pointer to the current thread */
static OSThread * volatile OS_Next_Thread; /* pointer to the next thread to run */

static OSThread *OS_thread[MAX_NO_THREADS + 1]; /* array of threads started so far */
static uint32_t OS_readySet; /* bitmask of threads that are ready to run */
static uint32_t OS_delayedSet; /* bitmask of threads that are delayed(blocked) */

static uint32_t stack_idleThread[STACK_SIZE_FOR_IDLE_TASK];

static OSThread idleThread;

static void idle(){
#if RTOS_DEBUG == 1U
		
    DIO_Set(TEST_PIN_1);
		
		DIO_Reset(TEST_PIN_1);

#endif
		
}
void main_idleThread() {
    while (1) {
			idle();
			//__WFI(); /* stop the CPU and Wait for Interrupt */
    }
}

void OS_init() {
    /* set the PendSV interrupt priority to the lowest level 0xFF */
    *(uint32_t volatile *)0xE000ED20 |= (0xFFU << 16);

    Set_SysticHandlerCallbacks(OS_tick,OS_sched);

    /* start idleThread thread */
    OSThread_start(&idleThread,
                   0U, /* idle thread priority */
                   &main_idleThread,
                   stack_idleThread, sizeof(stack_idleThread));
}

void OS_sched(void) {
    /* choose the next thread to execute... */
    OSThread *next;
    if (OS_readySet == 0U) { /* idle condition? */
        next = OS_thread[0]; /* the idle thread */
    }
    else {
        next = OS_thread[GET_NEXT_THREAD(OS_readySet)];
        ASSERT(next != (OSThread *)0);
				
    }

    /* trigger PendSV, if needed */
    if (next != OS_Current_Thread) {
        OS_Next_Thread = next;
        *(uint32_t volatile *)0xE000ED04 = (1U << 28);
    }
}

void OS_run(void) {
    /* callback to configure and start interrupts */
    OS_onStartup();

    INT_DISABLE();
    OS_sched();
    INT_ENABLE();

    /* the following code should never execute */
    ASSERT(0);
}

void OS_tick(void) {
    uint32_t workingSet = OS_delayedSet;
    while (workingSet != 0U) {
        OSThread *t = OS_thread[GET_NEXT_THREAD(workingSet)];
        uint32_t bit;
				
        uint8_t cond = (((t != (OSThread *)0) && (t->timeout != 0U)));
				ASSERT(cond);
        bit = (1U << (t->prio - 1U));
        --t->timeout;
        if (t->timeout == 0U) {
            OS_readySet   |= bit;  /* insert to set */
            OS_delayedSet &= ~bit; /* remove from set */
        }
        workingSet &= ~bit; /* remove from working set */
    }
}

void OS_delay(uint32_t ticks) {
    uint32_t bit;
   INT_DISABLE();

    /* never call OS_delay from the idleThread */
    ASSERT(OS_Current_Thread != OS_thread[0]);

    OS_Current_Thread->timeout = ticks;
    bit = (1U << (OS_Current_Thread->prio - 1U));
    OS_readySet &= ~bit;
    OS_delayedSet |= bit;
    OS_sched();
    INT_ENABLE();
}

void OSThread_start(
    OSThread *me,
    uint8_t prio, /* thread priority */
    OSThreadHandler threadHandler,
    void *stkSto, uint32_t stkSize)
{
    /* round down the stack top to the 8-byte boundary
    * NOTE: ARM Cortex-M stack grows down from hi -> low memory
    */
    uint32_t *sp = (uint32_t *)((((uint32_t)stkSto + stkSize) / 8) * 8);
    uint32_t *stk_limit;

    /* priority must be in ragne
    * and the priority level must be unused
    */
    ASSERT((prio < MAX_NO_THREADS) && (OS_thread[prio] == (OSThread *)0));

    *(--sp) = (1U << 24);  /* xPSR */
    *(--sp) = (uint32_t)threadHandler; /* PC */
    *(--sp) = 0x0000000EU; /* LR  */
    *(--sp) = 0x0000000CU; /* R12 */
    *(--sp) = 0x00000003U; /* R3  */
    *(--sp) = 0x00000002U; /* R2  */
    *(--sp) = 0x00000001U; /* R1  */
    *(--sp) = 0x00000000U; /* R0  */
    /* additionally, fake registers R4-R11 */
    *(--sp) = 0x0000000BU; /* R11 */
    *(--sp) = 0x0000000AU; /* R10 */
    *(--sp) = 0x00000009U; /* R9 */
    *(--sp) = 0x00000008U; /* R8 */
    *(--sp) = 0x00000007U; /* R7 */
    *(--sp) = 0x00000006U; /* R6 */
    *(--sp) = 0x00000005U; /* R5 */
    *(--sp) = 0x00000004U; /* R4 */

    /* save the top of the stack in the thread's attibute */
    me->sp = sp;

    /* round up the bottom of the stack to the 8-byte boundary */
    stk_limit = (uint32_t *)(((((uint32_t)stkSto - 1U) / 8) + 1U) * 8);

    /* pre-fill the unused part of the stack with 0xDEADBEEF */
    for (sp = sp - 1U; sp >= stk_limit; --sp) {
        *sp = 0xDEADBEEFU;
    }

    /* register the thread with the OS */
    OS_thread[prio] = me;
    me->prio = prio;
    /* make the thread ready to run */
    if (prio > 0U) {
        OS_readySet |= (1U << (prio - 1U));
    }
}

__attribute__ ((naked))
void PendSV_Handler(void) {
__asm volatile (
    /* __disable_irq(); */
    "  CPSID         I                 \n"

    /* if (OS_Current_Thread != (OSThread *)0) { */
    "  LDR           r1,=OS_Current_Thread       \n"
    "  LDR           r1,[r1,#0x00]     \n"
    "  CBZ           r1,PendSV_restore \n"

    /*     push registers r4-r11 on the stack */
    "  PUSH          {r4-r11}          \n"

    /*     OS_Current_Thread->sp = sp; */
    "  LDR           r1,=OS_Current_Thread       \n"
    "  LDR           r1,[r1,#0x00]     \n"
    "  STR           sp,[r1,#0x00]     \n"
    /* } */

    "PendSV_restore:                   \n"
    /* sp = OS_Next_Thread->sp; */
    "  LDR           r1,=OS_Next_Thread       \n"
    "  LDR           r1,[r1,#0x00]     \n"
    "  LDR           sp,[r1,#0x00]     \n"

    /* OS_Current_Thread = OS_Next_Thread; */
    "  LDR           r1,=OS_Next_Thread       \n"
    "  LDR           r1,[r1,#0x00]     \n"
    "  LDR           r2,=OS_Current_Thread       \n"
    "  STR           r1,[r2,#0x00]     \n"

    /* pop registers r4-r11 */
    "  POP           {r4-r11}          \n"

    /* __enable_irq(); */
    "  CPSIE         I                 \n"

    /* return to the next thread */
    "  BX            lr                \n"
    );
}