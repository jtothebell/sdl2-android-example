#include <android/log.h>
#include "SDL.h"
#include "SDL_video.h"
/*
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL2_gfxPrimitives.h"
#include "SDL_ttf.h"
 */

#include "hostVmShared.h"


const int sdlWindowWidth = 128;
const int sdlWindowHeight = 128;
const int PicoScreenWidth = 128;
const int PicoScreenHeight = 128;

SDL_Window *window;                    // Declare a pointer
SDL_Surface *surface;
SDL_Texture *texture;
SDL_Renderer* renderer;

void *pixels;
uint8_t *base;
int pitch;

SDL_Rect DestR;
SDL_Rect SrcR;
double textureAngle = 0;
SDL_RendererFlip flip = SDL_FLIP_NONE;

Color paletteColors[16];

bool quit = false;
u_long t = 0;
SDL_Event event;

int joystickCount;

uint8_t currKDown;
uint8_t currKHeld;

bool lDown = false;
bool rDown = false;
bool stretchKeyPressed = false;

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

void handleEvents() {
    uint8_t kUp = 0;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                if ((event.key.keysym.sym == SDLK_AC_BACK) ||
                    (event.key.keysym.sym == SDLK_ESCAPE)) {
                    quit = true;
                }
                break;
            case SDL_JOYBUTTONDOWN :
                switch (event.jbutton.button)
                {
                    case KEYCODE_BUTTON_START:  currKDown |= P8_KEY_PAUSE; break;
                    case KEYCODE_DPAD_LEFT:  currKDown |= P8_KEY_LEFT; break;
                    case KEYCODE_DPAD_RIGHT: currKDown |= P8_KEY_RIGHT; break;
                    case KEYCODE_DPAD_UP:    currKDown |= P8_KEY_UP; break;
                    case KEYCODE_DPAD_DOWN:  currKDown |= P8_KEY_DOWN; break;
                    case KEYCODE_BUTTON_A:     currKDown |= P8_KEY_X; break;
                    case KEYCODE_BUTTON_B:     currKDown |= P8_KEY_O; break;

                    case KEYCODE_BUTTON_L1: lDown = true; break;
                    case KEYCODE_BUTTON_R1: rDown = true; break;
                    case KEYCODE_BUTTON_SELECT: stretchKeyPressed = true; break;
                }
                //__android_log_print(ANDROID_LOG_INFO, "sdl", "joy button down %d", event.jbutton.button);
                break;
            case SDL_JOYBUTTONUP :
                switch (event.jbutton.button)
                {
                    case KEYCODE_BUTTON_START:  kUp |= P8_KEY_PAUSE; break;
                    case KEYCODE_DPAD_LEFT:  kUp |= P8_KEY_LEFT; break;
                    case KEYCODE_DPAD_RIGHT: kUp |= P8_KEY_RIGHT; break;
                    case KEYCODE_DPAD_UP:    kUp |= P8_KEY_UP; break;
                    case KEYCODE_DPAD_DOWN:  kUp |= P8_KEY_DOWN; break;
                    case KEYCODE_BUTTON_A:     kUp |= P8_KEY_X; break;
                    case KEYCODE_BUTTON_B:     kUp |= P8_KEY_O; break;

                    case KEYCODE_BUTTON_L1: lDown = false; break;
                    case KEYCODE_BUTTON_R1: rDown = false; break;
                }
                //__android_log_print(ANDROID_LOG_INFO, "sdl", "joy button up %d", event.jbutton.button);
                break;
            case SDL_JOYAXISMOTION:
                //__android_log_print(ANDROID_LOG_INFO, "sdl", "joy axis motion %d %d", event.jaxis.which, event.jaxis.axis);

                //event.jaxis.value;
                break;
            default:
                break;
        }
    }

    currKHeld |= currKDown;
    currKHeld ^= kUp;
}

void drawScreen() {
    SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255);
    SDL_RenderClear(renderer);

    SDL_LockTexture(texture, NULL, &pixels, &pitch);

    for (int y = 0; y < PicoScreenHeight; y ++){
        for (int x = 0; x < PicoScreenWidth; x ++){
            u_long c = (x + y + (t / 10)) % 16;
            Color col = paletteColors[c];

            base = ((Uint8 *)pixels) + (4 * ( y * PicoScreenHeight + x));
            base[0] = col.Blue;
            base[1] = col.Green;
            base[2] = col.Red;
            base[3] = col.Alpha;
        }
    }

    //present
    SDL_UnlockTexture(texture);

    SDL_UnlockTexture(texture);
    SDL_RenderCopyEx(renderer, texture, &SrcR, &DestR, textureAngle, NULL, flip);

    SDL_RenderPresent(renderer);

}

int SDL_main(int argc, char* argv[]) {
    paletteColors[0] = COLOR_00;
    paletteColors[1] = COLOR_01;
    paletteColors[2] = COLOR_02;
    paletteColors[3] = COLOR_03;
    paletteColors[4] = COLOR_04;
    paletteColors[5] = COLOR_05;
    paletteColors[6] = COLOR_06;
    paletteColors[7] = COLOR_07;
    paletteColors[8] = COLOR_08;
    paletteColors[9] = COLOR_09;
    paletteColors[10] = COLOR_10;
    paletteColors[11] = COLOR_11;
    paletteColors[12] = COLOR_12;
    paletteColors[13] = COLOR_13;
    paletteColors[14] = COLOR_14;
    paletteColors[15] = COLOR_15;

    SDL_Init(SDL_INIT_EVERYTHING);              // Initialize SDL2

    // Create an application window with the following settings:
    /*
    window = SDL_CreateWindow(
        "An SDL2 window",                  // window title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        640,                               // width, in pixels
        480,                               // height, in pixels
        SDL_WINDOW_OPENGL                  // flags - see below
    );
     */
    window = SDL_CreateWindow(
            "FAKE-08",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            sdlWindowWidth,
            sdlWindowHeight,
            SDL_RENDERER_ACCELERATED);

    // Check that the window was successfully created
    if (window == NULL) {
        // In the case that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    int actualWinWidth, actualWinHeight;
    int stretchedPicoWidth, stretchedPicoHeight;

    SDL_GetWindowSize(window, &actualWinWidth, &actualWinHeight);

    int xScaleFactor = actualWinWidth / PicoScreenWidth;
    int yScaleFactor = actualWinHeight / PicoScreenHeight;
    int scaleFactor = xScaleFactor < yScaleFactor ? xScaleFactor : yScaleFactor;

    stretchedPicoWidth = PicoScreenWidth * scaleFactor;
    stretchedPicoHeight = PicoScreenHeight * scaleFactor;


    // Setup renderer
    renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        // In the case that the window could not be made...
        printf("Could not create renderer: %s\n", SDL_GetError());
        return 1;
    }

    texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            PicoScreenWidth,
            PicoScreenHeight);
    if (!texture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Couldn't load texture: %s", SDL_GetError());
        return 4;
    }

    DestR.x = actualWinWidth / 2 - stretchedPicoWidth / 2;
    DestR.y = actualWinHeight / 2 - stretchedPicoHeight / 2;
    DestR.w = stretchedPicoWidth;
    DestR.h = stretchedPicoHeight;

    SrcR.x = 0;
    SrcR.y = 0;
    SrcR.w = PicoScreenWidth;
    SrcR.h = PicoScreenHeight;

    joystickCount = SDL_NumJoysticks();
    for (int i = 0; i < joystickCount; i++) {
        if (SDL_JoystickOpen(i) == NULL) {
            printf("Failed to open joystick %d!\n", i);
            return 1;
        }
    }



    // Event loop
    while(true) {
        handleEvents();

        if (quit){
            break;
        }

        drawScreen();

        t++;
    }


    // Close and destroy the window
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();

    return 0;
}
