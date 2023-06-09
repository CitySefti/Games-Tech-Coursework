#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"
#include "LifeUp.h"
#include "BulletLifeUp.h"
#include "Alienship.h"

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char *argv[])
	: GameSession(argc, argv)
{
	mLevel = 0;
	mAsteroidCount = 0;
	mEnemyCount = 0;
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	Animation *explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation *asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	Animation *spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");

	Animation* lifeup_anim = AnimationManager::GetInstance().CreateAnimationFromFile("lifeup", 128, 128, 128, 128, "lifeup.png");
	Animation* bulletlife_anim = AnimationManager::GetInstance().CreateAnimationFromFile("bulletlife", 128, 128, 128, 128, "bulletlife.png");
	Animation* alienship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("alienship", 128, 128, 128, 128, "alienship.png");

	//Create the GUI
	CreateGUI();

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);

	CreateLifeUp(1);
	CreateBulletLife(1);
	CreateAlienship(1);
	
	// Start the game
	GameSession::Start();
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	switch (key)
	{
	case ' ':
		mSpaceship->Shoot();
		break;
	case 13: // Enter
		mStartScreenTitle->SetVisible(false);
		mStartScreenLabel->SetVisible(false);
		mLivesLabel->SetVisible(true);
		mScoreLabel->SetVisible(true);
		mGameWorld->AddObject(CreateSpaceship());
		CreateAsteroids(10);
	default:
		break;
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is pressed start applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
	// If left arrow key is pressed start rotating anti-clockwise
	case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
	// If right arrow key is pressed start rotating clockwise
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
	// If up arrow key is pressed start applying backward thrust
	case GLUT_KEY_DOWN: mSpaceship->Thrust(-10); break;
	// Default case - do nothing
	default: break;
	}
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	switch (key)
	{
	// If up arrow key is released stop applying forward thrust
	case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
	// If left arrow key is released stop rotating
	case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
	// If right arrow key is released stop rotating
	case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
	// If down arrow key is released stop applying backward thrust
	case GLUT_KEY_DOWN: mSpaceship->Thrust(0); break;
	// Default case - do nothing
	default: break;
	} 
}


// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(object->GetPosition());
	explosion->SetRotation(object->GetRotation());

	if (object->GetType() == GameObjectType("Asteroid"))
	{
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;
		if (mEnemyCount <= 0 && mAsteroidCount <= 0) {
			SetTimer(500, START_NEXT_LEVEL);
		}
	}
	if (object->GetType() == GameObjectType("LifeUp"))
	{
		mGameWorld->AddObject(explosion);
	}
	if (object->GetType() == GameObjectType("BulletLifeUp"))
	{
		mGameWorld->AddObject(explosion);
		mSpaceship->AddBulletLife(500); // add 0.5 seconds, change to 1000 to make difference more noticable when testing
	}
	if (object->GetType() == GameObjectType("Alienship"))
	{
		mGameWorld->AddObject(explosion);
		mEnemyCount--;
		if (mEnemyCount <= 0 && mAsteroidCount <= 0) {
			SetTimer(500, START_NEXT_LEVEL);
		}
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (value == CREATE_NEW_PLAYER)
	{
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}

	if (value == START_NEXT_LEVEL)
	{
		mLevel++;

		int num_asteroids = 10 + 2 * mLevel;
		CreateAsteroids(num_asteroids);

		if (mLevel % 2 == 0) {
			CreateLifeUp(1);
		}

		if (mLevel % 3 == 0) {
			int num_Alienship = mLevel / 3;
			CreateAlienship(num_Alienship);
		}

		int roll = rand() % 101; // Random integer between 0 and 100

		if (roll >= 80) {
			CreateBulletLife(1);
		}
	}

	if (value == SHOW_GAME_OVER)
	{
		mGameOverLabel->SetVisible(true);
	}

}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////
shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);
	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	// Return the spaceship so it can be added to the world
	return mSpaceship;

}

void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.2f);
		mGameWorld->AddObject(asteroid);
	}
}

void Asteroids::CreateLifeUp(const uint num_LifeUp)
{
	for (uint i = 0; i < num_LifeUp; i++) {

		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("lifeup");
		shared_ptr<Sprite> lifeup_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		lifeup_sprite->SetLoopAnimation(true);

		shared_ptr<GameObject> lifeup = make_shared<LifeUp>();
		lifeup->SetBoundingShape(make_shared<BoundingSphere>(lifeup->GetThisPtr(), 1.0f));
		lifeup->SetSprite(lifeup_sprite);
		lifeup->SetScale(0.1f);
		//lifeup->SetRotation(180);
		lifeup->SetAngle(180);
		mGameWorld->AddObject(lifeup);
	}
}

void Asteroids::CreateBulletLife(const uint num_BulletLifeUp)
{
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("bulletlife");
	shared_ptr<Sprite> bulletlife_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	bulletlife_sprite->SetLoopAnimation(true);

	shared_ptr<GameObject> bulletlife = make_shared<BulletLifeUp>();
	bulletlife->SetBoundingShape(make_shared<BoundingSphere>(bulletlife->GetThisPtr(), 1.0f));
	bulletlife->SetSprite(bulletlife_sprite);
	bulletlife->SetScale(0.1f);
	//bulletlife->SetRotation(180);
	bulletlife->SetAngle(180);
	mGameWorld->AddObject(bulletlife);
}

void Asteroids::CreateAlienship(const uint num_Alienship)
{
	mEnemyCount = num_Alienship;
	for (uint i = 0; i < num_Alienship; i++) {

		Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("alienship");
		shared_ptr<Sprite> alienship_sprite = make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		alienship_sprite->SetLoopAnimation(true);

		shared_ptr<GameObject> alienship = make_shared<Alienship>();
		alienship->SetBoundingShape(make_shared<BoundingSphere>(alienship->GetThisPtr(), 4.0f));
		alienship->SetSprite(alienship_sprite);
		alienship->SetScale(0.2f);
		mGameWorld->AddObject(alienship);
	}
}

void Asteroids::CreateGUI()
{
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("SCORE: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	// Set the visibility of the label to false (hidden)
	mScoreLabel->SetVisible(false);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> score_component
		= static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

	mStartScreenTitle = shared_ptr<GUILabel>(new GUILabel("ASTEROIDS"));
	mStartScreenTitle->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mStartScreenTitle->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	shared_ptr<GUIComponent> title_component
		= static_pointer_cast<GUIComponent>(mStartScreenTitle);
	mGameDisplay->GetContainer()->AddComponent(title_component, GLVector2f(0.5f, 0.75f));

	mStartScreenLabel = shared_ptr<GUILabel>(new GUILabel("PRESS ENTER TO START"));
	mStartScreenLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mStartScreenLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	shared_ptr<GUIComponent> start_screen_component
		= static_pointer_cast<GUIComponent>(mStartScreenLabel);
	mGameDisplay->GetContainer()->AddComponent(start_screen_component, GLVector2f(0.5f, 0.25f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("LIVES: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Set the visibility of the label to false (hidden)
	mLivesLabel->SetVisible(false);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = shared_ptr<GUILabel>(new GUILabel("GAME OVER"));
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> game_over_component
		= static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));
}

void Asteroids::OnScoreChanged(int score)
{
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "SCORE: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "LIVES: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

	if (lives_left >= 0) 
	{ 
		SetTimer(1000, CREATE_NEW_PLAYER); 
	}
	else
	{
		SetTimer(500, SHOW_GAME_OVER);
	}
}

void Asteroids::AddLives(int lives_left) 
{
	std::ostringstream msg_stream;
	msg_stream << "LIVES: " << lives_left;
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);
	lives_left += 1;
}

shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation *anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}




