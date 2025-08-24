#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class CyqnusAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    explicit CyqnusAudioProcessorEditor (CyqnusAudioProcessor&);
    ~CyqnusAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    CyqnusAudioProcessor& audioProcessor;

    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
	using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    juce::Slider attack, hold, decay, sustain, release, masterGain;
    std::unique_ptr<SliderAttachment> aAttack, aHold, aDecay, aSustain, aRelease, aGain;

    juce::ComboBox osc1Wave, osc2Wave, osc3Wave;
    juce::Slider osc1Level, osc1Coarse, osc1Fine, osc1PW, osc1Detune,
                 osc2Level, osc2Coarse, osc2Fine, osc2PW, osc2Detune,
                 osc3Level, osc3Coarse, osc3Fine, osc3PW, osc3Detune;

    std::unique_ptr<ComboBoxAttachment> aOsc1Wave, aOsc2Wave, aOsc3Wave;
    std::unique_ptr<SliderAttachment> aOsc1Level, aOsc1Coarse, aOsc1Fine, aOsc1PW, aOsc1Detune,
                                      aOsc2Level, aOsc2Coarse, aOsc2Fine, aOsc2PW, aOsc2Detune,
                                      aOsc3Level, aOsc3Coarse, aOsc3Fine, aOsc3PW, aOsc3Detune;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CyqnusAudioProcessorEditor)
};
