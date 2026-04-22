#include "gd32c10x.h"
#include "LED.h"
#include "delay.h"
#include "can.h"
#include "time.h"
#include "key.h"
#include "etk_usart.h"
#include "systick.h"

volatile uint8_t LED_Flag=0;
volatile uint8_t time_50ms=0;
volatile uint8_t time_500ms=0;
uint8_t KEY1_state=0;
uint8_t KEY2_state=0;

extern volatile uint8_t tick;

#define NOT_PASS 		0
#define SINGLE_PASS 1

void NVIC_Configuration(void)
{
	nvic_priority_group_set(NVIC_PRIGROUP_PRE0_SUB4);
	nvic_irq_enable(CAN0_RX0_IRQn,0, 0);
	nvic_irq_enable(CAN1_RX0_IRQn,0, 1);
	nvic_irq_enable(CAN0_EWMC_IRQn,0, 2);
	nvic_irq_enable(CAN1_EWMC_IRQn,0, 3);
	nvic_irq_enable(TIMER1_IRQn,0, 4);
	nvic_irq_enable(TIMER2_IRQn,0, 5);
}

void KEY1_SINGLE_CLICK_Handler(void* btn)
{
	KEY1_state = SINGLE_PASS;
}

void KEY2_SINGLE_CLICK_Handler(void* btn)
{
	KEY2_state = SINGLE_PASS;
}

int main(void)
{
	int send_flag;
	uint8_t test[8]={0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
	
	SystemInit();
	NVIC_Configuration();
	Can_And_CanFD_Init();
	etk_usart_init();
	LED_Init();
	Delay_Init();
	Timer1_Init();
	Timer2_Init();
	key_state_machine_init();
	
	LED_ALL_ON();
	delay_ms(300);
	LED_ALL_OFF();
	
	while(1)
	{
		if(KEY1_state==SINGLE_PASS)
		{
			KEY1_state=NOT_PASS;
			send_flag = CAN_BUS_Send_Std_Data(CAN0,0x123,test,8);
			if(send_flag == 1) LED_CAN0_R_On; else LED_CAN0_G_On;
		}
		if(KEY2_state==SINGLE_PASS)
		{
			KEY2_state=NOT_PASS;
			send_flag = CANFD_BUS_Send_Std_Data(CAN1,0x123,test,8);
			if(send_flag == 1) LED_CAN1_R_On; else LED_CAN1_G_On;
		}
		
		// 原子读取并清零，逐个消费积累的 tick，防止丢失导致定时偏差
		if(tick)
		{
			uint8_t pending;
			__disable_irq();
			pending = tick;
			tick = 0;
			__enable_irq();
			while(pending--) custom_timer();
		}
		
		custom_parse_can_msg();
		Change_LED_State_From_Flag(LED_Flag); 
		
		if(time_500ms==1)
		{
			time_500ms=0;
			LED_SYS_TOGGLE;
		}
	}
}
