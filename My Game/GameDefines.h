/// \file GameDefines.h
/// \brief Game specific defines.

#pragma once

#include "Defines.h"

/// \brief Sprite type.
///
/// Note: NUM_SPRITES must be last.

enum facingDirection {LEFT, RIGHT};

enum animationType {
	RUN_ANIM, JUMP_ANIM, ATTACK_ANIM, DIE_ANIM, WALL_BREAK, DOOR_ANIM,
	NUM_ANIMATIONS
};

/// \brief Sprite type.
///
/// Note: NUM_SPRITES must be last.
enum eSpriteType{
	TILE_SPRITE, TITLE_SCREEN, FAST_SPRITE, FIGHTER_SPRITE, SHIELD_SPRITE, TURRET_SPRITE, 
  BULLET_SPRITE, BULLET2_SPRITE,
  GREENLINE_SPRITE, SMOKE_SPRITE, SPARK_SPRITE, LADDERL_SPRITE, LADDERR_SPRITE,
	EXIT_SPRITE, PAUSE_SCREEN, EXIT_SCREEN, DOOR_SPRITE, ENDPOINT_SPRITE, ELEVATOR_SPRITE, SWORD_SPRITE, ELECTRIC_SPRITE, SWITCH_SPRITE, 
	NO_SPRITE, TELEPORTER_SPRITE, BREAKABLE_SPRITE, TELEPORTSPAWN_SPRITE,
	START_BUTTON, EXIT_BUTTON, POINTER_SPRITE, CONTINUE_BUTTON, MENU_BUTTON,
	SHIELDUP_SPRITE, SHIELDDOWN_SPRITE, KEY_SPRITE, STOP_SPRITE, GUN_SPRITE, RESTART_SPRITE, HP_SPRITE, LOSE_SPRITE, INDICATOR_SPRITE,
  NUM_SPRITES //MUST BE LAST
}; //eSpriteType