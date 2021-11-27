// ElevateSimulation
// This project simulates elevates in a building 
//

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <string>

#include "ElevatorSimulation.h"
#include "Building.h"
#include "Person.h"
#include "Elevator.h"

using namespace std;

mutex personQMutex;
mutex personWaitingQMutex;

void ElevatorSimulation::genPersonThread(int floors)
{
    while (true) {
        srand(time(NULL));
        int floorFrom = rand() % floors + 1;
        int floorTo = rand() % floors + 1;

        if (floorFrom != floorTo) {
            std::cout << endl << "Generated person waiting at floor " << floorFrom << " going to floor " << floorTo << endl;

            Floor f(floorFrom), t(floorTo);

            lock_guard<mutex> lck(personQMutex);
            auto a = allPersons.find(floorFrom);
            deque<Person> q;
            if (a != allPersons.end()) {
                q = a->second;
            }
            if (floorFrom % 2 == 0) {
                q.push_back(Male(f, t));
            }
            else {
                q.push_back(Female(f, t));
            }
            allPersons[floorFrom] = q;

            lock_guard<mutex> lck2(personWaitingQMutex);
            personsWaiting.insert({ floorFrom, personsWaiting[floorFrom] + 1 });
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(30 * 1000));
    }
}

thread ElevatorSimulation::generatePerson(int floors)
{
    // need pass the function name and the class name as the first 2 parameters
    std::thread genPerson(&ElevatorSimulation::genPersonThread, this, floors);

    return genPerson;
}

void ElevatorSimulation::printQueueThread() {
    while (true) {
        {
            lock_guard<mutex> lk(personQMutex);
            for (auto p = allPersons.begin(); p != allPersons.end(); ++p) {
                std::deque<Person> pn = p->second;
                for (auto q = pn.begin(); q != pn.end(); q++) {
                    cout << "Person is waiting at floor " << q->getFromFloor().getFloor() << " going to floor " << q->getToFloor().getFloor() << endl;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20 * 1000));
    }
}

thread ElevatorSimulation::printPersonalQueu()
{
    std::thread pThread(&ElevatorSimulation::printQueueThread, this);
    return pThread;
}

bool ElevatorSimulation::peopeWaitingAt(shared_ptr<Elevator> e, int f) {
    lock_guard<mutex> lck2(personWaitingQMutex);
    if ((personsWaiting[f] > 0) && (e->getCapacity() < e->getNumOfPersons())) {
        return true;
    }
    return false;
}

void ElevatorSimulation::travelTo(shared_ptr<Elevator> e, Floor to) {
    if (e->getCurrentFloor() != to.getFloor()) {
        std::cout << "Elevator is going from floor " << e->getCurrentFloor() << " to " << to.getFloor() << endl;
        int floors = abs(e->getCurrentFloor() - to.getFloor());
        Direction direction = Direction::up;
        if (e->getCurrentFloor() > to.getFloor())
            direction = Direction::down;

        for (int i = 1; i < floors + 1; ++i) {
            // at each floor, check if there is person waiting and if capacity is full, and then open door to get more people in
            int next = ((direction == Direction::up) ? (e->getCurrentFloor() + 1) : (e->getCurrentFloor() - 1));
            if (peopeWaitingAt(e, next)) {
                cout << "TBD: need stop at floor " << next << " to take people in" << endl;
                /*
                deque<Person> p = allPersons[next];

                while (!p.empty() && (e->getNumOfPersons() < e->getCapacity()) {
                    Person pn = p.front();
                    if ((direction == Direction::up && pn.getToFloor() > e->getCurrentFloor() && pn.getToFloor() <= to.getFloor()) ||
                            (direction == Direction::down && pn.getToFloor() < e->getCurrentFloor() && pn.getToFloor() >= to.getFloor())) {
                        if (direction == Direction::up) {


                        }
                    }
                }
                */
            }
            else {
                if (i != floors)
                    cout << "Elevator is passing floor " + std::to_string(next) << endl;
                else {
                    std::cout << "Elevator is arrived at floor " << std::to_string(to.getFloor()) << endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(e->getTravelTime() * 1000));
                }
                e->setCurrentFloor(next);
            }
        }
    }
}

bool ElevatorSimulation::noPersonsWaiting() {
    if (allPersons.empty())
        return true;
    for (auto a = allPersons.begin(); a != allPersons.end(); ++a) {
        deque<Person> p = a->second;
        if (!p.empty()) {
            return false;
        }
    }
    return true;
}

void ElevatorSimulation::started(Building *b)
{
    while (true) {
        { // put in a block so that the lock can be release before sleep
            shared_ptr<Elevator> e = b->getElevator();
            Direction direction = Direction::up;
            int nextFloor = e->getCurrentFloor() + 1;

            lock_guard<mutex> lk(personQMutex);
            while (!noPersonsWaiting()) {
                if (nextFloor > building.getNumOfFloors())  // do not go over the floors limit
                    nextFloor = 1;

                // find the next floor which has person waiting and closest to current floor
                auto q = allPersons.find(nextFloor);
                while (q == allPersons.end() && nextFloor <= b->getNumOfFloors()) {
                    nextFloor += 1;
                    q = allPersons.find(nextFloor);
                }
                if (q != allPersons.end()) {
                    std::deque<Person> pq = q->second;
                    if (!pq.empty()) {
                        Person p = pq.front();
                        b->getElevator()->addPerson(p);

                        // go to the floor to pick up the first person
                        travelTo(e, p.getFromFloor());

                        auto v = personsWaiting.find(p.getFromFloor().getFloor());
                        if (v != personsWaiting.end())
                            v->second = v->second - 1;

                        // send the first person to the to floor
                        e->setCurrentFloor(p.getFromFloor().getFloor());
                        travelTo(e, p.getToFloor());
                        std::cout << "Person who goes from floor " << std::to_string(p.getFromFloor().getFloor()) + " to floor "
                            + std::to_string(p.getToFloor().getFloor()) << " existed the door" << endl;
                        e->setCurrentFloor(p.getToFloor().getFloor());
                        pq.pop_front();
                        allPersons[p.getFromFloor().getFloor()] = pq;  // use = to update hash value

                        nextFloor = e->getCurrentFloor() + 1;
                    }
                }
            }
        }
    }
}

thread ElevatorSimulation::startSimulation(Building *b)
{
    cout << "Elevation started running ..." << endl;
    thread pThread(&ElevatorSimulation::started, this, b);
    return pThread;
}

int main()
{
    std::cout << "Simulation of Elevators in the building!" << endl;

    Building building(5);    
    // ElevatorSimulation *es = new ElevatorSimulation(building);  -- need call delete
    //unique_ptr<ElevatorSimulation> es(new ElevatorSimulation(building));
    unique_ptr<ElevatorSimulation> es = std::make_unique<ElevatorSimulation>(building);

    thread t1 = es->printPersonalQueu();
    thread t2 = es->generatePerson(es->numOfFloors());
    std::this_thread::sleep_for(std::chrono::milliseconds(10 * 1000)); // generate people first
    thread t3 = es->startSimulation(&building);

    t1.join();
    t2.join();
    t3.join();

    cout << "all done" << endl;

    return 0;
}