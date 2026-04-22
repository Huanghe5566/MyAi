#include "gd32c10x.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "can.h"
#include "a.h"    // 目标 DBC 协议解析/打包库
#include "b.h"    // 源 DBC 协议解析库

// 全局变量定义，用于暂存发送消息结构体
can_trasnmit_message_struct g_transmit_message;

#ifndef EINVAL
#    define EINVAL 22
#endif

// 定义消息转发的目标通道，默认为 CAN0
#define OUT_CAN CAN0 

/**
 * 转换源1：名爵 MG6，对应源 DBC 信号定义：
 * 0x23c - VehSpdAvg_h2HSC2 (平均车速)
 * 0x1e9 - VehDynYawRate_h2HSC2 (横摆角速度)
 * 0x194 - TrEstdGear_h2HSC2 (挡位信号)
 */
#define SCS_HSC2_FrP19 0x23c
#define SCS_HSC2_FrP05 0x1e9
#define GW_HSC2_ECM_FrP04 0x194

/**
 * 转换目标：ME02 项目 DBC 信号值定义
 * 713/714/715 消息组
 */
double g_yawrate = 0.0;     // 存储横摆角速度 (rad/s)
double vf_speed = 7.17;     // 存储车速 (km/h)
int standstillsts = 0;      // 车辆静止状态
int wheelDirection = 0;     // 车轮旋转方向
int breakPedalSwitchStatus; // 制动踏板状态
int g_gear = 0;             // 倒车标志位：1-倒车(R), 0-非倒车
int g_gear_val = 0;         // 挡位数值：R=0xDF, N=0x7D, P=0xFB, D=0xFC

/**
 * @brief 定时发送函数：车速信息 (0x714) - 10ms 周期
 */
void timer_func_10_spd() {
    struct a_pms_body_control_info_t t;
    // pms_vehicle_spd_flag: 1 表示有效，0 表示无效
    t.pms_vehicle_spd_flag = (vf_speed > 0.05) ? 1 : 0; 
    t.pms_vehicle_spd = a_pms_body_control_info_pms_vehicle_spd_encode(vf_speed);
    
    g_transmit_message.tx_sfid = 0x714;
    g_transmit_message.tx_dlen = 8;
    g_transmit_message.tx_efid = 0;
    g_transmit_message.fd_flag = 0;
    g_transmit_message.tx_ff = CAN_FF_STANDARD; // 标准 11 位 ID
    
    a_pms_body_control_info_pack(&g_transmit_message.tx_data[0], &t, 8);
    can_message_transmit(OUT_CAN, &g_transmit_message);
}

/**
 * @brief 定时发送函数：角速度信息 (0x713) - 10ms 周期
 */
void timer_func_10_yaw() {
    struct a_esc_vdc2_t t;
    a_esc_vdc2_init(&t);
    t.esc_yaw_rate = a_esc_vdc2_esc_yaw_rate_encode(g_yawrate);
    
    g_transmit_message.tx_sfid = 0x713;
    g_transmit_message.tx_dlen = 8;
    g_transmit_message.tx_efid = 0;
    g_transmit_message.fd_flag = 0;
    g_transmit_message.tx_ff = CAN_FF_STANDARD;
    
    a_esc_vdc2_pack(&g_transmit_message.tx_data[0], &t, 8);
    can_message_transmit(OUT_CAN, &g_transmit_message);
}

/**
 * @brief 定时发送函数：挡位信息 (0x715) - 100ms 周期
 */
void timer_func_100_gear() {
    struct a_pms_pt_info_indicate_t t;
    can_trasnmit_message_struct TxMessage;
    can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &TxMessage);
    
    TxMessage.tx_sfid = 0x715;
    TxMessage.tx_ff = CAN_FF_STANDARD;
    TxMessage.tx_ft = CAN_FT_DATA;
    TxMessage.tx_dlen = 8;
    TxMessage.fd_flag = CAN_FDF_CLASSIC;

    a_pms_pt_info_indicate_init(&t);
    // 挡位映射：0X7D-N, 0XDF-R, 0XFB-P, 0XFC-D
    t.pms_gear_position_ind = a_pms_pt_info_indicate_pms_gear_position_ind_encode(g_gear_val);
    t.pms_pt_ready_ind = 1; // 动力系统就绪标识
    
    a_pms_pt_info_indicate_pack(&TxMessage.tx_data[0], &t, 8);
    can_message_transmit(OUT_CAN, &TxMessage);
}

/**
 * @brief 定时发送函数：诊断报文 (0x700) - 2000ms 周期
 * 报文内容固定为 02 10 03 00 00 00 00 00
 */
void timer_func_2000_diag() {
    can_trasnmit_message_struct TxMessage;
    can_struct_para_init(CAN_TX_MESSAGE_STRUCT, &TxMessage);

    TxMessage.tx_sfid = 0x700;          // 诊断请求 ID
    TxMessage.tx_efid = 0;
    TxMessage.tx_ff   = CAN_FF_STANDARD; // 标准 11 位 ID
    TxMessage.tx_ft   = CAN_FT_DATA;     // 数据帧
    TxMessage.tx_dlen = 8;
    TxMessage.fd_flag = CAN_FDF_CLASSIC;

    // 固定报文内容：02 10 03 00 00 00 00 00
    TxMessage.tx_data[0] = 0x02;
    TxMessage.tx_data[1] = 0x10;
    TxMessage.tx_data[2] = 0x03;
    TxMessage.tx_data[3] = 0x00;
    TxMessage.tx_data[4] = 0x00;
    TxMessage.tx_data[5] = 0x00;
    TxMessage.tx_data[6] = 0x00;
    TxMessage.tx_data[7] = 0x00;

    can_message_transmit(OUT_CAN, &TxMessage);
}

/**
 * @brief 调试打印函数 - 1000ms 周期
 */
void timer_func_print_1000ms(){
    printf("Speed:%.2f km/h | Yaw:%.3f rad/s | GearFlag:%d | GearVal:0x%X\r\n", 
            vf_speed, g_yawrate, g_gear, g_gear_val);
}

// 定时任务结构体定义
typedef void (* timer_func)();
struct Timer_ {
    int ct;         // 当前计数值 (ms)
    int max_ct;     // 触发周期 (ms)
    timer_func func;// 触发的回调函数
} timers[] = {
    {0,  10,   timer_func_10_spd},
    {3,  10,   timer_func_10_yaw},
    {5,  100,  timer_func_100_gear},
    {7,  1000, timer_func_print_1000ms},
    {11, 2000, timer_func_2000_diag},   // 2000ms 发送诊断报文 0x700
    {-1, -1,   NULL} // 结束标识
};

/**
 * @brief 位域截取函数：从 64 位数据中截取指定起始和长度的位
 */
uint64_t bits_cutL(uint64_t *t, unsigned char sz, int start_bit, int bit_len)
{
    uint64_t x_l, x_h; 
    if (bit_len > 64) return 0;
    int n = start_bit >> 6; // 计算在哪一个 64 位块中
    uint64_t bit_mask = bit_len >= 64 ? 0xffffffffffffffffL : (1LL << bit_len) - 1;
    x_l = *(t + n);
    x_h = (n + 1) >= sz ? 0 : *(t + n + 1);
    start_bit -= n << 6;
    if (start_bit + bit_len > 64) {
        // 跨 64 位块截取
        x_l = (x_l >> start_bit) & ((1LL << (64 - start_bit)) - 1);
        x_l = (x_l | (x_h << (64 - start_bit))) & bit_mask;
    } else {
        x_l = (x_l >> start_bit) & bit_mask;
    }
    return x_l;
}

/**
 * @brief 核心解析函数：处理接收到的 CAN 消息
 */
void custom_parse_can_msg() 
{
    struct canmsg *c = NULL;
    uint32_t msgid;
    int i;
    
    // 轮询接收缓存队列
    for(i=0; i<2; i++){
        if(rcvmsgs[i]._update){
            c = &rcvmsgs[i];
            c->_update = 0; // 清除更新标志
            break;
        }
    }
    if(c == NULL) return;

    msgid = c->msgid;
    
    // 情况 1: 处理特定的测试 CAN ID 0x281
    if(msgid == 0x281){
        uint8_t data[8];
        for(i=0; i<8; i++) data[7-i] = c->data[i]; // 大端转换
        // 截取 33 位开始的长 12 位数据，应用偏移和缩放
        vf_speed = (float)(bits_cutL((uint64_t*)data, 2, 33, 12) * 0.05 + -30.0);
    }
    // 情况 2: 解析名爵 MG6 车速消息 (0x23c)
    else if(msgid == SCS_HSC2_FrP19){
        struct b_scs_hsc2_fr_p19_t t;
        b_scs_hsc2_fr_p19_unpack(&t, c->data, 8);
        vf_speed = b_scs_hsc2_fr_p19_veh_spd_avg_h2_hsc2_decode(t.veh_spd_avg_h2_hsc2);
    }
    // 情况 3: 解析名爵 MG6 横摆角速度 (0x1e9)
    else if(msgid == SCS_HSC2_FrP05){
        struct b_scs_hsc2_fr_p05_t t;
        b_scs_hsc2_fr_p05_unpack(&t, c->data, 8);
        g_yawrate = b_scs_hsc2_fr_p05_veh_dyn_yaw_rate_h2_hsc2_decode(t.veh_dyn_yaw_rate_h2_hsc2);
        g_yawrate = g_yawrate * 3.1415926 / 180.0; // 转换为弧度制
    }
    // 情况 4: 解析名爵 MG6 挡位 (0x194)
    else if(msgid == GW_HSC2_ECM_FrP04){
        struct b_gw_hsc2_ecm_fr_p04_t t;
        b_gw_hsc2_ecm_fr_p04_unpack(&t, c->data, 8);
        
        uint8_t gear_raw = t.tr_estd_gear_h2_hsc2; // 0x1-D, 0xE-R, 0xF-P, 0xD-N
        if(gear_raw == 0x1)      g_gear_val = 0xFC; // D
        else if(gear_raw == 0xE) g_gear_val = 0xDF; // R
        else if(gear_raw == 0xF) g_gear_val = 0xFB; // P
        else if(gear_raw == 0xD) g_gear_val = 0x7D; // N
        
        g_gear = (gear_raw == 0x0E) ? 1 : 0; // 设置倒车标志
    }
    // 情况 5: 解析 ME02 项目的 ESP2 消息 (0x154)
    else if(msgid == 0x154){
        struct a_esp2_t t;
        a_esp2_unpack(&t, c->data, 8);
        g_yawrate = a_esp2_yrs_decode(t.yrs);
    }
    // 情况 6: 解析 ME02 项目的 ABS1 消息 (0x200)
    else if(msgid == 0x200){
        struct a_abs1_t t;
        a_abs1_unpack(&t, c->data, 8);
        vf_speed = a_abs1_vehicle_speed_decode(t.vehicle_speed);
        // 根据速度自动判断静止状态
        if(vf_speed > 0.1){
            standstillsts = 0;
            wheelDirection = 1; // 默认前进
        } else {
            standstillsts = 1;
            wheelDirection = 0;
        }
    }
    // 情况 7: 解析 ME02 项目的 VCM3 挡位消息 (0x216)
    else if(msgid == 0x216){
        struct a_vcm3_t t;
        a_vcm3_unpack(&t, c->data, 8);
        int gear_raw = t.vcm_shift_lever_position;
        if(gear_raw == 6)      { g_gear_val = 0xDF; g_gear = 1; } // R
        else if(gear_raw == 5) { g_gear_val = 0x7D; g_gear = 0; } // N
        else if(gear_raw == 7) { g_gear_val = 0xFB; g_gear = 0; } // P
        else if(gear_raw == 4) { g_gear_val = 0xFC; g_gear = 0; } // D
    }
}

/**
 * @brief 定时器轮询处理 - 在 1ms 中断中调用
 */
void custom_timer() 
{
    int i;
    struct Timer_ *t;
    for(i=0; i < 100; i++){
        t = &timers[i];
        if(t->ct < 0) break; // 遍历到结束标识退出
        
        timers[i].ct++;
        if(timers[i].ct >= timers[i].max_ct){
            timers[i].ct = 0; // 计数值重置
            if(timers[i].func != NULL) {
                timers[i].func(); // 执行对应的周期任务
            }
        }
    }
}
