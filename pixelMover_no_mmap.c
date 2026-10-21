#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <linux/input.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <poll.h>

// frame buffer name = RPi-Sense FB
// Joystick name = Raspberry Pi Sense HAT Joystick



// little endian so flip this shit
// 15 16 d9 6a | d7 28 04 00 | 01 00        |   1c 00   | 01 00 00 00
//  tv_sec       tv_usec        type            code        value
// dont care   |  dont care  | 1=keypressed | which key | 0 = release, 1 = pressed, 2 = held

struct input_event joystick;
int pixeloffset;
typedef struct
{
    unsigned int x;
    unsigned int y;
} coord;
const unsigned short RED = 0xF800;
const unsigned short OFF = 0x0000;
const char *fb0 = "/dev/fb0";
unsigned short blank[64] = {0};
int main(void) {

FILE *JS_event = fopen("/dev/input/event4", "rb"); //open event4 (JS_event) with read binary mode
if (JS_event == NULL) {
    perror("JS_event");
    return 1;
}

FILE *RGBLED_event = fopen ("/dev/fb0", "wb");
if (RGBLED_event == NULL) {
    perror("RGBLED_event");
    return 1;
}
        fseek(RGBLED_event, 0, SEEK_SET);
        fwrite(&blank, sizeof(blank), 1, RGBLED_event);
        fflush(RGBLED_event);
coord cursor;
cursor.x = 0;
cursor.y = 0;
        fseek(RGBLED_event, cursor.x*2+cursor.y*16, SEEK_SET);
        fwrite(&RED, sizeof(RED), 1, RGBLED_event);
        fflush(RGBLED_event);
while (fread(&joystick, sizeof(joystick), 1, JS_event) == 1) { // joystick is a 16 bytes struct
    if (joystick.type == EV_KEY) {
        fseek(RGBLED_event, cursor.x*2+cursor.y*16, SEEK_SET);
        fwrite(&OFF, sizeof(OFF), 1, RGBLED_event);
        fflush(RGBLED_event);
        switch (joystick.code)
        {
        case KEY_UP: //up
        if (joystick.value > 0 && cursor.y > 0) {
            cursor.y -= 1;
        }
            break;
        case KEY_DOWN: //down 
        if (joystick.value > 0 && cursor.y < 7) {
        cursor.y += 1;
        }
                  break;
        case KEY_LEFT: //left
        if (joystick.value > 0 && cursor.x > 0) {
        cursor.x -= 1;
        }
                  break;
        case KEY_RIGHT:    //right
        if (joystick.value > 0 && cursor.x < 7) {
         cursor.x += 1;
        }
                  break;
        case KEY_ENTER: // enter
                  break;
        default:
            break;
        }
    printf("Code = %d, Value = %d \n", joystick.code, joystick.value);
        fseek(RGBLED_event, cursor.x*2+cursor.y*16, SEEK_SET);
        fwrite(&RED, sizeof(RED), 1, RGBLED_event);
        fflush(RGBLED_event);
    }
}


}