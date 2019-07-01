#include <stdlib.h>
#include "TutorialAppl.h"
#include <iostream>
#include <string>

Define_Module(TutorialAppl);

void TutorialAppl::initialize(int stage) {
    BaseWaveApplLayer::initialize(stage);
    if (stage == 0) {
        lastSent = simTime();
    }
}

void TutorialAppl::onWSM(WaveShortMessage* wsm) {
    findHost()->getDisplayString().updateWith("r=16,green");
    std::string id = "Tesla";
    if ((id.compare(traciVehicle->getTypeId())) == 0) {
        traciVehicle->setSpeedMode(0000);
        traciVehicle->setSpeed(0.0);
    }
}

void TutorialAppl::handleSelfMsg(cMessage* msg) {
    if (WaveShortMessage* wsm = dynamic_cast<WaveShortMessage*>(msg)) {
        delete(wsm);
    }
    else {
        BaseWaveApplLayer::handleSelfMsg(msg);
    }
}

void TutorialAppl::handlePositionUpdate(cObject* obj) {
    BaseWaveApplLayer::handlePositionUpdate(obj);
    if (simTime() - lastSent >= 1) {
        WaveShortMessage* wsm =new WaveShortMessage();
        populateWSM(wsm);
        sendDelayedDown(wsm, uniform(100,1000));
        lastSent = simTime();
    }
}
