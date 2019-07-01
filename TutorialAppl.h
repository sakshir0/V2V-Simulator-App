#ifndef TUTORIALAPPL_H_
#define TUTORIALAPPL_H_

#include "veins/modules/application/ieee80211p/BaseWaveApplLayer.h"


class TutorialAppl : public BaseWaveApplLayer {
    public:
        virtual void initialize(int stage);
    protected:
        simtime_t lastSent; // the last time this sent a message
        virtual void onWSM(WaveShortMessage* wsm);
        virtual void handleSelfMsg(cMessage* msg);
        virtual void handlePositionUpdate(cObject* obj);
};

#endif /* TUTORIALAPPL_H_ */
