/*
 * catoms2DEvents.h
 *
 *  Created on: 2014 febrary 1st
 *      Author: Benoît
 */

#ifndef CATOMS2DEVENTS_H_
#define CATOMS2DEVENTS_H_

#include "catoms2DBlock.h"
#include "events.h"

namespace Catoms2D {

//===========================================================================================================
//
//          MotionStartEvent  (class)
//
//===========================================================================================================

class MotionStartEvent : public BlockEvent {
    Vecteur pivot;
    double angle;
public:
	MotionStartEvent(uint64_t, Catoms2DBlock *block,const Catoms2DBlock *pivotBlock,int sens);
	MotionStartEvent(MotionStartEvent *ev);
	~MotionStartEvent();
	void consumeBlockEvent() {};
	void consume();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          MotionStepEvent  (class)
//
//===========================================================================================================

class MotionStepEvent : public BlockEvent {
    Vecteur pivot;
    double angle;
public:
	MotionStepEvent(uint64_t, Catoms2DBlock *block,const Vecteur &pivot,double angle2goal);
	MotionStepEvent(MotionStepEvent *ev);
	~MotionStepEvent();
	void consumeBlockEvent() {};
	void consume();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          MotionStopEvent  (class)
//
//===========================================================================================================

class MotionStopEvent : public BlockEvent {
    Vecteur finalPosition;
public:
	MotionStopEvent(uint64_t, Catoms2DBlock *block);
	MotionStopEvent(MotionStepEvent *ev);
	~MotionStopEvent();
	void consumeBlockEvent() {};
	void consume();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          MotionEndEvent  (class)
//
//===========================================================================================================

class MotionEndEvent : public BlockEvent {
public:
	MotionEndEvent(uint64_t, Catoms2DBlock *block);
	MotionEndEvent(MotionEndEvent *ev);
	~MotionEndEvent();
	void consumeBlockEvent() {};
	void consume();
	const virtual string getEventName();
};

}
#endif /* CATOMS2DEVENTS_H_ */
