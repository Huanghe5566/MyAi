#ifndef _LED_H_
#define _LED_H_

#include "gd32c10x.h"

#define LED_RCC								rcu_periph_clock_enable(RCU_GPIOA);rcu_periph_clock_enable(RCU_GPIOB);
/*LED SYS*/
#define LED_SYS_Pin        		GPIO_PIN_0
#define LED_SYS_PORT       		GPIOA
#define LED_SYS_Off    				gpio_bit_set(LED_SYS_PORT,LED_SYS_Pin);
#define LED_SYS_On     				gpio_bit_reset(LED_SYS_PORT,LED_SYS_Pin);
#define LED_SYS_TOGGLE    		gpio_bit_write(LED_SYS_PORT, LED_SYS_Pin, (FlagStatus)(!(gpio_output_bit_get(LED_SYS_PORT, LED_SYS_Pin))));
/*LED CAN0_Green*/
#define LED_CAN0_G_Pin      	GPIO_PIN_0
#define LED_CAN0_G_PORT     	GPIOB
#define LED_CAN0_G_Off    		gpio_bit_set(LED_CAN0_G_PORT,LED_CAN0_G_Pin);
#define LED_CAN0_G_On     		gpio_bit_reset(LED_CAN0_G_PORT,LED_CAN0_G_Pin);
#define LED_CAN0_G_TOGGLE    	gpio_bit_write(LED_CAN0_G_PORT, LED_CAN0_G_Pin, (FlagStatus)(!(gpio_output_bit_get(LED_CAN0_G_PORT, LED_CAN0_G_Pin))));
/*LED CAN0_Red*/
#define LED_CAN0_R_Pin      	GPIO_PIN_1
#define LED_CAN0_R_PORT     	GPIOB
#define LED_CAN0_R_Off    		gpio_bit_set(LED_CAN0_R_PORT,LED_CAN0_R_Pin);
#define LED_CAN0_R_On     		gpio_bit_reset(LED_CAN0_R_PORT,LED_CAN0_R_Pin);
#define LED_CAN0_R_TOGGLE    	gpio_bit_write(LED_CAN0_R_PORT, LED_CAN0_R_Pin, (FlagStatus)(!(gpio_output_bit_get(LED_CAN0_R_PORT, LED_CAN0_R_Pin))));
/*LED CAN1_Green*/
#define LED_CAN1_G_Pin      	GPIO_PIN_10
#define LED_CAN1_G_PORT     	GPIOB
#define LED_CAN1_G_Off    		gpio_bit_set(LED_CAN1_G_PORT,LED_CAN1_G_Pin);
#define LED_CAN1_G_On     		gpio_bit_reset(LED_CAN1_G_PORT,LED_CAN1_G_Pin);
#define LED_CAN1_G_TOGGLE    	gpio_bit_write(LED_CAN1_G_PORT, LED_CAN1_G_Pin, (FlagStatus)(!(gpio_output_bit_get(LED_CAN1_G_PORT, LED_CAN1_G_Pin))));
/*LED CAN1_Red*/
#define LED_CAN1_R_Pin      	GPIO_PIN_11
#define LED_CAN1_R_PORT     	GPIOB
#define LED_CAN1_R_Off    		gpio_bit_set(LED_CAN1_R_PORT,LED_CAN1_R_Pin);
#define LED_CAN1_R_On     		gpio_bit_reset(LED_CAN1_R_PORT,LED_CAN1_R_Pin);
#define LED_CAN1_R_TOGGLE    	gpio_bit_write(LED_CAN1_R_PORT, LED_CAN1_R_Pin, (FlagStatus)(!(gpio_output_bit_get(LED_CAN1_R_PORT, LED_CAN1_R_Pin))));
/*LED1*/
#define LED1_Pin      				GPIO_PIN_15
#define LED1_PORT     				GPIOA
#define LED1_Off    					gpio_bit_set(LED1_PORT,LED1_Pin);
#define LED1_On     					gpio_bit_reset(LED1_PORT,LED1_Pin);
#define LED1_TOGGLE    				gpio_bit_write(LED1_PORT, LED1_Pin, (FlagStatus)(!(gpio_output_bit_get(LED1_PORT, LED1_Pin))));
/*LED2*/
#define LED2_Pin      				GPIO_PIN_3
#define LED2_PORT     				GPIOB
#define LED2_Off    					gpio_bit_set(LED2_PORT,LED2_Pin);
#define LED2_On     					gpio_bit_reset(LED2_PORT,LED2_Pin);
#define LED2_TOGGLE    				gpio_bit_write(LED2_PORT, LED2_Pin, (FlagStatus)(!(gpio_output_bit_get(LED2_PORT, LED2_Pin))));
/*LED3*/
#define LED3_Pin      				GPIO_PIN_4
#define LED3_PORT     				GPIOB
#define LED3_Off    					gpio_bit_set(LED3_PORT,LED3_Pin);
#define LED3_On     					gpio_bit_reset(LED3_PORT,LED3_Pin);
#define LED3_TOGGLE    				gpio_bit_write(LED3_PORT, LED3_Pin, (FlagStatus)(!(gpio_output_bit_get(LED3_PORT, LED3_Pin))));
/*LED4*/
#define LED4_Pin      				GPIO_PIN_5
#define LED4_PORT     				GPIOB
#define LED4_Off    					gpio_bit_set(LED4_PORT,LED4_Pin);
#define LED4_On     					gpio_bit_reset(LED4_PORT,LED4_Pin);
#define LED4_TOGGLE    				gpio_bit_write(LED4_PORT, LED4_Pin, (FlagStatus)(!(gpio_output_bit_get(LED4_PORT, LED4_Pin))));
/*LED5*/
#define LED5_Pin      				GPIO_PIN_6
#define LED5_PORT     				GPIOB
#define LED5_Off    					gpio_bit_set(LED5_PORT,LED5_Pin);
#define LED5_On     					gpio_bit_reset(LED5_PORT,LED5_Pin);
#define LED5_TOGGLE    				gpio_bit_write(LED5_PORT, LED5_Pin, (FlagStatus)(!(gpio_output_bit_get(LED5_PORT, LED5_Pin))));
/*LED6*/
#define LED6_Pin      				GPIO_PIN_7
#define LED6_PORT     				GPIOB
#define LED6_Off    					gpio_bit_set(LED6_PORT,LED6_Pin);
#define LED6_On     					gpio_bit_reset(LED6_PORT,LED6_Pin);
#define LED6_TOGGLE    				gpio_bit_write(LED6_PORT, LED6_Pin, (FlagStatus)(!(gpio_output_bit_get(LED6_PORT, LED6_Pin))));
/*LED7*/
#define LED7_Pin      				GPIO_PIN_8
#define LED7_PORT     				GPIOB
#define LED7_Off    					gpio_bit_set(LED7_PORT,LED7_Pin);
#define LED7_On     					gpio_bit_reset(LED7_PORT,LED7_Pin);
#define LED7_TOGGLE    				gpio_bit_write(LED7_PORT, LED7_Pin, (FlagStatus)(!(gpio_output_bit_get(LED7_PORT, LED7_Pin))));
/*LED8*/
#define LED8_Pin      				GPIO_PIN_9
#define LED8_PORT     				GPIOB
#define LED8_Off    					gpio_bit_set(LED8_PORT,LED8_Pin);
#define LED8_On     					gpio_bit_reset(LED8_PORT,LED8_Pin);
#define LED8_TOGGLE    				gpio_bit_write(LED8_PORT, LED8_Pin, (FlagStatus)(!(gpio_output_bit_get(LED8_PORT, LED8_Pin))));

void LED_Init(void);
void LED_ALL_ON(void);
void LED_ALL_OFF(void);
void Change_LED_State_From_Flag(uint8_t flag);
#endif //_LED_H_
