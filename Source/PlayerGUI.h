#include <JuceHeader.h>
#include "PlayerAudio.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
using namespace std;
using namespace juce;


class PlayerAudio;
class PlaylistListBoxModel;
class CircularSpeedLookAndFeel : public juce::LookAndFeel_V4
{
public:
	void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
		float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
		juce::Slider& slider) override
	{
		auto radius = (float)juce::jmin(width / 2, height / 2) - 4.0f;
		auto centreX = (float)x + (float)width * 0.5f;
		auto centreY = (float)y + (float)height * 0.5f;
		auto rx = centreX - radius;
		auto ry = centreY - radius;
		auto rw = radius * 2.0f;
		auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

		// Fill
		g.setColour(juce::Colours::white.withAlpha(0.3f));
		g.fillEllipse(rx, ry, rw, rw);

		// Outline
		g.setColour(juce::Colours::white);
		g.drawEllipse(rx, ry, rw, rw, 2.0f);

		// Pointer
		juce::Path p;
		auto pointerLength = radius * 0.8f;
		auto pointerThickness = 3.0f;
		p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
		p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));

		g.setColour(juce::Colours::lightgrey);
		g.fillPath(p);

		// Display speed value in center
		g.setColour(juce::Colours::white);
		g.setFont(12.0f);
		auto text = juce::String(slider.getValue(), 2) + "x";
		g.drawText(text, (int)rx, (int)(centreY - 8), (int)rw, 16, juce::Justification::centred);
	}
};


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
	void playNextSong();
	void timerCallback() override;
	void updateProgressBar();




	void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill);
	void releaseResources();

private:
	bool isPlaying = false;
	PlayerAudio playerAudio;
	double progressvalue = 0.0;
	CircularSpeedLookAndFeel circularLookAndFeel;

	// GUI elements;
	TextButton playPauseButton{ "Pause" };
	TextButton loadButton{ "Load File" };
	TextButton gotostartButton{ "Restart" };
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