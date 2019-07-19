//
// Generated file, do not edit! Created by nedtool 5.3 from IntersectMessage.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "IntersectMessage_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp


// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

EXECUTE_ON_STARTUP(
    omnetpp::cEnum *e = omnetpp::cEnum::find("directionType");
    if (!e) omnetpp::enums.getInstance()->add(e = new omnetpp::cEnum("directionType"));
    e->insert(STRAIGHT, "STRAIGHT");
    e->insert(RIGHT, "RIGHT");
    e->insert(LEFT, "LEFT");
)

Register_Class(IntersectMessage)

IntersectMessage::IntersectMessage(const char *name, short kind) : ::WaveShortMessage(name,kind)
{
    this->senderSpeed = 0;
    this->timeSent = 0;
    this->direction = 0;
    this->passed = false;
}

IntersectMessage::IntersectMessage(const IntersectMessage& other) : ::WaveShortMessage(other)
{
    copy(other);
}

IntersectMessage::~IntersectMessage()
{
}

IntersectMessage& IntersectMessage::operator=(const IntersectMessage& other)
{
    if (this==&other) return *this;
    ::WaveShortMessage::operator=(other);
    copy(other);
    return *this;
}

void IntersectMessage::copy(const IntersectMessage& other)
{
    this->vehicleId = other.vehicleId;
    this->vehicleType = other.vehicleType;
    this->senderSpeed = other.senderSpeed;
    this->senderPos = other.senderPos;
    this->timeSent = other.timeSent;
    this->roadId = other.roadId;
    this->direction = other.direction;
    this->passed = other.passed;
}

void IntersectMessage::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::WaveShortMessage::parsimPack(b);
    doParsimPacking(b,this->vehicleId);
    doParsimPacking(b,this->vehicleType);
    doParsimPacking(b,this->senderSpeed);
    doParsimPacking(b,this->senderPos);
    doParsimPacking(b,this->timeSent);
    doParsimPacking(b,this->roadId);
    doParsimPacking(b,this->direction);
    doParsimPacking(b,this->passed);
}

void IntersectMessage::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::WaveShortMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->vehicleId);
    doParsimUnpacking(b,this->vehicleType);
    doParsimUnpacking(b,this->senderSpeed);
    doParsimUnpacking(b,this->senderPos);
    doParsimUnpacking(b,this->timeSent);
    doParsimUnpacking(b,this->roadId);
    doParsimUnpacking(b,this->direction);
    doParsimUnpacking(b,this->passed);
}

const char * IntersectMessage::getVehicleId() const
{
    return this->vehicleId.c_str();
}

void IntersectMessage::setVehicleId(const char * vehicleId)
{
    this->vehicleId = vehicleId;
}

const char * IntersectMessage::getVehicleType() const
{
    return this->vehicleType.c_str();
}

void IntersectMessage::setVehicleType(const char * vehicleType)
{
    this->vehicleType = vehicleType;
}

double IntersectMessage::getSenderSpeed() const
{
    return this->senderSpeed;
}

void IntersectMessage::setSenderSpeed(double senderSpeed)
{
    this->senderSpeed = senderSpeed;
}

Coord& IntersectMessage::getSenderPos()
{
    return this->senderPos;
}

void IntersectMessage::setSenderPos(const Coord& senderPos)
{
    this->senderPos = senderPos;
}

::omnetpp::simtime_t IntersectMessage::getTimeSent() const
{
    return this->timeSent;
}

void IntersectMessage::setTimeSent(::omnetpp::simtime_t timeSent)
{
    this->timeSent = timeSent;
}

const char * IntersectMessage::getRoadId() const
{
    return this->roadId.c_str();
}

void IntersectMessage::setRoadId(const char * roadId)
{
    this->roadId = roadId;
}

int IntersectMessage::getDirection() const
{
    return this->direction;
}

void IntersectMessage::setDirection(int direction)
{
    this->direction = direction;
}

bool IntersectMessage::getPassed() const
{
    return this->passed;
}

void IntersectMessage::setPassed(bool passed)
{
    this->passed = passed;
}

class IntersectMessageDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    IntersectMessageDescriptor();
    virtual ~IntersectMessageDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(IntersectMessageDescriptor)

IntersectMessageDescriptor::IntersectMessageDescriptor() : omnetpp::cClassDescriptor("IntersectMessage", "WaveShortMessage")
{
    propertynames = nullptr;
}

IntersectMessageDescriptor::~IntersectMessageDescriptor()
{
    delete[] propertynames;
}

bool IntersectMessageDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<IntersectMessage *>(obj)!=nullptr;
}

const char **IntersectMessageDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *IntersectMessageDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int IntersectMessageDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 8+basedesc->getFieldCount() : 8;
}

unsigned int IntersectMessageDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<8) ? fieldTypeFlags[field] : 0;
}

const char *IntersectMessageDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "vehicleId",
        "vehicleType",
        "senderSpeed",
        "senderPos",
        "timeSent",
        "roadId",
        "direction",
        "passed",
    };
    return (field>=0 && field<8) ? fieldNames[field] : nullptr;
}

int IntersectMessageDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='v' && strcmp(fieldName, "vehicleId")==0) return base+0;
    if (fieldName[0]=='v' && strcmp(fieldName, "vehicleType")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderSpeed")==0) return base+2;
    if (fieldName[0]=='s' && strcmp(fieldName, "senderPos")==0) return base+3;
    if (fieldName[0]=='t' && strcmp(fieldName, "timeSent")==0) return base+4;
    if (fieldName[0]=='r' && strcmp(fieldName, "roadId")==0) return base+5;
    if (fieldName[0]=='d' && strcmp(fieldName, "direction")==0) return base+6;
    if (fieldName[0]=='p' && strcmp(fieldName, "passed")==0) return base+7;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *IntersectMessageDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "string",
        "string",
        "double",
        "Coord",
        "simtime_t",
        "string",
        "int",
        "bool",
    };
    return (field>=0 && field<8) ? fieldTypeStrings[field] : nullptr;
}

const char **IntersectMessageDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 6: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *IntersectMessageDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 6:
            if (!strcmp(propertyname,"enum")) return "directionType";
            return nullptr;
        default: return nullptr;
    }
}

int IntersectMessageDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    IntersectMessage *pp = (IntersectMessage *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *IntersectMessageDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    IntersectMessage *pp = (IntersectMessage *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string IntersectMessageDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    IntersectMessage *pp = (IntersectMessage *)object; (void)pp;
    switch (field) {
        case 0: return oppstring2string(pp->getVehicleId());
        case 1: return oppstring2string(pp->getVehicleType());
        case 2: return double2string(pp->getSenderSpeed());
        case 3: {std::stringstream out; out << pp->getSenderPos(); return out.str();}
        case 4: return simtime2string(pp->getTimeSent());
        case 5: return oppstring2string(pp->getRoadId());
        case 6: return enum2string(pp->getDirection(), "directionType");
        case 7: return bool2string(pp->getPassed());
        default: return "";
    }
}

bool IntersectMessageDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    IntersectMessage *pp = (IntersectMessage *)object; (void)pp;
    switch (field) {
        case 0: pp->setVehicleId((value)); return true;
        case 1: pp->setVehicleType((value)); return true;
        case 2: pp->setSenderSpeed(string2double(value)); return true;
        case 4: pp->setTimeSent(string2simtime(value)); return true;
        case 5: pp->setRoadId((value)); return true;
        case 6: pp->setDirection((directionType)string2enum(value, "directionType")); return true;
        case 7: pp->setPassed(string2bool(value)); return true;
        default: return false;
    }
}

const char *IntersectMessageDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 3: return omnetpp::opp_typename(typeid(Coord));
        default: return nullptr;
    };
}

void *IntersectMessageDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    IntersectMessage *pp = (IntersectMessage *)object; (void)pp;
    switch (field) {
        case 3: return (void *)(&pp->getSenderPos()); break;
        default: return nullptr;
    }
}


