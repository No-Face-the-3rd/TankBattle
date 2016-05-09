#pragma once

#ifndef AICONTROL_H
#define AICONTROL_H


#include "TankBattleNet.h"
#include "sfwdraw.h"
#include <vector>

enum moveState
{
	ABYSS,START, SEARCH, ACTIVE, FIND
};

struct AI
{
	float targetLoc[3], aimTarget[3], startLoc[3];
	int turning;
	int forward;
	int toggleTurn;
	int target;
	tankNet::TankBattleStateData lastState, curState;
	tankNet::TankBattleCommand lastCom, curCom;
	bool started = false;
	std::vector<float> lastSeenTime, lastPosUpdateTime, lastCanUpdateTime;
	moveState moveState = ABYSS, prevMoveState = ABYSS;
	bool clockwise = false;
	float stickTime, formerStickTime;
	bool unstickSeen;
	float searchTime;

	float tmpOut;

	void update(tankNet::TankBattleStateData state, float deltaTime);
	int checkTurn();
	int checkForward();
	void checkMotion();
	void controlTurret();
	void checkFire();
	void locResets();
	void comReset();
	void checkSeen(const float &dt);
	void checkUpdated(const float &dt);
	void targetLocMove(const float &dt);
	void search(float start[3], const float &dt);
	void aim();
	void avoid();
	void unstick(const float &dt);
};

float getAngle(float dir[3]);
float getAngle(float dirA[3], float dirB[3]);
void getDir(float posA[3], float posB[3], float out[3]);
float mag(float vec[3]);



#endif