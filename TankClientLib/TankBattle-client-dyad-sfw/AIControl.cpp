#pragma once

#include "AIControl.h"

#include <math.h>
#undef NONE
#undef PI
#define PI 3.14159265358979323846264338327950288419716939937510f
#define HALFPI PI/2.0f


void AI::update(tankNet::TankBattleStateData state, float deltaTime)
{
	lastCom = curCom;
	comReset();
	lastState = curState;
	curState = state;
	locResets();
	

	checkMotion();
	controlTurret();
	checkFire();

	checkSeen(deltaTime);
	checkUpdated(deltaTime);

	targetLocMove();

	turning = checkTurn();
	forward = checkForward();
	

	return;
}

int AI::checkTurn()
{
	float tmpDir[3];
	getDir(curState.position, targetLoc, tmpDir);
	float tmp = getAngle(curState.forward, tmpDir);

	float buffer = 0.20f;
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
	tmpDir[1] = 0.0f;

	if (mag(tmpDir) > 14.0f)
	{
		if ((tmp <= HALFPI || tmp >= 3.0f * HALFPI))
			return 1;
		else if ((tmp > HALFPI && tmp < 3.0f * HALFPI))
			return 2;
	}
	else
	{
		if ((tmp <= HALFPI || tmp >= 3.0f * HALFPI))
			return 2;
		else if ((tmp > HALFPI && tmp < 3.0f * HALFPI))
			return 1;
	}

	return 0;	
}

void AI::checkMotion()
{
	if (turning && forward)
	{
		if(toggleTurn)
			curCom.tankMove = turning == 1 ? tankNet::TankMovementOptions::LEFT : tankNet::TankMovementOptions::RIGHT;
		else
			curCom.tankMove = forward == 1 ? tankNet::TankMovementOptions::FWRD : tankNet::TankMovementOptions::BACK;
		toggleTurn = (++toggleTurn) % 25;
	}
	else if (turning)
	{
		curCom.tankMove = turning == 1 ? tankNet::TankMovementOptions::LEFT : tankNet::TankMovementOptions::RIGHT;
		toggleTurn = 0;
	}
	else if (forward)
	{
		curCom.tankMove = forward == 1 ? tankNet::TankMovementOptions::FWRD : tankNet::TankMovementOptions::BACK;
		toggleTurn = 0;
	}
	else
	{
		curCom.tankMove = tankNet::TankMovementOptions::HALT;
		toggleTurn = 0;
	}
	return;
}

void AI::controlTurret()
{
	float tmpDir[3];
	getDir(curState.position, aimTarget, tmpDir);
	float tmp = getAngle(curState.cannonForward, tmpDir);

	float buffer = 0.20f;
	if ((tmp >= PI && tmp <= 2.0f  * PI - buffer))
		curCom.cannonMove = tankNet::CannonMovementOptions::RIGHT;
	else if ((tmp < PI && tmp >= buffer))
		curCom.cannonMove = tankNet::CannonMovementOptions::LEFT;
	else
		curCom.cannonMove = tankNet::CannonMovementOptions::HALT;
}

void AI::checkFire()
{
	if ((unsigned)curCom.cannonMove == 0)
	{
		float tmpVec[3];
		for (int i = 0; i < 3; i++)
			tmpVec[i] = aimTarget[i] - curState.position[i];
		if (mag(tmpVec) >= 4.0f && mag(tmpVec) <= 20.0f)
			curCom.fireWish = 1;
		else
			curCom.fireWish = 0;
	}
}

void AI::locResets()
{
	if (!started && state == ABYSS)
	{
		for (int i = 0; i < lastSeenTime.size(); i++)
			lastSeenTime.pop_back();
		for (int i = 0; i < lastPosUpdateTime.size(); i++)
			lastPosUpdateTime.pop_back();
		for (int i = 0; i < lastCanUpdateTime.size(); i++)
			lastCanUpdateTime.pop_back();
		startLoc[0] = targetLoc[0] = aimTarget[0] = curState.position[0];
		startLoc[1] = targetLoc[1] = aimTarget[1] = 0.0f;
		startLoc[2] = targetLoc[2] = aimTarget[2] = curState.position[2];
		started = true;
		target = -1;
		state = START;
	}
}

void AI::comReset()
{
	curCom.cannonMove = tankNet::CannonMovementOptions::HALT;
	curCom.fireWish = 0.0f;
	curCom.msg = tankNet::TankBattleMessage::NONE;
	curCom.tankMove = tankNet::TankMovementOptions::HALT;
}

void AI::checkSeen(const float & dt)
{
	while (lastSeenTime.size() < curState.tacticoolCount)
	{
		lastSeenTime.push_back(-0.1f);
	}
	if (curState.tacticoolCount)
		for (int i = 0; i < curState.tacticoolCount; i++)
		{
			if (curState.tacticoolData[i].inSight)
			{
				lastSeenTime.at(i) = 0.0f;
			}
			else
				lastSeenTime.at(i) += dt;
		}
}

void AI::checkUpdated(const float & dt)
{
	while (lastPosUpdateTime.size() < curState.tacticoolCount)
	{
		lastPosUpdateTime.push_back(-0.1f);
	}
	while (lastCanUpdateTime.size() < curState.tacticoolCount)
	{
		lastCanUpdateTime.push_back(-0.1f);
	}
	if (curState.tacticoolCount)
		for (int i = 0; i < curState.tacticoolCount; i++)
		{
			if (!(curState.tacticoolData[i].lastKnownPosition[0] - lastState.tacticoolData[i].lastKnownPosition[0] <= FLT_EPSILON && curState.tacticoolData[i].lastKnownPosition[2] - lastState.tacticoolData[i].lastKnownPosition[2] <= FLT_EPSILON))
				lastPosUpdateTime.at(i) = 0.0f;
			else
				lastPosUpdateTime.at(i) += dt;
			if (!(curState.tacticoolData[i].lastKnownCannonForward[0] - lastState.tacticoolData[i].lastKnownCannonForward[0] <= FLT_EPSILON && curState.tacticoolData[i].lastKnownCannonForward[2] - lastState.tacticoolData[i].lastKnownCannonForward[2] <= FLT_EPSILON))
				lastCanUpdateTime.at(i) = 0.0f;
			else
				lastCanUpdateTime.at(i) += dt;
		}
}

void AI::targetLocMove()
{
	if (!curState.tacticoolCount)
	{
		if (this->state == START)
			this->state = SEARCH;
		else if (this->state != SEARCH)
		{
			this->state = ABYSS;
			started = false;
		}
	}

	if (curState.tacticoolCount)
	{
		for (int i = 0; i < curState.tacticoolCount; i++)
		{
			if (curState.tacticoolData[i].isAlive)
			{
				float tmp[3], tmp2[3];
				for (int j = 0; j < 3; j++)
				{
					tmp[j] = curState.tacticoolData[i].lastKnownPosition[j] - curState.position[j];
					if (target >= 0)
						tmp2[j] = targetLoc[j] - curState.position[j];
					else
						tmp2[j] = 10000;
				}
				if (mag(tmp) < mag(tmp2))
					target = i;
			}
			else
				if (target == i)
					target = -1;
		}
		if(target >= 0)
			for (int i = 0; i < 3; i++)
				targetLoc[i] = aimTarget[i] = curState.tacticoolData[target].lastKnownPosition[i];
		else
			for (int i = 0; i < 3; i++)
				targetLoc[i] = aimTarget[i] = curState.position[i];
		targetLoc[1] = aimTarget[1] = 0.0f;
	}

	switch (state)
	{
	case SEARCH:
		break;
	case ACTIVE:
		break;
	case FIND:
		break;
	}


}

void AI::search(float start[3])
{

}

void AI::avoid()
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