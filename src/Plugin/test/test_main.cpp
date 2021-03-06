#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "../EffectPlayer.h"
#include "../ParameterizedEvent.h"
#include "../ClientMessenger.h"
#include "../SharedCommunication/readablesharedvector.h"
#include "../SharedCommunication/SharedTypes.h"
#include "HLVR_Experimental.h"
#include "DiscreteHapticEvent.h"
#include "../include/bindings/cpp/hlvr_system.hpp"
#include "../include/bindings/cpp/hlvr_event.hpp"
#include "../include/bindings/cpp/hlvr_timeline.hpp"

#include <iostream>
#include <type_traits>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/asio/io_service.hpp>
#include <functional>
#include <chrono>

template<typename T>
T time(std::function<void()> fn) {
	auto then = std::chrono::high_resolution_clock::now();
	fn();
	auto now = std::chrono::duration_cast<T>(std::chrono::high_resolution_clock::now() - then);
	return now;
}

boost::uuids::random_generator idGenerator;

const float DELTA_TIME = 0.05f;






//Creates two playables for testing purposes, first at 0.0 seconds, next at 1.0 seconds.
std::vector<std::unique_ptr<PlayableEvent>> makePlayables() {
	std::vector<std::unique_ptr<PlayableEvent>> events;
	DiscreteHapticEvent a(0.0f);
	ParameterizedEvent e;
	std::vector<uint32_t> region = { hlvr_region_upper_ab_left };
	e.Set(HLVR_EventKey_Target_Regions_UInt32s, region.data(), region.size());
	
	a.parse(e);
	events.push_back(std::unique_ptr<PlayableEvent>(new DiscreteHapticEvent(a)));

	DiscreteHapticEvent b(1.0f);
	b.parse(e);
	events.push_back(std::unique_ptr<PlayableEvent>(new DiscreteHapticEvent(b)));
	return events;
}


TEST_CASE("The haptics player works", "[HapticsPlayer]") {

	//Now I see why we shouldn't have classes take hard references to resources.
	//Messenger should probably be an interface, else we need it to test EffectPlayer
	boost::asio::io_service io;
	ClientMessenger m(io);
	EffectPlayer player(io, m);

	REQUIRE(player.GetNumLiveEffects() == 0);
	REQUIRE(player.GetNumReleasedEffects() == 0);


	SECTION("Retrieving a nonexistent handle shouldn't crash") {
		EffectHandle randomlyChosen = 1245;
		REQUIRE_NOTHROW([&]() {
			auto handle = player.GetInfo(randomlyChosen);
			REQUIRE(!handle);
		});
		
	}

	SECTION("Creating an effect should work, and the effect should be not be playing by default") {
		EffectHandle h = player.Create(makePlayables());
		
		REQUIRE(player.GetNumLiveEffects() == 1);

		auto info = player.GetInfo(h);
		REQUIRE(info->State != 0); //this enum should be exposed. 0 = playing
		REQUIRE(info->CurrentTime == Approx(0.0f));
	}

	SECTION("Pausing an effect should work") {
		EffectHandle h = player.Create(makePlayables());
		
		auto info = player.GetInfo(h);
		REQUIRE(info->Duration > DELTA_TIME);

		player.Play(h);
		player.Update(DELTA_TIME);
		player.Pause(h);

		info = player.GetInfo(h);
		REQUIRE(info->State != HLVR_EffectInfo_State_Playing);
		REQUIRE(info->CurrentTime == Approx(DELTA_TIME));
	}

	SECTION("Stopping an effect should work") {
		EffectHandle h = player.Create(makePlayables());
		auto info = player.GetInfo(h);
		REQUIRE(info->Duration > DELTA_TIME);

		player.Play(h);
		player.Update(DELTA_TIME);
		player.Stop(h);

		info = player.GetInfo(h);
		REQUIRE(info->State != HLVR_EffectInfo_State_Playing);
		REQUIRE(info->CurrentTime == Approx(DELTA_TIME));
	}

	SECTION("Resuming an effect should work") {
		EffectHandle h = player.Create(makePlayables());
		auto info = player.GetInfo(h);
		REQUIRE(info->Duration > DELTA_TIME);

		player.Play(h);
		player.Update(DELTA_TIME);
		player.Pause(h);
		player.Update(DELTA_TIME * 10);
		player.Play(h);
		player.Update(DELTA_TIME);

		info = player.GetInfo(h);
		REQUIRE(info->State == HLVR_EffectInfo_State_Playing);
		REQUIRE(info->CurrentTime == Approx(DELTA_TIME * 2));
	}

	SECTION("An effect should stop after reaching its duration") {
		EffectHandle h = player.Create(makePlayables());

		auto info = player.GetInfo(h);
		REQUIRE(info->Duration > DELTA_TIME);


		player.Play(h);
		player.Update(info->Duration + DELTA_TIME);
		info = player.GetInfo(h);
		REQUIRE(info->State != HLVR_EffectInfo_State_Playing);
		REQUIRE(info->CurrentTime == Approx(info->Duration + DELTA_TIME));
	}

	SECTION("Releasing an effect should work") {
		EffectHandle h = player.Create(makePlayables());
		player.Release(h);
		REQUIRE(player.GetNumLiveEffects() == 0);
		REQUIRE(player.GetNumReleasedEffects() == 1);
		
	}

	SECTION("A released effect should be cleaned up properly") {
		EffectHandle h = player.Create(makePlayables());

		SECTION("If it was playing at the time of release, it should not be deleted until it is done playing") {
			player.Play(h);
			auto duration = player.GetInfo(h)->Duration;
			player.Release(h);
			player.Update(DELTA_TIME);
			REQUIRE(player.GetNumReleasedEffects() == 1);
			player.Update(DELTA_TIME);
			REQUIRE(player.GetNumReleasedEffects() == 1);

			player.Update(duration + DELTA_TIME);
			REQUIRE(player.GetNumLiveEffects() == 0);
			REQUIRE(player.GetNumReleasedEffects() == 0);
		}

		SECTION("If it was not playing at time of release, it should be deleted in the next update") {
			player.Release(h);
			player.Update(DELTA_TIME);
			REQUIRE(player.GetNumReleasedEffects() == 0);
			REQUIRE(player.GetNumLiveEffects() == 0);
		}

		SECTION("You shouldn't be able to interact with a released effect") {
			player.Release(h);
			REQUIRE(player.Play(h) == HLVR_Error_NoSuchHandle);
			REQUIRE(player.Pause(h) == HLVR_Error_NoSuchHandle);
			REQUIRE(player.Stop(h) == HLVR_Error_NoSuchHandle);
		}
	}


	


	
	
		
	

}
HLVR_EventKey key_float = static_cast<HLVR_EventKey>(1);
HLVR_EventKey key_uint = static_cast<HLVR_EventKey>(2);
HLVR_EventKey key_int = static_cast<HLVR_EventKey>(3);
auto key_vecint = static_cast<HLVR_EventKey>(4);
auto key_vecfloat = static_cast<HLVR_EventKey>(5);

TEST_CASE("The events system works", "[EventSystem]") {

	ParameterizedEvent event;
	

	SECTION("You should be able to get out what you put in") {
		event.Set(key_float, 1.0f);
		REQUIRE(event.GetOr(key_float, 999.0f) == Approx(1.0f));

		event.Set(key_int, 1);
		REQUIRE(event.GetOr(key_int, 999) == 1);

		event.Set(key_vecint, std::vector<int>({ 1 }));
		REQUIRE(event.GetOr(key_vecint, std::vector<int>({ 999 })).at(0) == 1);
	}

	SECTION("You should get a default value if you supply a wrong key") {
		event.Set(key_float, 1.0f);
		REQUIRE(event.GetOr(key_int, 999) == 999);
	}

	SECTION("You should get a default value if you supply the wrong type") {
		event.Set(key_float, 1.0f);
		REQUIRE(event.GetOr(key_float, 999) == 999);
	}

	SECTION("If you overwrite a key with a different value, you should get the new value out") {
		event.Set(key_float, 1.0f);
		event.Set(key_float, 2.0f);
		REQUIRE(event.GetOr(key_float, 999.0f) == Approx(2.0f));
	}

	SECTION("If you overwrite a key with a different type, you should get the new type out") {
		event.Set(key_float, 1.0f);
		event.Set(key_float, 2);
		REQUIRE(event.GetOr(key_float, 999) == 2);
	}

	SECTION("If you request a key that isn't present, you should get the default value") {
		REQUIRE(event.GetOr(key_float, 999) == 999);
		REQUIRE(event.GetOr(key_int, 999.0f) == 999.0f);
		REQUIRE(event.GetOr(key_vecfloat, std::vector<float>({ 999.0f })).at(0) == Approx(999.0f));
		REQUIRE(event.GetOr(key_vecint, std::vector<int>({ 999 })).at(0) == 999);
	}

	SECTION("TryGet will return false if the key is not found") {
		int x;
		REQUIRE(!event.TryGet(key_int, &x));
	}

	SECTION("TryGet will not modify the given value if the key is not found") {
		int x = 152;
		event.TryGet(key_int, &x);
		REQUIRE(x == 152);
	}

	SECTION("TryGet will return the value if found") {
		int x;
		event.Set(key_int, 152);
		REQUIRE(event.TryGet(key_int, &x));
		REQUIRE(x == 152);
	}

	SECTION("HasKey will return true if the key is present") {
		event.Set(key_int, 122);
		REQUIRE(event.HasKey(key_int));
	}

	SECTION("HasKey will return false if the key is not present") {
		REQUIRE(!event.HasKey(key_int));
	}
}

TEST_CASE("Validation machinery should work") {
	SECTION("If a key is not present, it isn't an error (using validate, because we support optional)") {
		ParameterizedEvent data;
		auto result = validate<int>(key_int, data, [](int effect) { return true; });
		REQUIRE(!result);

	}
	SECTION("If a key is present, it also isn't an error") {
		ParameterizedEvent data;
		data.Set(HLVR_EventKey_DiscreteHaptic_Waveform_Int, 1);
		auto result = validate<int>(HLVR_EventKey_DiscreteHaptic_Waveform_Int, data, [](int effect) { return true; });
		REQUIRE(!result);

	}

	SECTION("If a key is not present, it returns an error (using validate_required)") {
		ParameterizedEvent data;
		auto result = validate<int>(HLVR_EventKey_DiscreteHaptic_Waveform_Int, data, [](int effect) { return true; }, key_required);
		REQUIRE(*result == HLVR_Event_KeyParseError_KeyRequired);
	}


	SECTION("If the key has a value but it's the wrong type, it should return error") {
		ParameterizedEvent data;
		data.Set(HLVR_EventKey_DiscreteHaptic_Waveform_Int, 1.0f);
		auto result = validate<int>(HLVR_EventKey_DiscreteHaptic_Waveform_Int, data, [](int effect) { return true; });
		REQUIRE(*result == HLVR_Event_KeyParseError_WrongValueType);
	}

	SECTION("If the key has a value but it fails the constraint, it should return error") {
		ParameterizedEvent data;
		data.Set(HLVR_EventKey_DiscreteHaptic_Waveform_Int, 1);
		auto result = validate<int>(HLVR_EventKey_DiscreteHaptic_Waveform_Int, data, [](int effect) { return false; });
		REQUIRE(*result == HLVR_Event_KeyParseError_InvalidValue);
	}
	SECTION("If the key has a value but and passes the constraint, there should be no error") {
		ParameterizedEvent data;
		data.Set(HLVR_EventKey_DiscreteHaptic_Waveform_Int, 1);
		auto result = validate<int>(HLVR_EventKey_DiscreteHaptic_Waveform_Int, data, [](int effect) { return true; });
		REQUIRE(!result);
	}
}

TEST_CASE("Higher level event validation should work") {
	auto playable = PlayableEvent::make(HLVR_EventType_DiscreteHaptic, 0.0f);
	HLVR_Event_ValidationResult result;

	SECTION("An event with no data should be valid") {
		ParameterizedEvent data;
		playable->debug_parse(data, &result);
		REQUIRE(result.Count == 0);
	}


}

TEST_CASE("Retrieving the service version should work") {
	boost::asio::io_service io;

	ClientMessenger m(io);
	io.run_one();
	io.run_one();
	io.run_one();
	io.run_one();
	io.run_one();

	//This is a weird case setup,  I just want to see if it works
	SECTION("So does it?") {
		HLVR_RuntimeInfo info = { 0 };
		auto v = m.ConnectedToService(&info);
		if (v) {
			if (info.MajorVersion == 0) {
				REQUIRE(info.MinorVersion > 0);
			}
			else {
				REQUIRE(info.MinorVersion >= 0);
				REQUIRE(info.MajorVersion >= 1);
			}
		}
		
	}
}
TEST_CASE("BodyView should work") {
	boost::asio::io_service io;
	
	ClientMessenger m(io);
	io.run_one();

	SECTION("The shared memory should instantiate") {
		auto r = m.ReadBodyView();
	}
}

TEST_CASE("Bindings should at least compile ;)") {

	hlvr::system system;
	
	SECTION("An uninitialized system should assert false") {
		REQUIRE(!system);
	}

	SECTION("An uninitialized system should have nullptr native handle") {
		REQUIRE(system.native_handle() == nullptr);
	}

	SECTION("Instantiating system successfully should result in a non-null native_handle and bool operator() should return true") {
		auto potentialSystem = hlvr::system::make();
		if (potentialSystem) {
			INFO("System instantiated");
			REQUIRE(potentialSystem->native_handle() != nullptr);
			REQUIRE(*potentialSystem);
		}
		else {
			INFO("Warning! System was not instantiated - this is not expected, but we still need to test it")
			REQUIRE(!potentialSystem);
			INFO(potentialSystem.error().what());
		}
	}

	SECTION("Moving should work") {
		auto potentialSystem = hlvr::system::make();

		


		if (potentialSystem) {
			system = std::move(*potentialSystem);
			REQUIRE(system);
			REQUIRE(system.native_handle() != nullptr);
		}
		else {
			REQUIRE(1 == 2);
			INFO("Can't test moving if I can't instantiate the plugin..");
		}
	}


	SECTION("Events") {
		hlvr::event event;
		REQUIRE(!event);
		REQUIRE(event.native_handle() == nullptr);
		if (auto realEvent = hlvr::event::make(HLVR_EventType_EndAnalogAudio)) {
			REQUIRE(realEvent->native_handle() != nullptr);
			event = std::move(*realEvent);
			REQUIRE(event);
		}

	}

	SECTION("Timelines") {
		hlvr::timeline timeline;
		REQUIRE(!timeline);
		REQUIRE(timeline.native_handle() == nullptr);

		if (auto realTimeline = hlvr::timeline::make()) {
			REQUIRE(realTimeline->native_handle() != nullptr);
			timeline = std::move(*realTimeline);
			REQUIRE(timeline);
		}

	}

	


}
int main(int argc, char* argv[]) {
	int result = Catch::Session().run(argc, argv);

	std::cin.get();
	return (result < 0xff ? result : 0xff);

}

