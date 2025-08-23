#pragma once
#include <JuceHeader.h>
#include "AHDSR.h"
#include "SynthSound.h"
#include "Oscillator.h"

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

	AHDSR ampEnv;
	Oscillator osc1, osc2, osc3;

	std::atomic<float>* pAttack{ nullptr };
	std::atomic<float>* pHold{ nullptr };
	std::atomic<float>* pDecay{ nullptr };
	std::atomic<float>* pSustain{ nullptr };
	std::atomic<float>* pRelease{ nullptr };

	std::atomic<float>* pOsc1Wave{ nullptr };
	std::atomic<float>* pOsc1Level{ nullptr };
	std::atomic<float>* pOsc1Coarse{ nullptr };
	std::atomic<float>* pOsc1Fine{ nullptr };
	std::atomic<float>* pOsc1PW{ nullptr };
	std::atomic<float>* pOsc1Detune{ nullptr };

	std::atomic<float>* pOsc2Wave{ nullptr };
	std::atomic<float>* pOsc2Level{ nullptr };
	std::atomic<float>* pOsc2Coarse{ nullptr };
	std::atomic<float>* pOsc2Fine{ nullptr };
	std::atomic<float>* pOsc2PW{ nullptr };
	std::atomic<float>* pOsc2Detune{ nullptr };

	std::atomic<float>* pOsc3Wave{ nullptr }; 
	std::atomic<float>* pOsc3Level{ nullptr };
	std::atomic<float>* pOsc3Coarse{ nullptr };
	std::atomic<float>* pOsc3Fine{ nullptr };
	std::atomic<float>* pOsc3PW{ nullptr };
	std::atomic<float>* pOsc3Detune{ nullptr };


	double sampleRate = 44100.0;
	float  currentFreq = 440.0f;
	float  phase = 0.0f;
	float  level = 1.0f;
};