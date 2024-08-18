/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AmplifierAudioProcessor::AmplifierAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
     )
#endif
{
    chain.template get<shaper>().functionToUse = [](float x) {


        x = std::tanh(std::abs(x)-0.5);//soft clipper

        float y;

        (x < 0.9) ? y = x : y = 0.9;//Hard clipper
        //return  juce::jlimit(-0.75f, 0.75f, sample);

        return x;
        

        };
    
        
        
}

AmplifierAudioProcessor::~AmplifierAudioProcessor()
{

    
}

//==============================================================================
const juce::String AmplifierAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AmplifierAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AmplifierAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AmplifierAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AmplifierAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AmplifierAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AmplifierAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AmplifierAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AmplifierAudioProcessor::getProgramName (int index)
{
    return {};
}

void AmplifierAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AmplifierAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    spec.sampleRate = sampleRate;

    chain.prepare(spec);
    chain.reset();
}

void AmplifierAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AmplifierAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
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

void AmplifierAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    chain.template get<input>().setGainDecibels(apvts.getParameterAsValue("inputID").getValue());
    chain.template get<output>().setGainDecibels(apvts.getParameterAsValue("outputID").getValue());

    juce::dsp::AudioBlock<float> block(buffer);

    chain.process(juce::dsp::ProcessContextReplacing<float>(block));
}

//==============================================================================
bool AmplifierAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AmplifierAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
    //return new AmplifierAudioProcessorEditor (*this);
}

//==============================================================================
void AmplifierAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AmplifierAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AmplifierAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout AmplifierAudioProcessor::setParams() {

    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    juce::NormalisableRange<float> inputRange(0, 46, 0.1, 1);
    juce::NormalisableRange<float> outputRange(-46, 46, 0.1, 1);

    layout.add(
        std::make_unique<juce::AudioParameterFloat>("inputID",
            "input",
            inputRange,
            15.0),


        std::make_unique<juce::AudioParameterFloat>("outputID",
            "output",
            outputRange,
            0)
    );


    return layout;
}
