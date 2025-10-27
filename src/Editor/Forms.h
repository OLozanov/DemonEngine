///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/menu.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/toolbar.h>
#include <wx/frame.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/radiobut.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/panel.h>
#include <wx/treectrl.h>
#include <wx/choice.h>
#include <wx/spinctrl.h>
#include <wx/tglbtn.h>
#include <wx/notebook.h>
#include <wx/spinbutt.h>
#include <wx/listbox.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/statbox.h>

///////////////////////////////////////////////////////////////////////////

#define ID_PRT 1000
#define ID_RUN 1001
#define ID_BLOCKS 1002

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrame
///////////////////////////////////////////////////////////////////////////////
class MainFrame : public wxFrame
{
	private:

	protected:
		wxMenuBar* m_menubar;
		wxMenu* m_fileMenu;
		wxMenu* m_editMenu;
		wxMenu* m_viewMenu;
		wxMenu* m_objectsMenu;
		wxMenu* m_mapMenu;
		wxMenu* m_helpMenu;
		wxToolBar* m_toolBar;
		wxToolBarToolBase* m_addTool;
		wxToolBarToolBase* m_subTool;
		wxToolBarToolBase* m_solidTool;
		wxToolBarToolBase* m_moveTool;
		wxToolBarToolBase* m_rotateTool;
		wxToolBarToolBase* m_scaleTool;
		wxToolBarToolBase* m_vertTool;
		wxToolBarToolBase* m_blockTool;
		wxToolBarToolBase* m_objectTool;
		wxToolBarToolBase* m_polyTool;
		wxToolBarToolBase* m_decalTool;
		wxToolBarToolBase* m_displaceTool;
		wxToolBarToolBase* m_surfaceEditTool;
		wxToolBarToolBase* m_cpTool;
		wxToolBarToolBase* m_surfaceTool;

		// Virtual event handlers, override them in your derived class
		virtual void onMenuNew( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuOpen( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuSave( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuSaveAs( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuPreferences( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuQuit( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDuplicate( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuPortals( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuPlane( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuCylinder( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuCone( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuSphere( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuSurface( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuCp( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuMapInfo( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuBuildMap( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuRebuildCsg( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMenuRunMap( wxCommandEvent& event ) { event.Skip(); }
		virtual void onAddBlock( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSubBlock( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSolidBlock( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMoveMode( wxCommandEvent& event ) { event.Skip(); }
		virtual void onRotateMode( wxCommandEvent& event ) { event.Skip(); }
		virtual void onScaleMode( wxCommandEvent& event ) { event.Skip(); }
		virtual void onVertType( wxCommandEvent& event ) { event.Skip(); }
		virtual void onBlockType( wxCommandEvent& event ) { event.Skip(); }
		virtual void onObjectType( wxCommandEvent& event ) { event.Skip(); }
		virtual void onPolyType( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDecalType( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDisplaceType( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSurfaceType( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCpType( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCreateSurface( wxCommandEvent& event ) { event.Skip(); }


	public:

		MainFrame( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("DemonEd 2.0"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1185,703 ), long style = wxCLOSE_BOX|wxDEFAULT_FRAME_STYLE|wxMAXIMIZE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER|wxTAB_TRAVERSAL );

		~MainFrame();

};

///////////////////////////////////////////////////////////////////////////////
/// Class PlaneDlg
///////////////////////////////////////////////////////////////////////////////
class PlaneDlg : public wxDialog
{
	private:

	protected:
		wxStaticBitmap* m_bitmap2;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_widthEdit;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_heightEdit;
		wxRadioButton* m_xyRadio;
		wxRadioButton* m_xzRadio;
		wxRadioButton* m_yzRadio;
		wxCheckBox* m_triangleChbox;
		wxButton* m_okBtn;
		wxButton* m_cancelBtn;

		// Virtual event handlers, override them in your derived class
		virtual void onOkBtn( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCancelBtn( wxCommandEvent& event ) { event.Skip(); }


	public:

		PlaneDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Plane Block"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 192,319 ), long style = wxDEFAULT_DIALOG_STYLE );

		~PlaneDlg();

};

///////////////////////////////////////////////////////////////////////////////
/// Class BoxDlg
///////////////////////////////////////////////////////////////////////////////
class BoxDlg : public wxDialog
{
	private:

	protected:
		wxStaticBitmap* m_bitmap1;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_widthEdit;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_heightEdit;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_depthEdit;
		wxButton* m_okBtn;
		wxButton* m_cancelBtn;

		// Virtual event handlers, override them in your derived class
		virtual void onOkBtn( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCancelBtn( wxCommandEvent& event ) { event.Skip(); }


	public:

		BoxDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Box Block"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 187,278 ), long style = wxDEFAULT_DIALOG_STYLE );

		~BoxDlg();

};

///////////////////////////////////////////////////////////////////////////////
/// Class CylinderDlg
///////////////////////////////////////////////////////////////////////////////
class CylinderDlg : public wxDialog
{
	private:

	protected:
		wxStaticBitmap* m_bitmap3;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_heightEdit;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_radiusEdit;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_sidesEdit;
		wxCheckBox* m_smoothBox;
		wxCheckBox* m_halfBox;
		wxButton* m_okBtn;
		wxButton* m_cancelBtn;

		// Virtual event handlers, override them in your derived class
		virtual void onOkBtn( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCancelBtn( wxCommandEvent& event ) { event.Skip(); }


	public:

		CylinderDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Cylinder Block"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 188,332 ), long style = wxDEFAULT_DIALOG_STYLE );

		~CylinderDlg();

};

///////////////////////////////////////////////////////////////////////////////
/// Class ConeDlg
///////////////////////////////////////////////////////////////////////////////
class ConeDlg : public wxDialog
{
	private:

	protected:
		wxStaticBitmap* m_bitmap3;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_heightEdit;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_radiusEdit;
		wxStaticText* m_staticText3;
		wxTextCtrl* m_sidesEdit;
		wxCheckBox* m_smoothBox;
		wxButton* m_okBtn;
		wxButton* m_cancelBtn;

		// Virtual event handlers, override them in your derived class
		virtual void onOkBtn( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCancelBtn( wxCommandEvent& event ) { event.Skip(); }


	public:

		ConeDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Cone/Pyramid Block"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 188,309 ), long style = wxDEFAULT_DIALOG_STYLE );

		~ConeDlg();

};

///////////////////////////////////////////////////////////////////////////////
/// Class SphereDlg
///////////////////////////////////////////////////////////////////////////////
class SphereDlg : public wxDialog
{
	private:

	protected:
		wxStaticBitmap* m_bitmap5;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_radiusEdit;
		wxStaticText* m_staticText2;
		wxTextCtrl* m_sidesEdit;
		wxCheckBox* m_smoothBox;
		wxCheckBox* m_halfBox;
		wxButton* m_okBtn;
		wxButton* m_cancelBtn;

		// Virtual event handlers, override them in your derived class
		virtual void onOkBtn( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCancelBtn( wxCommandEvent& event ) { event.Skip(); }


	public:

		SphereDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Sphere Block"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 191,296 ), long style = wxDEFAULT_DIALOG_STYLE );

		~SphereDlg();

};

///////////////////////////////////////////////////////////////////////////////
/// Class Console
///////////////////////////////////////////////////////////////////////////////
class Console : public wxPanel
{
	private:

	protected:
		wxPanel* m_previewPanel;
		wxNotebook* m_tabPanel;
		wxPanel* m_materialPanel;
		wxTreeCtrl* m_materialTree;
		wxButton* m_matRefreshBtn;
		wxButton* m_matApplyBtn;
		wxPanel* m_meshPanel;
		wxTreeCtrl* m_meshTree;
		wxButton* m_meshRefreshBtn;
		wxButton* m_meshCreateBtn;
		wxPanel* m_lightPanel;
		wxStaticText* m_staticText56;
		wxChoice* m_lightTypeBox;
		wxStaticText* m_staticText57;
		wxTextCtrl* m_lightRadiusEdt;
		wxStaticText* m_lightAngleCaption;
		wxTextCtrl* m_lightAngleEdt;
		wxTextCtrl* m_lightAngleInnerEdt;
		wxStaticText* m_staticText11;
		wxTextCtrl* m_lightFalloffEdt;
		wxStaticText* m_staticText12;
		wxTextCtrl* m_lightPowerEdt;
		wxStaticText* m_staticText55;
		wxChoice* m_lightShadowBox;
		wxPanel* m_lightColorPanel;
		wxButton* m_createLightBtn;
		wxPanel* m_entityPanel;
		wxTreeCtrl* m_entityTree;
		wxButton* m_createEntityBtn;
		wxPanel* m_surfacePanel;
		wxStaticText* m_staticText27;
		wxSpinCtrlDouble* m_dispRadiusEdit;
		wxStaticText* m_staticText28;
		wxSpinCtrlDouble* m_dispPowerEdit;
		wxStaticText* m_staticText48;
		wxSpinCtrl* m_layerCtrl;
		wxBitmapToggleButton* m_dispUpBtn;
		wxBitmapToggleButton* m_dispDownBtn;
		wxBitmapToggleButton* m_dispSmoothBtn;
		wxBitmapToggleButton* m_dispPaintBtn;
		wxStaticText* m_staticText50;
		wxButton* m_addDetailBtn;
		wxButton* m_editDetailBtn;
		wxButton* m_clearDetailBtn;

		// Virtual event handlers, override them in your derived class
		virtual void onMatTreeSelected( wxTreeEvent& event ) { event.Skip(); }
		virtual void onRefreshMaterials( wxCommandEvent& event ) { event.Skip(); }
		virtual void onApplyMaterial( wxCommandEvent& event ) { event.Skip(); }
		virtual void onMeshTreeSelected( wxTreeEvent& event ) { event.Skip(); }
		virtual void onRefreshMesh( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCreateMesh( wxCommandEvent& event ) { event.Skip(); }
		virtual void onChangeLightType( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSelectLightColor( wxMouseEvent& event ) { event.Skip(); }
		virtual void onCreateLight( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEntityTreeSelected( wxTreeEvent& event ) { event.Skip(); }
		virtual void onCreateObject( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDispRadiusChange( wxSpinDoubleEvent& event ) { event.Skip(); }
		virtual void onDispRadiusChange( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDispPowerChange( wxSpinDoubleEvent& event ) { event.Skip(); }
		virtual void onDispPowerChange( wxCommandEvent& event ) { event.Skip(); }
		virtual void onLayerChange( wxSpinEvent& event ) { event.Skip(); }
		virtual void onLayerChange( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDispUpBtn( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDispDownBtn( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDispSmoothBtn( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDispPaintBtn( wxCommandEvent& event ) { event.Skip(); }
		virtual void onAddDetailBtn( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEditDetailBtn( wxCommandEvent& event ) { event.Skip(); }
		virtual void onClearDetailBtn( wxCommandEvent& event ) { event.Skip(); }


	public:

		Console( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 300,753 ), long style = wxFULL_REPAINT_ON_RESIZE|wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~Console();

};

///////////////////////////////////////////////////////////////////////////////
/// Class DetailDlg
///////////////////////////////////////////////////////////////////////////////
class DetailDlg : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText51;
		wxSpinCtrl* m_layerCtrl;
		wxStaticText* m_staticText53;
		wxTextCtrl* m_modelCtrl;
		wxStaticText* m_staticText52;
		wxTextCtrl* m_materialCtrl;
		wxStaticText* m_staticText54;
		wxSpinCtrlDouble* m_densityCtrl;
		wxButton* m_addBtn;
		wxTreeCtrl* m_treeCtrl5;
		wxButton* m_addDetailBtn;
		wxButton* m_removeDetailBtn;

		// Virtual event handlers, override them in your derived class
		virtual void onAddBtn( wxCommandEvent& event ) { event.Skip(); }
		virtual void onAddDetailBtn( wxCommandEvent& event ) { event.Skip(); }
		virtual void onRemoveDetailDlg( wxCommandEvent& event ) { event.Skip(); }


	public:

		DetailDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Surface Details"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 498,211 ), long style = wxDEFAULT_DIALOG_STYLE );

		~DetailDlg();

};

///////////////////////////////////////////////////////////////////////////////
/// Class PolyDlg
///////////////////////////////////////////////////////////////////////////////
class PolyDlg : public wxDialog
{
	private:

	protected:
		wxCheckBox* m_portalBox;
		wxCheckBox* m_skyBox;
		wxCheckBox* m_invisibleBox;
		wxCheckBox* m_transparentBox;
		wxCheckBox* m_nocollisionBox;
		wxCheckBox* m_twosideBox;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_smgroopEdit;
		wxStaticText* m_staticText1;
		wxSpinButton* m_umoveBtn;
		wxTextCtrl* m_umoveEdit;
		wxStaticText* m_staticText2;
		wxSpinButton* m_uscaleBtn;
		wxTextCtrl* m_uscaleEdit;
		wxStaticText* m_staticText3;
		wxSpinButton* m_vmoveBtn;
		wxTextCtrl* m_vmoveEdit;
		wxStaticText* m_staticText4;
		wxSpinButton* m_vscaleBtn;
		wxTextCtrl* m_vscaleEdit;
		wxStaticText* m_staticText5;
		wxSpinButton* m_rotateBtn;
		wxTextCtrl* m_rotateEdit;
		wxStaticText* m_layersCaption;
		wxListBox* m_layerList;
		wxButton* m_addLayerBtn;
		wxButton* m_delLayerBtn;
		wxButton* m_editLayerBtn;
		wxButton* m_subdivideBtn;
		wxTextCtrl* m_resEdit;

		// Virtual event handlers, override them in your derived class
		virtual void onPortalBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSkyBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void onInvisibleBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void onTransparentBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void onNocollisionBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void onTwosideBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSmoothGroopEnter( wxCommandEvent& event ) { event.Skip(); }
		virtual void onUMoveDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void onUMoveUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void onUMoveRateChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void onUScaleDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void onUScaleUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void onUScaleRateChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void onVMoveDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void onVMoveUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void onVMoveRateChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void onVScaleDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void onVScaleUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void onVScaleRateChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void onRotateDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void onRotateUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void onRotateRateChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void onAddLayer( wxCommandEvent& event ) { event.Skip(); }
		virtual void onDeleteLayer( wxCommandEvent& event ) { event.Skip(); }
		virtual void onEditLayer( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSubdivide( wxCommandEvent& event ) { event.Skip(); }


	public:

		PolyDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Polygon Properties"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 510,249 ), long style = wxDEFAULT_DIALOG_STYLE );

		~PolyDlg();

};

///////////////////////////////////////////////////////////////////////////////
/// Class MapInfoDlg
///////////////////////////////////////////////////////////////////////////////
class MapInfoDlg : public wxDialog
{
	private:

	protected:
		wxPropertyGrid* m_propertyGrid;

		// Virtual event handlers, override them in your derived class
		virtual void onShow( wxShowEvent& event ) { event.Skip(); }
		virtual void onChanged( wxPropertyGridEvent& event ) { event.Skip(); }


	public:

		MapInfoDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Map Properties"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 284,431 ), long style = wxDEFAULT_DIALOG_STYLE );

		~MapInfoDlg();

};

///////////////////////////////////////////////////////////////////////////////
/// Class PreferencesDlg
///////////////////////////////////////////////////////////////////////////////
class PreferencesDlg : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText7;
		wxTextCtrl* m_moveEdt;
		wxCheckBox* m_moveBox;
		wxStaticText* m_staticText8;
		wxTextCtrl* m_rotateEdt;
		wxCheckBox* m_rotateBox;
		wxStaticText* m_staticText9;
		wxTextCtrl* m_scaleEdt;
		wxCheckBox* m_scaleBox;
		wxStaticText* m_staticText13;
		wxTextCtrl* m_flySpeedEdt;
		wxButton* m_cancelBtn;
		wxButton* m_okBtn;

		// Virtual event handlers, override them in your derived class
		virtual void onCancel( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOk( wxCommandEvent& event ) { event.Skip(); }


	public:

		PreferencesDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 276,207 ), long style = wxDEFAULT_DIALOG_STYLE );

		~PreferencesDlg();

};

///////////////////////////////////////////////////////////////////////////////
/// Class ObjectDlg
///////////////////////////////////////////////////////////////////////////////
class ObjectDlg : public wxDialog
{
	private:

	protected:
		wxPropertyGrid* m_propertyGrid;

		// Virtual event handlers, override them in your derived class
		virtual void onPropertyChange( wxPropertyGridEvent& event ) { event.Skip(); }


	public:

		ObjectDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Object Properties"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 306,480 ), long style = wxDEFAULT_DIALOG_STYLE );

		~ObjectDlg();

};

///////////////////////////////////////////////////////////////////////////////
/// Class SurfaceDlg
///////////////////////////////////////////////////////////////////////////////
class SurfaceDlg : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText29;
		wxTextCtrl* m_xPowerEdit;
		wxStaticText* m_staticText30;
		wxTextCtrl* m_yPowerEdit;
		wxStaticText* m_staticText33;
		wxTextCtrl* m_xSizeEdit;
		wxStaticText* m_staticText34;
		wxTextCtrl* m_ySizeEdit;
		wxStaticText* m_staticText31;
		wxTextCtrl* m_xResEdit;
		wxStaticText* m_staticText32;
		wxTextCtrl* m_yResEdit;
		wxStaticText* m_staticText35;
		wxChoice* m_mappingCombo;
		wxStaticText* m_staticText36;
		wxChoice* m_typeCombo;
		wxButton* m_createBtn;
		wxButton* m_cancelBtn;

		// Virtual event handlers, override them in your derived class
		virtual void onTypeChange( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCreate( wxCommandEvent& event ) { event.Skip(); }
		virtual void onCancel( wxCommandEvent& event ) { event.Skip(); }


	public:

		SurfaceDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Parametric Surface"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 435,185 ), long style = wxDEFAULT_DIALOG_STYLE );

		~SurfaceDlg();

};

///////////////////////////////////////////////////////////////////////////////
/// Class SurfaceCpDlg
///////////////////////////////////////////////////////////////////////////////
class SurfaceCpDlg : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText37;
		wxTextCtrl* m_weightEdit;
		wxButton* m_setWeightBtn;
		wxButton* m_closeBtn;

		// Virtual event handlers, override them in your derived class
		virtual void onSetWeight( wxCommandEvent& event ) { event.Skip(); }
		virtual void onClose( wxCommandEvent& event ) { event.Skip(); }


	public:

		SurfaceCpDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Surface Control Points"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 249,108 ), long style = wxDEFAULT_DIALOG_STYLE );

		~SurfaceCpDlg();

};

///////////////////////////////////////////////////////////////////////////////
/// Class SurfaceEditDlg
///////////////////////////////////////////////////////////////////////////////
class SurfaceEditDlg : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText38;
		wxTextCtrl* m_xresEdit;
		wxStaticText* m_staticText39;
		wxTextCtrl* m_yresEdit;
		wxStaticText* m_staticText40;
		wxChoice* m_mappingCombo;
		wxStaticText* m_staticText43;
		wxSpinButton* m_umoveBtn;
		wxTextCtrl* m_umoveEdit;
		wxStaticText* m_staticText44;
		wxSpinButton* m_uscaleBtn;
		wxTextCtrl* m_uscaleEdit;
		wxStaticText* m_staticText45;
		wxSpinButton* m_vmoveBtn;
		wxTextCtrl* m_vmoveEdit;
		wxStaticText* m_staticText46;
		wxSpinButton* m_vscaleBtn;
		wxTextCtrl* m_vscaleEdit;
		wxStaticText* m_staticText47;
		wxSpinButton* m_rotateBtn;
		wxTextCtrl* m_rotateEdit;
		wxCheckBox* m_collisionBox;
		wxCheckBox* m_normalsBox;
		wxCheckBox* m_xcycleBox;
		wxCheckBox* m_ycycleBox;
		wxCheckBox* m_xendBox;
		wxCheckBox* m_yendBox;
		wxButton* m_applyBtn;
		wxButton* m_closeBtn;

		// Virtual event handlers, override them in your derived class
		virtual void onUMoveDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void onUMoveUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void onUMoveRateChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void onUScaleDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void onUScaleUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void onUScaleRateChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void onVMoveDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void onVMoveUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void onVMoveRateChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void onVScaleDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void onVScaleUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void onVScaleRateChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void onRotateDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void onRotateUp( wxSpinEvent& event ) { event.Skip(); }
		virtual void onRotateRateChanged( wxCommandEvent& event ) { event.Skip(); }
		virtual void onApply( wxCommandEvent& event ) { event.Skip(); }
		virtual void onClose( wxCommandEvent& event ) { event.Skip(); }


	public:

		SurfaceEditDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Surface Properties"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 603,264 ), long style = wxDEFAULT_DIALOG_STYLE );

		~SurfaceEditDlg();

};

///////////////////////////////////////////////////////////////////////////////
/// Class BlockDlg
///////////////////////////////////////////////////////////////////////////////
class BlockDlg : public wxDialog
{
	private:

	protected:
		wxStaticText* m_staticText46;
		wxChoice* m_typeBox;
		wxStaticText* m_staticText47;
		wxChoice* m_classBox;
		wxSpinButton* m_orderBtn;

		// Virtual event handlers, override them in your derived class
		virtual void onChangeType( wxCommandEvent& event ) { event.Skip(); }
		virtual void onOrderDown( wxSpinEvent& event ) { event.Skip(); }
		virtual void onOrderUp( wxSpinEvent& event ) { event.Skip(); }


	public:

		BlockDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Block Properties"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 303,217 ), long style = wxDEFAULT_DIALOG_STYLE );

		~BlockDlg();

};

///////////////////////////////////////////////////////////////////////////////
/// Class ResourceDlg
///////////////////////////////////////////////////////////////////////////////
class ResourceDlg : public wxDialog
{
	private:

	protected:
		wxTreeCtrl* m_resourceTree;
		wxButton* m_closeBtn;
		wxButton* m_selectBtn;

		// Virtual event handlers, override them in your derived class
		virtual void onItemActivated( wxTreeEvent& event ) { event.Skip(); }
		virtual void onSelectionChanged( wxTreeEvent& event ) { event.Skip(); }
		virtual void onClose( wxCommandEvent& event ) { event.Skip(); }
		virtual void onSelect( wxCommandEvent& event ) { event.Skip(); }


	public:

		ResourceDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Select Resource"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 445,305 ), long style = wxDEFAULT_DIALOG_STYLE );

		~ResourceDlg();

};

