#include "MainComponent.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>

MainComponent::MainComponent()
{
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
    int gap = 4;
    int halfWidth = getWidth() / 2;

    player1.setBounds(0, 0, halfWidth - gap / 2, getHeight());
    player2.setBounds(halfWidth + gap / 2, 0, getWidth() - halfWidth - gap / 2, getHeight());
}

void MainComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::cornsilk);
    int line_pos = getWidth() / 2;
    g.fillRect(line_pos - 2, 0, 4, getHeight());
}