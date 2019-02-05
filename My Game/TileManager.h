/// \file TileManager.h
/// \brief Interface for the tile manager CTileManager.

#pragma once

#include <vector>

using namespace std;

#include "Common.h"
#include "Sprite.h"
#include "GameDefines.h"
/// \brief The tile manager.
///
/// The tile manager is responsible for the
/// tile-based background.

// objects that the img parser will detect based on color
// colors:
/*
  0 0 0 - wall
  0 0 155 -ladder-right
  0 0 255 - ladder left
  255 0 0 - exit
  0 155 255 - fast
  1 155 255 - fighter
  2 155 255 - shield
  0 255 0 - doors (r value is the key)
	0 250 250 - keys (r value is the key)
  0 0 100 - moving platform (level 2)
  0 100 100 - end points for moving platform
  100 0 100 - stop points for moving platform
	0 200 200 - elecricity (r val is the index)
	0 200 0 - switch (r val is the index)
	150 150 0 - teleporter
	125 125 125 - teleporter spawn point
	50 50 0 - breakable wall
	75 75 75 - Enemy Spawn
*/
enum class objColor { NONE, WALL, LADDER_R, LADDER_L, EXIT,
  FAST, FIGHTER, SHIELD, DOOR, ELEVATOR, ENDPOINT, STOPPOINT, 
  ELECTRICITY, SWITCH_ELECTRIC, TELEPORTER, BREAKABLEWALL , 
  TELEPORTERSPAWN, KEY, ENEMY_SPAWNS, GUN_SPAWN
};

class CTileManager: public CCommon{
  private:
    int m_nWidth = 0; ///< Number of tiles wide.
    int m_nHeight = 0; ///< Number of tiles high.

    float m_fTileSize; ///< Tile width and height.
    Vector2 m_vTileRadius; ///< Tile radius.
		Vector2 m_vExitLocation;//where the exit is for each map

    int    m_nLevel = 0;
    char** m_chMap = nullptr; ///< The level map.

    vector<BoundingBox> m_vecWalls; ///< AABBs for the walls.

    vector<Vector3> m_vecLadders; ///< Positions of ladders.
		vector<Vector3> m_vecDoors; //positions of the doors
		vector<Vector2> m_vecEndPoints; //positions of the endpoints
		vector<Vector2> m_vecStopPoints; //positions of the stops
		vector<Vector2> m_vecElevators; //positions of the elevators
		vector<Vector2> m_vecBreakableWalls;
		vector<Vector3> m_vecTraps;
		vector<Vector3> m_vecSwitches;
		vector<Vector3> m_vecTeleporters;
		vector<Vector2> m_vecTeleportSpawns;
		vector<Vector3> m_vecKeys;
		vector<Vector2> m_vecEnemies;
    vector<Vector2> m_vecGuns;

		Vector2 m_vFighterLocation;
		Vector2 m_vFastLocation;
		Vector2 m_vShieldLocation;

    objColor CTileManager::getTileColor(unsigned char* buffer, const int &i); //used to simplify the map parsing

		//moved this over to public so that the object manager can call it
    //void MakeBoundingBoxes(char* filename); ///< Make bounding boxes for walls.

  public:
    CTileManager(size_t tilesize); ///< Constructor.
    ~CTileManager(); ///< Destructor.

    void LoadMap(char* filename); ///< Load a map.
		void LoadMapFromImageFile(char* filename);//got this from lab 4
		void MakeBoundingBoxes(); ///< Make bounding boxes for walls
    void Draw(eSpriteType t); ///< Draw the map with a given tile.
    void DrawBoundingBoxes(eSpriteType t); ///< Draw the bounding boxes.
		void clear();

    bool Visible(const Vector2& v0, const Vector2& v1, float radius); ///< Check visibility.

    vector<Vector3> GetLadders() { return m_vecLadders; }; ///< Get Ladders
		vector<Vector3> getDoors() { return m_vecDoors; }// get the vector of dooors
		vector<Vector2> getEndPoints() { return m_vecEndPoints; }//ya know
		vector<Vector2> getElevators() { return m_vecElevators; }//ya know
		vector<Vector2> getStopPoints() { return m_vecStopPoints; }
		vector<Vector3> getTraps() { return m_vecTraps; }
		vector<Vector3> getSwitches() { return m_vecSwitches; }
		vector<Vector3> getTeleporters() { return m_vecTeleporters; }
		vector<Vector2> getBreakableWalls() { return m_vecBreakableWalls; }
		vector<Vector2> getTeleporterSpawns() { return m_vecTeleportSpawns; }
		vector<Vector2> getEnemies() { return m_vecEnemies; }
		vector<Vector3> getKeys() { return m_vecKeys; }
    vector<Vector2> getGuns() { return m_vecGuns; }
		Vector2 getExitLocation() { return m_vExitLocation; };//"this must be my exit" --- Oso Oso
		Vector2 getFastLocation() { return m_vFastLocation; }
		Vector2 getFighterLocation() { return m_vFighterLocation; }
		Vector2 getShieldLocation() { return m_vShieldLocation; }

    void setLevel(int l) { m_nLevel = l; };

    template<class t> bool CollideWithWall(const t& s, vector<BoundingBox>& walls); ///< Check object collision with a wall.
}; //CTileManager

/// \brief Template for bounding shape collisions.
/// 
/// Determine whether a bounding shape intersects a wall. If it does,
/// then the second parameter is set to the walls that it collided with,
/// which will usually be one or two walls (one horizontal, one vertical).
/// \param s A bounding shape.
/// \param walls [OUT] A vector of the walls that collided with. 
/// \return true if the bounding shape intersects a wall.

template<class t> bool CTileManager::CollideWithWall(const t& s, vector<BoundingBox>& walls){
  bool hit = false;
  walls.clear();

  for(auto wall: m_vecWalls){
    if(s.Intersects(wall)){
      hit = true;
      walls.push_back(wall);
    } //if
  } //for

  return hit;
} //CollideWithWall