/////////////////////////////////////////////////////////////////////////////
// Name:        xti.cpp
// Purpose:     eXtended RTTI support sample
// Author:      Stefan Csomor, Francesco Montorsi
// Modified by:
// Created:     13/5/2007
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor, Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wx.h"
#include "wx/variant.h"
#include "wx/xml/xml.h"
#include "wx/frame.h"
#include "wx/notebook.h"
#include "wx/event.h"
#include "wx/spinbutt.h"
#include "wx/spinctrl.h"

#include "wx/xtistrm.h"
#include "wx/xtixml.h"
#include "wx/txtstrm.h"
#include "wx/wfstream.h"
#include "wx/spinctrl.h"

#include "classlist.h"

#if !wxUSE_EXTENDED_RTTI
    #error This sample requires XTI (eXtended RTTI) enabled
#endif


// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// Define a new application type, each program should derive a class from wxApp
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{
public:
    // ctor(s)
    MyFrame(const wxString& title);

    void OnPersist(wxCommandEvent& event);
    void OnDepersist(wxCommandEvent& event);
    void OnGenerateCode(wxCommandEvent& event);
    void OnDumpClasses(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// IDs for the controls and the menu commands
enum
{
    // menu items
    Minimal_Persist = wxID_HIGHEST,
    Minimal_Depersist,
    Minimal_GenerateCode,
    Minimal_DumpClasses,
    Minimal_Quit = wxID_EXIT,
    Minimal_About = wxID_ABOUT
};

// ----------------------------------------------------------------------------
// event tables and other macros for wxWidgets
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Persist, MyFrame::OnPersist)
    EVT_MENU(Minimal_Depersist, MyFrame::OnDepersist)
    EVT_MENU(Minimal_GenerateCode, MyFrame::OnGenerateCode)
    EVT_MENU(Minimal_DumpClasses, MyFrame::OnDumpClasses)
    EVT_MENU(Minimal_Quit,  MyFrame::OnQuit)
    EVT_MENU(Minimal_About, MyFrame::OnAbout)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

// ============================================================================
// implementation
// ============================================================================

void RegisterFrameRTTI();

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    RegisterFrameRTTI();

    // create the main application window
    MyFrame *frame = new MyFrame(_T("Extended RTTI sample"));

    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(true);

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned false here, the
    // application would exit immediately.
    return true;
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(300, 200))
{
    // set the frame icon
    SetIcon(wxICON(sample));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *fileMenu = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, _T("&About...\tF1"), _T("Show about dialog"));

    fileMenu->Append(Minimal_Persist, _T("Persist a wxFrame to XML..."), 
                    _T("Creates a wxFrame using wxXTI and saves its description as XML"));
    fileMenu->Append(Minimal_Depersist, _T("Depersist XML file..."), 
                    _T("Loads the description of wxFrame from XML"));
    fileMenu->Append(Minimal_GenerateCode, _T("Generate code for a wxFrame saved to XML..."), 
                    _T("Generates the C++ code which belong to a persisted wxFrame"));
    fileMenu->AppendSeparator();
    fileMenu->Append(Minimal_DumpClasses, _T("Dump registered classes..."), 
                    _T("Dumps the description of all wxWidgets classes registered in XTI"));
    fileMenu->AppendSeparator();
    fileMenu->Append(Minimal_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWidgets!"));
#endif // wxUSE_STATUSBAR
}



// ----------------------------------------------------------------------------
// XTI sample code
// ----------------------------------------------------------------------------

// this is the kind of source code that would end up being generated by a 
// designer corresponding to the information we are setting up via RTTI 
// in the CreateFrameRTTI function:
//
//  class MyXTIFrame : public wxFrame
//  {
//  public:
//       //   construction
//      MyXTIFrame()
//      {
//          Init();
//          m_button = NULL;
//      }
//
//      bool Create(wxWindow *parent,
//                  wxWindowID id,
//                  const wxString& title,
//                  const wxPoint& pos = wxDefaultPosition,
//                  const wxSize& size = wxDefaultSize,
//                  long style = wxDEFAULT_FRAME_STYLE,
//                  const wxString& name = wxFrameNameStr)
//      {
//          return wxFrame::Create( parent, id, title, pos, size, style, name );
//      }
//
//      void SetButton( wxButton * button ) { m_button = button; }
//      wxButton* GetButton() const { return m_button; }
//
//      void ButtonClickHandler( wxEvent & WXUNUSED(event) )
//      {
//          wxMessageBox( "Button Clicked ", "Hi!", wxOK );
//      }
//
//  protected:
//      wxButton* m_button;
//
//      DECLARE_EVENT_TABLE()
//      DECLARE_DYNAMIC_CLASS_NO_COPY(MyXTIFrame)
//  };
//
//  IMPLEMENT_DYNAMIC_CLASS_XTI(MyXTIFrame, MyXTIFrame, "x.h")
//
//  WX_BEGIN_PROPERTIES_TABLE(MyXTIFrame)
//      WX_PROPERTY( Button, wxButton*, SetButton, GetButton, )
//  WX_END_PROPERTIES_TABLE()
//
//  WX_BEGIN_HANDLERS_TABLE(MyXTIFrame)
//      WX_HANDLER( ButtonClickHandler, wxCommandEvent )
//  WX_END_HANDLERS_TABLE()
//
//  WX_CONSTRUCTOR_5( MyXTIFrame, wxWindow*, Parent, wxWindowID, Id,
//                    wxString, Title, wxPoint, Position, wxSize, Size )
//
//  BEGIN_EVENT_TABLE(MyXTIFrame, wxFrame)
//  END_EVENT_TABLE()

// the following class "persists" (i.e. saves) a wxFrame into a wxWriter

class MyDesignerPersister : public wxPersister
{
public:
    MyDesignerPersister( wxDynamicObject * frame)
    {
        m_frame = frame;
    }

    virtual bool BeforeWriteDelegate( wxWriter *WXUNUSED(writer),
                                    const wxObject *object,  
                                    const wxClassInfo* WXUNUSED(classInfo), 
                                    const wxPropertyInfo *propInfo, 
                                    const wxObject *&eventSink, 
                                    const wxHandlerInfo* &handlerInfo )  
    {
        // this approach would be used it the handler would not 
        // be connected really in the designer, so we have to supply 
        // the information
        if ( object == m_frame->GetProperty(wxT("Button")).GetAsObject() && 
            propInfo == CLASSINFO( wxButton )->FindPropertyInfo("OnClick") )
        {
            eventSink = m_frame;
            handlerInfo = m_frame->GetClassInfo()->
                FindHandlerInfo("ButtonClickHandler");
            return true;
        }
        return false;
    }

private:
    wxDynamicObject *m_frame;
};

// sometimes linkers (at least MSVC and GCC ones) optimize the final EXE 
// even in debug mode pruning the object files which he "thinks" are useless;
// thus the classes defined in those files won't be available in the XTI
// table and the program will fail to allocate them.
// The following macro implements a simple hack to ensure that a given
// class is linked in.
#define wxENSURE_CLASS_IS_LINKED(x)     { x test; }

void RegisterFrameRTTI()
{
    // set up the RTTI info for a class (MyXTIFrame) which
    // is not defined anywhere in this program
    wxDynamicClassInfo *dyninfo = 
        dynamic_cast< wxDynamicClassInfo *>( wxClassInfo::FindClass(wxT("MyXTIFrame"))) ;
    if ( dyninfo == NULL )
    {
        dyninfo = new wxDynamicClassInfo(wxT("myxtiframe.h"),
                            wxT("MyXTIFrame"), 
                            CLASSINFO(wxFrame) );

        // this class has a property named "Button" and the relative handler:
        dyninfo->AddProperty("Button", wxGetTypeInfo((wxButton**) NULL));
        dyninfo->AddHandler("ButtonClickHandler", 
            NULL /* no instance of the handler method */, CLASSINFO( wxEvent ) );
    }
}

wxDynamicObject* CreateFrameRTTI()
{
    int baseID = 100;
    wxxVariant Params[10];

    // the class is now part of XTI internal table so that we can
    // get a pointer to it just searching it like any other class:
    wxFrame* frame;
    wxClassInfo *info = wxClassInfo::FindClass(wxT("MyXTIFrame"));
    wxASSERT( info );
    wxDynamicObject* frameWrapper = 
        dynamic_cast<wxDynamicObject*>( info->CreateObject() );
    Params[0] = wxxVariant((wxWindow*)(NULL));
    Params[1] = wxxVariant(wxWindowID(baseID++));
    Params[2] = wxxVariant(wxString(wxT("This is a frame created from XTI")));
    Params[3] = wxxVariant(wxPoint(-1,-1));
    Params[4] = wxxVariant(wxSize(400,300));
    Params[5] = wxxVariant((long)wxDEFAULT_FRAME_STYLE);
    wxASSERT( info->Create(frameWrapper, 6, Params ));
    frame = dynamic_cast<wxFrame*>(frameWrapper->GetSuperClassInstance());

    // now build a notebook inside it:
    wxNotebook* notebook;
    info = wxClassInfo::FindClass("wxNotebook");
    wxASSERT( info );
    notebook = wxDynamicCast( info->CreateObject(), wxNotebook );
    Params[0] = wxxVariant((wxWindow*)frame);
    Params[1] = wxxVariant(wxWindowID(baseID++));
    Params[2] = wxxVariant(wxPoint( 10, 10 ));
    Params[3] = wxxVariant(wxDefaultSize);
    Params[4] = wxxVariant((long)0);
    wxASSERT( info->Create(notebook, 5, Params ));

    // button page

    wxPanel* panel;
    info = wxClassInfo::FindClass("wxPanel");
    wxASSERT( info );
    panel = wxDynamicCast( info->CreateObject(), wxPanel );
    Params[0] = wxxVariant((wxWindow*)(notebook));
    Params[1] = wxxVariant(wxWindowID(baseID++));
    Params[2] = wxxVariant(wxPoint(-1,-1));
    Params[3] = wxxVariant(wxSize(-1,-1));
    Params[4] = wxxVariant((long)0);
    Params[5] = wxxVariant(wxString(wxT("Hello")));
    wxASSERT( info->Create(panel, 6, Params ));
    notebook->AddPage( panel, "Buttons" );

    wxButton* button;
    info = wxClassInfo::FindClass("wxButton");
    wxASSERT( info );
    button = wxDynamicCast( info->CreateObject(), wxButton );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(baseID++));
    Params[2] = wxxVariant(wxString(wxT("Click Me!")));
    Params[3] = wxxVariant(wxPoint( 10, 10 ));
    Params[4] = wxxVariant(wxSize(-1,-1));
    Params[5] = wxxVariant((long)0);
    wxASSERT( info->Create(button, 6, Params ));
    frameWrapper->SetProperty( "Button", wxxVariant( button ) );

    // other controls page

    info = wxClassInfo::FindClass("wxPanel");
    wxASSERT( info );
    panel = wxDynamicCast( info->CreateObject(), wxPanel );
    Params[0] = wxxVariant((wxWindow*)(notebook));
    Params[1] = wxxVariant(wxWindowID(baseID++));
    Params[2] = wxxVariant(wxPoint(-1,-1));
    Params[3] = wxxVariant(wxSize(-1,-1));
    Params[4] = wxxVariant((long)0);
    Params[5] = wxxVariant(wxString(wxT("Hello")));
    wxASSERT( info->Create(panel, 6, Params ));
    notebook->AddPage( panel, "Other Standard controls" );

    wxControl* control;
    info = wxClassInfo::FindClass("wxCheckBox");
    wxASSERT( info );
    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(baseID++));
    Params[2] = wxxVariant(wxString(wxT("A Checkbox")));
    Params[3] = wxxVariant(wxPoint( 10, 10 ));
    Params[4] = wxxVariant(wxSize(-1,-1));
    Params[5] = wxxVariant((long)0);
    wxASSERT( info->Create(control, 6, Params ));

    info = wxClassInfo::FindClass("wxRadioButton");
    wxASSERT( info );
    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(baseID++));
    Params[2] = wxxVariant(wxString(wxT("A Radiobutton")));
    Params[3] = wxxVariant(wxPoint( 10, 30 ));
    Params[4] = wxxVariant(wxSize(-1,-1));
    Params[5] = wxxVariant((long)0);
    wxASSERT( info->Create(control, 6, Params ));

    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[1] = wxxVariant(wxWindowID(baseID++));
    Params[2] = wxxVariant(wxString(wxT("Another One")));
    Params[3] = wxxVariant(wxPoint( 10, 50 ));
    wxASSERT( info->Create(control, 6, Params ));

    info = wxClassInfo::FindClass("wxStaticText");
    wxASSERT( info );
    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(baseID++));
    Params[2] = wxxVariant(wxString(wxT("A Static Text!")));
    Params[3] = wxxVariant(wxPoint( 10, 70 ));
    Params[4] = wxxVariant(wxSize(-1,-1));
    Params[5] = wxxVariant((long)0);
    wxASSERT( info->Create(control, 6, Params ));

    info = wxClassInfo::FindClass("wxStaticBox");
    wxASSERT( info );
    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(baseID++));
    Params[2] = wxxVariant(wxString(wxT("A Static Box")));
    Params[3] = wxxVariant(wxPoint( 10, 90 ));
    Params[4] = wxxVariant(wxSize(100,80));
    Params[5] = wxxVariant((long)0);
    wxASSERT( info->Create(control, 6, Params ));

    info = wxClassInfo::FindClass("wxTextCtrl");
    wxASSERT( info );
    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(baseID++));
    Params[2] = wxxVariant(wxString(wxT("A Text Control")));
    Params[3] = wxxVariant(wxPoint( 10, 200 ));
    Params[4] = wxxVariant(wxSize(-1,-1));
    Params[5] = wxxVariant((long)0);
    wxASSERT( info->Create(control, 6, Params ));

    // spins and gauges page

    info = wxClassInfo::FindClass("wxPanel");
    wxASSERT( info );
    panel = wxDynamicCast( info->CreateObject(), wxPanel );
    Params[0] = wxxVariant((wxWindow*)(notebook));
    Params[1] = wxxVariant(wxWindowID(baseID++));
    Params[2] = wxxVariant(wxPoint(-1,-1));
    Params[3] = wxxVariant(wxSize(-1,-1));
    Params[4] = wxxVariant((long)0);
    Params[5] = wxxVariant(wxString(wxT("Hello")));
    wxASSERT( info->Create(panel, 6, Params ));
    notebook->AddPage( panel, "Spins and Sliders" );

    wxENSURE_CLASS_IS_LINKED(wxSpinButton);

    info = wxClassInfo::FindClass("wxSpinButton");
    wxASSERT( info );
    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(baseID++));
    Params[2] = wxxVariant(wxPoint( 10, 10 ));
    Params[3] = wxxVariant(wxSize(-1,-1));
    Params[4] = wxxVariant((long)wxSP_VERTICAL | wxSP_ARROW_KEYS);
    wxASSERT( info->Create(control, 5, Params ));

    wxENSURE_CLASS_IS_LINKED(wxSpinCtrl);

    info = wxClassInfo::FindClass("wxSpinCtrl");
    wxASSERT( info );
    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(baseID++));
    Params[2] = wxxVariant(wxString("20"));
    Params[3] = wxxVariant(wxPoint( 40, 10 ));
    Params[4] = wxxVariant(wxSize(40,-1));
    Params[5] = wxxVariant((long) wxSP_ARROW_KEYS);
    wxASSERT( info->Create(control, 6, Params ));

    // MSVC likes to exclude from link wxGauge...
    wxENSURE_CLASS_IS_LINKED(wxGauge)
    wxENSURE_CLASS_IS_LINKED(wxCheckBox)
    wxENSURE_CLASS_IS_LINKED(wxSpinCtrl)

#ifdef __WXMSW__
    // under wxMSW wxGauge is simply #defined to wxGauge95
    info = wxClassInfo::FindClass("wxGauge95");
#else
    info = wxClassInfo::FindClass("wxGauge");
#endif
    wxASSERT( info );
    control = wxDynamicCast( info->CreateObject(), wxControl );
    Params[0] = wxxVariant((wxWindow*)(panel));
    Params[1] = wxxVariant(wxWindowID(baseID++));
    Params[2] = wxxVariant((int) 100);
    Params[3] = wxxVariant(wxPoint( 10, 50 ));
    Params[4] = wxxVariant(wxSize(-1,-1));
    Params[5] = wxxVariant((long) wxGA_HORIZONTAL);
    wxASSERT( info->Create(control, 6, Params ));
    dynamic_cast<wxGauge*>(control)->SetValue(20);

    return frameWrapper;
}

bool SaveFrameRTTI(const wxString &testFileName, wxDynamicObject *frame)
{
    // setup the XML document
    wxXmlDocument xml;
    wxXmlNode *root = new wxXmlNode(wxXML_ELEMENT_NODE, 
                                    "TestXTI", "This is the content");
    xml.SetRoot(root);

    // setup the XTI writer and persister
    wxXmlWriter writer(root);
    MyDesignerPersister persister(frame);

    // write the given wxObject into the XML document
    wxxVariantArray empty;
    writer.WriteObject( frame, frame->GetClassInfo(), &persister, 
                        wxString("myTestFrame"), empty );

    return xml.Save(testFileName);
}

wxDynamicObject* LoadFrameRTTI(const wxString &fileName)
{
    // load the XML document
    wxXmlDocument xml;
    if (!xml.Load(fileName))
        return NULL;

    wxXmlNode *root = xml.GetRoot();
    if (root->GetName() != "TestXTI")
        return NULL;

    // now depersist the wxFrame we saved into it using wxRuntimeDepersister
    wxRuntimeDepersister Callbacks;
    wxXmlReader Reader( root );
    int obj = Reader.ReadObject( wxString("myTestFrame"), &Callbacks );
    return (wxDynamicObject*)Callbacks.GetObject( obj );
}

bool GenerateFrameRTTICode(const wxString &inFileName, const wxString &outFileName)
{
    // is loading the streamed out component from xml and writing code that  
    // will create the same component 

    wxFFileOutputStream fos( outFileName );
    wxTextOutputStream tos( fos );
    if (!fos.IsOk())
        return false;

    wxXmlDocument xml;
    if (!xml.Load(inFileName))
        return false;

    wxXmlNode *root = xml.GetRoot();
    if (root->GetName() != "TestXTI")
        return false;

    // read the XML file using the wxCodeDepersister
    wxCodeDepersister Callbacks(&tos);
    wxXmlReader Reader(root);

    // ReadObject will return the ID of the object read??
    Reader.ReadObject( wxString("myTestFrame"), &Callbacks );

    return true;
}




// ----------------------------------------------------------------------------
// MyFrame event handlers
// ----------------------------------------------------------------------------

void MyFrame::OnPersist(wxCommandEvent& WXUNUSED(event))
{
    // first create a frame using XTI calls
    wxDynamicObject *frame = CreateFrameRTTI();
    if (!frame)
    {
        wxLogError(wxT("Cannot create the XTI frame!"));
        return;
    }

    // show the frame we're going to save to the user
    wxFrame *trueFrame = dynamic_cast<wxFrame *>( frame->GetSuperClassInstance() );
    trueFrame->Show();

    // ask the user where to save it
    wxFileDialog dlg(this, wxT("Where should the frame be saved?"),
                     wxEmptyString, wxT("test.xml"), wxT("XML files (*.xml)|*.xml"), 
                     wxFD_SAVE);
    if (dlg.ShowModal() == wxID_CANCEL)
        return;

    // then save it to a test XML file
    if (!SaveFrameRTTI(dlg.GetPath(), frame))
    {
        wxLogError(wxT("Cannot save the XTI frame into '%s'"), dlg.GetPath());
        return;
    }

    // now simply delete it
    delete frame;
}

void MyFrame::OnDepersist(wxCommandEvent& WXUNUSED(event))
{
    // ask the user which file to load
    wxFileDialog dlg(this, wxT("Which file contains the frame to depersist?"),
                     wxEmptyString, wxT("test.xml"), wxT("XML files (*.xml)|*.xml"), 
                     wxFD_OPEN);
    if (dlg.ShowModal() == wxID_CANCEL)
        return;

    wxObject *frame = LoadFrameRTTI(dlg.GetPath());
    if (!frame)
    {
        wxLogError(wxT("Could not depersist the wxFrame from '%s'"), dlg.GetPath());
        return;
    }

    wxFrame *trueFrame = dynamic_cast<wxFrame*>( frame );
    if ( !trueFrame )
    {
        wxDynamicObject* dyno = dynamic_cast< wxDynamicObject* >( frame );
        if ( dyno )
            trueFrame = dynamic_cast< wxFrame *>( dyno->GetSuperClassInstance() );
    }

    if ( trueFrame )
        trueFrame->Show();
    else
        wxLogError(wxT("Could not show the frame"));
}

void MyFrame::OnGenerateCode(wxCommandEvent& WXUNUSED(event))
{
    // ask the user which file to load
    wxFileDialog dlg(this, wxT("Which file contains the frame to work on?"),
                     wxEmptyString, wxT("test.xml"), wxT("XML files (*.xml)|*.xml"), 
                     wxFD_OPEN);
    if (dlg.ShowModal() == wxID_CANCEL)
        return;

    // ask the user which file to load
    wxFileDialog dlg2(this, wxT("Where should the C++ code be saved?"),
                      wxEmptyString, wxT("test.cpp"), wxT("Source files (*.cpp)|*.cpp"), 
                      wxFD_SAVE);
    if (dlg2.ShowModal() == wxID_CANCEL)
        return;

    if (!GenerateFrameRTTICode(dlg.GetPath(), dlg2.GetPath()))
        wxLogError(wxT("Could not generate the code for the frame!"));
}

void MyFrame::OnDumpClasses(wxCommandEvent& WXUNUSED(event))
{
    ClassListDialog dlg(this);
    dlg.ShowModal();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox(wxString::Format(
                    _T("Welcome to %s!\n")
                    _T("\n")
                    _T("This sample demonstrates wxWidgets eXtended RTTI (XTI) system."),
                    wxVERSION_STRING
                ),
                _T("About wxWidgets XTI sample"),
                wxOK | wxICON_INFORMATION,
                this);
}
