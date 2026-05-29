#include "AudioSystem.hpp"
#include "Log/Logger.hpp"
#include "raylib.h"
#include <algorithm>
#include <optional>

void AudioSystem::Update(const float deltaT)
{
	std::vector<rAudioBuffer*> toRemove;

	for (auto& [buffer, pair] : m_sounds)
	{
		if (!IsSoundValid(pair.first) || !IsSoundPlaying(pair.first))
		{
			toRemove.emplace_back(buffer);
			continue;
		}

		if (m_sfxVolumeChange)
		{
			::SetSoundVolume(pair.first, pair.second * m_sfxVolume);
		}
	}

	m_sfxVolumeChange = false;

	for (rAudioBuffer* buffer : toRemove)
	{
		m_sounds.erase(buffer);
	}

	Logger::Write("State: ", static_cast<u32>(m_musicState));

	switch (m_musicState)
	{
	case MusicState::STOPPED:
	{
	}
	break;

	case MusicState::PLAYING:
	{
		if (!IsMusicValid(m_currentMusic.music))
		{
			Logger::Write<LogLevel::ERROR>("Music playback error");
			StopMusic();
			break;
		}

		if (m_fadeTargetS &&
			GetMusicTimeLength(m_currentMusic.music) - GetMusicTimePlayed(m_currentMusic.music) <= m_fadeTargetS)
		{
			m_musicState = MusicState::FADING_OUT;
		}

		if (GetMusicTimeLength(m_currentMusic.music) == GetMusicTimePlayed(m_currentMusic.music))
		{
			if (m_currentMusic.loop)
			{
				m_musicState = MusicState::FADING_IN;
			}

			else
			{
				StopMusic();
			}
		}

		UpdateMusicStream(m_currentMusic.music);
	}
	break;

	case MusicState::PAUSED:
	{
	}
	break;

	case MusicState::FADING_IN:
	{
		UpdateFade(true);

		UpdateMusicStream(m_currentMusic.music);
	}
	break;

	case MusicState::FADING_OUT:
	{
		UpdateFade(false);

		UpdateMusicStream(m_currentMusic.music);
	}
	break;

	case MusicState::QUEUED:
	{
		MusicSwap();
	}
	break;
	}
}

void AudioSystem::PlaySound(const Sound& sound, float volume)
{
	if (!IsSoundValid(sound))
	{
		return;
	}

	volume = std::clamp(volume, 0.0f, 1.0f);

	::PlaySound(sound);
	::SetSoundVolume(sound, volume * m_sfxVolume);

	m_sounds.emplace(sound.stream.buffer, std::make_pair(sound, volume));
}

void AudioSystem::StopSound(const Sound& sound)
{
	if (!IsSoundValid(sound))
	{
		return;
	}

	m_sounds.erase(sound.stream.buffer);

	if (!IsSoundPlaying(sound))
	{
		return;
	}

	::StopSound(sound);
}

void AudioSystem::SetSoundVolume(const Sound& sound, float volume)
{
	if (!IsSoundValid(sound) || !IsSoundPlaying(sound))
	{
		return;
	}

	volume = std::clamp(volume, 0.0f, 1.0f);

	::SetSoundVolume(sound, volume * m_sfxVolume);

	m_sounds[sound.stream.buffer].second = volume;
}

void AudioSystem::SetSoundPitch(const Sound& sound, const float pitch)
{
	if (!IsSoundValid(sound) || !IsSoundPlaying(sound))
	{
		return;
	}

	::SetSoundPitch(sound, pitch);
}

void AudioSystem::SetSoundPan(const Sound& sound, float pan)
{
	if (!IsSoundValid(sound) || !IsSoundPlaying(sound))
	{
		return;
	}

	pan = std::clamp(pan, -1.0f, 1.0f);

	::SetSoundPan(sound, pan);
}

void AudioSystem::PlayMusic(const Music& music, float volume, const bool loop, const float crossfadeS) // NOLINT
{
	if (!IsMusicValid(music))
	{
		return;
	}

	volume = std::clamp(volume, 0.0f, 1.0f);
	MusicData newData = {music, volume, crossfadeS, loop};

	if (m_musicState == MusicState::STOPPED)
	{
		m_currentMusic = newData;
		m_currentMusic.music.looping = m_currentMusic.loop;
		PlayMusicStream(m_currentMusic.music);
		SetMusicVolumeInternal(volume);

		m_musicState = MusicState::PLAYING;

		if (crossfadeS > 0.0)
		{
			m_fadeTargetS = m_currentMusic.fadeTargetS;
			m_musicState = MusicState::FADING_IN;
			SetMusicVolumeInternal(0);
		}

		return;
	}

	if (m_musicState == MusicState::PLAYING)
	{
		m_nextMusic = newData;
		m_fadeTargetS = m_nextMusic->fadeTargetS;
		m_musicState = MusicState::FADING_OUT;
	}

	else
	{
		StopMusic();
		PlayMusic(music, volume, loop);
	}
}

void AudioSystem::StopMusic(const float fadeOutDuration)
{
	if (m_musicState == MusicState::STOPPED)
	{
		return;
	}

	if (fadeOutDuration > 0.0f && m_musicState != MusicState::FADING_OUT)
	{
		m_fadeTargetS = fadeOutDuration;
		m_musicState = MusicState::FADING_OUT;
		m_nextMusic.reset();

		return;
	}

	if (IsMusicValid(m_currentMusic.music))
	{
		StopMusicStream(m_currentMusic.music);
	}

	m_currentMusic = MusicData{};
	m_nextMusic.reset();
	m_musicState = MusicState::STOPPED;
	m_fadeTargetS = 0;
}

bool AudioSystem::IsMusicPlaying()
{
	if (m_musicState == MusicState::PLAYING || m_musicState == MusicState::FADING_IN)
	{
		if (!IsMusicValid(m_currentMusic.music))
		{
			Logger::Write<LogLevel::ERROR>("Music playback error");
			StopMusic();

			return false;
		}

		return IsMusicStreamPlaying(m_currentMusic.music);
	}

	return false;
}

void AudioSystem::PauseMusic()
{
	if (m_musicState == MusicState::PLAYING || m_musicState == MusicState::FADING_IN)
	{
		if (!IsMusicValid(m_currentMusic.music))
		{
			Logger::Write<LogLevel::ERROR>("Music playback error");
			StopMusic();

			return;
		}

		PauseMusicStream(m_currentMusic.music);
		m_musicState = MusicState::PAUSED;
	}
}

void AudioSystem::ResumeMusic()
{
	if (m_musicState == MusicState::PAUSED)
	{
		if (!IsMusicValid(m_currentMusic.music))
		{
			Logger::Write<LogLevel::ERROR>("Music playback error");
			StopMusic();

			return;
		}

		ResumeMusicStream(m_currentMusic.music);
		m_musicState = MusicState::PLAYING;
	}
}

std::optional<float> AudioSystem::GetMusicLengthS()
{
	if (m_musicState == MusicState::STOPPED)
	{
		return std::nullopt;
	}

	if (!IsMusicValid(m_currentMusic.music))
	{
		Logger::Write<LogLevel::ERROR>("Music playback error");
		StopMusic();

		return std::nullopt;
	}

	return GetMusicTimeLength(m_currentMusic.music);
}

std::optional<float> AudioSystem::GetMusicPlayedS()
{
	if (m_musicState == MusicState::STOPPED)
	{
		return std::nullopt;
	}

	if (!IsMusicValid(m_currentMusic.music))
	{
		Logger::Write<LogLevel::ERROR>("Music playback error");
		StopMusic();

		return std::nullopt;
	}

	return GetMusicTimePlayed(m_currentMusic.music);
}

void AudioSystem::SeekMusic(const float timeS)
{
	if (m_musicState == MusicState::STOPPED)
	{
		Logger::Write("Stopped");
		return;
	}

	if (!IsMusicValid(m_currentMusic.music))
	{
		Logger::Write<LogLevel::ERROR>("Music playback error");
		StopMusic();

		return;
	}
	if (timeS > GetMusicTimeLength(m_currentMusic.music))
	{
		Logger::Write<LogLevel::WARN>("Music seek past end");
		return;
	}

	Logger::Write("Seeked");

	SeekMusicStream(m_currentMusic.music, timeS);
}

void AudioSystem::SetMusicVolume(float volume)
{
	volume = std::clamp(volume, 0.0f, 1.0f);

	m_musicVolume = volume;

	SetMusicVolumeInternal(volume);
}

void AudioSystem::SetMusicPitch(const float pitch)
{
	if (m_musicState == MusicState::STOPPED)
	{
		return;
	}

	if (!IsMusicValid(m_currentMusic.music))
	{
		return;
	}

	::SetMusicPitch(m_currentMusic.music, pitch);
}

void AudioSystem::SetMusicPan(float pan)
{
	pan = std::clamp(pan, -1.0f, 1.0f);
	if (m_musicState == MusicState::STOPPED)
	{
		return;
	}

	if (!IsMusicValid(m_currentMusic.music))
	{
		Logger::Write<LogLevel::ERROR>("Music playback error");
		StopMusic();

		return;
	}

	::SetMusicPan(m_currentMusic.music, pan);
}

void AudioSystem::SetMusicLoop(const bool loop)
{
	if (m_musicState == MusicState::STOPPED)
	{
		return;
	}

	if (!IsMusicValid(m_currentMusic.music))
	{
		Logger::Write<LogLevel::ERROR>("Music playback error");
		StopMusic();

		return;
	}

	m_currentMusic.loop = loop;
	m_currentMusic.music.looping = true;
}

void AudioSystem::SetMasterVolume(float volume)
{
	volume = std::clamp(volume, 0.0f, 1.0f);

	::SetMasterVolume(volume);
}

void AudioSystem::SetSFXVolume(float volume)
{
	volume = std::clamp(volume, 0.0f, 1.0f);

	m_sfxVolume = volume;
	m_sfxVolumeChange = true;
}

void AudioSystem::UpdateFade(const bool fadingIn)
{
	float fadeCompletion;

	if (fadingIn && GetMusicTimePlayed(m_currentMusic.music) <= m_fadeTargetS)
	{
		fadeCompletion = (GetMusicTimePlayed(m_currentMusic.music)) / m_fadeTargetS;
	}

	else if (!fadingIn &&
			 GetMusicTimeLength(m_currentMusic.music) - GetMusicTimePlayed(m_currentMusic.music) <= m_fadeTargetS)
	{
		fadeCompletion =
		(GetMusicTimeLength(m_currentMusic.music) - GetMusicTimePlayed(m_currentMusic.music)) / m_fadeTargetS;
	}

	float volume = std::clamp(fadeCompletion, 0.001f, 0.999f);
	SetMusicVolumeInternal(volume);

	Logger::Write(volume);

	if (fadingIn && volume >= 0.99)
	{
		m_musicState = MusicState::PLAYING;
	}

	else if (!fadingIn && volume <= 0.01)
	{
		if (m_nextMusic)
		{
			m_musicState = MusicState::QUEUED;
		}

		else if (m_currentMusic.loop)
		{
			m_musicState = MusicState::FADING_IN;
		}

		else
		{
			StopMusic();
		}
	}
}

void AudioSystem::MusicSwap()
{
	StopMusic();

	m_currentMusic = *m_nextMusic;
	m_nextMusic.reset();

	m_fadeTargetS = m_currentMusic.fadeTargetS;
	m_currentMusic.music.looping = m_currentMusic.loop;

	PlayMusicStream(m_currentMusic.music);
	SetMusicVolumeInternal(0.0);

	m_musicState = MusicState::FADING_IN;
}

void AudioSystem::SetMusicVolumeInternal(float volume)
{
	if (!IsMusicValid(m_currentMusic.music))
	{
		return;
	}

	m_currentMusic.volume = std::clamp(volume, 0.0f, 1.0f);

	::SetMusicVolume(m_currentMusic.music, m_currentMusic.volume * m_musicVolume);
}