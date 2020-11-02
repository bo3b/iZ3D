//#pragma once
//#include "c:\sourcecode\libraries\wxwidgets2.7\include\wx\msw\control.h"
#include <wx/wx.h>
#include <wx/dcbuffer.h>

enum ledBGOrient{
    ledBG_ORIENT_VERTICAL = 0,
    ledBG_ORIENT_HORIZONTAL
};

enum ledBGDrawMode{
    ledBG_DOUBLE_SIDED = 0,
    ledBG_SINGLE_SIDED_TOP_LEFT,
    ledBG_SINGLE_SIDED_BOTTOM_RIGHT
};

enum ledBGSizeMode{
    ledBG_FIXED_N_BARS = 0,
    ledBG_FIXED_BAR_SIZE
};
/*
	class ledBarGraph - simple wxControl that simulates an LED Bar Graph with multiple colors
*/
class wxLedBarGraph :	public wxControl
{
	DECLARE_DYNAMIC_CLASS( wxLedBarGraph );
	DECLARE_EVENT_TABLE();
public:
	//default constructor
	wxLedBarGraph() {
		Init();
	}

	//the useful constructor
	wxLedBarGraph(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		long style = wxSUNKEN_BORDER, const wxValidator& validator = wxDefaultValidator)
	{
		Init();
		Create(parent, id, pos, size, style, validator);
	}

    ~wxLedBarGraph(void);

public:
    //functions used to configure the control
    void SetSizingMode( ledBGSizeMode mode );
    void SetDrawingMode( ledBGDrawMode mode );
    void SetOrientation( ledBGOrient orient );

	void SetMaxValue( double val );
	void SetMinValue( double val );
	void SetValue( double val );

	void SetNBars( int nBars );
	void SetBarWidths( int width );


private:
	// Creation
	bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
		long style = wxSUNKEN_BORDER, const wxValidator& validator = wxDefaultValidator);

	// Common initialization
	void Init() {
		m_nBars = 25;
        m_barWidths = 6;

		m_value = 0;
		m_maxVal = 1;
		m_minVal = -1;

		m_orientation = ledBG_ORIENT_HORIZONTAL;
		m_sizingMode = ledBG_FIXED_BAR_SIZE;
        m_mode = ledBG_SINGLE_SIDED_TOP_LEFT;
	}

	// Overrides
	wxSize DoGetBestSize() const {
		if( m_orientation == ledBG_ORIENT_HORIZONTAL ){
            return wxSize(155, 25);
		}else{
		    return wxSize(25, 155);
		}
	}

	//force redraw when I get resized
	void OnResize( wxSizeEvent& evt );

	//emtpy - prevent the drawing of a background.
	//this is combined with a double-buffered DC to eliminate the annoying flashing on control update
	void OnEraseBackGround(wxEraseEvent& myEvent);

	//draw the led bar graph
	void OnPaint(wxPaintEvent& evt);

	//scale the internal value to (-1) -> (1)
	void RescaleValues( ){
	 	double val = m_realValue - m_minVal;
		double span = (m_maxVal - m_minVal)/2;

		m_value = (val - span)/span;
		this->Refresh();
	}

private:
    int m_orientation;
    int m_mode;
    int m_sizingMode;

    //Only one of these works at a time, if the wrong sizingMode is selected,
    //the set methods will ignore your input.
	int m_nBars;		//around 25 looks good most places
    int m_barWidths;      //6 looks good

	int m_startX;



	double m_maxVal;
	double m_minVal;
	double m_realValue;
	double m_value;		//INTERNAL:  always scaled to the range of -1 to 1.
						//0 is centered, -1 is the far left bar, 1 is the far right bar
						//values > 1 or < -1 will trigger the "max-out" state, coloring the
						//appropriate bar brighter

};



