#include "ConeDlg.h"
#include "Editor.h"

ConeDlgImpl::ConeDlgImpl(wxWindow* parent, Editor& editor)
: ConeDlg(parent)
, m_editor(editor)
{
}

void ConeDlgImpl::onOkBtn(wxCommandEvent& event)
{
    double height = 2;
    double radius = 2;
    int sides = 6;

    m_heightEdit->GetValue().ToDouble(&height);
    m_radiusEdit->GetValue().ToDouble(&radius);
    m_sidesEdit->GetValue().ToInt(&sides);

    bool smooth = m_smoothBox->GetValue();

    m_editor.createCone(height, radius, sides, smooth);

    Close();
}

void ConeDlgImpl::onCancelBtn(wxCommandEvent& event)
{
    Close();
}