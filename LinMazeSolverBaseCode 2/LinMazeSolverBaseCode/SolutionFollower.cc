#include <Pololu3piPlus32U4.h>
#include <PololuMenu.h>

using namespace Pololu3piPlus32U4;

#include "MazeSolver.h"
#include "SolutionFollower.h"
#include "Shared.h"

void SolutionFollower::turnLeft() {

  motors.setSpeeds(baseSpeed, baseSpeed);
  delay(250);
  motors.setSpeeds(0, 0);

  motors.setSpeeds(-baseSpeed, baseSpeed);
  delay(730);
  motors.setSpeeds(0, 0);
  state = LINE_FOLLOWER;
}

void SolutionFollower::turnRight() {

  motors.setSpeeds(baseSpeed, baseSpeed);
  delay(250);
  motors.setSpeeds(0, 0);

  motors.setSpeeds(baseSpeed, -baseSpeed);
  delay(730);
  motors.setSpeeds(0, 0);
  state = LINE_FOLLOWER;
}

void SolutionFollower::followLine() {
  // get position & error
  int16_t position = lineSensors.readLineBlack(lineSensorValues);
  int16_t error = position - 2000;

  // calculate speed difference with PID formula
  int16_t speedDifference = error * (int32_t)proportional / 256 + (error - lastError) * (int32_t)derivative / 256;

  // store error
  lastError = error;

  // get new speed & constrain
  int16_t leftSpeed = (int16_t)baseSpeed + speedDifference;
  int16_t rightSpeed = (int16_t)baseSpeed - speedDifference;
  leftSpeed = constrain(leftSpeed, minSpeed, (int16_t)maxSpeed);
  rightSpeed = constrain(rightSpeed, minSpeed, (int16_t)maxSpeed);

  // update motor speed
  motors.setSpeeds(leftSpeed, rightSpeed);
}


void MazeSolver::checkIfJunction() {
  lineSensors.readLineBlack(lineSensorValues);

  bool junction = false;

  if (lineSensorValues[0] > 950) junction = true;  // detect a line to the left
  if (lineSensorValues[1] > 950) junction = true;  // detect a line to the left
  if (lineSensorValues[3] > 950) junction = true;  // detect a line to the right
  if (lineSensorValues[4] > 950) junction = true;  // detect a line to the right
  // any other case contains one of these types

  if (junction) {
    state = JUNCTION;
    motors.setSpeeds(0, 0);
  }
}


void MazeSolver::identifyJunction() {

  delay(500);
  // move forward to identify other junctions
  motors.setSpeeds(baseSpeed, baseSpeed);
  delay(250);
  motors.setSpeeds(0, 0);
  lineSensors.readLineBlack(lineSensorValues);



  // if can sense everywhere -> FINISHED
  if (lineSensorValues[0] > 950 && lineSensorValues[1] > 950 && lineSensorValues[2] > 950 && lineSensorValues[3] > 950 && lineSensorValues[4] > 950) {
    state = FINISHED;
    return;
  }


  // if there's a left take it
  if (lineSensorValues[0] > 750) {
    state = TURN_LEFT;
    if (lineSensorValues[2] > 750 || lineSensorValues[4] > 750)

      addDecision(LEFT);

    return;
  }

  if (lineSensorValues[2] > 750) {
    motors.setSpeeds(baseSpeed, baseSpeed);
    delay(100);

    state = LINE_FOLLOWER;


    addDecision(FORWARD);


    return;
  }

  // if there's a left take it
  if (lineSensorValues[4] > 750) {


    state = TURN_RIGHT;


    return;
  }


  // any other case -> keep going
  state = LINE_FOLLOWER;
}
