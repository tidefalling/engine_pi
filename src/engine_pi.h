/*****************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Engine Instrumentation Plugin
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register  Modified by Owen Popplestone *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 ***************************************************************************
 */

#ifndef _EnginePI_H_
#define _EnginePI_H_

#include "wx/wxprec.h"

#include <wx/timer.h>
#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#define     PLUGIN_VERSION_MAJOR    0
#define     PLUGIN_VERSION_MINOR    1

#define     MY_API_VERSION_MAJOR    1
#define     MY_API_VERSION_MINOR    8

#include "../../../include/ocpn_plugin.h"

#include "nmea0183/nmea0183.h"




#include "instrument.h"

#include "wind.h"
#include "speedometer.h"


class EngineWindow;

//----------------------------------------------------------------------------------------------------------
//    The PlugIn Class Definition
//----------------------------------------------------------------------------------------------------------


class engine_pi : public wxTimer,opencpn_plugin_18
{
public:
      engine_pi(void *ppimgr);

//    The required PlugIn Methods
      int Init(void);
      bool DeInit(void);

      int GetAPIVersionMajor();
      int GetAPIVersionMinor();
      int GetPlugInVersionMajor();
      int GetPlugInVersionMinor();

      wxString GetCommonName();
      wxString GetShortDescription();
      wxString GetLongDescription();

//    The optional method overrides

      void SetNMEASentence(wxString &sentence);
      void OnContextMenuItemCallback(int id);
      void UpdateAuiStatus(void);

//    The override PlugIn Methods
      bool RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp);
      void SetCursorLatLon(double lat, double lon);
      bool RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp);
      int GetToolbarToolCount(void);
      void ShowPreferencesDialog( wxWindow* parent );
      void OnToolbarToolCallback(int id);
      void SetPluginMessage(wxString &message_id, wxString &message_body);
      void SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix);


private:
      wxWindow         *m_parent_window;

      EngineWindow     *m_pengine_window;
      wxAuiManager     *m_AUImgr;
      int               m_show_id;
      int               m_hide_id;

};



class EngineWindow : public wxWindow
{
public:
      EngineWindow(wxWindow *pparent, wxWindowID id);
      ~EngineWindow();

      void OnPaint(wxPaintEvent& event);
      void SetSentence(wxString &sentence);
      void OnSize(wxSizeEvent& event);
      
private:

      NMEA0183        m_NMEA0183;                 // Used to parse NMEA Sentences

      wxString          m_NMEASentence;
      double            mLat, mLon, mSog, mCog, mVar, mRsa;
    
     wxGridSizer*          itemGridSizer;
     
     DashboardInstrument *wind;
     DashboardInstrument *rpm;
     DashboardInstrument *alttemp;
     DashboardInstrument *altcurrent;
     DashboardInstrument *battemp;
     DashboardInstrument *housecurrentdraw;
     DashboardInstrument *enginetmp;
     DashboardInstrument *oilpress;






DECLARE_EVENT_TABLE()
};


#endif



