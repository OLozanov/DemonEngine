#pragma once

#include "Forms.h"
#include "ResourceDlg.h"

class Editor;

class PolyDlgImpl : public PolyDlg
{
public:
    PolyDlgImpl(wxWindow* parent, Editor& editor);

	void update();

private:
	void onPortalBox(wxCommandEvent& event) override;
	void onSkyBox(wxCommandEvent& event) override;
	void onInvisibleBox(wxCommandEvent& event) override;
	void onTransparentBox(wxCommandEvent& event) override;
	void onNocollisionBox(wxCommandEvent& event) override;
	void onTwosideBox(wxCommandEvent& event) override;

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

	void onUMoveRateChanged(wxCommandEvent& event) override;
	void onVMoveRateChanged(wxCommandEvent& event) override;
	void onUScaleRateChanged(wxCommandEvent& event) override;
	void onVScaleRateChanged(wxCommandEvent& event) override;
	void onRotateRateChanged(wxCommandEvent& event) override;

	void onSmoothGroopEnter(wxCommandEvent& event) override;

	void onSubdivide(wxCommandEvent& event) override;

	void onAddLayer(wxCommandEvent& event) override;
	void onDeleteLayer(wxCommandEvent& event) override;
	void onEditLayer(wxCommandEvent& event) override;

    Editor& m_editor;
	ResourceDlgImpl* m_resourceDlg;

	float m_umoveRate;
	float m_vmoveRate;
	float m_uscaleRate;
	float m_vscaleRate;
	float m_rotateRate;
};