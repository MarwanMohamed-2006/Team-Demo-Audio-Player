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
        btn->setColour(juce::TextButton::textColourOffId, juce::Colours::black);
        btn->setColour(juce::TextButton::buttonColourId, juce::Colours::white);
    }

    loopButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    loopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);
    runABButton.setColour(juce::TextButton::textColourOffId, juce::Colours::black);
    runABButton.setColour(juce::TextButton::buttonColourId, juce::Colours::white);


    playlistListBox.setModel(playlistModel.get());
    addAndMakeVisible(playlistListBox);




    addAndMakeVisible(metadataLabel);
    metadataLabel.setText("No file loaded", juce::dontSendNotification);
    metadataLabel.setJustificationType(juce::Justification::centredLeft);
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(metadataLabel);


    playerAudio.onMetadataLoaded = [this](const juce::String& title
                                        , const juce::String& artist
                                        , const juce::String& album)
        {
            juce::String metadata =
                                    "Title: " + title + "\n" +
                                    "Artist: " + artist + "\n" +
                                    "Album: " + album;
            metadataLabel.setText(metadata, juce::dontSendNotification);
        };



    runABButton.addListener(this);
    addAndMakeVisible(runABButton);


    positionSlider.setRange(0.0, 1.0, 0.001);
    positionSlider.setValue(0.0);
    positionSlider.addListener(this);
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::darkgrey);

    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(positionSlider);
    startTimer(100);

    timeLabel.setText("00:00 / 00:00", juce::dontSendNotification);
    timeLabel.setJustificationType(juce::Justification::centred);
    timeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(timeLabel);


    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.setSliderStyle(juce::Slider::LinearBarVertical);
    volumeSlider.setColour(juce::Slider::trackColourId, juce::Colours::white);

    addAndMakeVisible(volumeSlider);


    speedSlider.setRange(0.5, 2.0, 0.01);
    speedSlider.setValue(1);
    speedSlider.addListener(this);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    speedSlider.setSliderStyle(juce::Slider::Rotary); 
    speedSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::white);
    speedSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);
    speedSlider.setColour(juce::Slider::thumbColourId, juce::Colours::lightgrey);
    speedSlider.setLookAndFeel(&circularLookAndFeel);
    addAndMakeVisible(speedSlider);


    muteButton.setButtonText("Mute");
    muteButton.setToggleState(true, juce::dontSendNotification);
    muteButton.setClickingTogglesState(true);
    muteButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::white);
    muteButton.setColour(juce::TextButton::textColourOnId, juce::Colours::red);


    muteButton.onClick = [this]() {
        const bool ismuted = muteButton.getToggleState();
        muteButton.setButtonText(ismuted ? "mute" : "unmute");
        playerAudio.setGain(ismuted ? 1.0f : 0.0f);
        };
    addAndMakeVisible(&muteButton);


    startTimerHz(20);
    addAndMakeVisible(display);


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
    loadButton.setBounds(20, 20, 100, 40);
    metadataLabel.setBounds(20, 100, getWidth() - 40, 80);

    gotostartButton.setBounds(60, 260, 200, 45);
    playPauseButton.setBounds(300, 260, 200, 45);
    endButton.setBounds(540, 260, 200, 45);
    muteButton.setBounds(540, 400, 70, 30);

    volumeSlider.setBounds(560, 315, 20, 80); 
    speedSlider.setBounds(620, 310, 80, 80);
    reset_speed.setBounds(620, 400, 80, 30);

    positionSlider.setBounds(20, 180, getWidth() - 40, 30);
    timeLabel.setBounds(20, 150, getWidth() - 40, 20);
    display.setBounds(20, 220, getWidth() - 40, 20);

    setA_Button.setBounds(20, 360, 80, 40);
    setB_Button.setBounds(110, 360, 80, 40);
    clearABButton.setBounds(200, 360, 100, 40);
    runABButton.setBounds(310, 360, 100, 40);
    loopButton.setBounds(420, 360, 80, 40);

    playlistListBox.setBounds(20, 450, getWidth() - 40, 200);
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
                auto files = fc.getResults();
                if (files.size() > 0)
                {
                    size_t initialSize = playerAudio.getPlaylist().size();


                    playerAudio.addFilesToPlaylist(files);


                    playlistListBox.updateContent();


                    if (initialSize == 0)
                    {
                        playerAudio.loadAndPlayFile(0);
                        playlistListBox.selectRow(0);
                    }


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

        if (!isLooping && !playerAudio.isABLoopEnabled() && (current_pos >= length - 0.05)) 
        {
            playNextSong();
            return;
        }

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
    g.fillAll(juce::Colours::black);

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
void PlayerGUI::playNextSong()
{
    auto& playlist = playerAudio.getPlaylist();
    int currentIndex = playerAudio.getCurrentIndex();

    if (currentIndex >= 0 && currentIndex + 1 < (int)playlist.size())
    {
        int nextIndex = currentIndex + 1;
        playerAudio.loadAndPlayFile(nextIndex);
        playlistListBox.selectRow(nextIndex);
    }
    else
    {
        isPlaying = false;
        playPauseButton.setButtonText("Play");
    }
}