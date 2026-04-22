#include "time.h"
#include "multi_button.h"

int Timer1_count=0;
int Timer2_count=0;
volatile uint8_t tick = 0;
extern volatile uint8_t time_50ms;
extern volatile uint8_t time_500ms;

void Timer1_Init(void)
{
    timer_parameter_struct timer_initpara;
    rcu_periph_clock_enable(RCU_TIMER1);
    timer_deinit(TIMER1);
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 119;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER1, &timer_initpara);
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(TIMER1, TIMER_INT_UP);
    timer_enable(TIMER1);
}

void Timer2_Init(void)
{
    timer_parameter_struct timer_initpara;
    rcu_periph_clock_enable(RCU_TIMER2);
    timer_deinit(TIMER2);
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = 119;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER2, &timer_initpara);
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);
    timer_enable(TIMER2);
}

void TIMER1_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP))
    {
        timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
        Timer1_count++;
        if(Timer1_count % 50 == 0) time_50ms = 1;
        if(Timer1_count % 500 == 0) time_500ms = 1;
        if(Timer1_count >= 5000) Timer1_count = 0;
        tick = 1;
    }
}

void TIMER2_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP))
    {
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
        Timer2_count++;
        if(Timer2_count % 30 == 0)
        {
            button_ticks();
        }
        if(Timer2_count >= 1000) Timer2_count = 0;
    }
}
