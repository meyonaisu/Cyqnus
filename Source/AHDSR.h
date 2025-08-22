#pragma once
#include <JuceHeader.h>

struct AHDSR {
public:
	struct Params {
		float attack = 0.01f;
		float hold = 0.0f;
		float decay = 1.0f;
		float sustain = 0.5f;
		float release = 0.01f;
	};

	void setSampleRate(double sr);
	void reset();
	void setParameters(const Params& p);
	void noteOn();
	void noteOff();
	bool isActive();
	float getNextSample();

private:
	enum class State {Idle, Attack, Hold, Decay, Sustain, Release};
	State state = State::Idle;

	Params params;
	double sampleRate{ 44100.0 };
	float  t{ 0.0f };
	float  level{ 0.0f };
	float  releaseStartLevel{ 0.0f };
};