#ifndef TUTORIALAPPL_H_
#define TUTORIALAPPL_H_

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"
#include "IntersectMessage_m.h"



class TutorialAppl : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
    protected:
        simtime_t lastSent; // the last time car sent a message
        bool canGo; //if the car can go through intersection
        virtual void onISM(IntersectMessage* wsm);
        virtual void handleSelfMsg(cMessage* msg);
        virtual void handleLowerMsg(cMessage *msg);
        int getDirection(std::string currRoad, std::string nextRoad);
        IntersectMessage* populateISM(IntersectMessage *ism, bool passed);
        virtual void handlePositionUpdate(cObject* obj);
};

#endif /* TUTORIALAPPL_H_ */
