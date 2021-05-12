/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================

class FlangerAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    FlangerAudioProcessor();
    ~FlangerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    juce::AudioProcessorValueTreeState extracted();
    
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
    
    //define the indexes associated to the waveform list
    enum waveformIndex {
        Sine=0,
        Triangle=1,
        Sawtooth_Up=2,
        Sawtooth_Down=3,
    };
    
    //declaration of the function for the choice of the LFO waveform
    float LfoWaveform (float LfoPhase, int waveformChoice);
    
    //declaration of the ValueTreeState
    //and the function for the definition and the initialization of the parameters
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();

private:
    //==============================================================================
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FlangerAudioProcessor)
    
    //declaration of the circular buffer
    juce::AudioSampleBuffer delayBuffer;
    
    //declaration of the size of the circular buffer
    int delayBufferSamples;
    
    //index for the writing position in the circular buffer
    int dpw;
    
    //SR to be retrieved from the DAW
    int SR;
    float inverseSR;
    
    //global phase declaration
    float ph;
   
};
