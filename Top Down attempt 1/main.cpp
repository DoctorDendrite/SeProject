#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <Windows.h>

using namespace sf;

static const float PI = 3.141592654f;

class Bullet
{
public:
	RectangleShape shape;
	Vector2f currVelocity;
	sf::Texture bulletFile;

	Bullet(float height = 4.f)
		: currVelocity(0.f, 0.f)
	{			
		this->shape.setOrigin(Vector2f(this->shape.getPosition().x + (height * 3 / 2), this->shape.getPosition().y + (height / 2)));
		this->shape.setSize(Vector2f(height * 3, height));
		//this->shape.setFillColor(Color::Yellow);
	}	
};


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


void prepGuns(std::vector<gunStats> &listOfGunStats);




int main()
{
	// Temp rand (use better formula)
	srand(time(NULL));
	
	bool isFullscreen = true;

	RenderWindow window(VideoMode(1920, 1080), "360 Shooter!", Style::Default | sf::Style::Fullscreen);
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(false);
	window.setMouseCursorVisible(false);
	
	sf::Texture cursor;
	sf::RectangleShape cursorTexture(Vector2f(31.f, 31.f));
	cursorTexture.setOrigin((cursorTexture.getSize() + Vector2f(1.f, 1.f)) / 2.f);
	cursor.loadFromFile("textures/crosshair.png");
	cursorTexture.setTexture(&cursor);


	// PLAYER variables

	// Drawing
	RectangleShape player(Vector2f(31.f, 31.f));
	Texture playerTexture;
	playerTexture.loadFromFile("textures/player.png");	// UPDATE, just a debug
	player.setTexture(&playerTexture);
	//player.setFillColor(Color::Blue);	
	player.setSize(Vector2f(100.f, 100.f));// Player char circle is colored white
	player.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);		// Initially, put player in the center
	player.setOrigin(player.getSize() / 2.f);
	Vector2f playerVelocity(0.f, 0.f);

	// Player data
	float playerSpeed = 150.0f;
	const float WALK_SPEED = 150.f;
	float currSpeed = WALK_SPEED;
	const float distMuzzFromChar = 30.f;
	
	// Controls
	std::vector<Keyboard::Key> myKeyBinds = { Keyboard::W, Keyboard::S, Keyboard::A, Keyboard::D  };
	enum CONTROLS { MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT };
	std::vector<std::string> ControlNames = { "Move Up", "Move Down", "Move Left", "Move Right" };
	// Init test


	// Bullet variables
	Bullet aBullet(4.f);
	std::vector<Bullet> bullets;

	aBullet.bulletFile.loadFromFile("textures/b2.png");
	aBullet.shape.setTexture(&aBullet.bulletFile);



	// Enemy variables
	RectangleShape enemy;
	Texture enemyTexture;
	enemyTexture.loadFromFile("textures/BadGuy.png");	// UPDATE, just a debug
	enemy.setTexture(&enemyTexture);
	enemy.setFillColor(Color::Red);
	enemy.setSize(Vector2f(100.f, 100.f));

	std::vector<RectangleShape> enemies;
	int spawnCounter = 20;


	// Vectors
	Vector2f playerCenter, mousePosWindow, aimDir, aimDirNorm;


	// Timing related
	Clock clockFT, clockRoF, clockFlash;	// clockFT is for measuring FRAMETIME, clockROF is to measure RATE OF FIRE
	float deltaTime;	// deltaTime is frametime;


	// Gun related
	size_t currGun;
	gunStats currGunStats;
	std::vector<gunStats> listOfGunStats;
	prepGuns(listOfGunStats);

	// enum GUNS { BERETTA, GLOCK, REMINGTON, AA12, M14, AK47, NUM_OF_GUNS };
	currGun = AA12;
	currGunStats = listOfGunStats[currGun];
	bool triggerHeld = false;

	while (window.isOpen())
	{
		deltaTime = clockFT.restart().asSeconds();
		
		Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case Event::Closed:
					window.close();
					break;

				case Event::KeyReleased:
					if (event.key.code == Keyboard::Escape)
					{
						if (isFullscreen)
						{
							window.create(sf::VideoMode(1920, 1080), "360 Shooter!", sf::Style::Default);
							window.setMouseCursorVisible(true);
							isFullscreen = false;
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
						//printf("test\n");
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
		
		cursorTexture.setPosition(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

		// GAME LOGIC
		// Update
		playerCenter = Vector2f(player.getPosition().x , player.getPosition().y);
		mousePosWindow = Vector2f(Mouse::getPosition(window));
		aimDir = mousePosWindow - playerCenter;
		aimDirNorm = aimDir / sqrt(pow(aimDir.x, 2) + pow(aimDir.y, 2));

		player.setRotation(atan2f(aimDirNorm.y, aimDirNorm.x) * 180.f / PI);
		
		// ~~~ DEBUG
		//std::cout << aimDirNorm.x << " " << aimDirNorm.y << "\t\tdT = " << deltaTime << std::endl;

		// Movement
		// Detect diagonal movements and counteract Pythagorean multiplier if necessary
		if ((Keyboard::isKeyPressed(myKeyBinds[MOVE_LEFT]) || Keyboard::isKeyPressed(myKeyBinds[MOVE_RIGHT]))
			&& (Keyboard::isKeyPressed(myKeyBinds[MOVE_UP]) || Keyboard::isKeyPressed(myKeyBinds[MOVE_DOWN])))
			playerSpeed = currSpeed * sinf(45.f * PI/180);
		else
			playerSpeed = currSpeed;

		// Reset the velocity
		playerVelocity = Vector2f(0.f, 0.f);

		// Update the current player velocity
		if (Keyboard::isKeyPressed(myKeyBinds[MOVE_LEFT]))
			playerVelocity.x += -playerSpeed * deltaTime;
		if (Keyboard::isKeyPressed(myKeyBinds[MOVE_RIGHT]))
			playerVelocity.x += playerSpeed * deltaTime;
		if (Keyboard::isKeyPressed(myKeyBinds[MOVE_UP]))
			playerVelocity.y += -playerSpeed * deltaTime;
		if (Keyboard::isKeyPressed(myKeyBinds[MOVE_DOWN]))
			playerVelocity.y += playerSpeed* deltaTime;
		
		// Move the player to their velocity
		player.move(playerVelocity);


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
				Vector2f currVector;
				float currAngle, currAngleRad;

				// If it fires one bullet at a time
				if (!currGunStats.isShotgun)
				{
					// Pick a random spread for the bullet
					currAngle = rand() % (int(currGunStats.spreadDegrees * 100) / 100) - int(currGunStats.spreadDegrees * 50) / 100 - 0.5f;
					//printf("currAngle: %f\n", currAngle);
					currAngleRad = currAngle * PI / 180.f;

					// Rotate vector of bullet direction
					currVector.x = aimDirNorm.x * cosf(currAngleRad) - aimDirNorm.y * sinf(currAngleRad);
					currVector.y = aimDirNorm.x * sinf(currAngleRad) + aimDirNorm.y * cosf(currAngleRad);
					
					// Initially set the position of hte bullet
					aBullet.shape.setPosition(playerCenter);					
					aBullet.shape.setRotation(atan2f(currVector.y, currVector.x) * 180.f / PI);
					aBullet.shape.move(aimDirNorm * distMuzzFromChar);
					aBullet.currVelocity = currVector * currGunStats.muzzVelocity * deltaTime + playerVelocity;
					bullets.push_back(Bullet(aBullet));
					// ADD ROTATION OF BULLET TO POINT IN SPECIFIC DIRECTION BC WE'LL REPLACE IT WITH A RECTANGLE SHAPE 

					clockRoF.restart().Zero;
					triggerHeld = true;
				}
				// Otherwise, if it fires multiple pellets like a shotgun (rework this boolean to an enum later to add lasers and other weird bullets)
				else
				{
					float degBetweenPellets = currGunStats.spreadDegrees / (currGunStats.numPellets - 1);
										
					currAngle = -currGunStats.spreadDegrees / 2.f;
					currAngleRad = currAngle * PI / 180.f;

					for (size_t currPellet = 0; currPellet < currGunStats.numPellets; currPellet++)
					{
						// Rotate vector formula
						currVector.x = aimDirNorm.x * cosf(currAngleRad) - aimDirNorm.y * sinf(currAngleRad);
						currVector.y = aimDirNorm.x * sinf(currAngleRad) + aimDirNorm.y * cosf(currAngleRad);
												
						aBullet.shape.setPosition(playerCenter);
						aBullet.shape.setRotation(atan2f(aimDirNorm.y, aimDirNorm.x) * 180.f / PI);
						aBullet.shape.move(aimDirNorm * distMuzzFromChar);
						aBullet.currVelocity = currVector * currGunStats.muzzVelocity * deltaTime + playerVelocity;
						bullets.push_back(Bullet(aBullet));	

						// fix later?
						if(currPellet == currGunStats.numPellets - 1)
							currAngle = currGunStats.spreadDegrees / 2.f;
						else
							currAngle += degBetweenPellets + (rand() % int(currGunStats.spreadDegrees * 10) / 70 - (int)(currGunStats.spreadDegrees * 10 / 70));
						
						currAngleRad = currAngle * PI / 180.f;
						clockRoF.restart().Zero;
						triggerHeld = true;

					}	// End for-loop for each shotgun pellet

				}	// End if-else statement for shooting
			}	// End firetime if-statement
		}	// End semi-auto check


		// Update bullets
		for (size_t i = 0; i < bullets.size(); i++)
		{
			// Update bullet position via its respective velocity
			bullets[i].shape.move(bullets[i].currVelocity);

			// If bullet is out of bounds (window)
			if (bullets[i].shape.getPosition().x < 0 - aBullet.shape.getSize().x * 2 || bullets[i].shape.getPosition().x > window.getSize().x + aBullet.shape.getSize().x * 2
				|| bullets[i].shape.getPosition().y < 0 - aBullet.shape.getSize().y * 2 || bullets[i].shape.getPosition().y > window.getSize().y + aBullet.shape.getSize().y * 2)
			{
				// Then delete them to free memory
				bullets.erase(bullets.begin() + i);			
			}
			else
			{
				// Enemy collision
				for (size_t k = 0; k < enemies.size(); k++)
				{
					if (bullets[i].shape.getGlobalBounds().intersects(enemies[k].getGlobalBounds()))
					{
						bullets.erase(bullets.begin() + i);
						enemies.erase(enemies.begin() + k);
						//printf("you just hit enemy %i\n", k);
						break;
					}
				}
			}		
		}

		//
		//for (size_t i = 0; i < bullets.size(); i++)
			//printf("bul size: %i\n", bullets.size());


		// FINISHED GAME LOGIC

		// DRAW EVERYTHING (window)
		// Clear first
		window.clear();

		// Draw enemies
		for (size_t i = 0; i < enemies.size(); i++)
		{
			window.draw(enemies[i]);
		}

		// Draw every bullet
		for (size_t i = 0; i < bullets.size(); i++)
			window.draw(bullets[i].shape);

		// Then draw player (on top of bullets)
		window.draw(player);
		window.draw(cursorTexture);
		window.display();


	}


	return 0;
}


void prepGuns(std::vector<gunStats> &listOfGunStats)
{
	listOfGunStats.clear();
	
	listOfGunStats.resize(NUM_OF_GUNS);

	// enum GUNS { BERETTA, GLOCK, REMINGTON, AA12, M14, AK47, NUM_OF_GUNS };

	listOfGunStats[BERETTA] = { 630.f, 1000.f, true , 10.f };
	listOfGunStats[GLOCK] = { 1100.f, 950.f, false , 15.f };
	listOfGunStats[REMINGTON] = { 80.f, 1200.f, true, 8.f, true, 7,  };
	listOfGunStats[AA12] = { 300.f, 1000.f, false, 20.f, true, 6,  };
	listOfGunStats[M14] = { 535.f, 2300.f, true, 3.5f };
	listOfGunStats[AK47] = { 600.f, 2600.f, false, 5.5f, };


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
