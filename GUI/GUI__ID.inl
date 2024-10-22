
/************************************************************
*
*                    GUI_KEY_...
*
*************************************************************
*/
#define GUI_KEY_BACKSPACE         8
#define GUI_KEY_TAB               9
#define GUI_KEY_ENTER             13
#define GUI_KEY_LEFT              16
#define GUI_KEY_UP                17
#define GUI_KEY_RIGHT             18
#define GUI_KEY_DOWN              19
#define GUI_KEY_HOME              23
#define GUI_KEY_END               24
#define GUI_KEY_SHIFT             25
#define GUI_KEY_CONTROL           26
#define GUI_KEY_ESCAPE            27
#define GUI_KEY_INSERT            29
#define GUI_KEY_DELETE            30
#define GUI_KEY_SPACE             32
#define GUI_KEY_F1                40
#define GUI_KEY_F2                41
/*********************************************************************
*
*       Dialog item IDs
*
*  The IDs below are arbitrary values. They just have to be unique
*  within the dialog.
*
*  If you use your own Ids, we recommend to use values above GUI_ID_USER.
*/
#define GUI_ID_OK                1
#define GUI_ID_CANCEL            2
#define GUI_ID_YES               3
#define GUI_ID_NO                4
#define GUI_ID_CLOSE             5
#define GUI_ID_HELP              6
#define GUI_ID_MAXIMIZE          7
#define GUI_ID_MINIMIZE          8
#define GUI_ID_VSCROLL  0xFE
#define GUI_ID_HSCROLL  0xFF
#define GUI_ID_EDIT0    0x100
#define GUI_ID_EDIT1    0x101
#define GUI_ID_EDIT2    0x102
#define GUI_ID_EDIT3    0x103
#define GUI_ID_EDIT4    0x104
#define GUI_ID_EDIT5    0x105
#define GUI_ID_EDIT6    0x106
#define GUI_ID_EDIT7    0x107
#define GUI_ID_EDIT8    0x108
#define GUI_ID_EDIT9    0x109
#define GUI_ID_LISTBOX0 0x110
#define GUI_ID_LISTBOX1 0x111
#define GUI_ID_LISTBOX2 0x112
#define GUI_ID_LISTBOX3 0x113
#define GUI_ID_LISTBOX4 0x114
#define GUI_ID_LISTBOX5 0x115
#define GUI_ID_LISTBOX6 0x116
#define GUI_ID_LISTBOX7 0x117
#define GUI_ID_LISTBOX8 0x118
#define GUI_ID_LISTBOX9 0x119
#define GUI_ID_CHECK0   0x120
#define GUI_ID_CHECK1   0x121
#define GUI_ID_CHECK2   0x122
#define GUI_ID_CHECK3   0x123
#define GUI_ID_CHECK4   0x124
#define GUI_ID_CHECK5   0x125
#define GUI_ID_CHECK6   0x126
#define GUI_ID_CHECK7   0x127
#define GUI_ID_CHECK8   0x128
#define GUI_ID_SLIDER0  0x130
#define GUI_ID_SLIDER1  0x131
#define GUI_ID_SLIDER2  0x132
#define GUI_ID_SLIDER3  0x133
#define GUI_ID_SLIDER4  0x134
#define GUI_ID_SLIDER5  0x135
#define GUI_ID_SLIDER6  0x136
#define GUI_ID_SLIDER7  0x137
#define GUI_ID_SLIDER8  0x138
#define GUI_ID_SLIDER9  0x139
#define GUI_ID_SCROLLBAR0 0x140
#define GUI_ID_SCROLLBAR1 0x141
#define GUI_ID_SCROLLBAR2 0x142
#define GUI_ID_SCROLLBAR3 0x142
#define GUI_ID_RADIO0 0x150
#define GUI_ID_RADIO1 0x151
#define GUI_ID_RADIO2 0x152
#define GUI_ID_RADIO3 0x153
#define GUI_ID_RADIO4 0x154
#define GUI_ID_RADIO5 0x155
#define GUI_ID_RADIO6 0x156
#define GUI_ID_RADIO7 0x157
#define GUI_ID_TEXT0  0x160
#define GUI_ID_TEXT1  0x161
#define GUI_ID_TEXT2  0x162
#define GUI_ID_TEXT3  0x163
#define GUI_ID_TEXT4  0x164
#define GUI_ID_TEXT5  0x165
#define GUI_ID_TEXT6  0x166
#define GUI_ID_TEXT7  0x167
#define GUI_ID_TEXT8  0x168
#define GUI_ID_TEXT9  0x169
#define GUI_ID_BUTTON0 0x170
#define GUI_ID_BUTTON1 0x171
#define GUI_ID_BUTTON2 0x172
#define GUI_ID_BUTTON3 0x173
#define GUI_ID_BUTTON4 0x174
#define GUI_ID_BUTTON5 0x175
#define GUI_ID_BUTTON6 0x176
#define GUI_ID_BUTTON7 0x177
#define GUI_ID_BUTTON8 0x178
#define GUI_ID_BUTTON9 0x179
#define GUI_ID_DROPDOWN0  0x180
#define GUI_ID_DROPDOWN1  0x181
#define GUI_ID_DROPDOWN2  0x182
#define GUI_ID_DROPDOWN3  0x183
#define GUI_ID_MULTIEDIT0 0x190
#define GUI_ID_MULTIEDIT1 0x191
#define GUI_ID_MULTIEDIT2 0x192
#define GUI_ID_MULTIEDIT3 0x193
#define GUI_ID_LISTVIEW0  0x200
#define GUI_ID_LISTVIEW1  0x201
#define GUI_ID_LISTVIEW2  0x202
#define GUI_ID_LISTVIEW3  0x203
#define GUI_ID_PROGBAR0   0x210
#define GUI_ID_PROGBAR1   0x211
#define GUI_ID_PROGBAR2   0x212
#define GUI_ID_PROGBAR3   0x213
#define GUI_ID_USER     0x800
/**********************************
*
*      Text styles
*
***********************************
*/
#define GUI_TS_NORMAL           (0)
#define GUI_TS_UNDERLINE        (1 << 0)
#define GUI_TS_STRIKETHRU       (1 << 1)
#define GUI_TS_OVERLINE         (1 << 2)
