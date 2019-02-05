/// \file sound.h
/// \brief Interface for the Audio Player class CAudio.

#pragma once

#include <Audio.h>

#include "Defines.h"
#include "Abort.h"

#include "WindowDesc.h"
#include "Settings.h"
#include "Component.h"

using namespace std;

/// \brief The sound descriptor.
///
/// A sound descriptor contains the information needed
/// to identify an instance of a sound effect.

class CSoundDesc{
  public:
    int m_nEffectIndex = -1; ///< Effect index.
    int m_nInstanceIndex = -1; ///< Instance index.
}; //CSoundDesc

/// \brief The Audio Player. 
///
/// The Audio Player allows you to play multiple 
/// overlapping copies of sounds simultaneously. It reads settings from the
/// XML settings file, including a list of file names to be loaded. 
/// It can set the volume, pitch, and position of each
/// sound instance played. Since it implements 3D sound,
/// make sure your sound files are mono, not stereo. If
/// you hear a horrible screeching sound instead of your
/// nice sound sample, it probably means that your sound
/// file is stereo, not mono. Use a sound editor such as
/// Audacity to convert your sound from stereo to mono.
/// Make sure you call BeginFrame once per frame to prevent
/// multiple copies of a sound from playing at the same time,
/// which only makes one sound but LOUDER.

class CAudio: 
  public CWindowDesc,
  public CSettingsManager,
  public CComponent{
  
  private:
    static bool m_bSingularityViolation; ///< Enforces that this is a singular class.

  protected:
    AudioEngine* m_pAudioEngine; ///< XAudio 2.8 Engine wrapped up in DirectXTK.
    vector<SoundEffect*> m_pSoundEffects; ///< A list of sound effect.
    SoundEffectInstance*** m_pInstance; ///< A list of arrays of sound effect instances.

    bool* m_bPlayed; ///< Whether each sound was started this frame.

    int* m_nInstanceCount; ///< Number of copies of each sound.

    int m_nCount = 0; ///< Number of sounds loaded.

    bool m_bMuted = false; ///< Whether mute is on.
    Vector2 m_vEmitterPos; ///< Position of the emitter (the thing making the sound).
    Vector3 m_vListenerPos; ///< Position of the listener.

    void createInstances(int index, int n, SOUND_EFFECT_INSTANCE_FLAGS flags); ///< Create sound instances.
    int Load(wchar_t *filename); ///< Load sound from file.
    int getNextInstance(int index); ///< Get the next instance that is not playing.

  public:
    CAudio(); ///< Constructor.
    ~CAudio(); ///< Destructor.

    void Load(); ///< Load sounds from list in XML document.
    
    CSoundDesc play(int i, const Vector2& s, float vol=1.0f, float p=0.0f); ///< Play a sound.
    CSoundDesc play(int i, float vol=1.0f); ///< Play a sound.

    CSoundDesc loop(int i); ///< Loop a sound.
    CSoundDesc loop(int i, const Vector2& s); ///< Loop a sound.
    
    CSoundDesc vary(int i, float var=0.1f, float vol=1.0f, float p=0.0f); ///< Play a randomly varied sound.
    CSoundDesc vary(int i, const Vector2& s, float var=0.1f, float vol=1.0f, float p=0.0f); ///< Play a randomly varied sound.

    void BeginFrame(); ///< Start of frame notification.
    void mute(); ///< Mute/unmute the sounds.
    void stop(); ///< Stop all sounds.
    void stop(int i); ///< Stop a sound.
    void stop(const CSoundDesc& d); ///< Stop a sound instance.

    void SetListenerPos(const Vector2& pos); ///< Set the listener position.
    void SetPitch(int i, float f); ///< Set pitch of first instance of sound.
}; //CAudio