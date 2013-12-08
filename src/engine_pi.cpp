
#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
#include "wx/wx.h"
#endif //precompiled headers

#include <wx/aui/aui.h>

#include "engine_pi.h"

wxFont *g_pFontData;
wxFont *g_pFontLabel;
wxFont *g_pFontSmall;
wxFont *g_pFontTitle;


// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return (opencpn_plugin *) new engine_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}


//---------------------------------------------------------------------------------------------------------
//
//    Engine PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------





engine_pi::engine_pi( void *ppimgr ) :
        wxTimer( this ), opencpn_plugin_18( ppimgr )

{};


int engine_pi::Init(void)
{
//      printf("engine_pi Init()\n");

      m_pengine_window = NULL;

      // Get a pointer to the opencpn display canvas, to use as a parent for windows created
      m_parent_window = GetOCPNCanvasWindow();

      g_pFontTitle = new wxFont( 10, wxFONTFAMILY_SWISS, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL );
      g_pFontData = new wxFont( 14, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
      g_pFontLabel = new wxFont( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );
      g_pFontSmall = new wxFont( 8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL );

     
     
     
      // Create the Context Menu Items

      //    In order to avoid an ASSERT on msw debug builds,
      //    we need to create a dummy menu to act as a surrogate parent of the created MenuItems
      //    The Items will be re-parented when added to the real context meenu
      wxMenu dummy_menu;

      wxMenuItem *pmi = new wxMenuItem(&dummy_menu, -1, _("Show Engine Instrumentation"));
      m_show_id = AddCanvasContextMenuItem(pmi, this );
      SetCanvasContextMenuItemViz(m_show_id, true);

      wxMenuItem *pmih = new wxMenuItem(&dummy_menu, -1, _("Hide Engine Instrumentation"));
      m_hide_id = AddCanvasContextMenuItem(pmih, this );
      SetCanvasContextMenuItemViz(m_hide_id, false);

        m_pengine_window = new EngineWindow(m_parent_window, wxID_ANY);

        m_AUImgr = GetFrameAuiManager();
        m_AUImgr->AddPane(m_pengine_window);
        m_AUImgr->GetPane(m_pengine_window).Name(_T("Engine Management"));

        m_AUImgr->GetPane(m_pengine_window).Float();
        m_AUImgr->GetPane(m_pengine_window).FloatingPosition(300,30);

        m_AUImgr->GetPane(m_pengine_window).Caption(_T("Engine Management"));
        m_AUImgr->GetPane(m_pengine_window).CaptionVisible(true);
        m_AUImgr->GetPane(m_pengine_window).GripperTop(true);
        m_AUImgr->GetPane(m_pengine_window).CloseButton(true);
        m_AUImgr->GetPane(m_pengine_window).Show(false);
       m_AUImgr->Update();
//Start( 5000, wxTIMER_CONTINUOUS );
      return (
           INSTALLS_CONTEXTMENU_ITEMS     |
           WANTS_NMEA_SENTENCES           |
           USES_AUI_MANAGER
            );
}

bool engine_pi::DeInit(void)
{
      m_AUImgr->DetachPane(m_pengine_window);
      if(m_pengine_window)
      {
          m_pengine_window->Close();
//          m_pdemo_window->Destroy(); //Gives a Segmentation fault
          delete g_pFontTitle;
          delete g_pFontData;
          delete g_pFontLabel;
          delete g_pFontSmall;

      }
      
      
      return true;
}

int engine_pi::GetAPIVersionMajor()
{
      return MY_API_VERSION_MAJOR;
}

int engine_pi::GetAPIVersionMinor()
{
      return MY_API_VERSION_MINOR;
}

int engine_pi::GetPlugInVersionMajor()
{
      return PLUGIN_VERSION_MAJOR;
}

int engine_pi::GetPlugInVersionMinor()
{
      return PLUGIN_VERSION_MINOR;
}

wxString engine_pi::GetCommonName()
{
      return _("Engine");
}

wxString engine_pi::GetShortDescription()
{
      return _("Engine PlugIn for OpenCPN");
}

wxString engine_pi::GetLongDescription()
{
      return _("Engine PlugIn for OpenCPN\n\
Engine instrumentation.");

}

void engine_pi::SetNMEASentence(wxString &sentence)
{
//      printf("engine_pi::SetNMEASentence\n");
      if(m_pengine_window)
      {
            m_pengine_window->SetSentence(sentence);
      }
}


void engine_pi::OnContextMenuItemCallback(int id)
{
      wxLogMessage(_T("engine_pi OnContextMenuCallBack()"));
     ::wxBell();

      //  Note carefully that this is a "reference to a wxAuiPaneInfo classs instance"
      //  Copy constructor (i.e. wxAuiPaneInfo pane = m_AUImgr->GetPane(m_pdemo_window);) will not work

      wxAuiPaneInfo &pane = m_AUImgr->GetPane(m_pengine_window);
      if(!pane.IsOk())
            return;

      if(!pane.IsShown())
      {
//            printf("show\n");
            SetCanvasContextMenuItemViz(m_hide_id, true);
            SetCanvasContextMenuItemViz(m_show_id, false);

            pane.Show(true);
            m_AUImgr->Update();

      }
      else
      {
//            printf("hide\n");
            SetCanvasContextMenuItemViz(m_hide_id, false);
            SetCanvasContextMenuItemViz(m_show_id, true);

            pane.Show(false);
         m_AUImgr->Update();
     }

/*
      if(NULL == m_pdemo_window)
      {
            m_pdemo_window = new demoWindow(m_parent_window, wxID_ANY);

            SetCanvasContextMenuItemViz(m_hide_id, true);
            SetCanvasContextMenuItemViz(m_show_id, false);
      }
      else
      {
            m_pdemo_window->Close();
            m_pdemo_window->Destroy();
            m_pdemo_window = NULL;

            SetCanvasContextMenuItemViz(m_hide_id, false);
            SetCanvasContextMenuItemViz(m_show_id, true);
      }
*/
}

void engine_pi::UpdateAuiStatus(void)
{
      //    This method is called after the PlugIn is initialized
      //    and the frame has done its initial layout, possibly from a saved wxAuiManager "Perspective"
      //    It is a chance for the PlugIn to syncronize itself internally with the state of any Panes that
      //    were added to the frame in the PlugIn ctor.

      //    We use this callback here to keep the context menu selection in sync with the window state


      wxAuiPaneInfo &pane = m_AUImgr->GetPane(m_pengine_window);
      if(!pane.IsOk())
            return;

      printf("update %d\n",pane.IsShown());

      SetCanvasContextMenuItemViz(m_hide_id, pane.IsShown());
      SetCanvasContextMenuItemViz(m_show_id, !pane.IsShown());

}

bool engine_pi::RenderOverlay(wxDC &dc, PlugIn_ViewPort *vp)
{
 
            return false;
}
void engine_pi::SetCursorLatLon(double lat, double lon)
{

}
bool engine_pi::RenderGLOverlay(wxGLContext *pcontext, PlugIn_ViewPort *vp)
{
  
            return false;

}
int engine_pi::GetToolbarToolCount(void)
{
      return 1;
}
void engine_pi::ShowPreferencesDialog( wxWindow* parent )
{

}
void engine_pi::OnToolbarToolCallback(int id)
{

}
void engine_pi::SetPluginMessage(wxString &message_id, wxString &message_body)
{

}
void engine_pi::SetPositionFixEx(PlugIn_Position_Fix_Ex &pfix)
{

}

//----------------------------------------------------------------
//
//    Engine Window Implementation
//
//----------------------------------------------------------------

BEGIN_EVENT_TABLE(EngineWindow, wxWindow)
  EVT_PAINT ( EngineWindow::OnPaint )
  EVT_SIZE(EngineWindow::OnSize)


END_EVENT_TABLE()

EngineWindow::EngineWindow(wxWindow *pparent, wxWindowID id)
      :wxWindow(pparent, id, wxDefaultPosition, wxSize(500,200),
             wxSIMPLE_BORDER, _T("OpenCPN PlugIn"))
 // exitwxDEFAULT_FRAME_STYLE, _T("OpenCPN PlugIn"))
{
      mLat = 0.0;
      mLon = 1.0;
      mSog = 2.0;
      mCog = 3.0;
      mVar = 4.0;
      mRsa = 5.0;
     
	itemGridSizer = new wxGridSizer( 2, 4, 0, 0 );
	itemGridSizer->SetMinSize(270,150);


 wind = new DashboardInstrument_Wind( this, wxID_ANY,
                        _("     Wind"), OCPN_DBP_STC_AWA );
                ( (DashboardInstrument_Dial *) wind )->SetOptionMainValue( _T("%.0f"),
                        DIAL_POSITION_BOTTOMLEFT );
                ( (DashboardInstrument_Dial *) wind )->SetOptionExtraValue(
                        OCPN_DBP_STC_AWS, _T("%.1f"), DIAL_POSITION_INSIDE );
                        
 wind->SetData( OCPN_DBP_STC_AWA, 10,
                            _T("\u00B0") );
                            
                            
                            
 rpm = new DashboardInstrument_Speedometer( this, wxID_ANY,
                        _("     RPM"), OCPN_DBP_STC_SOG, 0, 4000 );
                ( (DashboardInstrument_Dial *) rpm )->SetOptionLabel( 250,
                        DIAL_LABEL_HORIZONTAL );
                //(DashboardInstrument_Dial *)instrument->SetOptionMarker(0.1, DIAL_MARKER_SIMPLE, 5);
                ( (DashboardInstrument_Dial *) rpm )->SetOptionMarker( 250,
                        DIAL_MARKER_SIMPLE, 2 );
                ( (DashboardInstrument_Dial *) rpm )->SetOptionExtraValue(
                        OCPN_DBP_STC_STW, _T("STW\n%.2f"), DIAL_POSITION_BOTTOMLEFT );                        
 
 rpm->SetData( OCPN_DBP_STC_SOG, 2000,
                            _T("\u00B0") );
                                                       
 alttemp = new DashboardInstrument_Speedometer( this, wxID_ANY,
                        _("     Alternator Temp"), OCPN_DBP_STC_SOG, 0, 150 );
                ( (DashboardInstrument_Dial *) alttemp )->SetOptionLabel( 10,
                        DIAL_LABEL_HORIZONTAL );
                //(DashboardInstrument_Dial *)instrument->SetOptionMarker(0.1, DIAL_MARKER_SIMPLE, 5);
                ( (DashboardInstrument_Dial *) alttemp )->SetOptionMarker( 10,
                        DIAL_MARKER_SIMPLE, 2 );
                ( (DashboardInstrument_Dial *) alttemp )->SetOptionExtraValue(
                        OCPN_DBP_STC_STW, _T("STW\n%.2f"), DIAL_POSITION_BOTTOMLEFT );                                 
 
 alttemp->SetData( OCPN_DBP_STC_SOG, 75,
                            _T("\u00B0") );                           
   
 altcurrent = new DashboardInstrument_Speedometer( this, wxID_ANY,
                        _("  Alternator Current"), OCPN_DBP_STC_SOG, 0, 150 );
                ( (DashboardInstrument_Dial *) altcurrent )->SetOptionLabel( 10,
                        DIAL_LABEL_HORIZONTAL );
                //(DashboardInstrument_Dial *)instrument->SetOptionMarker(0.1, DIAL_MARKER_SIMPLE, 5);
                ( (DashboardInstrument_Dial *) altcurrent )->SetOptionMarker( 10,
                        DIAL_MARKER_SIMPLE, 2 );
                ( (DashboardInstrument_Dial *) altcurrent)->SetOptionExtraValue(
                        OCPN_DBP_STC_STW, _T("STW\n%.2f"), DIAL_POSITION_BOTTOMLEFT );                                 
 
 altcurrent->SetData( OCPN_DBP_STC_SOG, 12,
                            _T("\u00B0") );   
                            
 battemp = new DashboardInstrument_Speedometer( this, wxID_ANY,
                        _("  Battery Temp"), OCPN_DBP_STC_SOG, 0, 100 );
                ( (DashboardInstrument_Dial *) battemp )->SetOptionLabel( 10,
                        DIAL_LABEL_HORIZONTAL );
                //(DashboardInstrument_Dial *)instrument->SetOptionMarker(0.1, DIAL_MARKER_SIMPLE, 5);
                ( (DashboardInstrument_Dial *) battemp )->SetOptionMarker( 10,
                        DIAL_MARKER_SIMPLE, 2 );
                ( (DashboardInstrument_Dial *) battemp )->SetOptionExtraValue(
                        OCPN_DBP_STC_STW, _T("STW\n%.2f"), DIAL_POSITION_BOTTOMLEFT );                                 
 
 battemp->SetData( OCPN_DBP_STC_SOG, 35,
                            _T("\u00B0") );   
                            
 housecurrentdraw = new DashboardInstrument_Speedometer( this, wxID_ANY,
                        _("  House Current"), OCPN_DBP_STC_SOG, 0, 100 );
                ( (DashboardInstrument_Dial *) housecurrentdraw )->SetOptionLabel( 10,
                        DIAL_LABEL_HORIZONTAL );
                //(DashboardInstrument_Dial *)instrument->SetOptionMarker(0.1, DIAL_MARKER_SIMPLE, 5);
                ( (DashboardInstrument_Dial *) housecurrentdraw )->SetOptionMarker( 10,
                        DIAL_MARKER_SIMPLE, 2 );
                ( (DashboardInstrument_Dial *) housecurrentdraw )->SetOptionExtraValue(
                        OCPN_DBP_STC_STW, _T("STW\n%.2f"), DIAL_POSITION_BOTTOMLEFT );                                 
 
 housecurrentdraw->SetData( OCPN_DBP_STC_SOG, 5,
                            _T("\u00B0") );                             
                            
 enginetmp = new DashboardInstrument_Speedometer( this, wxID_ANY,
                        _("  Engine Temp"), OCPN_DBP_STC_SOG, 0, 300 );
                ( (DashboardInstrument_Dial *) enginetmp )->SetOptionLabel( 25,
                        DIAL_LABEL_HORIZONTAL );
                //(DashboardInstrument_Dial *)instrument->SetOptionMarker(0.1, DIAL_MARKER_SIMPLE, 5);
                ( (DashboardInstrument_Dial *) enginetmp )->SetOptionMarker( 25,
                        DIAL_MARKER_SIMPLE, 2 );
                ( (DashboardInstrument_Dial *) enginetmp )->SetOptionExtraValue(
                        OCPN_DBP_STC_STW, _T("STW\n%.2f"), DIAL_POSITION_BOTTOMLEFT );                                 
 
 enginetmp->SetData( OCPN_DBP_STC_SOG, 190,
                            _T("\u00B0") );                          
                            
                            
 oilpress = new DashboardInstrument_Speedometer( this, wxID_ANY,
                        _("  Oil Pressure"), OCPN_DBP_STC_SOG, 0, 100 );
                ( (DashboardInstrument_Dial *) oilpress )->SetOptionLabel( 10,
                        DIAL_LABEL_HORIZONTAL );
                //(DashboardInstrument_Dial *)instrument->SetOptionMarker(0.1, DIAL_MARKER_SIMPLE, 5);
                ( (DashboardInstrument_Dial *) oilpress )->SetOptionMarker( 10,
                        DIAL_MARKER_SIMPLE, 2 );
                ( (DashboardInstrument_Dial *) oilpress )->SetOptionExtraValue(
                        OCPN_DBP_STC_STW, _T("STW\n%.2f"), DIAL_POSITION_BOTTOMLEFT );                                 
 
 oilpress->SetData( OCPN_DBP_STC_SOG, 82,
                            _T("\u00B0") );                          
                                                       
                            
     
   
 itemGridSizer->Add( wind, 0, wxEXPAND , 0 );
 itemGridSizer->Add( rpm, 0, wxEXPAND, 0 );
 itemGridSizer->Add( alttemp, 0, wxEXPAND , 0 );
 itemGridSizer->Add( altcurrent, 0, wxEXPAND , 0 );
 itemGridSizer->Add( battemp, 0, wxEXPAND , 0 );
 itemGridSizer->Add( housecurrentdraw, 0, wxEXPAND , 0 );
 itemGridSizer->Add( enginetmp, 0, wxEXPAND , 0 );
 itemGridSizer->Add( oilpress, 0, wxEXPAND, 0 );



  this->SetSizer(itemGridSizer);
 this->SetSize(wxSize(270,150));
 

  Fit();
  Layout();
  //Refresh();
  SetMinSize( itemGridSizer->GetMinSize() );
}
  
     


EngineWindow::~EngineWindow()
{
	//delete all news
	delete wind;
	delete rpm;
	delete alttemp;
	delete altcurrent;
	delete battemp;
	delete housecurrentdraw;
	delete enginetmp;
	delete oilpress;
	delete itemGridSizer;
}

void EngineWindow::OnSize(wxSizeEvent& event)
{
     event.Skip();
    //  printf("engineWindow OnSize()\n");
    //  itemGridSizer->SetMinSize(GetParent()->GetClientSize());
   //   mPanel->SetSize( mPanel->GetParent( )->GetCientSize( ) ); 
   //Fit();
   //wind->SetMinSize( wind->GetSize(wxVERTICAL,GetClientSize() )/4);
  // wind->SetMinSize( itemGridSizer->GetSize()/8 );
 // wind->SetSize(wxSize(150,150));
      Layout();
    Refresh();
}


void EngineWindow::SetSentence(wxString &sentence)
{
      m_NMEA0183 << sentence;

      bool bGoodData = false;

      if(m_NMEA0183.PreParse())
      {
            if(m_NMEA0183.LastSentenceIDReceived == _T("RMC"))
            {
                  if(m_NMEA0183.Parse())
                  {
                              if(m_NMEA0183.Rmc.IsDataValid == NTrue)
                              {
                                    float llt = m_NMEA0183.Rmc.Position.Latitude.Latitude;
                                    int lat_deg_int = (int)(llt / 100);
                                    float lat_deg = lat_deg_int;
                                    float lat_min = llt - (lat_deg * 100);
                                    mLat = lat_deg + (lat_min/60.);
                                    if(m_NMEA0183.Rmc.Position.Latitude.Northing == South)
                                          mLat = -mLat;

                                    float lln = m_NMEA0183.Rmc.Position.Longitude.Longitude;
                                    int lon_deg_int = (int)(lln / 100);
                                    float lon_deg = lon_deg_int;
                                    float lon_min = lln - (lon_deg * 100);
                                    mLon = lon_deg + (lon_min/60.);
                                    if(m_NMEA0183.Rmc.Position.Longitude.Easting == West)
                                          mLon = -mLon;

                                    mSog = m_NMEA0183.Rmc.SpeedOverGroundKnots;
                                    mCog = m_NMEA0183.Rmc.TrackMadeGoodDegreesTrue;

                                    if(m_NMEA0183.Rmc.MagneticVariationDirection == East)
                                          mVar =  m_NMEA0183.Rmc.MagneticVariation;
                                    else if(m_NMEA0183.Rmc.MagneticVariationDirection == West)
                                          mVar = -m_NMEA0183.Rmc.MagneticVariation;

                                    bGoodData = true;

                              }
                        }
                  }
                  if( m_NMEA0183.LastSentenceIDReceived == _T("RSA") ) {
					 
                              if( m_NMEA0183.Parse() ) 
                              {
                                     if( m_NMEA0183.Rsa.IsStarboardDataValid == NTrue ) 
                                     {
                                              mRsa =  m_NMEA0183.Rsa.Starboard;
                                     } 
                                     else if( m_NMEA0183.Rsa.IsPortDataValid == NTrue ) 
                                     {
                                              mRsa = -m_NMEA0183.Rsa.Port;
                                     }
                                     bGoodData = true;
                              }
                  }

        }

      //    Got the data, now do something with it

      if(bGoodData)
      {
            Refresh(false);
      }
}

void EngineWindow::OnPaint(wxPaintEvent& event)
{
      //wxLogMessage(_T("engine_pi onpaint"));

      wxPaintDC dc ( this );

      //printf("onpaint\n");
      // access voltagepanel->textpane.setvalue(mRsa);
      //    m_text->SetLabel(wxString::Format(wxT("%d"), count));
            dc.Clear();

            wxString data;
        //    m_Latitude->SetValue(mLat);
            //data.Printf(_T("Lat: %g "), mLat);
            //dc.DrawText(data, 10, 10);
		//	m_Longitude->SetValue(mLon);  
            //data.Printf(_T("Lon: %g"), mLon);
            //dc.DrawText(data, 10, 40);
		//	m_SpeedOverGround->SetValue(mSog);

           //data.Printf(_T("Sog: %g"), mSog);
           //dc.DrawText(data, 10, 70);
		//	m_Rpm->SetLabel(wxString::Format(wxT("%f"), mCog));
			
        //    m_CourseOverGround->SetValue(mCog);
            //data.Printf(_T("Cog: %g"), mCog);
            //dc.DrawText(data, 10, 100);
            
        //    m_Variation->SetValue(mVar);
            
        //    m_RudderAngle->SetValue(mRsa);
            
            oilpress->SetData( OCPN_DBP_STC_SOG, mSog * 10,
                            _T("\u00B0") ); 
                            
            rpm->SetData( OCPN_DBP_STC_SOG, mSog * 200,
                            _T("\u00B0") );   
            
    

     
}
