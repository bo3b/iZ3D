/////////////////////////////////////////////////////////////////////////////
// Name:        led.h
// Purpose:
// Author:      Joachim Buermann
// Id:          $Id$
// Copyright:   (c) 2001 Joachim Buermann
/////////////////////////////////////////////////////////////////////////////

#ifndef __LED_H
#define __LED_H

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

enum awxLedState {
    awxLED_OFF = 0,
    awxLED_ON,
    awxLED_BLINK
};

enum awxLedColour {
    awxLED_LUCID = 0,
    awxLED_RED,
    awxLED_GREEN,
    awxLED_YELLOW
};

class BlinkTimer;

class awxLed : public wxWindow
{
protected:
    // bitmap for double buffering
    wxBitmap* m_bitmap;
    wxIcon* m_icons[2];
    awxLedState m_state;
    BlinkTimer* m_timer;
    int m_blink;
    int m_x;
    int m_y;
	unsigned int m_timerInterval;
	bool m_on;
	awxLedState m_onState;
	awxLedState m_offState;
	awxLedColour m_onColour;
	awxLedColour m_offColour;
protected:
    // protected member functions
    void DrawOnBitmap();
public:
    awxLed(wxWindow* parent,
		wxWindowID id,
		const wxPoint& pos = wxPoint(0,0),
		const wxSize& size = wxSize(15,15),
		// red LED is default
		awxLedColour color = awxLED_RED,
		long style = 0, int timerInterval = 500 );
    ~awxLed();
    void Blink();
    void OnErase(wxEraseEvent& ) {
	   Redraw();
    };
    void OnPaint(wxPaintEvent& ) {
	   wxPaintDC dc(this);
	   dc.DrawBitmap(*m_bitmap,0,0,false);
    };
    void OnSizeEvent(wxSizeEvent& event) {
	   wxSize size = event.GetSize();
	   m_x = (size.GetX() - m_icons[0]->GetWidth()) >> 1;
	   m_y = (size.GetY() - m_icons[0]->GetHeight()) >> 1;
	   if(m_x < 0) m_x = 0;
	   if(m_y < 0) m_y = 0;
	   DrawOnBitmap();
    };
    void Redraw() {
	   wxClientDC dc(this);
	   DrawOnBitmap();
	   dc.DrawBitmap(*m_bitmap,0,0,false);
    };
	void SetTimerInterval( unsigned int timerInterval );
	unsigned int GetTimerInterval();
    void SetColour(awxLedColour colour);
    void SetState(awxLedState state);
	void SetOn( awxLedColour colour, awxLedState state = awxLED_ON );
	void SetOff( awxLedColour colour, awxLedState state = awxLED_ON );
	void TurnOn( bool on = true );
	void TurnOff();
	void Toggle();
	bool IsOn();
    DECLARE_EVENT_TABLE()
};

class BlinkTimer : public wxTimer
{
protected:
    awxLed* m_led;
public:
    BlinkTimer(awxLed *led) : wxTimer() {
	   m_led = led;
    };
    void Notify() {
	   m_led->Blink();
    };
};

#endif
