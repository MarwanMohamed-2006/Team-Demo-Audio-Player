#include "PlayerAudio.h"
#include <taglib/fileref.h>
#include <taglib/tag.h>
PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);

   
    if (abLoopEnabled && transportSource.getCurrentPosition() > loopEndTime)
    {
        transportSource.setPosition(loopStartTime);
        transportSource.start();
    }
    else if (looping && !abLoopEnabled && transportSource.hasStreamFinished())
    {
        transportSource.setPosition(0.0);
        transportSource.start();
    }
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}

bool PlayerAudio::loadFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            // Disconnect old source first
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            // Create new reader source
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            // Attach safely
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);

            loopEndTime = getLength();

            transportSource.start();
            return true; 
        }
    }
    return false;
}

void PlayerAudio::start()
{
    transportSource.start();
}

void PlayerAudio::stop()
{
    transportSource.stop();
}

void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
}

void PlayerAudio::setPosition(double pos)
{
    transportSource.setPosition(pos);
}

double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}

void PlayerAudio::mute()
{
    transportSource.setGain(0.0f);
}

void PlayerAudio::unmute()
{
    transportSource.setGain(1.0f);
}

void PlayerAudio::setLooping(bool shouldLoop)
{
    looping = shouldLoop;
}

bool PlayerAudio::isLooping() const
{
    return looping;
}

void PlayerAudio::set_start(double startTime)
{
    loopStartTime = juce::jlimit(0.0, getLength(), startTime);
}

void PlayerAudio::set_end(double endTime)
{
    loopEndTime = juce::jlimit(0.0, getLength(), endTime);
}

void PlayerAudio::enableABLoop(bool enable)
{
    abLoopEnabled = enable;
    if (enable)
        looping = false;
}

bool PlayerAudio::isABLoopEnabled() const
{
    return abLoopEnabled;
}

double PlayerAudio::getLoopStart() const
{
    return loopStartTime;
}

double PlayerAudio::getLoopEnd() const
{
    return loopEndTime;
}

void PlayerAudio::clearABLoop()
{
    abLoopEnabled = false;
    loopStartTime = 0.0;
    double length = getLength();
    loopEndTime = length > 0.0 ? length : 0.0;
}


void PlayerAudio::extractMetadata(const juce::File& file)
{
    TagLib::FileRef f(file.getFullPathName().toRawUTF8());

    
    if (f.isNull() || f.tag()->isEmpty()) // Check if file is null or tag is empty
    {
        
        if (onMetadataLoaded)
            onMetadataLoaded(file.getFileNameWithoutExtension(), "Unknown Artist"); // Default values
    }
    else
    {
       
        juce::String title = f.tag()->title().isEmpty() ? file.getFileNameWithoutExtension() : juce::String::fromUTF8(f.tag()->title().toCString(true));
        juce::String artist = juce::String::fromUTF8(f.tag()->artist().toCString(true));

        if (onMetadataLoaded)
            onMetadataLoaded(title, artist);
    }
    
}




