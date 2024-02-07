#include "WallBreaker.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
using namespace std;

//float cornerRadius = 15.0f;
float deltaTime = GetFrameTime() * 50;

void WallBreaker::Main()
{
	InitWindow(screenWidth, screenHeight, "Wall Breaker");

	InitAudioDevice();
	// Load BGM
	bgm = LoadMusicStream("resources/Bgm.mp3");
	hit = LoadMusicStream("resources/hit.mp3");
	PlayMusicStream(bgm);

	Start();

	SetTargetFPS(60); // This will become important
	while (!WindowShouldClose())
	{
		UpdateMusicStream(bgm);
		Update();
	}

	UnloadMusicStream(bgm);
	CloseAudioDevice();
	CloseWindow();
}

void WallBreaker::Start()
{
	srand((int)time(NULL));

#pragma region BRICKS
	float spaceForBricks = screenWidth - (BRICKS_PER_ROW * GAP + GAP);
	Vector2 brickSize = Vector2{ spaceForBricks / BRICKS_PER_ROW, BRICK_HEIGHT };

	bricks.clear();

	for (int row = 0; row < ROWS_OF_BRICKS; row++)
	{
		for (int col = 0; col < BRICKS_PER_ROW; col++)
		{
			if (rand() % 4 == 0) continue;
			float x = GAP + (GAP + brickSize.x) * col;
			float y = GAP + (GAP + brickSize.y) * row;

			Rectangle rect = Rectangle{ x, y, brickSize.x, brickSize.y };
			
			Brick brick = Brick{ colors[row], rect};

			bricks.push_back(brick);
		}
	}
#pragma endregion 

#pragma region PLAYER
	player.position = Vector2{ screenWidth / 2, screenHeight * 9 / 10 };
	player.size = Vector2{ screenWidth / 10 , 20 };
	player.curLife = MAX_LIVES; // at the beginning
#pragma endregion
	ball.position = Vector2{ screenWidth / 2, screenHeight * 9 / 10 - 30 };
}

bool WallBreaker::CircleToRoundRectangleCollision(Vector2 circlePos, float radius, Rectangle rect, float roundness, Vector2& contactPoint, Vector2& contactNormal) {

	float w = rect.width - 2 * roundness;
	float h = rect.height - 2 * roundness;
	Vector2 rectCenter = { rect.x + roundness + w / 2, rect.y + roundness + h / 2 };

	Vector2 closestPoint = {
		max(rectCenter.x - w / 2, min(circlePos.x, rectCenter.x + w / 2)),
		max(rectCenter.y - h / 2, min(circlePos.y, rectCenter.y + h / 2))
	};

	Vector2 dif = { circlePos.x - closestPoint.x, circlePos.y - closestPoint.y };
	float distance = sqrtf(dif.x * dif.x + dif.y * dif.y);


	if (distance < radius) {
		Vector2 normalDif = { dif.x / distance, dif.y / distance };
		contactPoint = circlePos;
		contactNormal = normalDif;

		float speed = sqrtf(ball.speed.x * ball.speed.x + ball.speed.y * ball.speed.y);

		float dotProduct = ball.speed.x * contactNormal.x + ball.speed.y * contactNormal.y;
		Vector2 reflection = {
			ball.speed.x - 2 * dotProduct * contactNormal.x,
			ball.speed.y - 2 * dotProduct * contactNormal.y
		};

		float reflectionM = sqrtf(reflection.x * reflection.x + reflection.y * reflection.y);
		Vector2 normalRef = {
			reflection.x / reflectionM,
			reflection.y / reflectionM
		};
		ball.speed = {
			normalRef.x * speed * -1 * deltaTime,
			normalRef.y * speed * -1 * deltaTime
		};

		Vector2 brickCenter = { rect.x + rect.width / 2, rect.y + rect.height / 2 };
		float distanceToCenter = circlePos.x - brickCenter.x;

		
		if (contactNormal.x != 0) { 
			ball.speed.x = (contactNormal.x > 0 ? -1 : 1) * fabs(ball.speed.x);
		}
		if (contactNormal.y != 0) { 
			ball.speed.y = (contactNormal.y > 0 ? -1 : 1) * fabs(ball.speed.y);
		}


		float changeSpeed = sqrtf(ball.speed.x * ball.speed.x + ball.speed.y * ball.speed.y);
		ball.speed = {
			ball.speed.x / changeSpeed * speed,
			ball.speed.y / changeSpeed * speed
		};

		return true;
	}
	return false;
}

void WallBreaker::EvalCurFrame()
{
	if (gameOver)
	{
		if (IsKeyPressed(KEY_ENTER))
		{
			Start();
			gameOver = false;
		}
		return;
	}

	if (levelWon)
	{
		if (IsKeyPressed(KEY_ENTER))
		{
			Start();
			levelWon = false;
		}
		return;
	}

	if (IsKeyPressed(KEY_P))
		gamePaused = !gamePaused;

	if (gamePaused) return;


	if (!ball.active)
	{
		ball.position = Vector2{ player.position.x, screenHeight * 9 / 10 - 30 };

		if (IsKeyPressed(KEY_SPACE))
		{
			ball.active = true;
			ball.speed = Vector2{ 0, -300 * deltaTime };
		}
	}
	else
	{
		ball.position.x += ball.speed.x;
		ball.position.y += ball.speed.y;
	}

	// player position
	if (IsKeyDown(KEY_LEFT))
		player.position.x -= 5;

	// we have reached the far left
	if (player.position.x - player.size.x / 2 <= 0)
		player.position.x = player.size.x / 2; // stick it to the far left

	// we have reached the far right
	if (player.position.x + player.size.x / 2 >= screenWidth)
		player.position.x = screenWidth - player.size.x / 2; // stick it to the far right

	if (IsKeyDown(KEY_RIGHT))
		player.position.x += 5;

	if (superBallActive) {
		for (int i = 0; i < bricks.size(); i++) {
			if (CheckCollisionCircleRec(ball.position, ball.radius, bricks[i].rect)) {
				bricks.erase(bricks.begin() + i);
				i--;
			}
		}
	}
	else {
		// Collision with the Bricks
		Vector2 contactPoint, contactNormal;
		for (int i = 0; i < bricks.size(); i++) {
			if (CircleToRoundRectangleCollision(ball.position, ball.radius, bricks[i].rect, 0.3f, contactPoint, contactNormal)) {
				// Collision handling
				if (rand() % 2 == 0) { 
					PowerUp newPowerUp = {
						Vector2{ bricks[i].rect.x + bricks[i].rect.width / 2, bricks[i].rect.y + bricks[i].rect.height },
						Vector2{ 0, 1.5f },
						rand() % 2 + 1,
						true
					};
					powerUps.push_back(newPowerUp);
				}

				bricks.erase(bricks.begin() + i); // delete the brick

				// Adjust ball speed based on contact normal
				if (fabs(contactNormal.x) > 0.1) ball.speed.x *= -1;
				if (fabs(contactNormal.y) > 0.1) ball.speed.y *= -1;

				break;
			}
		}
	}

	// Colision with the pedal
	if (CheckCollisionCircleRec(ball.position, ball.radius, player.GetRect())) {
		
		if (ball.speed.y > 0) // we are going downwards
		{
			ball.speed.y *= -1;
			ball.speed.x = (ball.position.x - player.position.x) / (player.size.x / 4);
		}
	}

	// collision with the walls
	if ((ball.position.x + ball.radius >= screenWidth) || ball.position.x - ball.radius <= 0)
		ball.speed.x *= -1;
	if (ball.position.y - ball.radius <= 0) // we hit the top
		ball.speed.y *= -1;
	if (ball.position.y + ball.radius >= screenHeight)
	{
		player.curLife--;
		ball.active = false;
		ball.speed = Vector2{ 0 , 0 };
	}
	if (player.curLife == 0)
		gameOver = true;
	else
	{
		if (bricks.size() == 0)
			levelWon = true;
	}
}

void WallBreaker::DrawCurFrame()
{
	BeginDrawing();
	ClearBackground(BLACK);

	if (gameOver)
	{
		DrawText("Press Enter to play again",
			GetScreenWidth() / 2 - MeasureText("Press Enter to play again", 30) / 2,
			GetScreenHeight() / 2 - 15,
			30, GRAY);
	}
	else if (levelWon)
	{
		DrawText("You Won! Press Enter to go to next level!",
			GetScreenWidth() / 2 - MeasureText("You Won! Press Enter to go to next level!", 30) / 2,
			GetScreenHeight() / 2 - 15,
			30, GRAY);
	}
	else
	{
		player.Draw();
		ball.Draw();

		for (Brick b : bricks)
			b.Draw();

		// draw player lives!
		for (int i = 0; i < MAX_LIVES; i++)
		{
			if (i < player.curLife)
				DrawRectangle(10 + 40 * i, screenHeight - 20, 30, 10, LIGHTGRAY);
			else
				DrawRectangle(10 + 40 * i, screenHeight - 20, 30, 10, GRAY);
		}
	}

	for (auto& powerUp : powerUps) {
		powerUp.Draw();
	}

	if (!powerUpText.empty()) {
		// Show the name of the item
		DrawText(powerUpText.c_str(), screenWidth / 2 - MeasureText(powerUpText.c_str(), 20) / 2, screenHeight / 2, 20, RED);
	}

	EndDrawing();
}

void WallBreaker::Update() {
	

	float previousTime = GetTime();
	EvalCurFrame();
	UpdatePowerUps(); 

	if (superBallActive) {
		if (++superBallFrames >= 900) { // 900 frames = 5s
			superBallActive = false;
			superBallFrames = 0;
		}
	}

	if (!powerUpText.empty()) {
		if (++powerUpTextFrames >= 270) { // 1.5s
			powerUpText = "";
			powerUpTextFrames = 0;
		}
	}

	DrawCurFrame();
	float currentTime = GetTime();
	deltaTime = currentTime - previousTime;
}


void WallBreaker::UpdatePowerUps() {
	for (int i = 0; i < powerUps.size(); i++) {
		powerUps[i].position.y += powerUps[i].speed.y;

		if (CheckCollisionCircleRec(powerUps[i].position, 10, player.GetRect())) {
			
			if (powerUps[i].type == 1) {
				player.curLife = min(player.curLife + 1, MAX_LIVES);
				powerUpText = "life++";
				powerUpTextFrames = 0;
			}
			else if (powerUps[i].type == 2) {
				ActivateSuperBallEffect();
			}
			else if (powerUps[i].type == 3) {
				// EMPTY
			}

			powerUps.erase(powerUps.begin() + i);
			i--;
		}
		else if (powerUps[i].position.y > screenHeight) { // 如果道具落出屏幕下方，也将其删除
			powerUps.erase(powerUps.begin() + i);
			i--;
		}
	}
}

void WallBreaker::ActivateSuperBallEffect() {
	superBallActive = true;
	superBallFrames = 0;
	powerUpText = "SuperBall Activated!";
	powerUpTextFrames = 0;
}
