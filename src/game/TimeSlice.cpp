#include "TimeSlice.h"
#include "Platform.h"

TimeSlice::TimeSlice(double budget) : TimeBudgetSeconds(budget)
{
	Restart();
}

void TimeSlice::Restart()
{
	_start = Platform::ElaspedTime();
}

bool TimeSlice::IsTimeElapsed() const
{
	double budget = Platform::ElaspedTime() - _start;

	return budget > TimeBudgetSeconds;
}

bool TimeSlice::IsRemainingLessThan(double seconds)
{
	double budget = Platform::ElaspedTime() - _start;

	return budget > (TimeBudgetSeconds - seconds);
}
