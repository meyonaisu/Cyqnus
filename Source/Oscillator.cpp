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

void Oscillator::setFinetune(float cents) {
	fine = juce::jlimit(-100.0f, 100.0f, cents);
	updatePhaseIncrement();
}

void Oscillator::setPhaseOffset(float offset) {
	phase = offset;
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
	const float twoPi = static_cast<float>(juce::MathConstants<double>::twoPi);

	switch (waveform) {
	case Sine:
		sample = std::sin(phase * twoPi);
		break;
	case Saw:
		sample = 1.0f - 2.0f * phase;
		break;
	case Square:
		sample = (phase < 0.5f) ? 1.0f : -1.0f;
		break;
	case Triangle:
		if (phase < 0.5f)
			sample = -1.0f + 4.0f * phase;
		else
			sample = 3.0f - 4.0f * phase;
		break;
	case Pulse:
		sample = (phase < pulseWidth) ? 1.0f : -1.0f;
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

	phaseInc = adjustedFrequency / static_cast<float>(sampleRate);
}

void Oscillator::wrapPhase() {
	while (phase >= 1.0f)
		phase -= 1.0f;
	while (phase < 0.0f)
		phase += 1.0f;
}