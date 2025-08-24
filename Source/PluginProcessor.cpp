/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CyqnusAudioProcessor::CyqnusAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    apvts(*this, nullptr, "PARAMS", createParameterLayout())
#endif
{
    for (int i = 0; i < kNumVoice; ++i) {
        synth.addVoice(new SynthVoice(apvts));
    }
    synth.addSound(new SynthSound());
}

CyqnusAudioProcessor::~CyqnusAudioProcessor()
{
}

//==============================================================================
const juce::String CyqnusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CyqnusAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool CyqnusAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool CyqnusAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double CyqnusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CyqnusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int CyqnusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CyqnusAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String CyqnusAudioProcessor::getProgramName(int index)
{
    return {};
}

void CyqnusAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void CyqnusAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (int i = 0; i < synth.getNumVoices(); ++i)
        if (auto* voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            voice->prepareToPlay(sampleRate, samplesPerBlock);

    procSpec.sampleRate = sampleRate;
    procSpec.maximumBlockSize = samplesPerBlock;
    procSpec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    masterGain.prepare(procSpec);
    masterGain.setRampDurationSeconds(0.05);
}

void CyqnusAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CyqnusAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void CyqnusAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    juce::MidiBuffer keyboardMidiMessages;
    keyboardState.processNextMidiBuffer(keyboardMidiMessages, 0, buffer.getNumSamples(), true);

    midiMessages.addEvents(keyboardMidiMessages, 0, buffer.getNumSamples(), 0);

    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    const float gain = apvts.getRawParameterValue("masterGain")->load();
    masterGain.setGainLinear(gain);

    juce::dsp::AudioBlock<float> block(buffer);
    masterGain.process(juce::dsp::ProcessContextReplacing<float>(block));
}

//==============================================================================
bool CyqnusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* CyqnusAudioProcessor::createEditor()
{
    return new CyqnusAudioProcessorEditor(*this);
}

//==============================================================================
void CyqnusAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    if (auto xml = state.createXml())
        copyXmlToBinary(*xml, destData);
}

void CyqnusAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CyqnusAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout CyqnusAudioProcessor::createParameterLayout()
{
    using FloatParam = juce::AudioParameterFloat;
    using Range = juce::NormalisableRange<float>;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    auto secondsRange = Range(0.001f, 32.0f, 0.0f, 0.25f);
    auto sustainRange = Range(0.0f, 1.0f);
    auto gainRange = Range(0.0f, 1.0f);

    params.push_back(std::make_unique<FloatParam>("ampAttack", "Attack", secondsRange, 0.01f));
    params.push_back(std::make_unique<FloatParam>("ampHold", "Hold", secondsRange, 0.0f));
    params.push_back(std::make_unique<FloatParam>("ampDecay", "Decay", secondsRange, 1.0f));
    params.push_back(std::make_unique<FloatParam>("ampSustain", "Sustain", sustainRange, 0.5f));
    params.push_back(std::make_unique<FloatParam>("ampRelease", "Release", secondsRange, 0.01f));

    params.push_back(std::make_unique<FloatParam>("masterGain", "Master Gain", gainRange, 0.8f));

    auto oscWaveChoices = juce::StringArray{ "Sine", "Saw", "Square", "Triangle", "Pulse", "Noise" };
    auto oscLevelRange = Range{ 0.0f, 1.0f };
    auto oscCoarseRange = Range{ -24.0f, 24.0f, 1.0f };
    auto oscFineRange = Range{ -100.0f, 100.0f };
    auto oscPWRange = Range{ 0.01f, 0.99f };
    auto oscDetuneRange = Range{ 0.0f, 10.0f };

    // o1
    params.push_back(std::make_unique<juce::AudioParameterChoice>("osc1Wave", "Osc 1 Waveform", oscWaveChoices, 0));
    params.push_back(std::make_unique<FloatParam>("osc1Level", "Osc 1 Level", oscLevelRange, 0.8f));
    params.push_back(std::make_unique<FloatParam>("osc1Coarse", "Osc 1 Coarse", oscCoarseRange, 0.0f));
    params.push_back(std::make_unique<FloatParam>("osc1Fine", "Osc 1 Fine", oscFineRange, 0.0f));
    params.push_back(std::make_unique<FloatParam>("osc1PW", "Osc 1 PulseWidth", oscPWRange, 0.5f));
    params.push_back(std::make_unique<FloatParam>("osc1Detune", "Osc 1 Detune", oscDetuneRange, 0.0f));
    // o2
    params.push_back(std::make_unique<juce::AudioParameterChoice>("osc2Wave", "Osc 2 Waveform", oscWaveChoices, 0));
    params.push_back(std::make_unique<FloatParam>("osc2Level", "Osc 2 Level", oscLevelRange, 0.8f));
    params.push_back(std::make_unique<FloatParam>("osc2Coarse", "Osc 2 Coarse", oscCoarseRange, 12.0f));
    params.push_back(std::make_unique<FloatParam>("osc2Fine", "Osc 2 Fine", oscFineRange, 0.0f));
    params.push_back(std::make_unique<FloatParam>("osc2PW", "Osc 2 PulseWidth", oscPWRange, 0.5f));
    params.push_back(std::make_unique<FloatParam>("osc2Detune", "Osc 2 Detune", oscDetuneRange, 0.0f));
    // o3
    params.push_back(std::make_unique<juce::AudioParameterChoice>("osc3Wave", "Osc 3 Waveform", oscWaveChoices, 0));
    params.push_back(std::make_unique<FloatParam>("osc3Level", "Osc 3 Level", oscLevelRange, 0.8f));
    params.push_back(std::make_unique<FloatParam>("osc3Coarse", "Osc 3 Coarse", oscCoarseRange, 24.0f));
    params.push_back(std::make_unique<FloatParam>("osc3Fine", "Osc 3 Fine", oscFineRange, 0.0f));
    params.push_back(std::make_unique<FloatParam>("osc3PW", "Osc 3 PulseWidth", oscPWRange, 0.5f));
    params.push_back(std::make_unique<FloatParam>("osc3Detune", "Osc 3 Detune", oscDetuneRange, 0.0f));

    return { params.begin(), params.end() };
}