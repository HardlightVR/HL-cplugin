#include "stdafx.h"
#include "EnumTranslator.h"

#include <boost/assign/list_of.hpp>
#include <boost/assign/list_inserter.hpp>
template<typename T> struct map_init_helper
{
	T& data;
	map_init_helper(T& d) : data(d) {}
	map_init_helper& operator() (typename T::key_type const& key, typename T::mapped_type const& value)
	{
		data[key] = value;
		return *this;
	}
};

template<typename T> map_init_helper<T> map_init(T& item)
{
	return map_init_helper<T>(item);
};

std::string EnumTranslator::ToString(Location loc) const
{
	return _locationMap.left.at(loc);

}

std::string EnumTranslator::ToString(Effect effect) const
{
	return _effectMap.left.at(effect);


}

std::string EnumTranslator::ToString(Side side) const
{
	return _sideMap.left.at(side);
}

Effect EnumTranslator::ToEffect(std::string effect, Effect defaultEffect)
{
	
	if (_effectMap.right.find(effect) != _effectMap.right.end())
	{
		return _effectMap.right.at(effect);
	}
	return defaultEffect;
}

Effect EnumTranslator::ToEffect(std::string effect) const
{
	return _effectMap.right.at(effect);
}

Side EnumTranslator::ToSide(std::string side) const
{
	return _sideMap.right.at(side);
}

Side EnumTranslator::ToSide(std::string side, Side defaultSide)
{
	if (_sideMap.right.find(side) != _sideMap.right.end())
	{
		return _sideMap.right.at(side);
	}
	return defaultSide;
}

std::string EnumTranslator::ToString(JsonLocation jsonLoc) const
{
	return _jsonLocationMap.left.at(jsonLoc);
}

JsonLocation EnumTranslator::ToJsonLocation(std::string jsonLoc) const
{
	return _jsonLocationMap.right.at(jsonLoc);
}

JsonLocation EnumTranslator::ToJsonLocation(std::string jsonLoc, JsonLocation defaultLocation)
{
	if (_jsonLocationMap.right.find(jsonLoc) != _jsonLocationMap.right.end())
	{
		return _jsonLocationMap.right.at(jsonLoc);
	} 
	return defaultLocation;
}

Location EnumTranslator::ToLocation(std::string location, Location defaultLocation) 
{
	if (_locationMap.right.find(location) != _locationMap.right.end()) {
		return _locationMap.right.at(location);
	}
	return defaultLocation;
	
}

Location EnumTranslator::ToLocation(std::string location) const
{
	return _locationMap.right.at(location);
}

EnumTranslator::EnumTranslator() {
	init_locations();
	init_effects();
	init_sides();
	init_json_locations();
	init_areas();
	init_familymap();
	init_locarea_map();
	init_regions();
}

AreaFlag EnumTranslator::ToArea(std::string area, AreaFlag defaultArea) const
{
	if (_areaMap.right.find(area) != _areaMap.right.end()) {
		return _areaMap.right.at(area);
	}
	return defaultArea;
}

AreaFlag EnumTranslator::ToArea(std::string area) const
{
	return _areaMap.right.at(area);
}

std::string EnumTranslator::ToString(AreaFlag area) const
{
	return _areaMap.left.at(area);
}

uint32_t EnumTranslator::ToEffectFamily(std::string effectFamily) const
{
	return _effectFamilyMap.left.at(effectFamily);
}

std::string EnumTranslator::ToString(uint32_t effectFamily) const
{
	return _effectFamilyMap.right.at(effectFamily);
}

AreaFlag EnumTranslator::ToArea(Location loc) const
{
	return _locationAreaMap.left.at(loc);
}

std::string EnumTranslator::ToRegionString(AreaFlag f) const
{
	return _regionMap.left.at(f);
}

EnumTranslator::~EnumTranslator()
{
}

void EnumTranslator::init_areas()
{
	boost::assign::insert(_areaMap)
		(AreaFlag::None, "None")
		(AreaFlag::Forearm_Left, "Forearm_Left")
		(AreaFlag::Upper_Arm_Left, "Upper_Arm_Left")
		(AreaFlag::Shoulder_Left, "Shoulder_Left")
		(AreaFlag::Back_Left, "Back_Left")
		(AreaFlag::Chest_Left, "Chest_Left")
		(AreaFlag::Upper_Ab_Left, "Upper_Ab_Left")
		(AreaFlag::Mid_Ab_Left, "Mid_Ab_Left")
		(AreaFlag::Lower_Ab_Left, "Lower_Ab_Left")
		(AreaFlag::Forearm_Right, "Forearm_Right")
		(AreaFlag::Upper_Arm_Right, "Upper_Arm_Right")
		(AreaFlag::Shoulder_Right, "Shoulder_Right")
		(AreaFlag::Back_Right, "Back_Right")
		(AreaFlag::Chest_Right, "Chest_Right")
		(AreaFlag::Upper_Ab_Right, "Upper_Ab_Right")
		(AreaFlag::Mid_Ab_Right, "Mid_Ab_Right")
		(AreaFlag::Lower_Ab_Right, "Lower_Ab_Right")
		(AreaFlag::Left_All, "Left_All")
		(AreaFlag::Right_All, "Right_All")
		(AreaFlag::All_Areas, "All_Areas")
		(AreaFlag::Forearm_Both, "Forearm_Both")
		(AreaFlag::Upper_Arm_Both, "Upper_Arm_Both")
		(AreaFlag::Shoulder_Both, "Shoulder_Both")
		(AreaFlag::Back_Both, "Back_Both")
		(AreaFlag::Chest_Both, "Chest_Both")
		(AreaFlag::Upper_Ab_Both, "Upper_Ab_Both")
		(AreaFlag::Mid_Ab_Both, "Mid_Ab_Both")
		(AreaFlag::Lower_Ab_Both, "Lower_Ab_Both");

}
void EnumTranslator::init_familymap()
{
	boost::assign::insert(_effectFamilyMap)
		("bump", 1)
		("buzz", 2)
		("click", 3)
		("double_click", 4)
		("fuzz", 5)
		("hum", 6)
		("long_double_sharp_tick", 7)
		("pulse", 8)
		("pulse_sharp", 9)
		("sharp_click", 10)
		("sharp_tick", 11)
		("short_double_click", 12)
		("short_double_sharp_tick", 13)
		("transition_click", 14)
		("transition_hum", 15)
		("triple_click", 16)
		("doom_buzz", 666);
}

void EnumTranslator::init_locarea_map()
{
	boost::assign::insert(_locationAreaMap)
		(Location::Forearm_Left, AreaFlag::Forearm_Left)
		(Location::Upper_Arm_Left, AreaFlag::Upper_Arm_Left)
		(Location::Shoulder_Left, AreaFlag::Shoulder_Left)
		(Location::Upper_Back_Left, AreaFlag::Back_Left)
		(Location::Chest_Left, AreaFlag::Chest_Left)
		(Location::Upper_Ab_Left, AreaFlag::Upper_Ab_Left)
		(Location::Mid_Ab_Left, AreaFlag::Mid_Ab_Left)
		(Location::Lower_Ab_Left, AreaFlag::Lower_Ab_Left)
		(Location::Forearm_Right, AreaFlag::Forearm_Right)
		(Location::Upper_Arm_Right, AreaFlag::Upper_Arm_Right)
		(Location::Shoulder_Right, AreaFlag::Shoulder_Right)
		(Location::Upper_Back_Right, AreaFlag::Back_Right)
		(Location::Chest_Right, AreaFlag::Chest_Right)
		(Location::Upper_Ab_Right, AreaFlag::Upper_Ab_Right)
		(Location::Mid_Ab_Right, AreaFlag::Mid_Ab_Right)
		(Location::Lower_Ab_Right, AreaFlag::Lower_Ab_Right);
}

void EnumTranslator::init_regions()
{
	boost::assign::insert(_regionMap)
		(AreaFlag::Back_Left, "left_back")
		(AreaFlag::Back_Right, "right_back")
		(AreaFlag::Shoulder_Left, "left_shoulder")
		(AreaFlag::Shoulder_Right, "right_shoulder")
		(AreaFlag::Upper_Arm_Left, "left_upper_arm")
		(AreaFlag::Upper_Arm_Right, "right_upper_arm")
		(AreaFlag::Forearm_Left, "left_forearm")
		(AreaFlag::Forearm_Right, "right_forearm")
		(AreaFlag::Chest_Left, "left_upper_chest")
		(AreaFlag::Chest_Right, "right_upper_chest")
		(AreaFlag::Upper_Ab_Left, "left_upper_ab")
		(AreaFlag::Upper_Ab_Right, "right_upper_ab")
		(AreaFlag::Mid_Ab_Left, "left_mid_ab")
		(AreaFlag::Mid_Ab_Right, "right_mid_ab")
		(AreaFlag::Lower_Ab_Left, "left_lower_ab")
		(AreaFlag::Lower_Ab_Right, "right_lower_ab");

}


void EnumTranslator::init_effects() {

	boost::assign::insert(_effectMap)
		(Effect::Buzz_100, "Buzz_100")
		(Effect::Buzz_20, "Buzz_20")
		(Effect::Buzz_40, "Buzz_40")
		(Effect::Buzz_60, "Buzz_60")
		(Effect::Buzz_80, "Buzz_80")
		(Effect::Double_Click_100, "Double_Click_100")
		(Effect::Double_Click_60, "Double_Click_60")
		(Effect::Long_Buzz_for_Program_Stop_100, "Long_Buzz_for_Program_Stop_100")
		(Effect::Long_Double_Sharp_Click_Medium_100, "Long_Double_Sharp_Click_Medium_100")
		(Effect::Long_Double_Sharp_Click_Medium_60, "Long_Double_Sharp_Click_Medium_60")
		(Effect::Long_Double_Sharp_Click_Medium_80, "Long_Double_Sharp_Click_Medium_80")
		(Effect::Long_Double_Sharp_Click_Strong_100, "Long_Double_Sharp_Click_Strong_100")
		(Effect::Long_Double_Sharp_Click_Strong_30, "Long_Double_Sharp_Click_Strong_30")
		(Effect::Long_Double_Sharp_Click_Strong_60, "Long_Double_Sharp_Click_Strong_60")
		(Effect::Long_Double_Sharp_Click_Strong_80, "Long_Double_Sharp_Click_Strong_80")
		(Effect::Long_Double_Sharp_Tick_100, "Long_Double_Sharp_Tick_100")
		(Effect::Long_Double_Sharp_Tick_60, "Long_Double_Sharp_Tick_60")
		(Effect::Long_Double_Sharp_Tick_80, "Long_Double_Sharp_Tick_80")
		(Effect::Medium_Click_100, "Medium_Click_100")
		(Effect::Medium_Click_60, "Medium_Click_60")
		(Effect::Medium_Click_80, "Medium_Click_80")
		(Effect::Pulsing_Medium_100, "Pulsing_Medium_100")
		(Effect::Pulsing_Medium_60, "Pulsing_Medium_60")
		(Effect::Pulsing_Sharp_100, "Pulsing_Sharp_100")
		(Effect::Pulsing_Sharp_60, "Pulsing_Sharp_60")
		(Effect::Pulsing_Strong_100, "Pulsing_Strong_100")
		(Effect::Pulsing_Strong_60, "Pulsing_Strong_60")
		(Effect::Sharp_Click_100, "Sharp_Click_100")
		(Effect::Sharp_Click_30, "Sharp_Click_30")
		(Effect::Sharp_Click_60, "Sharp_Click_60")
		(Effect::Sharp_Tick_100, "Sharp_Tick_100")
		(Effect::Sharp_Tick_60, "Sharp_Tick_60")
		(Effect::Sharp_Tick_80, "Sharp_Tick_80")
		(Effect::Short_Double_Click_Medium_100, "Short_Double_Click_Medium_100")
		(Effect::Short_Double_Click_Medium_60, "Short_Double_Click_Medium_60")
		(Effect::Short_Double_Click_Medium_80, "Short_Double_Click_Medium_80")
		(Effect::Short_Double_Click_Strong_100, "Short_Double_Click_Strong_100")
		(Effect::Short_Double_Click_Strong_30, "Short_Double_Click_Strong_30")
		(Effect::Short_Double_Click_Strong_60, "Short_Double_Click_Strong_60")
		(Effect::Short_Double_Click_Strong_80, "Short_Double_Click_Strong_80")
		(Effect::Short_Double_Sharp_Tick_100, "Short_Double_Sharp_Tick_100")
		(Effect::Short_Double_Sharp_Tick_60, "Short_Double_Sharp_Tick_60")
		(Effect::Short_Double_Sharp_Tick_80, "Short_Double_Sharp_Tick_80")
		(Effect::Smooth_Hum_10, "Smooth_Hum_10")
		(Effect::Smooth_Hum_20, "Smooth_Hum_20")
		(Effect::Smooth_Hum_30, "Smooth_Hum_30")
		(Effect::Smooth_Hum_40, "Smooth_Hum_40")
		(Effect::Smooth_Hum_50, "Smooth_Hum_50")
		(Effect::Soft_Bump_100, "Soft_Bump_100")
		(Effect::Soft_Bump_30, "Soft_Bump_30")
		(Effect::Soft_Bump_60, "Soft_Bump_60")
		(Effect::Soft_Fuzz, "Soft_Fuzz")
		(Effect::Strong_Buzz, "Strong_Buzz")
		(Effect::Strong_Click_100, "Strong_Click_100")
		(Effect::Strong_Click_1_100, "Strong_Click_1_100")
		(Effect::Strong_Click_2_80, "Strong_Click_2_80")
		(Effect::Strong_Click_30, "Strong_Click_30")
		(Effect::Strong_Click_3_60, "Strong_Click_3_60")
		(Effect::Strong_Click_4_30, "Strong_Click_4_30")
		(Effect::Strong_Click_60, "Strong_Click_60")
		(Effect::Transition_Click_10, "Transition_Click_10")
		(Effect::Transition_Click_100, "Transition_Click_100")
		(Effect::Transition_Click_20, "Transition_Click_20")
		(Effect::Transition_Click_40, "Transition_Click_40")
		(Effect::Transition_Click_60, "Transition_Click_60")
		(Effect::Transition_Hum_10, "Transition_Hum_10")
		(Effect::Transition_Hum_100, "Transition_Hum_100")
		(Effect::Transition_Hum_20, "Transition_Hum_20")
		(Effect::Transition_Hum_40, "Transition_Hum_40")
		(Effect::Transition_Hum_60, "Transition_Hum_60")
		(Effect::Transition_Hum_80, "Transition_Hum_80")

		(Effect::Triple_Click, "Triple_Click")
		(Effect::Transition_Click_80, "Transition_Click_80");


}

void EnumTranslator::init_sides()
{
	boost::assign::insert(_sideMap)
		(Side::Inherit, "inherit")
		(Side::Left, "left")
		(Side::Right, "right")
		(Side::Mirror, "mirror")
		(Side::NotSpecified, "not_specified");
}

void EnumTranslator::init_json_locations()
{
	boost::assign::insert(_jsonLocationMap)
		(JsonLocation::Chest, "chest")
		(JsonLocation::Forearm, "forearm")
		(JsonLocation::Lower_Ab, "lower_ab")
		(JsonLocation::Mid_Ab, "mid_ab")
		(JsonLocation::Shoulder, "shoulder")
		(JsonLocation::Upper_Ab, "upper_ab")
		(JsonLocation::Upper_Arm, "upper_arm")
		(JsonLocation::Upper_Back, "upper_back")
		;
}

void EnumTranslator::init_locations() {
	boost::assign::insert(_locationMap)
	(Location::Chest_Left, "Chest_Left")
	(Location::Chest_Right, "Chest_Right")
	(Location::Forearm_Left, "Forearm_Left")
	(Location::Forearm_Right, "Forearm_Right")
	(Location::Lower_Ab_Left, "Lower_Ab_Left")
	(Location::Lower_Ab_Right, "Lower_Ab_Right")
	(Location::Mid_Ab_Left, "Mid_Ab_Left")
	(Location::Mid_Ab_Right, "Mid_Ab_Right")
	(Location::Shoulder_Left, "Shoulder_Left")
	(Location::Shoulder_Right, "Shoulder_Right")
	(Location::Upper_Ab_Left, "Upper_Ab_Left")
	(Location::Upper_Ab_Right, "Upper_Ab_Right")
	(Location::Upper_Arm_Left, "Upper_Arm_Left")
	(Location::Upper_Arm_Right, "Upper_Arm_Right")
	(Location::Upper_Back_Left, "Upper_Back_Left")
	(Location::Upper_Back_Right, "Upper_Back_Right")
		;
}
