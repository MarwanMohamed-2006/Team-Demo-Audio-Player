#include "MainComponent.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>

MainComponent::MainComponent()
{
    backgroundImage = juce::ImageCache::getFromFile(juce::File("C:\\Users\\Administrator\\Downloads\\space2.png"));
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
    juce::AudioBuffer<float> tempBuffer(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    juce::AudioSourceChannelInfo tempbuffer(&tempBuffer, 0, bufferToFill.numSamples);

    player2.getNextAudioBlock(tempbuffer);
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        bufferToFill.buffer->addFrom(channel, bufferToFill.startSample, tempBuffer, channel, 0, bufferToFill.numSamples);
    }
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
    int halfHeight = getHeight() / 2;
    g.setColour(juce::Colours::darkblue);
    g.fillRect(0, halfHeight - 50, getWidth(), 10);
}