#include "BlockDlg.h"
#include "Editor.h"

BlockDlgImpl::BlockDlgImpl(wxWindow* parent, Editor& editor)
: BlockDlg(parent)
, m_editor(editor)
{
}

void BlockDlgImpl::update()
{
    Block* sblock = m_editor.selectedBlock();

    if (sblock)
    {
        m_typeBox->Enable();
        m_orderBtn->Enable();

        int type = static_cast<int>(sblock->type()) - 1;
        m_typeBox->SetSelection(type);
    }
    else
    {
        m_typeBox->Disable();
        m_orderBtn->Disable();
    }
}

void BlockDlgImpl::onChangeType(wxCommandEvent& event)
{
    Block* sblock = m_editor.selectedBlock();

    if (!sblock) return;

    int type = m_typeBox->GetSelection() + 1;
    sblock->setType(static_cast<BlockType>(type));

    onBlockUpdate();
}

void BlockDlgImpl::onOrderDown(wxSpinEvent& event)
{

}

void BlockDlgImpl::onOrderUp(wxSpinEvent& event)
{

}