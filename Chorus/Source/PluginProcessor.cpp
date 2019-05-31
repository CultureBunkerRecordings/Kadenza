/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Kadeza_chorusAudioProcessor::Kadeza_chorusAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    //Construct and add parameters
    
    addParameter(mDryWetParameter =  new AudioParameterFloat("drywet","Dry Wet", 0.0f, 1.0f,0.0f));
    addParameter(mDepthParameter =  new AudioParameterFloat("depth","Depth", 0.0f, 1.0f, 0.0f));
    addParameter(mRateParameter =  new AudioParameterFloat("rate","Rate", 0.1f, 20.0f, 0.5f));
    addParameter(mPhaseOffsetParameter =  new AudioParameterFloat("phase","Phase Offset", 0.0f, 1.0f, 0.0f));
    addParameter(mFeedbackParameter =  new AudioParameterFloat("feedback","Feedback", 0.0f, 0.98f,0.0f));

    addParameter(mTypeParameter =  new AudioParameterInt("type","Type", 0, 1, 0));

    //initialise to default values
    
    mLFOPhase = 0;
    mCircularBufferLeft = nullptr;
    mCircularBufferRight = nullptr;
    mCircularBufferWriteHead = 0;
    mCircularBufferLength = 0;
    mFeedbackLeft = 0;
    mFeedbackRight = 0;
}

Kadeza_chorusAudioProcessor::~Kadeza_chorusAudioProcessor()
{
    if(mCircularBufferRight != nullptr)
    {
        delete[] mCircularBufferRight;
        mCircularBufferRight = nullptr;
    }
    if(mCircularBufferLeft != nullptr)
    {
        delete[] mCircularBufferLeft;
        mCircularBufferLeft = nullptr;
    }
}

float Kadeza_chorusAudioProcessor::linInterp(float inSample_X, float inSample_Y, float inFloatPhase)
{
    return (1 - inFloatPhase) * inSample_X + inFloatPhase * inSample_Y;
}
//==============================================================================
const String Kadeza_chorusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Kadeza_chorusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Kadeza_chorusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Kadeza_chorusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Kadeza_chorusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Kadeza_chorusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Kadeza_chorusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Kadeza_chorusAudioProcessor::setCurrentProgram (int index)
{
}

const String Kadeza_chorusAudioProcessor::getProgramName (int index)
{
    return {};
}

void Kadeza_chorusAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Kadeza_chorusAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //inititials data for samplerate and reset phase and write heads
    mFeedbackLeft = 0;
    mFeedbackRight = 0;
    
    //int phase
    mLFOPhase = 0;
    //calc buffer length
    mCircularBufferLength = sampleRate * MAX_DELAY_TIME;
   
    //left buffer
    if(mCircularBufferLeft == nullptr)
    {
        mCircularBufferLeft = new float[mCircularBufferLength];
    };
    
    //right buffer
    if(mCircularBufferRight == nullptr)
    {
        mCircularBufferRight = new float[mCircularBufferLength];
    };
    
    //clear junk data
    zeromem(mCircularBufferLeft,  sizeof(float) * mCircularBufferLength);
    
    //clear junk
    zeromem(mCircularBufferRight, sizeof(float) * mCircularBufferLength);

    //init write head
    mCircularBufferWriteHead = 0;
    
}

void Kadeza_chorusAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Kadeza_chorusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void Kadeza_chorusAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
   
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    //obtain left and right pointers
    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);
    
    //iterate through all the samples
    for(int i = 0;i<buffer.getNumSamples();i++)
    {
        //write to buffer
        mCircularBufferLeft[mCircularBufferWriteHead] = leftChannel[i] + mFeedbackLeft;
        mCircularBufferRight[mCircularBufferWriteHead] = rightChannel[i] + mFeedbackRight;
        
        //generate Left LFO output
        float lfoOutLeft = sin(2 * M_PI * mLFOPhase);
        
        //Calculate right channel LFO phase
        float lfoPhaseRight = mLFOPhase + *mPhaseOffsetParameter;
        
        if(lfoPhaseRight > 1)
        {
            lfoPhaseRight -= 1;
        }
        
        //Generate Right LFO output
        float lfoOutRight = sin(2*M_PI* lfoPhaseRight);

        //moving LFO phase forward
        mLFOPhase += *mRateParameter / getSampleRate();
        
        if(mLFOPhase > 1)
        {
            mLFOPhase -= 1;
        };
        
        //Control LFO Depth
        lfoOutLeft *= *mDepthParameter;
        lfoOutRight *= *mDepthParameter;
        
        float lfoMappedLeft = 0;
        float lfoMappedRight = 0;
        
        //Map to LFO  to our desired delay time;
        
        //Chorus
        if(*mTypeParameter == 1)
        {
             lfoMappedLeft = jmap(lfoOutLeft, -1.f, 1.f,  0.005f, 0.03f);
             lfoMappedRight = jmap(lfoOutRight, -1.f, 1.f, 0.005f, 0.03f);
        }
        //Flanger
        else
        {
            lfoMappedLeft = jmap(lfoOutLeft, -1.f, 1.f,  0.001f, 0.005f);
            lfoMappedRight = jmap(lfoOutRight, -1.f, 1.f, 0.001f, 0.005f);
        }
        
        
        //Calculate delay times
        float delayTimeInSamplesLeft = getSampleRate() * lfoMappedLeft;
        float delayTimeInSamplesRight = getSampleRate()* lfoMappedRight;
        
        
        //Calculate left readHead
        float DelayReadHeadLeft = mCircularBufferWriteHead - delayTimeInSamplesLeft;
        
        if(DelayReadHeadLeft < 0)
        {
            DelayReadHeadLeft+=mCircularBufferLength;
        }
        
        //Calculate right readhead
        float DelayReadHeadRight = mCircularBufferWriteHead - delayTimeInSamplesRight;
        
        if(DelayReadHeadRight < 0)
        {
            DelayReadHeadRight+=mCircularBufferLength;
        }
        
        //Interp variables Left
        int readHeadLeft_x = (int) DelayReadHeadLeft;
        int readHeadLeft_x1 = readHeadLeft_x + 1;
        
        float readHeadfloatLeft = DelayReadHeadLeft - readHeadLeft_x;
        
        if (readHeadLeft_x1 >= mCircularBufferLength)
        {
            readHeadLeft_x1 -= mCircularBufferLength;
        };
        
       
        //Interp variables right
        int readHeadRight_x = (int) DelayReadHeadRight;
        int readHeadRight_x1 = readHeadRight_x + 1;
        
        float readHeadfloatRight = DelayReadHeadRight - readHeadRight_x;
        
        if (readHeadRight_x1 >= mCircularBufferLength)
        {
            readHeadRight_x1 -= mCircularBufferLength;
        };
        
        //Generate left and right samples
        float Delay_Sample_Left = linInterp(mCircularBufferLeft[readHeadLeft_x],mCircularBufferLeft[readHeadLeft_x1],  readHeadfloatLeft);
        float Delay_Sample_Right = linInterp(mCircularBufferRight[readHeadRight_x],  mCircularBufferRight[readHeadRight_x1],readHeadfloatRight);
        
        mFeedbackLeft = Delay_Sample_Left * *mFeedbackParameter;
        mFeedbackRight = Delay_Sample_Right * *mFeedbackParameter;
        
        //dry wet signal output
        buffer.setSample(0, i, buffer.getSample(0, i)* (1-*mDryWetParameter) + Delay_Sample_Left* *mDryWetParameter);
        buffer.setSample(1, i, buffer.getSample(1, i)* (1-*mDryWetParameter) + Delay_Sample_Right* *mDryWetParameter);
        
        mCircularBufferWriteHead++;
        
        //circular buffer
        if(mCircularBufferWriteHead>=mCircularBufferLength)
        {
            mCircularBufferWriteHead = 0;
        }
        
    }
}

//==============================================================================
bool Kadeza_chorusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Kadeza_chorusAudioProcessor::createEditor()
{
    return new Kadeza_chorusAudioProcessorEditor (*this);
}

//==============================================================================
void Kadeza_chorusAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    
    std::unique_ptr<XmlElement> xml(new XmlElement("FlangrChorus"));
    
    xml->setAttribute("DryWet", *mDryWetParameter);
    xml->setAttribute("Depth", *mDepthParameter);
    xml->setAttribute("Rate", *mRateParameter);
    xml->setAttribute("Phase", *mPhaseOffsetParameter);
    xml->setAttribute("Feedback", *mFeedbackParameter);
    xml->setAttribute("Type", *mTypeParameter);
    
    copyXmlToBinary(*xml, destData);

    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Kadeza_chorusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    
    if (xml.get() != nullptr && xml->hasTagName("FlangerChorus"))
    {
        *mDryWetParameter = xml->getDoubleAttribute("DryWet");
        *mDepthParameter = xml->getDoubleAttribute("Depth");
        *mRateParameter = xml->getDoubleAttribute("Depth");
        *mPhaseOffsetParameter = xml->getDoubleAttribute("Phase");
        *mFeedbackParameter = xml->getDoubleAttribute("Feedback");
        
        *mTypeParameter = xml->getIntAttribute("Type");
        
    }
    
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Kadeza_chorusAudioProcessor();
}
