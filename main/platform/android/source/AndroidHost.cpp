
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
        128,
        128,
        WINDOW_FLAGS,
        RENDERER_FLAGS,
        PIXEL_FORMAT,
        _desktopSdl2SettingsPrefix,
        _desktopSdl2customBiosLua,
        "/mnt/sdcard/roms/p8carts"
    );
}


InputState_t Host::scanInput(){
    currKDown = 0;
    currKHeld = 0;
    stretchKeyPressed = false;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case KEYCODE_BUTTON_START:currKDown |= P8_KEY_PAUSE; break;
                    case KEYCODE_DPAD_LEFT:  currKDown |= P8_KEY_LEFT; break;
                    case KEYCODE_DPAD_RIGHT: currKDown |= P8_KEY_RIGHT; break;
                    case KEYCODE_DPAD_UP:    currKDown |= P8_KEY_UP; break;
                    case KEYCODE_DPAD_DOWN:  currKDown |= P8_KEY_DOWN; break;
                    case KEYCODE_BUTTON_A:     currKDown |= P8_KEY_X; break;
                    case KEYCODE_BUTTON_B:     currKDown |= P8_KEY_O; break;
                    case KEYCODE_BUTTON_Y:     currKDown |= P8_KEY_X; break;
                    case KEYCODE_BUTTON_R1:     stretchKeyPressed = true; break;
                    case KEYCODE_BUTTON_L2:     quit = 1; break;
                }
                break;

            case SDL_QUIT:
                quit = 1;
                break;
        }
    }

    int mouseX = 0;
	int mouseY = 0;
    uint32_t sdlMouseBtnState = SDL_GetMouseState(&mouseX, &mouseY);
    //adjust for scale
    mouseX -= mouseOffsetX;
    mouseY -= mouseOffsetY;
    mouseX /= scaleX;
    mouseY /= scaleY;
    uint8_t picoMouseState = 0;
    if (sdlMouseBtnState & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        picoMouseState |= 1;
    }
    if (sdlMouseBtnState & SDL_BUTTON(SDL_BUTTON_MIDDLE)) {
        picoMouseState |= 4;
    }
    if (sdlMouseBtnState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        picoMouseState |= 2;
    }

    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    //continuous-response keys
    if(keystate[SDL_SCANCODE_LEFT]){
        currKHeld |= P8_KEY_LEFT;
    }
    if(keystate[SDL_SCANCODE_RIGHT]){
        currKHeld |= P8_KEY_RIGHT;;
    }
    if(keystate[SDL_SCANCODE_UP]){
        currKHeld |= P8_KEY_UP;
    }
    if(keystate[SDL_SCANCODE_DOWN]){
        currKHeld |= P8_KEY_DOWN;
    }
    if(keystate[SDL_SCANCODE_Z]){
        currKHeld |= P8_KEY_X;
    }
    if(keystate[SDL_SCANCODE_X]){
        currKHeld |= P8_KEY_O;
    }
    if(keystate[SDL_SCANCODE_C]){
        currKHeld |= P8_KEY_X;
    }
    
    return InputState_t {
        currKDown,
        currKHeld,
        (int16_t)mouseX,
        (int16_t)mouseY,
        picoMouseState
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

