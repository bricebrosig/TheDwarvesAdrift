/// \file Renderer.cpp
/// \brief Code for the renderer class CRenderer.

#include "Renderer.h"
#include "ComponentIncludes.h"

#include "DebugPrintf.h"
#include "Abort.h"

CRenderer::CRenderer():
  CSpriteRenderer(Batched2D){
} //constructor

/// Load the specific images needed for this game.
/// This is where eSpriteType values from GameDefines.h get
/// tied to the names of sprite tags in gamesettings.xml. Those
/// sprite tags contain the name of the corresponding image file.
/// If the image tag or the image file are missing, then
/// the game should abort from deeper in the Engine code,
/// leaving you with a dialog box that tells you what
/// went wrong.

void CRenderer::LoadImages(){  
  BeginResourceUpload();

  Load(TILE_SPRITE, "tile"); 
	Load(TITLE_SCREEN, "titleScreen");
  Load(GREENLINE_SPRITE, "greenline");
  Load(BULLET_SPRITE, "bullet");
  Load(BULLET2_SPRITE, "bullet2");
  Load(SMOKE_SPRITE, "smoke");
  Load(SPARK_SPRITE, "spark");
  Load(TURRET_SPRITE, "turret");
  Load(LADDERL_SPRITE, "ladderL");
	Load(LADDERR_SPRITE, "ladderR");
	Load(FIGHTER_SPRITE, "fight");
	Load(FAST_SPRITE, "fast");
	Load(SHIELD_SPRITE, "shield");
	Load(EXIT_SPRITE, "exit");
  Load(PAUSE_SCREEN, "pause");
  Load(EXIT_SCREEN, "exitScreen");
	Load(DOOR_SPRITE, "door");
	Load(ENDPOINT_SPRITE, "endPoint");
	Load(ELEVATOR_SPRITE, "elevator");
  Load(SWORD_SPRITE, "sword");
	Load(ELECTRIC_SPRITE, "electricity");
	Load(SWITCH_SPRITE, "switch");
	Load(NO_SPRITE, "noSprite");
	Load(TELEPORTER_SPRITE, "teleporter");
	Load(BREAKABLE_SPRITE, "breakableWall");
	Load(TELEPORTSPAWN_SPRITE, "teleporterSpawn");
	Load(START_BUTTON, "startButton");
	Load(EXIT_BUTTON, "exitButton");
	Load(CONTINUE_BUTTON, "contButton");
	Load(MENU_BUTTON, "menuButton");
	Load(SHIELDUP_SPRITE, "shieldUp");
	Load(SHIELDDOWN_SPRITE, "shieldDown");
	Load(KEY_SPRITE, "key");
	Load(STOP_SPRITE, "stopPoint");
  Load(GUN_SPRITE, "gunTurret");
	Load(RESTART_SPRITE, "restart");
  Load(HP_SPRITE, "hpHUD");
  Load(LOSE_SPRITE, "lose");
  Load(INDICATOR_SPRITE, "indicator");

 
  EndResourceUpload();
} //LoadImages

/// Draw an axially aligned bounding box using the green line sprite.
/// \param aabb An axially aligned bounding box.

void CRenderer::DrawBoundingBox(const BoundingBox& aabb){
  const Vector3 center = aabb.Center;
  const Vector3 extent = aabb.Extents;
  
  const Vector2 topleft(center.x - extent.x, center.y + extent.y);
  const Vector2 topright(center.x + extent.x, center.y + extent.y);
  const Vector2 bottomleft(center.x - extent.x, center.y - extent.y);
  const Vector2 bottomright(center.x + extent.x, center.y - extent.y);
  
  DrawLine(GREENLINE_SPRITE, topleft, topright);
  DrawLine(GREENLINE_SPRITE, bottomleft, bottomright);
  DrawLine(GREENLINE_SPRITE, topleft, bottomleft);
  DrawLine(GREENLINE_SPRITE, topright, bottomright);
} //DrawBoundingBox


