# mbed-nucleo-robosumo
Simple messy robosumo program made with mbed for STMNucleo32 board (might work with other boards supporting mbed as well). I made it for a minisumo robot i competed with at [Robogames.utb.cz 2019](https://robogames.utb.cz/). Used Libraries:
* [HC_SR04_Ultrasonic_Library](https://os.mbed.com/users/ejteb/code/HC_SR04_Ultrasonic_Library/)
* [Encoder](https://os.mbed.com/users/vsluiter/code/Encoder/)

<p align="center">
  <img src="https://raw.githubusercontent.com/kukosek/mbed-nucleo-robosumo/master/sumo.gif" />
</p>

## Features
### Search tactics
The robot is searching for the oponent by measuring the distance with the ultrasound sensor.
#### 3 moving tactics, that are changed every x seconds if the oponent wasn't found:
* Rotating from one side to the other, the offset is being slowly changed
* Slowly rotating to one side with the right side being as center
* Slowly rotating to one side

### Atack tactic
If the oponent is found, the robot goes straigt. If it detects an encounter with the oponent (by motor encoders), it starts a         timer. When the timer reaches x seconds, it twitches, then the timer resets.

### Others
* The program is started by pressing a custom button
* The default field color can be setted by holding the custom button for 5 seconds
* An custom LED can be used to indicate the state

## Usage
 1. First create an project in [mbed](https://ide.mbed.com/compiler/) 
 2. Import the files (Or rewrite the arleady existing files with these) from this repository
 3. Edit the pins in main.cpp

## Known issues
* The rate customizing doesn't work like it should

### TODOs
* Refactoring the code
* translating notes to english
* encoder motor PID control
