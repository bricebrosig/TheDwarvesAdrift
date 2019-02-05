/// \file SndList.h
/// \brief Enumerated type for sounds.

#pragma once

#include "Sound.h"

/// \brief Game sound enumerated type. 
///
/// These are the sounds used in actual gameplay. 
/// The sounds must be listed here in the same order that
/// they are in the sound settings XML file.

enum eSoundType{ 
  GUN_SOUND, LASER_SOUND, RICOCHET_SOUND, OW_SOUND, CLANG_SOUND, TITLE_SOUND, DEATH_SOUND, ENEMY_OW
}; //eSoundType