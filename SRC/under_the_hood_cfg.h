#ifndef __BSP_CFG_H__
#define __BSP_CFG_H__

#define UNDER_THE_HOOD_DEBUG  1U   //will be supportive to see systic timer interrupt

/* on-board DIOs for PORTA*/
#define TEST_PIN    (1U << 2)
#define TEST_PIN_1  (1U << 3)
#define TEST_PIN_2  (1U << 4)
#define TEST_PIN_3  (1U << 5)
#define TEST_PIN_4  (1U << 6)
#define TEST_PIN_5  (1U << 7)

/* system clock tick [Hz] */
#define BSP_TICKS_PER_SEC 100U

#endif /*__BSP_CFG_H__*/



