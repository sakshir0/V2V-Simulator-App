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
        needToChange = true;
        needToChange2 = true;
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
    if (isRSU && ((ism->getVehicleId())[0] != '\0')) {
        //std::cout << "cri1" << std::endl;
        findHost()->getDisplayString().updateWith("r=16,green");
        //if car has not yet passed intersection update info for RSU, o/w delete info
        if (!(ism->getPassed())) {
            //add message to RSU data
            addData(ism);
        }
        else {
            removeData(ism);
        }
        //every second, calculate who is allowed to go for RSU and send msg
        //start should also have cars send at same sim time as that is how it is set in SUMO
        if ((simTime() - lastSentRSU >= 1) && simTime() > 6.0) {
            std::list<const char*> allowedList = calculateAllowedVehicles();
            RSUMessage* rsm = new RSUMessage();
            rsm->setAllowedVehiclesArraySize(allowedList.size());
            int i = 0;
            for(auto&& allowed: allowedList) {
                rsm->setAllowedVehicles(i, allowed);
                ++i;
            }
            sendDown(rsm);
            lastSentRSU = simTime();
        }
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

//given list of vehicle msgs, figure out who has right of way based on yield to right rules,
//return those people. if all four cars in intersection, break ties with right/left street.
//for use with trying to figure out who gets to go first when multiple people trying to go straight
std::vector<IntersectMessage*> TutorialAppl::yieldToRight(std::vector<IntersectMessage*> vehicles) {
    bool haveSeen1 = false;
    bool haveSeen2 = false;
    bool haveSeen3 = false;
    bool haveSeen4 = false;
    std::string currPriority = "";
    std::vector<IntersectMessage*> returnedVehicles;
    for(auto&& msg: vehicles) {
        std::string roadId = msg->getRoadId();
        //std::cout << roadId << endl;
        //keep track of which roads we have seen
        if (roadId == "1i") {
            haveSeen1 = true;
        }
        else if (roadId == "2i") {
            haveSeen2 = true;
        }
        else if (roadId == "3i") {
            haveSeen3 = true;
        }
        else {
            haveSeen4 = true;
        }
    }

    //if all four cars in intersection, break tie with left/right lane
    if (haveSeen1 && haveSeen2 && haveSeen3 && haveSeen4) {
        //std::cout << "1" << endl;
        currPriority = "1i";
    }
    //priorities for vehicles
    else if (haveSeen2 && !haveSeen4) {
        //std::cout << "2" << endl;
        currPriority = "2i";
    }
    else if (haveSeen1 && !haveSeen3) {
        //std::cout << "3" << endl;
        currPriority = "1i";
    }
    else if (haveSeen3 && !haveSeen2) {
        //std::cout << "4" << endl;
        currPriority = "3i";
    }
    else {
        //std::cout << "5" << endl;
        currPriority = "4i";
    }
    //figure out which vehicles are allowed to go based on which lane got priority
    for(auto&& msg: vehicles) {
        std::string roadId = msg->getRoadId();
        if ((currPriority == "1i" || currPriority == "2i") &&
            (roadId == "1i" || roadId == "2i")) {
            returnedVehicles.push_back(msg);
        }
        else if ((currPriority == "3i" || currPriority == "4i") &&
                 (roadId == "3i" || roadId == "4i")) {
            returnedVehicles.push_back(msg);
        }
    }
    return returnedVehicles;
}

std::list<const char*> TutorialAppl::getVehicleIds(std::vector<IntersectMessage*> vehicles) {
    std::list<const char*> vehicleIds;
    for(auto&& msg: vehicles) {
        const char* vehicleId = msg->getVehicleId();
        vehicleIds.push_back(vehicleId);
    }
    return vehicleIds;
}

//straight over turning and right over left
std::list<const char*> TutorialAppl::priorityCars(std::vector<IntersectMessage*> vehicles) {
    std::vector<IntersectMessage*> allowedVs;
    //if anyone wants to go straight, they get first priority
    for(auto&& msg: vehicles) {
        int direction = msg->getDirection();
        const char * roadId = msg->getRoadId();
        if (direction == STRAIGHT) {
            allowedVs.push_back(msg);
        }
    }


    //if there are multiple people who wanted to go straight, we yield to right
    allowedVs = yieldToRight(allowedVs);
    /*
    for(auto&& msg: allowedVs) {
        std::cout << msg->getVehicleId() << endl;
    }
    std::cout << "allowedVs end" << endl;
     */

    //we can allow anyone who wants to turn right in same direction
    //as people going straight to go

    //if there were cars who wanted to go straight, check for right turns and return
    if (!allowedVs.empty()) {
        for(auto&& msg: vehicles) {
            int direction = msg->getDirection();
            std::string straightRoadId = (allowedVs.front())->getRoadId();
            std::string roadId = msg->getRoadId();
             if ((((straightRoadId == "1i" || straightRoadId == "2i") &&
                 (roadId == "1i" || roadId == "2i")) ||
                ((straightRoadId == "3i" || straightRoadId == "4i") &&
                 (roadId == "3i" || roadId == "4i"))) &&
                direction == RIGHT) {
                allowedVs.push_back(msg);
            }
        }
        return getVehicleIds(allowedVs);
    }


    //no one wants to go straight, allow all right turns to go
    else {
        for(auto&& msg: vehicles) {
            int direction = msg->getDirection();
            if (direction == RIGHT) {
                allowedVs.push_back(msg);
            }
        }
    }
    //with right turns present, can also allow left turns to go
    //if they are not turning onto same lane as right
    if (!allowedVs.empty()) {
        bool is1Or2 = false;
        bool is3Or4 = false;
        for (auto&&msg: allowedVs) {
            std::string roadId = msg->getRoadId();
            if (roadId == "1i" || roadId == "2i") {
                is1Or2 = true;
            }
            else if (roadId == "3i" || roadId == "4i") {
                is3Or4 = true;
            }
        }
        //check if anyone wants to turn left from that lane
        for(auto&& msg: vehicles) {
            int direction = msg->getDirection();
            std::string roadId = msg->getRoadId();
            if (direction == LEFT) {
                if ((roadId == "1i" || roadId == "2i") && !is1Or2) {
                    allowedVs.push_back(msg);
                }
                else if ((roadId == "3i" || roadId == "4i") && !is3Or4) {
                    allowedVs.push_back(msg);
                }
            }
        }
        return getVehicleIds(allowedVs);
    }

    //no one wanted to turn straight or right, check if anyone wants to turn left
    //only one person can go when trying to turn left at a time
    bool haveSeen1 = false;
    bool haveSeen2 = false;
    bool haveSeen3 = false;
    bool haveSeen4 = false;
    for(auto&& msg: vehicles) {
        int direction = msg->getDirection();
        std::string roadId = msg->getRoadId();
        if (direction == LEFT) {
            allowedVs.push_back(msg);
            //will be used to get rid of ties
            if (roadId == "1i") {
                haveSeen1 = true;
            }
            else if (roadId == "2i") {
                haveSeen2 = true;
            }
            else if (roadId == "3i") {
                haveSeen3 = true;
            }
            else {
                haveSeen4 = true;
            }
        }
    }
    std::string currPriority = "";
    //if all four cars in intersection, break tie with left/right lane
    if (haveSeen1 && haveSeen2 && haveSeen3 && haveSeen4) {
        currPriority = "1i";
    }
    //priorities for vehicles
    else if (haveSeen2 && !haveSeen4) {
        currPriority = "2i";
    }
    else if (haveSeen1 && !haveSeen3) {
        currPriority = "1i";
    }
    else if (haveSeen3 && !haveSeen2) {
        currPriority = "3i";
    }
    else {
        currPriority = "4i";
    }
    //figure out which lane got priority for left turns
    std::vector<IntersectMessage*> returnedVehicles;
    for(auto&& msg: allowedVs) {
        std::string roadId = msg->getRoadId();
        if (currPriority == roadId) {
            returnedVehicles.push_back(msg);
        }
    }
    return getVehicleIds(returnedVehicles);
}

//calculates which vehicles are allowed to go through intersection
std::list<const char*> TutorialAppl::calculateAllowedVehicles() {
    std::list<const char*> allowedVehicles;
    //someone has requested to go through intersection
    if (!RSUData.empty()) {
        //get all vehicles who sent message at same time
        auto it = RSUData.begin();
        std::vector<IntersectMessage*> vehicleMsgs;
        //all cars that can go at same time
        simtime_t earliest = (RSUData.front())->getTimeSent();
        while (it != RSUData.end() && ((*it)->getTimeSent() - earliest) < 0.01) {
            vehicleMsgs.push_back((*it));
            it++;
        }
        allowedVehicles = priorityCars(vehicleMsgs);
        return allowedVehicles;
    }
    //no one wants to go, return empty list
    else {
        return allowedVehicles;
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

//this fxn does not work correctly and is probably not useful
bool TutorialAppl::passedIntersection() {
    Coord juncPos = traci->junction("0").getPosition();
    Coord prevPos = mobility->getPositionAt(simTime() - 1);
    Coord pos = mobility->getPositionAt(simTime());
    std::string roadId = traciVehicle->getRoadId();

    if (((pos.x-juncPos.x > 2.0) && prevPos.x < juncPos.x) ||
        ((pos.x-juncPos.x < -2.0) && prevPos.x > juncPos.x) ||
        ((pos.y-juncPos.y > 2.0) && prevPos.y < juncPos.y) ||
        ((pos.y-juncPos.y < -2.0) && prevPos.y > juncPos.y)) {
        return true;
    }
    else {
        return false;
    }
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
        if (!canGo && distance <= 60.0) {
            traciVehicle->slowDown(0, 500);
        }
        /*
        if (distance<=90 && !canGo &&needToChange2) {
            std::cout << "cri" << endl;
            traciVehicle->stopAt(traciVehicle->getRoadId(),70,
                                     traciVehicle->getLaneIndex(),50,0);
            needToChange2 = false;
        }
        */
        //otherwise they will go through intersection
        else if (canGo && needToChange) {
            //std::cout << "cri" << endl;
            traciVehicle->slowDown(25, 500);
            needToChange = false;
        }

        //create new msg to RSU and send it
        IntersectMessage* ism = new IntersectMessage();
        //if they have passed intersection they will let RSU know

        if(roadId == "1o" || roadId == "2o" ||
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




