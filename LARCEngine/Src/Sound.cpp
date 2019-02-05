/// \file sound.cpp
/// \brief Code for the Audio Player class CAudio.

#include <stdio.h>

#include "sound.h"
#include "ComponentIncludes.h"
#include "Helpers.h"

static const float SCALE = 500.0f; ///< Scale from Render World to Audio World.
static const float DEPTH = 100.0f; ///< Default Z depth for sounds.

bool CAudio::m_bSingularityViolation = false;

/// Set member variables to sensible values and initialize
/// the XAudio Engine using DirectXTK. If the compiler is
/// in debug mode, then set the XAudio engine in debug mode
/// too.

CAudio::CAudio(): 
  m_vEmitterPos(m_vWinCenter),
  m_vListenerPos(Vector3(m_vEmitterPos.x, m_vEmitterPos.y, 0.0f))
{
  if(m_bSingularityViolation)
    ABORT("Do not instantiate more than one audio player.");

  HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED); 

  if(SUCCEEDED(hr)){
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
    #ifdef _DEBUG
      eflags = eflags | AudioEngine_Debug;
    #endif
    m_pAudioEngine = new AudioEngine(eflags);
  } //if

  else m_pAudioEngine = nullptr;
} //constructor

/// Reclaim all dynamic memory and shut down the audio engine.

CAudio::~CAudio(){ 
  for(int i=0; i<m_nCount; i++){
    for(int j=0; j<m_nInstanceCount[i]; j++)
      delete m_pInstance[i][j];
    delete [] m_pInstance[i];
  } //for

  delete [] m_nInstanceCount;
  delete [] m_pInstance;
  delete [] m_bPlayed;

  for(int i=0; i<(int)m_pSoundEffects.size(); i++)
    delete m_pSoundEffects[i];

  delete m_pAudioEngine;
} //destructor

/// Load a sound from a file.
/// \param filename name of file to be loaded
/// \return Size of sound effect loaded

int CAudio::Load(wchar_t *filename){
  m_vEmitterPos = m_vWinCenter;
  m_pSoundEffects.push_back(new SoundEffect(m_pAudioEngine, filename));
  return (int)m_pSoundEffects.size() - 1;
} //load

/// Create sound instances.
/// \param index Index of sound.
/// \param n Number of instances of sound wanted.
/// \param flags Flags.

void CAudio::createInstances(int index, int n, SOUND_EFFECT_INSTANCE_FLAGS flags){
  m_nInstanceCount[index] = n;
  m_pInstance[index] = new SoundEffectInstance*[n];

  for(int i=0; i<n; i++)
    m_pInstance[index][i] = m_pSoundEffects[index]->CreateInstance(flags).release();
} //createInstances

/// Get the index into the instance array of the next unplayed instance of a sound.
/// \param index Index of sound in the sound array.
/// \return Index of  next unplayed instance in its instance array.

int CAudio::getNextInstance(int index){  
  int instance = 0; //current instance

  //get status of first instance
  SoundState state = m_pInstance[index][instance]->GetState();

  //find next unplayed instance, if any
  while(instance < m_nInstanceCount[index] && (state == PLAYING)) //while current copy in use
    if(++instance < m_nInstanceCount[index]) //go to next copy
      state = m_pInstance[index][instance]->GetState();

  return instance;
} //getNextInstance

/// Play a sound effect in stereo.
/// \param i Index of sound to be played.
/// \param s Position of emitter in 2D.
/// \param vol Volume (defaults to 1.0f).
/// \param p Pitch (defaults to zero).
/// \return Sound descriptor for playing sound.

CSoundDesc CAudio::play(int i, const Vector2& s, float vol, float p){
  CSoundDesc desc;

  if(i < 0 || i >= m_nCount || m_bMuted || m_bPlayed[i]) //if bad index, or muted, or already started
    return desc; //bail out

  desc.m_nEffectIndex = i;
  const int instance = getNextInstance(i); //instance of sound

  if(instance >= 0 && instance < m_nInstanceCount[i]){ //if unused copy found
    desc.m_nInstanceIndex = instance;
    m_pInstance[i][instance]->Play(); //play it
    m_bPlayed[i] = true;

    Vector2 v2 = m_vEmitterPos;
    Vector2 v3 = Vector3(v2.x, v2.y, 0.0f)/SCALE;

    AudioListener listener;
    listener.SetPosition(v3);
    
    //set position
    AudioEmitter emitter;
    emitter.SetPosition(Vector3(s.x, s.y, DEPTH)/SCALE);
    m_pInstance[i][instance]->Apply3D(listener, emitter);
    
    //set volume
    const float v = max(0.0f, min(vol, 1.0f)); //volume
    m_pInstance[i][instance]->SetVolume(v);

    m_pInstance[i][instance]->SetPitch(p);
  } //if

  return desc;
} //play

/// Play a sound effect in mono.
/// \param i Index of sound to be played.
/// \param vol Volume (defaults to 1.0f).
/// \return Sound descriptor for playing sound.

CSoundDesc CAudio::play(int i, float vol){
  return play(i, m_vEmitterPos, vol);
} //play

/// Loop a sound effect in stereo.
/// \param i Index of sound to be looped.
/// \param s Position of emitter in 2D.
/// \return Sound descriptor for playing sound.

CSoundDesc CAudio::loop(int i, const Vector2& s){
  CSoundDesc desc;

  if(i < 0 || i >= m_nCount || m_bMuted || m_bPlayed[i]) //if bad index, or muted, or already started
    return desc; //bail out
  
  desc.m_nEffectIndex = i;
  const int instance = getNextInstance(i); //instance of sound

  if(instance >= 0 && instance < m_nInstanceCount[i]){ //if unused copy found
    desc.m_nInstanceIndex = instance;
    m_pInstance[i][instance]->Play(true); //play looped
    m_bPlayed[i] = true;

    Vector2 v2 = m_vEmitterPos;
    Vector2 v3 = Vector3(v2.x, v2.y, 0.0f)/SCALE;

    AudioListener listener;
    listener.SetPosition(v3);
    
    //set position
    AudioEmitter emitter;
    emitter.SetPosition(Vector3(s.x, s.y, DEPTH)/SCALE);
    m_pInstance[i][instance]->Apply3D(listener, emitter);
  } //if

  return desc;
} //loop

/// Loop a sound effect in mono.
/// \param i Index of sound to be looped.
/// \return Sound descriptor for playing sound.

CSoundDesc CAudio::loop(int i){
  return loop(i, m_vEmitterPos);
} //loop

/// Play a sound effect in stereo, but slightly differently each 
/// time it is played. The pitch is varied up and down by a random
/// amount scaled by the coefficient of variability that is
/// provided as a parameter.
/// \param i Index of sound to be played.
/// \param s Position of emitter in 2D.
/// \param var Coefficient of variability (defaults to 0.25).
/// \param vol Volume (defaults to 1.0).
/// \param p Pitch (defaults to 0).
/// \return Sound descriptor for playing sound.

CSoundDesc CAudio::vary(int i, const Vector2& s, float var, float vol, float p){
  p += var*(2.0f*m_pRandom->randf() - 1.0f); //vary pitch
  return play(i, s, vol, p); //play at new volume and pitch
} //vary

/// Play a sound effect in mono, but slightly differently each 
/// time it is played. The pitch is varied up and down by a random
/// amount scaled by the coefficient of variability that is
/// provided as a parameter.
/// \param i Index of sound to be played.
/// \param var Coefficient of variability (defaults to 0.25).
/// \param vol Volume (defaults to 1.0).
/// \param p Pitch (defaults to 0).
/// \return Sound descriptor for playing sound.

CSoundDesc CAudio::vary(int i, float var, float vol, float p){
  p += var*(2.0f*m_pRandom->randf() - 1.0f); //vary pitch
  return play(i, m_vEmitterPos, vol, p); //play at new volume and pitch
} //vary

/// Stop an instance of a sound effects.
/// \param d Descriptor for a sound instance.

void CAudio::stop(const CSoundDesc& d){
  const int i = d.m_nEffectIndex; //shorthand for effect index
  if(i < 0 || i >= m_nCount)return; //bail if not in range

  const int j = d.m_nInstanceIndex; //shorthand for instance index.
  if(j < 0 || j >= m_nInstanceCount[i])return; //bail if not in range

  m_pInstance[i][j]->Stop(); //stop it
} //stop

/// Stop all instances of a sound effect.
/// \param i Index of sound to be stopped.

void CAudio::stop(int i){
  if(i < 0 || i >= m_nCount)return;

  for(int j=0; j<m_nInstanceCount[i]; j++) //for each instance of that sound
    m_pInstance[i][j]->Stop();
} //stop

/// Stop all instances of all sound effects.

void CAudio::stop(){
  for(int i=0; i<m_nCount; i++) //for each sound
    stop(i); //stop it
} //stop

/// Get ready for new animation frame by flagging all sounds 
/// as unplayed.

void CAudio::BeginFrame(){
  for(int i=0; i<m_nCount; i++)
    m_bPlayed[i] = false;
} //BeginFrame

/// Load the sound files from the file list in g_xmlSettings.
/// Processes sound file names in \<sound\> tags within a \<sounds\>\</sounds\> pair.
/// Starts by counting the number of sound files needed, and creating arrays of
/// the right size.

void CAudio::Load(){
  if(m_pXmlSettings == nullptr)
    ABORT("Cannot access gamesettings.xml.");

  //find <sounds> tag 
  XMLElement* snd =
    m_pXmlSettings->FirstChildElement("sounds"); //<sounds> tag
  
  if(snd == nullptr)
    ABORT("Cannot find <sounds> tag in gamesettings.xml");

  string path(snd->Attribute("path")); //get path

  //count number of sounds in list

  int n = 0; //counter
  for(auto s=snd->FirstChildElement("sound"); s; s=s->NextSiblingElement("sound"))
    ++n;

  //create arrays and initialize

  m_pInstance = new SoundEffectInstance**[n];
  m_nInstanceCount = new int[n];
  m_bPlayed = new bool[n];

  for(int i=0; i<n; i++){
    m_pInstance[i] = nullptr;
    m_nInstanceCount[i] = 0;
    m_bPlayed[i] = false;
  } //for

  //load sounds from sound list

  for(auto s=snd->FirstChildElement("sound"); s; s=s->NextSiblingElement("sound")){
    const int n = max(1, s->IntAttribute("instances")); //get number of instances
   
    const string filename = path + "\\" + s->Attribute("file");

    wchar_t* wfilename = nullptr; //wide file name
    MakeWideFileName(filename.c_str(), wfilename); //convert the former to the latter

    createInstances(Load(wfilename), n, 
      SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters);

    m_nCount++;
    delete [] wfilename;
  } //for
} //Load

/// If the sound is muted, unmute it. If not, mute it.
/// This means stopping sounds that are currently playing and
/// preventing future sounds from getting started.

void CAudio::mute(){
  m_bMuted = !m_bMuted; //toggle
  if(m_bMuted)stop(); //stop all sounds  
} //mute

/// Move the listener to a new position.
/// \param pos New position of listener.

void CAudio::SetListenerPos(const Vector2& pos){
  m_vEmitterPos = pos;
} //SetListenerPos

/// Set the pitch of the first instance of a sound.
/// \param i Index of sound to be played.
/// \param f New pitch.

void CAudio::SetPitch(int i, float f){
  m_pInstance[i][0]->SetPitch(f);
} //SetPitch