# Animation_System
Animation System that handles 2D and 3D models
2D models can use a sprite atlas or skeleton
Data for the models and animations are read from JSON files

The main code for this project is found in build/vx2017

Inside the Animation_Parser files are seperated into 2D or 3D depending on which part of the animation
system they are used for.

Important files include ImGui_handler and those within the Animation_Parser.
The node_editor.cpp and header files are a work-in-progress and are not fully functional

This project contains lots of error handling to deal with unexpected input, for example: trying to use
an animation or model that does not exist will load the default 3D animation to avoid crashing.

When entering the name of an animation or model you do not have to include the file type, this program
will automatically find this and act appropiately using data from the JSON file, for example:
loading in a 2D skeletal animation.

More information can be found here: http://www.jodieduff.co.uk/animation_system.html