#include "PlaneDlg.h"
#include "Editor.h"

PlaneDlgImpl::PlaneDlgImpl(wxWindow* parent, Editor& editor)
: PlaneDlg(parent)
, m_editor(editor)
{
}

void PlaneDlgImpl::onOkBtn(wxCommandEvent& event)
{
    double width = 2;
    double height = 2;

    m_widthEdit->GetValue().ToDouble(&width);
    m_heightEdit->GetValue().ToDouble(&height);

    PlaneType pltype;

    if (m_xyRadio->GetValue()) pltype = PlaneType::XY;
    if (m_xzRadio->GetValue()) pltype = PlaneType::XZ;
    if (m_yzRadio->GetValue()) pltype = PlaneType::YZ;

    if (m_triangleChbox->IsChecked())
        m_editor.createTriangle(width, height, pltype);
    else
        m_editor.createPlane(width, height, pltype);

    Close();
}

void PlaneDlgImpl::onCancelBtn(wxCommandEvent& event)
{
    Close();
}