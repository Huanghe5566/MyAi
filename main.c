/*
	GBit科技
1、	CAN0 接口连接整车CAN网络。主循环检测K1按键状态。
按键按下后，D1-D8流水灯。同时串口打印流水灯。
2、CAN1接口连接CANFD接口。由外部K2按键触发。
逻辑请在CAN.C中修改。
*/


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

extern uint8_t tick;

#define NOT_PASS 		0
#define SINGLE_PASS 1

void NVIC_Configuration(void)
{
	nvic_priority_group_set(NVIC_PRIGROUP_PRE0_SUB4);
	nvic_irq_enable(CAN0_RX0_IRQn,0, 0);//CAN 接收中断使能
	nvic_irq_enable(CAN1_RX0_IRQn,0, 1);//CANFD 接收中断使能
	nvic_irq_enable(CAN0_EWMC_IRQn,0, 2);//CAN 错误中断使能
	nvic_irq_enable(CAN1_EWMC_IRQn,0, 3);//CANFD 错误中断使能
	nvic_irq_enable(TIMER1_IRQn,0, 4);//定时器1中断使能
	nvic_irq_enable(TIMER2_IRQn,0, 5);//定时器2中断使能
}

void KEY1_SINGLE_CLICK_Handler(void* btn)//KEY1单击按键处理函数
{
	KEY1_state = SINGLE_PASS;
}

void KEY2_SINGLE_CLICK_Handler(void* btn)//KEY2单击按键处理函数
{
	KEY2_state = SINGLE_PASS;
}

int main(void)
{
	// 基础硬件初始化
	systick_config();
	etk_usart0_init(115200);
	LED_Init();
	LED_ALL_OFF();
	can_config();
	Timer1_Init();
	Timer2_Init();
	NVIC_Configuration();
	
	// 按键初始化 (MultiButton)
	key_init();
	
	printf("GD32C103 CAN Gateway Started...\r\n");

	while (1)
	{
		// 处理 CAN 消息解析和转发
		custom_parse_can_msg();
		
		// 1ms 任务轮询 (在 timer_func 中执行)
		if(tick == 1)
		{
			tick = 0;
			custom_timer();
		}
		
		// 50ms 周期任务
		if(time_50ms == 1)
		{
			time_50ms = 0;
			// 处理按键状态
			button_ticks(); 
		}
		
		// 500ms 周期任务 (心跳灯)
		if(time_500ms == 1)
		{
			time_500ms = 0;
			LED_SYS_TOGGLE;
		}
		
		// 处理按键事件
		if(KEY1_state == SINGLE_PASS)
		{
			KEY1_state = NOT_PASS;
			printf("KEY1 Pressed - Triggering Action...\r\n");
			LED_ALL_ON();
		}
	}
}
