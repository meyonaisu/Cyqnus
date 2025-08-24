/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

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

    juce::Slider attack, hold, decay, sustain, release, masterGain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> aAttack, aHold, aDecay, aSustain, aRelease, aGain;

    juce::ComboBox osc1Wave;
    juce::Slider osc1Level, osc1Coarse, osc1Fine, osc1PW, osc1Detune;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> aOsc1Wave;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> aOsc1Level, aOsc1Coarse, aOsc1Fine, aOsc1PW, aOsc1Detune;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CyqnusAudioProcessorEditor)
};
