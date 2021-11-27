#pragma once

#include <stack>
#include "Elevator.h"
#include "Person.h"

enum class Direction { up, down, idle };
enum class Event { door_open, door_close };

class Elevator
{
	int capacity;
	int numOfPersons;
	int currentFloor;
	int toFloor;
	std::vector<int> floorsSelected;
	int travelTime;
	Direction direction;
	Event event;
	std::vector<Person> persons;

public:
	Elevator(int c, int t, Direction d) : capacity(10), travelTime(5), currentFloor(c), toFloor(t), direction(d) {
		numOfPersons = 0;
		event = Event::door_close;
	}

	int getCapacity() { return capacity; }
	int getNumOfPersons() { return numOfPersons; }
	int getTravelTime() { return travelTime; }
	int getCurrentFloor() { return currentFloor; }
	void setCurrentFloor(int f) { currentFloor = f; }
	void settoFloor(int t) { toFloor = t; }
	std::vector<Person> getPersons() { return persons; }
	void addPerson(Person p) { persons.push_back(p); numOfPersons++; }
};