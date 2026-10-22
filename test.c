#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
// frame buffer name = RPi-Sense FB
// Joystick name 
// Raspberry Pi Sense HAT Joystick



// little endian so flip this shit
// 15 16 d9 6a | d7 28 04 00 | 01 00        |   1c 00   | 01 00 00 00
//  tv_sec       tv_usec        type            code        value
// dont care   |  dont care  | 1=keypressed | which key | 0 = release, 1 = pressed, 2 = held

struct input_event joystick;
struct fb_fix_screeninfo sfill; // this struct has a char id array which will store the "name" of files we open so we can compare them
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


int fb_fd = -1;
    for(int i = 0; i < 31; ++i) {
        char path[32]; // just need to hold the file path string, can be lower than 32
        snprintf(path, sizeof(path), "/dev/fb%d", i); // store formatted file path string with /dev/fb<i>
        int fd = open(path, O_RDWR);  // open /dev/fb<i>, returns file descriptor for my code
        if (fd == -1) { // returns -1 if couldnt open path
        continue;                                 // this fbN doesn't exist, try the next number
        }
 // ioctl asks the file (fd) FBIOGEt_FSCREENINFO, and store the result in the sfill struct. 
 // compares the struct we filled's id with the name of fd. both return 0 = success!
        if (ioctl(fd, FBIOGET_FSCREENINFO, &sfill) == 0 && strcmp(sfill.id, "RPi-Sense FB") == 0) {
        fb_fd = fd;
        break;
            }
            close(fd);                                    // wrong device, close it before trying the next
        }
    if (fb_fd == -1) {                                // loop finished without finding it
        fprintf(stderr, "Could not find RPi-Sense FB\n");
        return 1;
    }



int js_fd = -1;
for (int i = 0; i < 31; ++i) {
    char path[32]; // just needs to hold the file path string, can be lower than 32
    snprintf(path, sizeof(path), "/dev/input/event%d", i); // store formatted file path string /dev/input/event<i>

    int fd = open(path, O_RDONLY); // read only, we never write to the joystick
    if (fd == -1) {                // returns -1 if it couldn't open path
        continue;                  // this eventN doesn't exist, try the next number
    }

    // ioctl asks the device (fd) for its name (EVIOCGNAME) and stores it in name.
    // It returns the length of the name on success, -1 on failure.
    // strcmp returns 0 when the device's name matches the one we're looking for.
    char name[256];
    if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) >= 0 && strcmp(name, "Raspberry Pi Sense HAT Joystick") == 0) {
        js_fd = fd; // found it, keep this file descriptor
        break;
    }
    close(fd); // wrong device, close it before trying the next
}
if (js_fd == -1) { // loop finished without finding it
    fprintf(stderr, "Could not find Raspberry Pi Sense HAT Joystick\n");
    return 1;
}


// memory map the fb_fd device/file to rgb_matrix, thus we can simply rgb_matrix[index] = (16 bit pixel colorcode)
unsigned short *rgb_matrix = mmap(NULL, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0); // set mmap to rd/wr and map_shared means changing the rgb_matrix will also change the actual device/file fd_fb.
if (rgb_matrix == MAP_FAILED) {
    perror("mmap fb");
    return 1;
}
for (int i = 0; i < 63; ++i) {
    rgb_matrix[i] = OFF;
}

coord cursor;
cursor.x = 0;
cursor.y = 0;
//        void *mmap(void addr[.length], size_t length, int prot, int flags, int fd, off_t offset);
// 
rgb_matrix[cursor.x+(cursor.y*8)] = RED;
// set current pixel to red
while (read(js_fd, &joystick, sizeof(joystick)) == sizeof(joystick))  { // joystick is a 16 bytes struct
    if (joystick.type == EV_KEY) {
rgb_matrix[cursor.x+(cursor.y*8)] = OFF;
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
rgb_matrix[cursor.x+(cursor.y*8)] = RED;
    }
}


}