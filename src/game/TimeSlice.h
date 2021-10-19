#pragma once

class TimeSlice {
private:
	double _start;
public:
	double TimeBudgetSeconds;

	TimeSlice(double budget);
	void Restart();

	bool IsTimeElapsed() const;
};