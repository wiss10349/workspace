#include "ev3api.h"
#include "app.h"
#include "stdlib.h"
#include <stdio.h>

#if defined(BUILD_MODULE)
#include "module_cfg.h"
#else
#include "kernel_cfg.h"
#endif

#define DEBUG

#ifdef DEBUG
#define _debug(x) (x)
#else
#define _debug(x)
#endif
// センサーポート定義
static const sensor_port_t
    ultrasonic_sensor = EV3_PORT_3; // 頭部超音波センサー
// モーターポート定義
static const motor_port_t
    L_motor      = EV3_PORT_B, // 左足ラージモーター(Left)
    R_motor      = EV3_PORT_D, // 右足ラージモーター(Right)
    M_motor      = EV3_PORT_C; // 首ミディアムモーター(Mid)

static int distance; // 距離用グローバル変数
static FILE *bt = NULL; // Bluetoothファイルポインタの宣言

void main_task(intptr_t unused) {
    // センサーとセンサーポートを接続する
    ev3_sensor_config (ultrasonic_sensor, ULTRASONIC_SENSOR);
    // モーターとモーターポートを接続する
    ev3_motor_config (L_motor, LARGE_MOTOR);
    ev3_motor_config (R_motor, LARGE_MOTOR);
    ev3_motor_config (M_motor, MEDIUM_MOTOR);
    // LEDの色を緑に設定(亀なので)
    ev3_led_set_color(LED_GREEN);
    // 文字サイズをMIDに
    ev3_lcd_set_font ( EV3_FONT_MEDIUM );
    // Bluetoothファイルをオープン
    bt = ev3_serial_open_file(EV3_SERIAL_BT);
    assert(bt != NULL);
    ev3_sta_cyc(DIS_CYCHDR);
    //act_tsk(IDLE_TASK);
    static int head = 0;

    while(1)
    {
        // Bluetoothの接続を確認するまで待機
        while (!ev3_bluetooth_is_connected()){
            ev3_lcd_draw_string("Waiting for Bluetooth",0,0);
            tslp_tsk(100);
        }
        char c = fgetc(bt);
        //sus_tsk(IDLE_TASK);
        ev3_lcd_draw_string("Misoan is alive!!!",0,0);
        switch(c){
        case 'w':
            fprintf(bt, "Move forward.\n");
            ev3_lcd_draw_string("WWW",0,60);
            ev3_motor_set_power(L_motor,-40);
            ev3_motor_set_power(R_motor,-40);
            break;

        case 's':
            fprintf(bt, "Move backward.\n");
            ev3_lcd_draw_string("SSS",0,60);
            ev3_motor_set_power(L_motor,40);
            ev3_motor_set_power(R_motor,40);
            break;

        case 'a':
            fprintf(bt, "Turn left.\n");
            ev3_lcd_draw_string("AAA",0,60);
            ev3_motor_set_power(L_motor,40);
            ev3_motor_set_power(R_motor,-40);
            break;

        case 'd':
            fprintf(bt, "Turn right.\n");
            ev3_lcd_draw_string("DDD",0,60);
            ev3_motor_set_power(L_motor,-40);
            ev3_motor_set_power(R_motor,40);
            break;

        case 'f':
            fprintf(bt, "Neck = %d.\n", head);
            ev3_lcd_draw_string("FFF",0,60);
            SYSTIM start_time;
            SYSTIM now_time;
            get_tim(&start_time);
            get_tim(&now_time);
            if(head == 0){
                while (1000 > (now_time - start_time)) {
                    get_tim(&now_time);
                    ev3_motor_set_power(M_motor,20);
                }
                ev3_motor_stop(M_motor, true);
                head = 1;
                fprintf(bt, "Neck = %d.\n", head);
                break;
            }else if(head == 1){
               while (1000 > (now_time - start_time)) {
                    get_tim(&now_time);
                    ev3_motor_set_power(M_motor,-20);
                }
                ev3_motor_stop(M_motor, true);
                head = 0;
                fprintf(bt, "Neck = %d.\n", head);
                break;
            }
            // ev3_motor_reset_counts(M_motor);
            // if(head = 0){
            //     while (60 > ev3_motor_get_counts(M_motor)) {
            //         ev3_motor_set_power(M_motor,20);
            //     }
            //     ev3_motor_stop(M_motor, true);
            //     head = 1;
            // }else if(head = 1){
            //    while (60 >- ev3_motor_get_counts(M_motor)) {
            //         ev3_motor_set_power(M_motor,-20);
            //     }
            //     ev3_motor_stop(M_motor, true);
            //     head = 0;
            // }
            break;

        default:
            ev3_motor_stop(L_motor, false);
            ev3_motor_stop(R_motor, false);
            ev3_motor_stop(M_motor, false);
        }
        tslp_tsk(100);
        // 障害物検知
        // if((distance < 10) && (c != 'q')){
        //     ev3_lcd_draw_string("Misoan is scared!!",0,0);
        //     //fprintf(bt, "Misoan in scared.\n");
        //     ev3_motor_stop(L_motor, false);
        //     ev3_motor_stop(R_motor, false);
        //     tslp_tsk(3000);
        // }
        // if(c == 'q'){
        //     break;
        // }
    }
    ev3_stp_cyc(DIS_CYCHDR);
    ext_tsk();
}

// void idle_task(intptr_t unused) {
//     while(1) {
//     	fprintf(bt, "Press 'h' for usage instructions.\n");
//     	tslp_tsk(1000);
//     }
// }

void dis_cychdr(intptr_t unused)
{
    char distance_buffer[30];
    // 反射値を測って変数に格納
    distance = ev3_ultrasonic_sensor_get_distance(ultrasonic_sensor);
    // 文字配列に読み取った値を格納する
    sprintf(distance_buffer, "DISTANCE:%d   ",distance);
    ev3_lcd_draw_string(distance_buffer, 0,100);
}
