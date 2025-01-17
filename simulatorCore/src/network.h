/*
 * network.h
 *
 *  Created on: 24 mars 2013
 *      Author: dom
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <deque>
#include <string.h>

#include "tDefs.h"
#include "rate.h"
#include "buildingBlock.h"

using namespace std;

class Message;
class WirelessMessage;
class NetworkInterface;
class P2PNetworkInterface;
class WirelessNetworkInterface;

typedef std::shared_ptr<Message> MessagePtr;
typedef std::shared_ptr<WirelessMessage> WirelessMessagePtr;

#ifdef DEBUG_MESSAGES
#define MESSAGE_CONSTRUCTOR_INFO()			(cout << getMessageName() << " constructor (" << id << ")" << endl)
#define MESSAGE_DESTRUCTOR_INFO()			(cout << getMessageName() << " destructor (" << id << ")" << endl)
#else
#define MESSAGE_CONSTRUCTOR_INFO()
#define MESSAGE_DESTRUCTOR_INFO()
#endif

//===========================================================================================================
//
//          Message  (class)
//
//===========================================================================================================

class Message {
protected:
	static uint64_t nextId;
	static uint64_t nbMessages;
public:
	uint64_t id;
	unsigned int type;
	P2PNetworkInterface *sourceInterface, *destinationInterface;

	Message();
	virtual ~Message();

	static uint64_t getNbMessages();
	virtual string getMessageName();

	virtual unsigned int size() { return(4); }
	virtual Message* clone();
};

template <class T>
class MessageOf:public Message {
    T *ptrData;
    public :
    MessageOf(int t,const T &data):Message() { type=t; ptrData = new T(data);};
    ~MessageOf() { delete ptrData; };
    T* getData() const { return ptrData; };
    virtual Message* clone() {
        MessageOf<T> *ptr = new MessageOf<T>(type,*ptrData);
        ptr->sourceInterface = sourceInterface;
        ptr->destinationInterface = destinationInterface;
        return ptr;
    }
};

//===========================================================================================================
//
//          P2PMessage  (class)
//
//===========================================================================================================

/*
class P2PMessage {
public :
	P2PNetworkInterface *sourceInterface, *destinationInterface;
	virtual Message* clone();
};

template <class T>
class P2PMessageOf:public P2PMessage {
    T *ptrData;
    public :
    MessageOf(int t,const T &data):Message() { type=t; ptrData = new T(data);};
    ~MessageOf() { delete ptrData; };
    T* getData() const { return ptrData; };
    virtual Message* clone() {
        MessageOf<T> *ptr = new MessageOf<T>(type,*ptrData);
        ptr->sourceInterface = sourceInterface;
        ptr->destinationInterface = destinationInterface;
        return ptr;
    }
};
*/

//===========================================================================================================
//
//          WirelessMessage  (class)
//
//===========================================================================================================

class WirelessMessage {
protected:
    static uint64_t nextId;
    static uint64_t nbMessages;
public:
    uint64_t id;
    unsigned int type;

    WirelessNetworkInterface *sourceInterface;
    bID destinationId;
   
    WirelessMessage(bID destId);
    virtual ~WirelessMessage();
    
    static uint64_t getNbMessages();
    virtual string getMessageName();
    
    virtual unsigned int size() { return(4); }
    virtual WirelessMessage* clone();
};

template <class T>
class WirelessMessageOf:public WirelessMessage {
    T *ptrData;
    public :
    WirelessMessageOf(int t,const T &data, bID destId):WirelessMessage(destId) { type=t; ptrData = new T(data);};
    ~WirelessMessageOf() { delete ptrData; };
    T* getData() const { return ptrData; };
    virtual WirelessMessage* clone() {
        WirelessMessageOf<T> *ptr = new WirelessMessageOf<T>(type,*ptrData, destinationId);
        ptr->sourceInterface = sourceInterface;
        return ptr;
    }
};

//===========================================================================================================
//
//	    NetworkInterface  (class)
//
//===========================================================================================================
class NetworkInterface {
protected :
	static unsigned int nextId;
	static int defaultDataRate;

	BaseSimulator::Rate* dataRate;
public:
	unsigned int globalId;
	unsigned int localId;
	BaseSimulator::BuildingBlock * hostBlock;
	Time availabilityDate;

	NetworkInterface(BaseSimulator::BuildingBlock *b);
	virtual ~NetworkInterface() = 0;
	virtual void send() = 0;
};


//===========================================================================================================
//
//          P2PNetworkInterface  (class)
//
//===========================================================================================================

class P2PNetworkInterface : public NetworkInterface {
public:
	MessagePtr messageBeingTransmitted;
	deque<MessagePtr> outgoingQueue;
	P2PNetworkInterface *connectedInterface;
	P2PNetworkInterface(BaseSimulator::BuildingBlock *b);
	~P2PNetworkInterface();
	
	void send(Message *m);
	
	bool addToOutgoingBuffer(MessagePtr msg);
	virtual void send();
	void connect(P2PNetworkInterface *ni);
	int getConnectedBlockId() {
        return (connectedInterface!=NULL && connectedInterface->hostBlock!=NULL)?connectedInterface->hostBlock->blockId:-1;
	}

	bool isConnected();
	
	void setDataRate(BaseSimulator::Rate* r); 
	Time getTransmissionDuration(MessagePtr &m);

};

//==========================================================================================================
//
//	WirelessNetworkInterface  (class)
//
//==========================================================================================================

class WirelessNetworkInterface : public NetworkInterface {
protected:
    float transmitPower;
    float receptionThreshold;
    float receptionSensitivity;
    bool collisionOccuring;
    bool transmitting;
    bool receiving;
    bool channelAvailability;
    WirelessMessagePtr messageBeingReceived;
public:
    bool first;
    WirelessMessagePtr messageBeingTransmitted;
    deque<WirelessMessagePtr> outgoingQueue;
    WirelessNetworkInterface(BaseSimulator::BuildingBlock *b, float power, float threshold, float sensitivity);
	~WirelessNetworkInterface();
    
    void setReceptionThreshold(float threshold);
    float getReceptionThreshold();
    
    bool addToOutgoingBuffer(WirelessMessagePtr msg);
    virtual void send();
    void startReceive(WirelessMessagePtr msg);
    void stopReceive();
    void setTransmitPower(int power);
    void setAvailability(bool availability) { channelAvailability = availability;};
    bool getAvailability(){return channelAvailability;};
    float getTransmitPower();
    float pathLoss(float power, float distance, float gain, float tHeight, float rHeight); 
    float shadowing(float exponent, float distance, float deviation);
    Time getTransmissionDuration(WirelessMessagePtr &m);
    bool isTransmitting(){return transmitting;};
    bool isReceiving(){return receiving;};
};
#endif /* NETWORK_H_ */
