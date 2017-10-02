#pragma once
#include <boost/variant.hpp>
#include "ParameterizedEvent.h"
#include "PlayableEvent.h"
#include "NSLoader.h"


class BasicHapticEvent : public PlayableEvent {
public:	
	BasicHapticEvent();
	
	template<typename T>
	struct Loc {
		uint32_t value;
		explicit Loc(uint32_t val) : value(val) {}
		Loc() : value(0) {}
	
	};

	
	struct node {};
	struct region {};

	using Where = boost::variant<std::vector<Loc<node>>, std::vector<Loc<region>>>;
	float strength() const;
	uint32_t effectFamily() const;

	/* PlayableEvent impl */
	float time() const override;
	float duration() const override;
	NSVR_EventType type() const override;
	bool parse(const ParameterizedEvent&) override;
	void serialize(NullSpaceIPC::HighLevelEvent& event) const override;
	static constexpr NSVR_EventType descriptor = NSVR_EventType::NSVR_EventType_SimpleHaptic;

private:
	float m_time;
	float m_strength;
	float m_duration;
	Where m_area;
	std::string m_parsedEffectFamily;
	uint32_t m_requestedEffectFamily;



	virtual bool isEqual(const PlayableEvent& other) const override;


};
template<typename T>
bool operator==(const BasicHapticEvent::Loc<T>& lhs, const BasicHapticEvent::Loc<T>& rhs) { return lhs.value == rhs.value; }

typedef boost::variant<BasicHapticEvent> SuitEvent;
