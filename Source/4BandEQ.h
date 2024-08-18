/*
  ==============================================================================

    4BandEQ.h
    Created: 6 Apr 2024 10:26:51am
    Author:  Tordur Hansen
    Notes: Just a wrapper to turn four IIR filters into one Processor

  ==============================================================================
/*/

#pragma once




class ampEQ : public juce::dsp::ProcessorBase {

public:

    ampEQ();
    ~ampEQ() {}
    float sampleRate;
    int blockSize, numChannels;

    void prepare(const juce::dsp::ProcessSpec&) override;
    void process(const juce::dsp::ProcessContextReplacing<float>&) override;
    void reset() override;

    void setCoefficient(int band);

private:
    juce::dsp::IIR::Filter<float> bands[4];
    //juce::dsp::IIR::Coefficients<float> coeff[4];




};