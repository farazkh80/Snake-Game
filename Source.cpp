#include <iostream>
#include <list>
#include <thread>
using namespace std;

#include <Windows.h>

int nScreenWidth = 120;
int nScreenHeight = 30;

struct sSnakeSegment {
	int x;
	int y;
};


int main() {

	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;
	while (1) {
		list<sSnakeSegment> snake = { {60,15},{61,15},{62,15},{63,15},{64,15},{65,15},{66,15},{67,15},{68,15},{69,15} };
		int nFoodX = 30;
		int nFoodY = 15;
		int nScore = 0;
		int nSnakeDirection = 3; // 0 N, 1 E, 2 S, 3 W
		bool bDead = false; //end of game
		bool bKeyLeft = false, bKeyRight = false, bKeyLeftOld = false, bKeyRightOld = false;
		int Htime = 120;
		int Vtime = 200;

		while (!bDead) {

			// Timing and Input
			auto t1 = chrono::system_clock::now();
			while ((chrono::system_clock::now() - t1) < ((nSnakeDirection % 2 == 1) ? (chrono::milliseconds)Htime : (chrono::milliseconds)Vtime))
			{
				// Get Input, 
				bKeyRight = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;
				bKeyLeft = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;

				if (bKeyRight && !bKeyRightOld)
				{
					nSnakeDirection++;
					if (nSnakeDirection == 4) nSnakeDirection = 0;
				}

				if (bKeyLeft && !bKeyLeftOld)
				{
					nSnakeDirection--;
					if (nSnakeDirection == -1) nSnakeDirection = 3;
				}

				bKeyRightOld = bKeyRight;
				bKeyLeftOld = bKeyLeft;
			}




			// Game Logic
			// Update snake pos
			switch (nSnakeDirection) {
			case 0: //up
				snake.push_front({ snake.front().x, snake.front().y - 1 });
				break;
			case 1:
				snake.push_front({ snake.front().x + 1, snake.front().y });
				break;
			case 2:
				snake.push_front({ snake.front().x, snake.front().y + 1 });
				break;
			case 3:
				snake.push_front({ snake.front().x - 1, snake.front().y });
				break;

			}

			// Collision Detect Snake V Food
			if (snake.front().x == nFoodX && snake.front().y == nFoodY)
			{
				nScore++;
				while (screen[nFoodY * nScreenWidth + nFoodX] != L' ')
				{
					nFoodX = rand() % nScreenWidth;
					nFoodY = (rand() % (nScreenHeight - 3)) + 3;
				}

				for (int i = 0; i < 3; i++)
					snake.push_back({ snake.back().x, snake.back().y });

				Htime *= 0.80;
				Vtime *= 0.80;
			}

			// Collision Detect Snake V World
			if (snake.front().x < 0 || snake.front().x >= nScreenWidth)
				bDead = true;
			if (snake.front().y < 3 || snake.front().y >= nScreenHeight)
				bDead = true;

			// Collision Detect Snake V Snake
			for (list<sSnakeSegment>::iterator i = snake.begin(); i != snake.end(); i++)
				if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y)
					bDead = true;

			// Chop off Snakes tail :-/
			snake.pop_back();


			// Display to player
			// Clear Screen
			for (int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';

			// Draw Stats and Border
			for (int i = 0; i < nScreenWidth; i++) {
				screen[i] = L'=';
				screen[2 * nScreenWidth + i] = L'=';
			}
			wsprintf(&screen[nScreenWidth + 5], L"S N A K E ! !                SCORE: %d", nScore);


			// Draw Snake Body
			for (auto s : snake)
				screen[s.y * nScreenWidth + s.x] = bDead ? L'+' : L'O';

			// Draw snake head
			screen[snake.front().y * nScreenWidth + snake.front().x] = bDead ? L'x' : L'@';

			// Draw a food
			screen[nFoodY * nScreenWidth + nFoodX] = L'%';

			if (bDead)
				wsprintf(&screen[15 * nScreenWidth + 40], L"    PRESS 'SPACE' TO PLAY AGAIN    ");

			// Display Screen
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
		}
		// Wait for space
		while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);
	}
	return 0;
}


