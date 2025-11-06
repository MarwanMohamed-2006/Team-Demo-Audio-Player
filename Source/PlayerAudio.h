#pragma once // PlayerAudio.h
#include <JuceHeader.h>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <vector>
#include <functional>

class PlayerAudio
{
public:
	PlayerAudio();
	~PlayerAudio();
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
	void releaseResources();

	bool loadFile(const juce::File& file);
	void start();
	void stop();
	void setGain(float gain);
	void setPosition(double pos);
	double getPosition() const;
	double getLength() const;
	void mute();
	void unmute();
	void setLooping(bool shouldLoop);
	bool isLooping() const;
	void setSpeed(double speed);
	void set_start(double startTime);
	void set_end(double endTime);
	void enableABLoop(bool enable);
	bool isABLoopEnabled() const;
	double getLoopStart() const;
	double getLoopEnd() const;
	void clearABLoop();
	int getCurrentIndex() const;

	juce::String getWildcardFiles() const;

	std::function<void(const juce::String& title,
		const juce::String& artist)> onMetadataLoaded;

	void extractMetadata(const juce::File& file);

	std::vector<juce::File>& getPlaylist() { return playlist; }
	void addFilesToPlaylist(const juce::Array<juce::File>& files);
	void loadAndPlayFile(int index);

	double startpercentage() const



	{
		double length = getLength();
		return length > 0.0 ? loopStartTime / length : 0.0;
	}

	double endpercentage() const
	{
		double length = getLength();
		return length > 0.0 ? loopEndTime / length : 0.0;
	}
private:
	juce::AudioFormatManager formatManager;
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
	juce::AudioTransportSource transportSource;
	juce::ResamplingAudioSource resampleSource{ &transportSource, false, 2 };
	bool looping = false;
	bool abLoopEnabled = false;
	double loopStartTime = 0.0;
	double loopEndTime = 0.0;

	double durationINsecond = 0.0;
	juce::String artist;
	juce::String title;

	std::vector<juce::File> playlist;
	int currentFileIndex = -1;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};