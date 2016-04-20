#include "Output.h"

std::ostream &operator<<(std::ostream &os, const AI &a)
{
	os << "Target Loc: " << std::endl;
	os << (float*)a.targetLoc << std::endl;
	os << "Aim Target: " << std::endl;
	os << (float*)a.aimTarget << std::endl;
	os << "Turning: " << a.turning << " Move: " << a.forward << std::endl;
	os << "ToggleTurn: " << a.toggleTurn << " Target Ind: " << a.target << std::endl;
	os << a.curState << std::endl;
	return os;
}

std::ostream &operator<<(std::ostream &os, const tankNet::TankBattleStateData &a)
{
	os << "Player ID: " << a.playerID << std::endl;
	os << "Health: " << a.currentHealth << std::endl;
	os << "Pos: " << std::endl;
	os << (float*)a.position << std::endl;
	os << "For: " << std::endl;
	os << (float*)a.forward << std::endl;
	os << "Cannon For: " << std::endl;
	os << (float*)a.cannonForward << std::endl;
	os << "Can Fire: " << a.canFire << std::endl;
	os << " Tactical Count: " << a.tacticoolCount << std::endl;
	if (a.tacticoolCount)
		for (int i = 0; i < a.tacticoolCount; i++)
			os << a.tacticoolData[i] << std::endl;
	return os;
}

std::ostream &operator<<(std::ostream &os, const tankNet::TankBattleCommand &a)
{
	os << "Message type: " << (unsigned)a.msg << std::endl;
	os << "Move Order: " << (unsigned)a.tankMove << std::endl;
	os << "Aim Order: " << (unsigned)a.cannonMove << std::endl;
	os << "Fire Order: " << a.fireWish << std::endl;
	os << "Message Length: " << a.messageLength << std::endl;
	return os;
}

std::ostream &operator<<(std::ostream &os, const tankNet::TankTacticalInfo &a)
{
	os << "Player ID: " << a.playerID << std::endl;
	os << "Health: " << (unsigned)a.health << std::endl;
	os << "Alive: " << a.isAlive << " Visible: " << a.inSight << std::endl;
	os << "Last Known Pos: " << std::endl;
	os << (float*)a.lastKnownPosition << std::endl;
	os << "Last Known Dir: " << std::endl;
	os << (float*)a.lastKnownDirection << std::endl;
	os << "Last Known Tank For: " << std::endl;
	os << (float*)a.lastKnownTankForward << std::endl;
	os << "Last Known Cannon For: " << std::endl;
	os << (float*)a.lastKnownCannonForward << std::endl;
	return os;
}

std::ostream &operator<<(std::ostream &os, float a[3])
{
	os << a[0] << ", " << a[1] << ", " << a[2];
	return os;
}