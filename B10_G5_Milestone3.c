#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    armPotentiometer, sensorPotentiometer)
#pragma config(Sensor, in2,    rightIR,        sensorReflection)
#pragma config(Sensor, in3,    leftIR,         sensorReflection)
#pragma config(Sensor, dgtl3,  sonarLed,       sensorDigitalOut)
#pragma config(Sensor, dgtl4,  frontSonar,     sensorSONAR_cm)
#pragma config(Sensor, dgtl6,  leftButton,     sensorTouch)
#pragma config(Sensor, dgtl7,  rightButton,    sensorTouch)
#pragma config(Sensor, I2C_1,  leftDTSensor,   sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  rightDTSensor,  sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port8,           rightDT,       tmotorVex393_MC29, openLoop, driveRight, encoderPort, I2C_2)
#pragma config(Motor,  port9,           leftDT,        tmotorVex393_MC29, openLoop, driveLeft, encoderPort, I2C_1)
#pragma config(Motor,  port10,          arm,           tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

int leftDriveTrainEncoderTarget = 0;
int rightDriveTrainEncoderTarget = 0;

//limit for motor power
const int MAX_POWER = 50;

//arm states
static const int ARM_MIN_POSITION_SENSOR_VALUE = 560;
static const int ARM_MID_POSITION_SENSOR_VALUE = 1880;
static const int ARM_MAX_POSITION_SENSOR_VALUE = 3300;

//robot task states
bool pointedAtTarget = true;
bool atTarget = false;
bool hitWall = false;

//initialize arm state to point up
int armEncoderTarget = ARM_MID_POSITION_SENSOR_VALUE;

//function to set the driveTrain state

void setDriveTrainEncoderTargets(int xDirectionTarget, int yDirectionTarget, bool zero)
{
	//zero the sensors
  if(zero)
  {
		SensorValue[leftDTSensor] = 0;
		SensorValue[rightDTSensor] = 0;
	}

	//assign motor power to respective orentations
	leftDriveTrainEncoderTarget = yDirectionTarget + xDirectionTarget;
	rightDriveTrainEncoderTarget = -(yDirectionTarget - xDirectionTarget);
}



task armController()
{
	//set the constants for the P controler
	const float KP = 0.200;

	//create error placeholder for the nessasary for the P controler
	int error;

	while (true)
	{
		//intialize error as the differance from the true value and the target
		error = armEncoderTarget - (SensorValue[armPotentiometer]);
		//assign power to the motor by KP * error
		motor[arm] = KP * error;
		wait1Msec(10);
	}
}

task leftDriveTrainEncPIDControl()
{
	/*const float KP = 0.085;
	const float KI = 0.0001;
  const float KD = 0.030;*/

  //set the constants for the PID controler
  const float KP = 1.80;
	const float KI = 0.20;
  const float KD = 2.85;



  // set the integral limit to prevent an integral wind up
  const float INTEGRAL_LIMIT = 20.0 / KI;

  //create variables for the nessasary for the PID controler
	int error;
	int errorPrevious;
  int integral;
	int derivative;
	int power;

	while (true)
	{
		//intialize error as the differance from the true value and the target
		error = leftDriveTrainEncoderTarget - (SensorValue[leftDTSensor]);
		//calculate the real time integral as an increment of the slices of error under the the SensorValue vs time curve
		integral += error;

		//calculate the real time derivitive by calculating the difference between the error and previos error
	  derivative = error - errorPrevious;

	  //To prevent integral wind up limmit how big the intgral can get
		if(abs(integral) > INTEGRAL_LIMIT)
		{
			integral = sgn(integral) * INTEGRAL_LIMIT;
		}

		//assign the respective gains
		int pGain = KP * error;
		int iGain = KI * integral;
		int dGain = KD * derivative;

		// add all the gains to make the power
		power = pGain + dGain + iGain; //+ iGain + dGain;
		//writeDebugStream("%-5d %-5d\n" , dGain, power);

		//limit the power to avoid skid
		if(abs(power) > MAX_POWER)
		{
			power = sgn(power) * MAX_POWER;
		}
		//assign power to the motor
		motor[leftDT] = power;

		//save the previous error for next loop
		errorPrevious = error;

		wait1Msec(10);
	}
}

task rightDriveTrainEncPIDControl()
{
	/*const float KP = 0.085;
	const float KI = 0.0001;
  const float KD = 0.030;*/

  //set the constants for the PID controler
  const float KP = 1.80;
	const float KI = 0.20;
  const float KD = 2.85;



  // set the integral limit to prevent an integral wind up
  const float INTEGRAL_LIMIT = 20.0 / KI;

  //create variables for the nessasary for the PID
	int error;
	int errorPrevious;
  int integral;
	int derivative;
	int power;


	while (true)
	{
		//intialize error as the differance from the true value and the target
		error = (rightDriveTrainEncoderTarget) - (SensorValue[rightDTSensor]);

		//calculate the real time integral as an increment of the slices of error under the the SensorValue vs time curve
		integral += error;

		//calculate the real time derivitive by calculating the difference between the error and previos error
	  derivative = error - errorPrevious;

	  //To prevent integral wind up limmit how big the intgral can get
		if(abs(integral) > INTEGRAL_LIMIT)
		{
			integral = sgn(integral) * INTEGRAL_LIMIT;
		}

		//assign the respective gains
		int pGain = KP * error;
		int iGain = KI * integral;
		int dGain = KD * derivative;

		// add all the gains to make the power
		power = pGain + dGain + iGain; //+ iGain + dGain;
		//writeDebugStream("%-5d %-5d\n" , dGain, power);

		//limit the power to avoid skid
		if(abs(power) > MAX_POWER)
		{
			power = sgn(power) * MAX_POWER;
		}
		//assign power to the motor
		motor[rightDT] = power;
		//save the previous error for next loop
		errorPrevious = error;
		//writeDebugStream("%-5d %-5d %-5d \n" ,time1[T1], SensorValue[leftDTSensor],  SensorValue[rightDTSensor]);
		wait1Msec(10);
	}
}
bool lastReadRight = false;
bool lastReadLeft = true;

task lastRead()
{
	while(true)
	{
			if(1000 > SensorValue[leftIR])
			{
				lastReadRight = false;
				lastReadLeft = true;
			}

			if(1000 > SensorValue[rightIR])
			{
				lastReadRight = true;
				lastReadLeft = false;
			}

			wait1Msec(10);
	}
}

task targetCheck()
{
	while(true)
	{
		//if the beacon is in the prons of the robot turn on light
		if(SensorValue[frontSonar] < 8 && SensorValue[frontSonar] != -1)
		{
			//indicate it is at the target
			atTarget = true;
			//turn on LED
			SensorValue[sonarLed] = 1;
		}
		else
		{
			//indicate it is not at the target
			atTarget = false;
			//turn off LED
			SensorValue[sonarLed] = 0;
		}
		wait1Msec(10);
	}
}

int threshold;
task trackSource()
{
	//set the intal rotation to 0
	int rotation = 0;
	//set the rotation step
	int delta = 1;
	//set the IR sensor threshold


  //int counter = 0;
	int rightIRSen = 0;
	int leftIRSen = 0;
	while(true)
	{
		//reset rotation upon every search

			rotation = 0;
		  while(!pointedAtTarget)
		  {

				//check if the robot is looking or at the target
			  leftIRSen = SensorValue[leftIR];
			  rightIRSen = SensorValue[rightIR];
				if((threshold > (leftIRSen) && threshold > (rightIRSen)) || atTarget)
				{

					//declare the robot is indeed pointing at the target

				 	//stop the robot

				 	setDriveTrainEncoderTargets(0,0, true);


				 	pointedAtTarget = true;
				 	//counter++;
				}
			else
			{
				if(lastReadRight)
					{

							delta = abs(delta);

					}
					else if(lastReadLeft)
					{

							delta = -abs(delta);


					}
					rotation += delta;
					setDriveTrainEncoderTargets(rotation, 0, false);
	  	}
			wait1Msec(5);
		}
		wait1Msec(10);
	}

}




task main()
{
	//start tasks ready for project
	//task to start right drivetrain PID
	startTask(rightDriveTrainEncPIDControl);
	//task to start left drivetrain PID
	startTask(leftDriveTrainEncPIDControl);
	//task to find source

	//task to see if the target is in conecttion proximity
	startTask(targetCheck);
	//task to check enviromental hazzards
	//
	startTask(lastRead);

	startTask(armController);

	startTask(trackSource);

	while(true)
	{
	  //if right button is pressed perform bumper test
		if(SensorValue[rightButton])
		{
			threshold = 3500;
			pointedAtTarget = false;
			waitUntil(pointedAtTarget || atTarget);
			setDriveTrainEncoderTargets(0, 2300, true);
			waitUntil(SensorValue[leftDTSensor] > 2290 || atTarget);
			setDriveTrainEncoderTargets(0,0,true);
			threshold = 2500;
			pointedAtTarget = false;
			waitUntil(pointedAtTarget || atTarget);
			setDriveTrainEncoderTargets(0, 600, true);
			waitUntil(SensorValue[leftDTSensor] > 590 || atTarget);
			setDriveTrainEncoderTargets(0,0,true);
			threshold = 1500;
			while(!atTarget)
			{

		    pointedAtTarget = false;
			  waitUntil(pointedAtTarget || atTarget);
			  setDriveTrainEncoderTargets(0, 100, true);
			  waitUntil(SensorValue[leftDTSensor] > (90) || atTarget);
			  setDriveTrainEncoderTargets(0,0,true);

		  }

		  armEncoderTarget = ARM_MAX_POSITION_SENSOR_VALUE;
		  waitUntil(SensorValue[armPotentiometer] > 3250);
		  wait1Msec(250);
		  armEncoderTarget = ARM_MIN_POSITION_SENSOR_VALUE;
		  waitUntil(SensorValue[armPotentiometer] < 600);
		  wait1Msec(250);
		  setDriveTrainEncoderTargets(0,-400,true);
		}
		//if left button is pressed perform IR tracker test
		/*if(SensorValue[leftButton])
		{
			//set the target pointer to false so task trackSource() can find it again
			pointedAtTarget = false;
	  }*/

	  /*if(SensorValue[leftButton])
	  {
	  	pointedAtTarget = false;
	  	waitUntil(pointedAtTarget || atTarget);
	  	wait1Msec(100);
	  	setDriveTrainEncoderTargets(0,1500,true);
			waitUntil(SensorValue[leftDTSensor] > 1490 || atTarget || hitWall);
			wait1Msec(50);
			setDriveTrainEncoderTargets(0,0,true);
	  }*/

		if(SensorValue[leftButton])
		{
			threshold = 1500;
			setDriveTrainEncoderTargets(0,0,true);
			while(!atTarget)
			{

		    pointedAtTarget = false;
			  waitUntil(pointedAtTarget || atTarget);
			  setDriveTrainEncoderTargets(0, 100, true);
			  waitUntil(SensorValue[leftDTSensor] > 90 || atTarget);
			  setDriveTrainEncoderTargets(0,0,true);
		  }

		  armEncoderTarget = ARM_MAX_POSITION_SENSOR_VALUE;
		  waitUntil(SensorValue[armPotentiometer] > 3250);
		  wait1Msec(250);
		  armEncoderTarget = ARM_MIN_POSITION_SENSOR_VALUE;
		  waitUntil(SensorValue[armPotentiometer] < 600);
		  wait1Msec(250);
		  setDriveTrainEncoderTargets(0,-400,true);
		}

		wait1Msec(10);
	}
}
