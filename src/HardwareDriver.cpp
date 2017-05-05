#include "stdafx.h"
#include "HardwareDriver.h"
#include "HardwareDriver.h"
#include "NSLoader.h"
#include "EffectCommand.pb.h"
#include "IRetainedEvent.h"
#include <queue>
typedef std::queue<NullSpaceIPC::EffectCommand> CommandBuffer;



class MyBasicHapticEvent : public IRetainedEvent {
public:
	MyBasicHapticEvent(CommandBuffer& commands, uint32_t area, float duration, float strength, uint32_t effect):
		m_area(area), 
		m_duration(duration), 
		m_strength(strength), 
		m_effect(effect), 
		m_time(0), 
		m_commands(commands),
		m_playing(true){

	}
private:
	CommandBuffer& m_commands;
	uint32_t m_area;
	float m_duration;
	float m_strength;
	uint32_t m_effect;
	float m_time; 
	
	bool m_playing;
	virtual void Begin() override
	{
		//we could cache the command if this ever ever ever becomes a bottleneck
		using namespace NullSpaceIPC;
		EffectCommand command;
		command.set_area(m_area);
		command.set_command(m_duration == 0.0? EffectCommand_Command::EffectCommand_Command_PLAY : NullSpaceIPC::EffectCommand_Command_PLAY_CONTINUOUS);
		command.set_effect(m_effect);
		command.set_strength(m_strength);		
		m_commands.push(std::move(command));
	}


	virtual void Pause() override
	{
		using namespace NullSpaceIPC;
		EffectCommand command;
		command.set_area(m_area);
		command.set_command(EffectCommand_Command::EffectCommand_Command_HALT);
		m_playing = false;
	}


	virtual void Resume() override
	{
		Begin();
	}


	virtual void Update(float dt) override
	{
		if (m_playing) {
			m_time += dt;
		}
	}


	virtual bool Finished() override
	{
		return m_time >= m_duration;
	}

};
void Hardlight_Mk3_ZoneDriver::update(float dt)
{
	for (auto& event : m_events) {
		event.event->Update(dt);
	}
}

class RetainedEventCreator : public boost::static_visitor<std::unique_ptr<IRetainedEvent>> {
private:
	//Main ID of the effect
	boost::uuids::uuid& m_id;
	CommandBuffer& m_buffer;
public:
	RetainedEventCreator(boost::uuids::uuid& id, CommandBuffer& buffer) : m_id(id), m_buffer(buffer) {}

	std::unique_ptr<IRetainedEvent> operator()(BasicHapticEvent& hapticEvent) {
		auto ptr = std::unique_ptr<IRetainedEvent>(new MyBasicHapticEvent(m_buffer, hapticEvent.Area, hapticEvent.Duration, hapticEvent.Strength, hapticEvent.RequestedEffectFamily));
		return ptr;
	}
};
void Hardlight_Mk3_ZoneDriver::createRetained(boost::uuids::uuid handle, const SuitEvent & event)
{
	CommandBuffer temp;
	auto ptr = boost::apply_visitor(RetainedEventCreator(handle, temp), event);
	//the back of our vector is always the active effect
	
	m_events.push_back(GeneratedEvent(handle, std::move(ptr)));
}

void Hardlight_Mk3_ZoneDriver::controlRetained(boost::uuids::uuid handle, NSVR_PlaybackCommand command)
{

	auto it = std::find(m_events.begin(), m_events.end(), GeneratedEvent(handle, nullptr));
	if (it == m_events.end()) {
		return;
	}


	switch (command) {
	case NSVR_PlaybackCommand::NSVR_PlaybackCommand_Play:
		(*it).event->Resume();
		break;
	case NSVR_PlaybackCommand::NSVR_PlaybackCommand_Pause:
		(*it).event->Pause();
		break;
	case NSVR_PlaybackCommand::NSVR_PlaybackCommand_Reset:
		(*it).event->Pause();
		m_events.erase(std::remove(m_events.begin(), m_events.end(), it));
		break;
	default:
		break;
	}
}
