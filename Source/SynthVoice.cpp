#include "SynthVoice.h"
 
SynthVoice::SynthVoice(juce::AudioProcessorValueTreeState& state)
	: apvts(state) {
	pAttack  = apvts.getRawParameterValue("ampAttack");
	pHold    = apvts.getRawParameterValue("ampHold");
	pDecay   = apvts.getRawParameterValue("ampDecay");
	pSustain = apvts.getRawParameterValue("ampSustain");
	pRelease = apvts.getRawParameterValue("ampRelease");

	pOsc1Wave = apvts.getRawParameterValue("osc1Wave");
	pOsc1Level = apvts.getRawParameterValue("osc1Level");
	pOsc1Coarse = apvts.getRawParameterValue("osc1Coarse");
	pOsc1Fine = apvts.getRawParameterValue("osc1Fine");
	pOsc1PW = apvts.getRawParameterValue("osc1PW");
	pOsc1Detune = apvts.getRawParameterValue("osc1Detune");

	pOsc2Wave = apvts.getRawParameterValue("osc2Wave");
	pOsc2Level = apvts.getRawParameterValue("osc2Level");
	pOsc2Coarse = apvts.getRawParameterValue("osc2Coarse");
	pOsc2Fine = apvts.getRawParameterValue("osc2Fine");
	pOsc2PW = apvts.getRawParameterValue("osc2PW");
	pOsc2Detune = apvts.getRawParameterValue("osc2Detune");

	pOsc3Wave = apvts.getRawParameterValue("osc3Wave");
	pOsc3Level = apvts.getRawParameterValue("osc3Level");
	pOsc3Coarse = apvts.getRawParameterValue("osc3Coarse");
	pOsc3Fine = apvts.getRawParameterValue("osc3Fine");
	pOsc3PW = apvts.getRawParameterValue("osc3PW");
	pOsc3Detune = apvts.getRawParameterValue("osc3Detune");
}

void SynthVoice::prepareToPlay(double sampleRate, int) {
	this->sampleRate = (sampleRate > 0.0) ? sampleRate : 44100.0;
	ampEnv.setSampleRate(sampleRate);

	osc1.setSampleRate(sampleRate);
	osc2.setSampleRate(sampleRate);
	osc3.setSampleRate(sampleRate);
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

	auto configureOsc = [this](Oscillator& osc,
		std::atomic<float>* wave,
		std::atomic<float>* level,
		std::atomic<float>* coarse,
		std::atomic<float>* fine,
		std::atomic<float>* pw,
		std::atomic<float>* detune)
		{
			osc.setWaveform(static_cast<Oscillator::Waveform>(static_cast<int>(wave->load())));
			osc.setLevel(level->load());
			osc.setCoarse(static_cast<int>(coarse->load()));
			osc.setFinetine(fine->load());
			osc.setPulseWidth(pw->load());
			osc.setDetuneSpread(detune->load());
			osc.setFrequency(currentFreq);
		};

	configureOsc(osc1, pOsc1Wave, pOsc1Level, pOsc1Coarse, pOsc1Fine, pOsc1PW, pOsc1Detune);
	configureOsc(osc2, pOsc2Wave, pOsc2Level, pOsc2Coarse, pOsc2Fine, pOsc2PW, pOsc2Detune);
	configureOsc(osc3, pOsc3Wave, pOsc3Level, pOsc3Coarse, pOsc3Fine, pOsc3PW, pOsc3Detune);
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

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& output, int startSample, int numSamples)
{
	if (!ampEnv.isActive())
	{
		clearCurrentNote();
		return;
	}

	auto* left = output.getWritePointer(0, startSample);
	auto* right = output.getNumChannels() > 1 ? output.getWritePointer(1, startSample) : nullptr;

	for (int i = 0; i < numSamples; ++i)
	{
		float oscMix = (osc1.getNextSample() + osc2.getNextSample() + osc3.getNextSample()) / 3.0f;

		float env = ampEnv.getNextSample();
		float sample = oscMix * env * level;

		left[i] += sample;
		if (right) right[i] += sample;
	}

	if (!ampEnv.isActive())
		clearCurrentNote();
}

