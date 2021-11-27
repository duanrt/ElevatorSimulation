#pragma once

#include <iostream>
#include <deque>
#include <thread>
#include "Building.h"
#include "Person.h"

using namespace std;

class ElevatorSimulation
{
	// default is private
	Building building;
	std::unordered_map<int, std::deque<Person>> allPersons; // [ floor: [ persons ] ]
	std::unordered_map<int, int> personsWaiting;  // [floor : number]

public:
	ElevatorSimulation(Building b) : building(b) {
		for (int i = 0; i < b.getNumOfFloors(); ++i) {
			personsWaiting[i] = 0;
		}
	}

	std::unordered_map<int, std::deque<Person>> getAllPersons() { return allPersons; }
	std::unordered_map<int, int> getPersonsWaiting() { return personsWaiting; }
	int numOfFloors() { return building.getNumOfFloors(); }
	thread generatePerson(int);
	void genPersonThread(int floors);
	void printQueueThread();
	thread printPersonalQueu();
	thread startSimulation(Building *);
	void started(Building*);
	bool peopeWaitingAt(shared_ptr<Elevator> e, int f);
	bool noPersonsWaiting();
	void travelTo(shared_ptr<Elevator> e, Floor to);
};