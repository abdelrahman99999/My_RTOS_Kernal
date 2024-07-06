#ifndef __SUPPORT_H__
#define __SUPPORT_H__

#define NULL ((void *)0)

#define GET_NEXT_THREAD(x) (32U - __builtin_clz(x))
#define INT_DISABLE()    __asm volatile ("cpsid i")
#define INT_ENABLE()     __asm volatile ("cpsie i")

#endif // __SUPPORT_H__