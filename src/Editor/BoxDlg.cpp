#include "BoxDlg.h"
#include "Editor.h"

BoxDlgImpl::BoxDlgImpl(wxWindow* parent, Editor& editor)
: BoxDlg(parent)
, m_editor(editor)
{
}

void BoxDlgImpl::onOkBtn(wxCommandEvent& event)
{
    double width = 2;
    double height = 2;
    double depth = 2;

    m_widthEdit->GetValue().ToDouble(&width);
    m_heightEdit->GetValue().ToDouble(&height);
    m_depthEdit->GetValue().ToDouble(&depth);

    m_editor.createBox(width, height, depth);

    Close();
}

void BoxDlgImpl::onCancelBtn(wxCommandEvent& event)
{
    Close();
}