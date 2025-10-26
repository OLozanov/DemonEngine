#include "PreferencesDlg.h"
#include "Editor.h"
#include "View/PerspectiveView.h"

PreferencesDlgImpl::PreferencesDlgImpl(wxWindow* parent, Editor& editor, PerspectiveView* mainView)
: PreferencesDlg(parent)
, m_editor(editor)
, m_mainView(mainView)
{
}

void PreferencesDlgImpl::onCancel(wxCommandEvent& event)
{
    Hide();
}

void PreferencesDlgImpl::onOk(wxCommandEvent& event)
{
    double val;

    if ((m_moveEdt->GetValue()).ToDouble(&val)) m_editor.setMoveStep(val);
    if ((m_rotateEdt->GetValue()).ToDouble(&val)) m_editor.setRotateStep(val);
    if ((m_scaleEdt->GetValue()).ToDouble(&val)) m_editor.setScaleStep(val);

    m_editor.setDiscreteMove(m_moveBox->IsChecked());
    m_editor.setDiscreteRotate(m_rotateBox->IsChecked());
    m_editor.setDiscreteScale(m_scaleBox->IsChecked());

    if ((m_flySpeedEdt->GetValue()).ToDouble(&val)) m_mainView->setMoveSpeed(val);

    Hide();
}