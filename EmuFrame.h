#pragma once
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <windows.h>
#include <random>
#include <cstdlib>

using namespace cv;
//emulator used: https://tasvideos.org/BizHawk
//note that the provided config file for blizhawk should be used so the game is loaded on startup, the correct inputs are used and the window is the correct size (x3)
//the path to the mario ROM will also need to be changed in this config file
//for installing opencv see: https://www.tutorialspoint.com/how-to-install-opencv-for-cplusplus-in-windows#:~:text=Installing%20OpenCV,extract%20it%20in%20C%20Drive.
//or use vcpkg  https://vcpkg.io/en/
//to see the suggested approach for implementing NEAT into this project see the task list (visaul studio -> view -> task list), open all code files and filter open documents to hide 3d party TODO's
class EmuFrame
{
private:
	
	const LPCWSTR emulatorPath{ L"C:\\Blizhawk\\EmuHawk.exe"}; //path to emulator.exe

	const DWORD startupSleepTime = 10000; //time to wait for emulators to start
	
	const int population = 5; // Population size (number of instances)

	const float maxNoMoveTime = 100.f; // max time of not moving before mario is considered dead

	const bool twoRows = true; //bool indicating if instances should be placed in 2 rows or 1 row

	bool active = true; //bool indicating program is running
	
	int gen = 1; //Generation number

	//name used for finding emulator windows (moved to top of cpp file as a global because of callback function)
	/*std::string emuWindowName = "Super Mario Land (World) (Rev A) [GB] - BizHawk";*/ 

	//vector containing windowhandles of all instances (moved to top of cpp file as a global because of callback function)
	/*std::vector<HWND> gameboyHandles;*/
	
	//data for each instance of the game
	struct instanceData
	{
		// TODO : add network here (ensures each instance has its own network)
		//NeuralNet brain;
		
		//2D vector of containing the postion of all objects currently on screen of istance each object type has its own vector 
		//so if multiple instances of the same object are on screen, they are pushed on the corresponding vector
		std::vector <std::vector<Point2f>> objectPositions;
		
		Point2f playerPos; //current position of mario in instance, if mario not found, position is (0,0)
		
		std::vector <Point2f> enemiesPos; //vector containing the position of all enemies currently on screen
		
		std::vector <Point2f> terrainPos; //vector containing the position of all obstacles currently on screen
		
		Point2f previousTerrainCheck; //pos of terrainobject used for checking if mario is moving
		
		float noMoveTime = 0.0f; //cuur time since last move

		float totalNoMoveTime = 0.0f; //total time of not moving

		float aliveTime = 0.0f; //total time of being alive
		
		bool alive = true; //bool indicating if mario is alive thus indicating if instance is still active
	};
	
	std::vector <instanceData> gameInstances; // vector of all instances data ( size = population )

	//Vars for template matching
	int spriteCount = 26; //sum of all sprites for data and + 2 for start and dead check
	int marioSpriteCount = 12;
	int enemySpriteCount = 5;
	int terrainSpriteCount = 7;
	
	//template matching vars
	std::vector <Mat> gameScreens; //vector containing all screens of all instances
	std::vector<Mat> templateImages; //vector containing all template images
	std::vector<double> thresholds; //vector containing all thresholds for template matching


	//hold this button to savely quit the training
	int saveQuitKey = 0x1B; //esc-key

	//buttons scancodes used for SendInput() to Emulator (note that these are not standard inputs for BlizHawk, use the provided config file for blizhawk for these inputs or change to your liking)
	// find scancodes here: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	int left = 0x41; //A-key
	int right = 0x44; //D-key
	int jump = 0x58; //X-key
	int pause = 0x4F; //O-key
	int enter = 0x0D; //Enter-key
	int reboot = 0x51; //Q-key
	
	std::vector<int> possibleOutputs = { left,right,jump }; //possible outputs for neural network


	//|||||||||||||||||||||||||||||||||||||||||||||//
	//||||||||||Initialization Functioins||||||||||//
	//|||||||||||||||||||||||||||||||||||||||||||||//

	//Starts emualators for every instance
	//Also pauses every instance on start
	//And sets the window size/pos for every instance
	void InitEmulators();

	//Set Image paths and their thresholds for template matching
	void SetImagesAndThresholds();

	//|||||||||||||||||||||||||||||||||||||||||||||//
	//|Functions for getting the screen from Emu's|//
	//|||||||||||||||||||||||||||||||||||||||||||||//

	// Callback function to collect window handles (defined outisde of class)
	/*BOOL EnumChildProc(HWND hwnd, LPARAM lParam)*/

	//Caputre screen from every emulator instance
	void CaptureScreen();

	//|||||||||||||||||||||||||||||||||||||||||||||//
	//Functions for extraxting data from ScreenCaps//
	//|||||||||||||||||||||||||||||||||||||||||||||//

	//Get the position of all objects in the game
	void GetPos(std::vector<std::vector<Point2f>>& pos, int iter);

	//Check if mario was found and save pos
	bool CheckPlayer(Point2f& pos, int iter);

	//Check if enemies were found and save their pos
	bool CheckEnemies(std::vector<Point2f>& pos, int iter);

	//Check if terrain objects were found and save their pos
	void CheckTerrain(std::vector<Point2f>& pos, int iter);


	//|||||||||||||||||||||||||||||||||||||||||||||//
	//|Util functions for getting and sending data|//
	//|||||||||||||||||||||||||||||||||||||||||||||//

	//Check if mario is moving
	bool BotNotMoving(int iter);

	//Check if start button is on screen
	bool CheckStart(int iter);

	//Sending input to currently selected emulator
	void SentInput(int scancode, int pressSleep);




	//|||||||||||||||||||||||||||||||||||||||||||||//
	//|||||||| Main Program Loop Functions|||||||||//
	//|||||||||||||||||||||||||||||||||||||||||||||//


	

	// For every instance where mario is alive, read screen, extract data from screen and save it
	void ReadScreen();

	//Starts the game if not started yet
	void DoStart();

	//Checks if mario should still be consinderd alive
	void CheckMarioAlive();

	// Determines input to be sent for every instance using NEAT
	void DoNEAT();

	//Check if all instances are dead
	bool AllInstancesDead();

	//NEAT logic for new generation and resetting instances
	void NewGen();

	//Check if quit key is held and close the emulators (esc-key)
	void checkquit();

	//Closes all Emulators
	void cleanup();
	
	public:
	//Executes the main program loop
	void Run();

};

