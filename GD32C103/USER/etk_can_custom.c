#include "etk_can_custom.h"
#include "can.h"
#include "a.h"
#include "b.h"
#include <stdio.h>

extern can_trasnmit_message_struct g_transmit_message;
extern canmsg rcvmsgs[2];

float vf_speed = 0.0f;
float g_yawrate = 0.0f;
uint8_t g_gear = 0;
uint8_t g_gear_val = 0xFB;
uint8_t standstillsts = 1;
uint8_t wheelDirection = 0;

void timer_func_10_spd() {
    struct a_pms_body_control_info_t t;
    t.pms_vehicle_spd_flag = (vf_speed > 0.05) ? 1 : 0; 
    t.pms_vehicle_spd = a_pms_body_control_info_pms_vehicle_spd_encode(vf_speed);
    g_transmit_message.tx_sfid = 0x714;
    g_transmit_message.tx_dlen = 8;
    g_transmit_message.tx_ff = CAN_FF_STANDARD;
    a_pms_body_control_info_pack(&g_transmit_message.tx_data[0], &t, 8);
    can_message_transmit(OUT_CAN, &g_transmit_message);
}

void timer_func_2000_diag() {
    can_trasnmit_message_struct TxMessage;
    can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &TxMessage);
    TxMessage.tx_sfid = 0x700;
    TxMessage.tx_ff = CAN_FF_STANDARD;
    TxMessage.tx_dlen = 8;
    TxMessage.tx_data[0] = 0x02; TxMessage.tx_data[1] = 0x10; TxMessage.tx_data[2] = 0x03;
    can_message_transmit(OUT_CAN, &TxMessage);
}

typedef void (* timer_func)();
struct Timer_ { int ct; int max_ct; timer_func func; } timers[] = {
    {0, 10, timer_func_10_spd},
    {11, 2000, timer_func_2000_diag},
    {-1, -1, NULL}
};

void custom_timer() {
    for(int i=0; i < 10; i++) {
        if(timers[i].ct < 0) break;
        if(++timers[i].ct >= timers[i].max_ct) {
            timers[i].ct = 0;
            if(timers[i].func) timers[i].func();
        }
    }
}
/* ... rest of mapping logic ... */
