#ifndef YE_KEYBOARD_HPP
#define YE_KEYBOARD_HPP

#include <map>
#include <sstream>

namespace YE {

    class Keyboard {

        const static uint16_t kKeyCount = 287;

        public:
            enum class Key : uint16_t;

            enum class State : uint8_t {
                PRESSED = 0 ,
                BLOCKED ,
                HELD ,
                RELEASED
            };

            struct KeyState {
                State current_state = State::RELEASED;
                State previous_state = State::RELEASED;
                uint32_t frames_held = 0;
            };

            static void Initialize();
            static void Update();

            inline static KeyState GetKeyState(Key key) { return keys[key]; }

            inline static bool Pressed(Key key) { return keys[key].current_state == State::PRESSED; }
            inline static bool Blocked(Key key) { return keys[key].current_state == State::BLOCKED; }
            inline static bool Held(Key key) { return keys[key].current_state == State::HELD; }
            inline static bool KeyDown(Key key) { 
                return keys[key].current_state == State::PRESSED  || 
                       keys[key].current_state == State::BLOCKED ||
                       keys[key].current_state == State::HELD; 
            }
            inline static bool Released(Key key) { return keys[key].current_state == State::RELEASED; }

            inline static bool LCtrlLayer() { 
                return Blocked(Key::YE_LCTRL) || Held(Key::YE_LCTRL);
            }
            inline static bool RCtrlLayer() { 
                return Blocked(Key::YE_RCTRL) || Held(Key::YE_RCTRL);
            }

            inline static bool LAltLayer() { 
                return Blocked(Key::YE_LALT) || Held(Key::YE_LALT);
            }
            inline static bool RAltLayer() { 
                return Blocked(Key::YE_RALT) || Held(Key::YE_RALT);
            }

            inline static bool LCtrlShiftLayer() { 
                return LCtrlLayer() && (Blocked(Key::YE_LSHIFT) || Held(Key::YE_LSHIFT));
            }
            inline static bool RCtrlShiftLayer() { 
                return RCtrlLayer() && (Blocked(Key::YE_RSHIFT) || Held(Key::YE_RSHIFT));
            }

            inline static bool LCtrlAltLayer() { 
                return LCtrlLayer() && (Blocked(Key::YE_LALT) || Held(Key::YE_LALT));
            }
            inline static bool RCtrlAltLayer() { 
                return RCtrlLayer() && (Blocked(Key::YE_RALT) || Held(Key::YE_RALT));
            }

            inline static bool LCtrlAltShiftLayer() { 
                return LCtrlLayer() && LAltLayer() && (Blocked(Key::YE_LSHIFT) || Held(Key::YE_LSHIFT));
            }
            inline static bool RCtrlAltShiftLayer() { 
                return RCtrlLayer() && RAltLayer() && (Blocked(Key::YE_RSHIFT) || Held(Key::YE_RSHIFT));
            }

            inline static bool LCtrlLayerKey(Key key) { 
                return LCtrlLayer() && Pressed(key);
            }
            inline static bool RCtrlLayerKey(Key key)  { 
                return RCtrlLayer() && Pressed(key); 
            }

            inline static bool LAltLayerKey(Key key) { 
                return LAltLayer() && Pressed(key);
            }   
            inline static bool RAltLayerKey(Key key) { 
                return RAltLayer() && Pressed(key);
            }

            inline static bool LCtrlShiftLayerKey(Key key) { 
                return LCtrlShiftLayer() && Pressed(key); 
            }
            inline static bool RCtrlShiftLayerKey(Key key) { 
                return RCtrlShiftLayer() && Pressed(key);
            }

            inline static bool LCtrlAltLayerKey(Key key) { 
                return LCtrlAltLayer() && Pressed(key);
            }
            inline static bool RCtrlAltLayerKey(Key key) { 
                return RCtrlAltLayer() && Pressed(key);
            }
            
            inline static bool LCtrlAltShiftLayerKey(Key key) { 
                return LCtrlAltShiftLayer() && Pressed(key);
            }

            inline static bool RCtrlAltShiftLayerKey(Key key) { 
                return RCtrlAltShiftLayer() && Pressed(key);
            }

        private:
            
            // const uint8_t* state = nullptr;
            static std::map<Key , KeyState> keys;

        public:

            enum class Key : uint16_t {
                YE_UNKNOWN = 0 ,
                YE_UNKNOWN1 = 1 ,
                YE_UNKNOWN2 = 2 ,
                YE_UNKNOWN3 = 3 ,

                YE_A = 4,
                YE_B = 5,
                YE_C = 6,
                YE_D = 7,
                YE_E = 8,
                YE_F = 9,
                YE_G = 10,
                YE_H = 11,
                YE_I = 12,
                YE_J = 13,
                YE_K = 14,
                YE_L = 15,
                YE_M = 16,
                YE_N = 17,
                YE_O = 18,
                YE_P = 19,
                YE_Q = 20,
                YE_R = 21,
                YE_S = 22,
                YE_T = 23,
                YE_U = 24,
                YE_V = 25,
                YE_W = 26,
                YE_X = 27,
                YE_Y = 28,
                YE_Z = 29,

                YE_1 = 30,
                YE_2 = 31,
                YE_3 = 32,
                YE_4 = 33,
                YE_5 = 34,
                YE_6 = 35,
                YE_7 = 36,
                YE_8 = 37,
                YE_9 = 38,
                YE_0 = 39,

                YE_RETURN = 40,
                YE_ESCAPE = 41,
                YE_BACKSPACE = 42,
                YE_TAB = 43,
                YE_SPACE = 44,

                YE_MINUS = 45,
                YE_EQUALS = 46,
                YE_LEFTBRACKET = 47,
                YE_RIGHTBRACKET = 48,
                YE_BACKSLASH = 49, 
                YE_NONUSHASH = 50, 
                YE_SEMICOLON = 51,
                YE_APOSTROPHE = 52,
                YE_GRAVE = 53, 
                YE_COMMA = 54,
                YE_PERIOD = 55,
                YE_SLASH = 56,

                YE_CAPSLOCK = 57,

                YE_F1 = 58,
                YE_F2 = 59,
                YE_F3 = 60,
                YE_F4 = 61,
                YE_F5 = 62,
                YE_F6 = 63,
                YE_F7 = 64,
                YE_F8 = 65,
                YE_F9 = 66,
                YE_F10 = 67,
                YE_F11 = 68,
                YE_F12 = 69,

                YE_PRINTSCREEN = 70,
                YE_SCROLLLOCK = 71,
                YE_PAUSE = 72,
                YE_INSERT = 73, 
                YE_HOME = 74,
                YE_PAGEUP = 75,
                YE_DELETE = 76,
                YE_END = 77,
                YE_PAGEDOWN = 78,
                YE_RIGHT = 79,
                YE_LEFT = 80,
                YE_DOWN = 81,
                YE_UP = 82,

                YE_NUMLOCKCLEAR = 83, 
                YE_KP_DIVIDE = 84,
                YE_KP_MULTIPLY = 85,
                YE_KP_MINUS = 86,
                YE_KP_PLUS = 87,
                YE_KP_ENTER = 88,
                YE_KP_1 = 89,
                YE_KP_2 = 90,
                YE_KP_3 = 91,
                YE_KP_4 = 92,
                YE_KP_5 = 93,
                YE_KP_6 = 94,
                YE_KP_7 = 95,
                YE_KP_8 = 96,
                YE_KP_9 = 97,
                YE_KP_0 = 98,
                YE_KP_PERIOD = 99,

                YE_NONUSBACKSLASH = 100, 
                YE_APPLICATION = 101, 
                YE_POWER = 102, 
                YE_KP_EQUALS = 103,
                YE_F13 = 104,
                YE_F14 = 105,
                YE_F15 = 106,
                YE_F16 = 107,
                YE_F17 = 108,
                YE_F18 = 109,
                YE_F19 = 110,
                YE_F20 = 111,
                YE_F21 = 112,
                YE_F22 = 113,
                YE_F23 = 114,
                YE_F24 = 115,
                YE_EXECUTE = 116,
                YE_HELP = 117,
                YE_MENU = 118,
                YE_SELECT = 119,
                YE_STOP = 120,
                YE_AGAIN = 121,   /**< redo */
                YE_UNDO = 122,
                YE_CUT = 123,
                YE_COPY = 124,
                YE_PASTE = 125,
                YE_FIND = 126,
                YE_MUTE = 127,
                YE_VOLUMEUP = 128,
                YE_VOLUMEDOWN = 129,
            /*     YE_LOCKINGCAPSLOCK = 130,  */
            /*     YE_LOCKINGNUMLOCK = 131, */
            /*     YE_LOCKINGSCROLLLOCK = 132, */
                YE_KP_COMMA = 133,
                YE_KP_EQUALSAS400 = 134,

                YE_INTERNATIONAL1 = 135,
                YE_INTERNATIONAL2 = 136,
                YE_INTERNATIONAL3 = 137, 
                YE_INTERNATIONAL4 = 138,
                YE_INTERNATIONAL5 = 139,
                YE_INTERNATIONAL6 = 140,
                YE_INTERNATIONAL7 = 141,
                YE_INTERNATIONAL8 = 142,
                YE_INTERNATIONAL9 = 143,
                YE_LANG1 = 144,  
                YE_LANG2 = 145,  
                YE_LANG3 = 146, 
                YE_LANG4 = 147, 
                YE_LANG5 = 148,  
                YE_LANG6 = 149, 
                YE_LANG7 = 150, 
                YE_LANG8 = 151, 
                YE_LANG9 = 152, 

                YE_ALTERASE = 153,
                YE_SYSREQ = 154,
                YE_CANCEL = 155,
                YE_CLEAR = 156,
                YE_PRIOR = 157,
                YE_RETURN2 = 158,
                YE_SEPARATOR = 159,
                YE_OUT = 160,
                YE_OPER = 161,
                YE_CLEARAGAIN = 162,
                YE_CRSEL = 163,
                YE_EXSEL = 164,

                YE_KP_00 = 176,
                YE_KP_000 = 177,
                YE_THOUSANDSSEPARATOR = 178,
                YE_DECIMALSEPARATOR = 179,
                YE_CURRENCYUNIT = 180,
                YE_CURRENCYSUBUNIT = 181,
                YE_KP_LEFTPAREN = 182,
                YE_KP_RIGHTPAREN = 183,
                YE_KP_LEFTBRACE = 184,
                YE_KP_RIGHTBRACE = 185,
                YE_KP_TAB = 186,
                YE_KP_BACKSPACE = 187,
                YE_KP_A = 188,
                YE_KP_B = 189,
                YE_KP_C = 190,
                YE_KP_D = 191,
                YE_KP_E = 192,
                YE_KP_F = 193,
                YE_KP_XOR = 194,
                YE_KP_POWER = 195,
                YE_KP_PERCENT = 196,
                YE_KP_LESS = 197,
                YE_KP_GREATER = 198,
                YE_KP_AMPERSAND = 199,
                YE_KP_DBLAMPERSAND = 200,
                YE_KP_VERTICALBAR = 201,
                YE_KP_DBLVERTICALBAR = 202,
                YE_KP_COLON = 203,
                YE_KP_HASH = 204,
                YE_KP_SPACE = 205,
                YE_KP_AT = 206,
                YE_KP_EXCLAM = 207,
                YE_KP_MEMSTORE = 208,
                YE_KP_MEMRECALL = 209,
                YE_KP_MEMCLEAR = 210,
                YE_KP_MEMADD = 211,
                YE_KP_MEMSUBTRACT = 212,
                YE_KP_MEMMULTIPLY = 213,
                YE_KP_MEMDIVIDE = 214,
                YE_KP_PLUSMINUS = 215,
                YE_KP_CLEAR = 216,
                YE_KP_CLEARENTRY = 217,
                YE_KP_BINARY = 218,
                YE_KP_OCTAL = 219,
                YE_KP_DECIMAL = 220,
                YE_KP_HEXADECIMAL = 221,

                YE_LCTRL = 224,
                YE_LSHIFT = 225,
                YE_LALT = 226, 
                YE_LGUI = 227, 
                YE_RCTRL = 228,
                YE_RSHIFT = 229,
                YE_RALT = 230,
                YE_RGUI = 231,

                YE_MODE = 257, 

                YE_AUDIONEXT = 258,
                YE_AUDIOPREV = 259,
                YE_AUDIOSTOP = 260,
                YE_AUDIOPLAY = 261,
                YE_AUDIOMUTE = 262,
                YE_MEDIASELECT = 263,
                YE_WWW = 264,
                YE_MAIL = 265,
                YE_CALCULATOR = 266,
                YE_COMPUTER = 267,
                YE_AC_SEARCH = 268,
                YE_AC_HOME = 269,
                YE_AC_BACK = 270,
                YE_AC_FORWARD = 271,
                YE_AC_STOP = 272,
                YE_AC_REFRESH = 273,
                YE_AC_BOOKMARKS = 274,

                YE_BRIGHTNESSDOWN = 275,
                YE_BRIGHTNESSUP = 276,
                YE_DISPLAYSWITCH = 277, 
                YE_KBDILLUMTOGGLE = 278,
                YE_KBDILLUMDOWN = 279,
                YE_KBDILLUMUP = 280,
                YE_EJECT = 281,
                YE_SLEEP = 282,

                YE_APP1 = 283,
                YE_APP2 = 284,

                YE_AUDIOREWIND = 285,
                YE_AUDIOFASTFORWARD = 286,

                YE_SOFTLEFT = 287, 
                YE_SOFTRIGHT = 288, 
                YE_CALL = 289, 
                YE_ENDCALL = 290, 

                YE_NUM_YES = 512 
            };
    };

}

#endif // !YE_YEBOARD_HPP