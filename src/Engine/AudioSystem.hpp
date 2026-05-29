#pragma once

#include "SystemManager.hpp"
#include "Types.hpp"
#include "raylib.h"
#include <optional>
#include <unordered_map>

class AudioSystem : public System
{
public:

	void Update(const float deltaT) override;

	void PlaySound(const Sound& sound, float volume = 1);
	void StopSound(const Sound& sound);
	void SetSoundVolume(const Sound& sound, float volume);
	static void SetSoundPitch(const Sound& sound, const float pitch);
	static void SetSoundPan(const Sound& sound, float pan);

	void PlayMusic(const Music& music, float volume = 1, const bool loop = false, const float crossfadeS = 0);
	void StopMusic(const float fadeOutDuration = 0);
	bool IsMusicPlaying();
	void PauseMusic();
	void ResumeMusic();
	std::optional<float> GetMusicLengthS();
	std::optional<float> GetMusicPlayedS();
	void SeekMusic(const float timeS);
	void SetMusicVolume(float volume);
	void SetMusicPitch(const float pitch);
	void SetMusicPan(float pan);
	void SetMusicLoop(const bool loop);

	static void SetMasterVolume(float volume);
	void SetSFXVolume(float volume);

private:

	enum class MusicState : u8
	{
		STOPPED,
		PLAYING,
		PAUSED,
		FADING_IN,
		FADING_OUT,
		QUEUED
	};

	struct MusicData
	{
		Music music = {};
		float volume = 1;
		float fadeTargetS = 0;
		bool loop = false;
	};

	void UpdateFade(const bool fadingIn);
	void MusicSwap();
	void SetMusicVolumeInternal(float volume);

	float m_sfxVolume = 1;
	float m_musicVolume = 1;

	MusicState m_musicState = MusicState::STOPPED;
	MusicData m_currentMusic;
	std::optional<MusicData> m_nextMusic;
	float m_fadeTargetS = 0;

	bool m_sfxVolumeChange = false;
	std::unordered_map<rAudioBuffer*, std::pair<Sound, float>> m_sounds;
};