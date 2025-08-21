#include "AHDSR.h"

void AHDSR::setSampleRate(double sr) {
	this->sampleRate = (sr > 0.0) ? sr : 44100.0f;
}

void AHDSR::reset() {
	state = State::Idle;
	level = 0.0f;
	t = 0.0f;
}

void AHDSR::setParameters(const Params& p) {
	params.attack = std::max(0.0f, p.attack);
	params.hold = std::max(0.0f, p.hold);
	params.decay = std::max(0.0f, p.decay);
	params.sustain = juce::jlimit(0.0f, 1.0f, p.sustain);
	params.release = std::max(0.0f, p.release);
}

void AHDSR::noteOn() {
	state = State::Attack;
	t = 0.0f;
}

void AHDSR::noteOff() {
	state = State::Release;
	t = 0.0f;
}

bool AHDSR::isActive() {
	return state != State::Idle;
}

float AHDSR::getNextSample() {
	const float dt = 1.0 / static_cast<float>(sampleRate);

	switch (state) {
		case State::Idle: return 0.0f;
		case State::Attack: {
			if (params.attack <= 0.0f) {
				level = 1.0f;
				state = State::Hold;
				t = 0.0f;
				return level;
			}

			t += dt;
			level = juce::jlimit(0.0f, 1.0f, t / params.attack);
			if (level >= 1.0f) {
				state = State::Hold;
				t = 0.0f;
			}
			return level;
		}
		case State::Hold: {
			if (params.hold <= 0.0f) {
				state = State::Decay;
				t = 0.0f;
				return level;
			}

			t += dt;
			if (t >= params.hold) {
				state = State::Decay;
				t = 0.0f;
			}
			return level;
		}
		case State::Decay: {
			if (params.decay <= 0.0f) {
				level = params.sustain;
				state = State::Sustain;
				return level;
			}

			t += dt;
			const float a = (params.decay > 0.0f) ? (t / params.decay) : 1.0f;
			level = juce::jlimit(0.0f, 1.0f, 1.0f - (a * (1.0f - params.sustain)));
			if (t >= params.decay) {
				state = State::Sustain;
				level = params.sustain;
			}
			return level;
		}
		case State::Sustain: {
			return params.sustain;
		}
		case State::Release: {
			if (params.release <= 0.0f) {
				level = 0.0f;
				state = State::Idle;
				return level;
			}

			t += dt;
			const double progress = juce::jlimit(0.0f, 1.0f, t / params.release);
			level = static_cast<float>(releaseStartLevel * (1.0 - progress));

			if (progress >= 1.0 || level <= 1.0e-5f) {
				level = 0.0f;
				state = State::Idle;
			}
			return level;
		}
		default: { jassertfalse; return 0.0f; }
	}
}