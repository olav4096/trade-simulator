#pragma once

#include "Simulation.h"

struct BuySellParams
{
	float StopLoss;
	float StopLossInc;
	float TakeProfit;
	float TakeProfitInc;
};

class BuySellSimulator : public Simulator
{
public:
	using Params = BuySellParams;
};