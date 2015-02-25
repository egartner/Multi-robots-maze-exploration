/*
 * robotBlocksEvents.h
 *
 *  Created on: 2014 febrary 1st
 *      Author: Benoît
 */

#ifndef ROBOTBLOCKSEVENTS_H_
#define ROBOTBLOCKSEVENTS_H_

#include "robotBlocksBlock.h"
#include "events.h"

namespace RobotBlocks {

//===========================================================================================================
//
//          MotionStartEvent  (class)
//
//===========================================================================================================

class MotionStartEvent : public BlockEvent {
    Vecteur finalPosition;
public:
	MotionStartEvent(uint64_t, RobotBlocksBlock *block,const Vecteur &fpos);
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
    Vecteur finalPosition,motionStep;
public:
	MotionStepEvent(uint64_t, RobotBlocksBlock *block,const Vecteur &fpos);
	MotionStepEvent(uint64_t, RobotBlocksBlock *block,const Vecteur &fpos,const Vecteur &step);
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
	MotionStopEvent(uint64_t, RobotBlocksBlock *block,const Vecteur &fpos);
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
	MotionEndEvent(uint64_t, RobotBlocksBlock *block);
	MotionEndEvent(MotionEndEvent *ev);
	~MotionEndEvent();
	void consumeBlockEvent() {};
	void consume();
	const virtual string getEventName();
};

}
#endif /* ROBOTBLOCKSEVENTS_H_ */
