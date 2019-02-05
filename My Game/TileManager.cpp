 /// \file TileManager.cpp
/// \brief Code for the tile manager CTileManager.

#include "TileManager.h"
#include "Renderer.h"

#include "ObjectManager.h"
#include "DebugPrintf.h"
#include "Abort.h"

#define STBI_ASSERT(x)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/// \param tilesize Width and height of square tile in pixels.

CTileManager::CTileManager(size_t tilesize):
  m_fTileSize((float)tilesize),
  m_vTileRadius(Vector2(m_fTileSize, m_fTileSize)/2){
} //constructor

/// Delete the memory used for storing the map.

CTileManager::~CTileManager(){
  for(int i=0; i<m_nHeight; i++)
    delete [] m_chMap[i];

  delete [] m_chMap;
} //destructor

/// Make the AABBs for the walls. Care is taken to use the
/// longest horizontal and vertical AABBs possible so that
/// there aren't so many of them.

void CTileManager::MakeBoundingBoxes() {
  bool** used = new bool*[m_nHeight]; //which walls have been used in an AABB already

  for (int i = 0; i < m_nHeight; i++) {
    used[i] = new bool[m_nWidth];

    for (int j = 0; j < m_nWidth; j++)
      used[i][j] = false;
  } //for

  m_vecWalls.clear(); //no walls yet

  BoundingBox aabb; //current bounding box
  const Vector3 vTileExtents = 0.5f*Vector3(m_fTileSize, m_fTileSize, m_fTileSize);

  //vertical walls with more than one tile
  const Vector2 vstart = m_vTileRadius + Vector2(0, m_fTileSize*(m_nHeight - 1)); //start position
  Vector2 pos = vstart; //current position

  for (int j = 0; j < m_nWidth; j++) {
    int i = 0;
    pos.y = vstart.y;

    while (i < m_nHeight) {
      while (i < m_nHeight && (m_chMap[i][j] != 'W' || used[i][j])) {
        i++; pos.y -= m_fTileSize;
      } //while

      if (i < m_nHeight) {
        aabb.Center = Vector3(pos.x, pos.y, 0);
        aabb.Extents = vTileExtents;
        used[i][j] = true;
        int singleI = i;
        int singleJ = j;
        i++; pos.y -= m_fTileSize;

        bool bSingleTile = true;

        while (i < m_nHeight && m_chMap[i][j] == 'W' && !used[i][j]) {
          BoundingBox b;
          b.Center = Vector3(pos.x, pos.y, 0);
          b.Extents = vTileExtents;
          BoundingBox::CreateMerged(aabb, aabb, b);
          used[i][j] = true;
          bSingleTile = false;
          i++; pos.y -= m_fTileSize;
        } //while

        if (!bSingleTile) {
          if (m_vecWalls.size() > 0) {

            bool merged = false;
            // attempt to find any previous walls that are the same height and centered at the 
            // same Y position. If one is found, merge it with the current aabb, otherwise push the aabb on the vector
            for (auto &b : m_vecWalls) {
              if (b.Center.y == aabb.Center.y && b.Extents.y == aabb.Extents.y &&
                // checks if they are adjacent bounding boxes
                (b.Center.x + b.Extents.x == aabb.Center.x - aabb.Extents.x)) {
                // merge and stop loop
                BoundingBox::CreateMerged(b, b, aabb);
                merged = true;
                break;
              }
            }
            // if the box wasnt merged add it to vecWalls
            if (!merged)
              m_vecWalls.push_back(aabb);
          }
          // push back aabb if first wall
          else {
            m_vecWalls.push_back(aabb);
          }
        }
        else {
          // mark the unused tile
          used[singleI][singleJ] = false;
        }
      }
    } //while

    pos.x += m_fTileSize;
  } //for

  //horizontal walls with more than one tile

  pos = vstart; //current position
  int falseCount = 0;
  for (int i = 0; i < m_nHeight; i++) {
    int j = 0;
    pos.x = vstart.x;

    while (j < m_nWidth) {
      // while within width and not W
      while (j < m_nWidth && (m_chMap[i][j] != 'W' || used[i][j])) {
        j++; pos.x += m_fTileSize;
      } //while

      // if within width
      if (j < m_nWidth) {
        aabb.Center = Vector3(pos.x, pos.y, 0);
        aabb.Extents = vTileExtents;
        used[i][j] = true;
        int singleI = i;
        int singleJ = j;
        j++; pos.x += m_fTileSize;


        bool bSingleTile = true;

        while (j < m_nWidth && m_chMap[i][j] == 'W' && !used[i][j]) {
          BoundingBox b;
          b.Center = Vector3(pos.x, pos.y, 0);
          b.Extents = vTileExtents;
          BoundingBox::CreateMerged(aabb, aabb, b);
          used[i][j] = true;
          bSingleTile = false;
          j++; pos.x += m_fTileSize;
        } //while

        if (!bSingleTile) {
          if (m_vecWalls.size() > 0) {

            bool merged = false;
            // attempt to find any previous walls that are the same height and centered at the 
            // same X position. If one is found, merge it with the current aabb, otherwise push the aabb on the vector
            for (auto &b : m_vecWalls) {
              if (b.Center.x == aabb.Center.x && b.Extents.x == aabb.Extents.x &&
                // checks if they are adjacent bounding boxes
                (b.Center.y - b.Extents.y == aabb.Center.y + aabb.Extents.y)) {
                // merge and stop loop
                BoundingBox::CreateMerged(b, b, aabb);
                merged = true;
                break;
              }
            }
            // if the box wasnt merged add it to vecWalls
            if (!merged)
              m_vecWalls.push_back(aabb);
          }
          // push back aabb if first wall
          else {
            m_vecWalls.push_back(aabb);
          }
        }
        else {
          // mark single tile as unused
          used[singleI][singleJ] = false;
        }

      }
    } //while

    pos.y -= m_fTileSize;
  } //for

  //orphaned single tiles that are not on an edge
  pos = vstart + Vector2(m_fTileSize, -m_fTileSize);

  for (int i = 1; i < m_nHeight - 1; i++) {
    for (int j = 1; j < m_nWidth - 1; j++) {
      // 'W' tiles that are unused
      if (m_chMap[i][j] == 'W' && !used[i][j]) {
        BoundingBox b;
        b.Center = Vector3(pos.x, pos.y, 0);
        b.Extents = vTileExtents;
        m_vecWalls.push_back(b);
      } //if

      pos.x += m_fTileSize;
    } //for

    pos.x = vstart.x + m_fTileSize;
    pos.y -= m_fTileSize;
  } //for

  //clean up and exit

  for (int i = 0; i < m_nHeight; i++)
    delete[] used[i];

  delete[] used;
} //MakeBoundingBoxes

/// Delete the old map (if any), allocate the right sized
/// chunk of memory for the new map, and read it from a
/// text file.
/// \param filename Name of the map file.

void CTileManager::LoadMap(char* filename){
  if(m_chMap != nullptr){ //unload any previous maps
    for(int i=0; i<m_nHeight; i++)
      delete [] m_chMap[i];

    delete [] m_chMap;
  } //if

  FILE *input; //input file handle

  fopen_s(&input, filename, "rb"); //open the map file
  if(input == nullptr) //abort if it's missing
    ABORT("Map &s not found.", filename);

  


  //read map file into a character buffer 

  fseek(input, 0, SEEK_END); //seek to end of map file
  const int n = ftell(input); //get file size in bytes
  rewind(input); //seek to start of file

  char *buffer = new char[n + 1]; //temporary character buffer
  fread(buffer, n, 1, input); //read the whole thing in a chunk
  fclose(input); //close the map file, we're done with it

  //get map width and height into m_nWidth and m_nHeight

  m_nWidth = -1; m_nHeight = 0;
  int w = 0; //width of current row

  for(int i=0; i<n; i++){
    if(buffer[i] != '\n')
      w++; //skip characters until the end of line
    else{
      if(w == 0)ABORT("Panic!");
      if(w != m_nWidth && m_nWidth >= 0 && w != 0) //not the same length as the previous one
        ABORT("Line %d of map is not the same length as the previous one.", m_nHeight);
      m_nWidth = w; w = 0; m_nHeight++; //next line
    } //else
  } //for

  m_nWidth--;

  //allocate space for the map 
  
  m_chMap = new char*[m_nHeight];

  for(int i=0; i<m_nHeight; i++)
    m_chMap[i] = new char[m_nWidth];

  //load the map information from the buffer to the map

  int index = 0; //index into character buffer
  
  for(int i=0; i<m_nHeight; i++){
    for(int j=0; j<m_nWidth; j++){
      m_chMap[i][j] = buffer[index]; //load character into map
      index++; //next index
    } //for
    index += 2; //skip end of line character
  } //for

  m_vWorldSize = Vector2((float)m_nWidth, (float)m_nHeight)*m_fTileSize;

  delete [] buffer; //clean up
} //LoadMap


// returns a objColor enum based on the color of a given pixel. Used in LoadMapFromImageFile
objColor CTileManager::getTileColor(unsigned char* buffer, const int &i) {
  int r = buffer[i];
  int g = buffer[i + 1];
  int b = buffer[i + 2];

	if (r == 0 && g == 0 && b == 0)
		return objColor::WALL;

	else if (r == 0 && g == 0 && b == 155)
		return objColor::LADDER_R;

	else if (r == 0 && g == 0 && b == 255)
		return objColor::LADDER_L;

	else if (r == 255 && g == 0 && b == 0)
		return objColor::EXIT;

	else if (r == 0 && g == 155 && b == 255)
		return objColor::FAST;

	else if (r == 1 && g == 155 && b == 255)
		return objColor::FIGHTER;

	else if (r == 2 && g == 155 && b == 255)
		return objColor::SHIELD;

	else if (g == 255 && b == 0)
		return objColor::DOOR;

	else if (g == 250 && b == 250)
		return objColor::KEY;

	else if (r == 0 && g == 0 && b == 100)
		return objColor::ELEVATOR;

	else if (r == 0 && g == 100 && b == 100)
		return objColor::ENDPOINT;

	else if (r == 100 && g == 0 && b == 100)
		return objColor::STOPPOINT;

	else if (g == 200 && b == 200)
		return objColor::ELECTRICITY;

	else if (g == 200 && b == 0)
		return objColor::SWITCH_ELECTRIC;

	else if (r == 150 && g == 150)
		return objColor::TELEPORTER;

	else if (r == 125 && g == 125 && b == 125)
		return objColor::TELEPORTERSPAWN;

	else if (r == 50 && g == 50)
		return objColor::BREAKABLEWALL;

	else if (r == 75 && g == 75 && b == 75)
		return objColor::ENEMY_SPAWNS;

  else if (r == 42 && g == 42 && b == 42)
    return objColor::GUN_SPAWN;

  else
      return objColor::NONE;
}

//sweet function from lab 4
void CTileManager::LoadMapFromImageFile(char* filename) {

  if (m_chMap != nullptr) { //unload any previous maps
    for (int i = 0; i < m_nHeight; i++)
      delete[] m_chMap[i];
    delete[] m_chMap;
  } //if

  //read map file into a byte buffer 
  int channels = 0;
  unsigned char* buffer = stbi_load(filename, &m_nWidth, &m_nHeight, &channels, 0);

  //allocate space for the map 

  m_chMap = new char*[m_nHeight];

  for (int i = 0; i < m_nHeight; i++)
    m_chMap[i] = new char[m_nWidth];

  //load the map information from the buffer to the map

  int index = 0;

  for (int i = 0; i < m_nHeight; i++)
    for (int j = 0; j < m_nWidth; j++) {
           
      const objColor currentColor = getTileColor(buffer, index);
      m_chMap[i][j] = (currentColor == objColor::WALL) ? 'W' : 'F'; //load character into map

      switch (currentColor){
        case objColor::NONE:
        case objColor::WALL: break;

        case objColor::LADDER_R: {
          // position of object
          Vector3 objPos = Vector3((float)j, m_nHeight - (float)i, 0)*m_fTileSize;

          // store sprite type in Z pos
          objPos.z = (float)LADDERR_SPRITE;
          // adjusting for center position of object
          objPos.x += m_fTileSize / 2.0f;
          objPos.y -= m_fTileSize / 2.0f;

          m_vecLadders.push_back(objPos);
        } break;

        case objColor::LADDER_L: {
          // position of object
          Vector3 objPos = Vector3((float)j, m_nHeight - (float)i, 0)*m_fTileSize;

          // store sprite type in Z pos
          objPos.z = (float)LADDERL_SPRITE;
          // adjusting for center position of object
          objPos.x += m_fTileSize / 2.0f;
          objPos.y -= m_fTileSize / 2.0f;

          m_vecLadders.push_back(objPos);
        } break;

        case objColor::EXIT: {
          Vector2 objPos = Vector2((float)j, m_nHeight - (float)i)*m_fTileSize;
          objPos.x += m_fTileSize / 2.0f;
          objPos.y -= m_fTileSize / 2.0f;
          m_vExitLocation = objPos + Vector2(m_fTileSize / 2.0f, -m_fTileSize);
        } break;

        case objColor::FAST: {
          Vector2 objPos = Vector2((float)j, m_nHeight - (float)i)*m_fTileSize;
          // adjusting for center position of object
          objPos.x += m_fTileSize / 2.0f;
          objPos.y -= m_fTileSize / 2.0f;

          m_vFastLocation = objPos;
          } break;

        case objColor::FIGHTER: {
          Vector2 objPos = Vector2((float)j, m_nHeight - (float)i)*m_fTileSize;
          // adjusting for center position of object
          objPos.x += m_fTileSize / 2.0f;
          objPos.y -= m_fTileSize / 2.0f;

          m_vFighterLocation = objPos;
        } break;

        case objColor::SHIELD: {
          Vector2 objPos = Vector2((float)j, m_nHeight - (float)i)*m_fTileSize;
          // adjusting for center position of object
          objPos.x += m_fTileSize / 2.0f;
          objPos.y -= m_fTileSize / 2.0f;

          m_vShieldLocation = objPos;
        } break;

        case objColor::DOOR: {
          Vector3 objPos = Vector3((float)j, m_nHeight - (float)i, 0)*m_fTileSize;

					objPos.z = buffer[i];
          // adjusting for center position of object
          objPos.x += m_fTileSize / 2.0f;
          objPos.y -= m_fTileSize / 2.0f;
					objPos.z = buffer[index];

          m_vecDoors.push_back(objPos);
        } break;

        case objColor::ELEVATOR: {
          Vector2 objPos = Vector2((float)j, m_nHeight - (float)i)*m_fTileSize;

          // adjusting for center position of object
          objPos.x += m_fTileSize;
          objPos.y -= m_fTileSize / 2.0f;

          m_vecElevators.push_back(objPos);
        } break;

        case objColor::ENDPOINT: {
          Vector2 objPos = Vector2((float)j, m_nHeight - (float)i)*m_fTileSize;

          // adjusting for center position of object
          objPos.y -= m_fTileSize / 2.0f;

          m_vecEndPoints.push_back(objPos);
        } break;
        case objColor::STOPPOINT: {
          Vector2 objPos = Vector2((float)j, m_nHeight - (float)i)*m_fTileSize;

          // adjusting for center position of object
          //objPos.x += m_fTileSize / 2.0f;
          objPos.y -= m_fTileSize / 2.0f;
					
          m_vecStopPoints.push_back(objPos);
        } break;
				case objColor::ELECTRICITY: {
					//make electricity group buffer[i]
					Vector3 objPos = Vector3((float)j, m_nHeight - (float)i, 0)*m_fTileSize;

					objPos.z = buffer[index]; //index of electricity
					// adjusting for center position of object
					objPos.x += m_fTileSize / 2.0f;
					objPos.y -= m_fTileSize / 2.0f;

					m_vecTraps.push_back(objPos);
				}break;

				case objColor::SWITCH_ELECTRIC: {
					//make switch group buffer [i]
					Vector3 objPos = Vector3((float)j, m_nHeight - (float)i, 0)*m_fTileSize;

					objPos.z = buffer[index]; //index of switch
					// adjusting for center position of object
					objPos.x += m_fTileSize / 2.0f;
					objPos.y -= m_fTileSize / 2.0f;

					m_vecSwitches.push_back(objPos);
				}break;

				case objColor::TELEPORTER: {
					Vector3 objPos = Vector3((float)j, m_nHeight - (float)i, 0)*m_fTileSize; // this is a vector three incase we want to use that 3rd channel as a key

					objPos.z = buffer[index]; //index of switch
					// adjusting for center position of object
					objPos.x += m_fTileSize;

					m_vecTeleporters.push_back(objPos);
				}break;

				case objColor::TELEPORTERSPAWN: {
					Vector2 objPos = Vector2((float)j, m_nHeight - (float)i)*m_fTileSize;

					// adjusting for center position of object
					objPos.x += m_fTileSize / 2.0f;
					objPos.y -= m_fTileSize / 2.0f;

					m_vecTeleportSpawns.push_back(objPos);
				}break;

				case objColor::BREAKABLEWALL: {
					Vector2 objPos = Vector2((float)j, m_nHeight - (float)i)*m_fTileSize;

					// adjusting for center position of object
					objPos.x += m_fTileSize/2.0f;
					objPos.y += m_fTileSize;

					m_vecBreakableWalls.push_back(objPos);
				}break;

				case objColor::KEY: {
					Vector3 objPos = Vector3((float)j, m_nHeight - (float)i, 0)*m_fTileSize;

					// adjusting for center position of object
					objPos.x += m_fTileSize / 2.0f;
					objPos.y -= m_fTileSize / 2.0f;
					objPos.z = buffer[index];

					m_vecKeys.push_back(objPos);
				}break;

				case objColor::ENEMY_SPAWNS: {
					Vector2 objPos = Vector2((float)j, m_nHeight - (float)i)*m_fTileSize;

					// adjusting for center position of object
					objPos.x += m_fTileSize / 2.0f;
					objPos.y -= m_fTileSize / 2.0f;

					m_vecEnemies.push_back(objPos);
				}break;

        case objColor::GUN_SPAWN: {
          Vector2 objPos = Vector2((float)j, m_nHeight - (float)i)*m_fTileSize;

          // adjusting for center position of object
          objPos.x += m_fTileSize / 2.0f;
          objPos.y -= m_fTileSize / 2.0f;

          m_vecGuns.push_back(objPos);
        }break;

        default:  break;
      }
      index += channels;
    } //for

    //finish up

  m_vWorldSize = Vector2((float)m_nWidth, (float)m_nHeight)*m_fTileSize;
  MakeBoundingBoxes();

  stbi_image_free(buffer);
} //LoadMapFromImageFile

/// This is for debug purposes so that you can verify that
/// the collision shapes are in the right places.
/// \param t Line sprite to be stretched to draw the line.

void CTileManager::DrawBoundingBoxes(eSpriteType t){
  for(auto&  p: m_vecWalls)
    m_pRenderer->DrawBoundingBox(p);
} //DrawBoundingBoxes

void CTileManager::clear(){
	m_vecLadders.clear();
	m_vecDoors.clear();
	m_vecElevators.clear();
	m_vecEndPoints.clear();
	m_vecStopPoints.clear();
	m_vecSwitches.clear();
	m_vecTraps.clear();
	m_vecBreakableWalls.clear();
	m_vecTeleporters.clear();
	m_vecTeleportSpawns.clear();
	m_vecKeys.clear();
	m_vecEnemies.clear();
  m_vecGuns.clear();
}

/// Draw order is top-down, left-to-right so that the image
/// agrees with the map text file viewed in NotePad.
/// \param t Sprite type for a 3-frame sprite: 0 is floor, 1 is wall, 2 is an error flag.
 
void CTileManager::Draw(eSpriteType t){
  CSpriteDesc2D desc;
  desc.m_nSpriteIndex = t;


  // change wall tint based on current level
  XMFLOAT4 wallTint;
  switch (m_nLevel) {
    case 0: wallTint = XMFLOAT4(Colors::DodgerBlue); break;
    case 1: wallTint = XMFLOAT4(Colors::SeaGreen); break;
    case 2: wallTint = XMFLOAT4(Colors::SteelBlue); break;
    case 3: wallTint = XMFLOAT4(Colors::SlateGray); break;
    case 4: wallTint = XMFLOAT4(Colors::Firebrick); break;
    default: wallTint = XMFLOAT4(Colors::AliceBlue); break;
  }

  const Vector2 vstart = m_vTileRadius + Vector2(0, m_fTileSize*(m_nHeight - 1)); //start position
  Vector2 pos = vstart; //current position
  
  for(int i=0; i<m_nHeight; i++){
    for(int j=0; j<m_nWidth; j++){
      desc.m_vPos = pos;

      switch(m_chMap[i][j])
			{
      case 'F': {
        desc.m_nCurrentFrame = 0;
        desc.m_f4Tint = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // floors should always have default tint
        }
        break;
      case 'W': {
        desc.m_f4Tint = wallTint; // apply wall tint
        desc.m_nCurrentFrame = 1; 
      }break;
        default:  desc.m_nCurrentFrame = 2; break;
      } //switch

      m_pRenderer->Draw(desc);
      pos.x += m_fTileSize; //left to right
    } //for

    pos.x = vstart.x;
    pos.y -= m_fTileSize; //top to bottom
  } //for
} //Draw

/// Check whether a circle is visible from a point, that is, either the left
/// or the right side of the object (from the perspective of the point)
/// has no walls between it and the point. This gives some weird behavior
/// when the circle is partially hidden by a block, but it doesn't seem
/// particularly unnatural in practice. It'll do.
/// \param p0 A point.
/// \param p1 Center of circle.
/// \param radius Radius of circle.
/// \return true If the circle is visible from the point.

bool CTileManager::Visible(const Vector2& p0, const Vector2& p1, float radius){
  bool visible = true;

  for(auto i=m_vecWalls.begin(); i!=m_vecWalls.end() && visible; i++){
    Vector2 direction = p0 - p1;
    direction.Normalize();
    const Vector2 norm = Vector2(-direction.y, direction.x);

    const float delta = min(radius, 16.0f);

    //left-hand triangle
    const Vector3 v0(p0);
    const Vector3 v1(p1 + radius*norm);
    const Vector3 v2(p1 + (radius - delta)*norm);
    
    //right-hand triangle (or perhaps it's the other way round)
    const Vector3 v3(p1 - radius*norm);
    const Vector3 v4(p1 - (radius - delta)*norm);

    visible = !(*i).Intersects(v0, v1, v2) || !(*i).Intersects(v0, v3, v4);
  } //for

  return visible;
} //Visible
