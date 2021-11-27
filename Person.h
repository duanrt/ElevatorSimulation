#pragma once

#include "Elevator.h"
#include "Floor.h"

enum class Sex { male, female };

class Person
{
protected:
	Floor fromFloor;
	Floor toFloor;

public:
	Person(Floor from, Floor to) : fromFloor(from), toFloor(to) { }

	Floor getFromFloor() { return fromFloor; }
	Floor getToFloor() { return toFloor; }
	Sex getSex() {}
};

class Male : public Person
{
	Sex sex;

public:
	Male(Floor from, Floor to) : Person(from, to) { sex = Sex::male; }
	Sex getSex() { return sex; }
};

class Female : public Person
{
	Sex sex;

public:
	Female(Floor from, Floor to) : Person(from, to) { sex = Sex::female; }
	Sex getSex() { return sex; }
};