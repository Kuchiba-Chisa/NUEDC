#ifndef __PWM_H
#define	__PWM_H

#include "main.h"
#define   PWMA_IN1  TIM1->CCR1
#define   PWMA_IN2  TIM1->CCR2
#define   PWMB_IN1  TIM1->CCR3
#define   PWMB_IN2  TIM1->CCR4


void Set_PWM(int motor_left,int motor_right);

#endif

