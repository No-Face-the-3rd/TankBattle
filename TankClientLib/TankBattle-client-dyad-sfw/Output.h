#pragma once




#ifndef OUTPUT_H
#define OUTPUT_H

#include "AIControl.h"
#include <iostream>
#include <sstream>

std::ostream &operator<<(std::ostream &os, const AI &a);
std::ostream &operator<<(std::ostream &os, const tankNet::TankBattleStateData &a);
std::ostream &operator<<(std::ostream &os, const tankNet::TankBattleCommand &a);
std::ostream &operator<<(std::ostream &os, const tankNet::TankTacticalInfo &a);
std::ostream &operator<<(std::ostream &os, float a[3]);

#endif