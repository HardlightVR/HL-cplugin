﻿#include "stdafx.h"
#include "Engine.h"
#include "HapticFileInfo.h"
#include "Enums.h"
#include "flatbuffers\flatbuffers.h"

#include "Sequence_generated.h"
#include "Pattern_generated.h"

#include "HapticEffect_generated.h"
#include "HapticFrame_generated.h"
#include "HapticPacket_generated.h"
#include "Wire\FlatbuffDecoder.h"
#include <boost\bind.hpp>
#include "EventList.h"
#include <chrono>
void Engine::executeTimestep()
{
	
	constexpr auto fraction_of_second = (1.0f / 1000.f);
	auto dt = m_hapticsExecutionInterval.total_milliseconds() * fraction_of_second;
	auto effectCommands = m_player.Update(dt);
	for (const auto& command : effectCommands) {
		m_messenger.WriteHaptics(command);
	}
	
}

Engine::Engine() :

	_decoder(std::make_unique<FlatbuffDecoder>()),
	_isEnginePlaying(true),
	m_ioService(),
	m_messenger(m_ioService.GetIOService()),
	m_player(),
	_currentHandleId(0),
	m_hapticsExecutionInterval(boost::posix_time::milliseconds(5)),
	m_hapticsExecutionTimer(m_ioService.GetIOService()),
	m_hapticsTimestep(m_ioService.GetIOService(), m_hapticsExecutionInterval),
	m_cachedTracking({})
{

	m_hapticsTimestep.SetEvent([this]() {executeTimestep(); });
	m_hapticsTimestep.Start();
	//scheduleTimestep();
}



Engine::~Engine()
{
	m_player.ClearAll();
	std::this_thread::sleep_for(std::chrono::milliseconds(25));
	m_hapticsTimestep.Stop();
	m_ioService.Shutdown();
}

int Engine::PollStatus(NSVR_System_Status* status)
{
	return Poll(status);
	
}

uint32_t Engine::GenHandle()
{
	//todo: bounds check
	_currentHandleId += 1;
	return _currentHandleId;
}


bool Engine::Poll(NSVR_System_Status* status) {
	if (auto optionalResponse = m_messenger.ReadSuits()) {
		auto suits = optionalResponse.get();
		if ((std::time(nullptr) - suits.timestamp) > 1) {
			status->ConnectedToService = 0;
			status->ConnectedToSuit = 0;
			return true;
		}
		status->ConnectedToService = 1;
		status->ConnectedToSuit = 0;

		for (int i = 0; i < 4; i++) {
			if (suits.SuitsFound[i]) {
				auto sStatus  = suits.Suits[i].Status;
				if (sStatus == NullSpace::SharedMemory::Connected) {
					status->ConnectedToSuit = true;
				}
			}
		}
	}
	return true;
}




bool Engine::EngineCommand(NSVR_EngineCommand command)
{
	switch (command) {
	case NSVR_EngineCommand::NSVR_EngineCommand_ResumeAll:
		m_player.PlayAll();
		break;
	case NSVR_EngineCommand::NSVR_EngineCommand_PauseAll:
		m_player.PauseAll();
		break;
	case NSVR_EngineCommand::NSVR_EngineCommand_DestroyAll:
		m_player.ClearAll();
		break;
	case NSVR_EngineCommand::NSVR_EngineCommand_EnableTracking:
		{
		NullSpaceIPC::DriverCommand command;
		command.set_command(NullSpaceIPC::DriverCommand_Command_ENABLE_TRACKING);
		m_messenger.WriteCommand(command);
		}
		break;
	case NSVR_EngineCommand::NSVR_EngineCommand_DisableTracking:
		{
		NullSpaceIPC::DriverCommand command;
		command.set_command(NullSpaceIPC::DriverCommand_Command_DISABLE_TRACKING);
		m_messenger.WriteCommand(command);
		}
		break;
	default:
		break;
	}
	
	return true;
}






void Engine::HandleCommand(unsigned int handle, NSVR_PlaybackCommand c)
{
	HapticHandle h = HapticHandle(handle);
	switch (c) {
	case NSVR_PlaybackCommand::NSVR_PlaybackCommand_Pause:
		m_player.Pause(h);
		break;
	case NSVR_PlaybackCommand::NSVR_PlaybackCommand_Play:
		m_player.Play(h);
		break;
	case 3: //release
		m_player.Release(h);
		break;
	case NSVR_PlaybackCommand::NSVR_PlaybackCommand_Reset:
		m_player.Stop(h);
		break;
	default:
		break;
	}
	
}



void Engine::GetError(NSVR_ErrorInfo* errorInfo)
{
	strncpy_s(errorInfo->ErrorString, 512, _currentError.c_str(), 512);
	errorInfo->ErrorString[511] = '\0';
	//todo: Keep track of LastResult as well
}

int Engine::CreateEffect(uint32_t handle, void* data, unsigned int size)
{
	flatbuffers::Verifier verifier(reinterpret_cast<uint8_t*>(data), size);
	if (NullSpace::Events::VerifySuitEventListBuffer(verifier)) {
		auto encodedList = NullSpace::Events::GetSuitEventList(data);
		auto decodedList = FlatbuffDecoder::Decode(encodedList);
		m_player.Create(handle, decodedList);
		return 1;
	}

	return 0;
}

int Engine::CreateEffect(EventList * list, uint32_t handle)
{
	if (list == nullptr) {
		return -1;
	}

	m_player.Create(handle, list->Events());
	return 1;


}


void copyQuaternion(NSVR_Quaternion& lhs, const NullSpace::SharedMemory::Quaternion& rhs) {
	lhs.w = rhs.w;
	lhs.x = rhs.x;
	lhs.y = rhs.y;
	lhs.z = rhs.z;
}

void copyTracking(NSVR_TrackingUpdate& lhs, const NullSpace::SharedMemory::TrackingUpdate& rhs) {
	copyQuaternion(lhs.chest, rhs.chest);
	copyQuaternion(lhs.left_upper_arm, rhs.left_upper_arm);
	copyQuaternion(lhs.right_upper_arm, rhs.right_upper_arm);
}

int Engine::PollTracking(NSVR_TrackingUpdate* q)
{
	
	if (auto trackingUpdate = m_messenger.ReadTracking()) {
		copyTracking(*q, *trackingUpdate);
		return NSVR_Success_Unqualified;
	}
	else {
		return NSVR_Success_NoDataAvailable;
	}
}

int Engine::PollLogs(NSVR_LogEntry * entry)
{
	if (auto logEntry = m_messenger.ReadLog()) {
		auto str = *logEntry;
		entry->Length = str.length();
		strncpy_s(entry->Message, 512, str.c_str(), 512);
		entry->Message[511] = '\0';
		return NSVR_Success_Unqualified;
	}
	else {
		return NSVR_Success_NoDataAvailable;
	}
}

