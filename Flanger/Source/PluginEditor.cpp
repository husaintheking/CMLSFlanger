/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
//==============================================================================
FlangerAudioProcessorEditor::FlangerAudioProcessorEditor (FlangerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    //adding the background image
    auto background = juce::ImageCache::getFromMemory(BinaryData::flang_jpg, BinaryData::flang_jpgSize);
    if (!background.isNull())
        mImageComponent.setImage(background, juce::RectanglePlacement::stretchToFit);
    else
        jassert(!background.isNull());
    addAndMakeVisible(mImageComponent);

    // colour characteristics 
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colour(0xFF00FF00));                 // thumb
    getLookAndFeel().setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::slategrey);   // slider (right)
    getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::white);          // slider (left)
    getLookAndFeel().setColour(juce::Label::textColourId, juce::Colours::white);                       // label

    // depth
    depthSlider.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    depthSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 60, 25);
    addAndMakeVisible (depthSlider);
    addAndMakeVisible(depthLabel);
    depthLabel.setText("DEPTH", juce::dontSendNotification);
    depthLabel.attachToComponent(&depthSlider, false);
    depthLabel.setJustificationType(juce::Justification::centred);
    depthLabel.setFont(juce::Font(15.0f, juce::Font::bold));

    // width
    widthSlider.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    widthSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 60, 25);
    addAndMakeVisible (widthSlider);
    addAndMakeVisible(widthLabel);
    widthLabel.setText("WIDTH", juce::dontSendNotification);
    widthLabel.attachToComponent(&widthSlider, false);
    widthLabel.setJustificationType(juce::Justification::centred);
    widthLabel.setFont(juce::Font(15.0f, juce::Font::bold));

    // delay
    delaySlider.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    delaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 25);
    addAndMakeVisible (delaySlider);
    addAndMakeVisible(delayLabel);
    delayLabel.setText("DELAY", juce::dontSendNotification);
    delayLabel.attachToComponent(&delaySlider, false);
    delayLabel.setJustificationType(juce::Justification::centred);
    delayLabel.setFont(juce::Font(15.0f, juce::Font::bold));

    // frequency
    frequencySlider.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 25);
    addAndMakeVisible (frequencySlider);
    addAndMakeVisible(frequencyLabel);
    frequencyLabel.setText("FREQUENCY", juce::dontSendNotification);
    frequencyLabel.attachToComponent(&frequencySlider, false);
    frequencyLabel.setJustificationType(juce::Justification::centred);
    frequencyLabel.setFont(juce::Font(15.0f, juce::Font::bold));

    // feedback
    feedbackSlider.setSliderStyle (juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    feedbackSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 60, 25);
    addAndMakeVisible (feedbackSlider);
    addAndMakeVisible(feedbackLabel);
    feedbackLabel.setText("FEEDBACK", juce::dontSendNotification);
    feedbackLabel.attachToComponent(&feedbackSlider, false);
    feedbackLabel.setJustificationType(juce::Justification::centred);
    feedbackLabel.setFont(juce::Font(15.0f, juce::Font::bold));
   
    // inverter button
    inverterButton.setButtonText("Positive");
    inverterButton.setClickingTogglesState(true);
    inverterButton.onClick = [this]() {

        if (inverterButton.getToggleState())
        {
            inverterButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::black);
            inverterButton.setButtonText("Positive");
        }
        else
        {
            inverterButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
            inverterButton.setButtonText("Negative");
            inverterButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
        }
    };

    addAndMakeVisible(inverterButton);
    addAndMakeVisible(inverterLabel);
    inverterLabel.setText("POLARITY", juce::dontSendNotification);
    inverterLabel.attachToComponent(&inverterButton, false);
    inverterLabel.setJustificationType(juce::Justification::centred);
    inverterLabel.setFont(juce::Font(15.0f, juce::Font::bold));

    // wave button
    waveChoice.addItem("Sine", 1);
    waveChoice.addItem("Triangle", 2);
    waveChoice.addItem("Sawtooth_Up", 3);
    waveChoice.addItem("Sawtooth_Down", 4);
    waveChoice.setColour(juce::ComboBox::backgroundColourId, juce::Colours::black);
    waveChoice.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(waveChoice);
    addAndMakeVisible(waveLabel);
    waveLabel.setText("WAVEFORM", juce::dontSendNotification);
    waveLabel.attachToComponent(&waveChoice, false);
    waveLabel.setJustificationType(juce::Justification::centred);
    waveLabel.setFont(juce::Font(15.0f, juce::Font::bold));

    // bypass button
    bypassButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    bypassButton.setButtonText("BYPASS OFF");
    bypassButton.setClickingTogglesState(true);
    bypassButton.onClick = [this]() {

        if (bypassButton.getToggleState())
        {
            bypassButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFF00FF00));
            bypassButton.setButtonText("BYPASS ON");
            bypassButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
        }
        else
        {
            bypassButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
            bypassButton.setButtonText("BYPASS OFF");
        }
    };
   
    addAndMakeVisible(bypassButton);
    addAndMakeVisible(bypassLabel);
    bypassLabel.attachToComponent(&bypassButton, false);
    bypassLabel.setJustificationType(juce::Justification::centred);
    bypassLabel.setFont(juce::Font(15.0f, juce::Font::bold));

    // mono/stereo button
    monoButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    monoButton.setButtonText("MONO OFF");
    monoButton.setClickingTogglesState(true);
    monoButton.onClick = [this]() {

        if (monoButton.getToggleState())
        {
            monoButton.setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xFF00FF00));
            monoButton.setButtonText("MONO ON");
            monoButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
        }
        else
        {
            monoButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
            monoButton.setButtonText("MONO OFF");
        }
    };

    addAndMakeVisible(monoButton);
    addAndMakeVisible(monoButton);
    
    // attachments between the GUI controls and the parameters of the ValueTreeState
    depthSliderAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DEPTH", depthSlider);
    widthSliderAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "WIDTH", widthSlider);
    delaySliderAttachment     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "DELAY", delaySlider);
    frequencySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FREQUENCY", frequencySlider);
    feedbackSliderAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "FEEDBACK", feedbackSlider);
    inverterButtonAttachment  = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "INVERTER", inverterButton);
    waveChoiceAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "WAVEFORM", waveChoice);
    bypassButtonAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "BYPASS", bypassButton);
    monoButtonAttachment      = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "MONO", monoButton);

    // size of plugin
    setSize (400, 600);
    setResizeLimits( 400, 600,  400,  600);

}

FlangerAudioProcessorEditor::~FlangerAudioProcessorEditor()
{
}

//==============================================================================
void FlangerAudioProcessorEditor::paint (juce::Graphics& g)
{
}

void FlangerAudioProcessorEditor::resized()
{
    // size of the graphic components
    feedbackSlider.setBounds((getWidth() / 4) * 0, (getHeight() / 4) * 0.3, 130, 130);
    frequencySlider.setBounds((getWidth() / 4) * 2.67, (getHeight() / 4) * 0.3, 130, 130);
    delaySlider.setBounds((getWidth() / 4) * 2.67, (getHeight() / 4) * 1.9, 130, 130);
    widthSlider.setBounds((getWidth() / 4) * 0, (getHeight() / 4) * 1.9, 130, 130);
    depthSlider.setBounds((getWidth() / 4) * 0, (getHeight() / 4) * 3, 130, 130);

    inverterButton.setBounds((getWidth() * 0 + 20), (getHeight() / 4) * 1.4, 90, 30);
    waveChoice.setBounds((getWidth() / 4) * 2 + (getWidth() / 4.5), (getHeight() / 4) * 1.4, 90, 30);
    bypassButton.setBounds((getWidth() / 4) * 2 + (getWidth() / 4.5), (getHeight() / 4) * 3, 90, 90);
    monoButton.setBounds((getWidth() / 4) * 2 + (getWidth() / 4.5), (getHeight() / 4) * 3.7, 90, 30);

    mImageComponent.setBounds(getLocalBounds()); // size of the BGD image
    
};
