/*
 * QEMU SDL display driver
 *
 * Copyright (c) 2003 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* Avoid compiler warning because macro is redefined in SDL_syswm.h. */
#undef WIN32_LEAN_AND_MEAN

#include <SDL.h>
#include <SDL_syswm.h>

#include "qemu-common.h"
#include "ui/console.h"
#include "ui/input.h"
#include "sysemu/sysemu.h"
#include "x_keymap.h"
#include "sdl_zoom.h"
///////////////////////////////
//#include "std_main.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//////////////////////////////////
static DisplayChangeListener *dcl;
static DisplaySurface *surface;
static SDL_Surface *real_screen;
static SDL_Surface *guest_screen = NULL;
static int gui_grab; /* if true, all keyboard/mouse events are grabbed */
static int last_vm_running;
static bool gui_saved_scaling;
static int gui_saved_width;
static int gui_saved_height;
static int gui_saved_grab;
static int gui_fullscreen;
static int gui_noframe;
static int gui_key_modifier_pressed;
static int gui_keysym;
static int gui_grab_code = KMOD_LALT | KMOD_LCTRL;
static uint8_t modifiers_state[256];
static SDL_Cursor *sdl_cursor_normal;
static SDL_Cursor *sdl_cursor_hidden;
static int absolute_enabled = 0;
static int guest_cursor = 0;
static int guest_x, guest_y;
static SDL_Cursor *guest_sprite = NULL;
static SDL_PixelFormat host_format;
static int scaling_active = 0;
static Notifier mouse_mode_notifier;
////////////////////////

int screen_mod = 0;
extern bool g_androidMode;
int ds_width;
int ds_height;
////////////////////

unsigned char *fb_data=NULL;
int fb_size;
int header_len;
int fd_fb;
void mmap_fb(int w, int h)
{
printf("sdl.c - mmap_fb()\n");

    char w_char[5];
    char h_char[5];
    sprintf(w_char, "%d", w);
    int w_len = strlen(w_char);
    sprintf(h_char, "%d", h);
    int h_len = strlen(h_char);
    printf("w_len : %d, h_len : %d\n",w_len, h_len);
    header_len = 9 + w_len + h_len;
    printf("sdl.c - mmap_fb() -> header_len : %d\n", header_len);
    int fb_size = w * h * 3 + header_len;

	const char *filepath = "./fb_data.ppm";
	int fd = open(filepath, O_RDWR | O_CREAT | O_TRUNC, (mode_t)0600);
    fd_fb = fd;
    if (fd == -1)
    {
        perror("Error opening file for writing");
        exit(EXIT_FAILURE);
    }

    lseek(fd, fb_size-1, SEEK_SET);
    write(fd, "", 1);
    
    fb_data = mmap(0, fb_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fb_data == MAP_FAILED)
    {
        close(fd);
        perror("Error mmapping the file");
        exit(EXIT_FAILURE);
    }
    int header_index = 0;
    fb_data[header_index++] = 'P'; fb_data[header_index++] = '6'; fb_data[header_index++] = '\n';
    int i;
    for(i=0;i<w_len;i++)
    {
	fb_data[header_index++]=w_char[i];
    }
	fb_data[header_index++] = ' ';
    for(i=0;i<h_len;i++)
	fb_data[header_index++]=h_char[i];
    fb_data[header_index++] = '\n';
    fb_data[header_index++] = '2'; fb_data[header_index++] = '5'; fb_data[header_index++] = '5'; fb_data[header_index++] = '\n';
    if(header_len != header_index)
    {
	printf("header error!!\n");
	exit(1);
    }
}

void make_frameData(int w, int h)
{
printf("sdl.c - make_frameData() -> real_screen's bpp : %d, w : %d\n", real_screen->pitch / w, w);


	if(fb_data != NULL)
	{
		printf("mmap reset\n");
		close(fd_fb);
		munmap(fb_data, fb_size);
		fb_data = 0;
	}
if(g_androidMode){
	mmap_fb(w,h);
	printf("%d, %d, %d\n", w, h, header_len);
	printf("make_frameData() -> mmap finished\n");
	unsigned short *tmp = real_screen->pixels;
	int i;
	int n=5;
	for(i=0;i < w*h;i++)
	{
		int x1 = i%h;
		int y1 = i/h;
		int x2 = (w-1) - y1;
		int y2 = x1;
		int i2 = w*y2 + x2;
		unsigned char r = (unsigned char)((tmp[i] >> 11) & 31);
		unsigned char g = (unsigned char)((tmp[i] >> 5) & 63);
		unsigned char b = (unsigned char)(tmp[i] & 31);
		unsigned char R = (unsigned char)((int)r*255/31);
		unsigned char G = (unsigned char)((int)g*255/63);
		unsigned char B = (unsigned char)((int)b*255/31);
		if(i2 > 0){
		fb_data[i2*3 + 2 + header_len] = B;
		fb_data[i2*3 + 1 + header_len] = G;
		fb_data[i2*3 + header_len] = R;
		}
//		if(i >=1280*767 + 1277 && n-->0)
		
	}
	printf("make_frameData finished!!\n");
}
else {
    mmap_fb(w, h);
    if(real_screen->pitch / w == 4){
	char *tmp = real_screen->pixels;
	int i, j;

	for(i=0,j=0;i<w*h*4;i++)
	{
		if(i%4==0)
		{
			fb_data[j*3+2 + header_len] = tmp[i];
		}
		else if(i%4==1)
		{
			fb_data[j*3+1 + header_len] = tmp[i];
		}
		else if(i%4==2)
		{
			fb_data[j*3 + header_len] = tmp[i];
			j++;
		}
	}
    }
    else if(real_screen->pitch / w == 2){
	unsigned short *tmp = real_screen->pixels;
	int i;
	for(i=0;i < (w*h);i++)
	{
		unsigned char r = (unsigned char)((tmp[i] >> 11) & 31);
		unsigned char g = (unsigned char)((tmp[i] >> 5) & 63);
		unsigned char b = (unsigned char)(tmp[i] & 31);
		unsigned char R = (unsigned char)((int)r*255/31);
		unsigned char G = (unsigned char)((int)g*255/63);
		unsigned char B = (unsigned char)((int)b*255/31);
		fb_data[i*3 + 2 + header_len] = B;
		fb_data[i*3 + 1 + header_len] = G;
		fb_data[i*3 + header_len] = R;
	}
    }
}
printf("sdl.c - make_frameData()-> finish!!\n");
}

int get_pixel_index(int w, int x, int y)
{//2차원 인덱스를 1차원 인덱스로 변환하는 함수
	return w*y+x;
}

void Update_frameData(int recx, int recy, int recw, int rech)
{
printf("sdl.c - make_frameData() -> real_screen's bpp : %d, w : %d\n", real_screen->pitch / real_screen->w, real_screen->w);
    if(g_androidMode){
	//printf("update!!\n");
	unsigned short * tmp = real_screen->pixels;
	int y, x;
	for(y=recy; y<recy + rech ;y++)
	{
		for(x=recx;x<recx +recw;x++)
		{
			int i= y* real_screen->w + x;

			int x2 = (real_screen->h-1) - y;
			int y2 = x;
			int i2 = rech*y2 + x2;
			unsigned char r = (unsigned char)((tmp[i] >> 11) & 31);
			unsigned char g = (unsigned char)((tmp[i] >> 5) & 63);
			unsigned char b = (unsigned char)(tmp[i] & 31);
			unsigned char R = (unsigned char)((int)r*255/31);
			unsigned char G = (unsigned char)((int)g*255/63);
			unsigned char B = (unsigned char)((int)b*255/31);
			if(i2 + header_len <0)
			printf("%d, %d, %d, %d, %d\n", x, y, x2, y2, i2);
			fb_data[i2*3 + header_len] = R;
			fb_data[i2*3+1 + header_len] = G;
			fb_data[i2*3+2 + header_len] = B;
			
			
		}
	}
    }

//printf("x,y,w,h : %d, %d, %d, %d\n", recx, recy, recw, rech);
    else{
    if(real_screen->pitch / recw == 4){
	char *tmp = real_screen->pixels;
	int y,x;
	for(y=recy;y<recy+rech;y++)
	{
		for(x=recx;x<recx+recw;x++)
		{
			int pixel_index = get_pixel_index(recw, x, y);
			int pixel_index_32 = pixel_index * 4;
			int pixel_index_fb = pixel_index * 3;
			fb_data[pixel_index_fb+2 + header_len] = tmp[pixel_index_32];
			fb_data[pixel_index_fb+1 + header_len] = tmp[pixel_index_32+1];
			fb_data[pixel_index_fb + header_len] = tmp[pixel_index_32+2];
		}
	}	
    }

    else if(real_screen->pitch / recw == 2){
	unsigned short * tmp = real_screen->pixels;
	int y, x;
	for(y=recy; y<recy + rech ;y++)
	{
		for(x=recx;x<recx +recw;x++)
		{
			int pixel_index = get_pixel_index(recw, x, y);
			int pixel_index_fb = pixel_index*3;
			unsigned char r = (unsigned char)((tmp[pixel_index] >> 11) & 31);
			unsigned char g = (unsigned char)((tmp[pixel_index] >> 5) & 63);
			unsigned char b = (unsigned char)(tmp[pixel_index] & 31);
			unsigned char R = (unsigned char)((int)r*255/31);
			unsigned char G = (unsigned char)((int)g*255/63);
			unsigned char B = (unsigned char)((int)b*255/31);
			fb_data[pixel_index_fb + header_len] = R;
			fb_data[pixel_index_fb+1 + header_len] = G;
			fb_data[pixel_index_fb+2 + header_len] = B;
			
		}
	}	
    }
    }
}

void Save_frameData(char* filename)
{
printf("Save_frameData()\n");
	int fd;
	FILE *f;
//	char filename[100] = "/home/project/project_fb/sdl_fb";
	fd = qemu_open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0666);
	if(fd < 0)
		printf("file open error!\n");
	f = fdopen(fd, "wb");
	int ret;
	ret = fprintf(f, "P6\n%d %d\n%d\n", real_screen->w, real_screen->h, 255);
	if(ret <0)
		printf("print error\n");
	int wrt;
//	wrt = fwrite(fb_data, 1, real_screen->pitch * real_screen->h, f);
	wrt = fwrite(fb_data, 1, 4 * real_screen->w * real_screen->h, f);	
	if(wrt < 0)
		printf("written error\n");
	close(fd);
}


////////////////////////
#if 0
#define DEBUG_SDL
#endif

static void sdl_update(DisplayChangeListener *dcl,
                       int x, int y, int w, int h)
{
//printf("sdl.c - sdl_update() - The screen is updating in this function\n");
//printf("sdl.c - sdl_update() -> guest_screen.w : %d, h : %d\n", guest_screen->w, guest_screen->h);
    SDL_Rect rec;
    rec.x = x;
    rec.y = y;
    rec.w = w;
    rec.h = h;

#ifdef DEBUG_SDL
    printf("SDL: Updating x=%d y=%d w=%d h=%d (scaling: %d)\n",
           x, y, w, h, scaling_active);
#endif
    if (guest_screen) {
        if (!scaling_active) {
            SDL_BlitSurface(guest_screen, &rec, real_screen, &rec);
        } else {
            if (sdl_zoom_blit(guest_screen, real_screen, SMOOTHING_ON, &rec) < 0) {
//guest_screen은 guest_os의 화면 real_screen은 qemu console화면이다.
                fprintf(stderr, "Zoom blit failed\n");
                exit(1);
            }
        }
    }
    //Update_frameData(rec.x, rec.y, rec.h, rec.w);
    Update_frameData(rec.x, rec.y, rec.w, rec.h);
    SDL_UpdateRect(real_screen, rec.x, rec.y, rec.w, rec.h);
}

static void do_sdl_resize(int width, int height, int bpp)
{

printf("sdl.c - do_sdl_resize()\n");
    int flags;
    SDL_Surface *tmp_screen;

//#ifdef DEBUG_SDL
    printf("SDL: Resizing to %dx%d bpp %d\n", width, height, bpp);
//#endif

    flags = SDL_HWSURFACE | SDL_ASYNCBLIT | SDL_HWACCEL;
    if (gui_fullscreen) {
        flags |= SDL_FULLSCREEN;
    } else {
        flags |= SDL_RESIZABLE;
    }
    if (gui_noframe)
        flags |= SDL_NOFRAME;

    tmp_screen = SDL_SetVideoMode(width, height, bpp, flags);
    if (!real_screen) {
        if (!tmp_screen) {
            fprintf(stderr, "Could not open SDL display (%dx%dx%d): %s\n",
                    width, height, bpp, SDL_GetError());
            exit(1);
        }
    } else {
        /*
         * Revert to the previous video mode if the change of resizing or
         * resolution failed.
         */
        if (!tmp_screen) {
            fprintf(stderr, "Failed to set SDL display (%dx%dx%d): %s\n",
                    width, height, bpp, SDL_GetError());
            return;
        }
    }

    real_screen = tmp_screen;
}

static void sdl_switch(DisplayChangeListener *dcl,
                       DisplaySurface *new_surface)
{
printf("sdl.c - sdl_switch()\n");
    PixelFormat pf;

    /* temporary hack: allows to call sdl_switch to handle scaling changes */
    if (new_surface) {
        surface = new_surface;
    }
    pf = qemu_pixelformat_from_pixman(surface->format);

    if (!scaling_active) {
        do_sdl_resize(surface_width(surface), surface_height(surface), 0);
    } else if (real_screen->format->BitsPerPixel !=
               surface_bits_per_pixel(surface)) {
        do_sdl_resize(real_screen->w, real_screen->h,
                      surface_bits_per_pixel(surface));
    }

    if (guest_screen != NULL) {
        SDL_FreeSurface(guest_screen);
    }

#ifdef DEBUG_SDL
    printf("SDL: Creating surface with masks: %08x %08x %08x %08x\n",
           pf.rmask, pf.gmask, pf.bmask, pf.amask);
#endif

    guest_screen = SDL_CreateRGBSurfaceFrom
        (surface_data(surface),
         surface_width(surface), surface_height(surface),
         surface_bits_per_pixel(surface), surface_stride(surface),
         pf.rmask, pf.gmask,
         pf.bmask, pf.amask);
}

static bool sdl_check_format(DisplayChangeListener *dcl,
                             pixman_format_code_t format)
{
//printf("sdl.c - sdl_check_format()\n");
    /*
     * We let SDL convert for us a few more formats than,
     * the native ones. Thes are the ones I have tested.
     */
    return (format == PIXMAN_x8r8g8b8 ||
            format == PIXMAN_b8g8r8x8 ||
            format == PIXMAN_x1r5g5b5 ||
            format == PIXMAN_r5g6b5);
}

/* generic keyboard conversion */

#include "sdl_keysym.h"

static kbd_layout_t *kbd_layout = NULL;

static uint8_t sdl_keyevent_to_keycode_generic(const SDL_KeyboardEvent *ev)
{
    int keysym;
    /* workaround for X11+SDL bug with AltGR */
    keysym = ev->keysym.sym;
    if (keysym == 0 && ev->keysym.scancode == 113)
        keysym = SDLK_MODE;
    /* For Japanese key '\' and '|' */
    if (keysym == 92 && ev->keysym.scancode == 133) {
        keysym = 0xa5;
    }
    return keysym2scancode(kbd_layout, keysym) & SCANCODE_KEYMASK;
}

/* specific keyboard conversions from scan codes */

#if defined(_WIN32)

static uint8_t sdl_keyevent_to_keycode(const SDL_KeyboardEvent *ev)
{
    return ev->keysym.scancode;
}

#else

#if defined(SDL_VIDEO_DRIVER_X11)
#include <X11/XKBlib.h>

static int check_for_evdev(void)
{
    SDL_SysWMinfo info;
    XkbDescPtr desc = NULL;
    int has_evdev = 0;
    char *keycodes = NULL;

    SDL_VERSION(&info.version);
    if (!SDL_GetWMInfo(&info)) {
        return 0;
    }
    desc = XkbGetKeyboard(info.info.x11.display,
                          XkbGBN_AllComponentsMask,
                          XkbUseCoreKbd);
    if (desc && desc->names) {
        keycodes = XGetAtomName(info.info.x11.display, desc->names->keycodes);
        if (keycodes == NULL) {
            fprintf(stderr, "could not lookup keycode name\n");
        } else if (strstart(keycodes, "evdev", NULL)) {
            has_evdev = 1;
        } else if (!strstart(keycodes, "xfree86", NULL)) {
            fprintf(stderr, "unknown keycodes `%s', please report to "
                    "qemu-devel@nongnu.org\n", keycodes);
        }
    }

    if (desc) {
        XkbFreeKeyboard(desc, XkbGBN_AllComponentsMask, True);
    }
    if (keycodes) {
        XFree(keycodes);
    }
    return has_evdev;
}
#else
static int check_for_evdev(void)
{
	return 0;
}
#endif

static uint8_t sdl_keyevent_to_keycode(const SDL_KeyboardEvent *ev)
{
    int keycode;
    static int has_evdev = -1;

    if (has_evdev == -1)
        has_evdev = check_for_evdev();

    keycode = ev->keysym.scancode;

    if (keycode < 9) {
        keycode = 0;
    } else if (keycode < 97) {
        keycode -= 8; /* just an offset */
    } else if (keycode < 158) {
        /* use conversion table */
        if (has_evdev)
            keycode = translate_evdev_keycode(keycode - 97);
        else
            keycode = translate_xfree86_keycode(keycode - 97);
    } else if (keycode == 208) { /* Hiragana_Katakana */
        keycode = 0x70;
    } else if (keycode == 211) { /* backslash */
        keycode = 0x73;
    } else {
        keycode = 0;
    }
    return keycode;
}

#endif

static void reset_keys(void)
{
    int i;
    for(i = 0; i < 256; i++) {
        if (modifiers_state[i]) {
            qemu_input_event_send_key_number(dcl->con, i, false);
            modifiers_state[i] = 0;
        }
    }
}

static void sdl_process_key(SDL_KeyboardEvent *ev)
{
printf("sdl.c - sdl_process_key()\n");
    int keycode;

    if (ev->keysym.sym == SDLK_PAUSE) {
	printf("sdl_process_key() - send_key_qcode()\n");
        /* specific case */
        qemu_input_event_send_key_qcode(dcl->con, Q_KEY_CODE_PAUSE,
                                        ev->type == SDL_KEYDOWN);
        return;
    }

    if (kbd_layout) {
	printf("sdl_process_key() - keycode_generic()\n");
        keycode = sdl_keyevent_to_keycode_generic(ev);
    } else {
	//printf("sdl_process_key() - keycode()\n");
        keycode = sdl_keyevent_to_keycode(ev);
    }
	printf("sdl.c - sdl_process_key() -> keycode : %d\n", keycode);
    switch(keycode) {
    case 0x00:
        /* sent when leaving window: reset the modifiers state */
        reset_keys();
        return;
    case 0x2a:                          /* Left Shift */
    case 0x36:                          /* Right Shift */
    case 0x1d:                          /* Left CTRL */
    case 0x9d:                          /* Right CTRL */
    case 0x38:                          /* Left ALT */
    case 0xb8:                         /* Right ALT */
        if (ev->type == SDL_KEYUP)
            modifiers_state[keycode] = 0;
        else
            modifiers_state[keycode] = 1;
        break;
#define QEMU_SDL_VERSION ((SDL_MAJOR_VERSION << 8) + SDL_MINOR_VERSION)
#if QEMU_SDL_VERSION < 0x102 || QEMU_SDL_VERSION == 0x102 && SDL_PATCHLEVEL < 14
        /* SDL versions before 1.2.14 don't support key up for caps/num lock. */
    case 0x45: /* num lock */
    case 0x3a: /* caps lock */
        /* SDL does not send the key up event, so we generate it */
	printf("keycode : %d\n", keycode);
        qemu_input_event_send_key_number(dcl->con, keycode, true);
        qemu_input_event_send_key_number(dcl->con, keycode, false);
        return;
#endif
    }

    /* now send the key code */
    qemu_input_event_send_key_number(dcl->con, keycode,
                                     ev->type == SDL_KEYDOWN);
}

static void sdl_update_caption(void)
{
    char win_title[1024];
    char icon_title[1024];
    const char *status = "";

    if (!runstate_is_running())
        status = " [Stopped]";
    else if (gui_grab) {
        if (alt_grab)
            status = " - Press Ctrl-Alt-Shift to exit mouse grab";
        else if (ctrl_grab)
            status = " - Press Right-Ctrl to exit mouse grab";
        else
            status = " - Press Ctrl-Alt to exit mouse grab";
    }

    if (qemu_name) {
        snprintf(win_title, sizeof(win_title), "QEMU (%s)%s", qemu_name, status);
        snprintf(icon_title, sizeof(icon_title), "QEMU (%s)", qemu_name);
    } else {
        snprintf(win_title, sizeof(win_title), "QEMU%s", status);
        snprintf(icon_title, sizeof(icon_title), "QEMU");
    }

    SDL_WM_SetCaption(win_title, icon_title);
}

static void sdl_hide_cursor(void)
{
    if (!cursor_hide)
        return;

    if (qemu_input_is_absolute()) {
        SDL_ShowCursor(1);
        SDL_SetCursor(sdl_cursor_normal);
    } else {
        SDL_ShowCursor(1);
    }
}

static void sdl_show_cursor(void)
{
printf("sdl.c - sdl_show_cursor()-----------------------------------\n");
    if (!cursor_hide)
        return;

    if (!qemu_input_is_absolute() || !qemu_console_is_graphic(NULL)) {
        SDL_ShowCursor(1);
        if (guest_cursor &&
                (gui_grab || qemu_input_is_absolute() || absolute_enabled))
            SDL_SetCursor(guest_sprite);
        else
            SDL_SetCursor(sdl_cursor_normal);
    }
}

static void sdl_grab_start(void)
{
    /*
     * If the application is not active, do not try to enter grab state. This
     * prevents 'SDL_WM_GrabInput(SDL_GRAB_ON)' from blocking all the
     * application (SDL bug).
     */
    if (!(SDL_GetAppState() & SDL_APPINPUTFOCUS)) {
        return;
    }
    if (guest_cursor) {
        SDL_SetCursor(guest_sprite);
        if (!qemu_input_is_absolute() && !absolute_enabled) {
            SDL_WarpMouse(guest_x, guest_y);
        }
    } else
        sdl_hide_cursor();
    SDL_WM_GrabInput(SDL_GRAB_ON);
    gui_grab = 1;
    sdl_update_caption();
}

static void sdl_grab_end(void)
{
    SDL_WM_GrabInput(SDL_GRAB_OFF);
    gui_grab = 0;
    sdl_show_cursor();
    sdl_update_caption();
}

static void absolute_mouse_grab(void)
{
    int mouse_x, mouse_y;

    SDL_GetMouseState(&mouse_x, &mouse_y);
    if (mouse_x > 0 && mouse_x < real_screen->w - 1 &&
        mouse_y > 0 && mouse_y < real_screen->h - 1) {
        sdl_grab_start();
    }
}

static void sdl_mouse_mode_change(Notifier *notify, void *data)
{
    if (qemu_input_is_absolute()) {
        if (!absolute_enabled) {
            absolute_enabled = 1;
            if (qemu_console_is_graphic(NULL)) {
                absolute_mouse_grab();
            }
        }
    } else if (absolute_enabled) {
        if (!gui_fullscreen) {
            sdl_grab_end();
        }
        absolute_enabled = 0;
    }
}

static void sdl_send_mouse_event(int dx, int dy, int x, int y, int state)
{
printf("sdl_send_mouse_event -> dx : %d , dy : %d, x : %d, y : %d, state : %d\n", dx, dy, x, y, state);
    static uint32_t bmap[INPUT_BUTTON_MAX] = {
        [INPUT_BUTTON_LEFT]       = SDL_BUTTON(SDL_BUTTON_LEFT),
        [INPUT_BUTTON_MIDDLE]     = SDL_BUTTON(SDL_BUTTON_MIDDLE),
        [INPUT_BUTTON_RIGHT]      = SDL_BUTTON(SDL_BUTTON_RIGHT),
        [INPUT_BUTTON_WHEEL_UP]   = SDL_BUTTON(SDL_BUTTON_WHEELUP),
        [INPUT_BUTTON_WHEEL_DOWN] = SDL_BUTTON(SDL_BUTTON_WHEELDOWN),
    };
    static uint32_t prev_state;

    if (prev_state != state) {
	printf("sdl_send_mouse_event()-> prevstate !=state\n");
        qemu_input_update_buttons(dcl->con, bmap, prev_state, state);
        prev_state = state;
    }

    if (qemu_input_is_absolute()) {
	printf("sdl_send_mouse_event()->qemu_is_absolute()\n");
        qemu_input_queue_abs(dcl->con, INPUT_AXIS_X, x,
                             real_screen->w);
        qemu_input_queue_abs(dcl->con, INPUT_AXIS_Y, y,
                             real_screen->h);
    } else {
        if (guest_cursor) {
		printf("sdl_send_mouse_event_guest_cursor!!\n");
            x -= guest_x;
            y -= guest_y;
            guest_x += x;
            guest_y += y;
            dx = x;
            dy = y;
        }
	printf("sdl_send_mouse_event() -> !guest_cursor\n");
        qemu_input_queue_rel(dcl->con, INPUT_AXIS_X, dx);
        qemu_input_queue_rel(dcl->con, INPUT_AXIS_Y, dy);
    }
    qemu_input_event_sync();
}

static void sdl_scale(int width, int height)
{
printf("sdl.c - sld_scale()\n");
    int bpp = real_screen->format->BitsPerPixel;

#ifdef DEBUG_SDL
    printf("SDL: Scaling to %dx%d bpp %d\n", width, height, bpp);
#endif

    if (bpp != 16 && bpp != 32) {
        bpp = 32;
    }
    do_sdl_resize(width, height, bpp);
    scaling_active = 1;
}

static void toggle_full_screen(void)
{
printf("sdl.c - toggle_full_screen()\n");
    int width = surface_width(surface);
    int height = surface_height(surface);
    int bpp = surface_bits_per_pixel(surface);

    gui_fullscreen = !gui_fullscreen;
    if (gui_fullscreen) {
        gui_saved_width = real_screen->w;
        gui_saved_height = real_screen->h;
        gui_saved_scaling = scaling_active;

        do_sdl_resize(width, height, bpp);
        scaling_active = 0;

        gui_saved_grab = gui_grab;
        sdl_grab_start();
    } else {
        if (gui_saved_scaling) {
            sdl_scale(gui_saved_width, gui_saved_height);
        } else {
            do_sdl_resize(width, height, 0);
        }
        if (!gui_saved_grab || !qemu_console_is_graphic(NULL)) {
            sdl_grab_end();
        }
    }
    graphic_hw_invalidate(NULL);
    graphic_hw_update(NULL);
}

static void handle_keydown(SDL_Event *ev)
{
printf("keydown\n");
    int mod_state;
    int keycode;

    if (alt_grab) {
        mod_state = (SDL_GetModState() & (gui_grab_code | KMOD_LSHIFT)) ==
                    (gui_grab_code | KMOD_LSHIFT);
    } else if (ctrl_grab) {
        mod_state = (SDL_GetModState() & KMOD_RCTRL) == KMOD_RCTRL;
    } else {
        mod_state = (SDL_GetModState() & gui_grab_code) == gui_grab_code;
    }
    gui_key_modifier_pressed = mod_state;

    if (gui_key_modifier_pressed) {
        keycode = sdl_keyevent_to_keycode(&ev->key);
        switch (keycode) {
        case 0x21: /* 'f' key on US keyboard */
            toggle_full_screen();
            gui_keysym = 1;
            break;
        case 0x16: /* 'u' key on US keyboard */
            if (scaling_active) {
                scaling_active = 0;
                sdl_switch(dcl, NULL);
                graphic_hw_invalidate(NULL);
                graphic_hw_update(NULL);
            }
            gui_keysym = 1;
            break;
        case 0x02 ... 0x0a: /* '1' to '9' keys */
            /* Reset the modifiers sent to the current console */
            reset_keys();
            console_select(keycode - 0x02);
            gui_keysym = 1;
            if (gui_fullscreen) {
                break;
            }
            if (!qemu_console_is_graphic(NULL)) {
                /* release grab if going to a text console */
                if (gui_grab) {
                    sdl_grab_end();
                } else if (absolute_enabled) {
                    sdl_show_cursor();
                }
            } else if (absolute_enabled) {
                sdl_hide_cursor();
                absolute_mouse_grab();
            }
            break;
        case 0x1b: /* '+' */
        case 0x35: /* '-' */
            if (!gui_fullscreen) {
                int width = MAX(real_screen->w + (keycode == 0x1b ? 50 : -50),
                                160);
                int height = (surface_height(surface) * width) /
                    surface_width(surface);

                sdl_scale(width, height);
                graphic_hw_invalidate(NULL);
                graphic_hw_update(NULL);
                gui_keysym = 1;
            }
        default:
            break;
        }
    } else if (!qemu_console_is_graphic(NULL)) {
        int keysym = 0;

        if (ev->key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)) {
            switch (ev->key.keysym.sym) {
            case SDLK_UP:
                keysym = QEMU_KEY_CTRL_UP;
                break;
            case SDLK_DOWN:
                keysym = QEMU_KEY_CTRL_DOWN;
                break;
            case SDLK_LEFT:
                keysym = QEMU_KEY_CTRL_LEFT;
                break;
            case SDLK_RIGHT:
                keysym = QEMU_KEY_CTRL_RIGHT;
                break;
            case SDLK_HOME:
                keysym = QEMU_KEY_CTRL_HOME;
                break;
            case SDLK_END:
                keysym = QEMU_KEY_CTRL_END;
                break;
            case SDLK_PAGEUP:
                keysym = QEMU_KEY_CTRL_PAGEUP;
                break;
            case SDLK_PAGEDOWN:
                keysym = QEMU_KEY_CTRL_PAGEDOWN;
                break;
            default:
                break;
            }
        } else {
            switch (ev->key.keysym.sym) {
            case SDLK_UP:
                keysym = QEMU_KEY_UP;
                break;
            case SDLK_DOWN:
                keysym = QEMU_KEY_DOWN;
                break;
            case SDLK_LEFT:
                keysym = QEMU_KEY_LEFT;
                break;
            case SDLK_RIGHT:
                keysym = QEMU_KEY_RIGHT;
                break;
            case SDLK_HOME:
                keysym = QEMU_KEY_HOME;
                break;
            case SDLK_END:
                keysym = QEMU_KEY_END;
                break;
            case SDLK_PAGEUP:
                keysym = QEMU_KEY_PAGEUP;
                break;
            case SDLK_PAGEDOWN:
                keysym = QEMU_KEY_PAGEDOWN;
                break;
            case SDLK_BACKSPACE:
                keysym = QEMU_KEY_BACKSPACE;
                break;
            case SDLK_DELETE:
                keysym = QEMU_KEY_DELETE;
                break;
            default:
                break;
            }
        }
        if (keysym) {
            kbd_put_keysym(keysym);
        } else if (ev->key.keysym.unicode != 0) {
            kbd_put_keysym(ev->key.keysym.unicode);
        }
    }
    if (qemu_console_is_graphic(NULL) && !gui_keysym) {
        sdl_process_key(&ev->key);
    }
}



extern int play_test;
static void handle_keyup(SDL_Event *ev)
{
//////////////////////////////////////////
printf("keyup\n");
play_test++;

    int mod_state;

    if (!alt_grab) {
        mod_state = (ev->key.keysym.mod & gui_grab_code);
    } else {
        mod_state = (ev->key.keysym.mod & (gui_grab_code | KMOD_LSHIFT));
    }
    if (!mod_state && gui_key_modifier_pressed) {
        gui_key_modifier_pressed = 0;
        if (gui_keysym == 0) {
            /* exit/enter grab if pressing Ctrl-Alt */
            if (!gui_grab) {
                if (qemu_console_is_graphic(NULL)) {
                    sdl_grab_start();
                }
            } else if (!gui_fullscreen) {
                sdl_grab_end();
            }
            /* SDL does not send back all the modifiers key, so we must
             * correct it. */
            reset_keys();
            return;
        }
        gui_keysym = 0;
    }
    if (qemu_console_is_graphic(NULL) && !gui_keysym) {
        sdl_process_key(&ev->key);
    }
}

static void handle_mousemotion(SDL_Event *ev)
{
    int max_x, max_y;

    if (qemu_console_is_graphic(NULL) &&
        (qemu_input_is_absolute() || absolute_enabled)) {
	printf("handle_mousemotion() - 1\n");
        max_x = real_screen->w - 1;
        max_y = real_screen->h - 1;
        if (gui_grab && (ev->motion.x == 0 || ev->motion.y == 0 ||
            ev->motion.x == max_x || ev->motion.y == max_y)) {
            sdl_grab_end();
        }
        if (!gui_grab &&
            (ev->motion.x > 0 && ev->motion.x < max_x &&
            ev->motion.y > 0 && ev->motion.y < max_y)) {
            sdl_grab_start();
        }
    }
    if (gui_grab || qemu_input_is_absolute() || absolute_enabled) {
	printf("handle_mousemotion() - 2\n");
        sdl_send_mouse_event(ev->motion.xrel, ev->motion.yrel,
                             ev->motion.x, ev->motion.y, ev->motion.state);
    }
}

static void handle_mousebutton(SDL_Event *ev)
{
    int buttonstate = SDL_GetMouseState(NULL, NULL);
    SDL_MouseButtonEvent *bev;

    if (!qemu_console_is_graphic(NULL)) {
        return;
    }

    bev = &ev->button;
    if (!gui_grab && !qemu_input_is_absolute()) {
        if (ev->type == SDL_MOUSEBUTTONUP && bev->button == SDL_BUTTON_LEFT) {
            /* start grabbing all events */
            sdl_grab_start();
        }
    } else {
        if (ev->type == SDL_MOUSEBUTTONDOWN) {
            buttonstate |= SDL_BUTTON(bev->button);
        } else {
            buttonstate &= ~SDL_BUTTON(bev->button);
        }
        sdl_send_mouse_event(0, 0, bev->x, bev->y, buttonstate);
    }
}

static void handle_activation(SDL_Event *ev)
{
#ifdef _WIN32
    /* Disable grab if the window no longer has the focus
     * (Windows-only workaround) */
    if (gui_grab && ev->active.state == SDL_APPINPUTFOCUS &&
        !ev->active.gain && !gui_fullscreen) {
        sdl_grab_end();
    }
#endif
    if (!gui_grab && ev->active.gain && qemu_console_is_graphic(NULL) &&
        (qemu_input_is_absolute() || absolute_enabled)) {
        absolute_mouse_grab();
    }
    if (ev->active.state & SDL_APPACTIVE) {
        if (ev->active.gain) {
            /* Back to default interval */
            update_displaychangelistener(dcl, GUI_REFRESH_INTERVAL_DEFAULT);
        } else {
            /* Sleeping interval.  Not using the long default here as
             * sdl_refresh does not only update the guest screen, but
             * also checks for gui events. */
            update_displaychangelistener(dcl, 500);
        }
    }
}

extern int g_img_width;
extern int g_img_height;

static void sdl_refresh(DisplayChangeListener *dcl)
{
//printf("sdl.c - sdl_refresh()\n");
    SDL_Event ev1, *ev = &ev1;

    if (last_vm_running != runstate_is_running()) {
        last_vm_running = runstate_is_running();
        sdl_update_caption();
    }

    graphic_hw_update(NULL);
    SDL_EnableUNICODE(!qemu_console_is_graphic(NULL));

    while (SDL_PollEvent(ev)) {
	if(!screen_mod)
	{
            printf("sdl.c - sdl_refresh() - screen_mod\n");
	    screen_mod ++;
	    sdl_scale(g_img_width, g_img_height);
            //sdl_scale(ev->resize.w, ev->resize.h);
	    if(g_androidMode)
	    make_frameData(real_screen->h, real_screen->w);
	    else
	    make_frameData(real_screen->w, real_screen->h);
            graphic_hw_invalidate(NULL);
            graphic_hw_update(NULL);
	}
        switch (ev->type) {
        case SDL_VIDEOEXPOSE:
            sdl_update(dcl, 0, 0, real_screen->w, real_screen->h);
            break;
        case SDL_KEYDOWN:
            handle_keydown(ev);
            break;
        case SDL_KEYUP:
            handle_keyup(ev);
            break;
        case SDL_QUIT:
            if (!no_quit) {
                no_shutdown = 0;
                qemu_system_shutdown_request();
            }
            break;
        case SDL_MOUSEMOTION:
            handle_mousemotion(ev);
            break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            handle_mousebutton(ev);
            break;
        case SDL_ACTIVEEVENT:
            handle_activation(ev);
            break;
        case SDL_VIDEORESIZE:
	    make_frameData(real_screen->w, real_screen->h);
            sdl_scale(ev->resize.w, ev->resize.h);
            graphic_hw_invalidate(NULL);
            graphic_hw_update(NULL);
            break;
        default:
            break;
        }
    }
}

static void sdl_mouse_warp(DisplayChangeListener *dcl,
                           int x, int y, int on)
{
printf("sdl.c - sdl_mouse_warp()---------------------------\n");
    if (on) {
        if (!guest_cursor)
            sdl_show_cursor();
        if (gui_grab || qemu_input_is_absolute() || absolute_enabled) {
            SDL_SetCursor(guest_sprite);
            if (!qemu_input_is_absolute() && !absolute_enabled) {
                SDL_WarpMouse(x, y);
            }
        }
    } else if (gui_grab)
	//sdl_show_cursor();
        sdl_hide_cursor();
    guest_cursor = on;
    guest_x = x, guest_y = y;
}

static void sdl_mouse_define(DisplayChangeListener *dcl,
                             QEMUCursor *c)
{
printf("sdl.c - sdl_mouse_define()\n");
    uint8_t *image, *mask;
    int bpl;

    if (guest_sprite)
        SDL_FreeCursor(guest_sprite);

    bpl = cursor_get_mono_bpl(c);
    image = g_malloc0(bpl * c->height);
    mask  = g_malloc0(bpl * c->height);
    cursor_get_mono_image(c, 0x000000, image);
    cursor_get_mono_mask(c, 0, mask);
    guest_sprite = SDL_CreateCursor(image, mask, c->width, c->height, c->hot_x, c->hot_y);
    g_free(image);
    g_free(mask);

    if (guest_cursor &&
            (gui_grab || qemu_input_is_absolute() || absolute_enabled))
        SDL_SetCursor(guest_sprite);
}

static void sdl_cleanup(void)
{
    if (guest_sprite)
        SDL_FreeCursor(guest_sprite);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

static const DisplayChangeListenerOps dcl_ops = {
    .dpy_name             = "sdl",
    .dpy_gfx_update       = sdl_update,
    .dpy_gfx_switch       = sdl_switch,
    .dpy_gfx_check_format = sdl_check_format,
    .dpy_refresh          = sdl_refresh,
    .dpy_mouse_set        = sdl_mouse_warp,
    .dpy_cursor_define    = sdl_mouse_define,
};

void sdl_display_early_init(int opengl)
{
    if (opengl == 1 /* on */) {
        fprintf(stderr,
                "SDL1 display code has no opengl support.\n"
                "Please recompile qemu with SDL2, using\n"
                "./configure --enable-sdl --with-sdlabi=2.0\n");
    }
}
////////////////////////////////////////////////////////////////////////////////////
void touch_down_handler(int x, int y) 
{
printf("touch_down : %d, %d\n", x, y);
	//SDL_WarpMouse(x, y);
	sdl_send_mouse_event(0, 0, x, y , 1);

}

void touch_up_handler(int x, int y)
{
printf("touch_down : %d, %d\n", x, y);
	//SDL_WarpMouse(x, y);
	sdl_send_mouse_event(0, 0, x, y, 0);
}

#define ENTERKEY 28
#define L_CTRL 29
#define L_ALT 56
#define ESCKEY 1
#define L_SHIFT 42
#define F12KEY 88
#define F11KEY 87
#define KINPUT 1 //keyboard
#define MINPUT 0 //mouse
void key_down_command(int keycode)
{

	qemu_input_event_send_key_number(dcl->con, keycode, true);
}

void key_up_command(int keycode)
{
	qemu_input_event_send_key_number(dcl->con, keycode, false);
}

void back_button_down_handler()
{
	key_down_command(ESCKEY);
}

void back_button_up_handler()
{
	key_up_command(ESCKEY);
}
extern int sockfd;
int km[250];

void Make_keymap()
{
km[27] = 1; //esc
km[145] = 70; //scroll lock
km[112] = 59; km[113] = 60; km[114] = 61; km[115] = 62; km[116] = 63; km[117] = 64; km[118] = 65; km[119] = 66; km[120] = 67;
km[121] = 68; km[122] = 87; km[123] = 88;
km[192] = 41; km[49] = 2; km[50] = 3; km[51] = 4; km[52] = 5; km[53] = 6; km[54] = 7; km[55] = 8; km[56] = 9; km[57] = 10; km[48] = 11; km[189] = 12; km[187] = 13; km[220] = 43; km[8] = 14;
km[9] = 15; km[81] = 16; km[87] = 17; km[69] = 18; km[82] = 19; km[84] = 20; km[89] = 21; km[85] = 22; km[73] = 23; km[79] = 24; km[80] = 25; km[219] = 26; km[221] = 27; km[13] = ENTERKEY;
km[20] = 58; km[65] = 30; km[83] = 31; km[68] = 32; km[70] = 33; km[71] = 34; km[72] = 35; km[74] = 36; km[75] = 37; km[76] = 38; km[186] = 39; km[222] = 40;
km[16] = 42; km[90] = 44; km[88] = 45; km[67] = 46; km[86] = 47; km[66] = 48; km[78] = 49; km[77] = 50; km[188] = 51; km[190] = 52; km[191] = 53; km[16] = 54;
km[17] = 29; km[91] = 219; km[18] = 56; km[25] = 242; km[32] = 57; km[21] = 184; km[92] = 220; km[93] = 221; km[25] = 157;

km[37] = 203; km[40] = 208; km[39] = 205; km[38] = 200;

km[144] = 69; km[111] = 181; km[106] = 55; km[109] = 74;
km[103] = 71; km[104] = 72; km[105] = 73; km[107] = 78;
km[100] = 75; km[101] = 76; km[102] = 77;
km[97] = 79; km[98] = 80; km[99] = 81;
km[96] = 82; km[110] = 83;

km[45] = 210; km[36] = 199; km[33] = 201;
km[46] = 211; km[35] = 207; km[34] = 209;
}

extern double g_scaleConstant_X;
extern double g_scaleConstant_Y;

void* thread_sdl(void * arg)
{
printf("thread_sdl_running!!!!!!---------------------------\n");
printf("Making key map!!\n");
Make_keymap();
sleep(6);

key_down_command(ENTERKEY);
key_up_command(ENTERKEY);
printf("press_enter\n");
if(g_androidMode)
sleep(12);

if(g_androidMode){
key_down_command(F11KEY);
key_up_command(F11KEY);
key_down_command(F11KEY);
key_up_command(F11KEY);
}
//while(0)
if(socket_client_open()) printf("connect!!!------------------\n");
else printf("socket is not connected------------------\n");

while(sockfd > 0)
{
    char buf[100];
    char* temp;
    int x;
    int y;
    int state;
    int type;
    int prevstate=0;
    //printf("수신 대기중...\n");
    read(sockfd, buf, 100);
	printf("event : %s\n", buf);
    temp = strtok(buf, " ");
    type = atoi(temp);
    if(type == MINPUT){
	temp = strtok(NULL, " ");
	x = atoi(temp);
	x = x * g_scaleConstant_X; //scailing

	temp = strtok(NULL, " ");
	y = atoi(temp);
	y = y * g_scaleConstant_Y; //scailing

	temp = strtok(NULL, " ");
	state = atoi(temp);
	printf("client message : %s\n", buf);
	printf("client message : %d, %d, %d\n",x,y,state);
	
	if(g_androidMode == true)
	{
	    int tempx = y; int tempy = real_screen->h - x;
	    x = tempx; y = tempy;
	}
	sdl_send_mouse_event(0, 0, x, y, state);
	//if(state != prevstate)
	//    usleep(10000);
	//prevstate = state;
    }
    else if(type == KINPUT)
    {
	temp = strtok(NULL, " ");
	state = atoi(temp);
	int keycode;
	temp = strtok(NULL, " ");
	keycode = atoi(temp);
	keycode = km[keycode];
	if(state == 0)
	    key_up_command(keycode);
	else if(state == 1)
	    key_down_command(keycode);
    }
}

}
/////////////////////////////////////////////////////////////////////////////////////////////
void sdl_display_init(DisplayState *ds, int full_screen, int no_frame)
{
printf("sdl.c - sdl_display_init()\n");

pthread_t th_buffer;
int state;
state = pthread_create(&th_buffer, NULL, thread_sdl, NULL);  //마지막인자는 전달값을 넣으면 된다(void*) 여기선 전달 값 없다.
if(state != 0)
{
	printf("thread sdl error!!\n");
}

    int flags;
    uint8_t data = 0;
    const SDL_VideoInfo *vi;
    char *filename;

#if defined(__APPLE__)
    /* always use generic keymaps */
    if (!keyboard_layout)
        keyboard_layout = "en-us";
#endif
    if(keyboard_layout) {
        kbd_layout = init_keyboard_layout(name2keysym, keyboard_layout);
        if (!kbd_layout)
            exit(1);
    }

printf("sdl.c - sdl_display_init() - 1\n");
    if (no_frame)
        gui_noframe = 1;

    if (!full_screen) {
        setenv("SDL_VIDEO_ALLOW_SCREENSAVER", "1", 0);
    }
#ifdef __linux__
    /* on Linux, SDL may use fbcon|directfb|svgalib when run without
     * accessible $DISPLAY to open X11 window.  This is often the case
     * when qemu is run using sudo.  But in this case, and when actually
     * run in X11 environment, SDL fights with X11 for the video card,
     * making current display unavailable, often until reboot.
     * So make x11 the default SDL video driver if this variable is unset.
     * This is a bit hackish but saves us from bigger problem.
     * Maybe it's a good idea to fix this in SDL instead.
     */
    setenv("SDL_VIDEODRIVER", "x11", 0);
#endif

    /* Enable normal up/down events for Caps-Lock and Num-Lock keys.
     * This requires SDL >= 1.2.14. */
    setenv("SDL_DISABLE_LOCK_KEYS", "1", 1);

    flags = SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE;
    if (SDL_Init (flags)) {
        fprintf(stderr, "Could not initialize SDL(%s) - exiting\n",
                SDL_GetError());
        exit(1);
    }

    vi = SDL_GetVideoInfo();
    host_format = *(vi->vfmt);

    /* Load a 32x32x4 image. White pixels are transparent. */
    filename = qemu_find_file(QEMU_FILE_TYPE_BIOS, "qemu-icon.bmp");


    if (filename) {
        SDL_Surface *image = SDL_LoadBMP(filename);
        if (image) {
            uint32_t colorkey = SDL_MapRGB(image->format, 255, 255, 255);
            SDL_SetColorKey(image, SDL_SRCCOLORKEY, colorkey);
            SDL_WM_SetIcon(image, NULL);
        }
        g_free(filename);
    }

    if (full_screen) {
        gui_fullscreen = 1;
        sdl_grab_start();
    }


    dcl = g_malloc0(sizeof(DisplayChangeListener));
    dcl->ops = &dcl_ops;
    register_displaychangelistener(dcl);

    mouse_mode_notifier.notify = sdl_mouse_mode_change;
    qemu_add_mouse_mode_change_notifier(&mouse_mode_notifier);

    sdl_update_caption();
    SDL_EnableKeyRepeat(250, 50);
    gui_grab = 0;

    sdl_cursor_hidden = SDL_CreateCursor(&data, &data, 8, 1, 0, 0);
    sdl_cursor_normal = SDL_GetCursor();
printf("sdl.c - sdl_display_init()_exit\n");
    atexit(sdl_cleanup);
}
