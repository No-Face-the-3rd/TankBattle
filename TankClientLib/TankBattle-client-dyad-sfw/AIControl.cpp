#pragma once

#include "AIControl.h"

#include <math.h>

#undef PI
#define PI 3.14159265358979323846264338327950288419716939937510f
#define HALFPI PI/2.0f


tankNet::TankBattleCommand AI::update(tankNet::TankBattleStateData state, float deltaTime)
{
	tankNet::TankBattleCommand tmp;
	lastState = curState;
	curState = state;
	locResets();
	tmp.fireWish = 0;
	tmp.msg = tankNet::TankBattleMessage::GONE;


	checkMotion(tmp);
	controlTurret(tmp);
	checkFire(tmp);

	turning = checkTurn();
	forward = checkForward();

	if (curState.tacticoolCount)
	{
		for (int i = 0; i < curState.tacticoolCount; i++)
		{
			if (i + 1 >= lastSeenTime.size())
				lastSeenTime.push_back(0.0f);
			if (curState.tacticoolData[i].inSight)
				lastSeenTime.at(i) = 0.0f;
			else
				lastSeenTime.at(i) += deltaTime;
		}
		for (int i = 0; i < 3; i++)
		{
			targetLoc[i] = curState.tacticoolData[0].lastKnownPosition[i];// +10 * curState.tacticoolData[0].lastKnownTankForward[i];
			aimTarget[i] = curState.tacticoolData[0].lastKnownPosition[i];// +10 * curState.tacticoolData[0].lastKnownTankForward[i];
		}
	}
	else
		started = false;
	

	return tmp;
}

int AI::checkTurn()
{
	float tmpDir[3];
	getDir(curState.position, targetLoc, tmpDir);
	float tmp = getAngle(curState.forward, tmpDir);

	float buffer = 0.2f;
	if ((tmp <= HALFPI && tmp >= buffer) || (tmp >= PI + buffer && tmp < 3.0f * HALFPI))
		return 1;
	else if ((tmp > HALFPI && tmp <= PI - buffer) || (tmp >= 3.0f * HALFPI && tmp <= 2.0f * PI - buffer))
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


	if ((tmp <= HALFPI || tmp >= 3.0f * HALFPI) && mag(tmpDir) > 2.0f)
		return 1;
	else if((tmp > HALFPI && tmp < 3.0f * HALFPI) && mag(tmpDir) > 2.0f)
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
		toggleTurn = (++toggleTurn) % 3;
	}
	else if (turning)
		a.tankMove = turning == 1 ? tankNet::TankMovementOptions::LEFT : tankNet::TankMovementOptions::RIGHT;
	else if (forward)
		a.tankMove = forward == 1 ? tankNet::TankMovementOptions::FWRD : tankNet::TankMovementOptions::BACK;
	else
	{
		a.tankMove = tankNet::TankMovementOptions::HALT;
		toggleTurn = 0;
	}
	return;
}

void AI::controlTurret(tankNet::TankBattleCommand & a)
{
	float tmpDir[3];
	getDir(curState.position, aimTarget, tmpDir);
	float tmp = getAngle(curState.cannonForward, tmpDir);

	float buffer = 0.20f;
	if ((tmp >= PI && tmp <= 2.0f  * PI - buffer))
		a.cannonMove = tankNet::CannonMovementOptions::RIGHT;
	else if ((tmp < PI && tmp >= buffer))
		a.cannonMove = tankNet::CannonMovementOptions::LEFT;
	else
		a.cannonMove = tankNet::CannonMovementOptions::HALT;
}

void AI::checkFire(tankNet::TankBattleCommand &a)
{
	if ((unsigned)a.cannonMove == 0)
	{
		float tmpVec[3];
		for (int i = 0; i < 3; i++)
			tmpVec[i] = aimTarget[i] - curState.position[i];
		if (mag(tmpVec) >= 15.0f && mag(tmpVec) <= 17.0f)
			a.fireWish = 1;
		else
			a.fireWish = 0;
	}
}

void AI::locResets()
{
	if (!started)
	{
		for (int i = 0; i < 3; i++)
			startLoc[i] = targetLoc[i] = aimTarget[i] = curState.position[i];
		started = true;
		for (int i = 0; i < lastSeenTime.size(); i++)
			lastSeenTime.pop_back();
	}
}

void AI::targetMove(const float & dt)
{

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
	out[1] = 0.0f;
	float tmp = mag(out);
	for (int i = 0; i < 3; i++)
		out[i] = out[i] / tmp;
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