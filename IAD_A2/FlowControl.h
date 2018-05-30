/*
*	FILE		  : FlowControl.h
*   PROJECT		  :	IAD - Assignment 2
*   PROGRAMMER	  : Wes Martin And Ricardo Mohammed
*	FIRST VERSION : 12/3/18
*	DESCRIPTION	  : This file contains the
*/

#pragma once
#include <cstdio>
#include <cstdlib>

class FlowControl
{
public:

	FlowControl();

	void Reset();

	void Update(float deltaTime, float rtt);

	float GetSendRate();

private:

	enum Mode
	{
		Good,
		Bad
	};

	Mode mode;
	float penalty_time;
	float good_conditions_time;
	float penalty_reduction_accumulator;
};

