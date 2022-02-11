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
#include "vm.h"
#include "Audio.h"
#include "PicoRam.h"


const int sdlWindowWidth = 128;
const int sdlWindowHeight = 128;
const int PicoScreenWidth = 128;
const int PicoScreenHeight = 128;

SDL_Window *_mainWindow;                    // Declare a pointer
SDL_Surface *surface;
SDL_Texture *_mainTexture;
SDL_Renderer* _mainRenderer;

void *_mainPixels;
uint8_t *_mainBase;
int _mainPitch;

SDL_Rect _mainDestR;
SDL_Rect _mainSrcR;
double _mainTextureAngle = 0;
SDL_RendererFlip _mainFlip = SDL_FLIP_NONE;

Color paletteColors[144];

bool quit = false;
u_long t = 0;
SDL_Event _mainEvent;

int _mainJoystickCount;

uint8_t _mainCurrKDown;
uint8_t _mainCurrKHeld;

bool lDown = false;
bool rDown = false;
bool _mainStretchKeyPressed = false;

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
    while (SDL_PollEvent(&_mainEvent)) {
        switch (_mainEvent.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                if ((_mainEvent.key.keysym.sym == SDLK_AC_BACK) ||
                    (_mainEvent.key.keysym.sym == SDLK_ESCAPE)) {
                    quit = true;
                }
                break;
            case SDL_JOYBUTTONDOWN :
                switch (_mainEvent.jbutton.button)
                {
                    case KEYCODE_BUTTON_START:  _mainCurrKDown |= P8_KEY_PAUSE; break;
                    case KEYCODE_DPAD_LEFT:  _mainCurrKDown |= P8_KEY_LEFT; break;
                    case KEYCODE_DPAD_RIGHT: _mainCurrKDown |= P8_KEY_RIGHT; break;
                    case KEYCODE_DPAD_UP:    _mainCurrKDown |= P8_KEY_UP; break;
                    case KEYCODE_DPAD_DOWN:  _mainCurrKDown |= P8_KEY_DOWN; break;
                    case KEYCODE_BUTTON_A:     _mainCurrKDown |= P8_KEY_X; break;
                    case KEYCODE_BUTTON_B:     _mainCurrKDown |= P8_KEY_O; break;

                    case KEYCODE_BUTTON_L1: lDown = true; break;
                    case KEYCODE_BUTTON_R1: rDown = true; break;
                    case KEYCODE_BUTTON_SELECT: _mainStretchKeyPressed = true; break;
                }
                //__android_log_print(ANDROID_LOG_INFO, "sdl", "joy button down %d", _mainEvent.jbutton.button);
                break;
            case SDL_JOYBUTTONUP :
                switch (_mainEvent.jbutton.button)
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
                //__android_log_print(ANDROID_LOG_INFO, "sdl", "joy button up %d", _mainEvent.jbutton.button);
                break;
            case SDL_JOYAXISMOTION:
                //__android_log_print(ANDROID_LOG_INFO, "sdl", "joy axis motion %d %d", _mainEvent.jaxis.which, _mainEvent.jaxis.axis);

                //_mainEvent.jaxis.value;
                break;
            default:
                break;
        }
    }

    _mainCurrKHeld |= _mainCurrKDown;
    _mainCurrKHeld ^= kUp;
}

void drawScreen() {
    SDL_SetRenderDrawColor(_mainRenderer, 128, 0, 128, 255);
    SDL_RenderClear(_mainRenderer);

    SDL_LockTexture(_mainTexture, NULL, &_mainPixels, &_mainPitch);

    for (int y = 0; y < PicoScreenHeight; y ++){
        for (int x = 0; x < PicoScreenWidth; x ++){
            u_long c = (x + y + (t / 10)) % 16;
            Color col = paletteColors[c];

            _mainBase = ((Uint8 *)_mainPixels) + (4 * ( y * PicoScreenHeight + x));
            _mainBase[0] = col.Blue;
            _mainBase[1] = col.Green;
            _mainBase[2] = col.Red;
            _mainBase[3] = col.Alpha;
        }
    }

    //present
    SDL_UnlockTexture(_mainTexture);

    SDL_UnlockTexture(_mainTexture);
    SDL_RenderCopyEx(_mainRenderer, _mainTexture, &_mainSrcR, &_mainDestR, _mainTextureAngle, NULL, _mainFlip);

    SDL_RenderPresent(_mainRenderer);

}


void setUpPaletteColors(){
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

    for (int i = 16; i < 128; i++) {
        paletteColors[i] = {0, 0, 0, 0};
    }

    paletteColors[128] = COLOR_128;
    paletteColors[129] = COLOR_129;
    paletteColors[130] = COLOR_130;
    paletteColors[131] = COLOR_131;
    paletteColors[132] = COLOR_132;
    paletteColors[133] = COLOR_133;
    paletteColors[134] = COLOR_134;
    paletteColors[135] = COLOR_135;
    paletteColors[136] = COLOR_136;
    paletteColors[137] = COLOR_137;
    paletteColors[138] = COLOR_138;
    paletteColors[139] = COLOR_139;
    paletteColors[140] = COLOR_140;
    paletteColors[141] = COLOR_141;
    paletteColors[142] = COLOR_142;
    paletteColors[143] = COLOR_143;
}

int SDL_main(int argc, char* argv[]) {
    setUpPaletteColors();

    SDL_Init(SDL_INIT_EVERYTHING);              // Initialize SDL2

    // Create an application _mainWindow with the following settings:
    /*
    _mainWindow = SDL_CreateWindow(
        "An SDL2 _mainWindow",                  // _mainWindow title
        SDL_WINDOWPOS_UNDEFINED,           // initial x position
        SDL_WINDOWPOS_UNDEFINED,           // initial y position
        640,                               // width, in _mainPixels
        480,                               // height, in _mainPixels
        SDL_WINDOW_OPENGL                  // flags - see below
    );
     */
    _mainWindow = SDL_CreateWindow(
            "FAKE-08",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            sdlWindowWidth,
            sdlWindowHeight,
            SDL_RENDERER_ACCELERATED);

    // Check that the _mainWindow was successfully created
    if (_mainWindow == NULL) {
        // In the case that the _mainWindow could not be made...
        printf("Could not create _mainWindow: %s\n", SDL_GetError());
        return 1;
    }

    int actualWinWidth, actualWinHeight;
    int stretchedPicoWidth, stretchedPicoHeight;

    SDL_GetWindowSize(_mainWindow, &actualWinWidth, &actualWinHeight);

    int xScaleFactor = actualWinWidth / PicoScreenWidth;
    int yScaleFactor = actualWinHeight / PicoScreenHeight;
    int scaleFactor = xScaleFactor < yScaleFactor ? xScaleFactor : yScaleFactor;

    stretchedPicoWidth = PicoScreenWidth * scaleFactor;
    stretchedPicoHeight = PicoScreenHeight * scaleFactor;


    // Setup _mainRenderer
    _mainRenderer =  SDL_CreateRenderer( _mainWindow, -1, SDL_RENDERER_ACCELERATED);
    if (_mainRenderer == NULL) {
        // In the case that the _mainWindow could not be made...
        printf("Could not create _mainRenderer: %s\n", SDL_GetError());
        return 1;
    }

    _mainTexture = SDL_CreateTexture(
            _mainRenderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            PicoScreenWidth,
            PicoScreenHeight);
    if (!_mainTexture) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Couldn't load _mainTexture: %s", SDL_GetError());
        return 4;
    }

    _mainDestR.x = actualWinWidth / 2 - stretchedPicoWidth / 2;
    _mainDestR.y = actualWinHeight / 2 - stretchedPicoHeight / 2;
    _mainDestR.w = stretchedPicoWidth;
    _mainDestR.h = stretchedPicoHeight;

    _mainSrcR.x = 0;
    _mainSrcR.y = 0;
    _mainSrcR.w = PicoScreenWidth;
    _mainSrcR.h = PicoScreenHeight;

    _mainJoystickCount = SDL_NumJoysticks();
    for (int i = 0; i < _mainJoystickCount; i++) {
        if (SDL_JoystickOpen(i) == NULL) {
            printf("Failed to open joystick %d!\n", i);
            return 1;
        }
    }

    // _mainEvent loop
    while(true) {
        handleEvents();

        if (quit){
            break;
        }

        drawScreen();

        t++;
    }


    // Close and _mainDestRoy the _mainWindow
    SDL_DestroyTexture(_mainTexture);
    SDL_DestroyRenderer(_mainRenderer);
    SDL_DestroyWindow(_mainWindow);

    // Clean up
    SDL_Quit();

    return 0;
}
