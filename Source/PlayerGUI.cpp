#include "PlayerGUI.h"

using namespace std;
using namespace juce;

PlayerGUI::PlayerGUI()
{
    // Add buttons
    for (auto* btn : { &loadButton, &endButton , &stopButton , &playButton , &muteButton, &gotostartButton, })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    // position slider
    positionSlider.setRange(0.0, 1.0, 0.001);
    positionSlider.setValue(0.0);
    positionSlider.addListener(this);
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(positionSlider);
    startTimer(100);

    timeLabel.setText("00:00 / 00:00", juce::dontSendNotification);
    timeLabel.setJustificationType(juce::Justification::centred);
    timeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(timeLabel);



    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.setSliderStyle(juce::Slider::LinearVertical);
    addAndMakeVisible(volumeSlider);

    // muted button 
    muteButton.setButtonText("Mute");
    muteButton.setToggleState(true, juce::dontSendNotification);
    muteButton.setClickingTogglesState(true);

    muteButton.onClick = [this]() {
        const bool ismuted = muteButton.getToggleState();
        muteButton.setButtonText(ismuted ? "mute" : "unmute");
        playerAudio.setGain(ismuted ? 1.0f : 0.0f);
        };
    addAndMakeVisible(&muteButton);

    // loop button
    loopButton.addListener(this);
    addAndMakeVisible(loopButton);

    loopButton.onClick = [this]()
        {
            isLooping = loopButton.getToggleState();
            playerAudio.setLooping(isLooping);
        };
    addAndMakeVisible(&loopButton);
}
PlayerGUI::~PlayerGUI()
{
}

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}
void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);
}


void PlayerGUI::resized()
{
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    stopButton.setBounds(240, y, 80, 40);
    muteButton.setBounds(340, y, 80, 40);
    playButton.setBounds(440, y, 80, 40);
    endButton.setBounds(540, y, 80, 40);
    gotostartButton.setBounds(640, y, 100, 40);
    loopButton.setBounds(760, y, 80, 40);

    /*prevButton.setBounds(340, y, 80, 40);
    nextButton.setBounds(440, y, 80, 40);*/

    volumeSlider.setBounds(500, 50, getWidth() - 40, 100);
    positionSlider.setBounds(20, 150, getWidth() - 40, 30);
    timeLabel.setBounds(20, 185, getWidth() - 40, 20);



}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        juce::FileChooser chooser("Select audio files...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                {
                    playerAudio.loadFile(file);
                }
            });
    }


    if (button == &stopButton)
    {
        playerAudio.stop();
        playerAudio.setPosition(0.0);
    }

    if (button == &playButton)
    {
        playerAudio.start();
    }


    if (button == &muteButton)
    {
        playerAudio.mute();
    }
    if (button == &gotostartButton) {
        playerAudio.start();
        playerAudio.setPosition(0.0);
    }
    if (button == &endButton) {
        playerAudio.getLength();
        playerAudio.setPosition(playerAudio.getLength());
    }
}


void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        playerAudio.setGain((float)slider->getValue());
    }
    else if (slider == &positionSlider)
    {
        if (positionSlider.isMouseButtonDown())
        {
            double length = playerAudio.getLength();
            if (length > 0)
            {
                double newPosition = positionSlider.getValue() * length;
                playerAudio.setPosition(newPosition);
            }
        }
    }
}
void PlayerGUI::timerCallback()
{
    double length = playerAudio.getLength();
    if (length > 0.0)
    {
        double current_pos = playerAudio.getPosition();
        double new_pos = current_pos / length;

        if (!positionSlider.isMouseButtonDown())
        {
            positionSlider.setValue(new_pos);
        }
        juce::String currentTime = formatTime(current_pos);
        juce::String totalTime = formatTime(length);
        timeLabel.setText(currentTime + " / " + totalTime, juce::dontSendNotification);
    }
}
juce::String PlayerGUI::formatTime(double seconds)
{
    int totalSeconds = static_cast<int>(seconds);
    int minutes = totalSeconds / 60;
    int secs = totalSeconds % 60;

    return juce::String::formatted("%02d:%02d", minutes, secs);
}