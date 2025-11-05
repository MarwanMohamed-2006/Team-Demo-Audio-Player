#include <JuceHeader.h>
#include "PlayerAudio.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
using namespace std;
using namespace juce;


class PlayerAudio;
class PlaylistListBoxModel;


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
	void timerCallback() override;
	void updateProgressBar();




	void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill);
	void releaseResources();

private:
	bool isPlaying = false;
	PlayerAudio playerAudio;
	double progressvalue = 0.0;


	// GUI elements;
	TextButton playPauseButton{ "Play" };
	TextButton loadButton{ "Load File" };
	TextButton gotostartButton{ "go to start" };
	TextButton endButton{ "End" };
	TextButton muteButton;
	ToggleButton loopButton{ "Loop" };
	bool isLooping = false;
	TextButton reset_speed{ "Speed Reset" };

	TextButton setA_Button{ "Set A" };
	TextButton setB_Button{ "Set B" };
	TextButton clearABButton{ "Clear A-B" };
	ToggleButton runABButton{ "A-B Loop" };

	Slider volumeSlider;
	Slider speedSlider;
	Slider positionSlider;
	Label timeLabel;
	Label metadataLabel;
	ProgressBar display{ progressvalue };

	bool setAMarker = false;
	bool setBMarker = false;
	double aMarkerPos = -1.0;
	double bMarkerPos = -1.0;

	bool isDraggingPosition = false;

	std::unique_ptr<juce::FileChooser> fileChooser;

	void buttonClicked(Button* button) override;
	void sliderValueChanged(Slider* slider) override;
	juce::String formatTime(double seconds);

	juce::ListBox playlistListBox;
	std::unique_ptr<PlaylistListBoxModel> playlistModel;



	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)

};