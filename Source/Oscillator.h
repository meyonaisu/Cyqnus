#pragma once
#include <JuceHeader.h>

class Oscillator {
public:
	enum Waveform { Sine, Saw, Square, Triangle, Pulse, Noise };
	Oscillator();
	void setSampleRate(double sr);
	void setFrequency(float freq);
	void setWaveform(Waveform wf);
	void setLevel(float lvl);
	void setCoarse(int semis);
	void setFinetune(float cents);
	void setPhaseOffset(float offset);
	void setPulseWidth(float pw);
	void setDetuneSpread(float speedHz);
	float getNextSample();

private:
	void updatePhaseIncrement();
	void wrapPhase();

	double sampleRate{ 44100.0 };
	float  frequency{ 440.0f };
	float  phase{ 0.0f };
	float  phaseInc{ 0.0f };
	float  level{ 0.0f };
	int    coarse{ 0 };
	float  fine{ 0.0f };
	float  pulseWidth{ 0.5f };
	float  detuneSpread{ 0.0f };

	Waveform waveform = Sine;
	juce::Random random;
};