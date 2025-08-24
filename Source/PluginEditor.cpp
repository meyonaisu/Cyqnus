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
CyqnusAudioProcessorEditor::CyqnusAudioProcessorEditor(CyqnusAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), keyboardComponent(audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setSize(800, 800);
    addAndMakeVisible(keyboardComponent);

    auto& apvts = audioProcessor.apvts;

    configKnob(attack);     addAndMakeVisible(attack);
    configKnob(hold);       addAndMakeVisible(hold);
    configKnob(decay);      addAndMakeVisible(decay);
    configKnob(sustain);    addAndMakeVisible(sustain);
    configKnob(release);    addAndMakeVisible(release);

    masterGain.setSliderStyle(juce::Slider::LinearHorizontal);
    masterGain.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 18);
    addAndMakeVisible(masterGain);

    aAttack = std::make_unique<SliderAttachment>(apvts, "ampAttack", attack);
    aHold = std::make_unique<SliderAttachment>(apvts, "ampHold", hold);
    aDecay = std::make_unique<SliderAttachment>(apvts, "ampDecay", decay);
    aSustain = std::make_unique<SliderAttachment>(apvts, "ampSustain", sustain);
    aRelease = std::make_unique<SliderAttachment>(apvts, "ampRelease", release);
    aGain = std::make_unique<SliderAttachment>(apvts, "masterGain", masterGain);

    osc1Wave.addItemList(juce::StringArray{ "Sine", "Saw", "Square", "Triangle", "Pulse", "Noise" }, 1);
    addAndMakeVisible(osc1Wave);
    configKnob(osc1Level);  addAndMakeVisible(osc1Level);
    configKnob(osc1Coarse); addAndMakeVisible(osc1Coarse);
    configKnob(osc1Fine);   addAndMakeVisible(osc1Fine);
    configKnob(osc1PW);     addAndMakeVisible(osc1PW);
    configKnob(osc1Detune); addAndMakeVisible(osc1Detune);

    aOsc1Wave = std::make_unique<ComboBoxAttachment>(apvts, "osc1Wave", osc1Wave);
    aOsc1Level = std::make_unique<SliderAttachment>(apvts, "osc1Level", osc1Level);
    aOsc1Coarse = std::make_unique<SliderAttachment>(apvts, "osc1Coarse", osc1Coarse);
    aOsc1Fine = std::make_unique<SliderAttachment>(apvts, "osc1Fine", osc1Fine);
    aOsc1PW = std::make_unique<SliderAttachment>(apvts, "osc1PW", osc1PW);
    aOsc1Detune = std::make_unique<SliderAttachment>(apvts, "osc1Detune", osc1Detune);

    osc2Wave.addItemList(juce::StringArray{ "Sine", "Saw", "Square", "Triangle", "Pulse", "Noise" }, 1);
    addAndMakeVisible(osc2Wave);
    configKnob(osc2Level);  addAndMakeVisible(osc2Level);
    configKnob(osc2Coarse); addAndMakeVisible(osc2Coarse);
    configKnob(osc2Fine);   addAndMakeVisible(osc2Fine);
    configKnob(osc2PW);     addAndMakeVisible(osc2PW);
    configKnob(osc2Detune); addAndMakeVisible(osc2Detune);

    aOsc2Wave = std::make_unique<ComboBoxAttachment>(apvts, "osc2Wave", osc2Wave);
    aOsc2Level = std::make_unique<SliderAttachment>(apvts, "osc2Level", osc2Level);
    aOsc2Coarse = std::make_unique<SliderAttachment>(apvts, "osc2Coarse", osc2Coarse);
    aOsc2Fine = std::make_unique<SliderAttachment>(apvts, "osc2Fine", osc2Fine);
    aOsc2PW = std::make_unique<SliderAttachment>(apvts, "osc2PW", osc2PW);
    aOsc2Detune = std::make_unique<SliderAttachment>(apvts, "osc2Detune", osc2Detune);

    osc3Wave.addItemList(juce::StringArray{ "Sine", "Saw", "Square", "Triangle", "Pulse", "Noise" }, 1);
    addAndMakeVisible(osc3Wave);
    configKnob(osc3Level);  addAndMakeVisible(osc3Level);
    configKnob(osc3Coarse); addAndMakeVisible(osc3Coarse);
    configKnob(osc3Fine);   addAndMakeVisible(osc3Fine);
    configKnob(osc3PW);     addAndMakeVisible(osc3PW);
    configKnob(osc3Detune); addAndMakeVisible(osc3Detune);

    aOsc3Wave = std::make_unique<ComboBoxAttachment>(apvts, "osc3Wave", osc3Wave);
    aOsc3Level = std::make_unique<SliderAttachment>(apvts, "osc3Level", osc3Level);
    aOsc3Coarse = std::make_unique<SliderAttachment>(apvts, "osc3Coarse", osc3Coarse);
    aOsc3Fine = std::make_unique<SliderAttachment>(apvts, "osc3Fine", osc3Fine);
    aOsc3PW = std::make_unique<SliderAttachment>(apvts, "osc3PW", osc3PW);
    aOsc3Detune = std::make_unique<SliderAttachment>(apvts, "osc3Detune", osc3Detune);
}

CyqnusAudioProcessorEditor::~CyqnusAudioProcessorEditor()
{
}

//==============================================================================
void CyqnusAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black); // background

    g.setFont(15.0f);
    g.setColour(juce::Colours::white);
    // === Section headers ===
    g.drawFittedText("AMPLIFIER ENVELOPE", { 10,  5, 200, 20 }, juce::Justification::left, 1);
    g.drawFittedText("MASTER", { 570,  5, 200, 20 }, juce::Justification::left, 1);

    g.drawFittedText("OSCILLATORS", { 10, 140, 200, 20 }, juce::Justification::left, 1);
    g.drawFittedText("Oscillator 1", { 10, 165, 200, 20 }, juce::Justification::left, 1);
    g.drawFittedText("Oscillator 2", { 10, 285, 200, 20 }, juce::Justification::left, 1);
    g.drawFittedText("Oscillator 3", { 10, 405, 200, 20 }, juce::Justification::left, 1);

    g.setFont(13.0f);
    g.setColour(juce::Colours::grey);
    // === Envelope labels ===
    g.drawFittedText("Attack", { 10,  95, 100, 20 }, juce::Justification::centredTop, 1);
    g.drawFittedText("Hold", { 120,  95, 100, 20 }, juce::Justification::centredTop, 1);
    g.drawFittedText("Decay", { 230,  95, 100, 20 }, juce::Justification::centredTop, 1);
    g.drawFittedText("Sustain", { 340,  95, 100, 20 }, juce::Justification::centredTop, 1);
    g.drawFittedText("Release", { 450,  95, 100, 20 }, juce::Justification::centredTop, 1);
    g.drawFittedText("Gain", { 560,  95, 100, 20 }, juce::Justification::centredTop, 1);
    // === Oscillator column headers (applies to all 3 rows) ===
    g.drawFittedText("Waveform", { 10, 200, 100, 20 }, juce::Justification::centredTop, 1);
    g.drawFittedText("Level", { 120, 200, 100, 20 }, juce::Justification::centredTop, 1);
    g.drawFittedText("Coarse", { 230, 200, 100, 20 }, juce::Justification::centredTop, 1);
    g.drawFittedText("Fine", { 340, 200, 100, 20 }, juce::Justification::centredTop, 1);
    g.drawFittedText("Pulse W.", { 450, 200, 100, 20 }, juce::Justification::centredTop, 1);
    g.drawFittedText("Detune", { 560, 200, 100, 20 }, juce::Justification::centredTop, 1);

    // draw dividing lines for clarity
    g.setColour(juce::Colours::darkgrey);
    g.drawLine(0.0f, 135.0f, (float)getWidth(), 135.0f, 1.0f); // line under env
    g.drawLine(0.0f, 260.0f, (float)getWidth(), 260.0f, 0.5f); // line under osc1
    g.drawLine(0.0f, 380.0f, (float)getWidth(), 380.0f, 0.5f); // line under osc2

    // Add a section header for the keyboard
    g.setFont(15.0f);
    g.setColour(juce::Colours::white);
    g.drawFittedText("KEYBOARD", { 10, 525, 200, 20 }, juce::Justification::left, 1);
}

void CyqnusAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10);

    auto envRow = area.removeFromTop(120);
    int knobW = 90, knobH = 90;
    int x = 10;
    attack.setBounds(x, envRow.getY(), knobW, knobH); x += 100;
    hold.setBounds(x, envRow.getY(), knobW, knobH); x += 100;
    decay.setBounds(x, envRow.getY(), knobW, knobH); x += 100;
    sustain.setBounds(x, envRow.getY(), knobW, knobH);
    x += 100;
    release.setBounds(x, envRow.getY(), knobW, knobH); x += 100;

    masterGain.setBounds(x, envRow.getY(), knobW, knobH);

    area.removeFromTop(20);
    auto placeOscRow = [&](auto& wave, auto& level, auto& coarse, auto& fine, auto& pw, auto& detune, int rowY)
        {
            wave.setBounds(10, rowY, 100, 24);
            level.setBounds(120, rowY, knobW, knobH);
            coarse.setBounds(230, rowY, knobW, knobH);
            fine.setBounds(340, rowY, knobW, knobH);
            pw.setBounds(450, rowY, knobW, knobH);
            detune.setBounds(560, rowY, knobW, knobH);
        };

    int oscRowHeight = 120;
    placeOscRow(osc1Wave, osc1Level, osc1Coarse, osc1Fine, osc1PW, osc1Detune, area.removeFromTop(oscRowHeight).getY());
    placeOscRow(osc2Wave, osc2Level, osc2Coarse, osc2Fine, osc2PW, osc2Detune, area.removeFromTop(oscRowHeight).getY());
    placeOscRow(osc3Wave, osc3Level, osc3Coarse, osc3Fine, osc3PW, osc3Detune, area.removeFromTop(oscRowHeight).getY());

    // Position the keyboard at the bottom
    keyboardComponent.setBounds(10, 550, getWidth() - 20, 100);
}