#pragma once

#ifndef AICONTROL_H
#define AICONTROL_H


#include "TankBattleNet.h"
#include "sfwdraw.h"

struct AI
{
	float targetLoc[3], aimTarget[3];
	int turning;
	int forward;
	bool toggleTurn;
	int toggleTurnBuffer;
	int target;
	tankNet::TankBattleStateData lastState, curState;


	tankNet::TankBattleCommand update(tankNet::TankBattleStateData state, float deltaTime);
	int checkTurn();
	int checkForward();
	void checkMotion(tankNet::TankBattleCommand &a);
	void controlTurret(tankNet::TankBattleCommand &a);
};

float getAngle(float dir[3]);
float getAngle(float dirA[3], float dirB[3]);
void getDir(float posA[3], float posB[3], float out[3]);
float mag(float vec[3]);



#endif