#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include <iostream>
#define debug(name, value) std::cout << name << ": [" << value << "]\n";

using namespace sf;

static const float PI = 3.141592654f;
static const float DIAGONAL_COEFF = sinf(45.f * PI / 180);
static const sf::Vector2f INIT_PLAYER_VELOCITY(0.f, 0.f);

// Universal gun data
enum GUNS { BERETTA, GLOCK, REMINGTON, AA12, M14, AK47, NUM_OF_GUNS };

struct gunStats
{
	// Universal stats
	//float damage;
	float currRPM, muzzVelocity;
	bool isSemiAuto;
	float spreadDegrees;

	// Shotgun-specific
	bool isShotgun;
	size_t numPellets;

	// Auto calculate
	float currFireTime = 60.f / currRPM;
};

class Bullet
{
public:
	sf::RectangleShape shape;
	sf::Vector2f currVelocity;
	sf::Texture bulletFile;

	Bullet(float height = 4.f)
		: currVelocity(0.f, 0.f)
	{
		this->shape.setOrigin(sf::Vector2f(this->shape.getPosition().x + (height * 3 / 2), this->shape.getPosition().y + (height / 2)));
		this->shape.setSize(sf::Vector2f(height * 3, height));
		//this->shape.setFillColor(Color::Yellow);
	}
};

class Game
{
private:
	static sf::Texture cursor;
	static sf::RectangleShape cursorTexture;
	static RectangleShape player;
	static Texture playerTexture;
	static Vector2f playerVelocity;

	// Player data
	static float playerSpeed;
	static const float WALK_SPEED;
	static float currSpeed;
	static const float distMuzzFromChar;

	// Controls
	static std::vector<Keyboard::Key> myKeyBinds;
	enum CONTROLS { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };
	static std::vector<std::string> ControlNames;
	// Init test

	// Bullet variables
	static Bullet aBullet;
	static std::vector<Bullet> bullets;

	// Enemy variables
	static RectangleShape enemy;
	static Texture enemyTexture;

	static std::vector<RectangleShape> enemies;
	static int spawnCounter;

	// Vectors
	static Vector2f playerCenter, mousePosWindow, aimDir, aimDirNorm;

	// Timing related
	static Clock clockFT, clockRoF, clockFlash;	// clockFT is for measuring FRAMETIME, clockROF is to measure RATE OF FIRE
	static float deltaTime;	// deltaTime is frametime;

	// Gun related
	static size_t currGun;
	static gunStats currGunStats;
	static std::vector<gunStats> listOfGunStats;
	static bool triggerHeld;

public:
	static void PrepGuns(std::vector<gunStats>& listOfGunStats);
	static void Initialize(const sf::RenderWindow& window);
	static void PollEvent(sf::RenderWindow& window, bool isFullscreen);
	static void UpdatePlayerAndCursor(const sf::RenderWindow& window);
	static void UpdateBullets(const sf::RenderWindow& window);
	static void GameSequence(sf::RenderWindow& window, bool isFullscreen);

	static void FireGun();
	static void FireShotgun();
};

sf::Vector2f GetPlayerCenter(const sf::RectangleShape& player);
sf::Vector2f GetAimDirection(const sf::RenderWindow& window, const sf::Vector2f playerCenter);
float __CRTDECL GetRotationAngle(const sf::Vector2f& aimDirection);
sf::Vector2f NewPlayerVelocity(float speed, float time, bool moveLeft, bool moveRight, bool moveUp, bool moveDown);

float GetCurrentFireAngleRad(float spreadAngleDeg);
sf::Vector2f GetFireRotationVector(const sf::Vector2f& aim, float angle);

bool ObjectOutOfBounds(const sf::RenderWindow& window, const sf::Vector2f& pos, const sf::Vector2f& size);
bool BulletCollidesWithObject(const Bullet& myBullet, const sf::RectangleShape& object);
