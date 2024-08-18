/*
  ==============================================================================

    4BandEQ.cpp
    Created: 6 Apr 2024 10:26:51am
    Author:  Bboy_

  ==============================================================================
  */
#include <JuceHeader.h>
#include "4BandEQ.h"

ampEQ::ampEQ() {

    sampleRate = 44100;
    numChannels = 2;
    blockSize = 1024;

    /*for (int i = 0; i < 4; ++i) {
        bands[i].coefficients = juce::dsp::IIR::Coefficients<float>::makeAllPass(sampleRate, 700);

    }*/


}

void ampEQ::prepare(const juce::dsp::ProcessSpec& spec) {

    sampleRate = spec.sampleRate;
    numChannels = spec.numChannels;
    blockSize = spec.maximumBlockSize;

    //Set filter types. They're constant in this one
    bands[0].coefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, 64.0, 6.7, juce::Decibels::decibelsToGain( - 24.0));
    bands[1].coefficients = juce::dsp::IIR::Coefficients<float>::makeAllPass(sampleRate, 360.0, 0.2);
    bands[2].coefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, 2740.0, 1.0);
    bands[3].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, 8920,1, juce::Decibels::decibelsToGain(-24.0));
    
    for (int i = 0; i < 4; ++i) {
        bands[i].prepare(spec);
    }
}

void ampEQ::process(const juce::dsp::ProcessContextReplacing<float>& context){

    for (int i = 0; i < 4; ++i) {
        bands[i].process(context);
    }
}
void ampEQ::reset() {

    for (int i = 0; i < 4; ++i) {
        bands[i].reset();
    }
}

void ampEQ::setCoefficient(int band) {

}

