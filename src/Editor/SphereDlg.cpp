#include "SphereDlg.h"
#include "Editor.h"

SphereDlgImpl::SphereDlgImpl(wxWindow* parent, Editor& editor)
: SphereDlg(parent)
, m_editor(editor)
{
}

void SphereDlgImpl::onOkBtn(wxCommandEvent& event)
{
    double radius = 2;
    int sides = 8;

    m_radiusEdit->GetValue().ToDouble(&radius);
    m_sidesEdit->GetValue().ToInt(&sides);

    bool smooth = m_smoothBox->GetValue();
    bool hemisphere = m_halfBox->GetValue();

    if (hemisphere)
        m_editor.createHemisphere(radius, sides, smooth);
    else
        m_editor.createSphere(radius, sides, smooth);

    Close();
}

void SphereDlgImpl::onCancelBtn(wxCommandEvent& event)
{
    Close();
}