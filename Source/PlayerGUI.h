#pragma once // PlayerGUI.h
#include <JuceHeader.h>
#include "PlayerAudio.h"
using namespace std;
using namespace juce;

class PlayerGUI : public juce::Component,
	public Button::Listener,
	public Slider::Listener,
	public Timer
{
public:
	PlayerGUI();
	~PlayerGUI() override;

	void resized() override;
	void paint(Graphics& g) override;

	void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill);
	void releaseResources();

	void timerCallback() override;

private:
	PlayerAudio playerAudio;
	// GUI elements
	TextButton stopButton{ "Stop" };
	TextButton playButton{ "play" };
	TextButton loadButton{ "Load File" };
	TextButton gotostartButton{ "go to start" };
	TextButton endButton{ "End" };
	TextButton muteButton;
	ToggleButton loopButton{ "Loop" };
	bool isLooping = false;

	TextButton setA_Button{ "Set A" };
	TextButton setB_Button{ "Set B" };
	TextButton clearABButton{ "Clear A-B" };
	ToggleButton runABButton{ "A-B Loop" };

	juce::Slider volumeSlider;
	juce::Slider positionSlider;
	juce::Label timeLabel;

	bool setAMarker = false;
	bool setBMarker = false;
	double aMarkerPos = -1.0;
	double bMarkerPos  = -1.0;

	bool isDraggingPosition = false;

	std::unique_ptr<juce::FileChooser> fileChooser;
	// Event handlers
	void buttonClicked(Button* button) override;
	void sliderValueChanged(Slider* slider) override;
	juce::String formatTime(double seconds);
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)

};