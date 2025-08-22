#include "SynthVoice.h"
 
SynthVoice::SynthVoice(juce::AudioProcessorValueTreeState& state)
	: apvts(state) {
	pAttack  = apvts.getRawParameterValue("ampAttack");
	pHold    = apvts.getRawParameterValue("ampHold");
	pDecay   = apvts.getRawParameterValue("ampDecay");
	pSustain = apvts.getRawParameterValue("ampSustain");
	pRelease = apvts.getRawParameterValue("ampRelease");
}

void SynthVoice::prepareToPlay(double sampleRate, int) {
	this->sampleRate = (sampleRate > 0.0) ? sampleRate : 44100.0;
	ampEnv.setSampleRate(sampleRate);
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound) {
	return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int) {
	currentFreq = static_cast<float>(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber));
	level = juce::jlimit(0.0f, 1.0f, velocity);
	phase = 0.0f;

	AHDSR::Params envParams;
	envParams.attack  = pAttack->load();
	envParams.hold    = pHold->load();
	envParams.decay   = pDecay->load();
	envParams.sustain = pSustain->load();
	envParams.release = pRelease->load();
	ampEnv.setParameters(envParams);
	ampEnv.noteOn();
}

void SynthVoice::stopNote(float, bool allowTailOff) {
	if (allowTailOff) {
		ampEnv.noteOff();
	} else {
		ampEnv.reset();
		clearCurrentNote();
	}
}

void SynthVoice::pitchWheelMoved(int) {}
void SynthVoice::controllerMoved(int, int) {}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& output, int startSample, int numSamples) {
	if (!ampEnv.isActive()) {
		clearCurrentNote();
		return;
	}

	auto* left = output.getWritePointer(0, startSample);
	auto* right = output.getNumChannels() > 1 ? output.getWritePointer(1, startSample) : nullptr;

	const float twoPiOverSR = 2.0f * static_cast<float>(juce::MathConstants<double>::pi) / static_cast<float>(getSampleRate());
	const float phaseInc = currentFreq * twoPiOverSR;

	for (int i = 0; i < numSamples; ++i) {
		const float osc = std::sin(phase);
		phase += phaseInc;
		if (phase >= juce::MathConstants<float>::twoPi)
			phase -= juce::MathConstants<float>::twoPi;

		const float env = ampEnv.getNextSample();
		const float sample = osc * env * level;

		left[i] = sample;
		if (right) right[i] = sample;
	} 

	if (!ampEnv.isActive())
		clearCurrentNote();
}

