﻿#include "stdafx.h"
#include "TestClass.h"
#include "HapticFileInfo.h"
#include "Enums.h"
#include "flatbuffers\flatbuffers.h"

#include "Sequence_generated.h"
#include "Pattern_generated.h"

#include "HapticEffect_generated.h"
#include "HapticFrame_generated.h"
#include "HapticPacket_generated.h"


TestClass::TestClass(LPSTR param) : _resolver(std::string(param)), _wire("tcp://127.0.0.1:9452", "tcp://127.0.0.1:9453")
{



}


TestClass::~TestClass()
{
	//TODO: figure out why we can't leave destruction totally up to zmq context destroy
}

/* returns 1 for connected, 0 for disconnected */
int TestClass::PollStatus()
{
	Poll();
	return _suitStatus;
}

unsigned int TestClass::GenHandle()
{
	return _currentHandleId++;
}

bool TestClass::LoadSequence(LPSTR param)
{
	return _resolver.Load(SequenceFileInfo(std::string(param)));
}

bool TestClass::Poll() {
	return _wire.Receive(_suitStatus, _tracking);
}

void TestClass::PollTracking(NullSpaceDLL::TrackingUpdate& t) {
	Poll();
	t = _tracking;
}


int TestClass::PlayPattern(LPSTR param, Side side)
{
	auto name = std::string(param);
	if (_resolver.Load(PatternFileInfo(name))) {
		auto res = _resolver.ResolvePattern(name, side);
		_wire.Send(_wire.Encoder->Encode(res), name + Locator::getTranslator().ToString(side));
	}
	return 0;
}

int TestClass::PlayExperience(LPSTR param, Side side)
{
	auto name = std::string(param);
	if (_resolver.Load(ExperienceFileInfo(name))) {
		auto res = _resolver.ResolveExperience(name, side);
		_wire.Send(_wire.Encoder->Encode(res), name + Locator::getTranslator().ToString(side));
	}
	return 0;
}
//TODO: wrap with exception handling incase flatbuffers fails/file fails to load/etc

int TestClass::PlaySequence(unsigned int handle, LPSTR param, Location loc)
{
	auto name = std::string(param);
	if (_resolver.Load(SequenceFileInfo(name))) {
		auto res = _resolver.ResolveSequence(name, Area(loc));
		_wire.Send(_wire.Encoder->Encode(res), res.Name(), handle);
	}
	return 0;
}

int TestClass::PlayEffect(Effect e, Location loc, float duration, float time, unsigned int priority)
{
	//_wire.Send(_wire.Encoder->Encode(HapticEffect(e, loc, duration, time, priority)));
	return 0;
}

void TestClass::SetTrackingEnabled(bool wantTracking)
{
	_wire.Send(_wire.Encoder->Encode(wantTracking));

}

void TestClass::HandleCommand(unsigned int handle, short c)
{
	_wire.Send(_wire.Encoder->Encode(NullSpaceDLL::HandleCommand(handle, c)));
}
