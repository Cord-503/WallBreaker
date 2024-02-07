#pragma once
#include <vector>
#include "raylib.h"
#include <string>
using namespace std;

#ifndef WALLBREAKER_H
#define WALLBREAKER_H

class WallBreaker
{
public:

#define MAX_LIVES 5
#define ROWS_OF_BRICKS 5
#define BRICKS_PER_ROW 8
#define GAP 12 // pixels
#define BRICK_HEIGHT 24

	typedef struct Player {
		Vector2 position; // it's the center
		Vector2 size;
		int curLife;

		Rectangle GetRect()
		{
			return Rectangle
			{ position.x - size.x / 2,
			  position.y - size.y / 2,
			  size.x,
			  size.y 
			};
		}

		void Draw()
		{
			DrawRectangleRounded(GetRect(), 0.3, 8, DARKGREEN);
		}

	} Player;

	typedef struct Ball {
		Vector2 position;
		Vector2 speed = Vector2{ 0, -3 };
		int radius = 8;
		bool active = false;


		void Draw()
		{
			DrawCircle(position.x, position.y, radius, MAROON);
		}
	} Ball;

	Color colors[ROWS_OF_BRICKS] = {RED, PURPLE, ORANGE, BLUE, GREEN };

	typedef struct Brick {
		Color color;
		Rectangle rect;
		void Draw() { DrawRectangleRounded(rect, 0.3f, 8, color); }
	} Brick;
	vector<Brick> bricks;

	Player player = { 0 };
	Ball ball = { 0 };

	bool gamePaused = false;
	bool gameOver = false;
	bool levelWon = false;

	static const int screenWidth = 800;
	static const int screenHeight = 450;

	void Main();
	void Start();
	void EvalCurFrame();
	void DrawCurFrame();
	void Update();

	bool CircleToRoundRectangleCollision(Vector2 circlePos, float radius, Rectangle rect, float roundness, Vector2& contactPoint, Vector2& contactNormal);

	struct PowerUp {
		Vector2 position;
		Vector2 speed;
		int type;
		bool active;

		void Draw() {
			if (type == 1) {
				DrawCircle(position.x, position.y, 10, GREEN);
			}
			else if (type == 2) {
				DrawCircle(position.x, position.y, 10, BLUE);
			}
		}
	};

	vector<PowerUp> powerUps;

	void ActivateSuperBallEffect();

	void UpdatePowerUps();

	bool superBallActive = false;

	// superball
	int superBallFrames = 0; 
	string powerUpText = "";
	int powerUpTextFrames = 0;


	Music bgm;
	Music hit;

};
#endif // !WALLBREAKER_H

