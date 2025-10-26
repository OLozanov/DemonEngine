#include "EditorApp.h"

#include "MainFrame.h"
#include <wx/image.h>

IMPLEMENT_APP(EditorApp);

bool EditorApp::OnInit()
{
    bool wxsOK = true;
    
    wxInitAllImageHandlers();
    
    if (wxsOK)
    {
        MainFrame* frame = new MainFrameImpl(nullptr);
        frame->Show();
        frame->Maximize();
    }

    return wxsOK;

}