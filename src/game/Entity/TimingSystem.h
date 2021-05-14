#pragma once

class EntityManager;

class TimingSystem {

public :
	void UpdateTimers(EntityManager& em);
	void ApplyTimerActions(EntityManager& em);
};