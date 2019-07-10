//
// Generated file, do not edit! Created by nedtool 5.3 from IntersectMessage.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __INTERSECTMESSAGE_M_H
#define __INTERSECTMESSAGE_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0503
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
 
#include "veins/base/utils/Coord.h"
#include "veins/modules/messages/WaveShortMessage_m.h"
// }}

/**
 * Enum generated from <tt>IntersectMessage.msg:9</tt> by nedtool.
 * <pre>
 * enum directionType
 * {
 *     STRAIGHT = 0;
 *     RIGHT = 1;
 *     LEFT = 2;
 * }
 * </pre>
 */
enum directionType {
    STRAIGHT = 0,
    RIGHT = 1,
    LEFT = 2
};

/**
 * Class generated from <tt>IntersectMessage.msg:15</tt> by nedtool.
 * <pre>
 * packet IntersectMessage extends WaveShortMessage
 * {
 *     string vehicleId;
 *     double senderSpeed;
 *     Coord senderPos;
 *     simtime_t timeSent;
 *     string roadId; //what road the car is currently on
 *     int direction \@enum(directionType); //direction car wants to go
 *     bool passed; //if the car has already passed the intersection or not
 * }
 * </pre>
 */
class IntersectMessage : public ::WaveShortMessage
{
  protected:
    ::omnetpp::opp_string vehicleId;
    double senderSpeed;
    Coord senderPos;
    ::omnetpp::simtime_t timeSent;
    ::omnetpp::opp_string roadId;
    int direction;
    bool passed;

  private:
    void copy(const IntersectMessage& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const IntersectMessage&);

  public:
    IntersectMessage(const char *name=nullptr, short kind=0);
    IntersectMessage(const IntersectMessage& other);
    virtual ~IntersectMessage();
    IntersectMessage& operator=(const IntersectMessage& other);
    virtual IntersectMessage *dup() const override {return new IntersectMessage(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual const char * getVehicleId() const;
    virtual void setVehicleId(const char * vehicleId);
    virtual double getSenderSpeed() const;
    virtual void setSenderSpeed(double senderSpeed);
    virtual Coord& getSenderPos();
    virtual const Coord& getSenderPos() const {return const_cast<IntersectMessage*>(this)->getSenderPos();}
    virtual void setSenderPos(const Coord& senderPos);
    virtual ::omnetpp::simtime_t getTimeSent() const;
    virtual void setTimeSent(::omnetpp::simtime_t timeSent);
    virtual const char * getRoadId() const;
    virtual void setRoadId(const char * roadId);
    virtual int getDirection() const;
    virtual void setDirection(int direction);
    virtual bool getPassed() const;
    virtual void setPassed(bool passed);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const IntersectMessage& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, IntersectMessage& obj) {obj.parsimUnpack(b);}


#endif // ifndef __INTERSECTMESSAGE_M_H

