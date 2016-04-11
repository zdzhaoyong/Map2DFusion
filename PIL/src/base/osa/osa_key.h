/******************************************************************************

  Robot Toolkit ++ (RTK++)

  Copyright (c) 2007-2013 Shuhui Bu <bushuhui@nwpu.edu.cn>
  http://www.adv-ci.com

  ----------------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

*******************************************************************************/

#ifndef __RTK_OSA_KEY_H__
#define __RTK_OSA_KEY_H__

namespace pi {

/******************************************************************************
 *  PC-Keyboard
 *****************************************************************************/
// mouse keys
#define OSA_VK_M_LEFT                   0x0201
#define OSA_VK_M_CENTER                 0x0202
#define OSA_VK_M_RIGHT                  0x0203
#define OSA_VK_M_WHEEL                  0x0204

// keyboard keys
#define OSA_VK_ENTER                    0x0104
#define OSA_VK_BACK_SPACE               0x0105
#define OSA_VK_TAB                      0x0106
#define OSA_VK_CANCEL                   0x0107
#define OSA_VK_CLEAR                    0x0108              /*erase*/
#define OSA_VK_SHIFT_L                  0x0109
#define OSA_VK_SHIFT_R                  0x010a
#define OSA_VK_CONTROL_L                0x010b
#define OSA_VK_CONTROL_R                0x010c
#define OSA_VK_ALT_L                    0x010d
#define OSA_VK_ALT_R                    0x010e
#define OSA_VK_META_L                   0x010f
#define OSA_VK_META_R                   0x0110
#define OSA_VK_BREAK                    0x0111
#define OSA_VK_CAPS_LOCK                0x0112
#define OSA_VK_ESCAPE                   0x0113              /*  exit key */
#define OSA_VK_SPACE                    0x0114
#define OSA_VK_PAGE_UP                  0x0115
#define OSA_VK_PAGE_DOWN                0x0116
#define OSA_VK_GOTO_END                 0x0117
#define OSA_VK_HOME                     0x0118
#define OSA_VK_INSERT                   0x0119
#define OSA_VK_DELETE                   0x011a
#define OSA_VK_LEFT                     0x011b
#define OSA_VK_RIGHT                    0x011c
#define OSA_VK_UP                       0x011d     
#define OSA_VK_DOWN                     0x011e
#define OSA_VK_HELP                     0x011f

// alphabelt keys
#define OSA_VK_A                        'A'
#define OSA_VK_a                        'a'

#define OSA_VK_B                        'B'
#define OSA_VK_b                        'b'

#define OSA_VK_C                        'C'
#define OSA_VK_c                        'c'

#define OSA_VK_D                        'D'
#define OSA_VK_d                        'd'

#define OSA_VK_E                        'E'
#define OSA_VK_e                        'e'

#define OSA_VK_F                        'F'
#define OSA_VK_f                        'f'

#define OSA_VK_G                        'G'
#define OSA_VK_g                        'g'

#define OSA_VK_H                        'H'
#define OSA_VK_h                        'h'

#define OSA_VK_I                        'I'
#define OSA_VK_i                        'i'

#define OSA_VK_J                        'J'
#define OSA_VK_j                        'j'

#define OSA_VK_K                        'K'
#define OSA_VK_k                        'k'

#define OSA_VK_L                        'L'
#define OSA_VK_l                        'l'

#define OSA_VK_M                        'M'
#define OSA_VK_m                        'm'

#define OSA_VK_N                        'N'
#define OSA_VK_n                        'n'

#define OSA_VK_O                        'O'
#define OSA_VK_o                        'o'

#define OSA_VK_P                        'P'
#define OSA_VK_p                        'p'

#define OSA_VK_Q                        'Q'
#define OSA_VK_q                        'q'

#define OSA_VK_R                        'R'
#define OSA_VK_r                        'r'

#define OSA_VK_S                        'S'
#define OSA_VK_s                        's'

#define OSA_VK_T                        'T'
#define OSA_VK_t                        't'

#define OSA_VK_U                        'U'
#define OSA_VK_u                        'u'

#define OSA_VK_V                        'V'
#define OSA_VK_v                        'v'

#define OSA_VK_W                        'W'
#define OSA_VK_w                        'w'

#define OSA_VK_X                        'X'
#define OSA_VK_x                        'x'

#define OSA_VK_Y                        'Y'
#define OSA_VK_y                        'y'

#define OSA_VK_Z                        'Z'
#define OSA_VK_z                        'z'

// number keys
#define OSA_VK_0                        0x30
#define OSA_VK_1                        0x31
#define OSA_VK_2                        0x32
#define OSA_VK_3                        0x33
#define OSA_VK_4                        0x34
#define OSA_VK_5                        0x35
#define OSA_VK_6                        0x36
#define OSA_VK_7                        0x37
#define OSA_VK_8                        0x38
#define OSA_VK_9                        0x39

#define OSA_VK_COMMA                    0x0140
#define OSA_VK_PERIOD                   0x0141
#define OSA_VK_SLASH                    0x0142
#define OSA_VK_BACK_SLASH               0x0143
#define OSA_VK_QUOTE                    0x0144
#define OSA_VK_BACK_QUOTE               0x0145
#define OSA_VK_SEMICOLON                0x0146
#define OSA_VK_MINUS                    0x0147
#define OSA_VK_EQUALS                   0x0148
#define OSA_VK_OPEN_BRACKET             0x0149
#define OSA_VK_CLOSE_BRACKET            0x014a
#define OSA_VK_NUMPAD0                  0x014b
#define OSA_VK_NUMPAD1                  0x014c
#define OSA_VK_NUMPAD2                  0x014d
#define OSA_VK_NUMPAD3                  0x014e
#define OSA_VK_NUMPAD4                  0x014f
#define OSA_VK_NUMPAD5                  0x0150
#define OSA_VK_NUMPAD6                  0x0151
#define OSA_VK_NUMPAD7                  0x0152
#define OSA_VK_NUMPAD8                  0x0153
#define OSA_VK_NUMPAD9                  0x0154
#define OSA_VK_MULTIPLY                 0x0155
#define OSA_VK_DIVIDE                   0x0156
#define OSA_VK_ADD                      0x0157
#define OSA_VK_SUBTRACT                 0x0158
#define OSA_VK_DECIMAL                  0x0159

#define OSA_VK_F1                       0x015a
#define OSA_VK_F2                       0x015b
#define OSA_VK_F3                       0x015c
#define OSA_VK_F4                       0x015d
#define OSA_VK_F5                       0x015e
#define OSA_VK_F6                       0x015f
#define OSA_VK_F7                       0x0160
#define OSA_VK_F8                       0x0161
#define OSA_VK_F9                       0x0162
#define OSA_VK_F10                      0x0163
#define OSA_VK_F11                      0x0164
#define OSA_VK_F12                      0x0165

#define OSA_VK_PRINTSCREEN              0x0166
#define OSA_VK_NUM_LOCK                 0x0167
#define OSA_VK_SCROLL_LOCK              0x0168
#define OSA_VK_ACCEPT                   0x0169
#define OSA_VK_CONVERT                  0x016a
#define OSA_VK_NONCONVERT               0x016b
#define OSA_VK_MODE_CHANGE              0x016c
#define OSA_VK_KANA                     0x016d
#define OSA_VK_KANJI                    0x016e



/******************************************************************************
 *  Remote control keys
 *****************************************************************************/
#define OSA_VK_COLOR_KEY_0              OSA_VK_F1
#define OSA_VK_COLOR_KEY_1              OSA_VK_F2
#define OSA_VK_COLOR_KEY_2              OSA_VK_F3
#define OSA_VK_COLOR_KEY_3              OSA_VK_F4
#define OSA_VK_COLOR_KEY_4              OSA_VK_F5
#define OSA_VK_COLOR_KEY_5              OSA_VK_F6

#define OSA_VK_POWER                    OSA_VK_ESCAPE
#define OSA_VK_MENU                     OSA_VK_m
#define OSA_VK_OK                       OSA_VK_ENTER
#define OSA_VK_DIMMER                   OSA_VK_d
#define OSA_VK_WINK                     OSA_VK_w
#define OSA_VK_REWIND                   OSA_VK_r
#define OSA_VK_STOP                     OSA_VK_s
#define OSA_VK_PAUSE                    OSA_VK_u
#define OSA_VK_EJECT_INSERT             OSA_VK_e
#define OSA_VK_PLAY                     OSA_VK_p
#define OSA_VK_RECORD                   OSA_VK_c
#define OSA_VK_FAST_FWD                 OSA_VK_f
#define OSA_VK_SPEED_UP                 OSA_VK_ADD
#define OSA_VK_SPEED_DOWN               OSA_VK_MINUS
#define OSA_VK_SPEED_RESET              0x021e
#define OSA_VK_RSPEED_NEXT              0x021f
#define OSA_VK_GOTO_START               0x0220
#define OSA_VK_END                      0x0221
#define OSA_VK_PREV_TRACK               0x0222
#define OSA_VK_NEXT_TRACK               0x0223
#define OSA_VK_RANDOM                   0x0224
#define OSA_VK_CHANNEL_UP               OSA_VK_UP
#define OSA_VK_CHANNEL_DOWN             OSA_VK_DOWN

#define OSA_VK_SCAN_CHANNEL             OSA_VK_NULL
#define OSA_VK_PINP_TOGGLE              OSA_VK_NULL
#define OSA_VK_SPLIT_SCREEN_TOGGLE      OSA_VK_NULL
#define OSA_VK_DISPLAY_SWAP             OSA_VK_a
#define OSA_VK_SCREEN_MODE_NEXT         OSA_VK_NULL
#define OSA_VK_VIDEO_MODE_NEXT          OSA_VK_NULL
#define OSA_VK_VOLUME_UP                0x025c
#define OSA_VK_VOLUME_DOWN              0x025d
#define OSA_VK_MUTE_TOGGLE              0x025b
#define OSA_VK_SOUND_MODE_NEXT          0x0255
#define OSA_VK_SOUND_BAL_RIGHT          OSA_VK_NULL
#define OSA_VK_SOUND_BAL_LEFT           OSA_VK_NULL
#define OSA_VK_SOUND_FADE_FRONT         OSA_VK_NULL
#define OSA_VK_SOUND_FADE_REAR          OSA_VK_NULL
#define OSA_VK_SOUND_BASS_UP            OSA_VK_NULL
#define OSA_VK_SOUND_BASS_DOWN          OSA_VK_NULL
#define OSA_VK_INFO                     OSA_VK_NULL
#define OSA_VK_GUIDE                    0x0C03
#define OSA_VK_TELETEXT                 0x0C06
#define OSA_VK_SUBTITLE                 OSA_VK_NULL
#define OSA_VK_SERVICE_NAVIGATOR        0x0C02

} // end namespace pi

#endif /* end of __RTK_OSA_KEY_H__ */

