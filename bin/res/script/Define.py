#-*- coding: utf-8 -*-

#windows消息
WM_KEYFIRST         =  0x0100
WM_KEYDOWN          =  0x0100
WM_KEYUP            =  0x0101
WM_CHAR             =  0x0102
WM_DEADCHAR         =  0x0103
WM_SYSKEYDOWN       =  0x0104
WM_SYSKEYUP         =  0x0105
WM_SYSCHAR          =  0x0106
WM_SYSDEADCHAR      =  0x0107

WM_MOUSEFIRST       = 0x0200
WM_MOUSEMOVE        = 0x0200
WM_LBUTTONDOWN      = 0x0201
WM_LBUTTONUP        = 0x0202
WM_LBUTTONDBLCLK    = 0x0203
WM_RBUTTONDOWN      = 0x0204
WM_RBUTTONUP        = 0x0205
WM_RBUTTONDBLCLK    = 0x0206
WM_MBUTTONDOWN      = 0x0207
WM_MBUTTONUP        = 0x0208
WM_MBUTTONDBLCLK    = 0x0209
WM_MOUSEENTER       = 0x0401
WM_MOUSELEAVE       = 0x0402
WM_MOUSEWHEEL       = 0x020A


#鼠标消息
CM_ENTER   = 1
CM_LEAVE   = 2
CM_LDOWN   = 3
CM_LUP     = 4
CM_RDOWN   = 5
CM_RUP     = 6

#按键值
VK_RETURN        = 0x0D
VK_SHIFT         = 0x10
VK_CONTROL       = 0x11
VK_ESCAPE        = 0x1B
VK_SPACE         = 0x20
VK_LEFT          = 0x25
VK_UP            = 0x26
VK_RIGHT         = 0x27
VK_DOWN          = 0x28
VK_NUMPAD0       = 0x60
VK_NUMPAD1       = 0x61
VK_NUMPAD2       = 0x62
VK_NUMPAD3       = 0x63
VK_NUMPAD4       = 0x64
VK_NUMPAD5       = 0x65
VK_NUMPAD6       = 0x66
VK_NUMPAD7       = 0x67
VK_NUMPAD8       = 0x68
VK_NUMPAD9       = 0x69
VK_MULTIPLY      = 0x6A
VK_ADD           = 0x6B
VK_SEPARATOR     = 0x6C
VK_SUBTRACT      = 0x6D
VK_DECIMAL       = 0x6E
VK_DIVIDE        = 0x6F
VK_F1            = 0x70
VK_F2            = 0x71
VK_F3            = 0x72
VK_F4            = 0x73
VK_F5            = 0x74
VK_F6            = 0x75
VK_F7            = 0x76
VK_F8            = 0x77
VK_F9            = 0x78
VK_F10           = 0x79
VK_F11           = 0x7A
VK_F12           = 0x7B
VK_F13           = 0x7C
VK_F14           = 0x7D
VK_F15           = 0x7E
VK_F16           = 0x7F
VK_F17           = 0x80
VK_F18           = 0x81
VK_F19           = 0x82
VK_F20           = 0x83
VK_F21           = 0x84
VK_F22           = 0x85
VK_F23           = 0x86
VK_F24           = 0x87

VK_A = 65
VK_B = 66
VK_C = 67
VK_D = 68
VK_E = 69
VK_F = 70
VK_G = 71
VK_H = 72
VK_I = 73
VK_J = 74
VK_K = 75
VK_L = 76
VK_M = 77
VK_N = 78
VK_O = 79
VK_P = 80
VK_Q = 81
VK_R = 82
VK_S = 83
VK_T = 84
VK_U = 85
VK_V = 86
VK_W = 87
VK_X = 88
VK_Y = 89
VK_Z = 90

VK_h = 104
VK_m = 109
VK_p = 112

#游戏状态
GS_INIT         = 1
GS_TANSITION    = 4
GS_RUNNING      = 8
GS_EXIT         = 12
GS_GAME_OVER    = 13

#雾化效果
FT_LINEAR   = 1
FT_EXP      = 2
FT_EXP2     = 3
FT_PIXEL    = 16

#按钮状态
BS_NORMAL   = 0
BS_ACTIVE   = 1
BS_DOWN     = 2
BS_DISABLE  = 3
BS_HOT      = 4

CR_SELECT   = 0x7fffffff
CR_TIME     = 0.4
buttonColor = [0xaf808080, 0xafd0d0d0, 0xaf606060, 0xaf404040, 0xafa0a0a0]

#物理状态
PS_FREE     = 0    #休闲
PS_MOVE     = 1    #移动
PS_ROTATE   = 2    #旋转
PS_ATTACK   = 3    #攻击
PS_SKILL    = 4    #释放技能
PS_BE_HIT   = 5    #受击
PS_DEAD     = 6    #死亡
PS_RUN      = 7    #奔跑
PS_RIDE     = 8    #骑马
PS_JUMP     = 9    #跳跃
PS_SWIM     = 10   #游泳
PS_FOLLOW   = 11   #跟随

MENU_WHY_INIT = 0
MENU_WHY_PLAY = 1


MAX_FILL_MODE = 3 #填充模式
