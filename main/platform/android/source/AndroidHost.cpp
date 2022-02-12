
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <fstream>
#include <iostream>
using namespace std;

#include "../../SDL2Common/source/sdl2basehost.h"
#include "../../../source/hostVmShared.h"
#include "../../../source/nibblehelpers.h"
#include "../../../source/filehelpers.h"
#include "../../../source/logger.h"

// sdl
#ifdef __ANDROID__
#include "SDL.h"
#else
#include <SDL2/SDL.h>
#endif

#define WINDOW_FLAGS 0

#define RENDERER_FLAGS SDL_RENDERER_ACCELERATED
#define PIXEL_FORMAT SDL_PIXELFORMAT_ARGB8888

#define KEYCODE_BUTTON_A 0
#define KEYCODE_BUTTON_B 1
#define KEYCODE_BUTTON_X 2
#define KEYCODE_BUTTON_Y 3
#define KEYCODE_BUTTON_SELECT 4
#define KEYCODE_BUTTON_UNKNOWN0 5
#define KEYCODE_BUTTON_START 6
#define KEYCODE_BUTTON_L1 9
#define KEYCODE_BUTTON_R1 10
#define KEYCODE_DPAD_UP 11
#define KEYCODE_DPAD_DOWN 12
#define KEYCODE_DPAD_LEFT 13
#define KEYCODE_DPAD_RIGHT 14
#define KEYCODE_BUTTON_L2 15
#define KEYCODE_BUTTON_R2 16

SDL_Event event;
SDL_Point touchLocation = { 128 / 2, 128 / 2 };

//Analog joystick dead zone
const int JOYSTICK_DEAD_ZONE = 8000;
int jxDir = 0;
int jyDir = 0;


string _desktopSdl2SettingsDir = "fake08";
string _desktopSdl2SettingsPrefix = "fake08/";
string _desktopSdl2customBiosLua = "cartpath = \"~/p8carts/\"\n"
        "selectbtn = \"z\"\n"
        "pausebtn = \"esc\"\n"
        "exitbtn = \"close window\"\n"
        "sizebtn = \"\"";

Host::Host() 
{
    setPlatformParams(
        640,
        480,
        WINDOW_FLAGS,
        RENDERER_FLAGS,
        PIXEL_FORMAT,
        _desktopSdl2SettingsPrefix,
        _desktopSdl2customBiosLua,
        "/storage/emulated/0/roms/p8carts"
    );
}


InputState_t Host::scanInput(){
    currKDown = 0;
    uint8_t kUp = 0;
    int prevJxDir = jxDir;
    int prevJyDir = jyDir;
    stretchKeyPressed = false;

    uint8_t mouseBtnState = 0;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_JOYBUTTONDOWN :
                switch (event.jbutton.button)
                {
                    case KEYCODE_BUTTON_START:currKDown |= P8_KEY_PAUSE; break;
                    case KEYCODE_DPAD_LEFT:  currKDown |= P8_KEY_LEFT; break;
                    case KEYCODE_DPAD_RIGHT: currKDown |= P8_KEY_RIGHT; break;
                    case KEYCODE_DPAD_UP:    currKDown |= P8_KEY_UP; break;
                    case KEYCODE_DPAD_DOWN:  currKDown |= P8_KEY_DOWN; break;
                    case KEYCODE_BUTTON_A:     currKDown |= P8_KEY_X; break;
                    case KEYCODE_BUTTON_B:     currKDown |= P8_KEY_O; break;
                    case KEYCODE_BUTTON_Y:     currKDown |= P8_KEY_X; break;
                    case KEYCODE_BUTTON_SELECT:     stretchKeyPressed = true; break;
                    case KEYCODE_BUTTON_L2:     quit = 1; break;
                }
                break;

            case SDL_JOYBUTTONUP :
                switch (event.jbutton.button)
                {
                    case KEYCODE_BUTTON_START: kUp |= P8_KEY_PAUSE; break;
                    case KEYCODE_DPAD_LEFT:  kUp |= P8_KEY_LEFT; break;
                    case KEYCODE_DPAD_RIGHT: kUp |= P8_KEY_RIGHT; break;
                    case KEYCODE_DPAD_UP:    kUp |= P8_KEY_UP; break;
                    case KEYCODE_DPAD_DOWN:  kUp |= P8_KEY_DOWN; break;
                    case KEYCODE_BUTTON_A:     kUp |= P8_KEY_X; break;
                    case KEYCODE_BUTTON_B:     kUp |= P8_KEY_O; break;
                    case KEYCODE_BUTTON_Y:     kUp |= P8_KEY_X; break;

                }
                break;

            case SDL_JOYAXISMOTION :
                if (event.jaxis.which == 0)
                {
                    //X axis motion
                    if( event.jaxis.axis == 0 )
                    {
                        //Left of dead zone
                        if( event.jaxis.value < -JOYSTICK_DEAD_ZONE )
                        {
                            jxDir = -1;
                        }
                            //Right of dead zone
                        else if( event.jaxis.value > JOYSTICK_DEAD_ZONE )
                        {
                            jxDir =  1;
                        }
                        else
                        {
                            jxDir = 0;
                        }
                    }
                        //Y axis motion
                    else if( event.jaxis.axis == 1 )
                    {
                        //Below of dead zone
                        if( event.jaxis.value < -JOYSTICK_DEAD_ZONE )
                        {
                            jyDir = -1;
                        }
                            //Above of dead zone
                        else if( event.jaxis.value > JOYSTICK_DEAD_ZONE )
                        {
                            jyDir =  1;
                        }
                        else
                        {
                            jyDir = 0;
                        }
                    }
                }
                break;

            case SDL_FINGERDOWN:
                //touchId 0 is front, 1 is back. ignore back touches
                if (event.tfinger.touchId == 0) {
                    touchLocation.x = ((event.tfinger.x * _actualWindowWidth) - mouseOffsetX) / scaleX;
                    touchLocation.y = ((event.tfinger.y * _actualWindowHeight) - mouseOffsetY) / scaleY;
                    mouseBtnState = 1;
                }
                break;

            case SDL_FINGERMOTION:
                //touchId 0 is front, 1 is back. ignore back touches
                if (event.tfinger.touchId == 0) {
                    touchLocation.x = ((event.tfinger.x * _actualWindowWidth) - mouseOffsetX) / scaleX;
                    touchLocation.y = ((event.tfinger.y * _actualWindowHeight) - mouseOffsetY) / scaleY;
                    mouseBtnState = 1;
                }
                break;

            case SDL_FINGERUP:
                //do nothing for now?
                mouseBtnState = 0;
                break;

            case SDL_QUIT:
                quit = 1;
                break;
        }
    }

    currKHeld |= currKDown;
    currKHeld ^= kUp;

    //Convert joystick direction to kHeld and kDown values
    if (jxDir > 0) {
        currKHeld |= P8_KEY_RIGHT;
        currKHeld &= ~(P8_KEY_LEFT);

        if (prevJxDir != jxDir){
            currKDown |= P8_KEY_RIGHT;
        }
    }
    else if (jxDir < 0) {
        currKHeld |= P8_KEY_LEFT;
        currKHeld &= ~(P8_KEY_RIGHT);

        if (prevJxDir != jxDir){
            currKDown |= P8_KEY_LEFT;
        }
    }
    else if (prevJxDir != 0){
        currKHeld &= ~(P8_KEY_RIGHT);
        currKHeld &= ~(P8_KEY_LEFT);
        currKDown &= ~(P8_KEY_RIGHT);
        currKDown &= ~(P8_KEY_LEFT);
    }

    if (jyDir > 0) {
        currKHeld |= P8_KEY_DOWN;
        currKHeld &= ~(P8_KEY_UP);

        if (prevJyDir != jyDir){
            currKDown |= P8_KEY_DOWN;
        }
    }
    else if (jyDir < 0) {
        currKHeld |= P8_KEY_UP;
        currKHeld &= ~(P8_KEY_DOWN);

        if (prevJyDir != jyDir){
            currKDown |= P8_KEY_UP;
        }
    }
    else if (prevJyDir != 0){
        currKHeld &= ~(P8_KEY_UP);
        currKHeld &= ~(P8_KEY_DOWN);
        currKDown &= ~(P8_KEY_UP);
        currKDown &= ~(P8_KEY_DOWN);
    }

    return InputState_t {
            currKDown,
            currKHeld,
            (int16_t)touchLocation.x,
            (int16_t)touchLocation.y,
            mouseBtnState
    };
}

vector<string> Host::listcarts(){
    vector<string> carts;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (_cartDirectory.c_str())) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            if (isCartFile(ent->d_name)){
                carts.push_back(ent->d_name);
            }
        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
    }
    
    return carts;
}

