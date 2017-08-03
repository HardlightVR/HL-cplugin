#pragma once

#include "ReadableSharedQueue.h"
#include "ReadableSharedObject.h"
#include "WritableSharedQueue.h"
#include "ReadableSharedTracking.h"
#include "ReadableSharedVector.h"
#include "SharedTypes.h"
#include <boost\optional.hpp>
#include <boost\asio.hpp>
#include <boost\chrono.hpp>

#pragma warning(push)
#pragma warning(disable : 4267)
#include "EffectCommand.pb.h"
#include "DriverCommand.pb.h"
#include "HighLevelEvent.pb.h"
#pragma warning(pop)

typedef struct NSVR_ServiceInfo_ NSVR_ServiceInfo;
class ClientMessenger
{
public:
	ClientMessenger(boost::asio::io_service&);
	~ClientMessenger();

	boost::optional<NullSpace::SharedMemory::TrackingUpdate> ReadTracking();
	boost::optional<NullSpace::SharedMemory::SuitsConnectionInfo> ReadSuits();
	void WriteCommand(const NullSpaceIPC::DriverCommand& d);
	void WriteEvent(const NullSpaceIPC::HighLevelEvent& e);
	void WriteHaptics(const NullSpaceIPC::EffectCommand& e);
	boost::optional<std::string> ReadLog();

	std::vector<NullSpace::SharedMemory::RegionPair> ReadBodyView();
	bool ConnectedToService(NSVR_ServiceInfo* info);

private:
	//Write haptics to the suit using this shared queue
	std::unique_ptr<WritableSharedQueue> m_hapticsStream;

	//Read the most up-to-date tracking data from this object
	std::unique_ptr<ReadableSharedObject<NullSpace::SharedMemory::TrackingUpdate>> m_trackingData;

	//Read the most up-to-date suit connection information from this object
	// 
	std::unique_ptr<ReadableSharedObject<NullSpace::SharedMemory::SuitsConnectionInfo>> m_suitConnectionInfo;

	//Get logging info from engine. Note: only one consumer can reliably get the debug info
	std::unique_ptr<ReadableSharedQueue> m_logStream;

	//Sentinel to see if the driver is running
	std::unique_ptr<ReadableSharedObject<std::time_t>> m_sentinel;

	//Stream of commands to send to driver, such as ENABLE_TRACKING, DISABLE_TRACKING, etc.
	std::unique_ptr<WritableSharedQueue> m_commandStream;

	std::unique_ptr<ReadableSharedTracking> m_tracking;

	std::unique_ptr<ReadableSharedVector<NullSpace::SharedMemory::RegionPair>> m_bodyView;
	//We use a sentinel to see if the driver is responsive/exists
	boost::asio::deadline_timer m_sentinelTimer;

	//How often we read the sentinel
	boost::posix_time::milliseconds m_sentinelInterval;

	//If currentTime - sentinalTime > m_sentinalTimeout, we say that we are disconnected
	boost::chrono::milliseconds m_sentinalTimeout;



	void startAttemptEstablishConnection();

	void attemptEstablishConnection(const boost::system::error_code& ec);

	void startMonitorConnection();
	void monitorConnection(const boost::system::error_code& ec);

//	Encoder m_encoder;

	bool m_connectedToService;
};

