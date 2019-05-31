/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Kadeza_chorusAudioProcessorEditor::Kadeza_chorusAudioProcessorEditor (Kadeza_chorusAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    
    setSize (400, 315);
    
    auto& params = processor.getParameters();
    AudioParameterFloat* DryWetParam = (AudioParameterFloat*)params.getUnchecked(0);
    
    mDryWetSlider.setBounds(0, 15, 100, 100);
    mDryWetSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mDryWetSlider.setRange(DryWetParam->range.start, DryWetParam->range.end);
    mDryWetSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    mDryWetSlider.setValue(*DryWetParam);
    addAndMakeVisible(mDryWetSlider);
    
    mDryWetSlider.onDragStart = [DryWetParam]{DryWetParam->beginChangeGesture();};
    mDryWetSlider.onValueChange = [this, DryWetParam]{*DryWetParam = mDryWetSlider.getValue();};
    mDryWetSlider.onDragEnd = [DryWetParam]{DryWetParam->endChangeGesture();};
    
    
    
    AudioParameterFloat* DepthParam = (AudioParameterFloat*)params.getUnchecked(1);
    
    mDepthSlider.setBounds(100, 15, 100, 100);
    mDepthSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mDepthSlider.setRange(DepthParam->range.start, DepthParam->range.end);
    mDepthSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    mDepthSlider.setValue(*DepthParam);
    addAndMakeVisible(mDepthSlider);
    
    mDepthSlider.onDragStart = [DepthParam]{DepthParam->beginChangeGesture();};
    mDepthSlider.onValueChange = [this,DepthParam]{*DepthParam = mDepthSlider.getValue();};
    mDepthSlider.onDragEnd = [DepthParam]{DepthParam->endChangeGesture();};
    
    
    AudioParameterFloat* RateParam = (AudioParameterFloat*)params.getUnchecked(2);
    
    mRateSlider.setBounds(200, 15, 100, 100);
    mRateSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mRateSlider.setRange(RateParam->range.start, RateParam->range.end);
    mRateSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    mRateSlider.setValue(*RateParam);
    addAndMakeVisible(mRateSlider);
    
    mRateSlider.onDragStart = [RateParam]{RateParam->beginChangeGesture();};
    mRateSlider.onValueChange = [this,RateParam]{*RateParam = mRateSlider.getValue();};
    mRateSlider.onDragEnd = [RateParam]{RateParam->endChangeGesture();};

    
    AudioParameterFloat* PhaseOffsetParam = (AudioParameterFloat*)params.getUnchecked(3);
    
    mPhaseOffsetSlider.setBounds(300, 15, 100, 100);
    mPhaseOffsetSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mPhaseOffsetSlider.setRange(PhaseOffsetParam->range.start, PhaseOffsetParam->range.end);
    mPhaseOffsetSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    mPhaseOffsetSlider.setValue(*PhaseOffsetParam);
    addAndMakeVisible(mPhaseOffsetSlider);
    
    mPhaseOffsetSlider.onDragStart = [PhaseOffsetParam]{PhaseOffsetParam->beginChangeGesture();};
    mPhaseOffsetSlider.onValueChange = [this,PhaseOffsetParam]{*PhaseOffsetParam = mPhaseOffsetSlider.getValue();};
    mPhaseOffsetSlider.onDragEnd = [PhaseOffsetParam]{PhaseOffsetParam->endChangeGesture();};
    
    
    
    AudioParameterFloat* FeedbackParam = (AudioParameterFloat*)params.getUnchecked(4);
    
    mFeedbackSlider.setBounds(0, 115, 100, 100);
    mFeedbackSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mFeedbackSlider.setRange(FeedbackParam->range.start, FeedbackParam->range.end);
    mFeedbackSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    mFeedbackSlider.setValue(*FeedbackParam);
    addAndMakeVisible(mFeedbackSlider);
    
    mFeedbackSlider.onDragStart = [FeedbackParam]{FeedbackParam->beginChangeGesture();};
    mFeedbackSlider.onValueChange = [this,FeedbackParam]{*FeedbackParam = mFeedbackSlider.getValue();};
    mFeedbackSlider.onDragEnd = [FeedbackParam]{FeedbackParam->endChangeGesture();};
    
    AudioParameterInt* TypeParam = (AudioParameterInt*)params.getUnchecked(5);
    
    mType.setBounds(100, 115, 100, 30);
    mType.addItem("Chorus", 1);
    mType.addItem("Flanger", 2);
    addAndMakeVisible(mType);
    
    mType.onChange = [this,TypeParam]
    {
        TypeParam->beginChangeGesture();
        *TypeParam = mType.getSelectedItemIndex();
        TypeParam->endChangeGesture();
    };
    
    mType.setSelectedItemIndex(*TypeParam);
}

Kadeza_chorusAudioProcessorEditor::~Kadeza_chorusAudioProcessorEditor()
{
}

//==============================================================================
void Kadeza_chorusAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Kadenza Chorus!", getLocalBounds(), Justification::centredTop, 1);
}

void Kadeza_chorusAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
