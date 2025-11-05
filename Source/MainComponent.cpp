#include "MainComponent.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>

MainComponent::MainComponent()
{
    backgroundImage = juce::ImageCache::getFromFile(juce::File("C:\\Users\\Administrator\\Downloads\\darkness birds.png"));

    addAndMakeVisible(player1);
    addAndMakeVisible(player2);
    setSize(1000, 500);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    player1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    player2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    player1.getNextAudioBlock(bufferToFill);
    player2.getNextAudioBlock(bufferToFill);
    
}


void MainComponent::releaseResources()
{
    player1.releaseResources();
    player2.releaseResources();
}


void MainComponent::resized() 
{
    int halfHeight = getHeight() / 2;
    player1.setBounds(0, 0, getWidth(), halfHeight);
    player2.setBounds(0, halfHeight, getWidth(), getHeight() - halfHeight);
}
void MainComponent::paint(juce::Graphics& g)
{
    if (backgroundImage.isValid())
    {
        g.drawImage(backgroundImage, getLocalBounds().toFloat());
    }
    else
    {
        g.fillAll(juce::Colours::black);
    }
}