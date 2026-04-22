#include "time.h"
#include "multi_button.h"
int Timer1_count=0;
int Timer2_count=0;
volatile uint8_t tick = 0;
extern volatile uint8_t time_50ms;
extern volatile uint8_t time_500ms;
void Timer1_Init(void) { /* ... implementation ... */ }
void Timer2_Init(void) { /* ... implementation ... */ }
void TIMER1_IRQHandler(void) { 
    if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP)) {
        timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
        Timer1_count++;
        if((Timer1_count!=0)&&(Timer1_count%100==0)) time_50ms=1;
        if((Timer1_count!=0)&&(Timer1_count%500==0)) time_500ms=1;
        if(Timer1_count>=5000) Timer1_count=0;
        tick++; 
    }
}
void TIMER2_IRQHandler(void) { /* ... implementation ... */ }
