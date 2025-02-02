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
const sensor_port_t
    ultrasonic_sensor = EV3_PORT_3; // 頭部超音波センサー

// モーターポート定義
const motor_port_t
    L_motor = EV3_PORT_B, // 左足ラージモーター(Left)
    R_motor = EV3_PORT_D, // 右足ラージモーター(Right)
    M_motor = EV3_PORT_C; // 首ミディアムモーター(Mid)

// 変数
//int distance;         // 距離用グローバル変数
FILE *bt = NULL;      // Bluetoothファイルポインタの宣言
char command = '\0'; // 受信したコマンド
int head = 0;         // 首の状態

// メインタスク
void main_task(intptr_t unused)
{
    // センサーとセンサーポートを接続する
    ev3_sensor_config(ultrasonic_sensor, ULTRASONIC_SENSOR);
    ev3_motor_config(L_motor, LARGE_MOTOR);
    ev3_motor_config(R_motor, LARGE_MOTOR);
    ev3_motor_config(M_motor, MEDIUM_MOTOR);

    // LEDの色を緑に設定
    ev3_led_set_color(LED_GREEN);

    // Bluetoothファイルをオープン
    bt = ev3_serial_open_file(EV3_SERIAL_BT);
    assert(bt != NULL);

    // Bluetooth接続待機
    while (!ev3_bluetooth_is_connected()) {
        ev3_lcd_draw_string("Waiting for Bluetooth", 0, 0);
        tslp_tsk(100);
    }

    ev3_lcd_draw_string("Bluetooth Connected", 0, 0);

    // コマンド受信タスクと実行タスクを起動
    act_tsk(RECEIVE_TASK);
    act_tsk(EXECUTE_TASK);

    // メインタスクは終了
    ext_tsk();
}
