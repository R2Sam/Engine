#pragma once

#include "Engine/SystemManager.hpp"
#include "Types.hpp"
#include "raylib.h"
#include <optional>
#include <unordered_map>

/**
 * @file AudioSystem.hpp
 * @brief Sound effect and music playback management.
 */

/**
 * @class AudioSystem
 * @brief Handles playback of sounds and music with volume control and crossfading.
 */
class AudioSystem : public System
{
public:

	/**
	 * @brief Updates music stream and removes finished sounds.
	 * @param deltaT Unused.
	 */
	void Update(const float deltaT) override;

	// --- Sound effects -------------------------------------------------

	/**
	 * @brief Plays a sound effect.
	 * @param sound The sound asset.
	 * @param volume Playback volume (0.0–1.0), scaled by global SFX volume.
	 */
	void PlaySound(const Sound& sound, float volume = 1);

	/**
	 * @brief Stops a sound effect immediately.
	 * @param sound The sound asset.
	 */
	void StopSound(const Sound& sound);

	/**
	 * @brief Changes volume of a currently playing sound.
	 * @param sound The sound asset.
	 * @param volume New volume (0.0–1.0).
	 */
	void SetSoundVolume(const Sound& sound, float volume);

	/**
	 * @brief Changes pitch of a playing sound.
	 * @param sound The sound asset.
	 * @param pitch Pitch multiplier (1.0 = original).
	 */
	static void SetSoundPitch(const Sound& sound, const float pitch);

	/**
	 * @brief Changes stereo pan of a playing sound.
	 * @param sound The sound asset.
	 * @param pan -1.0 = left, 0.0 = center, 1.0 = right.
	 */
	static void SetSoundPan(const Sound& sound, const float pan);

	// --- Music ---------------------------------------------------------

	/**
	 * @brief Starts playing background music.
	 * @param music The music asset.
	 * @param volume Volume (0.0–1.0).
	 * @param loop Whether to repeat the track.
	 * @param crossfadeS Crossfade duration in seconds (if another music is playing).
	 */
	void PlayMusic(const Music& music, float volume = 1, const bool loop = false, const float crossfadeS = 0);

	/**
	 * @brief Stops the current music, optionally with a fade out.
	 * @param fadeOutDuration Seconds to fade out before stopping (0 = immediate).
	 */
	void StopMusic(const float fadeOutDuration = 0);

	/**
	 * @brief Checks if music is currently playing or fading in.
	 * @return True if music is active.
	 */
	bool IsMusicPlaying();

	/**
	 * @brief Pauses the current music stream.
	 */
	void PauseMusic();

	/**
	 * @brief Resumes a paused music stream.
	 */
	void ResumeMusic();

	/**
	 * @brief Gets the total length of the current music.
	 * @return Length in seconds, or std::nullopt if no music is loaded.
	 */
	std::optional<float> GetMusicLengthS();

	/**
	 * @brief Gets the current playback position.
	 * @return Position in seconds, or std::nullopt if no music is loaded.
	 */
	std::optional<float> GetMusicPlayedS();

	/**
	 * @brief Seeks to a specific time in the current music.
	 * @param timeS Target time in seconds.
	 */
	void SeekMusic(const float timeS);

	/**
	 * @brief Sets the music volume (without affecting sound effects).
	 * @param volume 0.0–1.0.
	 */
	void SetMusicVolume(float volume);

	/**
	 * @brief Sets the music pitch.
	 * @param pitch Multiplier (1.0 = original).
	 */
	void SetMusicPitch(const float pitch);

	/**
	 * @brief Sets the music stereo pan.
	 * @param pan -1.0 left … 1.0 right.
	 */
	void SetMusicPan(float pan);

	/**
	 * @brief Enables or disables looping for the current music.
	 * @param loop True to loop.
	 */
	void SetMusicLoop(const bool loop);

	// --- Global volume ------------------------------------------------

	/**
	 * @brief Sets the master volume for the entire audio device.
	 * @param volume 0.0–1.0.
	 */
	static void SetMasterVolume(float volume);

	/**
	 * @brief Sets the volume multiplier for all sound effects.
	 * @param volume 0.0–1.0.
	 */
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