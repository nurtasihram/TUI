#include "ext_monocolor.h"

void MonoColorStyle() {
	Widget::DefaultEffect(Widget::EffectItf::Simple);
	WObj::DesktopColor(RGB_WHITE);
	/* Button */
	Button::DefaultProps.aBkColor[BUTTON_CI_UNPRESSED] = RGB_WHITE;
	Button::DefaultProps.aBkColor[BUTTON_CI_PRESSED] = RGB_BLACK;
	Button::DefaultProps.aBkColor[BUTTON_CI_DISABLED] = RGB_BLACK; // DISC
	Button::DefaultProps.aTextColor[BUTTON_CI_UNPRESSED] = RGB_BLACK;
	Button::DefaultProps.aTextColor[BUTTON_CI_PRESSED] = RGB_WHITE;
	Button::DefaultProps.aTextColor[BUTTON_CI_DISABLED] = RGB_WHITE; // DISC
	/* CheckBox */
	CheckBox::DefaultProps.BkColor = RGB_WHITE;
	CheckBox::DefaultProps.TextColor = RGB_BLACK;
	CheckBox::DefaultProps.aBkColorBox[CHECKBOX_CI_INACTIV] = RGB_WHITE;
	CheckBox::DefaultProps.aBkColorBox[CHECKBOX_CI_ACTIV] = RGB_WHITE;
	/* Frame */
	Frame::DefaultProps.Border = 1;
	Frame::DefaultProps.ClientColor = RGB_WHITE;
	Frame::DefaultProps.FrameColor = RGB_WHITE;
	Frame::DefaultProps.aBarColor[FRAME_CI_FOCUSED] = RGB_BLACK;
	Frame::DefaultProps.aBarColor[FRAME_CI_LOSEFOCUS] = RGB_WHITE;
	Frame::DefaultProps.aTextColor[FRAME_CI_FOCUSED] = RGB_WHITE;
	Frame::DefaultProps.aTextColor[FRAME_CI_LOSEFOCUS] = RGB_BLACK;
	/* Header */
	Header::DefaultProps.BkColor = RGB_WHITE;
	Header::DefaultProps.TextColor = RGB_BLACK;
	/* ListBox */
	ListBox::DefaultProps.aBkColor[LISTBOX_CI_UNSEL] = RGB_WHITE;
	ListBox::DefaultProps.aBkColor[LISTBOX_CI_SEL] = RGB_BLACK;
	ListBox::DefaultProps.aBkColor[LISTBOX_CI_SELFOCUS] = RGB_BLACK;
	ListBox::DefaultProps.aBkColor[LISTBOX_CI_DISABLED] = RGB_WHITE; // DISC
	ListBox::DefaultProps.aTextColor[LISTBOX_CI_UNSEL] = RGB_BLACK;
	ListBox::DefaultProps.aTextColor[LISTBOX_CI_SEL] = RGB_WHITE;
	ListBox::DefaultProps.aTextColor[LISTBOX_CI_SELFOCUS] = RGB_WHITE;
	ListBox::DefaultProps.aTextColor[LISTBOX_CI_DISABLED] = RGB_BLACK; // DISC
	/* ListView */
	ListView::DefaultProps.aBkColor[LISTVIEW_CI_UNSEL] = RGB_WHITE;
	ListView::DefaultProps.aBkColor[LISTVIEW_CI_SEL] = RGB_BLACK;
	ListView::DefaultProps.aBkColor[LISTVIEW_CI_SELFOCUS] = RGB_BLACK;
	ListView::DefaultProps.aTextColor[LISTVIEW_CI_UNSEL] = RGB_BLACK;
	ListView::DefaultProps.aTextColor[LISTVIEW_CI_SEL] = RGB_WHITE;
	ListView::DefaultProps.aTextColor[LISTVIEW_CI_SELFOCUS] = RGB_WHITE;
	ListView::DefaultProps.GridColor = RGB_BLACK;
	/* MultiPage */
	MultiPage::DefaultProps.aBkColor[MULTIPAGE_CI_DISABLED] = RGB_BLACK;
	MultiPage::DefaultProps.aBkColor[MULTIPAGE_CI_ENABLED] = RGB_WHITE;
	MultiPage::DefaultProps.aTextColor[0] = RGB_BLACK;
	MultiPage::DefaultProps.aTextColor[1] = RGB_BLACK;
	/* ProgBar */
	ProgBar::DefaultProps.aBkColor[PROGBAR_CI_INACTIV] = RGB_WHITE;
	ProgBar::DefaultProps.aBkColor[PROGBAR_CI_ACTIV] = RGB_BLACK;
	ProgBar::DefaultProps.aTextColor[PROGBAR_CI_INACTIV] = RGB_BLACK;
	ProgBar::DefaultProps.aTextColor[PROGBAR_CI_ACTIV] = RGB_WHITE;
	/* Radio */
	Radio::DefaultProps.BkColor = RGB_WHITE;
	Radio::DefaultProps.TextColor = RGB_BLACK;
	/* ScrollBar */
	ScrollBar::DefaultProps.aBkColor[SCROLLBAR_CI_RAIL] = RGB_WHITE;
	ScrollBar::DefaultProps.aBkColor[SCROLLBAR_CI_BUTTON] = RGB_BLACK;
	ScrollBar::DefaultProps.aColor = RGB_WHITE;
	/* Slider */
	Slider::DefaultProps.BkColor = RGB_WHITE;
	Slider::DefaultProps.Color = RGB_BLACK;
	/* Static */
	Static::DefaultProps.BkColor = RGB_WHITE;
	Static::DefaultProps.Color = RGB_BLACK;
}
