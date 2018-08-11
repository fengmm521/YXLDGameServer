#include "AudioEngine.h"

using namespace CocosDenshion; 

NS_FM_BEGIN

AudioEngine::AudioEngine()
{
	m_bEnableBGMusic = true;
	m_bEndbleEffect = true;

	m_audioEngine = SimpleAudioEngine::sharedEngine();
}

AudioEngine::~AudioEngine()
{
}

//////////////////////////////////////////////////////////////////////////
// BackgroundMusic
//////////////////////////////////////////////////////////////////////////

void AudioEngine::playBackgroundMusic(const char* pszFilePath, bool bLoop)
{
    if (!m_bEnableBGMusic || ! pszFilePath)
    {
        return;
    }

    m_audioEngine->playBackgroundMusic(pszFilePath, bLoop);
}

void AudioEngine::stopBackgroundMusic(bool bReleaseData)
{
    m_audioEngine->stopBackgroundMusic(bReleaseData);
}


bool AudioEngine::isBackgroundMusicPlaying()
{
    return m_audioEngine->isBackgroundMusicPlaying();
}

//////////////////////////////////////////////////////////////////////////
// effect function
//////////////////////////////////////////////////////////////////////////

unsigned int AudioEngine::playEffect(const char* pszFilePath, bool bLoop)
{
	if (!m_bEndbleEffect)
	{
		return 0;
	}

    return m_audioEngine->playEffect(pszFilePath, bLoop);
}


void AudioEngine::stopAllEffects()
{
   m_audioEngine->stopAllEffects();
}


void AudioEngine::setEnableBGMusic( bool bEnable )
{
	if (bEnable == false)
	{
		stopBackgroundMusic();
	}
	m_bEnableBGMusic = bEnable;
}


void AudioEngine::setEnableEffect( bool bEnable )
{
	if (bEnable == false)
	{
		stopAllEffects();
	}
	
	m_bEndbleEffect = bEnable;
}

void AudioEngine::pauseBackgroundMusic()
{
	if (m_bEnableBGMusic == false)
	{
		return;
	}
	m_audioEngine->pauseBackgroundMusic();
}

void AudioEngine::resumeBackgroundMusic()
{
	if (m_bEnableBGMusic == false)
	{
		return;
	}
	m_audioEngine->resumeBackgroundMusic();
}

void AudioEngine::vibrate()
{
	m_audioEngine->vibrate();
}




NS_FM_END
