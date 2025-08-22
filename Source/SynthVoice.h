#pragma once
#include <JuceHeader.h>
#include "AHDSR.h"
#include "SynthSound.h"

class SynthVoice : public juce::SynthesiserVoice {
public:
	explicit SynthVoice(juce::AudioProcessorValueTreeState& state);

	bool canPlaySound(juce::SynthesiserSound* sound) override;
	void prepareToPlay(double sampleRate, int);
	void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int) override;
	void stopNote(float, bool allowTailOff) override;
	void pitchWheelMoved(int) override;
	void controllerMoved(int, int) override;
	void renderNextBlock(juce::AudioBuffer<float>& output, int startSample, int numSamples) override;

private:
	juce::AudioProcessorValueTreeState& apvts;

	std::atomic<float>* pAttack{ nullptr };
	std::atomic<float>* pHold{ nullptr };
	std::atomic<float>* pDecay{ nullptr };
	std::atomic<float>* pSustain{ nullptr };
	std::atomic<float>* pRelease{ nullptr };

	double sampleRate = 44100.0;
	float  currentFreq = 440.0f;
	float  phase = 0.0f;
	float  level = 1.0f;

	AHDSR ampEnv;
};