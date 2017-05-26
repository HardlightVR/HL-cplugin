#pragma once

#include "IHapticDevice.h"
#include "HardwareDriver.h"
#include "IRetainedEvent.h"
#include "EffectCommand.pb.h"
#include <queue>
#include <deque>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include "Enums.h"
#include <functional>
#include <mutex>


class MyBasicHapticEvent {
public:
	MyBasicHapticEvent(boost::uuids::uuid parent_id, boost::uuids::uuid unique_id, uint32_t area, float duration, float strength, uint32_t effect);

	explicit MyBasicHapticEvent(boost::uuids::uuid id);

	bool MyBasicHapticEvent::operator==(const MyBasicHapticEvent & other) const;
	bool IsFunctionallyIdentical(const MyBasicHapticEvent &other);

	CommandBuffer EmitCreationCommands() const;
	CommandBuffer EmitCleanupCommands() const;



	void Update(float dt);

	void LogicalPlay();
	void LogicalPause();

	bool Finished() const;
	boost::uuids::uuid m_parentId;
	uint32_t m_area;
	float m_duration;
	float m_strength;
	uint32_t m_effect;
	float m_time;
	boost::uuids::uuid m_uniqueId;
	bool m_playing;
	


	

};



class ZoneModel {
public:
	typedef std::vector<MyBasicHapticEvent> PlayingContainer;
	typedef std::vector<MyBasicHapticEvent> PausedContainer;


	//Public, to-be-threadsafe api
	void Put(MyBasicHapticEvent event);
	void Remove(boost::uuids::uuid id);
	void Play(boost::uuids::uuid id);
	void  Pause(boost::uuids::uuid id);

	const PausedContainer& PausedEvents();
	const PlayingContainer& PlayingEvents();
	CommandBuffer Update(float dt);
	void ZoneModel::swapOutEvent(const MyBasicHapticEvent& event);
	bool ZoneModel::isTopEvent(const MyBasicHapticEvent& event) const;
	ZoneModel();
private:
	struct UserCommand {
		enum class Command {
			Unknown = 0, Play = 1, Pause = 2, Remove = 3
		};
		boost::uuids::uuid id;
		Command command;

		UserCommand(boost::uuids::uuid id, Command c);
	};
	std::vector<MyBasicHapticEvent> m_stagingEvents;
	std::vector<UserCommand> m_stagingCommands;
	std::mutex m_stagingLock;

	inline PlayingContainer::iterator findPlayingEvent(const boost::uuids::uuid& id);
	inline PausedContainer::iterator findPausedEvent(const boost::uuids::uuid& id);

	//provides locked writing to the creation buffer
	void setCreationCommands(CommandBuffer buffer);

	//provides locked writing to the cleanup buffer
	void setCleanupCommands(CommandBuffer buffer);
	CommandBuffer m_creationCommands;
	CommandBuffer m_cleanupCommands;
	PlayingContainer m_events;
	PausedContainer m_pausedEvents;
	std::mutex m_eventsMutex;
	std::mutex m_pausedMutex;
	std::mutex m_commandsMutex;
	boost::uuids::random_generator m_idGen;


};

class RtpModel {
public:
	
	RtpModel(uint32_t area);
	void ChangeVolume(int newVolume);
	CommandBuffer Update(float dt);
private:
	int m_volume;
	uint32_t m_area;
	CommandBuffer m_commands;
	std::mutex m_mutex;
};
class Hardlight_Mk3_ZoneDriver : public HardwareDriver {
public:
	CommandBuffer update(float dt);

	virtual boost::uuids::uuid Id() const override;
	//this should really take a Location probably..
	Hardlight_Mk3_ZoneDriver(Location area);

	Location Location();


private:
	uint32_t m_area;

	ZoneModel m_retainedModel;
	RtpModel m_rtpModel;
	void createRetained(boost::uuids::uuid handle, const SuitEvent& event) override;
	void controlRetained(boost::uuids::uuid handle, NSVR_PlaybackCommand command) override;
	void realtime(const RealtimeArgs& args) override;

	boost::uuids::random_generator m_gen;
	enum class Mode {Retained, Realtime};
	void transitionInto(Mode mode);
	Mode m_currentMode;
	boost::uuids::uuid m_parentId;
	std::mutex m_mutex;
	CommandBuffer m_commands;
};





class HardlightDevice : public IHapticDevice {
public:
	HardlightDevice();

	virtual void RegisterDrivers(EventRegistry& registry) override;


	virtual void UnregisterDrivers(EventRegistry& registry) override;


	virtual CommandBuffer GenerateHardwareCommands(float dt) override;

private:
	std::vector<std::shared_ptr<Hardlight_Mk3_ZoneDriver>> m_drivers;


};




