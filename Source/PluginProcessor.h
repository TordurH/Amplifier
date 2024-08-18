/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "4BandEQ.h"


//==============================================================================
/**
*/
class AmplifierAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AmplifierAudioProcessor();
    ~AmplifierAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:

    enum{
        
        EQ,
        limiter,
        input,
        shaper,
        output,
        impulse
    };

    juce::dsp::ProcessorChain<
        ampEQ, //4 band EQ defined in 4BandEQ.h
        juce::dsp::Compressor<float>, 
        juce::dsp::Gain<float>, 
        juce::dsp::WaveShaper<float>, 
        juce::dsp::Gain<float>, 
        juce::dsp::Convolution
    > chain;


    juce::AudioProcessorValueTreeState apvts{*this, nullptr, "PARAMS", setParams()};

    static juce::AudioProcessorValueTreeState::ParameterLayout setParams();
    


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmplifierAudioProcessor)
};
