#pragma once
#include "SimpleAudioEngine.h"
#include "FmConfig.h"

using namespace CocosDenshion;

NS_FM_BEGIN

class AudioEngine
{
private:
	bool m_bEnableBGMusic;
	bool m_bEndbleEffect;
	SimpleAudioEngine* m_audioEngine;

public:
    AudioEngine();
    ~AudioEngine();

    SINGLETON_MODE(AudioEngine);

    void playBackgroundMusic(const char* pszFilePath, bool bLoop);
    void playBackgroundMusic(const char* pszFilePath) {
    	this->playBackgroundMusic(pszFilePath, false);
    }

    void stopBackgroundMusic(bool bReleaseData);
    void stopBackgroundMusic() {
    	this->stopBackgroundMusic(false);
    }

    bool isBackgroundMusicPlaying();

    void pauseBackgroundMusic();

    void resumeBackgroundMusic();


    unsigned int playEffect(const char* pszFilePath, bool bLoop);
    unsigned int playEffect(const char* pszFilePath) {
    	return this->playEffect(pszFilePath, false);
    }

    void stopAllEffects();

	void setEnableBGMusic(bool bEnable);

	void setEnableEffect(bool bEnable);

	void vibrate();
};

NS_FM_END
