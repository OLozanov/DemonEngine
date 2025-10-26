#pragma once

#include "Forms.h"

#include "Utils/EventHandler.h"

class Editor;

class SurfaceEditDlgImpl : public SurfaceEditDlg
{
public:
    SurfaceEditDlgImpl(wxWindow* parent, Editor& editor);

    void update();

	Event<void()> onSurfaceUpdate;

private:
	void onUMoveDown(wxSpinEvent& event) override;
	void onUMoveUp(wxSpinEvent& event) override;
	void onUScaleDown(wxSpinEvent& event) override;
	void onUScaleUp(wxSpinEvent& event) override;
	void onVMoveDown(wxSpinEvent& event) override;
	void onVMoveUp(wxSpinEvent& event) override;
	void onVScaleDown(wxSpinEvent& event) override;
	void onVScaleUp(wxSpinEvent& event) override;
	void onRotateDown(wxSpinEvent& event) override;
	void onRotateUp(wxSpinEvent& event) override;
	void onApply(wxCommandEvent& event) override;
	void onClose(wxCommandEvent& event) override;

	void onUMoveRateChanged(wxCommandEvent& event) override;
	void onVMoveRateChanged(wxCommandEvent& event) override;
	void onUScaleRateChanged(wxCommandEvent& event) override;
	void onVScaleRateChanged(wxCommandEvent& event) override;
	void onRotateRateChanged(wxCommandEvent& event) override;

    Editor& m_editor;

	float m_umoveRate;
	float m_vmoveRate;
	float m_uscaleRate;
	float m_vscaleRate;
	float m_rotateRate;
};