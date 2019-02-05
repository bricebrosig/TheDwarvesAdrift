/// \file ObjectManager.cpp
/// \brief Code for the the object manager class CObjectManager.

#include "ObjectManager.h"
#include "ComponentIncludes.h"
#include "ParticleEngine.h"
#include "Common.h"
#include "DebugPrintf.h"

#define TILESIZE 32
/// Create the tile manager;

CObjectManager::CObjectManager(){
  m_pTileManager = new CTileManager(TILESIZE);
} //constructor

/// Destruct all of the objects in the object list.

CObjectManager::~CObjectManager(){
  delete m_pTileManager;

  for(auto const& p: m_stdObjectList) //for each object
    delete p; //delete object

	delete m_pSwordPointer;
	delete m_pShieldPointer;
} //destructor

/// Load a map from a text file into the tile manager.

void CObjectManager::LoadMap(int level){
	switch (level)
	{
	case 0: m_pTileManager->LoadMapFromImageFile("Media\\Maps\\level1Pixels.png"); break;
	case 1: m_pTileManager->LoadMapFromImageFile("Media\\Maps\\Level2Pixels.png"); break;
	case 2: m_pTileManager->LoadMapFromImageFile("Media\\Maps\\Level3Pixels.png"); break;
	case 3: m_pTileManager->LoadMapFromImageFile("Media\\Maps\\Level4Pixels.png"); break;
  case 4: m_pTileManager->LoadMapFromImageFile("Media\\Maps\\Level5Pixels.png"); break;
	default: break; //leave the game
	}

  m_pTileManager->setLevel(level);
	m_pTileManager->MakeBoundingBoxes();
  vector<Vector3> ladders = m_pTileManager->GetLadders();

  for (auto l : ladders) {
    if (l.z == (float)LADDERL_SPRITE)
      create(LADDERL_SPRITE, Vector2(l.x, l.y));
    else
      create(LADDERR_SPRITE, Vector2(l.x, l.y));
  }

	for (auto& d : m_pTileManager->getDoors())
		create(DOOR_SPRITE, Vector2(d.x,d.y), (int)d.z);

	for (auto& e : m_pTileManager->getEndPoints())
		create(ENDPOINT_SPRITE, Vector2(e.x, e.y));

	for (auto& s : m_pTileManager->getStopPoints())
		create(STOP_SPRITE, Vector2(s.x, s.y));

	for (auto& e : m_pTileManager->getElevators())
		create(ELEVATOR_SPRITE, Vector2(e.x, e.y));
	
	for (auto& t : m_pTileManager->getTraps())
		create(ELECTRIC_SPRITE, Vector2(t.x, t.y), (int)t.z);

	for (auto& s : m_pTileManager->getSwitches())
		create(SWITCH_SPRITE, Vector2(s.x, s.y), (int)s.z);

	for (auto& t : m_pTileManager->getTeleporters())
		create(TELEPORTER_SPRITE, Vector2(t.x, t.y));

	for (auto& t : m_pTileManager->getBreakableWalls())
		create(BREAKABLE_SPRITE, Vector2(t.x, t.y));

	for (auto & t : m_pTileManager->getTeleporterSpawns())
		create(TELEPORTSPAWN_SPRITE, Vector2(t.x, t.y));

	for (auto & k : m_pTileManager->getKeys())
		create(KEY_SPRITE, Vector2(k.x, k.y), (int)k.z);

	for (auto & e : m_pTileManager->getEnemies())
		create(TURRET_SPRITE, Vector2(e.x, e.y));

  for (auto & e : m_pTileManager->getGuns())
    create(GUN_SPRITE, Vector2(e.x, e.y));

	//make the exit by asking the tile manager whats up
	create(EXIT_SPRITE, m_pTileManager->getExitLocation());

	//make the characters from the locations got from tilemanager
	characters.resize(3);
	characters[0] = create(FIGHTER_SPRITE, m_pTileManager->getFighterLocation());
	characters[1] = create(SHIELD_SPRITE, m_pTileManager->getShieldLocation());
	characters[2] = create(FAST_SPRITE, m_pTileManager->getFastLocation());
	whichPlayer = 2;
	m_pPlayer = characters.at(2);//set our player to that one
	m_pShieldPlayer = characters.at(1);//set the pointer to the shield player
	m_pShieldPointer = new CObject(SHIELDUP_SPRITE, m_pShieldPlayer->GetPos());
} //LoadMap

/// Create an object and put a pointer to it on the object list.
/// \param t Sprite type.
/// \param v Initial position..
/// \return Pointer to the object created.

CObject* CObjectManager::create(eSpriteType t, const Vector2& v) {

  CObject* p = nullptr;

  // create the appropriate object based on sprite type
  // push onto the corresponding list
  switch (t){

    case ELEVATOR_SPRITE: {
      p = new Elevator(DOWN, true, t, v);
      m_stdPlatformList.push_back(p);
    } break;

    case ENDPOINT_SPRITE: {
      p = new CObject(t, v);
      m_stdEndPointList.push_back(p);
    } break;

		case STOP_SPRITE: {
			p = new CObject(t, v);
			m_stdStopPointList.push_back(p);
		}break;

    case TURRET_SPRITE: {
      p = new EvilNPC(ATTACK, Vector2(1.0f, 1.0f), 1.0f, t, v);
			m_vEnemies.push_back(p);
    } break;

		case TELEPORTER_SPRITE: {
			p = new CObject(t, v);
			m_vTeleporterList.push_back(p);
		}break;

		case BREAKABLE_SPRITE: {
			p = new CObject(t, v);
			m_vBreakableWallList.push_back(p);
		}break;

		case TELEPORTSPAWN_SPRITE: {
			p = new CObject(t, v);
			m_vTeleportSpawns.push_back(p);
		}break;

    case GUN_SPRITE: {
      p = new GunTurret(RIGHT,t, v);
      m_vGuns.push_back(p);
    }break;

    default: {
      p = new CObject(t, v);
    } break;

  }//switch

  m_stdObjectList.push_back(p);

  return p;
} //create

CObject* CObjectManager::create(eSpriteType t, const Vector2& v, int i)
{
	CObject* p = nullptr;

	switch (t) {
		case ELECTRIC_SPRITE: {
			p = new Trap(true, i, t, v);
			m_vTrapList.push_back(p);
		}break;

		case SWITCH_SPRITE: {
			p = new Switch(false, i, t, v);
			m_vSwitchList.push_back(p);
		}break;

		case DOOR_SPRITE: {
			
				p = new Door(t, v, i); //door with a lock
				m_stdDoorList.push_back(p);
			
		}break;

		case KEY_SPRITE: {
			p = new Key(t, v, i); //door with a lock
			m_vKeys.push_back(p);
		}

		default: break;
	}//switch

	m_stdObjectList.push_back(p);

	return p;
}


/// Delete all of the objects managed by the object manager. 
/// This involves deleting all of the CObject instances pointed
/// to by the object list, then clearing the object list itself.

void CObjectManager::clear(){
  for(auto const& p: m_stdObjectList) //for each object
    delete p; //delete object

  m_stdObjectList.clear(); //clear the object list
	m_stdEndPointList.clear();
	m_stdPlatformList.clear();
	m_vSwitchList.clear();
	m_vTrapList.clear();
	m_stdDoorList.clear();
	m_vTeleporterList.clear();
	m_vTeleportSpawns.clear();
	m_vBreakableWallList.clear();
	m_vKeys.clear();
	m_vEnemies.clear();
  m_vGuns.clear();

	m_pTileManager->clear();

} //clear

/// Draw the tiled background and all of the objects.

void CObjectManager::draw(){
  //draw tiled backgrounds

  m_pTileManager->Draw(TILE_SPRITE);
  if(m_bDrawAABBs)
    m_pTileManager->DrawBoundingBoxes(GREENLINE_SPRITE);

  //draw objects
  for(auto const& p: m_stdObjectList){ //for each object
		m_pRenderer->Draw(*(CSpriteDesc2D*)p);
     
    if(m_bDrawAABBs)
      m_pRenderer->DrawBoundingBox(p->GetBoundingBox());
  } //for

  if (m_pSwordPointer) 
  {
    m_pRenderer->Draw(*(CSpriteDesc2D*)m_pSwordPointer);

    if (m_bDrawAABBs)
      m_pRenderer->DrawBoundingBox(m_pSwordPointer->GetBoundingBox());
  }

	if (m_pShieldPointer)
	{
		m_pRenderer->Draw(*(CSpriteDesc2D*)m_pShieldPointer);

		if (m_bDrawAABBs)
			m_pRenderer->DrawBoundingBox(m_pShieldPointer->GetBoundingBox());
	}
} //draw

/// Move all of the objects and perform 
/// broad phase collision detection and response.

void CObjectManager::move(const float & t) {
	if (state == TITLE_STATE) // if we are in the title state we just want to bail
		return;

	bool ladderFlag = false;//say we arent on a ladder

	for (auto const & p : m_stdObjectList) { //for each object
		const Vector2 oldpos = p->m_vPos; //its old position

		p->move(t); //move it

		//object to wall collision detection and response using
		//bounding spheres for the objects and AABBs for the walls.

		vector <BoundingBox> walls; //bounding boxes for wall collision

		// wall collision
		if (m_pTileManager->CollideWithWall(p->GetBoundingBox(), walls))
		{
			switch (p->m_nSpriteIndex) {
			case KEY_SPRITE:
			case FIGHTER_SPRITE:
			case FAST_SPRITE:
			case SHIELD_SPRITE:
				for (auto const& b : walls)
					p->CollisionResponse(b);
				break;
			case TURRET_SPRITE:
				for (auto const& b : walls)
					((EvilNPC*)p)->CollisionResponse(b);
				break;
			case BULLET2_SPRITE:
			case BULLET_SPRITE:
				p->kill();
				m_pAudio->play(RICOCHET_SOUND);
				break;
			default: break;
      } //switch
    }//if collide with wall

    // object specific collision
    switch (p->m_nSpriteIndex) {
		case KEY_SPRITE:
			for (auto const& w : m_vBreakableWallList) {
				if (p->GetBoundingBox().Intersects(w->GetBoundingBox())) {
					p->CollisionResponse(w->GetBoundingBox()); 
				}
			}
			break;
    case BULLET_SPRITE: // player projecties
      // collide with regular doors
      for (auto const& d : m_stdDoorList) {
        if (!((Door*)d)->getIsOpen() && p->GetBoundingBox().Intersects(d->GetBoundingBox())) {
          p->kill();
          m_pAudio->play(RICOCHET_SOUND);
        }
      }
      // breakable walls
      for (auto const& w : m_vBreakableWallList) {
        if (p->GetBoundingBox().Intersects(w->GetBoundingBox())) {
          p->kill();
          m_pAudio->play(RICOCHET_SOUND);
          w->kill();
        }
      }
      // switches
      for (auto const & s : m_vSwitchList) {
        if (p->GetBoundingBox().Intersects(s->GetBoundingBox())) {
          ((Switch*)s)->flipTrapSwitch();
          p->kill();
        }
      }
      // enemies
      //for (auto const& w : m_vEnemies) {
      //  if (p->GetBoundingBox().Intersects(w->GetBoundingBox())) {
      //    p->kill();
      //    m_pAudio->play(RICOCHET_SOUND);
      //    w->kill();
      //  }
      //}
      break;
    case BULLET2_SPRITE: // enemy projectiles
      // doors
      for (auto const& d : m_stdDoorList) {
        if (!((Door*)d)->getIsOpen() && p->GetBoundingBox().Intersects(d->GetBoundingBox())) {
          p->kill();
          m_pAudio->vary(RICOCHET_SOUND,0.2f);
        }
      }
      // shields
      if (m_pShieldPointer && p->GetBoundingBox().Intersects(m_pShieldPointer->GetBoundingBox())) {
        p->kill();
        m_pAudio->vary(RICOCHET_SOUND, 0.1f);
      }
      for (auto const& w : m_vBreakableWallList) {
        if (p->GetBoundingBox().Intersects(w->GetBoundingBox())) {
          p->kill();
          m_pAudio->play(RICOCHET_SOUND);
        }
          
      }
      break;
    case FIGHTER_SPRITE:
    case FAST_SPRITE:
    case SHIELD_SPRITE:
      p->m_bOnElevator = false;

      //platforms
      for (auto const& e : m_stdPlatformList) {
        if (p->GetBoundingBox().Intersects(e->GetBoundingBox())) {
          p->elevatorResponse(e->GetBoundingBox());
          p->m_bOnElevator = true;
        }
      }
      //doors
      for (auto const& d : m_stdDoorList) {
        if (!((Door*)d)->getIsOpen() && p->GetBoundingBox().Intersects(d->GetBoundingBox()))
          p->CollisionResponse(d->GetBoundingBox());
      }
      // breakable walls
      for (auto const& w : m_vBreakableWallList) {
        if (p->GetBoundingBox().Intersects(w->GetBoundingBox()))
          p->CollisionResponse(w->GetBoundingBox());
      }
      // teleporters
      for (auto & t : m_vTeleporterList) {
        if (m_pPlayer->m_bInteract && p->GetBoundingBox().Intersects(t->GetBoundingBox())) {
          for (auto & ts : m_vTeleportSpawns) {
            p->m_vPos = ts->GetPos();
          }
        }
      }
      // keys
      for (auto & k : m_vKeys) {
        if (p->GetBoundingBox().Intersects(k->GetBoundingBox())) {
          ((Key*)k)->unlockDoors();
        }
      }
      break;
    case ELEVATOR_SPRITE:
      // endpoints
      for (auto const& ep : m_stdEndPointList) {
        if (p->GetBoundingBox().Intersects(ep->GetBoundingBox())) {
          if (p->GetVelocity().y < 0.0f && p->GetBoundingBox().Center.y >= ep->GetBoundingBox().Center.y)
            ((Elevator*)p)->turnAround();
          else if (p->GetVelocity().y > 0.0f && p->GetBoundingBox().Center.y <= ep->GetBoundingBox().Center.y)
            ((Elevator*)p)->turnAround();
        }
      }
      // stops 
      for (auto const& ep : m_stdStopPointList) {
        if (p->GetBoundingBox().Intersects(ep->GetBoundingBox())) {
          if (p->GetVelocity().y < 0.0f && p->GetBoundingBox().Center.y <= ep->GetBoundingBox().Center.y && !((Elevator*)p)->justGotOffStop) 
            ((Elevator*)p)->stop(ep->m_vPos);
            
          else if (p->GetVelocity().y > 0.0f && p->GetBoundingBox().Center.y >= ep->GetBoundingBox().Center.y && !((Elevator*)p)->justGotOffStop)
            ((Elevator*)p)->stop(ep->m_vPos);
        }
          
			}
			break;
		case LADDERL_SPRITE:
		case LADDERR_SPRITE: {
			if (p->GetBoundingBox().Intersects(m_pPlayer->GetBoundingBox()))
				ladderFlag = true;//say we are on a ladder
		}break;

    case EXIT_SPRITE: {

      bool allOnExit = true;

      // check if all characters left in the vector are on the point
      for (const auto &c : characters) {
        allOnExit = allOnExit && p->GetBoundingBox().Intersects(c->GetBoundingBox());
      }

      if (allOnExit)
        state = WIN_STATE;

    }break;

		case TURRET_SPRITE: {


      float range = 256.0f;
      bool inRange = false;
      float closest = range + 1; // closest character x distance
      CObject* target = nullptr;

      // find closest player in range
      for (auto &c : characters) {
        Vector2 v = c->m_vPos - p->m_vPos;

        // if in range and closest player, update closest player var
        if (fabsf(v.x) <= range && fabsf(v.x) < closest) {
          // if visible
          if (m_pTileManager->Visible(p->m_vPos, c->m_vPos, c->m_Sphere.Radius)) {
            if (fabsf(v.y) <= 20.0f) { // dont worry about people above
              closest = v.x;
              inRange = true;
              target = c;
            }
          }
        }
      }

      if (inRange) {
        // attack
        p->setState(ATTACK);

        // player to the right
        if (closest > 0.0f) {
          p->turn(RIGHT);
        }
        else if (closest < 0.0f) {
          p->turn(LEFT);
        }

        // fire gun if its been long enough
        if (m_pTimer->elapsed(p->m_fGunTimer, 1.5f))
          if (p->GetVelocity().x != 0.0f)
            FireGun(p, BULLET2_SPRITE, target);
      }
      else {
        // its not mad, just walk around
        p->setState(PATROL);
      }
			

			// check for patrol point collision
			for (auto const& ep : m_stdEndPointList)
				if (p->GetBoundingBox().Intersects(ep->GetBoundingBox()))
					((EvilNPC*)p)->CollisionResponse(ep->GetBoundingBox());

			//doors collision
			for (auto const& d : m_stdDoorList) {
				if (!((Door*)d)->getIsOpen() && p->GetBoundingBox().Intersects(d->GetBoundingBox()))
					((EvilNPC*)p)->CollisionResponse(d->GetBoundingBox());
			}

      // breakable walls
      for (auto const& w : m_vBreakableWallList) {
        if (((EvilNPC*)p)->GetBoundingBox().Intersects(w->GetBoundingBox()))
        ((EvilNPC*)p)->CollisionResponse(w->GetBoundingBox());
      }

      // collide with player sheild
      if (m_pShieldPointer) 
        if(p->GetBoundingBox().Intersects(m_pShieldPointer->GetBoundingBox()))
          ((EvilNPC*)p)->CollisionResponse(m_pShieldPointer->GetBoundingBox());

		} break;
    
    case GUN_SPRITE: {
      
      bool isAggro = false;

      // aggro if any characters are in range
      for (auto &c : characters) {

        if (isAggro) {
          break; // already mad
        }
        else { // try and get mad

          GunTurret* gun = (GunTurret*)p;
          const float r = c->m_Sphere.Radius;
          bool bVisible = m_pTileManager->Visible(p->m_vPos, c->m_vPos, r);
          // vector between player and enemy
          Vector2 direction = c->GetPos() - gun->m_vPos;

          if (bVisible) {
            if (direction.x > 0.0f && direction.x < gun->getRange() || direction.x < 0.0f && direction.x > gun->getRange()) {
              isAggro = true;

              if (m_pTimer->elapsed(p->m_fGunTimer, 1.5f))
                FireGun(p, BULLET2_SPRITE);
            }
          }
        }
      }
    } break;

		case ELECTRIC_SPRITE: {
      for (auto &c : characters) {
        if (p->GetBoundingBox().Intersects(c->GetBoundingBox())) {
          //m_pAudio->vary(OW_SOUND);
          c->TrapReaction();//react to getting shocked
          c->damage(1);//damage player for 1
        }
      }
		}break;

		case SWITCH_SPRITE: {
			if (p->GetBoundingBox().Intersects(m_pPlayer->GetBoundingBox()) && m_pPlayer->m_bInteract)
			{
				((Switch*)p)->flipTrapSwitch();
			}
		}break;

		default: break;
		}//switch
	} //for every object

	m_pPlayer->m_bOnLadder = ladderFlag;

	// move the sword
  if (m_pSwordPointer) {

    // update position to match player
    m_pSwordPointer->move(t);

    // collide with enemies/switches/walls
    for (auto const& w : m_vBreakableWallList) {
      if (m_pSwordPointer->GetBoundingBox().Intersects(w->GetBoundingBox())) {
        w->kill();
      }
    }
    for (auto const & s : m_vSwitchList) {
      if (m_pSwordPointer->GetBoundingBox().Intersects(s->GetBoundingBox())) {
        ((Switch*)s)->flipTrapSwitch();
      }
    }
    for (auto const& w : m_vEnemies) {
      if (m_pSwordPointer->GetBoundingBox().Intersects(w->GetBoundingBox())) {
        w->kill();
        m_pAudio->play(ENEMY_OW);
      }
    }
  }
		

	if (m_pShieldPointer)
		m_pShieldPointer->move(t);
	//now do object-object collision detection and response and
	//remove any dead objects from the object list.

	BroadPhase(); //broad phase collision detection and response
	CullDeadObjects(); //remove dead objects from object list
} //move

CObject* CObjectManager::isTouchingElevator()
{
  for (int i = 0; i < m_stdPlatformList.size(); i++)
    if (m_pPlayer->GetBoundingBox().Intersects(m_stdPlatformList[i]->GetBoundingBox()))
      return (m_stdPlatformList[i]);

	return nullptr;
}

/// Create a bullet object and a flash particle effect.
/// It is assumed that the object is round and that the bullet
/// appears at the edge of the object in the direction
/// that it is facing and continues moving in that direction.
/// \param pObj Pointer to an object.
/// \param bullet Sprite type of bullet.

void CObjectManager::FireGun(CObject* pObj, eSpriteType bullet){
  

  const Vector2 view = pObj->GetViewVector();
  Vector2 pos = pObj->GetPos() + 
    0.5f*m_pRenderer->GetWidth(pObj->m_nSpriteIndex)*view;

  pos -= 0.5f*m_pRenderer->GetHeight(pObj->m_nSpriteIndex)*view;

  //create bullet object

  CObject* pBullet = nullptr;

  const Vector2 norm(view.y, -view.x); //normal to direction
  const float m = 2.0f*m_pRandom->randf() - 1.0f;
  const Vector2 deflection = 0.01f*m*norm;

  if (pObj->m_nSpriteIndex == TURRET_SPRITE ) {//shoot at the player!
    m_pAudio->play(LASER_SOUND);

    if (pObj->GetPos().x > m_pPlayer->GetPos().x) {

      pBullet = m_pObjectManager->create(bullet, pObj->GetPos() + -0.5f*m_pRenderer->GetWidth(pObj->m_nSpriteIndex)*Vector2::UnitX); //create bullet
      pBullet->SetVelocity(Vector2::UnitX * -400.0f);
      // face the player
      pObj->turn(LEFT);
    }
    else if (pObj->GetPos().x < m_pPlayer->GetPos().x) {
      pBullet = m_pObjectManager->create(bullet, pObj->GetPos() + 0.5f*m_pRenderer->GetWidth(pObj->m_nSpriteIndex)*Vector2::UnitX); //create bullet

      pBullet->SetVelocity(Vector2::UnitX * 400.0f);
      // face the player
      pObj->turn(RIGHT);
    }

    //particle effect for laser fire

    CParticleDesc2D d;

    d.m_nSpriteIndex = SPARK_SPRITE;
    d.m_vPos = pos;
    d.m_vVel = pObj->GetSpeed()*view;
    d.m_fLifeSpan = 0.25f;
    d.m_fScaleInFrac = 0.4f;
    d.m_fFadeOutFrac = 0.5f;
    d.m_fMaxScale = 0.5f;
    d.m_f4Tint = XMFLOAT4(Colors::Green);

    m_pParticleEngine->create(d);

  }
  else if (pObj->m_nSpriteIndex == GUN_SPRITE) {
    //m_pAudio->play(LASER_SOUND);
    m_pAudio->vary(LASER_SOUND,0.5f);

    if (pObj->GetPos().x > m_pPlayer->GetPos().x && pObj->m_nFacing == LEFT) {

      pBullet = m_pObjectManager->create(bullet, pObj->GetPos() + -0.5f*m_pRenderer->GetWidth(pObj->m_nSpriteIndex)*Vector2::UnitX); //create bullet
      pBullet->m_f4Tint = XMFLOAT4(Colors::Red); // gun turret has red lasers
      pBullet->SetVelocity(Vector2::UnitX * -500.0f);

    }
    else if (pObj->GetPos().x < m_pPlayer->GetPos().x  && pObj->m_nFacing == RIGHT) {
      pBullet = m_pObjectManager->create(bullet, pObj->GetPos() + 0.5f*m_pRenderer->GetWidth(pObj->m_nSpriteIndex)*Vector2::UnitX); //create bullet
      pBullet->m_f4Tint = XMFLOAT4(Colors::Red); // gun turret has red lasers
      pBullet->SetVelocity(Vector2::UnitX * 500.0f);
    }

    
    //particle effect for laser fire

    CParticleDesc2D d;

    d.m_nSpriteIndex = SPARK_SPRITE;
    d.m_vPos = pos;
    d.m_vVel = pObj->GetSpeed()*view;
    d.m_fLifeSpan = 0.25f;
    d.m_fScaleInFrac = 0.4f;
    d.m_fFadeOutFrac = 0.5f;
    d.m_fMaxScale = 0.5f;
    d.m_f4Tint = XMFLOAT4(Colors::Red); // gun turret has red effects

    m_pParticleEngine->create(d);

  }
	else {
    pBullet = m_pObjectManager->create(bullet, pos);
    if (m_pPlayer->isFacing(RIGHT)) {//if we are facing to the right we ought to shoot the the right
      pBullet->SetVelocity(m_pPlayer->GetVelocity() + 500.0f*(norm + deflection));
      pBullet->m_nCurrentFrame = 1;
    }
    else { //and visa versa
      pBullet->SetVelocity(m_pPlayer->GetVelocity() + 500.0f*(-norm + deflection));
      pBullet->m_nCurrentFrame = 0;
    }
	}

  if(pBullet)
    pBullet->SetOrientation(m_pPlayer->GetOrientation()); 


} //FireGun

// fire gun at a given characer
void CObjectManager::FireGun(CObject* pObj, eSpriteType bullet, CObject* c) {


  const Vector2 view = pObj->GetViewVector();
  Vector2 pos = pObj->GetPos() +
    0.5f*m_pRenderer->GetWidth(pObj->m_nSpriteIndex)*view;

  pos -= 0.5f*m_pRenderer->GetHeight(pObj->m_nSpriteIndex)*view;

  //create bullet object

  CObject* pBullet = nullptr;

  const Vector2 norm(view.y, -view.x); //normal to direction
  const float m = 2.0f*m_pRandom->randf() - 1.0f;
  const Vector2 deflection = 0.01f*m*norm;

  if (pObj->m_nSpriteIndex == TURRET_SPRITE) {//shoot at the player!
    m_pAudio->play(LASER_SOUND);

    if (pObj->GetPos().x > c->GetPos().x) {

      pBullet = m_pObjectManager->create(bullet, pObj->GetPos() + -0.5f*m_pRenderer->GetWidth(pObj->m_nSpriteIndex)*Vector2::UnitX); //create bullet
      pBullet->SetVelocity(Vector2::UnitX * -400.0f);
      // face the player
      pObj->turn(LEFT);
    }
    else if (pObj->GetPos().x < c->GetPos().x) {
      pBullet = m_pObjectManager->create(bullet, pObj->GetPos() + 0.5f*m_pRenderer->GetWidth(pObj->m_nSpriteIndex)*Vector2::UnitX); //create bullet

      pBullet->SetVelocity(Vector2::UnitX * 400.0f);
      // face the player
      pObj->turn(RIGHT);
    }

    //particle effect for laser fire

    CParticleDesc2D d;

    d.m_nSpriteIndex = SPARK_SPRITE;
    d.m_vPos = pos;
    d.m_vVel = pObj->GetSpeed()*view;
    d.m_fLifeSpan = 0.25f;
    d.m_fScaleInFrac = 0.4f;
    d.m_fFadeOutFrac = 0.5f;
    d.m_fMaxScale = 0.5f;
    d.m_f4Tint = XMFLOAT4(Colors::Green);

    m_pParticleEngine->create(d);

  }
  else if (pObj->m_nSpriteIndex == GUN_SPRITE) {
    m_pAudio->play(LASER_SOUND);

    if (pObj->GetPos().x > c->GetPos().x && pObj->m_nFacing == LEFT) {

      pBullet = m_pObjectManager->create(bullet, pObj->GetPos() + -0.5f*m_pRenderer->GetWidth(pObj->m_nSpriteIndex)*Vector2::UnitX); //create bullet
      pBullet->m_f4Tint = XMFLOAT4(Colors::Red); // gun turret has red lasers
      pBullet->SetVelocity(Vector2::UnitX * -500.0f);

    }
    else if (pObj->GetPos().x < c->GetPos().x  && pObj->m_nFacing == RIGHT) {
      pBullet = m_pObjectManager->create(bullet, pObj->GetPos() + 0.5f*m_pRenderer->GetWidth(pObj->m_nSpriteIndex)*Vector2::UnitX); //create bullet
      pBullet->m_f4Tint = XMFLOAT4(Colors::Red); // gun turret has red lasers
      pBullet->SetVelocity(Vector2::UnitX * 500.0f);
    }


    //particle effect for laser fire

    CParticleDesc2D d;

    d.m_nSpriteIndex = SPARK_SPRITE;
    d.m_vPos = pos;
    d.m_vVel = pObj->GetSpeed()*view;
    d.m_fLifeSpan = 0.25f;
    d.m_fScaleInFrac = 0.4f;
    d.m_fFadeOutFrac = 0.5f;
    d.m_fMaxScale = 0.5f;
    d.m_f4Tint = XMFLOAT4(Colors::Red); // gun turret has red effects

    m_pParticleEngine->create(d);

  }

} //FireGun


void CObjectManager::swingSword()
{
  //if sword is not null
  //make a sword
  //store the pointer
  //put on list
  if (!m_pSwordPointer) {
    m_pSwordPointer = new CObject(SWORD_SPRITE, m_pPlayer->GetPos());
  }
}

/// Toggles between having the shield up and having the shield down
void CObjectManager::flipShield()
{
	if (m_pShieldPointer->m_bShieldUp)
	{
		m_pShieldPointer->m_bShieldUp = false;
		m_pShieldPlayer->m_bShieldUp = false;
	}
	else
	{
		m_pShieldPointer->m_bShieldUp = true;
		m_pShieldPlayer->m_bShieldUp = true;
	}
}

vector<CObject*> CObjectManager::getVecTraps()
{
	return m_vTrapList;
}

vector<CObject*> CObjectManager::getVecSwitches()
{
	return m_vSwitchList;
}

/// This is a "bring out yer dead" Monty Python type of thing.
/// Iterate through the objects and check whether their "is dead"
/// flag has been set. If so, then delete its pointer from
/// the object list and destruct the object.

void CObjectManager::CullDeadObjects(){
  for(auto i=m_stdObjectList.begin(); i!=m_stdObjectList.end();){
    if((*i)->IsDead())
		{ //"He's dead, Dave." --- Holly, Red Dwarf
			for (auto p = characters.begin(); p!=characters.end();) 
			{
				if (*p == *i)
				{
					//erase it
					if ((*p)->m_nSpriteIndex == SHIELD_SPRITE) {//if it is the shield man we need to also kill his shield!
						delete m_pShieldPointer;
						m_pShieldPointer = nullptr;
					}	
					p = characters.erase(p);
				}
				else ++p;
			}//for

			for (auto p = m_vEnemies.begin(); p != m_vEnemies.end();)
			{
				if (*p == *i)
				{
					//erase it
					p = m_vEnemies.erase(p);
				}
				else ++p;
			}//for

			for (auto w = m_vBreakableWallList.begin(); w != m_vBreakableWallList.end();)
			{
				if (*w == *i)
				{
					//delete it
					w = m_vBreakableWallList.erase(w);
				}
				else ++w;
			}

      delete *i; //delete object
      i = m_stdObjectList.erase(i); //remove from object list and advance to next object
    } //if

    else ++i; //advance to next object
  } //for

  if (m_pSwordPointer && m_pSwordPointer->tooOld()) {
    delete m_pSwordPointer;
    m_pSwordPointer = nullptr;
  }
} //CullDeadObjects

/// Perform collision detection and response for all pairs
/// of objects in the object list, making sure that each
/// pair is processed only once.

void CObjectManager::BroadPhase(){
  for(auto i=m_stdObjectList.begin(); i!=m_stdObjectList.end(); i++){
    for(auto j=next(i); j!=m_stdObjectList.end(); j++)
      NarrowPhase(*i, *j);
  } //for
} //BroadPhase

/// Perform collision detection and response for a pair of objects.
/// We are talking about bullets hitting the player and the
/// turrets here. When a collision is detected the response
/// is to delete the bullet (i.e. mark its "is dead" flag)
/// play a particle effect at the point of contact, and play one 
/// sound for the player and another for the turrets.
/// \param p0 Pointer to the first object.
/// \param p1 Pointer to the second object.

void CObjectManager::NarrowPhase(CObject* p0, CObject* p1){
  eSpriteType t0 = (eSpriteType)p0->m_nSpriteIndex;
  eSpriteType t1 = (eSpriteType)p1->m_nSpriteIndex;

	if (p0->m_Sphere.Intersects(p1->m_Sphere)) { //bounding spheres intersect
		if (t0 == BULLET_SPRITE && t1 == TURRET_SPRITE) { //bullet hits turret
			p0->kill();
			m_pAudio->play(ENEMY_OW);
      p1->kill();
		} //if

		else if (t1 == BULLET_SPRITE && t0 == TURRET_SPRITE) { //turret hit by bullet
			p1->kill();
			m_pAudio->play(ENEMY_OW);
      p0->kill();
		} //else if

		else if (t1 == BULLET2_SPRITE && t0 == FAST_SPRITE) { //bullet hits player
      if (!p1->IsDead()) {
        p1->kill();
        p0->damage(1);
      }
		} //else if

    else if (t0 == BULLET2_SPRITE && t1 == FAST_SPRITE) { //bullet hits player
      if (!p0->IsDead()) {
        p0->kill();
        p1->damage(1);
      }
    } //else if

    else if (t1 == BULLET2_SPRITE && t0 == FIGHTER_SPRITE) { //bullet hits player
      if (!p1->IsDead()) {
        p1->kill();
        p0->damage(1);
      }
    } //else if

    else if (t0 == BULLET2_SPRITE && t1 == FIGHTER_SPRITE) { //bullet hits player
      if (!p0->IsDead()) {
        p0->kill();
        p1->damage(1);
      }
    } //else if

    else if (t1 == BULLET2_SPRITE && t0 == SHIELD_SPRITE) { //bullet hits player
      if (!p1->IsDead()) {
        p1->kill();
        p0->damage(1);
      }
    } //else if

    else if (t0 == BULLET2_SPRITE && t1 == SHIELD_SPRITE) { //bullet hits player
      if (!p0->IsDead()) {
        p0->kill();
        p1->damage(1);
      }
    } //else if


    else if (t1 == TURRET_SPRITE && t0 == FAST_SPRITE) { //bullet hits player
      if (!p1->IsDead()) {
        p0->damage(1);
      }
    }

    else if (t0 == TURRET_SPRITE && t1 == FAST_SPRITE) { //bullet hits player
      if (!p0->IsDead()) {
        p1->damage(1);
      }
    }

    else if (t1 == TURRET_SPRITE && t0 == FIGHTER_SPRITE) { //bullet hits player
      if (!p1->IsDead()) {
        p0->damage(1);
      }
    }

    else if (t0 == TURRET_SPRITE && t1 == FIGHTER_SPRITE) { //bullet hits player
      if (!p0->IsDead()) {
        p1->damage(1);
      }
    }

    else if (t1 == TURRET_SPRITE && t0 == SHIELD_SPRITE) { //bullet hits player
      if (!p1->IsDead()) {
        p0->damage(1);
      }
    }

    else if (t0 == TURRET_SPRITE && t1 == SHIELD_SPRITE) { //bullet hits player
      if (!p0->IsDead()) {
        p1->damage(1);
      }
    }

	}//if
} //NarrowPhase
