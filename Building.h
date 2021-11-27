#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include "Elevator.h"

using namespace std;

// only one elevator in the building
class Building
{
	int numOfFloors;
	shared_ptr<Elevator> elevator;

public:
	Building(int f) : numOfFloors(f) { 
		elevator = make_shared<Elevator>( 1, 1, Direction::idle ); 
		std::cout << "Elevator is generated, idle at floor 1" << endl;
	}
	Building(int f, Elevator *e) : numOfFloors(f), elevator(e) {}

	int getNumOfFloors() { return numOfFloors; }
	shared_ptr<Elevator> getElevator() { return elevator; }
};