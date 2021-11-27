#pragma once

// floors start from 1, 2, 3, ...
class Floor {
	int number;
	int personsWaiting;

public:
	Floor() { number = 1; personsWaiting = 0; }
	Floor(int n) : number(n) { personsWaiting = 0; }
	Floor(int n, int p) : number(n), personsWaiting(p) {}

	int getFloor() { return number; }
	int getPersonsWaiting() { return personsWaiting; }
};

