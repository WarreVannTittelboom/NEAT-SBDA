#pragma once
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <windows.h>
#include <random>

using namespace cv;

int left = 0x41; //A-key
int right = 0x44; //D-key
int jump = 0x58; //X-key
int pause = 0x4F; //O-key
int enter = 0x0D; //Enter-key
int reboot = 0x51; //Q-key

std::vector<int> possibleOutputs = { left,right,jump }; //possible outputs for neural network

// Shared variables
Point2f playerPos;
std::vector <Point2f> enemiesPos;
std::vector <Point2f> terrainPos;
Point2f previousTerrainCheck;
bool new_location_available = false;
int spriteCount = 26;
int marioSpriteCount = 12;
int enemySpriteCount = 5;
int terrainSpriteCount = 7;


// Mutex and condition variable for synchronization
std::mutex mutex;
std::condition_variable conVar;

Mat currScreen;
std::string windowName = "Super Mario Land (World) (Rev A) [GB] - BizHawk";
std::vector<Mat> templateImages;
std::vector<double> thresholds;

std::vector <std::vector<Point2f>> objectPositions;
const LPCWSTR emulatorPath{ L"C:\\Blizhawk\\EmuHawk.exe" }; //path to emulator.exe


Rect windowRect;
HWND windowHandle;


//Sending input to currently selected emulator
void SentInput(int scancode, int pressSleep = 120)
{


	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.time = 0;
	ip.ki.wVk = 0;
	ip.ki.dwExtraInfo = 0;


	ip.ki.dwFlags = KEYEVENTF_SCANCODE;
	ip.ki.wScan = MapVirtualKey(scancode, 0);

	SendInput(1, &ip, sizeof(INPUT));
	Sleep(pressSleep);

	ip.ki.wVk = 0;
	ip.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
	Sleep(20);
}

//Image paths and their thresholds for template matching
void SetImagesAndThresholds()
{
	// Set the template image of the enemy to find
	templateImages[0] = imread("../SpriteSheet/mairo-s-n-r.png");
	templateImages[1] = imread("../SpriteSheet/mairo-s-n-l.png");
	templateImages[2] = imread("../SpriteSheet/mairo-s-j-r.png");
	templateImages[3] = imread("../SpriteSheet/mairo-s-j-l.png");
	templateImages[4] = imread("../SpriteSheet/mairo-s-v-r.png");
	templateImages[5] = imread("../SpriteSheet/mairo-s-v-l.png");
	templateImages[6] = imread("../SpriteSheet/mairo-b-n-r.png");
	templateImages[7] = imread("../SpriteSheet/mairo-b-n-l.png");
	templateImages[8] = imread("../SpriteSheet/mairo-b-j-r.png");
	templateImages[9] = imread("../SpriteSheet/mairo-b-j-l.png");
	templateImages[10] = imread("../SpriteSheet/mairo-b-v-r.png");
	templateImages[11] = imread("../SpriteSheet/mairo-b-v-l.png");

	templateImages[12] = imread("../SpriteSheet/enemy-1.png");
	templateImages[13] = imread("../SpriteSheet/enemy-2.png");
	templateImages[14] = imread("../SpriteSheet/enemy-3.png");
	templateImages[15] = imread("../SpriteSheet/enemy-4.png");
	templateImages[16] = imread("../SpriteSheet/enemy-5.png");


	templateImages[17] = imread("../SpriteSheet/block.png");
	templateImages[18] = imread("../SpriteSheet/brick.png");
	templateImages[19] = imread("../SpriteSheet/upgrade.png");
	templateImages[20] = imread("../SpriteSheet/obstacle1.png");
	templateImages[21] = imread("../SpriteSheet/pipe.png");
	templateImages[22] = imread("../SpriteSheet/floor.png");
	templateImages[23] = imread("../SpriteSheet/ground.png");

	templateImages[24] = imread("../SpriteSheet/start.png");

	templateImages[25] = imread("../SpriteSheet/mario-dead.png");

	// Set the similarity score threshold in correct order (first enmeey, second player, etc)
	thresholds[0] = 0.7;
	thresholds[1] = 0.7;
	thresholds[2] = 0.7;
	thresholds[3] = 0.7;
	thresholds[4] = 0.7;
	thresholds[5] = 0.7;
	thresholds[6] = 0.7;
	thresholds[7] = 0.7;
	thresholds[8] = 0.7;
	thresholds[9] = 0.7;
	thresholds[10] = 0.7;
	thresholds[11] = 0.7;

	thresholds[12] = 0.8;
	thresholds[13] = 0.8;
	thresholds[14] = 0.8;
	thresholds[15] = 0.95;
	thresholds[16] = 0.95;

	thresholds[17] = 1.0;
	thresholds[18] = 1.0;
	thresholds[19] = 1.0;
	thresholds[20] = 1.0;
	thresholds[21] = 1.0;
	thresholds[22] = 1.0;
	thresholds[23] = 1.0;
	thresholds[24] = 1.0;
	thresholds[25] = 0.85;

}

void InitEmu()
{
	// TODO : load best network from file
	ShellExecute(NULL, L"open", emulatorPath, NULL, NULL, SW_SHOWDEFAULT);
	Sleep(5000);
	templateImages.resize(spriteCount);
	thresholds.resize(100);
	
		objectPositions.resize(600);
		enemiesPos.resize(200);
		terrainPos.resize(200);

	//Image paths and their thresholds for template matching
	SetImagesAndThresholds();
	// Find the emulator window
	windowHandle = FindWindowA(NULL, windowName.c_str());
	if (windowHandle != NULL) {
		RECT rect;
		GetWindowRect(windowHandle, &rect);
		windowRect = Rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	}
	else {
		std::cerr << "Emulator window not found." << std::endl;
	}

	
}


void CaptureScreen()
{
	// Capture a screenshot of the emulator window
	HDC dc = GetDC(windowHandle);
	HDC hdc = CreateCompatibleDC(dc);
	HBITMAP hbmp = CreateCompatibleBitmap(dc, windowRect.width, windowRect.height);
	SelectObject(hdc, hbmp);
	BitBlt(hdc, 0, 0, windowRect.width, windowRect.height, dc, 0, 0, SRCCOPY | CAPTUREBLT);
	ReleaseDC(windowHandle, dc);

	// Convert the screenshot to an OpenCV Mat
	BITMAPINFO bmi = { sizeof(BITMAPINFOHEADER), windowRect.width, -windowRect.height, 1, 24, BI_RGB, 0, 0, 0, 0, 0 };
	Mat screen(windowRect.height, windowRect.width, CV_8UC3);
	GetDIBits(hdc, hbmp, 0, windowRect.height, screen.data, &bmi, DIB_RGB_COLORS);

	currScreen = screen;

	// Clean up the HDC and HBITMAP objects
	DeleteDC(hdc);
	DeleteObject(hbmp);

}

void GetPos(std::vector<std::vector<Point2f>>& pos)
{
	pos.resize(spriteCount);
	std::vector<Mat> results(spriteCount);

	for (int i = 0; i < spriteCount; ++i)
	{
		matchTemplate(currScreen, templateImages[i], results[i], TM_CCOEFF_NORMED);
		double threshold = thresholds[i];
		int iter = 0;
		while (true)
		{
			Point maxLoc;
			double maxVal;
			minMaxLoc(results[i], NULL, &maxVal, NULL, &maxLoc);
			pos[i].resize(20);
			if (maxVal >= threshold)
			{
				cv::Point2f test = Point2f(maxLoc.x + templateImages[i].cols / 2, maxLoc.y + templateImages[i].rows / 2);
				pos[i][iter] = test;
				++iter;
				rectangle(results[i], maxLoc, Point(maxLoc.x + templateImages[i].cols, maxLoc.y + templateImages[i].rows), Scalar::all(0), -1);
			}
			else
			{
				break;
			}
		}
	}
}

bool CheckPlayer(Point2f& pos)
{
	for (int i = 0; i < marioSpriteCount; ++i)
	{
		if (!(objectPositions[i][0].x == 0 && objectPositions[i][0].y == 0))
		{
			pos = objectPositions[i][0];
			return true;
		}
	}
	return false;
}


bool CheckEnemies(std::vector <Point2f>& pos)
{
	bool found = false;
	for (int i = marioSpriteCount; i < marioSpriteCount + enemySpriteCount; ++i)
	{
		for (int j = 0; j < objectPositions[j].size(); ++j)
		{
			if (!(objectPositions[i][j].x == 0 && objectPositions[i][j].y == 0))
			{
				pos.push_back(objectPositions[i][j]);
				found = true;
			}
		}

	}
	if (found) { return true; }
	return false;
}

void CheckTerrain(std::vector <Point2f>& pos)
{
	for (int i = marioSpriteCount + enemySpriteCount; i < marioSpriteCount + enemySpriteCount + terrainSpriteCount; ++i)
	{
		for (int j = 0; j < objectPositions[j].size(); ++j)
		{
			if (!(objectPositions[i][j].x == 0 && objectPositions[i][j].y == 0))
			{
				pos.push_back(objectPositions[i][j]);
			}
		}

	}
}

bool BotNotMoving()
{
	for (int i = marioSpriteCount + enemySpriteCount; i < marioSpriteCount + enemySpriteCount + terrainSpriteCount; ++i)
	{
		for (int j = 0; j < objectPositions[j].size(); ++j)
		{
			if (!(objectPositions[i][j].x == 0 && objectPositions[i][j].y == 0))
			{
				if (previousTerrainCheck == objectPositions[i][j])
				{
					return true;
				}
				else
				{
					previousTerrainCheck = objectPositions[i][j];
					return false;
				}
			}
		}

	}
}

bool CheckStart()
{
	int startIndex = marioSpriteCount + enemySpriteCount + terrainSpriteCount;
	for (int j = 0; j < objectPositions[j].size(); ++j)
	{
		if (!(objectPositions[startIndex][j].x == 0 && objectPositions[startIndex][j].y == 0))
		{
			return true;
		}
	}
	return false;
}

void ReadScreenThread()
{
	while (true)
	{
		auto start = std::chrono::high_resolution_clock::now();
		objectPositions.clear();
		CaptureScreen();
		GetPos(objectPositions);
		Point2f playerLoc;
		if (CheckPlayer(playerLoc))
		{
			std::cout << playerLoc.x << " " << playerLoc.y << "\n\n";
		}
		else
		{
			std::cout << "Player not found \n\n";
		}
		std::vector <Point2f> enemyLocs;
		if (CheckEnemies(enemyLocs))
		{
			std::cout << "Enemy count: " << enemyLocs.size() << "\n\n";
		}
		else
		{
			std::cout << "Enemies not found \n\n";
		}
		std::vector <Point2f> terrainLocs;
		CheckTerrain(terrainLocs);

		if (BotNotMoving())
		{
			std::cout << "Bot not moving \n\n";
		}
		else
		{
			std::cout << "Bot moving \n\n";
		}

		if (CheckStart())
		{
			std::cout<< "Start detect \n\n";
			SentInput(enter);
		}
		else
		{
			std::cout << "// \n\n";
		}
		auto stop = std::chrono::high_resolution_clock::now();
		auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<float>>(stop - start);
		// Update the shared variables
		{
			std::lock_guard<std::mutex> lock(mutex);
			playerPos = playerLoc;
			enemiesPos = enemyLocs;
			terrainPos = terrainLocs;
			new_location_available = true;
		}

		// Notify the second thread that new location information is available
		conVar.notify_one();
	}
}

// Function to continuously evaluate the NEAT algorithm using the latest player's location
void NeatThread()
{

	while (true)
	{
		// Wait for new location information
		{
			std::unique_lock<std::mutex> lock(mutex);
			conVar.wait(lock, [] { return new_location_available; });
			new_location_available = false;
		}
		// TODO: feed the game data to the NEAT evaluation function, then translate the output from this into which button should be pressed


		// Code for demonstration 
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> distribution1(300, 450);
		std::uniform_int_distribution<int> distribution2(150, 300);
		int randomNumber1 = distribution1(gen);
		int randomNumber2 = distribution2(gen);
		SentInput(possibleOutputs[1], randomNumber1);
		SentInput(possibleOutputs[2], randomNumber2);
		
	}

}

int main()
{
	InitEmu();

	std::thread screenshot_thread(ReadScreenThread);
	std::thread neat_thread(NeatThread);

	// Wait for the threads to finish
	screenshot_thread.join();
	neat_thread.join();

	return 0;
}

