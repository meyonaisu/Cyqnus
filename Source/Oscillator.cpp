#include "Oscillator.h"

Oscillator::Oscillator() {}

void Oscillator::setSampleRate(double sr) {
	sampleRate = (sr > 0.0) ? sr : 44100.0;
	updatePhaseIncrement();
}

void Oscillator::setFrequency(float freq) {
	frequency = juce::jmax(0.0f, freq);
	updatePhaseIncrement();
}

void Oscillator::setWaveform(Waveform wf) {
	waveform = wf;
}

void Oscillator::setLevel(float lvl) {
	level = juce::jlimit(0.0f, 1.0f, lvl);
}

void Oscillator::setCoarse(int semis) {
	coarse = semis;
	updatePhaseIncrement();
}

void Oscillator::setFinetine(float cents) {
	fine = juce::jlimit(-100.0f, 100.0f, cents);
	updatePhaseIncrement();
}

void Oscillator::setPhaseOffset(float rads) {
	phase = rads;
	wrapPhase();
}

void Oscillator::setPulseWidth(float pw) {
	pulseWidth = juce::jlimit(0.01f, 0.99f, pw);
}

void Oscillator::setDetuneSpread(float speedHz) {
	detuneSpread = juce::jmax(0.0f, speedHz);
}

float Oscillator::getNextSample() {
	float sample = 0.0f;

	switch (waveform) {
		case Sine:
			sample = std::sin(phase);
			break;
		case Saw:
			sample = 1.0f - (2.0f * phase / twoPi);
			break;
		case Square:
			sample = (phase < juce::MathConstants<float>::pi) ? 1.0f : -1.0f;
			break;
		case Triangle:
			if (phase < juce::MathConstants<float>::pi)
				sample = -1.0f + (2.0f * phase / juce::MathConstants<float>::pi);
			else
				sample = 3.0f - (2.0f * phase / juce::MathConstants<float>::pi);
			break;
		case Pulse:
			sample = (phase < (twoPi * pulseWidth)) ? 1.0f : -1.0f;
			break;
		case Noise:
			sample = random.nextFloat() * 2.0f - 1.0f;
			break;
		default: jassertfalse; break;
	}

	phase += phaseInc;
	wrapPhase();

	return sample * level;
}

void Oscillator::updatePhaseIncrement() {
	float semitoneRatio = std::pow(2.0f, coarse / 12.0f);
	float centsRatio = std::pow(2.0f, fine / 1200.0f);

	float adjustedFrequency = (frequency * semitoneRatio * centsRatio) + detuneSpread;
	phaseInc = (twoPi * adjustedFrequency) / sampleRate;
}

void Oscillator::wrapPhase() {
	if (phase >= twoPi)
		phase -= twoPi;
	else if (phase < 0.0f)
		phase += twoPi;
}