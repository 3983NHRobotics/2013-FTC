#pragma config(Hubs,  S1, HTMotor,  HTMotor,  HTServo,  HTMotor)
#pragma config(Sensor, S3,     HTSMUX,         sensorI2CCustom)
#pragma config(Sensor, S4,     US1,            sensorSONAR)
#pragma config(Motor,  motorA,          motorAutoScore, tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorB,           ,             tmotorNXT, openLoop)
#pragma config(Motor,  motorC,           ,             tmotorNXT, openLoop)
#pragma config(Motor,  mtr_S1_C1_1,     motorLeft,     tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C1_2,     motorRight,    tmotorTetrix, PIDControl, reversed, encoder)
#pragma config(Motor,  mtr_S1_C2_1,     motorArm,      tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C2_2,     motorG,        tmotorTetrix, PIDControl, encoder)
#pragma config(Motor,  mtr_S1_C4_1,     motorH,        tmotorTetrix, openLoop)
#pragma config(Motor,  mtr_S1_C4_2,     motorI,        tmotorTetrix, openLoop)
#pragma config(Servo,  srvo_S1_C3_1,    servo1,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_2,    servo2,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_3,    servo3,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_4,    servo4,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_5,    servo5,               tServoNone)
#pragma config(Servo,  srvo_S1_C3_6,    servo6,               tServoNone)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//####################################################################################//
//																																										//
//															2013-2014 Autonomous Code															//
//													  Team 3983 - Highlands Robotics													//
//												  Code by T. Kluge & Music by James H.											//
//																																										//
//####################################################################################//


//referenced files here -- DRIVER LOAD ORDER MATTERS

#include "joystickdriver.c"
#include "drivers/hitechnic-sensormux.h"
#include "drivers/hitechnic-irseeker-v2.h"
#include "drivers/hitechnic-gyro.h"

//#include "drivers/lego-ultrasound.h"

//variables + functions here

void moveForward(int mtime, int mpower);
void moveBackward(int mtime, int mpower);
void turnLeft(int mtime, int mpower);
void turnRight(int mtime, int mpower);
void scoreCube();
void killall();
void MissionImpossible();
void initializeRobot();
void displayText(int nLineNumber, const string cChar, int nValueDC, int nValueAC);
void displayText3(int nLineNumber, const string cChar, int nValueDC, int nValueAC, int nValueEnh);
void driveToEnd(int distanceToEdge);
bool searching = true;
bool koth = false;
bool goToEnd = true;
bool irOnLeft = false;
bool irOnRight = false;
bool findingRampTop = false;
int roundTime = 0;
int timeToEnd = 3500;
int kothAttackDistance = 50; //distance in cm (ultrasonic sensor)
int irFindVal = 150; //edit this to match the ultrasonic sensor read value for the correct distance
int sizeOfFile = 400;
int gyroLeanForwardMax = 50;
int gyroLeanForwardMin = 20;
int gyroLevelMax = 20;
int gyroLevelMin = -20;
string sTextLines[8];
string fileName = "IRvals.txt";
string irvalres = "";
float grotSpeed = 0;
float gheading = 0;

const tMUXSensor HTIRS2L = msensor_S3_1; //Left IRSeeker
const tMUXSensor HTIRS2R = msensor_S3_2; //Right IRSeeker
const tMUXSensor HTGYRO = msensor_S3_3; //Accel Sensor


task main()
{
	TFileHandle irFileHandle;
	TFileIOResult IOResult;
	HTGYROstartCal(HTGYRO);
	PlaySound(soundBlip);
	wait1Msec((2 * PI) * 1000); //TAUUUU
	PlaySound(soundFastUpwardTones);

	//_________________________________BLOCK TO GET SENSORVALUES FROM IRSEEKER_________________________
	//=================================================================================================
	int _dirDCL = 0;
	int _dirACL = 0;
	int dcS1L, dcS2L, dcS3L, dcS4L, dcS5L = 0;
	int acS1L, acS2L, acS3L, acS4L, acS5L = 0;
	int _dirEnhL, _strEnhL;
	int _dirDCR = 0;
	int _dirACR = 0;
	int dcS1R, dcS2R, dcS3R, dcS4R, dcS5R = 0;
	int acS1R, acS2R, acS3R, acS4R, acS5R = 0;
	int _dirEnhR, _strEnhR;

	// the default DSP mode is 1200 Hz.

	initializeRobot();
	waitForStart();
	ClearTimer(T1);
	OpenWrite(irFileHandle, IOResult, fileName, sizeOfFile);

	// FULLY DYNAMIC CODE W/ SCORING OF CUBE
	while(searching)
	{

		float irval = acS3L;
		StringFormat(irvalres, "%3.0f", irval);

		WriteText(irFileHandle, IOResult, "Test");
		WriteString(irFileHandle, IOResult, irvalres);
		WriteByte(irFileHandle, IOResult, 13);
		WriteByte(irFileHandle, IOResult, 10);


		_dirDCL = HTIRS2readDCDir(HTIRS2L);
		if (_dirDCL < 0)
			break; // I2C read error occurred

		_dirDCR = HTIRS2readDCDir(HTIRS2R);
		if (_dirDCR < 0)
			break; // I2C read error occurred
		// read the current modulated signal direction

		_dirACL = HTIRS2readACDir(HTIRS2L);
		if (_dirACL < 0)
			break; // I2C read error occurred
		_dirACR = HTIRS2readACDir(HTIRS2R);
		if (_dirACR < 0)
			break; // I2C read error occurred

		//===========LEFT SIDE
		// Read the individual signal strengths of the internal sensors
		// Do this for both unmodulated (DC) and modulated signals (AC)
		if (!HTIRS2readAllDCStrength(HTIRS2L, dcS1L, dcS2L, dcS3L, dcS4L, dcS5L))
			break; // I2C read error occurred
		if (!HTIRS2readAllACStrength(HTIRS2L, acS1L, acS2L, acS3L, acS4L, acS5L ))
			break; // I2C read error occurred
		//=============RIGHT SIDE
		if (!HTIRS2readAllDCStrength(HTIRS2R, dcS1R, dcS2R, dcS3R, dcS4R, dcS5R))
			break; // I2C read error occurred
		if (!HTIRS2readAllACStrength(HTIRS2R, acS1R, acS2R, acS3R, acS4R, acS5R ))
			break; // I2C read error occurred

		//=================Enhanced IR Values (Left and Right)===========
		// Read the Enhanced direction and strength
		if (!HTIRS2readEnhanced(HTIRS2L, _dirEnhL, _strEnhL))
			break; // I2C read error occurred
		if (!HTIRS2readEnhanced(HTIRS2R, _dirEnhR, _strEnhR))
			break; // I2C read error occurred

		//TEST THING FOR RECORDING US BEACON STRENGTH VALUES TO TEXT FILE FOR TESTING (DOES NOT ACTUALLY WORK)
		//^THAT BIT IS SOMEWHERE ELSE NOW

		//______________END SENSORVAL BLOCK________________________________________________________________
		//=================================================================================================

		if (acS3L < irFindVal && acS3R < irFindVal) { //While sensor is heading towards beacon: acs3 = side
			motor[motorLeft] = -80;
			motor[motorRight] = -80;

			if (time1[T1] > timeToEnd) {
				searching = false;
				koth = true;
				goToEnd = false;
			}
		}

		//===================================BLOCK FOR IR DETECTION=====================
		if (acS3L > irFindVal) { //if sensor is directly in front of beacon

			motor[motorLeft] = 0;
			motor[motorRight] = 0;
			irOnLeft = true;
			searching = false;
			koth = true;
			goToEnd = true;
		}

		if (acS3R > irFindVal) { //if sensor is directly in front of beacon

			motor[motorLeft] = 0;
			motor[motorRight] = 0;
			irOnRight = true;
			searching = false;
			koth = true;
			goToEnd = true;
		}

		//==================END IR DETECTION BLOCK========================

		wait1Msec(30);
	}//while searching

	Close(irFileHandle, IOResult);
	roundTime = time1[T1]; //probably unnecessary, is to cut out the time from the cube scorer
	scoreCube();

	if (goToEnd) {
		driveToEnd(timeToEnd - roundTime);//drive to end of ramp
	}

	wait1Msec(300);
	//turn left, forward, turn left, forward onto ramp
	if(irOnLeft) {
		turnLeft(1.1, 100);
	}
	if(irOnRight) {
		turnRight(1.1, 100);
	}
	wait1Msec(300);
	moveForward(1, 100);
	wait1Msec(300);
	if(irOnLeft) {
		turnLeft(1.1, 100);
	}
	if(irOnRight) {
		turnRight(1.1, 100);
	}
	wait1Msec(300);
  findingRampTop = true;
  while(findingRampTop) {//go forwards until level robot is detected
  	grotSpeed = HTGYROreadRot(HTGYRO);//find gyro rotation speed
		gheading += grotSpeed * 0.02;//find gyro heading in degrees??
		motor[motorLeft] = -100;
		motor[motorRight] = -100;
		if (gheading < gyroLevelMax && gheading > gyroLevelMin) {
			motor[motorLeft] = 0;
			motor[motorRight] = 0;
			findingRampTop = false;
		}//end if
		wait1Msec(10);
	}//end while findingramptop

	//Begin KotH routine

	while (koth) {

		grotSpeed = HTGYROreadRot(HTGYRO);//find gyro rotation speed
		gheading += grotSpeed * 0.02;//find gyro heading in degrees??

		//Loop of koth begins here
		while(gheading < gyroLevelMax && gheading > gyroLevelMin) {//while robot is on top of hill
			if (SensorValue[US1] < kothAttackDistance && nPgmTime < 27000) { //if something is in range AND program time is less than 27 seconds
				PlaySound(soundFastUpwardTones);
				moveForward(0.5, 100);
				while(gheading < gyroLeanForwardMax && gheading > gyroLeanForwardMin) {//while robot is tilted, move forwards
					motor[motorLeft] = -100;
					motor[motorRight] = -100;
					if(gheading < gyroLevelMax && gheading > gyroLevelMin) {//if motor is flat, stop motors
						motor[motorLeft] = 0;
						motor[motorRight] = 0;
					}//end of if robot is flat
					break;
				}//end of while robot is tilted downhll
			}//end of if us detects a thing
		}//end of while robot is on top of hill

		while(gheading < gyroLevelMax && gheading > gyroLevelMin) {//while robot is at bottom of hill, move robot backwards
			motor[motorLeft] = 100;
			motor[motorRight] = 100;
			if(gheading < gyroLeanForwardMax && gheading > gyroLeanForwardMin) {//if robot is tilted, continue with next section of code
				break;
			}
		}
		while(gheading < gyroLeanForwardMax && gheading > gyroLeanForwardMin) {//if robot is tilted uphillm move backwards
			motor[motorLeft] = 100;
			motor[motorRight] = 100;
			if (gheading < gyroLevelMax && gheading > gyroLevelMin) {//if robot is flat, stop motors and loop
				motor[motorLeft] = 0;
				motor[motorRight] = 0;
				break;
			}
		}//robot should be on top of hill again. Loop ends here.

		if (nPgmTime > 29000) {
			koth = false;
		}
		wait1Msec(10);
	}//while koth
	PlaySound(soundDownwardTones);

}//task main

//Define functions here instead of at the top
void driveToEnd(int distanceToEdge) {
	motor[motorLeft] = -80;
	motor[motorRight] = -80;
	wait1Msec(distanceToEdge);
	motor[motorLeft] = 0;
	motor[motorRight] = 0;
}

void moveForward(int mtime, int mpower) {

	motor[motorLeft] = -1 *mpower;
	motor[motorRight] = -1 * mpower;
	wait1Msec(mtime * 1000);
	motor[motorLeft] = 0;
	motor[motorRight] = 0;

}

void moveBackward(int mtime, int mpower) {

	motor[motorLeft] = mpower;
	motor[motorRight] = mpower;
	wait1Msec(mtime * 1000);
	motor[motorLeft] = 0;
	motor[motorRight] = 0;

}

void turnLeft(int mtime, int mpower) {

	motor[motorLeft] = mpower;
	motor[motorRight] = -1 * mpower;
	wait1Msec(mtime * 1000);
	motor[motorLeft] = 0;
	motor[motorRight] = 0;

}

void turnRight(int mtime, int mpower) {

	motor[motorLeft] = -1 * mpower;
	motor[motorRight] = mpower;
	wait1Msec(mtime * 1000);
	motor[motorLeft] = 0;
	motor[motorRight] = 0;

}

void armLift(int mtime, int mpower) {

	motor[motorArm] = mpower;
	//motor[motorArm2] = mpower;
	wait1Msec(mtime * 1000);
	motor[motorArm] = 0;
	//motor[motorArm2] = 0;

}

void scoreCube() {
	motor[motorAutoScore] = 40;
	wait1Msec(500);
	motor[motorAutoScore] = 0;

	motor[motorAutoScore] = -40;
	wait1Msec(500);
	motor[motorAutoScore] = 0;
}

void killall() {
	motor[motorLeft] = 0;
	motor[motorRight] = 0;
	motor[motorArm] = 0;
	PlaySound(soundException);
	PlaySound(soundDownwardTones);
}

void MissionImpossible()
{
	//        100 = Tempo
	//          6 = Default octave
	//    Quarter = Default note length
	//        10% = Break between notes
	//
	PlayTone(  880,    7); wait1Msec(  75);  // Note(D, Duration(32th))
	PlayTone(  933,    7); wait1Msec(  75);  // Note(D#, Duration(32th))
	PlayTone(  880,    7); wait1Msec(  75);  // Note(D, Duration(32th))
	PlayTone(  933,    7); wait1Msec(  75);  // Note(D#, Duration(32th))
	PlayTone(  880,    7); wait1Msec(  75);  // Note(D, Duration(32th))
	PlayTone(  933,    7); wait1Msec(  75);  // Note(D#, Duration(32th))
	PlayTone(  880,    7); wait1Msec(  75);  // Note(D, Duration(32th))
	PlayTone(  933,    7); wait1Msec(  75);  // Note(D#, Duration(32th))
	PlayTone(  880,    7); wait1Msec(  75);  // Note(D, Duration(32th))
	PlayTone(  880,    7); wait1Msec(  75);  // Note(D, Duration(32th))
	PlayTone(  933,    7); wait1Msec(  75);  // Note(D#, Duration(32th))
	PlayTone(  988,    7); wait1Msec(  75);  // Note(E, Duration(32th))
	PlayTone( 1047,    7); wait1Msec(  75);  // Note(F, Duration(32th))
	PlayTone( 1109,    7); wait1Msec(  75);  // Note(F#, Duration(32th))
	PlayTone( 1175,    7); wait1Msec(  75);  // Note(G, Duration(32th))
	PlayTone( 1175,   14); wait1Msec( 150);  // Note(G, Duration(16th))
	PlayTone(    0,   27); wait1Msec( 300);  // Note(Rest, Duration(Eighth))
	PlayTone( 1175,   14); wait1Msec( 150);  // Note(G, Duration(16th))
	PlayTone(    0,   27); wait1Msec( 300);  // Note(Rest, Duration(Eighth))
	PlayTone( 1398,   14); wait1Msec( 150);  // Note(A#, Duration(16th))
	PlayTone(    0,   14); wait1Msec( 150);  // Note(Rest, Duration(16th))
	PlayTone(  784,   14); wait1Msec( 150);  // Note(C, Duration(16th))
	PlayTone(    0,   14); wait1Msec( 150);  // Note(Rest, Duration(16th))
	PlayTone( 1175,   14); wait1Msec( 150);  // Note(G, Duration(16th))
	PlayTone(    0,   27); wait1Msec( 300);  // Note(Rest, Duration(Eighth))
	PlayTone( 1175,   14); wait1Msec( 150);  // Note(G, Duration(16th))
	PlayTone(    0,   27); wait1Msec( 300);  // Note(Rest, Duration(Eighth))
	PlayTone( 1047,   14); wait1Msec( 150);  // Note(F, Duration(16th))
	PlayTone(    0,   14); wait1Msec( 150);  // Note(Rest, Duration(16th))
	PlayTone( 1109,   14); wait1Msec( 150);  // Note(F#, Duration(16th))
	PlayTone(    0,   14); wait1Msec( 150);  // Note(Rest, Duration(16th))
	PlayTone( 1175,   14); wait1Msec( 150);  // Note(G, Duration(16th))
	PlayTone(    0,   27); wait1Msec( 300);  // Note(Rest, Duration(Eighth))
	PlayTone( 1175,   14); wait1Msec( 150);  // Note(G, Duration(16th))
	PlayTone(    0,   27); wait1Msec( 300);  // Note(Rest, Duration(Eighth))
	PlayTone( 1398,   14); wait1Msec( 150);  // Note(A#, Duration(16th))
	PlayTone(    0,   14); wait1Msec( 150);  // Note(Rest, Duration(16th))
	PlayTone(  784,   14); wait1Msec( 150);  // Note(C, Duration(16th))
	PlayTone(    0,   14); wait1Msec( 150);  // Note(Rest, Duration(16th))
	PlayTone( 1175,   14); wait1Msec( 150);  // Note(G, Duration(16th))
	PlayTone(    0,   27); wait1Msec( 300);  // Note(Rest, Duration(Eighth))
	PlayTone( 1175,   14); wait1Msec( 150);  // Note(G, Duration(16th))
	PlayTone(    0,   27); wait1Msec( 300);  // Note(Rest, Duration(Eighth))
	PlayTone( 1047,   14); wait1Msec( 150);  // Note(F, Duration(16th))
	PlayTone(    0,   14); wait1Msec( 150);  // Note(Rest, Duration(16th))
	PlayTone( 1109,   14); wait1Msec( 150);  // Note(F#, Duration(16th))
	PlayTone(    0,   14); wait1Msec( 150);  // Note(Rest, Duration(16th))
	PlayTone( 1398,   14); wait1Msec( 150);  // Note(A#, Duration(16th))
	PlayTone( 1175,   14); wait1Msec( 150);  // Note(G, Duration(16th))
	PlayTone(  880,  108); wait1Msec(1200);  // Note(D, Duration(Half))
	PlayTone(    0,    7); wait1Msec(  75);  // Note(Rest, Duration(32th))
	PlayTone( 1398,   14); wait1Msec( 150);  // Note(A#, Duration(16th))
	PlayTone( 1175,   14); wait1Msec( 150);  // Note(G, Duration(16th))
	PlayTone(  831,  108); wait1Msec(1200);  // Note(C#, Duration(Half))
	PlayTone(    0,    7); wait1Msec(  75);  // Note(Rest, Duration(32th))
	PlayTone( 1398,   14); wait1Msec( 150);  // Note(A#, Duration(16th))
	PlayTone( 1175,   14); wait1Msec( 150);  // Note(G, Duration(16th))
	PlayTone(  784,  108); wait1Msec(1200);  // Note(C, Duration(Half))
	PlayTone(    0,   14); wait1Msec( 150);  // Note(Rest, Duration(16th))
	PlayTone(  932,   14); wait1Msec( 150);  // Note(A#5, Duration(16th))
	PlayTone(  784,   14); wait1Msec( 150);  // Note(C, Duration(16th))
	return;
}

void initializeRobot() {
	servo[srvo_S1_C3_1] = 165;
	return;

}

void displayText(int nLineNumber, const string cChar, int nValueDC, int nValueAC)
{
	string sTemp;

	StringFormat(sTemp, "%4d %4d", nValueDC, nValueAC);

	// Check if the new line is the same as the previous one
	// Only update screen if it's different.
	if (sTemp != sTextLines[nLineNumber])
	{
		string sTemp2;

		sTextLines[nLineNumber] = sTemp;
		StringFormat(sTemp2, "%s:%s", cChar, sTemp);
		nxtDisplayTextLine(nLineNumber, sTemp2);
	}
}

void displayText3(int nLineNumber, const string cChar, int nValueDC, int nValueAC, int nValueEnh)
{
	string sTemp;

	StringFormat(sTemp, "%4d %4d %3d", nValueDC, nValueAC, nValueEnh);

	// Check if the new line is the same as the previous one
	// Only update screen if it's different.
	if (sTemp != sTextLines[nLineNumber])
	{
		string sTemp2;

		sTextLines[nLineNumber] = sTemp;
		StringFormat(sTemp2, "%s:%s", cChar, sTemp);
		nxtDisplayTextLine(nLineNumber, sTemp2);
	}
}