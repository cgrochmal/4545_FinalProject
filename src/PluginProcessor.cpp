

#include "PluginProcessor.h"
#include "PluginEditor.h"

const double PI = stk::PI;

using namespace std;

//==============================================================================
Musi45effectAudioProcessor::Musi45effectAudioProcessor()
{
    // In the constructor do any initializations that need to be done once
    
    // Set ranges and default values for all user params
    usrParams[lfoDepthParam].setMinMax(minLfoD, maxLfoD);
    usrParams[lfoDepthParam].setWithUparam(defLfoD);
    
    usrParams[lfoSpeedParam].setMinMax(minLfoS, maxLfoS);
    usrParams[lfoSpeedParam].setWithUparam(defLfoS);
    
    usrParams[feedbackParam].setMinMax(minF, maxF);
    usrParams[feedbackParam].setWithUparam(defF);
    
    usrParams[delayTimeParam].setMinMax(minD, maxD);
    usrParams[delayTimeParam].setWithUparam(defD);
    
    usrParams[wetParam].setMinMax(minW, maxW);
    usrParams[wetParam].setWithUparam(defW);
    
    usrParams[dryParam].setMinMax(minW, maxW);
    usrParams[dryParam].setWithUparam(defW);
    
    calcFilterCoeffs();

}

Musi45effectAudioProcessor::~Musi45effectAudioProcessor()
{
}

const String Musi45effectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

// STEP 4.3 - return the correct number of user parameters
int Musi45effectAudioProcessor::getNumParameters()
{
    return totalNumParams;;
}

float Musi45effectAudioProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    
    // Return the slider value of a user param when called with its index
    if (index <= totalNumParams)
        return usrParams[index].getVstVal();
    else
        return 0.0f;
}

void Musi45effectAudioProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    
    // Set algorithm parameters from user parameters
    usrParams[index].setWithVstVal(newValue);
    
    // we should only re-calculate the params for the filter that was changed.
    switch (index)
    {
        case lfoDepthParam:
            break;
        case lfoSpeedParam:
            setLfoFreq();
            break;
        case feedbackParam:
            break;
        case delayTimeParam:
            break;
        case wetParam:
            break;
        case dryParam:
            break;
        default:
            break;
    }
}

// STEP 4.7b
float Musi45effectAudioProcessor::getParameterDefaultValue (int index)
{
    switch (index)
    {
        case lfoDepthParam:
            return defLfoD;
        case lfoSpeedParam:
            return defLfoS;
        case feedbackParam:
            return defF;
        case delayTimeParam:
            return defD;
        case wetParam:
            return defW;
        case dryParam:
            return defW; //todo: defD
        default:
            break;
    }
    
    return 0.0f;
}

// STEP 4.8
const String Musi45effectAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
        case lfoSpeedParam:
            return "LFO speed";
        case lfoDepthParam:
            return "LFO depth";
        case feedbackParam:
            return "Feedback";
        case delayTimeParam:
            return "Delay Time";
        case wetParam:
            return "Wet Gain";
        case dryParam:
            return "Dry Gain";
        default:
            break;
    }
    
    return String::empty;
}

// STEP 4.9 - not sure who calls this or when!
const String Musi45effectAudioProcessor::getParameterText (int index)
{
    return String( usrParams[index].getUparamVal() );
    // return String (getParameter (index), 2);
}

const String Musi45effectAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String Musi45effectAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool Musi45effectAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool Musi45effectAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool Musi45effectAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Musi45effectAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool Musi45effectAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double Musi45effectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Musi45effectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int Musi45effectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Musi45effectAudioProcessor::setCurrentProgram (int index)
{
}

const String Musi45effectAudioProcessor::getProgramName (int index)
{
    return String();
}

void Musi45effectAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Musi45effectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    fs = sampleRate;
    
    int samps = maxD * fs * .001; //shane has 1000 * fs here
    DelayL.setMaximumDelay(samps);
    DelayR.setMaximumDelay(samps);
    
    // calc all alg params
    setLfoFreq();
    calcDelays(lfo);
    calcFilterCoeffs();
    
    // init the control rate counter
    cntrlCounter = 0;
}

// Calculate lfo stuff
void Musi45effectAudioProcessor::setLfoFreq()
{
    float lfoFreq = usrParams[lfoSpeedParam].getUparamVal();
    myLFO.setFreq(lfoFreq, fs/cntrlN);  // since the LFO only gets called every N samps, divide fs by N
}

void Musi45effectAudioProcessor::calcFBW()
{
    feedback = usrParams[feedbackParam].getUparamVal() * .01;
    wet = usrParams[wetParam].getUparamVal() * .01;
    dry = usrParams[dryParam].getUparamVal() * .01;
}



void Musi45effectAudioProcessor::calcFilterCoeffs()
{
    float coeffs[2];
    float fc = 500;
    
    Mu45FilterCalc::calcCoeffs1PoleLPF(coeffs, fc, fs);
    filterL.setCoefficients(coeffs[0], coeffs[1]);
    filterR.setCoefficients(coeffs[0], coeffs[1]);
}

//Calculates delay time
void Musi45effectAudioProcessor::calcDelays(float LFO)
{
    float msec = usrParams[delayTimeParam].getUparamVal();
    DelayL.setDelay(msec*fs*.001);
    DelayR.setDelay(msec*fs*.001);
    int samps = 30 * fs * .0019;
    samps = samps + .5*samps*LFO*(.01)*(usrParams[lfoDepthParam].getUparamVal());
    //std::cout << samps << std::endl;
    ChorusDelayL.setDelay(samps);
    ChorusDelayR.setDelay(samps);
}

void Musi45effectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void Musi45effectAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // Do the guts of your plugin's audio processing...
    const int numSamples = buffer.getNumSamples();
    
    // This version assumes we always have 2 channels
    float* channelDataL = buffer.getWritePointer (0);
    float* channelDataR = buffer.getWritePointer (1);
    
    float inSampL, inSampR;
    float CtempL, CtempR, tempL, tempR, filteredL, filteredR;
    float pannedL, pannedR;
    
    // The "inner loop"
    for (int i = 0; i < numSamples; ++i)
    {
        // get input samples
        inSampL = channelDataL[i];
        inSampR = channelDataR[i];
        
        // control rate stuff
        if (cntrlCounter % cntrlN == 0)
        {
            
            lfo = myLFO.tick();         // lfo varies between -1.0 and 1.0
            //cout << lfo << endl;
            calcDelays(lfo);
            calcFBW();
            pan = (lfo+1)/2;
            leftgain = cos(pan*(PI/2.0f));
            rightgain = sin((pan)*(PI/2.0f));
        }
        cntrlCounter++;     // update the control rate counter
        
        //Crazy voice effect
        /*
         tempL = ChorusDelayL.lastOut();
         tempR = ChorusDelayR.lastOut();
         ChorusDelayL.tick(inSampL + feedback*(tempL));
         ChorusDelayR.tick(inSampR + feedback*(tempR));
         
         channelDataL[i] = leftgain*(channelDataL[i] + wet*tempL);
         channelDataR[i] = rightgain*(channelDataR[i] + wet*tempR);
         */
        
        tempL = DelayL.lastOut();
        tempR = DelayR.lastOut();
        filteredL = filterL.tick(tempL);
        filteredR = filterR.tick(tempR);
        CtempL = ChorusDelayL.lastOut();
        CtempR = ChorusDelayR.lastOut();
        
        
        ChorusDelayL.tick(filteredL + feedback*(CtempL));
        ChorusDelayR.tick(filteredR + feedback*(CtempR));
        
        pannedL = leftgain* CtempL;
        pannedR = rightgain* CtempR;
        
        
        DelayL.tick(channelDataL[i] + .7*pannedL);
        DelayR.tick(channelDataR[i] + .7*pannedR);
        
        channelDataL[i] = (dry*channelDataL[i] + wet*pannedL);
        channelDataR[i] = (dry*channelDataR[i] + wet*pannedR);
    }
}
//==============================================================================
bool Musi45effectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Musi45effectAudioProcessor::createEditor()
{
    return new Musi45effectAudioProcessorEditor (*this);
}

//==============================================================================
void Musi45effectAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Musi45effectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Musi45effectAudioProcessor();
}


#include "PluginProcessor.h"
#include "PluginEditor.h"

const double PI = stk::PI;

using namespace std;

//==============================================================================
Musi45effectAudioProcessor::Musi45effectAudioProcessor()
{
    // In the constructor do any initializations that need to be done once
    
    // Set ranges and default values for all user params
    usrParams[lfoDepthParam].setMinMax(minLfoD, maxLfoD);
    usrParams[lfoDepthParam].setWithUparam(defLfoD);
    
    usrParams[lfoSpeedParam].setMinMax(minLfoS, maxLfoS);
    usrParams[lfoSpeedParam].setWithUparam(defLfoS);
    
    usrParams[feedbackParam].setMinMax(minF, maxF);
    usrParams[feedbackParam].setWithUparam(defF);
    
    usrParams[delayTimeParam].setMinMax(minD, maxD);
    usrParams[delayTimeParam].setWithUparam(defD);
    
    usrParams[wetParam].setMinMax(minW, maxW);
    usrParams[wetParam].setWithUparam(defW);
    
    usrParams[dryParam].setMinMax(minDr, maxDr);
    usrParams[dryParam].setWithUparam(defDr);
    
    calcFilterCoeffs();
    calcDelays(myLFO.tick());
}

Musi45effectAudioProcessor::~Musi45effectAudioProcessor()
{
}

//==============================================================================
const String Musi45effectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

// STEP 4.3 - return the correct number of user parameters
int Musi45effectAudioProcessor::getNumParameters()
{
    return totalNumParams;;
}

float Musi45effectAudioProcessor::getParameter (int index)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    
    // Return the slider value of a user param when called with its index
    if (index <= totalNumParams)
        return usrParams[index].getVstVal();
    else
        return 0.0f;
}

void Musi45effectAudioProcessor::setParameter (int index, float newValue)
{
    // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    
    // Set algorithm parameters from user parameters
    usrParams[index].setWithVstVal(newValue);
    
    // we should only re-calculate the params for the filter that was changed.
    switch (index)
    {
        case lfoDepthParam:
            break;
        case lfoSpeedParam:
            setLfoFreq();
            break;
        case feedbackParam:
            break;
        case delayTimeParam:
            break;
        case wetParam:
            break;
        case dryParam:
            break;
        default:
            break;
    }
}

// STEP 4.7b
float Musi45effectAudioProcessor::getParameterDefaultValue (int index)
{
    switch (index)
    {
        case lfoDepthParam:
            return defLfoD;
        case lfoSpeedParam:
            return defLfoS;
        case feedbackParam:
            return defF;
        case delayTimeParam:
            return defD;
        case wetParam:
            return defW;
        case dryParam:
            return defDr;
        default:
            break;
    }
    
    return 0.0f;
}

// STEP 4.8
const String Musi45effectAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
        case lfoSpeedParam:
            return "LFO speed";
        case lfoDepthParam:
            return "LFO depth";
        case feedbackParam:
            return "Feedback";
        case delayTimeParam:
            return "Delay Time";
        case wetParam:
            return "Wet Gain";
        case dryParam:
            return "Dry Gain";
        default:
            break;
    }
    
    return String::empty;
}

// STEP 4.9 - not sure who calls this or when!
const String Musi45effectAudioProcessor::getParameterText (int index)
{
    return String( usrParams[index].getUparamVal() );
    // return String (getParameter (index), 2);
}

const String Musi45effectAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String Musi45effectAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool Musi45effectAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool Musi45effectAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool Musi45effectAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Musi45effectAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool Musi45effectAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double Musi45effectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Musi45effectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int Musi45effectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Musi45effectAudioProcessor::setCurrentProgram (int index)
{
}

const String Musi45effectAudioProcessor::getProgramName (int index)
{
    return String();
}

void Musi45effectAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void Musi45effectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    fs = sampleRate;
    
    int samps = maxD * fs * .001;
    DelayL.setMaximumDelay(samps);
    DelayR.setMaximumDelay(samps);
    
    // calc all alg params
    calcDelays(lfo);
    calcFilterCoeffs();
    // init the control rate counter
    cntrlCounter = 0;
}

// Calculate lfo stuff
void Musi45effectAudioProcessor::setLfoFreq()
{
    float delaytime = usrParams[delayTimeParam].getUparamVal();
    float hertz = 1.0f/(delaytime*.001);
    myLFO.setFreq(hertz, fs/cntrlN);
    usrParams[lfoSpeedParam].setWithUparam(hertz);
    // since the LFO only gets called every N samps, divide fs by N
}

void Musi45effectAudioProcessor::calcFBW()
{
    feedback = usrParams[feedbackParam].getUparamVal() * .01;
    wet = usrParams[wetParam].getUparamVal() * .01;
}

void Musi45effectAudioProcessor::calcFilterCoeffs()
{
    float coeffs[2];
    float fc = 500;
    
    Mu45FilterCalc::calcCoeffs1PoleLPF(coeffs, fc, fs);
    filterL.setCoefficients(coeffs[0], coeffs[1]);
    filterR.setCoefficients(coeffs[0], coeffs[1]);
}

//Calculates delay time
void Musi45effectAudioProcessor::calcDelays(float LFO)
{
    float sec = usrParams[delayTimeParam].getUparamVal()*.001;
    DelayL.setDelay(sec*fs);
    DelayR.setDelay(sec*fs);
    int samps = 30 * fs * .001;
    samps = samps + .5*samps*LFO*(.01)*(usrParams[lfoDepthParam].getUparamVal());
    ChorusDelayL.setDelay(samps);
    ChorusDelayR.setDelay(samps);
}

void Musi45effectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void Musi45effectAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // Do the guts of your plugin's audio processing...
    const int numSamples = buffer.getNumSamples();
    
    // This version assumes we always have 2 channels
    float* channelDataL = buffer.getWritePointer (0);
    float* channelDataR = buffer.getWritePointer (1);
    
    float inSampL, inSampR;
    float CtempL, CtempR, tempL, tempR, filteredL, filteredR, pannedL, pannedR;
    
    // The "inner loop"
    for (int i = 0; i < numSamples; ++i)
    {
        // get input samples
        inSampL = channelDataL[i];
        inSampR = channelDataR[i];
        
        // control rate stuff
        if (cntrlCounter % cntrlN == 0)
        {
            setLfoFreq();
            lfo = myLFO.tick();         // lfo varies between -1.0 and 1.0
            //cout << lfo << endl;
            calcDelays(lfo);
            calcFBW();
            pan = (lfo+1)/2;
            leftgain = cos(pan*(PI/2.0f));
            rightgain = sin((pan)*(PI/2.0f));
        }
        cntrlCounter++;     // update the control rate counter
        
        //Crazy voice effect
        /*
         tempL = ChorusDelayL.lastOut();
         tempR = ChorusDelayR.lastOut();
         ChorusDelayL.tick(inSampL + feedback*(tempL));
         ChorusDelayR.tick(inSampR + feedback*(tempR));
         
         channelDataL[i] = leftgain*(channelDataL[i] + wet*tempL);
         channelDataR[i] = rightgain*(channelDataR[i] + wet*tempR);
         */
        
        tempL = DelayL.lastOut();
        tempR = DelayR.lastOut();
        filteredL = filterL.tick(tempL);
        filteredR = filterR.tick(tempR);
        CtempL = ChorusDelayL.lastOut();
        CtempR = ChorusDelayR.lastOut();
        ChorusDelayL.tick(filteredL + feedback*(CtempL));
        ChorusDelayR.tick(filteredR + feedback*(CtempR));
        
        pannedL = leftgain*CtempL;
        pannedR = rightgain*CtempR;
        
        DelayL.tick(channelDataL[i] + .85*tempL);
        DelayR.tick(channelDataR[i] + .85*tempR);
        
        
        channelDataL[i] = (dry*channelDataL[i] + wet*pannedL);
        channelDataR[i] = (dry*channelDataR[i] + wet*pannedR);
    }
}

//==============================================================================
bool Musi45effectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* Musi45effectAudioProcessor::createEditor()
{
    return new Musi45effectAudioProcessorEditor (*this);
}

//==============================================================================
void Musi45effectAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Musi45effectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Musi45effectAudioProcessor();
}
