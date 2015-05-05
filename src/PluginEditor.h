/*
 ==============================================================================
 
 This file was auto-generated by the Introjucer!
 
 It contains the basic framework code for a JUCE plugin editor.
 
 ==============================================================================
 */

#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
 */
class Musi45effectAudioProcessorEditor  :   public AudioProcessorEditor,
public SliderListener,  // STEP 3.1 - make Editor a SliderListener
public Timer,            // STEP 5.1 - make Editor a Timer
public ButtonListener
{
public:
    Musi45effectAudioProcessorEditor (Musi45effectAudioProcessor&);
    ~Musi45effectAudioProcessorEditor();
    
    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void sliderValueChanged (Slider*) override; // add the slider callback function
    void buttonStateChanged (Button *) override;
    void buttonClicked (Button *) override;
    void timerCallback() override;              // add the timer callback function
    void setPresets(float values[]);
    void usePreset(float values[]);
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Musi45effectAudioProcessor& processor;
    
    // Add sliders here:
    Slider lfoDepthSlider;
    Slider lfoSpeedSlider;
    Slider feedbackSlider;
    Slider delayTimeSlider;
    Slider wetSlider;
    Slider drySlider;
    
    TextButton preset1;
    TextButton preset2;
    TextButton preset3;
    TextButton preset4;
    
    TextButton set1;
    TextButton set2;
    TextButton set3;
    TextButton set4;
    
    TextButton tapdelay;
    TextEditor delaytime;
    
    // Add labels here:
    Label lfoDepthLabel;
    Label lfoSpeedLabel;
    Label feedbackLabel;
    Label delayTimeLabel;
    Label wetLabel;
    Label dryLabel;
    
    // Add any other private variables you need here.
    
    //    std::clock_t current;
    //    std::clock_t previous;
    //    std::clock_t clockstaken;
    
    Time tempoTimer;
    int current;
    int previous;
    
    int firstInterval;
    int secondInterval;
    int thirdInterval;
    int fourthInterval;
    
    float presets1 [6];
    float presets2 [6];
    float presets3 [6];
    float presets4 [6];
    
    
    // Timer * previous;
    // Timer * clockstaken;
    // STEP 3.5 - Add a method that returns a pointer to the Processor object. This is how the Editor can send data to the processor
    Musi45effectAudioProcessor& getProcessor() const
    {
        return static_cast<Musi45effectAudioProcessor&> (processor);
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Musi45effectAudioProcessorEditor)
};


#endif  // PLUGINEDITOR_H_INCLUDED
