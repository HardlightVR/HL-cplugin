#pragma once
#include "Enums.h"
#include <string>
#include "AreaFlags.h"
inline void hash_combine(std::size_t& seed) { }

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	hash_combine(seed, rest...);
}

class HapticArgs
{
public:

	virtual ~HapticArgs()
	{
	}

	virtual std::size_t GetCombinedHash() const = 0;
	virtual std::string ToString() const = 0;

};

class PatternArgs : public HapticArgs
{
public:
	PatternArgs(const std::string& name, Side side);
	~PatternArgs();
	Side Side;
	std::string Name;
	std::size_t GetCombinedHash() const override;
	std::string ToString() const override {
		return Name;
	}
	
};

class SequenceArgs : public HapticArgs
{
public:
	SequenceArgs(const std::string& name, AreaFlag loc, float strength);
	~SequenceArgs();
	AreaFlag Location;
	float Strength;
	std::string Name;
	std::size_t GetCombinedHash() const override;
	std::string ToString() const override {
		return Name;
	}
};

class ExperienceArgs : public HapticArgs
{
public:
	ExperienceArgs(const std::string& name, Side side);
	~ExperienceArgs();
	Side Side;
	std::string Name;
	std::size_t GetCombinedHash() const override;
	std::string ToString() const override {
		return Name;
	}
};