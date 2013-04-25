// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <sys/mman.h>
#include <sys/fbio.h>
#include <sys/consio.h>
#include <sys/kbio.h>
#include <sys/termios.h>
#include <AS3/AS3.h>

extern int tprintf(const char *, ...);

int VGL_Support_VESA_FULL_1024 = 0;

int __avm2_vgl_fb_w = 1024, __avm2_vgl_fb_h = 768, __avm2_vgl_fb_bpp = 8;
void *__avm2_stdin_mapping = 0;
int *__avm2_vgl_argb_buffer = 0;

void* mmap_file(void *addr, size_t len, int prot, int flags, int fd, off_t o2)
{
  void *result = 0;

  if(fd == 0)
  {
    result = __sys_mmap(addr, len, prot, MAP_ANON, -1, o2);
    if(result)
    {
        __avm2_stdin_mapping = result;
        __avm2_vgl_argb_buffer = (int*)malloc(__avm2_vgl_fb_w * __avm2_vgl_fb_h * 4);
#if MMAP_LOG
        tprintf("mmap fd0: %p (%d, %d)\n", __avm2_stdin_mapping, len, __avm2_vgl_fb_w * __avm2_vgl_fb_h * 4);
#endif
    }
  }
  return result;
}

int vgl_cur_mx = 0, vgl_cur_my = 0, vgl_cur_buttons = 0;

int vglttyioctl(int fd, int cmd, void *ptr1, void *ptr2)
{
    vid_info_t *vidt;
    video_info_t *vit;
    video_adapter_info_t *vait;
    keymap_t *km;
    mouse_info_t *mit;
    int i,j;
    const int sz = __avm2_vgl_fb_w * __avm2_vgl_fb_h * (__avm2_vgl_fb_bpp/8);

    switch(cmd) {
    case VT_GETMODE:
        ((vtmode_t*)ptr1)->mode = VT_PROCESS;
        break;
    case CONS_CURRENT:
        *((int*)ptr1) = KD_OTHER; // we try to look like a large vesa display
        break;
    case CONS_GET:
        *(int*)ptr1 = M_VESA_CG1024x768;
        break;
    case CONS_GETINFO:
        vidt = (vid_info_t*)ptr1;
        vidt->mv_csz = __avm2_vgl_fb_w;
        vidt->mv_rsz = __avm2_vgl_fb_h;
        break;
    case CONS_ADPINFO:
        vait = (video_adapter_info_t*)ptr1;
        vait->va_buffer_size = sz;
        vait->va_line_width = __avm2_vgl_fb_w;
        vait->va_window_size = sz;
        break;
    case CONS_MODEINFO:
        vit = (video_info_t*)ptr1;
        
        switch(vit->vi_mode) {
        case M_VESA_FULL_1024:
            if(VGL_Support_VESA_FULL_1024) {
                vit->vi_mode = M_VESA_FULL_1024;
                vit->vi_flags = V_INFO_COLOR | V_INFO_GRAPHICS | V_INFO_LINEAR | V_INFO_VESA;
                vit->vi_width = __avm2_vgl_fb_w;
                vit->vi_height = __avm2_vgl_fb_h;
                vit->vi_cwidth = __avm2_vgl_fb_w;
                vit->vi_cheight = __avm2_vgl_fb_h;
                vit->vi_depth = __avm2_vgl_fb_bpp = 32;
                
                vit->vi_pixel_fields[0] = 16;
                vit->vi_pixel_fields[1] = 8;
                vit->vi_pixel_fields[2] = 0;
                vit->vi_pixel_fields[3] = 24;

                vit->vi_pixel_fsizes[0] = 8;
                vit->vi_pixel_fsizes[1] = 8;
                vit->vi_pixel_fsizes[2] = 8;
                vit->vi_pixel_fsizes[3] = 0;

                vit->vi_mem_model = V_INFO_MM_DIRECT;
                vit->vi_pixel_size = 4;

                vit->vi_planes = 1;
                vit->vi_window = 0xFFFFFF; // TODO
                vit->vi_window_size = sz; // TODO
                vit->vi_window_gran = 0; // TODO
                vit->vi_buffer = 0xFFFFFF; // TODO
                vit->vi_buffer_size = sz; // TODO
                break;
            }
        default:
            vit->vi_mode = M_VESA_CG1024x768;
            vit->vi_flags = V_INFO_COLOR | V_INFO_GRAPHICS | V_INFO_LINEAR | V_INFO_VESA;
            vit->vi_width = __avm2_vgl_fb_w;
            vit->vi_height = __avm2_vgl_fb_h;
            vit->vi_cwidth = __avm2_vgl_fb_w;
            vit->vi_cheight = __avm2_vgl_fb_h;
            vit->vi_depth = __avm2_vgl_fb_bpp = 8;
            vit->vi_planes = 1; // TODO
            vit->vi_window = 0xFFFFFFFF; // TODO
            vit->vi_window_size = sz; // TODO
            vit->vi_window_gran = 0; // TODO
            vit->vi_buffer = 0xFFFFFFFF; // TODO
            vit->vi_buffer_size = sz; // TODO
            vit->vi_mem_model = V_INFO_MM_PACKED;
            vit->vi_pixel_size = 1;
            break;
        }
        break;
    case GIO_KEYMAP:
        km = (keymap_t*)ptr1;
        km->n_keys = NUM_KEYS;
        for(i=0; i<NUM_KEYS; i++) {
            for(j=0; j<NUM_STATES; j++) {
                km->key[i].map[j] = 0; // ???
            }
            km->key[i].spcl = 0;
            km->key[i].flgs = 0;
        }
        break;
    case CONS_MOUSECTL:
        mit = (mouse_info_t*)ptr1;
        if(mit->operation == MOUSE_GETINFO) {
            mit->u.data.buttons = vgl_cur_buttons;
            mit->u.data.x = vgl_cur_mx;
            mit->u.data.y = vgl_cur_my;
        }
        break;
    case KDGKBMODE:
        *(int*)ptr1 = K_XLATE; // TODO
        break;
    case KDSKBMODE:
    case VT_SETMODE:
    case VT_WAITACTIVE:
    case KDENABIO:
    case KDDISABIO:
    case KDSETMODE:
    case VT_RELDISP:
    case TIOCGETA:
    case TIOCSETA:
    case TIOCSETAW:
    case TIOCSETAF:
    case TIOCSPGRP:
    case TIOCGSID:
    case TIOCSCTTY:
    case TIOCSBRK:
    case TIOCDRAIN:
    case TIOCFLUSH:
    case TIOCSTOP:
    case TIOCSTART:
        // ignore these for now
        break;
    default:
        tprintf("unhandled ioctl cmd %d on fd %d\n", cmd, fd);
        break;
    }

    return 0;
}

