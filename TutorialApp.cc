#include <stdlib.h>
#include "TutorialAppl.h"
#include <iostream>
#include <string>
#include <list>
#include "veins/base/utils/Coord.h"

Define_Module(TutorialAppl);

void TutorialAppl::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        lastSent = simTime();
        canGo = false;
    }
}

void TutorialAppl::onISM(IntersectMessage* ism) {
    findHost()->getDisplayString().updateWith("r=16,green");
    /*
     * this code won't work because you are comparing type id with real id
    std::string id = "straightFromLeft0";
    if ((id.compare(traciVehicle->getTypeId())) == 0) {
        traciVehicle->setSpeedMode(0000);
        traciVehicle->setSpeed(0.0);
    }
    */
}

void TutorialAppl::handleSelfMsg(cMessage* msg) {
    if (IntersectMessage* ism = dynamic_cast<IntersectMessage*>(msg)) {
        delete(ism);
    }
    else {
        BaseWaveApplLayer::handleSelfMsg(msg);
    }
}

void TutorialAppl::handleLowerMsg(cMessage* msg) {
    //std::cout << "new handle  lower msg" << std::endl;
    IntersectMessage* ism = dynamic_cast<IntersectMessage*>(msg);
    onISM(ism);
    delete(msg);
}

int TutorialAppl::getDirection(std::string currRoad, std::string nextRoad) {
    int direction;
    if (currRoad == "1i") {
        if (nextRoad == "2o") {
            direction = STRAIGHT;
        }
        else if (nextRoad == "3o") {
            direction = RIGHT;
        }
        else  {
            direction = LEFT;
        }
    }
    else if (currRoad == "2i") {
        if (nextRoad == "1o") {
            direction = STRAIGHT;
        }
        else if (nextRoad == "4o") {
            direction = RIGHT;
        }
        else  {
            direction = LEFT;
        }
    }
    else if (currRoad == "3i") {
        if (nextRoad == "4o") {
            direction = STRAIGHT;
        }
        else if (nextRoad == "2o") {
            direction = RIGHT;
        }
        else  {
            direction = LEFT;
        }
    }
    else {
        if (nextRoad == "3o") {
            direction = STRAIGHT;
        }
        else if (nextRoad == "1o") {
            direction = RIGHT;
        }
        else  {
            direction = LEFT;
        }
    }
    return direction;
}

IntersectMessage* TutorialAppl::populateISM(IntersectMessage *ism, bool passed) {
    //get fields for ism
    const char* vehicleId = mobility->getExternalId().c_str();
    const char* roadId = mobility->getRoadId().c_str();
    double speed = mobility->getSpeed();
    Coord pos = mobility->getPositionAt(simTime());

    //gets 2nd and 3rd position in car route to figure out next road
    std::list<std::string> plannedRoadIds = traciVehicle->getPlannedRoadIds();
    std::list<std::string>::iterator it = plannedRoadIds.begin();
    std::advance(it, 1);
    std::string prevRoad = *it;
    std::advance(it, 1);
    //gets direction car is trying to turn in intersection
    int direction = getDirection(prevRoad, *it);

    //sets fields in message and returns
    ism->setVehicleId(vehicleId);
    ism->setSenderSpeed(speed);
    ism->setSenderPos(pos);
    ism->setRoadId(roadId);
    ism->setDirection(0); //***need to fix this***!!!
    ism->setPassed(passed);
    return ism;
}


void TutorialAppl::handlePositionUpdate(cObject* obj) {
    BaseWaveApplLayer::handlePositionUpdate(obj);
    if (simTime() - lastSent >= 1) {
        //get distance between car and junction
        Coord juncPos = traci->junction("0").getPosition();
        Coord pos = mobility->getPositionAt(simTime());
        double distance = pos.distance(juncPos);
        std::string roadId = traciVehicle->getRoadId();
        //if they cannot go through intersection, will slow down
        /*
        if (distance <= 20.0 && !canGo) {
            traciVehicle->slowDown(0, 1000);
        }
        //otherwise they will go through intersection
        else if (canGo) {
            traciVehicle->slowDown(20, 500);
        }
        */
        //create new msg to RSU and send it
        IntersectMessage* ism = new IntersectMessage();
        //if they have passed intersection they will let RSU know
        if (roadId == "1o" || roadId == "2o" ||
            roadId == "3o" || roadId == "4o") {
            ism = populateISM(ism, true);
        }
        else {
            ism = populateISM(ism, false);
        }
        sendDown(ism);
        lastSent = simTime();
    }
}

