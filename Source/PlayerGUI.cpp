#include "PlayerGUI.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <memory>

using namespace std;
using namespace juce;


class PlaylistListBoxModel : public juce::ListBoxModel
{
public:
    PlaylistListBoxModel(PlayerAudio& audio) : playerAudio(audio) {}

    int getNumRows() override
    {
        // The number of rows is the size of the playlist in PlayerAudio
        return (int)playerAudio.getPlaylist().size();
    }

    void paintListBoxItem(int rowNumber, juce::Graphics& g,
        int width, int height, bool rowIsSelected) override
    {
        if (rowIsSelected)
            g.fillAll(juce::Colours::blue.withAlpha(0.5f)); // Highlight selection

        if (rowNumber < getNumRows())
        {
            // Display filename without extension
            const juce::File& file = playerAudio.getPlaylist()[rowNumber];
            g.setColour(juce::Colours::white);
            g.setFont(height * 0.7f);
            g.drawText(file.getFileNameWithoutExtension(),
                5, 0, width - 10, height,
                juce::Justification::centredLeft, true);
        }
    }

    void selectedRowsChanged(int lastRowSelected) override
    {
        // When a row is selected (clicked), load and play that file
        if (lastRowSelected >= 0)
        {
            playerAudio.loadAndPlayFile(lastRowSelected);
        }
    }


private:

    PlayerAudio& playerAudio;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlaylistListBoxModel)
};




PlayerGUI::PlayerGUI()
    : playlistModel(std::make_unique<PlaylistListBoxModel>(playerAudio))
{
    // Add buttons
    for (auto* btn : { &loadButton, &endButton ,  &playPauseButton , &muteButton, &gotostartButton,&setA_Button, &setB_Button, &clearABButton,&reset_speed })

    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }



    playlistListBox.setModel(playlistModel.get());
    addAndMakeVisible(playlistListBox);




    addAndMakeVisible(metadataLabel);
    metadataLabel.setText("No file loaded", juce::dontSendNotification);
    metadataLabel.setJustificationType(juce::Justification::centredLeft);
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::yellow);
    addAndMakeVisible(metadataLabel);


    playerAudio.onMetadataLoaded = [this](const juce::String& title, const juce::String& artist)
        {
            juce::String metadata = artist.isEmpty() ? title : (title + " - " + artist);
            metadataLabel.setText("Now Playing: " + metadata, juce::dontSendNotification);
        };


    // A-B run checkbox
    runABButton.addListener(this);
    addAndMakeVisible(runABButton);

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


    // Speed slider 
    speedSlider.setRange(0.5, 2.0, 0.01);
    speedSlider.setValue(1);
    speedSlider.addListener(this);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speedSlider.setSliderStyle(juce::Slider::LinearVertical);
    addAndMakeVisible(speedSlider);

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

    //display progress bar
    startTimerHz(20);
    addAndMakeVisible(display);

    // loop button
    loopButton.addListener(this);
    addAndMakeVisible(loopButton);

    loopButton.onClick = [this]()
        {
            isLooping = loopButton.getToggleState();
            playerAudio.setLooping(isLooping);
        };
    addAndMakeVisible(&loopButton);

    runABButton.onClick = [this]()
        {
            bool abEnabled = runABButton.getToggleState();
            playerAudio.enableABLoop(abEnabled);
            if (abEnabled)
            {
                loopButton.setToggleState(false, juce::sendNotification);
                playerAudio.setLooping(false);
            }
            repaint();
        };
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

void PlayerGUI::resized()
{
    int y = 20;
    loadButton.setBounds(20, y, 100, 40);
    playPauseButton.setBounds(240, y, 80, 40);
    muteButton.setBounds(340, y, 80, 40);
    endButton.setBounds(540, y, 80, 40);
    gotostartButton.setBounds(640, y, 100, 40);
    loopButton.setBounds(760, y, 80, 40);
    reset_speed.setBounds(getWidth() - 160, 125, 80, 30);  


    volumeSlider.setBounds(getWidth() - 200, 30, 50, 100);
    speedSlider.setBounds(getWidth() - 150, 30, 50, 100);
    positionSlider.setBounds(20, 150, getWidth() - 40, 30);
    display.setBounds(20, 210, getWidth() - 40, 20);
    timeLabel.setBounds(20, 185, getWidth() - 40, 20);

    setA_Button.setBounds(860, y, 80, 40);
    setB_Button.setBounds(960, y, 80, 40);
    clearABButton.setBounds(1060, y, 100, 40);
    runABButton.setBounds(1160, y, 100, 40);

    metadataLabel.setBounds(20, y + 50, getWidth() - 40, 30);
    playlistListBox.setBounds(20, 250, getWidth() - 40, 150);
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        juce::File startDir = juce::File::getSpecialLocation(juce::File::userHomeDirectory);

        fileChooser.reset(new juce::FileChooser(
            "Select Audio Files to add to Playlist",
            startDir,
            playerAudio.getWildcardFiles()
        ));


        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectMultipleItems,
            // -----------------------------------------

            [this](const juce::FileChooser& fc)
            {
                auto files = fc.getResults(); // Get ALL selected files
                if (files.size() > 0)
                {
                    size_t initialSize = playerAudio.getPlaylist().size();

                    // Add files to the audio model
                    playerAudio.addFilesToPlaylist(files);

                    // Force the ListBox to refresh its contents
                    playlistListBox.updateContent();

                    // If the playlist was previously empty, load and play the first file
                    if (initialSize == 0)
                    {
                        playerAudio.loadAndPlayFile(0);
                        playlistListBox.selectRow(0); // Highlight the first file
                    }

                    // Reset markers
                    setAMarker = false;
                    setBMarker = false;
                    aMarkerPos = -1.0;
                    bMarkerPos = -1.0;
                    repaint();

                }
            });
    }

    if (button == &playPauseButton)
    {
        if (isPlaying)
        {
            playerAudio.stop();
            playPauseButton.setButtonText("Play");
            isPlaying = false; 
        }
        else
        {
            playerAudio.start();
            playPauseButton.setButtonText("Pause");
            isPlaying = true; 
        }
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

    if (button == &setA_Button)
    {
        double currentPos = playerAudio.getPosition();
        playerAudio.set_start(currentPos);
        aMarkerPos = playerAudio.startpercentage();
        setAMarker = true;
        repaint();
    }

    if (button == &setB_Button)
    {
        double currentPos = playerAudio.getPosition();
        playerAudio.set_end(currentPos);
        bMarkerPos = playerAudio.endpercentage();
        setBMarker = true;
        repaint();
    }

    if (button == &clearABButton)
    {
        playerAudio.clearABLoop();
        runABButton.setToggleState(false, juce::sendNotification);
        setAMarker = false;
        setBMarker = false;
        aMarkerPos = -1.0;
        bMarkerPos = -1.0;
        repaint();
    }
    if (button == &reset_speed)
    {
		speedSlider.setValue(1.0);
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
    if (slider == &speedSlider)
        playerAudio.setSpeed((float)slider->getValue());
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

        if (setAMarker)
        {
            aMarkerPos = playerAudio.startpercentage();
        }
        if (setBMarker)
        {
            bMarkerPos = playerAudio.endpercentage();
        }
    }
    else
    {
        setAMarker = false;
        setBMarker = false;
        aMarkerPos = -1.0;
        bMarkerPos = -1.0;
    }


    updateProgressBar();

    repaint();
}

void PlayerGUI::updateProgressBar()
{
    double position = playerAudio.getPosition();
    double length = playerAudio.getLength();
    if (length > 0.0)
        progressvalue = position / length;
    else
        progressvalue = 0.0;
}

juce::String PlayerGUI::formatTime(double seconds)
{
    int totalSeconds = static_cast<int>(seconds);
    int minutes = totalSeconds / 60;
    int secs = totalSeconds % 60;

    return juce::String::formatted("%02d:%02d", minutes, secs);
}

void PlayerGUI::paint(juce::Graphics& g)
{
    //  g.fillAll(juce::Colours::darkgrey);

    if (setAMarker && aMarkerPos >= 0.0)
    {
        auto sliderBounds = positionSlider.getBounds().toFloat();
        const float aX = sliderBounds.getX() + (sliderBounds.getWidth() * aMarkerPos);

        g.setColour(Colours::green);
        g.drawLine(aX, sliderBounds.getY(), aX, sliderBounds.getBottom(), 3.0f);

        g.setColour(Colours::white);
        g.setFont(12.0f);
        g.drawText("A", aX - 10, sliderBounds.getY() - 20, 20, 15, Justification::centred);
    }

    if (setBMarker && bMarkerPos >= 0.0)
    {
        auto sliderBounds = positionSlider.getBounds().toFloat();
        const float bX = sliderBounds.getX() + (sliderBounds.getWidth() * bMarkerPos);

        g.setColour(Colours::red);
        g.drawLine(bX, sliderBounds.getY(), bX, sliderBounds.getBottom(), 3.0f);

        g.setColour(Colours::white);
        g.setFont(12.0f);
        g.drawText("B", bX - 10, sliderBounds.getY() - 20, 20, 15, Justification::centred);
    }

    if (setAMarker && setBMarker && aMarkerPos >= 0.0 && bMarkerPos >= 0.0 && aMarkerPos < bMarkerPos &&
        playerAudio.isABLoopEnabled())
    {
        auto sliderBounds = positionSlider.getBounds().toFloat();
        const float startX = sliderBounds.getX() + (sliderBounds.getWidth() * aMarkerPos);
        const float endX = sliderBounds.getX() + (sliderBounds.getWidth() * bMarkerPos);

        g.setColour(Colours::yellow.withAlpha(0.3f));
        g.fillRect(startX, sliderBounds.getY(), endX - startX, sliderBounds.getHeight());
    }
}