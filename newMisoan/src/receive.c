#include "ev3api.h"
#include "app.h"
#include "receive.h"
#include "stdlib.h"
#include <stdio.h>

// コマンド受信タスク
void receive_task(intptr_t unused)
{
    while (1) {
        if (bt != NULL && ev3_bluetooth_is_connected()) {
            char c = fgetc(bt);
            if (c != EOF) {
                command = c; // コマンドを共有変数に保存
            }
        }
        tslp_tsk(100); // 適切に待機
    }
}
