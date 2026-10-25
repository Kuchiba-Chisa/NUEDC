#ifndef __MOTO_H
#define	__MOTO_H

#include "main.h"

#define   pwmA_IN1  TIM1->CCR1
#define   pwmA_IN2  TIM1->CCR2
#define   pwmB_IN1  TIM1->CCR3
#define   pwmB_IN2  TIM1->CCR4

void moto(int mode);
int Velocity_A(int TargetVelocity, int CurrentVelocity);
int Velocity_B(int TargetVelocity, int CurrentVelocity);

#endif
