#pragma config(Hubs,  S2, HTMotor,  HTMotor,  HTServo,  HTMotor)
#pragma config(Sensor, S1,     HTIRS2L,        sensorI2CCustom)
#pragma config(Sensor, S2,     ,               sensorI2CMuxController)
#pragma config(Sensor, S3,     HTSMUX,         sensorI2CCustom)
#pragma config(Sensor, S4,     US1,            sensorSONAR)
#pragma config(Motor,  motorA,          motorAutoScore, tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorB,           ,             tmotorNXT, openLoop)
#pragma config(Motor,  motorC,           ,             tmotorNXT, openLoop)
#pragma config(Motor,  mtr_S2_C1_1,     motorLeft,     tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S2_C1_2,     motorWinch,    tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S2_C2_1,     motorScoop,    tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S2_C2_2,     motorArm,      tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S2_C4_1,     motorRight,    tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S2_C4_2,     motorLift,     tmotorTetrix, PIDControl, encoder)
#pragma config(Servo,  srvo_S2_C3_1,    servo1,               tServoStandard)
#pragma config(Servo,  srvo_S2_C3_2,    servoUSSeeker,        tServoStandard)
#pragma config(Servo,  srvo_S2_C3_3,    servoLift,            tServoStandard)
#pragma config(Servo,  srvo_S2_C3_4,    servoClamp,           tServoStandard)
#pragma config(Servo,  srvo_S2_C3_5,    servoHook,            tServoStandard)
#pragma config(Servo,  srvo_S2_C3_6,    servoSweep,           tServoStandard)
#include "joystickdriver.c"
task main()
{
	while(true) {
		bFloatDuringInactiveMotorPWM = false;
		getJoystickSettings(joystick);
		//Anti-RageQuit code (prevents movement when the joysticks are just a bit off)

		//CONTROLLER 1 JOYSTICK 1 - Drive (Forward/Backward)
		if((joystick.joy1_y1<10&&joystick.joy1_y1>-10)&&(joystick.joy1_x1<10&&joystick.joy1_x1>-10))
		{
			joystick.joy1_y1=0;
			joystick.joy1_x1=0;
		}
		//CONTROLLER 1 JOYSTICK 2 - Drive (Turning)
		if((joystick.joy1_x2<10&&joystick.joy1_x2>-10)&&(joystick.joy1_y2<10&&joystick.joy1_y2>-10))
		{
			joystick.joy1_x2=0;
			joystick.joy1_y2=0;
		}
		motor[motorArm] = joystick.joy1_y1;
		motor[motorWinch] = joystick.joy1_y2;
	}
}
