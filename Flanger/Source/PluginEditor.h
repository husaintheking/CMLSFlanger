/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class FlangerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    FlangerAudioProcessorEditor (FlangerAudioProcessor&);
    ~FlangerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    //declaration of all graphic components
    juce::Slider depthSlider;
    juce::Slider widthSlider;
    juce::Slider delaySlider;
    juce::Slider frequencySlider;
    juce::Slider feedbackSlider;

    juce::Label depthLabel;
    juce::Label widthLabel;
    juce::Label delayLabel;
    juce::Label frequencyLabel;
    juce::Label feedbackLabel;
    juce::Label inverterLabel;
    juce::Label waveLabel;
    juce::Label bypassLabel;

    juce::Label TitleLabel;
    juce::TextButton inverterButton;
    juce::TextButton bypassButton;
    juce::TextButton monoButton;
    juce::ComboBox waveChoice;

    //background pic
    juce::ImageComponent mImageComponent;
    
    
    //declaration of all the attachment between GUI and ValueTreeState
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delaySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencySliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackSliderAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> inverterButtonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveChoiceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassButtonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> monoButtonAttachment;
    
    FlangerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FlangerAudioProcessorEditor)
};
