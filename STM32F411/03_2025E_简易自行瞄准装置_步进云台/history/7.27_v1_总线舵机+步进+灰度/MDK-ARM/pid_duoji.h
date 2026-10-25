#ifndef __PID_DUOJI_H
#define __PID_DUOJI_H

#define PATH_UPDATE_INTERVAL   50   // 路径点更新间隔(ms)
#define POINTS_PER_EDGE        50   // 每条边的点数（含端点）
#define TOTAL_PATH_POINTS      (4 * (POINTS_PER_EDGE - 1))  // 总路径点数 = 36

float constrain_float(float value, float min_val, float max_val);
float compute_pid_x(int target, int current);
float compute_pid_y(int target, int current);
void set_target_position(int x, int y);
void control_loop(void);
void pid_init(void) ;
void set_pid_params(float kp_x, float ki_x, float kd_x, float kp_y, float ki_y, float kd_y);
void init_path_points();
void update_path_in_timer_irq();
#endif // !__PID_DUOJI_H
