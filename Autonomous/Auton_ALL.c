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
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//###################################################################################//
//
//								2013-2014 Autonomous Code
//							 Team 3983 - Highlands Robotics
//						  Code by T. Kluge & Music by J. Harnett
//
//###################################################################################//

//referenced files here -- DRIVER LOAD ORDER MATTERS

#include "../drivers/JoystickDriver_CustomNoSplash.c"
#include "../PixelArt/Arrows.c"
#include "MenuSelector.c"
#include "BatterySelector.c"
//#include "MenusAndIcons.c"
#include "../drivers/hitechnic-sensormux.h"
#include "../drivers/hitechnic-irseeker-v2.h"
#include "../drivers/hitechnic-gyro.h"
#include "../drivers/hitechnic-accelerometer.h"
#include "../drivers/lego-light.h"
#include "../music/James_Music.c"
#include "../GlobalFunctions/GlobalFunctions.c"
#include "../AUTONDUM.c"
//#include "Auton_ORIGINAL.c"

//variables + functions here

void scoreCube();
void killall();
void initializeRobot();
void displayText(int nLineNumber, const string cChar, int nValueDC, int nValueAC);
void displayText3(int nLineNumber, const string cChar, int nValueDC, int nValueAC, int nValueEnh);
void driveToEnd(int motorPower, int distanceToEdge);
void pushOffRamp();
void turnTowardsRobot();
void turnTowardsCenter();

bool searching = true;
bool searchingForBot = false;
bool koth = false;
bool goToEnd = true;
bool turnedLeft = false;
bool turnedRight = false;
bool FORWARD_SCORE_FORWARD_LINE_1 = false;
bool FORWARD_SCORE_FORWARD_LINE_2 = false;
bool FORWARD_SCORE_BACKWARD_LINE_1 = false;
bool FORWARD_SCORE_BACKWARD_LINE_2 = false;
bool canFindWhite = true;
bool canFindBlack = false;

int roundTime = 0;
int timeToEnd = 4300;
int kothAttackDistance = 50; //distance in cm (ultrasonic sensor)
int irFindVal = 150; //edit this to match the ultrasonic sensor read value for the correct distance
int sizeOfFile = 400;
int accelTurnMax = 20;
int accelTurnMin = -20;
int USScanVal;
int degFirstTurn = 88; //87//if you do anything to this, do the opposite to the other (46)
int degFirstTurnForRev = 86; //87
int degSecondTurn = 48; //47//same for this one - should add up to 46 (0)
int degSecondTurnForRev = 120;
int batteryOffsetValue = 0; //make this smaller to make the bot turn more to the left - negative numbers are ok, according to maths
int degToUse;
int linesToFind = 0;
int linesFound = 0;
int WHITE_LINE_VALUE = 280;
int BLACK_MAT_VALUE = 200; //pointless

float rotSpeed = 0;
float heading = 92;

string sTextLines[8];
string fileName = "IRvals.txt";
string irvalres = "";


const tMUXSensor LEGOLS = msensor_S3_1; //Light sensor
//const tMUXSensor HTACCEL = msensor_S3_2; //Accel Sensor
const tMUXSensor HTGYRO = msensor_S3_3; //Gyro Sensor

//=============================================================================================================================================
//---------------------------------------------------BEGIN INITIALIZATION CODE-----------------------------------------------------------------
task main() {

	//Initialize the display with the program choices
	LSsetInactive(LEGOLS);
	chooseProgram();
	//setBatPower();

	switch (PROGID) {
	case 1:
		FORWARD_SCORE_FORWARD_LINE_1 = true;
		linesToFind = 1;
		break;
	case 2:
		FORWARD_SCORE_FORWARD_LINE_2 = true;
		linesToFind = 2;
		break;
	case 3:
		FORWARD_SCORE_BACKWARD_LINE_1 = true;
		linesToFind = 1;
		break;
	case 4:
		FORWARD_SCORE_BACKWARD_LINE_2 = true;
		linesToFind = 2;
		break;
	case 6:
		useDummyAutonomous();
		break;
	case 5:
		//useOriginalAutonomous();
		PlaySoundFile("Woops.rso");
		break;
	}

	if ((externalBatteryAvg / (float) 1000) >= 13) {
		degFirstTurn = 70 + batteryOffsetValue; //87//if you do anything to this, do the opposite to the other (46)
		degFirstTurnForRev = 87 + batteryOffsetValue; //87
		degSecondTurn = 30 + batteryOffsetValue; //47//same for this one - should add up to 46 (0)
		degSecondTurnForRev = 115 + batteryOffsetValue;
		
		} else if ((externalBatteryAvg / (float) 1000) >= 12 && (externalBatteryAvg / (float) 1000) < 13) {
		degFirstTurn = 71 + batteryOffsetValue; //87//if you do anything to this, do the opposite to the other (46)
		degFirstTurnForRev = 86 + batteryOffsetValue; //87
		degSecondTurn = 36 + batteryOffsetValue; //47//same for this one - should add up to 46 (0)
		degSecondTurnForRev = 120 + batteryOffsetValue;
		
		} else if ((externalBatteryAvg / (float) 1000) >= 11 && (externalBatteryAvg / (float) 1000) < 12) {
		degFirstTurn = 70 + batteryOffsetValue; //87//if you do anything to this, do the opposite to the other (46)
		degFirstTurnForRev = 85 + batteryOffsetValue; //87
		degSecondTurn = 37 + batteryOffsetValue; //47//same for this one - should add up to 46 (0)
		degSecondTurnForRev = 120 + batteryOffsetValue;
		
		} else if ((externalBatteryAvg / (float) 1000) >= 10 && (externalBatteryAvg / (float) 1000) < 11) {
		degFirstTurn = 86 + batteryOffsetValue; //87//if you do anything to this, do the opposite to the other (46)
		degFirstTurnForRev = 84 + batteryOffsetValue; //87
		degSecondTurn = 46 + batteryOffsetValue; //47//same for this one - should add up to 46 (0)
		degSecondTurnForRev = 120 + batteryOffsetValue;

		} else if ((externalBatteryAvg / (float) 1000) >= 9 && (externalBatteryAvg / (float) 1000) < 10) {
		degFirstTurn = 85 + batteryOffsetValue; //87//if you do anything to this, do the opposite to the other (46)
		degFirstTurnForRev = 83 + batteryOffsetValue; //87
		degSecondTurn = 45 + batteryOffsetValue; //47//same for this one - should add up to 46 (0)
		degSecondTurnForRev = 120 + batteryOffsetValue;

		} else {
		//keep default values
	}

//---------------------------------------------------------END INITIALIZATION CODE-------------------------------------------------------------
//=============================================================================================================================================

TFileHandle irFileHandle;
TFileIOResult IOResult;
//PlaySound(soundBlip);
//wait1Msec((2 * PI) * 1000); //TAUUUU
//wait1Msec(10000);//wait 10 seconds for other teams who **might** have better autonomous code
//PlaySound(soundFastUpwardTones);

//_________________________________BLOCK TO GET SENSORVALUES FROM IRSEEKER_________________________
//=================================================================================================
int _dirDCL = 0;
int _dirACL = 0;
int dcS1L, dcS2L, dcS3L, dcS4L, dcS5L = 0;
int acS1L, acS2L, acS3L, acS4L, acS5L = 0;
int _dirEnhL, _strEnhL;

// the default DSP mode is 1200 Hz.

initializeRobot();
HTGYROstartCal(HTGYRO);
servo[servoLift] = 123;
servo[servoSweep] = 199;
waitForStart();
LSsetActive(LEGOLS);
ClearTimer(T1);
OpenWrite(irFileHandle, IOResult, fileName, sizeOfFile);

// FULLY DYNAMIC CODE W/ SCORING OF CUBE
while(searching)
{

	_dirDCL = HTIRS2readDCDir(HTIRS2L);
	if (_dirDCL < 0)
		break; // I2C read error occurred

	_dirACL = HTIRS2readACDir(HTIRS2L);
	if (_dirACL < 0)
		break; // I2C read error occurred

	//===========LEFT SIDE
	// Read the individual signal strengths of the internal sensors
	// Do this for both unmodulated (DC) and modulated signals (AC)
	if (!HTIRS2readAllDCStrength(HTIRS2L, dcS1L, dcS2L, dcS3L, dcS4L, dcS5L))
		break; // I2C read error occurred
	if (!HTIRS2readAllACStrength(HTIRS2L, acS1L, acS2L, acS3L, acS4L, acS5L ))
		break; // I2C read error occurred

	//=================Enhanced IR Values (Left and Right)===========
	// Read the Enhanced direction and strength
	if (!HTIRS2readEnhanced(HTIRS2L, _dirEnhL, _strEnhL))
		break; // I2C read error occurred

	//______________END SENSORVAL BLOCK________________________________________________________________
	//=================================================================================================

	if (acS3L < irFindVal) { //While sensor is heading towards beacon: acs3 = side
		motor[motorLeft] = -80;
		motor[motorRight] = -80;

		if (time1[T1] > timeToEnd) {
			searching = false;
			koth = true;
			goToEnd = false;
			//if it doesnt find the beacon, dont bother returning to start if it has been set to
		}
	}

	//===================================BLOCK FOR IR DETECTION=====================
	if (acS3L > irFindVal) { //if sensor is directly in front of beacon

		if (time1[T1] < 2000) {
			wait1Msec(750);
		}
		if (time1[T1] > 2000) {
			wait1Msec(300);
		}

		motor[motorLeft] = 0;
		motor[motorRight] = 0;
		//irOnLeft = true;
		searching = false;
		koth = true;
		goToEnd = true;
	}

	//==================END IR DETECTION BLOCK========================

	wait1Msec(30);
}//while searching

//Close(irFileHandle, IOResult);
roundTime = time1[T1]; //probably unnecessary, is to cut out the time from the cube scorer

scoreCube();

if (goToEnd) {
	if (FORWARD_SCORE_FORWARD_LINE_1 || FORWARD_SCORE_FORWARD_LINE_2) {
		driveToEnd(-80, timeToEnd - roundTime);//drive to end of ramp
	}
	if (FORWARD_SCORE_BACKWARD_LINE_1 || FORWARD_SCORE_BACKWARD_LINE_2) {
		driveToEnd(80, roundTime);
	}
}

wait1Msec(300);
//=======================================================================================================================================
//------------------------BEGIN 90 DEGREE TURNS------------------------------------------------------------------------------------------

//HTGYROstartCal(HTGYRO);
ClearTimer(T3);

while (true) {

	wait1Msec(20);

	//if (abs(rotSpeed) > 3) {
	rotSpeed = HTGYROreadRot(HTGYRO);//find gyro rotation speed
	heading += (rotSpeed * 0.02);//find gyro heading in degrees??

	motor[motorLeft] = 60;
	motor[motorRight] = -60;

	if (FORWARD_SCORE_FORWARD_LINE_2 || FORWARD_SCORE_FORWARD_LINE_1) {
		degToUse = degFirstTurn;
		} else {
		degToUse = degFirstTurnForRev;
	}

	if (heading <= degToUse) {

		motor[motorLeft] = 0;
		motor[motorRight] = 0;
		ClearTimer(T3);
		//---------------LINE DETECTOR--------------------------
		//LSsetActive(LEGOLS);
		while (linesFound < linesToFind) {
			motor[motorLeft] = -10;
			motor[motorRight] = -10;
			wait1Msec(10);
			if (LSvalRaw(LEGOLS) >= WHITE_LINE_VALUE && canFindWhite) {
				canFindWhite = false;
				linesFound++;
			}

			if (LSvalRaw(LEGOLS) <= WHITE_LINE_VALUE && !canFindWhite) {
				canFindWhite = true;
			}

			if (linesFound >= linesToFind) {
				break;
			}

			if (time1[T3] > 3200) {
				linesFound = 100;
				break;
			}
		}
		LSsetInactive(LEGOLS);

		//if (linesToFind == 2 && FORWARD_SCORE_FORWARD_LINE_2 && linesFound == 2) {
		//		moveBackward(700, 100);
		//	}

		if (FORWARD_SCORE_FORWARD_LINE_1 || FORWARD_SCORE_FORWARD_LINE_2) {

			while (true) {
				wait1Msec(20);
				rotSpeed = HTGYROreadRot(HTGYRO);//find gyro rotation speed
				heading += (rotSpeed * 0.02);//find gyro heading in degrees??

				motor[motorLeft] = 60;
				motor[motorRight] = -60;

				if (heading <= degSecondTurn) {
					motor[motorLeft] = 0;
					motor[motorRight] = 0;

					moveForward(3.8, 100);
					break;
				}
			}
			} else {
			while (true) {
				wait1Msec(20);
				rotSpeed = HTGYROreadRot(HTGYRO);//find gyro rotation speed
				heading += (rotSpeed * 0.02);//find gyro heading in degrees??

				motor[motorLeft] = -60;
				motor[motorRight] = 60;

				if (heading >= degSecondTurnForRev) {
					motor[motorLeft] = 0;
					motor[motorRight] = 0;

					moveForward(3.8, 100);
					break;
				}
			}
		}
		break;
	}
}

//==================================================================================

//Begin KotH routine
servo[servoUSSeeker] = 92;
USScanVal = 92;
float heading = 92;
LSsetInactive(LEGOLS);

//This never has time to run, so comment it out
HTGYROstartCal(HTGYRO);

while (koth) {

	//wait1Msec(1000);
	//SCAN LEFT==========================
	while(true) {

		servo[servoUSSeeker] = ServoValue[servoUSSeeker] + 5;
		USScanVal += 5;
		wait1Msec(100);

		if (SensorValue[US1] < kothAttackDistance && nPgmTime < 27000) { //if something is in range AND program time is less than 27 seconds
			PlaySound(soundFastUpwardTones);
			searchingForBot = true;
			turnedLeft = true;
			turnedRight = false;
			turnTowardsRobot();
			pushOffRamp();
			turnTowardsCenter();
		}
		if (USScanVal > 135) {
			break;
		}
	}
	//SCAN RIGHT=========================
	while(true) {
		servo[servoUSSeeker] = ServoValue[servoUSSeeker] - 5;
		USScanVal -= 5;
		wait1Msec(100);
		if (USScanVal < 40) {
			break;
		}
		if (SensorValue[US1] < kothAttackDistance && nPgmTime < 27000) { //if something is in range AND program time is less than 27 seconds
			PlaySound(soundFastUpwardTones);
			searchingForBot = true;
			turnedLeft = false;
			turnedRight = true;
			turnTowardsRobot();
			pushOffRamp();
			turnTowardsCenter();
		}
	}

	if (nPgmTime > 29000) {
		koth = false;
	}
}//while koth

MissionImpossible();
}
//END MAIN IF PROGIDS THING

//Define functions here instead of at the top
void driveToEnd(int motorPower, int distanceToEdge) {
motor[motorLeft] = motorPower;
motor[motorRight] = motorPower;
wait1Msec(distanceToEdge);
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
motor[motorAutoScore] = 100;
wait1Msec(500);
motor[motorAutoScore] = 0;

motor[motorAutoScore] = -100;
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

void initializeRobot() {
servo[srvo_S2_C3_1] = 165;
return;

}

void pushOffRamp() {
moveForward(1.5, 100); //drive forward and backward (make sure this doesnt drive us off the ramp)
wait1Msec(200);
moveBackward(1.5, 100);
wait1Msec(200);
PlaySound(soundBlip);
}

void turnTowardsRobot() {
//int tmpUS = USScanVal;
time1[T2] = 0;

while(searchingForBot) {
	while (USScanVal > 92 ) {

		wait1Msec(20);

		//if (abs(rotSpeed) > 3) {
		rotSpeed = HTGYROreadRot(HTGYRO);//find gyro rotation speed
		heading += (rotSpeed * 0.02) / 0.2;//find gyro heading in degrees??
		//}

		//PlayTone(400, 500);
		motor[motorLeft] = -5;
		motor[motorRight] = 5;
		if (heading >= USScanVal) {
			motor[motorLeft] = 0;
			motor[motorRight] = 0;
			searchingForBot = false;

		}//end if
		break;
	}//end while

	while (USScanVal < 92) {

		wait1Msec(20);

		//if (abs(rotSpeed) > 3) {
		rotSpeed = HTGYROreadRot(HTGYRO);//find gyro rotation speed
		heading += (rotSpeed * 0.02) / 0.3;//find gyro heading in degrees??
		//}

		//PlayTone(400, 500);
		motor[motorLeft] = 5;
		motor[motorRight] = -5;
		if (heading <= USScanVal) {
			motor[motorLeft] = 0;
			motor[motorRight] = 0;
			searchingForBot = false;

		}//end if
		break;
	}//end while

	if (USScanVal == 92) {
		//do nothing
		break;
	}
}//end search
}//end void

void turnTowardsCenter() {

if (turnedLeft) {
	while(heading > 92) {

		wait1Msec(20);

		//if (abs(rotSpeed) > 3) {
		rotSpeed = HTGYROreadRot(HTGYRO);//find gyro rotation speed
		heading += (rotSpeed * 0.02) / 0.2;//find gyro heading in degrees??
		//}

		motor[motorLeft] = 5;
		motor[motorRight] = -5;

		if (heading <= 92) {
			motor[motorLeft] = 0;
			motor[motorRight] = 0;
			break;
		}
	}
	turnedLeft = false;
	turnedRight = false;
}

if (turnedRight) {
	while(heading < 92) {

		wait1Msec(20);

		//if (abs(rotSpeed) > 3) {
		rotSpeed = HTGYROreadRot(HTGYRO);//find gyro rotation speed
		heading += (rotSpeed * 0.02) / 0.3;//find gyro heading in degrees??
		//}

		motor[motorLeft] = -5;
		motor[motorRight] = 5;

		if (heading >= 92) {
			motor[motorLeft] = 0;
			motor[motorRight] = 0;
			break;
		}
	}
	turnedLeft = false;
	turnedRight = false;
}
}

// void setGyroValuesForBatteryLevel(int batlvl) {
// 	degFirstTurn = 88 - batlvl; //87//if you do anything to this, do the opposite to the other (46)
// 	degFirstTurnForRev = 86 - batlvl; //87
// 	degSecondTurn = 48 - batlvl; //47//same for this one - should add up to 46 (0)
// 	degSecondTurnForRev = 120 - batlvl;
// }

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
