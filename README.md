# NEAT-SBDA
## Introduction
NEAT-SBDA is framework for training AI agents using NEAT. <br/>
Specfically for training NEAT AI's playing Super Mario Land, but changing the project to work for similar GameBoy tiltes is possible. <br/>
The framework utilizes the BizHawk emulator for running Super Mario Land, allowing for easy configuration and control of gameplay. <br/>
Each genome from the population from the NEAT Algorithm is represented by a seperate emualtor instance. <br/>
The framework leverages computer vision techniques and these emulator instances to efficiently extract data from the game screen for training the NEAT-based AI systems. <br/>
By incorporating computer vision techniques using OpenCV, the framework allows for screen-based data acquisition, enabling the extraction of relevant information from the game screen. <br/>
There is also an additional project included to showcase the best results from the training project in real-time. <br/>
test <br/>
## Building
To build the project installing <a href="https://opencv.org/" target="_blank">OpenCV</a> is required. <br/>
I would suggest using <a href="https://vcpkg.io/en/" target="_blank">vcpkg manager</a> to do this quickly and seamlessly or follow the tutorial on the offical site.  <br/>
## Running The Project
Running the project will also require installing the <a href="https://tasvideos.org/BizHawk" target="_blank">BlizHawkr</a> Emulator. <br/>
The project folder also includes a config.ini file that should be replaced inside of the BlizHawk folder with the standard one. <br/>
Inside of this file you will also have to edit the path to the ROM that should be opendend on start. <br/>
Additionally, inside of the project, the path to where the BlizHawk is located and the title of the ROM should be specified correctly. <br/>
If you plan on using the Super Mario Land ROM and followed all steps correctly, you should be good to go! <br/>
## Change Game/ Add Template Data For Additional Levels
If you want to change the game to another similar one you will have to change the template image data and thresholds. <br/>
Also keep in mind to adjust to the correct ordering and sizing of the objects to ensure they will be put into the correct containers. <br/>
Also note that some functions like CheckMarioMoving will likely have to be changed entielry for different games and that the data gathered for evaluting the performance of the AI might also have to change. <br/>
## Implementing NEAT
Inside of the project you can find TODO comments that will elleborate on where and how to start with incomparitng NEAT logic into the project. 

