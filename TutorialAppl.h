#ifndef TUTORIALAPPL_H_
#define TUTORIALAPPL_H_

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "IntersectMessage_m.h"
#include "RSUMessage_m.h"

//for debugging
#include <iostream>

class TutorialAppl : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
    protected:
        ~TutorialAppl();
        //RSU
        simtime_t lastSentRSU; //last time RSU sent msg
        std::vector<IntersectMessage*> RSUData;
        virtual void onISM(IntersectMessage* wsm);
        virtual void addData(IntersectMessage* ism);
        virtual void removeData(IntersectMessage* ism);
        std::vector<IntersectMessage*> yieldToRight(std::vector<IntersectMessage*> vehicles);
        std::list<const char*> getVehicleIds(std::vector<IntersectMessage*> vehicles);
        std::list<const char*> priorityCars(std::vector<IntersectMessage*> vehicles);
        std::list<const char*> calculateAllowedVehicles();

        //Vehicles
        bool canGo; //if the car can go through intersection
        virtual void onRSM(RSUMessage *rsm);
        int getDirection(std::string currRoad, std::string nextRoad);
        void populateISM(IntersectMessage *ism, bool passed);
        virtual void handlePositionUpdate(cObject* obj);

        //General
        simtime_t lastSent; // the last time car sent a message
        bool isRSU; //if it is an RSU or a car
        virtual void handleSelfMsg(cMessage* msg);
        virtual void handleLowerMsg(cMessage *msg);
};

#endif /* TUTORIALAPPL_H_ */
