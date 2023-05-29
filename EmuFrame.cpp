#pragma once
#include "EmuFrame.h"

//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||//
//||||||Moved from members vars to global vars because of callback function||||||//
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||//
std::vector<HWND> gameboyHandles;
const std::string emuWindowName = "Super Mario Land (World) (Rev A) [GB] - BizHawk";
//|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||//

// Callback function to collect window handles (needs to be outside of the class)
BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam)
{
	HWND parentHandle = reinterpret_cast<HWND>(lParam);

	// Check if the window is a child of the parent window
	if (GetParent(hwnd) == parentHandle)
	{
		char windowTitle[256];
		GetWindowTextA(hwnd, windowTitle, sizeof(windowTitle));

		// Compare the window title with the name of your gameboy emulator windows
		if (strstr(windowTitle, emuWindowName.c_str()) != nullptr)
		{
			// Append the handle to the vector
			gameboyHandles.push_back(hwnd);
		}
	}

	return TRUE;
}

//Sending input to currently selected emulator
void EmuFrame::SentInput(int scancode, int pressSleep = 120)
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
void EmuFrame::SetImagesAndThresholds()
{
	// Set the template image of the enemy to find
	templateImages[0] = imread("./SpriteSheet/mairo-s-n-r.png");
	templateImages[1] = imread("./SpriteSheet/mairo-s-n-l.png");
	templateImages[2] = imread("./SpriteSheet/mairo-s-j-r.png");
	templateImages[3] = imread("./SpriteSheet/mairo-s-j-l.png");
	templateImages[4] = imread("./SpriteSheet/mairo-s-v-r.png");
	templateImages[5] = imread("./SpriteSheet/mairo-s-v-l.png");
	templateImages[6] = imread("./SpriteSheet/mairo-b-n-r.png");
	templateImages[7] = imread("./SpriteSheet/mairo-b-n-l.png");
	templateImages[8] = imread("./SpriteSheet/mairo-b-j-r.png");
	templateImages[9] = imread("./SpriteSheet/mairo-b-j-l.png");
	templateImages[10] = imread("./SpriteSheet/mairo-b-v-r.png");
	templateImages[11] = imread("./SpriteSheet/mairo-b-v-l.png");

	templateImages[12] = imread("./SpriteSheet/enemy-1.png");
	templateImages[13] = imread("./SpriteSheet/enemy-2.png");
	templateImages[14] = imread("./SpriteSheet/enemy-3.png");
	templateImages[15] = imread("./SpriteSheet/enemy-4.png");
	templateImages[16] = imread("./SpriteSheet/enemy-5.png");


	templateImages[17] = imread("./SpriteSheet/block.png");
	templateImages[18] = imread("./SpriteSheet/brick.png");
	templateImages[19] = imread("./SpriteSheet/upgrade.png");
	templateImages[20] = imread("./SpriteSheet/obstacle1.png");
	templateImages[21] = imread("./SpriteSheet/pipe.png");
	templateImages[22] = imread("./SpriteSheet/floor.png");
	templateImages[23] = imread("./SpriteSheet/ground.png");

	templateImages[24] = imread("./SpriteSheet/start.png");

	templateImages[25] = imread("./SpriteSheet/mario-dead.png");

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

void EmuFrame::InitEmulators()
{
	// TODO : Initialize the networks with random topologies (here or make new function and call it before this one in the run loop) or if saved training data is available, load it into some of them but still keep some random ones
	gameScreens.resize(population);
	gameInstances.resize(population);

	// open emualtor instance for each individual (population) + sleep for x seconds to let the emulators load
	for (int i = 0; population > i; ++i)
	{
		ShellExecute(NULL, L"open", emulatorPath, NULL, NULL, SW_SHOWDEFAULT);
	}
	Sleep(startupSleepTime);


	
	templateImages.resize(spriteCount);
	thresholds.resize(100);
	for (int i = 0; population > i; ++i)
	{
		gameInstances[i].objectPositions.resize(600);
		gameInstances[i].enemiesPos.resize(200);
		gameInstances[i].terrainPos.resize(200);
		
	}

	//Image paths and their thresholds for template matching
	SetImagesAndThresholds();



	// Get the handle of the desktop window for getting emulator instances
	HWND parentHandle = FindWindow(nullptr, L"Desktop");
	EnumChildWindows(parentHandle,  EnumChildProc, reinterpret_cast<LPARAM>(parentHandle));

	// Loop through the identified gameboy emulator windows and pause them
	for (int i = 0; population > i; ++i)
	{
		SetForegroundWindow(gameboyHandles[i]);
		Sleep(50);
		SentInput(pause);
		RECT rect;
		GetWindowRect(gameboyHandles[i], &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		if (twoRows)
		{

			SetWindowPos(gameboyHandles[i], nullptr, (i/2) * width, (i%2) * height, width, height, SWP_NOZORDER);
		}
		else
		{
			SetWindowPos(gameboyHandles[i], nullptr, i * width, 0, width, height, SWP_NOZORDER);
		}
		
	}
}

//Caputre screen from every emulator instance
void EmuFrame::CaptureScreen()
{
	// Loop through the identified gameboy emulator windows
	int count = 0;
	for (HWND handle : gameboyHandles)
	{
		// Retrieve the position and size of the window
		RECT rect;
		GetWindowRect(handle, &rect);

		// Calculate the width and height of the window
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		// Create a compatible device context
		HDC hdcWindow = GetDC(handle);
		HDC hdcMem = CreateCompatibleDC(hdcWindow);

		// Create a compatible bitmap
		BITMAPINFO bmi;
		memset(&bmi, 0, sizeof(BITMAPINFO));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = height;  // Positive height for correct orientation (bottom-up)
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;  // Assuming the emulator uses 32 bits per pixel (BGRA format)
		bmi.bmiHeader.biCompression = BI_RGB;

		HBITMAP hBitmap = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, nullptr, nullptr, 0);

		// Select the bitmap into the compatible device context
		SelectObject(hdcMem, hBitmap);

		// Copy the window content to the compatible device context
		BitBlt(hdcMem, 0, 0, width, height, hdcWindow, 0, 0, SRCCOPY);

		// Create a Mat object to store the window image
		cv::Mat windowImage(height, width, CV_8UC4);

		// Retrieve the bitmap data into the Mat object
		BITMAPINFOHEADER* header = &(bmi.bmiHeader);
		GetDIBits(hdcMem, hBitmap, 0, height, windowImage.data, &bmi, DIB_RGB_COLORS);

		// Release resources
		DeleteObject(hBitmap);
		DeleteDC(hdcMem);
		ReleaseDC(handle, hdcWindow);

		// Convert BGRA to BGR format
		cv::cvtColor(windowImage, windowImage, cv::COLOR_BGRA2BGR);

		// Flip the image vertically to correct orientation
		cv::flip(windowImage, windowImage, 0);
		gameScreens[count] = windowImage;
		++count;
	}
}

//Get the position of all objects in the game
void EmuFrame::GetPos(std::vector<std::vector<Point2f>>& pos, int iter)
{
	pos.resize(spriteCount);
	std::vector<Mat> results(spriteCount);

	for (int i = 0; i < spriteCount; ++i)
	{
		matchTemplate(gameScreens[iter], templateImages[i], results[i], TM_CCOEFF_NORMED);
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
				cv::Point2f result = Point2f(maxLoc.x + templateImages[i].cols / 2, maxLoc.y + templateImages[i].rows / 2);
				pos[i][iter] = result;
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

//Check if mario was found and save pos
bool EmuFrame::CheckPlayer(Point2f& pos, int iter)
{

	for (int i = 0; i < marioSpriteCount; ++i)
	{
		if (!(gameInstances[iter].objectPositions[i][0].x == 0 && gameInstances[iter].objectPositions[i][0].y == 0))
		{
			pos = gameInstances[iter].objectPositions[i][0];
			return true;
		}
	}
	return false;
}

//Check if enemies were found and save their pos
bool EmuFrame::CheckEnemies(std::vector <Point2f>& pos, int iter)
{
	bool found = false;
	for (int i = marioSpriteCount; i < marioSpriteCount + enemySpriteCount; ++i)
	{
		for (int j = 0; j < gameInstances[iter].objectPositions[j].size(); ++j)
		{
			if (!(gameInstances[iter].objectPositions[i][j].x == 0 && gameInstances[iter].objectPositions[i][j].y == 0))
			{
				pos.push_back(gameInstances[iter].objectPositions[i][j]);
				found = true;
			}
		}

	}
	if (found) { return true; }
	return false;
}

//Check if terrain objects were found and save their pos
void EmuFrame::CheckTerrain(std::vector <Point2f>& pos, int iter)
{
	for (int i = marioSpriteCount + enemySpriteCount; i < marioSpriteCount + enemySpriteCount + terrainSpriteCount; ++i)
	{
		for (int j = 0; j < gameInstances[iter].objectPositions[j].size(); ++j)
		{
			if (!(gameInstances[iter].objectPositions[i][j].x == 0 && gameInstances[iter].objectPositions[i][j].y == 0))
			{
				pos.push_back(gameInstances[iter].objectPositions[i][j]);
			}
		}

	}
}

//Check if mario is moving
bool EmuFrame::BotNotMoving(int iter)
{
	for (int i = marioSpriteCount + enemySpriteCount; i < marioSpriteCount + enemySpriteCount + terrainSpriteCount; ++i)
	{
		for (int j = 0; j < gameInstances[iter].objectPositions[j].size(); ++j)
		{
			if (!(gameInstances[iter].objectPositions[i][j].x == 0 && gameInstances[iter].objectPositions[i][j].y == 0))
			{
				if (gameInstances[i].previousTerrainCheck == gameInstances[iter].objectPositions[i][j])
				{
					return true;
				}
				else
				{
					gameInstances[i].previousTerrainCheck = gameInstances[iter].objectPositions[i][j];
					return false;
				}
			}
		}

	}
}

//Check if start is found on the screen
bool EmuFrame::CheckStart(int iter)
{
	int startIndex = marioSpriteCount + enemySpriteCount + terrainSpriteCount;
	for (int j = 0; j < gameInstances[iter].objectPositions[j].size(); ++j)
	{
		if (!(gameInstances[iter].objectPositions[startIndex][j].x == 0 && gameInstances[iter].objectPositions[startIndex][j].y == 0))
		{
			return true;
		}
	}
	return false;
}


// main program loop
void EmuFrame::Run()
{
	//Starts emualators for every instance
	InitEmulators();


	while (active)
	{
		//Reads screen and saves data for player and every instance of enemy and terrain in their respective vectors (for every instance)
		//Also starts the game if not started
		DoStart();
		ReadScreen();
		CheckMarioAlive();

		//Using NEAT algorithm, calculate the next move for every instance and send it to the emulator
		//Uses the 3 data vectors as input and determines which of the 3 outputs to send to the emulator
		DoNEAT();
		//if all instances dead, restart needed + new generation
		if (AllInstancesDead())
		{
			NewGen();
		}
		//check if quit key is held and close the emulators (esc-key)
		checkquit();
	}
}

// Read the screen and save the positions of all objects
void EmuFrame::ReadScreen()
{
	CaptureScreen();
	std::vector <instanceData> localGameInstances;
	localGameInstances.resize(population);
	// for every instance (if mario alive) save the postions of the player, enemies and terrain
	for (int iter = 0; population > iter; ++iter)
	{
		if (gameInstances[iter].alive)
		{
			auto start = std::chrono::high_resolution_clock::now();
			gameInstances[iter].objectPositions.clear();
			GetPos(gameInstances[iter].objectPositions, iter);

			Point2f playerLoc;
			CheckPlayer(playerLoc, iter);

			std::vector <Point2f> enemyLocs;
			CheckEnemies(enemyLocs, iter);

			std::vector <Point2f> terrainLocs;
			CheckTerrain(terrainLocs, iter);
			
			//if not moving add elapsed time to noMoveTime else set it to 0
			auto stop = std::chrono::high_resolution_clock::now();
			auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<float>>(stop - start);
			if (BotNotMoving(iter))
			{
				gameInstances[iter].noMoveTime += elapsed_seconds.count();
				gameInstances[iter].totalNoMoveTime += elapsed_seconds.count();
			}
			else
			{
				gameInstances[iter].noMoveTime = 0.0f;
			}

			gameInstances[iter].aliveTime += elapsed_seconds.count();
			
			localGameInstances[iter].playerPos = playerLoc;
			localGameInstances[iter].enemiesPos = enemyLocs;
			localGameInstances[iter].terrainPos = terrainLocs;
		}
	}

	// save data for every instance
	for (int i = 0; population > i; ++i)
	{
		gameInstances[i].playerPos = localGameInstances[i].playerPos;
		gameInstances[i].enemiesPos = localGameInstances[i].enemiesPos;
		gameInstances[i].terrainPos = localGameInstances[i].terrainPos;
	}

}

//Start game if not started
void EmuFrame::DoStart()
{
	for (int iter = 0; population > iter; ++iter)
	{
		if (gameInstances[iter].alive)
		{
			if (CheckStart(iter))
			{
				SetForegroundWindow(gameboyHandles[iter]);
				Sleep(50);
				SentInput(pause);
				Sleep(20);
				SentInput(enter);
				Sleep(300);
				SentInput(pause);
			}
		}
	}
}

//Checks if mario should be dead and sets alive to false if so
void EmuFrame::CheckMarioAlive()
{
	for (int iter = 0; population > iter; ++iter)
	{
		if (gameInstances[iter].alive)
		{
			//+1 because of start object count
			int marioStatusIndex = marioSpriteCount + enemySpriteCount + terrainSpriteCount +1;
			for (int j = 0; j < gameInstances[iter].objectPositions[j].size(); ++j)
			{
				if (!(gameInstances[iter].objectPositions[marioStatusIndex][j].x == 0 && gameInstances[iter].objectPositions[marioStatusIndex][j].y == 0))
				{
					gameInstances[iter].alive = false;
				}
			}

			if (gameInstances[iter].noMoveTime > maxNoMoveTime)
			{
				gameInstances[iter].alive = false;
			}
		}
	}
}

// Determines input to be sent for every instance using NEAT
void EmuFrame::DoNEAT()
{
	// TODO: feed the game data to the NEAT evaluation function, then translate the output from this into which button should be pressed
	
	// duration of press can be set by passing it with the SentInput function
	
	//sents random input for testing
	//Init random generator
	std::random_device rd;
	std::mt19937 gen(rd());
	for (int iter = 0; population > iter; ++iter)
	{
		if (gameInstances[iter].alive)
		{
			std::uniform_int_distribution<int> distribution(0, 2);
			int randomNumber = distribution(gen);
			SetForegroundWindow(gameboyHandles[iter]);
			Sleep(50);
			SentInput(pause);
			Sleep(20);
			SentInput(possibleOutputs[randomNumber]);
			Sleep(300);
			SentInput(pause);
		}
	}
}

//Returns true if all instances are dead
bool EmuFrame::AllInstancesDead()
{
	for (int i = 0; population > i; ++i)
	{
		if (gameInstances[i].alive)
		{
			return false;		
		}
	}
	return true;
}

//NEAT logic for new generation and resetting instances
void EmuFrame::NewGen()
{
	//All instances dead, restart needed + new generation
	
	//TODO : 1. Calc the fitness of each instance with use of timeAlive and timeNotMoving (from gameInstances data)
	//TODO : 2. Sort networks by fitness
	//TODO : 3. Check if the the best fitness surpasses the global best(read from file), if so update the global best and save this network (write to files)
	//TODO : 4. Select x amount of best networks as parents for new generation
	//TODO : 5. Create offspring by using crossover and mutation operations on these parents
	//TODO : 6. Make a new generation using this offspring (if the network is part of gameInstances save the networks locally and assign them after resseting the instances)
	
	int highestFitnessGen = 2; //assign highest fitness of this generation here
	int highestFitnessGlobal = 10; //assign highest fitness of all generations here (read from file)
	if (highestFitnessGlobal == highestFitnessGen) //assuming highest fitness was updated if needed in previous steps
	{
		std::cout << "\n\nGeneration " << gen << std::endl;
		std::cout << "\nNew highest global fitness: " << highestFitnessGlobal << std::endl;
	}
	else
	{
		std::cout << "\n\nGeneration " << gen << std::endl;
		std::cout << "\nHighest fitness of this generation: " << highestFitnessGen << std::endl;
		std::cout << "Global highest fitness: " << highestFitnessGlobal << std::endl;
	}
	
	for (int i = 0; population > i; ++i)
	{
		gameInstances[i] = {};
		gameInstances[i].objectPositions.resize(600);
		gameInstances[i].enemiesPos.resize(200);
		gameInstances[i].terrainPos.resize(200);
		// TODO: if network is part of the gameInstances assign the new network here
		SetForegroundWindow(gameboyHandles[i]);
		Sleep(50);
		SentInput(pause);
		Sleep(20);
		SentInput(reboot);
		Sleep(300);
		SentInput(pause);
	}
	++gen;
}

//Closes all Emulators
void EmuFrame::cleanup()
{
	std::cout << "cleanup" << std::endl;
	for (int i = 0; population > i; ++i)
	{
		SendMessage(gameboyHandles[i], WM_CLOSE, 0, 0);
	}
}

//Used to exit program when quit key is held down
void EmuFrame::checkquit()
{

	// Check the state of the key
	SHORT keyState = GetAsyncKeyState(saveQuitKey);

	// Check if the key was pressed
	if (keyState & 0x8000)
	{
		active = false;
		cleanup();
	}
}
