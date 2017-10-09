#pragma once

#include <general/timer.h>
#include "simulation.h"

using namespace DeltaWorks;

struct TInputVector
{
	int	directionChange = 0;
	int	speedChange = 0;
};

extern Timer::Time applicationStart;
static const count_t NumSamples = 100;
bool	BeginNextRange();
extern count_t accumulation;
extern Simulation	testSimulation;
extern TExperiment	currentSetup;


void	EvolveV(const TInputVector&);
void	Evolve();
void	StartNext(bool failed);
void	SetupScene();

void	Pause();
void	VerifyIntegrity();
void	MakeErrorFree();
void	RecoverOnly();

void	StartSimulation();
void	StopSimulation();
bool	SimulationActive();
