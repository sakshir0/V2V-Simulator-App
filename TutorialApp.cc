#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "veins/base/utils/Coord.h"
#include "TutorialAppl.h"


Define_Module(TutorialAppl);

void TutorialAppl::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        lastSent = simTime();
        canGo = false;
        //figuring out if it is an RSU
        std::string path = getFullPath();
        if (path.find("rsu") != std::string::npos) {
            isRSU = true;
        }
        else {
            isRSU = false;
        }
        if (dataOnSch) {
            //started service and server advertising, schedule message to self to send later
            startService(Channels::SCH2, 42, "Traffic Information Service");
        }
    }
}

TutorialAppl::~TutorialAppl() {
    for (auto it = RSUData.begin(); it != RSUData.end(); ++it) {
        delete(*it);
    }
    RSUData.clear();
}

/*
 * RSU functions
 */

void TutorialAppl::onISM(IntersectMessage* ism) {
    if (isRSU && (ism->getVehicleId())[0] != '\0') {
        //std::cout << ism->getVehicleId() << std::endl;
        findHost()->getDisplayString().updateWith("r=16,green");
        //if car has not yet passed intersection update info for RSU, o/w delete info
        if (!(ism->getPassed())) {
            //add message to RSU data
            addData(ism);
        }
        else {
            removeData(ism);
        }

        /*
        std::cout << "begin" << endl;
                for (auto const& j : RSUData) {
                    std::cout << j->getVehicleId() << endl;
                    std::cout << j->getTimeSent() << endl;
            }
        std::cout << "end" << endl;
        */

        //send message from RSU back to cars
        std::list<const char*> allowedList;
        std::list<const char*>::iterator it;
        it = allowedList.begin();
        //can't do this, remember char * get fucked up
        const char* str1 = "straightFromRight0";
        allowedList.insert (it,str1);

        RSUMessage* rsm = new RSUMessage();
        rsm->setAllowedVehiclesArraySize(allowedList.size());
        rsm->setAllowedVehicles(0, allowedList.front());
        sendDown(rsm);
        //std::cout << "1" << endl;
    }
}

bool greater(IntersectMessage* ism1, IntersectMessage* ism2) {
    return (ism1->getTimeSent() < ism2->getTimeSent());
}
//if no msg from that vehicle, creates new element in list. otherwise updates
void TutorialAppl::addData(IntersectMessage* ism) {
    IntersectMessage *ism2 = new IntersectMessage();
    ism2 = ism->dup();
    std::string vehicleId = ism2->getVehicleId();

    auto it = RSUData.begin();
    while (it != RSUData.end()) {
        std::string storedVehicleId = (*it)->getVehicleId();
        if (storedVehicleId == vehicleId) {
            simtime_t timeSent = (*it)->getTimeSent();
            delete(*it);
            it = RSUData.erase(it);
            //time sent remains the first time the msg was sent
            ism2->setTimeSent(timeSent);
        }
        else {
            ++it;
        }
    }
    RSUData.push_back(ism2);
    //order RSUData by when the car sent the msg
    std::sort (RSUData.begin(), RSUData.end(), greater);
}

//will remove message pertaining to a particular vehicle ID
void TutorialAppl::removeData(IntersectMessage* ism) {
    std::string vehicleId = ism->getVehicleId();
    auto it = RSUData.begin();
    while (it != RSUData.end()) {
        if ((*it)->getVehicleId() == vehicleId) {
            delete(*it);
            it = RSUData.erase(it);
        }
        else {
            ++it;
        }
    }
}

//calculates which vehicles are allowed to go through intersection
std::list<std::string> TutorialAppl::calculateAllowedVehicles() {
    if (isRSU) {

    }
}

/*
 * Car functions
 */

void TutorialAppl::onRSM(RSUMessage *rsm) {
    if (!isRSU) {
        findHost()->getDisplayString().updateWith("r=16,blue");

        std::string vehicleId = mobility->getExternalId().c_str();
        for (int i=0; i < rsm->getAllowedVehiclesArraySize(); ++i) {
            if (rsm->getAllowedVehicles(i) == vehicleId) {
                canGo = true;
            }
        }
    }
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

void TutorialAppl::populateISM(IntersectMessage *ism, bool passed) {
    BaseWaveApplLayer::populateWSM(ism);
    //get fields for ism
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
    ism->setVehicleId(mobility->getExternalId().c_str());
    ism->setTimeSent(simTime());
    ism->setSenderSpeed(speed);
    ism->setSenderPos(pos);
    ism->setRoadId(mobility->getRoadId().c_str());
    ism->setDirection(direction);
    ism->setPassed(passed);
}

void TutorialAppl::handlePositionUpdate(cObject* obj) {
    BaseWaveApplLayer::handlePositionUpdate(obj);
    //cars send messages every 1 second
    if (simTime() - lastSent >= 1 && !isRSU) {
        //get distance between car and junction
        Coord juncPos = traci->junction("0").getPosition();
        Coord pos = mobility->getPositionAt(simTime());
        double distance = pos.distance(juncPos);
        std::string roadId = traciVehicle->getRoadId();
        //if they cannot go through intersection, will slow down
        if (distance <= 20.0 && !canGo) {
            traciVehicle->slowDown(0, 1000);
        }
        //otherwise they will go through intersection
        else if (canGo) {
            traciVehicle->slowDown(20, 500);
        }

        //create new msg to RSU and send it
        IntersectMessage* ism = new IntersectMessage();
        //if they have passed intersection they will let RSU know
        if (roadId == "1o" || roadId == "2o" ||
            roadId == "3o" || roadId == "4o") {
            populateISM(ism, true);
        }
        else if (roadId == "1i" || roadId == "2i" ||
                 roadId == "3i" || roadId == "4i") {
            populateISM(ism, false);
        }

        if (dataOnSch) {
            //schedule message to self to send later
            scheduleAt(computeAsynchronousSendingTime(1,type_SCH),ism);
        }
        else {
            //send right away on CCH, because channel switching is disabled
            sendDown(ism);
        }
        lastSent = simTime();
    }
}

/*
 * General functions
 */
void TutorialAppl::handleSelfMsg(cMessage* msg) {
    WaveShortMessage* wsm = dynamic_cast<WaveShortMessage*>(msg);
    if (IntersectMessage* ism = dynamic_cast<IntersectMessage*>(wsm)) {
        sendDown(ism->dup());
        delete(ism);
    }
    else if (RSUMessage* rsm = dynamic_cast<RSUMessage*>(wsm)) {
        sendDown(rsm->dup());
        delete(rsm);
    }
    else {
        BaseWaveApplLayer::handleSelfMsg(msg);
    }
}

void TutorialAppl::handleLowerMsg(cMessage* msg) {
    //std::cout << "handle  lower msg" << std::endl;
    WaveShortMessage* wsm = dynamic_cast<WaveShortMessage*>(msg);
    if (IntersectMessage* ism = dynamic_cast<IntersectMessage*>(wsm)) {
        //std::cout << "is an ism" << std::endl;
        onISM(ism);
    }
    else if (RSUMessage* rsm = dynamic_cast<RSUMessage*>(wsm)) {
        //std::cout << "is an rsm" << std::endl;
        onRSM(rsm);
    }
    delete(msg);
}




