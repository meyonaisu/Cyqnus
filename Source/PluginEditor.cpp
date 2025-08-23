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

    osc1Wave.addItemList(juce::StringArray{ "Sine", "Saw", "Square", "Triangle", "Pulse", "Noise" }, 1);
    addAndMakeVisible(osc1Wave);
    configKnob(osc1Level);  addAndMakeVisible(osc1Level);
    configKnob(osc1Coarse); addAndMakeVisible(osc1Coarse);
    configKnob(osc1Fine);   addAndMakeVisible(osc1Fine);
    configKnob(osc1PW);     addAndMakeVisible(osc1PW);
    configKnob(osc1Detune); addAndMakeVisible(osc1Detune);

    aOsc1Wave = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, "osc1Wave", osc1Wave);
    aOsc1Level = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "osc1Level", osc1Level);
    aOsc1Coarse = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "osc1Coarse", osc1Coarse);
    aOsc1Fine = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "osc1Fine", osc1Fine);
    aOsc1PW = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "osc1PW", osc1PW);
    aOsc1Detune = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "osc1Detune", osc1Detune);

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

    g.setFont(13.0f);
    g.setColour(juce::Colours::grey);
    g.drawFittedText("Osc 1 Wave", { 10, 210, 100, 20 }, juce::Justification::centredTop, 1);
    g.drawFittedText("Level", { 120, 210, 100, 20 }, juce::Justification::centredTop, 1);
    g.drawFittedText("Coarse", { 230, 210, 100, 20 }, juce::Justification::centredTop, 1);
    g.drawFittedText("Fine", { 340, 210, 100, 20 }, juce::Justification::centredTop, 1);
    g.drawFittedText("PulseW", { 450, 210, 100, 20 }, juce::Justification::centredTop, 1);
    g.drawFittedText("Detune", { 560, 210, 100, 20 }, juce::Justification::centredTop, 1);
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

    // Now place Osc1 row
    int rowY = 180; // or compute from area
    osc1Wave.setBounds(10, rowY, 100, 24);
    osc1Level.setBounds(120, rowY, w, h);
    osc1Coarse.setBounds(230, rowY, w, h);
    osc1Fine.setBounds(340, rowY, w, h);
    osc1PW.setBounds(450, rowY, w, h);
    osc1Detune.setBounds(560, rowY, w, h);
}
