/*
 ==============================================================================
 
 This file was auto-generated by the Introjucer!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
Musi45effectAudioProcessorEditor::Musi45effectAudioProcessorEditor (Musi45effectAudioProcessor& p)
:   AudioProcessorEditor (&p), processor (p),
lfoDepthSlider("LfoSpeed"),
lfoSpeedSlider("LfoDepth %"),
feedbackSlider("Feedback %"),
delayTimeSlider("Delay Time"),
wetSlider("Wet %"),
tapdelay("Tap Tempo"),
lfoDepthLabel("","LFO Depth %"),
lfoSpeedLabel("","LFO Speed (Hz)"),
feedbackLabel("", "Feedback %"),
delayTimeLabel("", "Delay (ms)"),
wetLabel("", "Wet Gain"),
dryLabel("", "Dry Gain"),
tempoTimer()

{
    
    float labelFontSize = 12.0;
    
    lfoDepthLabel.setColour(juce::Label::textColourId, juce::Colour(255, 255, 255));
    lfoSpeedLabel.setColour(juce::Label::textColourId, juce::Colour(255, 255, 255));
    feedbackLabel.setColour(juce::Label::textColourId, juce::Colour(255, 255, 255));
    delayTimeLabel.setColour(juce::Label::textColourId, juce::Colour(255, 255, 255));
    wetLabel.setColour(juce::Label::textColourId, juce::Colour(255, 255, 255));
    dryLabel.setColour(juce::Label::textColourId, juce::Colour(255, 255, 255));
    
    // LFO depth
    lfoDepthSlider.setSliderStyle(Slider::LinearVertical);  // what kind of slider?
    lfoDepthSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    lfoDepthSlider.setRange(minLfoD, maxLfoD, 1.0);
    addAndMakeVisible(lfoDepthSlider);                      // add the slider to the GUI
    lfoDepthSlider.addListener(this);                       // add a listener for this function
    lfoDepthLabel.attachToComponent (&lfoDepthSlider, false);
    lfoDepthLabel.setFont(Font (labelFontSize));
    
    // LFO speed
    lfoSpeedSlider.setSliderStyle(Slider::LinearVertical);  // what kind of slider?
    lfoSpeedSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    lfoSpeedSlider.setRange(minLfoS, maxLfoS, 0.005);
    lfoSpeedSlider.setSkewFactor(0.3);
    addAndMakeVisible(lfoSpeedSlider);                      // add the slider to the GUI
    lfoSpeedSlider.addListener(this);                       // add a listener for this function
    lfoSpeedLabel.attachToComponent (&lfoSpeedSlider, false);
    lfoSpeedLabel.setFont(Font (labelFontSize));
    
    // Feedback
    feedbackSlider.setSliderStyle(Slider::LinearVertical);  // what kind of slider?
    feedbackSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    feedbackSlider.setRange(minF, maxF, .01);
    feedbackSlider.setSkewFactor(1.5);
    addAndMakeVisible(feedbackSlider);                      // add the slider to the GUI
    feedbackSlider.addListener(this);                       // add a listener for this function
    feedbackLabel.attachToComponent (&feedbackSlider, false);
    feedbackLabel.setFont(Font (labelFontSize));
    
    // Delay Time
    delayTimeSlider.setSliderStyle(Slider::LinearVertical);  // what kind of slider?
    delayTimeSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    delayTimeSlider.setRange(minD, maxD, 1);
    addAndMakeVisible(delayTimeSlider);                      // add the slider to the GUI
    delayTimeSlider.addListener(this);                       // add a listener for this function
    delayTimeLabel.attachToComponent (&delayTimeSlider, false);
    delayTimeLabel.setFont(Font (labelFontSize));
    
    // Wet Gain
    wetSlider.setSliderStyle(Slider::LinearVertical);  // what kind of slider?
    wetSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    wetSlider.setRange(minW, maxW, 1);
    addAndMakeVisible(wetSlider);                      // add the slider to the GUI
    wetSlider.addListener(this);                       // add a listener for this function
    wetLabel.attachToComponent (&wetSlider, false);
    wetLabel.setFont(Font (labelFontSize));
    
    // Dry Gain
    drySlider.setSliderStyle(Slider::LinearVertical);  // what kind of slider?
    drySlider.setTextBoxStyle(Slider::TextBoxBelow, false, 60, 20);
    drySlider.setRange(minW, maxW, 1);
    addAndMakeVisible(drySlider);                      // add the slider to the GUI
    drySlider.addListener(this);                       // add a listener for this function
    dryLabel.attachToComponent (&drySlider, false);
    dryLabel.setFont(Font (labelFontSize));
    
    
    //presets
    preset1.setButtonText("1");
    addAndMakeVisible(preset1);
    preset1.addListener(this);
    
    preset2.setButtonText("2");
    addAndMakeVisible(preset2);
    preset2.addListener(this);
    
    preset3.setButtonText("3");
    addAndMakeVisible(preset3);
    preset3.addListener(this);
    
    preset4.setButtonText("4");
    addAndMakeVisible(preset4);
    preset4.addListener(this);
    
    
    //set presets
    set1.setButtonText("Set 1");
    addAndMakeVisible(set1);
    set1.addListener(this);
    
    set2.setButtonText("Set 2");
    addAndMakeVisible(set2);
    set2.addListener(this);
    
    set3.setButtonText("Set 3");
    addAndMakeVisible(set3);
    set3.addListener(this);
    
    set4.setButtonText("Set 4");
    addAndMakeVisible(set4);
    set4.addListener(this);
    
    tapdelay.setButtonText("Tap Tempo");
    //tapdelay.setClickingTogglesState(true);
    addAndMakeVisible(tapdelay);
    tapdelay.addListener(this);
    //tapdelay.setTriggeredOnMouseDown(false);
    addAndMakeVisible(delaytime);
    
    // Preset1
    
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (550, 400);
    
    // STEP 5.4 - start the timer
    startTimer (80);      // timerCallback() will get called every N msec.
    
    tempoTimer = tempoTimer.getCurrentTime();
    //previous = clock();
    previous = tempoTimer.currentTimeMillis();
    firstInterval = 0;
    secondInterval = 0;
    thirdInterval = 0;
    
    
    
}

Musi45effectAudioProcessorEditor::~Musi45effectAudioProcessorEditor()
{
}

//==============================================================================
void Musi45effectAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (Colours::black);
    
    // STEP 1.6 - add some text to the UI
    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("FLORUS", getLocalBounds(), Justification::centredTop, 1);
    g.drawFittedText("Presets", 150, 265, 100, 50, Justification::centred, 1);
    g.drawFittedText("BPM", 465, 335, 100, 50, Justification::centred, 1);
    
    g.drawLine(185, 40, 185, 275, 2);
    g.drawLine(435, 40, 435, 400, 2);
    
    g.drawLine(0, 275, 435, 275, 2);
    g.drawLine(0, 40, 550, 40, 2);
    
    
}

void Musi45effectAudioProcessorEditor::resized()
{
    // Set the position and size of the slider objects
    int w = 80; int h = 400; int x = 40; int y = 60;
    wetSlider.setBounds(x-20, y, w, h - 200); x += w;
    drySlider.setBounds(x-20, y, w, h - 200); x += w;
    lfoDepthSlider.setBounds(x - 10, y, w, h - 200); x += w;
    lfoSpeedSlider.setBounds(x - 10, y, w, h - 200); x += w;
    feedbackSlider.setBounds(x - 10, y, w, h - 200); x += w;
    delayTimeSlider.setBounds(x+10, y, w, h - 200); x += w;
    
    
    //tapdelay.setBounds(x, y*2+40, 40, 40); x+=w;
    //delaytime.setBounds(x, y*2+40, w, h/2); x = 60; y = 300;
    
    x = 60; y = 300;
    
    preset1.setBounds(x, y, 40, 40);
    set1.setBounds(x, y + 50, 40, 40); x +=w;
    preset2.setBounds(x, y, 40, 40);
    set2.setBounds(x, y + 50, 40, 40); x +=w;
    preset3.setBounds(x, y, 40, 40);
    set3.setBounds(x, y + 50, 40, 40); x +=w;
    preset4.setBounds(x, y, 40, 40);
    set4.setBounds(x, y + 50, 40, 40); x +=w;
    x += 60;
    tapdelay.setBounds(x+10, y, 80, 40);
    delaytime.setBounds(x + 15, y+50, 40, 20);
    
    
}

//==============================================================================
// This is our Slider::Listener callback. Get's called when the user drags a slider.
void Musi45effectAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
    // by the host, rather than just modifying them directly, otherwise the host won't know
    // that they've changed.
    
    float vstVal, sliderVal;
    int paramIndex;
    
    if (slider == &lfoDepthSlider){
        paramIndex = Musi45effectAudioProcessor::lfoDepthParam;
    }
    else if (slider == &lfoSpeedSlider){
        paramIndex = Musi45effectAudioProcessor::lfoSpeedParam;
    }
    else if (slider == &feedbackSlider){
        paramIndex = Musi45effectAudioProcessor::feedbackParam;
    }
    else if (slider == &delayTimeSlider){
        paramIndex = Musi45effectAudioProcessor::delayTimeParam;
    }
    else if (slider == &wetSlider){
        paramIndex = Musi45effectAudioProcessor::wetParam;
    }
    else if (slider == &drySlider){
        paramIndex = Musi45effectAudioProcessor::dryParam;
    }
    
    // first set the usrParam from the slider, and get the vst normalized version of the uParam:
    sliderVal = slider->getValue();
    vstVal = getProcessor().usrParams[paramIndex].setWithUparam(sliderVal);
    
    // then call setParameter with the vst normalized value
    getProcessor().setParameterNotifyingHost(paramIndex, vstVal);
    
    
    
}

// button callback
void Musi45effectAudioProcessorEditor::buttonClicked(Button * button)
{
    
    if (button == &tapdelay) {
        /*if(tapdelay.getToggleState()) {
         float vstVal, sliderVal;
         int paramIndex;
         String time = std::to_string(tapdelay.getMillisecondsSinceButtonDown());
         delaytime.setText(time);
         std::cout << tapdelay.getMillisecondsSinceButtonDown() << std::endl;
         std::cout << time << std::endl;
         paramIndex = Musi45effectAudioProcessor::delayTimeParam;
         sliderVal = tapdelay.getMillisecondsSinceButtonDown();
         vstVal = getProcessor().usrParams[paramIndex].setWithUparam(sliderVal);
         getProcessor().setParameterNotifyingHost(paramIndex, vstVal);
         }*/
        //if(((clock()-previous)/(double) CLOCKS_PE_SEC)* ) {
        //current = clock();
        
        current = tempoTimer.currentTimeMillis();
        
        int newtimeIntervalMs = current - previous;
        
        //tempoTimer->stopTimer();
        //tempoTimer->startTimer(20);
        
        
        //average the most recent 4 time intervals
        
        fourthInterval = thirdInterval;
        thirdInterval = secondInterval;
        secondInterval = firstInterval;
        firstInterval = newtimeIntervalMs;
        
        double avg = (fourthInterval + thirdInterval + secondInterval + firstInterval)/4;
        
        
        //clockstaken = current - previous;
        
        //double timeInSeconds = clockstaken / (double) CLOCKS_PER_SEC;
        //double timeInMs = timeInSeconds * 1000;
        
        std::cout << newtimeIntervalMs << std::endl;
        
        
        int bpm = 60000/avg;
        
        
        
        delaytime.setText(std::to_string(bpm));
        
        int paramIndex = Musi45effectAudioProcessor::delayTimeParam;
        getProcessor().usrParams[paramIndex].setWithUparam(avg);
        
        
        previous = current;
        
        
    }
    
    else if (button == &preset1){
        usePreset(presets1);
    }
    else if (button == &preset2){
        usePreset(presets2);
    }
    else if (button == &preset3){
        usePreset(presets3);
    }
    else if (button == &preset4){
        usePreset(presets4);
    }
    else if (button == &set1){
        setPresets(presets1);
    }
    else if (button == &set2){
        setPresets(presets2);
    }
    else if (button == &set3){
        setPresets(presets3);
    }
    else if (button == &set4){
        setPresets(presets4);
    }
    
    
}

// button callback
void Musi45effectAudioProcessorEditor::buttonStateChanged(Button * button)
{
    
}



//==============================================================================
// STEP 5.3 - Set the timer callback to update the UI
// This timer periodically checks whether any of the filter's parameters have changed...
void Musi45effectAudioProcessorEditor::timerCallback()
{
    // get a pointer to our Processor object
    Musi45effectAudioProcessor& ourProc = getProcessor();
    
    // these get will update the sliders in the UI
    lfoSpeedSlider.setValue(ourProc.usrParams[Musi45effectAudioProcessor::lfoSpeedParam].getUparamVal(), dontSendNotification);
    lfoDepthSlider.setValue(ourProc.usrParams[Musi45effectAudioProcessor::lfoDepthParam].getUparamVal(), dontSendNotification);
    feedbackSlider.setValue(ourProc.usrParams[Musi45effectAudioProcessor::feedbackParam].getUparamVal(), dontSendNotification);
    delayTimeSlider.setValue(ourProc.usrParams[Musi45effectAudioProcessor::delayTimeParam].getUparamVal(), dontSendNotification);
    wetSlider.setValue(ourProc.usrParams[Musi45effectAudioProcessor::wetParam].getUparamVal(), dontSendNotification);
    drySlider.setValue(ourProc.usrParams[Musi45effectAudioProcessor::dryParam].getUparamVal(), dontSendNotification);
    
}

void Musi45effectAudioProcessorEditor::setPresets(float values[]) {
    values[0] = getProcessor().usrParams[Musi45effectAudioProcessor::lfoSpeedParam].getUparamVal();
    values[1] = getProcessor().usrParams[Musi45effectAudioProcessor::lfoDepthParam].getUparamVal();
    values[2] = getProcessor().usrParams[Musi45effectAudioProcessor::feedbackParam].getUparamVal();
    values[3] = getProcessor().usrParams[Musi45effectAudioProcessor::delayTimeParam].getUparamVal();
    values[4] = getProcessor().usrParams[Musi45effectAudioProcessor::wetParam].getUparamVal();
    values[5] = getProcessor().usrParams[Musi45effectAudioProcessor::dryParam].getUparamVal();
}

void Musi45effectAudioProcessorEditor::usePreset(float values[]) {
    getProcessor().usrParams[Musi45effectAudioProcessor::lfoSpeedParam].setWithUparam(values[0]);
    getProcessor().usrParams[Musi45effectAudioProcessor::lfoDepthParam].setWithUparam(values[1]);
    getProcessor().usrParams[Musi45effectAudioProcessor::feedbackParam].setWithUparam(values[2]);
    getProcessor().usrParams[Musi45effectAudioProcessor::delayTimeParam].setWithUparam(values[3]);
    getProcessor().usrParams[Musi45effectAudioProcessor::wetParam].setWithUparam(values[4]);
    getProcessor().usrParams[Musi45effectAudioProcessor::dryParam].setWithUparam(values[5]);
}

