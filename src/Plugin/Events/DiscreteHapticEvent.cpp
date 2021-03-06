#include "stdafx.h"
#include "DiscreteHapticEvent.h"
#include "Locator.h"

#include "validators.h"
#pragma warning(push)
#pragma warning(disable : 4267)
#include "HighLevelEvent.pb.h"
#pragma warning(pop)

DiscreteHapticEvent::DiscreteHapticEvent(float time) 
	: PlayableEvent(time),
	m_strength(1),
	m_duration(0)
{
	m_parsedEffectFamily = "click";
	m_requestedEffectFamily = Locator::getTranslator().ToEffectFamily(m_parsedEffectFamily);
}



float DiscreteHapticEvent::duration() const
{
	//Oneshots, aka 0 duration effects, last about a quarter second
	return m_duration == 0 ? 0.25f : m_duration;
}

uint32_t DiscreteHapticEvent::effectFamily() const
{
	return m_requestedEffectFamily;
}




std::vector<Validator> DiscreteHapticEvent::makeValidators() const  {

	return{
		make_optional_constraint<float>(HLVR_EventKey_DiscreteHaptic_Strength_Float, [](float strength) { return strength >= 0.0f && strength <= 1.0f; }),
		make_optional_constraint<uint32_t>(HLVR_EventKey_DiscreteHaptic_Repetitions_UInt32, [](uint32_t dur) { return dur > 0; /* well, it's unsigned... */}),
		make_optional_constraint<int>(HLVR_EventKey_DiscreteHaptic_Waveform_Int, [](int effect) { return effect > 0; })
	};
}
void DiscreteHapticEvent::doParse(const ParameterizedEvent& ev)
{
	m_strength = ev.GetOr<float>(HLVR_EventKey_DiscreteHaptic_Strength_Float, 1.0f);
	m_duration = ev.GetOr<uint32_t>(HLVR_EventKey_DiscreteHaptic_Repetitions_UInt32, 0);


	m_requestedEffectFamily = ev.GetOr<int>(HLVR_EventKey_DiscreteHaptic_Waveform_Int, 3);
	std::string effect = Locator::getTranslator().ToEffectFamilyString(m_requestedEffectFamily);
	m_parsedEffectFamily = effect;

}

bool DiscreteHapticEvent::isEqual(const PlayableEvent& other) const
{
	const auto& ev = static_cast<const DiscreteHapticEvent&>(other);
	return 
		m_strength == ev.m_strength
		&& m_requestedEffectFamily == ev.m_requestedEffectFamily
		&& m_parsedEffectFamily == ev.m_parsedEffectFamily
		&& m_duration == ev.m_duration;
}

void DiscreteHapticEvent::doSerialize(NullSpaceIPC::HighLevelEvent& event) const
{
	using namespace NullSpaceIPC;
	LocationalEvent* location = event.mutable_locational_event();
	SimpleHaptic* simple = location->mutable_simple_haptic();
	simple->set_repetitions(m_duration);
	simple->set_effect(m_requestedEffectFamily);
	simple->set_strength(m_strength);

	
}

float DiscreteHapticEvent::strength() const
{
	return m_strength;
}


//
//HLVR_EventType DiscreteHapticEvent::type() const
//{
//	return descriptor;
//}
