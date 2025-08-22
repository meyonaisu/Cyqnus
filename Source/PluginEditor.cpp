/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

static void configKnob(juce::Slider& s) {
    s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
}

//==============================================================================
CyqnusAudioProcessorEditor::CyqnusAudioProcessorEditor (CyqnusAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (540, 260);

	auto& apvts = audioProcessor.apvts;

	configKnob(attack);     addAndMakeVisible(attack);
	configKnob(hold);       addAndMakeVisible(hold);
	configKnob(decay);      addAndMakeVisible(decay);
	configKnob(sustain);    addAndMakeVisible(sustain);
	configKnob(release);    addAndMakeVisible(release);
    
	masterGain.setSliderStyle(juce::Slider::LinearHorizontal);
    masterGain.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 18);
    addAndMakeVisible(masterGain);

    aAttack  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampAttack", attack);
    aHold    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampHold", hold);
    aDecay   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampDecay", decay);
    aSustain = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampSustain", sustain);
    aRelease = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "ampRelease", release);
	aGain    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "masterGain", masterGain);
}

CyqnusAudioProcessorEditor::~CyqnusAudioProcessorEditor()
{
}

//==============================================================================
void CyqnusAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(16.0f);
    g.drawFittedText("Phase 1: 1-osc + AHDSR", getLocalBounds().reduced(8), juce::Justification::centredTop, 1);

    g.setFont(13.0f);
    auto label = [](juce::Graphics& gg, juce::Rectangle<int> r, const juce::String& text)
        {
            gg.setColour(juce::Colours::grey);
            gg.drawFittedText(text, r, juce::Justification::centredTop, 1);
        };

    label(g, { 10, 70, 100, 20 }, "Attack");
    label(g, { 120, 70, 100, 20 }, "Hold");
    label(g, { 230, 70, 100, 20 }, "Decay");
    label(g, { 340, 70, 100, 20 }, "Sustain");
    label(g, { 450, 70, 100, 20 }, "Release");
    label(g, { 10, 190, 520, 20 }, "Master Gain");
}

void CyqnusAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    auto top = area.removeFromTop(50);
    juce::ignoreUnused(top);

    auto knobRow = area.removeFromTop(120);
    auto w = 100, h = 90, y = knobRow.getY() + 20;

	attack.setBounds(10, y, w, h);
    hold.setBounds(120, y, w, h);
    decay.setBounds(230, y, w, h);
    sustain.setBounds(340, y, w, h);
    release.setBounds(450, y, w, h);

    area.removeFromTop(10);
	masterGain.setBounds(area.removeFromTop(40));
}
