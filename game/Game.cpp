
#include "Game.h"

sf::Texture Game::cursor;
sf::RectangleShape Game::cursorTexture(Vector2f(31.f, 31.f));
RectangleShape Game::player(Vector2f(31.f, 31.f));
Texture Game::playerTexture;
Vector2f Game::playerVelocity(0.f, 0.f);

// Player data
float Game::playerSpeed = 150.0f;
const float Game::WALK_SPEED = 150.f;
float Game::currSpeed = Game::WALK_SPEED;
const float Game::distMuzzFromChar = 30.f;

// Controls
std::vector<Keyboard::Key> Game::myKeyBinds = { Keyboard::W, Keyboard::S, Keyboard::A, Keyboard::D };
std::vector<std::string> Game::ControlNames = { "Move Up", "Move Down", "Move Left", "Move Right" };
// Init test

// Bullet variables
Bullet Game::aBullet(4.f);
std::vector<Bullet> Game::bullets;

// Enemy variables
RectangleShape Game::enemy;
Texture Game::enemyTexture;

std::vector<RectangleShape> Game::enemies;
int Game::spawnCounter = 20;

// Vectors
Vector2f Game::playerCenter, Game::mousePosWindow, Game::aimDir, Game::aimDirNorm;

// Timing related
Clock Game::clockFT, Game::clockRoF, Game::clockFlash;	// clockFT is for measuring FRAMETIME, clockROF is to measure RATE OF FIRE
float Game::deltaTime;	// deltaTime is frametime;

// Gun related
size_t Game::currGun;
gunStats Game::currGunStats;
std::vector<gunStats> Game::listOfGunStats;
bool Game::triggerHeld;

void Game::PrepGuns(std::vector<gunStats>& listOfGunStats)
{
	listOfGunStats.clear();
	listOfGunStats.resize(NUM_OF_GUNS);

	// enum GUNS { BERETTA, GLOCK, REMINGTON, AA12, M14, AK47, NUM_OF_GUNS };

	listOfGunStats[BERETTA] = { 630.f, 1000.f, true, 10.f, false, 1 };
	listOfGunStats[GLOCK] = { 1100.f, 950.f, false, 15.f, false, 1 };
	listOfGunStats[REMINGTON] = { 80.f, 1200.f, true, 8.f, true, 7, };
	listOfGunStats[AA12] = { 300.f, 1000.f, false, 20.f, true, 6, };
	listOfGunStats[M14] = { 535.f, 2300.f, true, 3.5f, false, 1 };
	listOfGunStats[AK47] = { 600.f, 2600.f, false, 5.5f, false, 1 };

	/*
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
	*/
}

void Game::Initialize(const sf::RenderWindow& window)
{
	cursorTexture.setOrigin((cursorTexture.getSize() + Vector2f(1.f, 1.f)) / 2.f);
	cursor.loadFromFile("textures/crosshair.png");
	cursorTexture.setTexture(&cursor);

	playerTexture.loadFromFile("textures/player.png");	// UPDATE, just a debug
	player.setTexture(&playerTexture);
	player.setSize(Vector2f(100.f, 100.f));// Player char circle is colored white
	player.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);		// Initially, put player in the center
	player.setOrigin(player.getSize() / 2.f);

	aBullet.bulletFile.loadFromFile("textures/b2.png");
	aBullet.shape.setTexture(&aBullet.bulletFile);

	enemyTexture.loadFromFile("textures/BadGuy.png");	// UPDATE, just a debug
	enemy.setTexture(&enemyTexture);
	enemy.setFillColor(Color::Red);
	enemy.setSize(Vector2f(100.f, 100.f));

	PrepGuns(listOfGunStats);

	currGun = GUNS::AA12;
	currGunStats = listOfGunStats[currGun];
}

int Game::PollEvent(sf::RenderWindow& window, bool isFullscreen)
{
	Event event;

	while (window.pollEvent(event))
	{
		switch (event.type)
		{
		case Event::Closed:
			// window.close();
			// break;
			return -1;

		case Event::KeyReleased:

		case sf::Keyboard::Escape:
			return (0);
			break;

			if (event.key.code == Keyboard::Escape)
			{
				if (isFullscreen)
				{
					window.create(sf::VideoMode(1920, 1080), "360 Shooter!", sf::Style::Default);
					window.setMouseCursorVisible(true);
				}
				else
				{
					window.create(sf::VideoMode(1920, 1080), "360 Shooter!", sf::Style::Fullscreen);
					window.setMouseCursorVisible(true);
					isFullscreen = true;
				}
			}

			break;

		case Event::KeyPressed:
			switch (event.key.code)
			{
			case Keyboard::Num1:
				currGun = BERETTA;
				break;

			case Keyboard::Num2:
				currGun = GLOCK;
				break;

			case Keyboard::Num3:
				currGun = REMINGTON;
				break;

			case Keyboard::Num4:
				currGun = AA12;
				break;

			case Keyboard::Num5:
				currGun = M14;
				break;

			case Keyboard::Num6:
				currGun = AK47;
				break;
			}

			currGunStats = listOfGunStats[currGun];
			break;

		case Event::Resized:
			break;
		}
	}

	return 1;
}

sf::Vector2f GetPlayerCenter(const sf::RectangleShape& player)
{
	return sf::Vector2f(player.getPosition().x, player.getPosition().y);
}

sf::Vector2f GetAimDirection(const sf::RenderWindow& window, const sf::Vector2f playerCenter)
{
	auto mousePosWindow = sf::Vector2f(Mouse::getPosition(window));
	auto aimDir = mousePosWindow - playerCenter;
	return aimDir / sqrt(pow(aimDir.x, 2) + pow(aimDir.y, 2));
}

constexpr float __CRTDECL ToDeg(float angleInDeg)
{
	return angleInDeg * 180.f / PI;
}

constexpr float __CRTDECL ToRad(float angleInDeg)
{
	return angleInDeg * PI / 180.f;
}

float __CRTDECL GetRotationAngle(const sf::Vector2f& aimDirection)
{
	return ToDeg(atan2f(aimDirection.y, aimDirection.x));
}

sf::Vector2f NewPlayerVelocity(float speed, float time, bool moveLeft, bool moveRight, bool moveUp, bool moveDown)
{
	auto vel = INIT_PLAYER_VELOCITY;

	// Update the current player velocity
	if (moveLeft)
		vel.x += -speed * time;

	if (moveRight)
		vel.x += speed * time;

	if (moveUp)
		vel.y += -speed * time;

	if (moveDown)
		vel.y += speed * time;

	return vel;
}

float GetCurrentFireAngle(float spreadAngleDeg)
{
	return rand() % (int(spreadAngleDeg * 100) / 100) - (int(spreadAngleDeg * 50) / 100) - 0.5f;
}

float GetCurrentFireAngleRad(float spreadAngleDeg)
{
	return ToRad(GetCurrentFireAngle(spreadAngleDeg));
}

sf::Vector2f GetFireRotationVector(const sf::Vector2f& aim, float angle)
{
	return sf::Vector2f(aim.x * cosf(angle) - aim.y * sinf(angle), aim.x * sinf(angle) + aim.y * cosf(angle));
}

bool ObjectOutOfBounds(const sf::RenderWindow& window, const sf::Vector2f& pos, const sf::Vector2f& size)
{
	return pos.x < 0 - size.x * 2 || pos.x > window.getSize().x + size.x * 2
		|| pos.y < 0 - size.y * 2 || pos.y > window.getSize().y + size.y * 2;
}

bool BulletCollidesWithObject(const Bullet& myBullet, const sf::RectangleShape& object)
{
	return myBullet.shape.getGlobalBounds().intersects(object.getGlobalBounds());
}

void Game::UpdatePlayerAndCursor(const sf::RenderWindow& window)
{
	cursorTexture.setPosition(sf::Vector2f(sf::Mouse::getPosition(window)));

	// GAME LOGIC
	// Update

	playerCenter = GetPlayerCenter(player);
	aimDirNorm = GetAimDirection(window, playerCenter);
	player.setRotation(GetRotationAngle(aimDirNorm));

	// // ~~~ DEBUG
	// std::cout << aimDirNorm.x << " " << aimDirNorm.y << "\t\tdT = " << deltaTime << std::endl;

	bool left = Keyboard::isKeyPressed(myKeyBinds[MOVE_LEFT]);
	bool right = Keyboard::isKeyPressed(myKeyBinds[MOVE_RIGHT]);
	bool up = Keyboard::isKeyPressed(myKeyBinds[MOVE_UP]);
	bool down = Keyboard::isKeyPressed(myKeyBinds[MOVE_DOWN]);

	// Movement
	// Detect diagonal movements and counteract Pythagorean multiplier if necessary
	if ((left || right) && (up || down))
		playerSpeed = currSpeed * DIAGONAL_COEFF;
	else
		playerSpeed = currSpeed;

	// Update the player velocity
	playerVelocity = NewPlayerVelocity(playerSpeed, deltaTime, left, right, up, down);

	// Move the player to their velocity
	player.move(playerVelocity);
}

void Game::UpdateBullets(const sf::RenderWindow& window)
{
	sf::Vector2f aSize;
	sf::Vector2f bulletPos;

	// Update bullets
	for (size_t i = 0; i < bullets.size(); i++)
	{
		// Update bullet position via its respective velocity
		bullets[i].shape.move(bullets[i].currVelocity);

		aSize = aBullet.shape.getPosition();
		bulletPos = bullets[i].shape.getPosition();

		// If bullet is out of bounds (window)
		if (ObjectOutOfBounds(window, bulletPos, aSize))
		{
			// Then delete them to free memory
			bullets.erase(bullets.begin() + i);
		}
		else
		{
			// Enemy collision
			for (size_t k = 0; k < enemies.size(); k++)
			{
				if (BulletCollidesWithObject(bullets[i], enemies[k]))
				{
					bullets.erase(bullets.begin() + i);
					enemies.erase(enemies.begin() + k);
					//printf("you just hit enemy %i\n", k);
					break;
				}
			}
		}
	}
}

void Game::FireGun()
{
	Vector2f currVector;
	float currAngleRad;

	// Pick a random spread for the bullet
	currAngleRad = GetCurrentFireAngleRad(currGunStats.spreadDegrees);

	// Rotate vector of bullet direction
	currVector = GetFireRotationVector(aimDirNorm, currAngleRad);

	// Initially set the position of the bullet
	aBullet.shape.setPosition(playerCenter);
	aBullet.shape.setRotation(GetRotationAngle(currVector));
	aBullet.shape.move(aimDirNorm * distMuzzFromChar);
	aBullet.currVelocity = currVector * currGunStats.muzzVelocity * deltaTime + playerVelocity;
	bullets.push_back(Bullet(aBullet));
	// ADD ROTATION OF BULLET TO POINT IN SPECIFIC DIRECTION BC WE'LL REPLACE IT WITH A RECTANGLE SHAPE 

	clockRoF.restart().Zero;
	triggerHeld = true;
}

void Game::FireShotgun()
{
	Vector2f currVector;
	float currAngle, currAngleRad;

	float degBetweenPellets = currGunStats.spreadDegrees / (currGunStats.numPellets - 1);

	for (size_t currPellet = 0; currPellet < currGunStats.numPellets; currPellet++)
	{
		currAngle = -currGunStats.spreadDegrees / 2.f;

		currAngleRad = ToRad(currAngle);
		currVector = GetFireRotationVector(aimDirNorm, currAngleRad);

		// Rotate vector formula
		auto bulletVelocity = currVector * currGunStats.muzzVelocity * deltaTime + playerVelocity;

		aBullet.shape.setPosition(playerCenter);
		aBullet.shape.setRotation(GetRotationAngle(aimDirNorm));
		aBullet.shape.move(aimDirNorm * distMuzzFromChar);
		aBullet.currVelocity = bulletVelocity;
		bullets.push_back(Bullet(aBullet));

		// currAngle = -currGunStats.spreadDegrees / 2.f;
		auto temp = (int)(currGunStats.spreadDegrees * 10) / 70;

		// fix later?
		if (currPellet == currGunStats.numPellets - 1)
			currAngle = currGunStats.spreadDegrees / 2.f;
		else
			currAngle += degBetweenPellets + (rand() % temp - temp);

		clockRoF.restart().Zero;
		triggerHeld = true;
	}	// End for-loop for each shotgun pellet
}

int Game::GameSequence(sf::RenderWindow& window, bool isFullscreen)
{
	sf::Event event;
	bool Running = true;

	while (Running)
	{
		//Verifying events
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				return (-1);

			case sf::Event::KeyPressed:
				switch (event.key.code)
				{
				case sf::Keyboard::Escape:
					return (0);
					break;

				case sf::Keyboard::Num1:
					currGun = GUNS::BERETTA;
					break;

				case sf::Keyboard::Num2:
					currGun = GUNS::GLOCK;
					break;

				case sf::Keyboard::Num3:
					currGun = GUNS::REMINGTON;
					break;

				case sf::Keyboard::Num4:
					currGun = GUNS::AA12;
					break;

				case sf::Keyboard::Num5:
					currGun = GUNS::M14;
					break;

				case sf::Keyboard::Num6:
					currGun = GUNS::AK47;
					break;
				}

				currGunStats = listOfGunStats[currGun];
				break;

			case Event::Resized:
				break;
			}
		}

		deltaTime = clockFT.restart().asSeconds();
		UpdatePlayerAndCursor(window);

		// Enemy update
		if (spawnCounter < 10)
			spawnCounter++;

		if (spawnCounter >= 10 && enemies.size() <= 15)
		{
			enemy.setPosition(Vector2f(rand() % window.getSize().x, rand() % window.getSize().y));
			enemies.push_back(RectangleShape(enemy));
			spawnCounter = 0;
		}

		// Shooting - check if the trigger WAS JUST held down before we check for shooting
		if (!Mouse::isButtonPressed(Mouse::Left))
			triggerHeld = false;

		// If we aren't in the specific case that it is a semi-auto, and we haven't held the trigger down, then WE CAN SHOOTTTT
		if (!(currGunStats.isSemiAuto && triggerHeld))
		{
			// If the trigger (LMB) is pressed and there's been enough elapsed time since the previous shot
			if (clockRoF.getElapsedTime().asSeconds() >= currGunStats.currFireTime && Mouse::isButtonPressed(Mouse::Left))
			{
				if (currGunStats.isShotgun)
				{
					// If it fires one bullet at a time
					FireShotgun();
				}
				else
				{
					// Otherwise, if it fires multiple pellets like a shotgun (rework this boolean to an enum later to add lasers and other weird bullets)
					FireGun();
				}	// End if-else statement for shooting
			}	// End firetime if-statement
		}	// End semi-auto check

		UpdateBullets(window);

		// FINISHED GAME LOGIC

		// DRAW EVERYTHING (window)

		// Clear first
		window.clear();

		// Draw enemies
		for (auto enemy : enemies)
			window.draw(enemy);

		// Draw every bullet
		for (auto bullet : bullets)
			window.draw(bullet.shape);

		// Then draw player (on top of bullets)
		window.draw(player);
		window.draw(cursorTexture);
		window.display();
	}

	//Never reaching this point normally, but just in case, exit the application
	return -1;
}
