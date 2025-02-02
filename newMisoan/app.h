#include "target_test.h"

/*
 *  各タスクの優先度の定義
 */

#define MAIN_PRIORITY	5		/* メインタスクの優先度 */
								/* HIGH_PRIORITYより高くすること */

#define HIGH_PRIORITY	9		/* 並行実行されるタスクの優先度 */
#define MID_PRIORITY	10
#define LOW_PRIORITY	11

/*
 *  ターゲットに依存する可能性のある定数の定義
 */

#ifndef STACK_SIZE
#define	STACK_SIZE		4096		/* タスクのスタックサイズ */
#endif /* STACK_SIZE */

#ifndef LOOP_REF
#define LOOP_REF		ULONG_C(1000000)	/* 速度計測用のループ回数 */
#endif /* LOOP_REF */

// センサーポート定義
extern const sensor_port_t ultrasonic_sensor;

// モーターポート定義
extern const motor_port_t L_motor, R_motor, M_motor;

//extern int distance;	// 距離用グローバル変数
extern FILE *bt;		// Bluetoothファイルポインタの宣言
extern char command;	// 受信したコマンド
extern int head;		// 首の状態

/*
 *  関数のプロトタイプ宣言
 */
#ifndef TOPPERS_MACRO_ONLY

void	main_task(intptr_t exinf);
void	receive_task(intptr_t exinf);
void	execute_task(intptr_t exinf);
//void idle_task(intptr_t exinf); // アイドル時タスク
//void dis_cychdr(intptr_t exinf); // 距離計測サイクリックハンドラ

#endif /* TOPPERS_MACRO_ONLY */
