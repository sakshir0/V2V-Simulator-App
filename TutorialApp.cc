#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "veins/base/utils/Coord.h"
#include "TutorialAppl.h"

Define_Module(TutorialAppl);

/*
 * Initialization and completion functions
 */

void TutorialAppl::initialize(int stage) {
    /*
     * This function initializes all variables. It sets the lastSent time to the
     * beginning of the simulation time. It also figures out if we are an RSU or a car
     * and sets isRSU accordingly. All cars are default not allowed to go through
     * the intersection so canGo is false. This function starts the service channel.
     * It also uses all the initialization from the base wave application layer.
     */
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        lastSent = simTime();
        //By default, you are not allowed to go through intersection
        canGo = false;
        //Figuring out if you are an RSU
        std::string path = getFullPath();
        if (path.find("rsu") != std::string::npos) {
            isRSU = true;
        }
        else {
            isRSU = false;
        }
        //Starts channels
        if (dataOnSch) {
            startService(Channels::SCH2, 42, "Traffic Information Service");
        }
    }
}

TutorialAppl::~TutorialAppl() {
    /*
     * This function goes through RSUData and deletes all
     * remaining IntersectMessage*. It then clears the vector.
     */
    for (auto it = RSUData.begin(); it != RSUData.end(); ++it) {
        delete(*it);
    }
    RSUData.clear();
}

/*
 * RSU functions
 */

void TutorialAppl::onISM(IntersectMessage* ism) {
    /*
     * This function receives a message from a car, called an IntersectMessage.
     * It will cause the RSU to display a green circle around it. If the car has not
     * yet passed the intersection, it will add the car's message/update it in RSUData.
     * If the car has passed, it will delete the car's message from RSUData. It will
     * then calculate the vehicles allowed to go through the intersection based on the
     * data it has. It uses regular intersection rules to determine who is allowed to go.
     * It will send an RSUMessage with a list of the vehicleId's that are allowed to go
     * to all cars.
     */

    /* At the beginning of the simulation, lots of empty vehicle ID's get sent. We want
     * to make sure it is a vehicle we defined so we check the ID is not empty.
     * We also only want RSU to evaluate ISM msgs.
     */
    if (isRSU && ((ism->getVehicleId())[0] != '\0')) {
        //When the RSU receives a message, it will display a green circle around it
        findHost()->getDisplayString().updateWith("r=16,green");

        //If car has not yet passed intersection update info for RSU, o/w delete info
        if (!(ism->getPassed())) {
            addData(ism);
        }
        else {
            removeData(ism);
        }

        /* Every second, RSU calculates which cars are allowed to go and sends message
         * We wait a few seconds into the simulation because SUMO and OMNeT are not perfectly
         * in sync at the very beginning and so even if in SUMO, we defined two cars to
         * start at the same simulation time in SUMO, they will not send a message at the same
         * time. We want the behavior of SUMO and OMNeT to always match.
         */
        //
        if ((simTime() - lastSentRSU >= 1) && simTime() > 6.0) {
            //Calculates vehicles allowed to go
            std::list<const char*> allowedList = calculateAllowedVehicles();
            //Creates and populates RSU message
            RSUMessage* rsm = new RSUMessage();
            rsm->setAllowedVehiclesArraySize(allowedList.size());
            int i = 0;
            for(auto&& allowed: allowedList) {
                rsm->setAllowedVehicles(i, allowed);
                ++i;
            }
            //Sends RSU message with specified delay
            sendDelayedDown(rsm, delayTimeRSU);
            lastSentRSU = simTime();
        }
    }
}


bool greater(IntersectMessage* ism1, IntersectMessage* ism2) {
    /*
     * This function returns true if ism2 was sent later than ism1
     * and false otherwise.
     */
    return (ism1->getTimeSent() < ism2->getTimeSent());
}
void TutorialAppl::addData(IntersectMessage* ism) {
    /*
     * The RSU will make a copy of the IntersectMessage as the original will get
     * deleted. If there is already a message in the list corresponding to the
     * vehicle ID, it will update all the fields in the message except for time
     * sent which remains as the original time sent. If there is no message, it will
     * create a new entry in the list for the new message.
     */

    //Copies intersect message
    IntersectMessage *ism2 = new IntersectMessage();
    ism2 = ism->dup();

    //Loops through data, checking if any of them match the message's vehicle ID
    std::string vehicleId = ism2->getVehicleId();
    auto it = RSUData.begin();
    while (it != RSUData.end()) {
        std::string storedVehicleId = (*it)->getVehicleId();
        //Delete original message, update time sent in ism2 to be original time sent
        if (storedVehicleId == vehicleId) {
            simtime_t timeSent = (*it)->getTimeSent();
            delete(*it);
            it = RSUData.erase(it);
            //Time sent remains the first time the msg was sent
            ism2->setTimeSent(timeSent);
        }
        else {
            ++it;
        }
    }
    //Push back ism
    RSUData.push_back(ism2);
    //Order RSUData by when the car sent the msg
    std::sort (RSUData.begin(), RSUData.end(), greater);
}

void TutorialAppl::removeData(IntersectMessage* ism) {
    /*
     * This function will remove data from the RSUData vector based on the
     * matching vehicleID in the ism. If it cannot find the ism with the
     * vehicle ID corresponding to the ism in the vector, it will do nothing.
     */
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

std::vector<IntersectMessage*> TutorialAppl::yieldToRight(std::vector<IntersectMessage*> vehicles) {
    /*
     * This function takes in a list of vehicle messages and figures out who has the right of way
     * based on the yield to right rule of an intersection. It is meant only for cars who are all
     * going straight at the same time step. It will return the car messages on either the
     * left/right side of the intersection or the up/down side of the intersection based
     * on who gets the right of way. If all four cars are in the intersection, it will break
     * the tie based on the left/right lane.
     */
    bool haveSeen1 = false;
    bool haveSeen2 = false;
    bool haveSeen3 = false;
    bool haveSeen4 = false;
    std::string currPriority = "";
    std::vector<IntersectMessage*> returnedVehicles;

    //Loop through vehicle messages, keep track of which roads we have seen
    for(auto&& msg: vehicles) {
        std::string roadId = msg->getRoadId();
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

    /* Figures out which lane should get priority. If cars on all four sides
     * of intersection, no one has right of way so break tie with
     * left/right lane.
     */
    if (haveSeen1 && haveSeen2 && haveSeen3 && haveSeen4) {
        currPriority = "1i";
    }
    //Vehicle will always yield to one on its right
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

    /* Figure out which vehicles are allowed to go based on which lane got priority.
     * Can let cars on either side of that road go since we assumed the vehicles are all
     * going straight and at the same time step.
     */
    for(auto&& msg: vehicles) {
        std::string roadId = msg->getRoadId();
        //If priority was left/right lanes, let all cars there go
        if ((currPriority == "1i" || currPriority == "2i") &&
            (roadId == "1i" || roadId == "2i")) {
            returnedVehicles.push_back(msg);
        }
        //If priority was up/down lanes, let all cars there go
        else if ((currPriority == "3i" || currPriority == "4i") &&
                 (roadId == "3i" || roadId == "4i")) {
            returnedVehicles.push_back(msg);
        }
    }
    return returnedVehicles;
}

std::vector<IntersectMessage*> TutorialAppl::yieldToRightLeftTurns(std::vector<IntersectMessage*> vehicles) {
    /*
     * This function takes in a list of vehicle messages and figures out who has the right of way
     * based on the yield to right rule of an intersection. It is meant only for cars who are all
     * going left at the same time step. It will return a list of one car that is allowed to go.
     * If all four cars are in the intersection, it will break the tie by allowing the car
     * in lane 1 to go.
     */
    std::string currPriority = "";
    std::vector<IntersectMessage*> returnedVehicles;
    bool haveSeen1 = false;
    bool haveSeen2 = false;
    bool haveSeen3 = false;
    bool haveSeen4 = false;
    for(auto&& msg: vehicles) {
        //Keep track of from which roads vehicles are trying to turn from
       std::string roadId = msg->getRoadId();
       //std::cout << msg->getVehicleId() << endl;
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
    //If all four cars in intersection, break tie with left/right lane
    if (haveSeen1 && haveSeen2 && haveSeen3 && haveSeen4) {
        currPriority = "1i";
    }
    //Figure out who has priority based on intersection rules
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

    //Allow car that got priority to proceed through intersection
    for(auto&& msg: vehicles) {
        std::string roadId = msg->getRoadId();
        if (currPriority == roadId) {
            returnedVehicles.push_back(msg);
        }
    }
    return returnedVehicles;
}

std::list<const char*> TutorialAppl::getVehicleIds(std::vector<IntersectMessage*> vehicles) {
    /*
     * This function loops through a vector of IntersectMessage*'s and returns a list
     * of vehicle IDs corresponding to the vehicle ID's in the messages.
     * If the vector is empty it will return an empty list.
     */
    std::list<const char*> vehicleIds;
    for(auto&& msg: vehicles) {
        const char* vehicleId = msg->getVehicleId();
        vehicleIds.push_back(vehicleId);
    }
    return vehicleIds;
}

std::list<const char*> TutorialAppl::priorityCars(std::vector<IntersectMessage*> vehicles) {
    /*
     * This function figures out for a particular time step, which cars have the right of
     * way in the intersection and returns a list of those vehicle IDs. It decides based on
     * regular intersection rules. It will allow people trying to go straight to go first,
     * it will prioritize right turns over left turns, and it will let the cars on the right
     * of other cars go first. If a particular car has gotten priority, for example someone
     * going straight through the intersection, we also check if we can let anyone else
     * through at that time step who does not conflict with the car who got priority.
     */
    std::vector<IntersectMessage*> allowedVs;
    /* Loop through vehicles to check if there is an emergency vehicle,
     * emergency vehicles get priority over everyone. No one else goes through
     * intersection when they go
     */
    for (auto&&msg: vehicles) {
        std::string vehicleType = msg->getVehicleType();
        if (vehicleType == "emergency") {
            allowedVs.push_back(msg);
        }
    }
    //If there were any emergency vehicles, return. They are the only ones allowed through.
    if (!allowedVs.empty()) return getVehicleIds(allowedVs);

    /*
     * No emergency vehicles, so next check if there is anyone who wants to go straight.
     * They get first priority according to intersection rules.
     */
    for(auto&& msg: vehicles) {
        int direction = msg->getDirection();
        const char * roadId = msg->getRoadId();
        if (direction == STRAIGHT) {
            allowedVs.push_back(msg);
        }
    }
    //If there are multiple people who wanted to go straight, we yield to person on right.
    allowedVs = yieldToRight(allowedVs);

    /*
     * We can allow anyone who wants to turn right in the same direction as the people
     * going straight to go. Thus, if there were cars who wanted to go straight,
     * check for right turns in same direction and then return as no one else can go.
     */
    if (!allowedVs.empty()) {
        for(auto&& msg: vehicles) {
            int direction = msg->getDirection();
            std::string straightRoadId = (allowedVs.front())->getRoadId();
            std::string roadId = msg->getRoadId();
            /*
             * If the direction up/down or left/right of the car trying to turn
             * right matches the direction of the cars we have allowed to go straight
             * we can also allow them to go.
             */

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


    /*
     * If no one wanted to go straight, the next people who get priority are
     * those trying to go right. Since right turns don't conflict with anyone, we
     * can allow everyone trying to go right to go.
     */
    else {
        for(auto&& msg: vehicles) {
            int direction = msg->getDirection();
            if (direction == RIGHT) {
                allowedVs.push_back(msg);
            }
        }
    }
    /*
     * If there were any right turns present, we can also allow left turns to go
     * if they are not going the same direction of the intersection as the right turns.
     */
    if (!allowedVs.empty()) {
        /* Check which directions of the intersections are occupied by cars trying
         * to turn right
         */
        bool is1Or2 = false;
        bool is3Or4 = false;
        for (auto&&msg: allowedVs) {
            std::string roadId = msg->getRoadId();
            //Left/right direction of intersection
            if (roadId == "1i" || roadId == "2i") {
                is1Or2 = true;
            }
            //Up/down direction of intersection
            else if (roadId == "3i" || roadId == "4i") {
                is3Or4 = true;
            }
        }
        std::vector<IntersectMessage*> allowedLefts;
        //Loop through vehicles and check if anyone wants to go left
        for(auto&& msg: vehicles) {
            int direction = msg->getDirection();
            std::string roadId = msg->getRoadId();
            if (direction == LEFT) {
                /*If no one is trying to turn right on that side of intersection,
                 * then left turns can go.
                 */
                if ((roadId == "1i" || roadId == "2i") && !is1Or2) {
                    allowedLefts.push_back(msg);
                }
                else if ((roadId == "3i" || roadId == "4i") && !is3Or4) {
                    allowedLefts.push_back(msg);
                }
            }
        }

        //Only one left turn on that side of the intersection can go, so we must yield.
        allowedLefts = yieldToRightLeftTurns(allowedLefts);
        //Only one left turn will be allowed to go at any time, so we can just get start of list.
        if (!allowedLefts.empty()) {
            allowedVs.push_back(allowedLefts.front());
        }
        return getVehicleIds(allowedVs);
    }

    /*
     * If no one wanted to turn straight or right, then the last person who gets priority
     * are left turns. We check if anyone wants to turn left and allow one left turn to go.
     */
    for(auto&& msg: vehicles) {
        int direction = msg->getDirection();
        if (direction == LEFT) {
            allowedVs.push_back(msg);
        }
    }
    //We let the left turn who gets right of way based on the rule: right before left.
    allowedVs = yieldToRightLeftTurns(allowedVs);
    return getVehicleIds(allowedVs);
}

std::list<const char*> TutorialAppl::calculateAllowedVehicles() {
    /*
     * This function calculates which vehicles are allowed to go through the
     * intersection according to the data the RSU has at that moment. It returns
     * a list of vehicle ID's of the vehicles that are allowed to go. If no one
     * wants to go through the intersection, it will return an empty list. The way
     * it figures out who is allowed to go through the intersection is based on
     * regular intersection rules. It will first get all the vehicles who sent their messages
     * at the earliest time out of any of the messages. Then it will figure out
     * which vehicles are allowed to go at that time step based on straight before
     * turning, right turns over left turns, and right before left as its priorities.
     */
    std::list<const char*> allowedVehicles;
    //Someone has requested to go through intersection
    if (!RSUData.empty()) {
        //Get all vehicles who sent message at same time
        auto it = RSUData.begin();
        std::vector<IntersectMessage*> vehicleMsgs;
        simtime_t earliest = (RSUData.front())->getTimeSent();
        while (it != RSUData.end() && ((*it)->getTimeSent() - earliest) < 0.01) {
            vehicleMsgs.push_back((*it));
            it++;
        }
        //Calculate out of the vehicles who sent a message at the same time, who gets priority
        allowedVehicles = priorityCars(vehicleMsgs);
        return allowedVehicles;
    }
    //No one wants to go, return empty list
    else {
        return allowedVehicles;
    }
}

/*
 * Car functions
 */

void TutorialAppl::onRSM(RSUMessage *rsm) {
    /*
     * This function handles what to do when a car receives a message from
     * the RSU. The car will loop through all the vehicles that are allowed
     * to go through the intersection according to the RSU. If they are are
     * allowed to go, they will set their canGo boolean variable to True.
     * Otherwise, nothing will happen, as the variable is by default False.
     * The car will also display a blue circle around it when it receives a message
     * from the RSU.
     */
    if (!isRSU) {
        findHost()->getDisplayString().updateWith("r=16,blue");
        /* If the vehicle's ID matches one of the allowed to go vehicles,
         * it sets canGo to true
         */
        std::string vehicleId = mobility->getExternalId().c_str();
        for (int i=0; i < rsm->getAllowedVehiclesArraySize(); ++i) {
            if (rsm->getAllowedVehicles(i) == vehicleId) {
                canGo = true;
            }
        }
    }
}

int TutorialAppl::getDirection(std::string currRoad, std::string nextRoad) {
    /*
     * This function takes in the roadId of the current road the vehicle is on
     * and the next road it is trying to go on and computes which direction it is
     * trying to go. Directions are defined as an enumeration in the
     * IntersectMessage.msg. Straight is 0, right is 1, and left is 2. Check the
     * documentation to see a diagram of the intersection with labeled roadId's or
     * check the defined SUMO simulation nodes/connections to understand the layout
     * of the intersection.
     */
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
    /*
     * This function takes in an IntersectMessage* and whether or not the
     * car has passed the intersection. It will populate all the fields of
     * the ism, including the vehicle ID, the vehicle type, the time the message
     * is sent, the road ID, the direction the car wants to go, and whether or not
     * it has passed the intersection.
     */
    BaseWaveApplLayer::populateWSM(ism);

    //Gets 2nd and 3rd position in car route to figure out next road
    std::list<std::string> plannedRoadIds = traciVehicle->getPlannedRoadIds();
    std::list<std::string>::iterator it = plannedRoadIds.begin();
    std::advance(it, 1);
    std::string prevRoad = *it;
    std::advance(it, 1);

    //Gets direction car is trying to turn in intersection
    int direction = getDirection(prevRoad, *it);

    //Sets fields in message
    ism->setVehicleId(mobility->getExternalId().c_str());
    ism->setVehicleType(traciVehicle->getTypeId().c_str());
    ism->setTimeSent(simTime());
    ism->setRoadId(mobility->getRoadId().c_str());
    ism->setDirection(direction);
    ism->setPassed(passed);
}


void TutorialAppl::handlePositionUpdate(cObject* obj) {
    /*
     * This function gets called whenever a car moves. If a car is allowed to
     * go through the intersection, it will speed up and go. Otherwise, if it
     * getting close to the intersection, it will slow down and eventually stop.
     * The cars also create, populate, and send messages to the RSU in this function.
     * These messages indicate where the car wants to go, when it sent the message,
     * whether or not it has passed the junction, and other necessary fields.
     */
    BaseWaveApplLayer::handlePositionUpdate(obj);

    //Cars send messages every 1 second
    if (simTime() - lastSent >= 1 && !isRSU) {
        //Get distance between car and junction
        Coord juncPos = traci->junction("0").getPosition();
        Coord pos = mobility->getPositionAt(simTime());
        double distance = pos.distance(juncPos);
        std::string roadId = traciVehicle->getRoadId();

        /* If car cannot go through intersection and it is getting close to junction,
         * it will stop
         */
        if (!canGo && distance <= 60.0) {
            traciVehicle->slowDown(0, 500);
        }
        //Otherwise car is allowed to go through intersection and will go
        else if (canGo) {
            traciVehicle->slowDown(25, 500);
        }

        //Car will create message to send to RSU
        IntersectMessage* ism = new IntersectMessage();
        /*
         * If the car has passed through the intersection, it will
         * populate a message to the RSU with bool set to True.
         * These road IDs indicate the car has passed through the intersection.
         */
        if(roadId == "1o" || roadId == "2o" ||
           roadId == "3o" || roadId == "4o") {
            populateISM(ism, true);
        }
        /*
         * Car has not passed through intersection, so will populate message to RSU
         * with bool set to False.
         */
        else if (roadId == "1i" || roadId == "2i" ||
                 roadId == "3i" || roadId == "4i") {
            populateISM(ism, false);
        }

        //If there is currently data on the channel, the car cannot send the message right then
        if (dataOnSch) {
            //schedule message to self to send later
            scheduleAt(computeAsynchronousSendingTime(1,type_SCH),ism);
        }
        //Send on CCH, because channel switching is disabled
        else {
            //Sends with specified delay in message
            simtime_t delayTimeCars = par("delayTimeCars");
            sendDelayedDown(ism, delayTimeCars);
        }
        lastSent = simTime();
    }
}

/*
 * General functions
 */

void TutorialAppl::handleSelfMsg(cMessage* msg) {
    /*
     * This a function inherited from the baseWaveApplLayer that handles a self message.
     * We simply do the same thing that layer of the application does in the handleSelfMsg
     * function, just with our new kinds of messages. If it is an IntersectMessage or
     * an RSUMessage, we will send a copy of the message down to the network layer
     * and then delete the message. Otherwise, we let the BaseWaveApplLayer handle
     * the message.
     */
    WaveShortMessage* wsm = dynamic_cast<WaveShortMessage*>(msg);
    //It is an IntersectMessage from car
    if (IntersectMessage* ism = dynamic_cast<IntersectMessage*>(wsm)) {
        sendDown(ism->dup());
        delete(ism);
    }
    //It is an RSUMessage from RSU
    else if (RSUMessage* rsm = dynamic_cast<RSUMessage*>(wsm)) {
        sendDown(rsm->dup());
        delete(rsm);
    }
    //Otherwise
    else {
        BaseWaveApplLayer::handleSelfMsg(msg);
    }
}

void TutorialAppl::handleLowerMsg(cMessage* msg) {
    /*
     * This is a function inherited from the baseWaveApplLayer that handles a lower
     * message to the network. We simply do the same thing that layer of the application
     * does in the handleLowerMsg function, just with our new kinds of messages. If
     * the network received an IntersectMessage, we call our onISM function. If it received
     * an RSUMessage, we call our on RSM function. We delete the message at the end of the
     * function.
     */
    WaveShortMessage* wsm = dynamic_cast<WaveShortMessage*>(msg);
    if (IntersectMessage* ism = dynamic_cast<IntersectMessage*>(wsm)) {
        onISM(ism);
    }
    else if (RSUMessage* rsm = dynamic_cast<RSUMessage*>(wsm)) {
        onRSM(rsm);
    }
    delete(msg);
}




