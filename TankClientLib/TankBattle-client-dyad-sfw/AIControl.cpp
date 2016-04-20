#pragma once

#include "AIControl.h"

#include <math.h>

tankNet::TankBattleCommand AI::update(tankNet::TankBattleStateData state, float deltaTime)
{
	tankNet::TankBattleCommand tmp;
	lastState = curState;
	curState = state;
	tmp.fireWish = 0;
	tmp.cannonMove = tankNet::CannonMovementOptions::HALT;
	tmp.msg = tankNet::TankBattleMessage::GONE;


	checkMotion(tmp);
	controlTurret(tmp);

	turning = checkTurn();
	forward = checkForward();

	if (curState.tacticoolCount)
		for (int i = 0; i < 3; i++)
		{
			targetLoc[i] = curState.tacticoolData[0].lastKnownPosition[i];
			aimTarget[i] = curState.tacticoolData[0].lastKnownPosition[i] + curState.tacticoolData[0].lastKnownTankForward[i];
		}
	else
		for (int i = 0; i < 3; i++)
			targetLoc[i] = aimTarget[i] = 0.0f;
	

	return tmp;
}

int AI::checkTurn()
{
	float tmpDir[3];
	getDir(curState.position, targetLoc, tmpDir);
	float tmp = getAngle(curState.forward, tmpDir);

	float buffer = 0.2f;
	if ((tmp <= PI / 2.0f && tmp >= buffer) || (tmp >= PI + buffer && tmp <= 3.0f * PI / 2.0f))
		return 1;
	else if ((tmp >= PI / 2.0f && tmp <= PI - buffer) || (tmp >= 3.0f * PI / 2.0f && tmp <= 2.0f * PI - buffer))
		return 2;
	else
		return 0;
}

int AI::checkForward()
{
	float tmpDir[3];
	getDir(curState.position, targetLoc, tmpDir);
	float tmp = getAngle(curState.forward, tmpDir);
	
	for (int i = 0; i < 3; i++)
		tmpDir[i] = targetLoc[i] - curState.position[i];

	

	if ((tmp <= PI / 2.0f || tmp >= 3.0f * PI / 2.0f) && mag(tmpDir) > 4.0f)
		return 1;
	else if((tmp >= PI / 2.0f && tmp <= 3.0f * PI / 2.0f) && mag(tmpDir) > 4.0f)
		return 2;
	else
		return 0;	
}

void AI::checkMotion(tankNet::TankBattleCommand &a)
{
	if (turning && forward)
	{
		if(toggleTurn)
			a.tankMove = turning == 1 ? tankNet::TankMovementOptions::LEFT : tankNet::TankMovementOptions::RIGHT;
		else
			a.tankMove = forward == 1 ? tankNet::TankMovementOptions::FWRD : tankNet::TankMovementOptions::BACK;
		toggleTurn = !toggleTurn;
	}
	else if (turning)
		a.tankMove = turning == 1 ? tankNet::TankMovementOptions::LEFT : tankNet::TankMovementOptions::RIGHT;
	else if (forward)
		a.tankMove = forward == 1 ? tankNet::TankMovementOptions::FWRD : tankNet::TankMovementOptions::BACK;
	else
	{
		a.tankMove = tankNet::TankMovementOptions::HALT;
		toggleTurn = false;
	}
	return;
}

void AI::controlTurret(tankNet::TankBattleCommand & a)
{
	float tmpDir[3];
	getDir(curState.position, aimTarget, tmpDir);
	float tmp = getAngle(curState.cannonForward, tmpDir);

	float buffer = 0.2f;
	if ((tmp >= PI + buffer))
		a.cannonMove = tankNet::CannonMovementOptions::LEFT;
	else if ((tmp <= PI - buffer))
		a.cannonMove = tankNet::CannonMovementOptions::RIGHT;
	else
		a.cannonMove = tankNet::CannonMovementOptions::HALT;
}

float getAngle(float dir[3])
{
	return std::atan2f(dir[2], dir[0]);
}

float getAngle(float dirA[3], float dirB[3])
{
	float tmp = getAngle(dirB) - getAngle(dirA);
	if (tmp < 0.0f)
		tmp += 2 * PI;
	return tmp;
}

void getDir(float posA[3], float posB[3], float out[3])
{
	for (int i = 0; i < 3; i++)
		out[i] = posB[i] - posA[i];
	float tmp = mag(out);
	for (int i = 0; i < 3; i++)
		out[i] = out[i] / tmp;
	out[1] = 0.0f;
	return;
}

float mag(float vec[3])
{
	float tmp = 0.0f;
	for (int i = 0; i < 3; i++)
		tmp += std::pow(vec[i], 2);
	tmp -= std::pow(vec[1], 2);
	tmp = std::sqrt(tmp);
	return tmp;
}