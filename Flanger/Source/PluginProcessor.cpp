/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FlangerAudioProcessor::FlangerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
}

FlangerAudioProcessor::~FlangerAudioProcessor()
{
}

//==============================================================================
const juce::String FlangerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}


bool FlangerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FlangerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FlangerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FlangerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FlangerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FlangerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FlangerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String FlangerAudioProcessor::getProgramName (int index)
{
    return {};
}

void FlangerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void FlangerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    
    const int totalNumInputChannels  = getTotalNumInputChannels();
    
    //Sample Rate from the DAW
    SR = sampleRate;
    inverseSR = 1.0f / (float)SR;
    
    // MAX DELAY in sec - SUM of max width and max delay offset
    float maxDelayTime = 40.0f / 1000.0f;
    
    //init of the circular delay buffer
    delayBufferSamples = (int)(maxDelayTime * (float)SR) + 1;
    delayBuffer.setSize(totalNumInputChannels, delayBufferSamples);
    delayBuffer.clear();
    
    //writing position of the delay buff
    dpw = 0;
    
    //init of the LFO phase
    ph = 0.0f;

}

void FlangerAudioProcessor::releaseResources()
{

}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FlangerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else

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

void FlangerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    //the processing is computed if BYPASS is disable
    if(*apvts.getRawParameterValue("BYPASS") == false)
    {
        
        juce::ScopedNoDenormals noDenormals;
        
        //retrieve information from the DAW
        const int totalNumInputChannels  = getTotalNumInputChannels();
        const int totalNumOutputChannels = getTotalNumOutputChannels();
        const int numSamples = buffer.getNumSamples();
    
        //retrieve information from the GUI
        float currDepth     = *apvts.getRawParameterValue("DEPTH");
        float currWidth     = *apvts.getRawParameterValue("WIDTH");
        float currDelay     = *apvts.getRawParameterValue("DELAY");
        float currFreq      = *apvts.getRawParameterValue("FREQUENCY");
        float currFeedback  = *apvts.getRawParameterValue("FEEDBACK");
        int WaveformIndex   = *apvts.getRawParameterValue("WAVEFORM");
        
        //variables used locally
        float localPhase;
        float phaseUpdate;
        int dpwLocal;
    
        //check of feedback polarity
        float currInverter = 1;
        if (*apvts.getRawParameterValue("INVERTER") == false)
            currInverter = -1;
    
        //values from ms to s
        currWidth = currWidth/1000.0f;
        currDelay = currDelay/1000.0f;
      
        //main loop over the number of the INPUT CH
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            //pointers to the buffers IO
            float* channelInData = buffer.getWritePointer(channel);
            float* channelOutData = buffer.getWritePointer(channel);
            
            //pointer to the circular buffer
            float* delayData = delayBuffer.getWritePointer(channel);
            
            localPhase = ph;
            dpwLocal = dpw;
            
            //if MONO == false --> OUTPUT becomes STEREO
            if (*apvts.getRawParameterValue("MONO") == false && channel !=0)
                localPhase = fmodf(localPhase + 0.25f, 1.0f);
            
            //loop over the number of samples in the audio buffer
            for (int sample = 0; sample < numSamples; ++sample)
            {
                //value of the current sample
                const float in = channelInData[sample];
                float out = 0.0f;
                
                //computes the current total delay in samples
                float localDelaySamples = SR * (currDelay + currWidth * (0.5f + 0.5f * LfoWaveform (localPhase, WaveformIndex)));
            
                //local variable for current reading position of the circular buffer - could be non int
                float readPosition = fmodf ((float)dpwLocal - localDelaySamples + (float)delayBufferSamples -3, delayBufferSamples);
                
                //takes decimal part of the reading position value
                float fraction = readPosition - floorf(readPosition);
                
                //explicit cast float to int of the floored reading position value: it becames an INDEX!
                int localReadPosition = (int)floorf(readPosition);
                //next INDEX
                int nextSample = (localReadPosition + 1) % delayBufferSamples;
                
                //LINEAR INTERPOLATION
                out = fraction * delayData[nextSample] + (1.0f-fraction) * delayData[localReadPosition];

                //feedback sample
                delayData[dpwLocal] = in + out * currFeedback * currInverter;
                
                //update the local writing position
                if (++dpwLocal >= delayBufferSamples) dpwLocal -= delayBufferSamples;
            
                //output sample
                channelOutData[sample] = in + out * currDepth;
            
                //update of the LFO local phase
                localPhase += currFreq * inverseSR;
                if (localPhase >= 1.0f) localPhase -= 1.0f;
            }
            
            //after the first cycle of the INPUT CH loop
            //the value of the localPhase is saved
            if (channel == 0)
                phaseUpdate = localPhase;
        }
        //in order to use it in the next audio buffer loop
        ph = phaseUpdate;
        dpw = dpwLocal;
    
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    }

}


// Waveform Choices Function
float FlangerAudioProcessor::LfoWaveform (float LfoPhase, int waveformChoice)
{
    float WaveOutput = 0.0f;

    switch (waveformChoice) {
            
        case Sine: {
            WaveOutput = 0.5f + 0.5f * sinf (2.0f * 3.14 * LfoPhase);
            break;
        }
        case Triangle: {
            if (LfoPhase < 0.25f)
                WaveOutput = 0.5f + 2.0f * LfoPhase;
            else if (LfoPhase < 0.75f)
                WaveOutput = 1.0f - 2.0f * (LfoPhase - 0.25f);
            else
                WaveOutput = 2.0f * (LfoPhase - 0.75f);
            break;
        }
        case Sawtooth_Up: {
            if (LfoPhase < 0.5f)
                WaveOutput = 0.5f + LfoPhase;
            else
                WaveOutput = LfoPhase - 0.5f;
            break;
        }
        case Sawtooth_Down: {
            if (LfoPhase < 0.5f)
                WaveOutput = 0.5f - LfoPhase;
            else
                WaveOutput = 1.5f - LfoPhase;
            break;
        }
    }

    return WaveOutput;
}


//==============================================================================
bool FlangerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FlangerAudioProcessor::createEditor()
{
    return new FlangerAudioProcessorEditor (*this);
}

//==============================================================================
void FlangerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{

}

void FlangerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{

}

//==============================================================================

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FlangerAudioProcessor();
}

//implement the function that creates the parameters vector
juce::AudioProcessorValueTreeState::ParameterLayout
    FlangerAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Definition of each params added in the ValueTree
    
    //Gfb   - DEPTH (MIX)
    params.push_back(std::make_unique<juce::AudioParameterFloat>
                     ("DEPTH", "depth", 0.0f, 1.0f, 0.5f));
    //Gff   - FEEDBACK GAIN
    params.push_back(std::make_unique<juce::AudioParameterFloat>
                     ("FEEDBACK", "feedback", 0.0f, 0.9f, 0.45f));
    //Mw    - LFO WIDTH
    params.push_back(std::make_unique<juce::AudioParameterFloat>
                     ("WIDTH", "width", 1.0f, 20.0f, 10.5f));
    
    //M0    - DELAY TIME OFFSET
    params.push_back(std::make_unique<juce::AudioParameterFloat>
                     ("DELAY", "delay", 1.0f, 20.0f, 10.5f));
        
    //fLFO  - LFO FREQUENCY
    params.push_back(std::make_unique<juce::AudioParameterFloat>
                     ("FREQUENCY", "frequency", 0.05f, 2.0f, 1.03f));
    
    //POLARITY INVERTER
    params.push_back(std::make_unique<juce::AudioParameterBool>
                     ("INVERTER", "inverter", true));
        
    //LFO WAVEFORM
    params.push_back(std::make_unique<juce::AudioParameterChoice>
                     ("WAVEFORM", "waveform", juce::StringArray("Sine","Triangle","Sawtooth-Up","Sawtooth-Down"), Sine));
        
    //BYPASS
    params.push_back(std::make_unique<juce::AudioParameterBool>
                         ("BYPASS", "bypass", false));
        
    //MONO/STEREO CHOICE
    params.push_back(std::make_unique<juce::AudioParameterBool>
                             ("MONO", "mono", false));
   

    return {params.begin(), params.end()};
}
