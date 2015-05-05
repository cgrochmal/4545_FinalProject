/*
 ==============================================================================
 PluginProcessor.h
 ==============================================================================
 */

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "UParam.h"
#include "BiQuad.h"
#include "OnePole.h"
#include "DelayL.h"
#include "DelayA.h"
#include "Delay.h"
#include "Mu45FilterCalc.h"
#include "Mu45LFO.h"

// min, max, and default for every user param. These can be set once
// here, and then used both in the Editor and Processor.

#define minLfoD 0.0
#define maxLfoD 100.0
#define defLfoD 50.0
#define minLfoS 0.01
#define maxLfoS 15.0
#define defLfoS 0.5
#define minF 0.0
#define maxF 100.0
#define defF 50.0
#define minD 50.0
#define maxD 2000.0
#define defD 500.0
#define defW 80.0
#define minW 0.0
#define maxW 100.0
#define minDr 0.0
#define maxDr 1.0
#define defDr 0.7


//==============================================================================
/**
 */
class Musi45effectAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    Musi45effectAudioProcessor();
    ~Musi45effectAudioProcessor();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;
    
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    //==============================================================================
    const String getName() const override;
    
    int getNumParameters() override;
    float getParameter (int index) override;
    void setParameter (int index, float newValue) override;
    float getParameterDefaultValue (int index) override;    // STEP 4.7a
    
    const String getParameterName (int index) override;
    const String getParameterText (int index) override;
    
    const String getInputChannelName (int channelIndex) const override;
    const String getOutputChannelName (int channelIndex) const override;
    bool isInputChannelStereoPair (int index) const override;
    bool isOutputChannelStereoPair (int index) const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool silenceInProducesSilenceOut() const override;
    double getTailLengthSeconds() const override;
    
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    
    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    
    //===== ADD NEW STUFF HERE =====================================================
    
    // Create an enumerated type called 'Parameters' for the User Parameters
    enum Parameters
    {
        lfoDepthParam = 0,
        lfoSpeedParam,
        feedbackParam,
        delayTimeParam,
        wetParam,
        dryParam,
        totalNumParams      // must end with this variable
    };
    
    
    // An array for storing User Params
    UParam usrParams[totalNumParams];
    
private:
    //==============================================================================
    
    float fs;
    float feedback, wet, dry;
    
    // stuff for doing control rate
    const int cntrlN = 10;          // the number of audio samples to process before processing control rate stuff
    long unsigned cntrlCounter;     // a counter for control rate stuff
    float lfo, depth;
    float leftgain, rightgain, pan;
    
    // Filters
    stk::DelayA ChorusDelayL, ChorusDelayR;             // The delays
    stk::DelayA DelayL, DelayR;
    stk::OnePole filterL, filterR;
    // LFO
    Mu45LFO myLFO;
    
    // Add any methods used in algorithm parameter calculation or DSP
    void setLfoFreq();
    void calcDelays(float LFO);
    void calcFBW();
    void calcFilterCoeffs();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Musi45effectAudioProcessor)
    
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
