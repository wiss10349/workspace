#include "ev3api.h"
#include "app.h"
#include "receive.h"
#include "stdlib.h"
#include <stdio.h>

// コマンド実行タスク
void execute_task(intptr_t unused)
{
    while (1) {
        switch (command) {
        case 'w':
            fprintf(bt, "Move forward.\n");
            ev3_motor_set_power(L_motor, -40);
            ev3_motor_set_power(R_motor, -40);
            break;

        case 's':
            fprintf(bt, "Move backward.\n");
            ev3_motor_set_power(L_motor, 40);
            ev3_motor_set_power(R_motor, 40);
            break;

        case 'a':
            fprintf(bt, "Turn left.\n");
            ev3_motor_set_power(L_motor, 40);
            ev3_motor_set_power(R_motor, -40);
            break;

        case 'd':
            fprintf(bt, "Turn right.\n");
            ev3_motor_set_power(L_motor, -40);
            ev3_motor_set_power(R_motor, 40);
            break;

        case 'f': {
            fprintf(bt, "Neck = %d.\n", head);
            SYSTIM start_time, now_time;
            get_tim(&start_time);
            get_tim(&now_time);
            if (head == 0) {
                while (1000 > (now_time - start_time)) {
                    get_tim(&now_time);
                    ev3_motor_set_power(M_motor, 20);
                }
                ev3_motor_stop(M_motor, true);
                head = 1;
            } else {
                while (1000 > (now_time - start_time)) {
                    get_tim(&now_time);
                    ev3_motor_set_power(M_motor, -20);
                }
                ev3_motor_stop(M_motor, true);
                head = 0;
            }
            break;
        }

        default:
            ev3_motor_stop(L_motor, false);
            ev3_motor_stop(R_motor, false);
            ev3_motor_stop(M_motor, false);
            break;
        }

        tslp_tsk(100);  // 適切に待機
    }
}
