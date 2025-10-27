///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Forms.h"

///////////////////////////////////////////////////////////////////////////

MainFrame::MainFrame( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	m_menubar = new wxMenuBar( 0 );
	m_fileMenu = new wxMenu();
	wxMenuItem* m_newItem;
	m_newItem = new wxMenuItem( m_fileMenu, wxID_ANY, wxString( wxT("New") ) , wxEmptyString, wxITEM_NORMAL );
	m_fileMenu->Append( m_newItem );

	wxMenuItem* m_openItem;
	m_openItem = new wxMenuItem( m_fileMenu, wxID_ANY, wxString( wxT("Open") ) , wxEmptyString, wxITEM_NORMAL );
	m_fileMenu->Append( m_openItem );

	m_fileMenu->AppendSeparator();

	wxMenuItem* m_saveItem;
	m_saveItem = new wxMenuItem( m_fileMenu, wxID_SAVE, wxString( wxT("Save") ) , wxEmptyString, wxITEM_NORMAL );
	m_fileMenu->Append( m_saveItem );
	m_saveItem->Enable( false );

	wxMenuItem* m_saveasItem;
	m_saveasItem = new wxMenuItem( m_fileMenu, wxID_ANY, wxString( wxT("SaveAs") ) , wxEmptyString, wxITEM_NORMAL );
	m_fileMenu->Append( m_saveasItem );

	m_fileMenu->AppendSeparator();

	wxMenuItem* m_prefItem;
	m_prefItem = new wxMenuItem( m_fileMenu, wxID_ANY, wxString( wxT("Preferences") ) , wxEmptyString, wxITEM_NORMAL );
	m_fileMenu->Append( m_prefItem );

	m_fileMenu->AppendSeparator();

	wxMenuItem* m_quitItem;
	m_quitItem = new wxMenuItem( m_fileMenu, wxID_ANY, wxString( wxT("Quit") ) , wxEmptyString, wxITEM_NORMAL );
	m_fileMenu->Append( m_quitItem );

	m_menubar->Append( m_fileMenu, wxT("File") );

	m_editMenu = new wxMenu();
	wxMenuItem* m_duplicateItem;
	m_duplicateItem = new wxMenuItem( m_editMenu, wxID_ANY, wxString( wxT("Duplicate") ) + wxT('\t') + wxT("Shift+D"), wxEmptyString, wxITEM_NORMAL );
	m_editMenu->Append( m_duplicateItem );

	m_editMenu->AppendSeparator();

	wxMenuItem* m_undoItem;
	m_undoItem = new wxMenuItem( m_editMenu, wxID_ANY, wxString( wxT("Undo") ) , wxEmptyString, wxITEM_NORMAL );
	m_editMenu->Append( m_undoItem );

	wxMenuItem* m_redoItem;
	m_redoItem = new wxMenuItem( m_editMenu, wxID_ANY, wxString( wxT("Redo") ) , wxEmptyString, wxITEM_NORMAL );
	m_editMenu->Append( m_redoItem );

	m_editMenu->AppendSeparator();

	wxMenuItem* m_addItem;
	m_addItem = new wxMenuItem( m_editMenu, wxID_ANY, wxString( wxT("Add") ) , wxEmptyString, wxITEM_NORMAL );
	m_editMenu->Append( m_addItem );

	wxMenuItem* m_subtractItem;
	m_subtractItem = new wxMenuItem( m_editMenu, wxID_ANY, wxString( wxT("Subtract") ) , wxEmptyString, wxITEM_NORMAL );
	m_editMenu->Append( m_subtractItem );

	wxMenuItem* m_solidItem;
	m_solidItem = new wxMenuItem( m_editMenu, wxID_ANY, wxString( wxT("Solid") ) , wxEmptyString, wxITEM_NORMAL );
	m_editMenu->Append( m_solidItem );

	m_menubar->Append( m_editMenu, wxT("Edit") );

	m_viewMenu = new wxMenu();
	wxMenuItem* m_portalsItem;
	m_portalsItem = new wxMenuItem( m_viewMenu, ID_PRT, wxString( wxT("Show Portals") ) , wxEmptyString, wxITEM_CHECK );
	m_viewMenu->Append( m_portalsItem );

	m_menubar->Append( m_viewMenu, wxT("View") );

	m_objectsMenu = new wxMenu();
	wxMenuItem* m_menuPlane;
	m_menuPlane = new wxMenuItem( m_objectsMenu, wxID_ANY, wxString( wxT("Plane") ) , wxEmptyString, wxITEM_NORMAL );
	m_objectsMenu->Append( m_menuPlane );

	wxMenuItem* m_menuBox;
	m_menuBox = new wxMenuItem( m_objectsMenu, wxID_ANY, wxString( wxT("Box") ) , wxEmptyString, wxITEM_NORMAL );
	m_objectsMenu->Append( m_menuBox );

	wxMenuItem* m_menuCylinder;
	m_menuCylinder = new wxMenuItem( m_objectsMenu, wxID_ANY, wxString( wxT("Cylinder") ) , wxEmptyString, wxITEM_NORMAL );
	m_objectsMenu->Append( m_menuCylinder );

	wxMenuItem* m_menuCone;
	m_menuCone = new wxMenuItem( m_objectsMenu, wxID_ANY, wxString( wxT("Cone") ) , wxEmptyString, wxITEM_NORMAL );
	m_objectsMenu->Append( m_menuCone );

	wxMenuItem* m_menuSphere;
	m_menuSphere = new wxMenuItem( m_objectsMenu, wxID_ANY, wxString( wxT("Sphere") ) , wxEmptyString, wxITEM_NORMAL );
	m_objectsMenu->Append( m_menuSphere );

	m_objectsMenu->AppendSeparator();

	wxMenuItem* m_menuSurface;
	m_menuSurface = new wxMenuItem( m_objectsMenu, wxID_ANY, wxString( wxT("Surface") ) , wxEmptyString, wxITEM_NORMAL );
	m_objectsMenu->Append( m_menuSurface );

	wxMenuItem* m_menuCp;
	m_menuCp = new wxMenuItem( m_objectsMenu, wxID_ANY, wxString( wxT("Control Points") ) , wxEmptyString, wxITEM_NORMAL );
	m_objectsMenu->Append( m_menuCp );

	m_menubar->Append( m_objectsMenu, wxT("Objects") );

	m_mapMenu = new wxMenu();
	wxMenuItem* m_mapPropsItem;
	m_mapPropsItem = new wxMenuItem( m_mapMenu, wxID_ANY, wxString( wxT("Map Properties") ) , wxEmptyString, wxITEM_NORMAL );
	m_mapMenu->Append( m_mapPropsItem );

	wxMenuItem* m_buildMapItem;
	m_buildMapItem = new wxMenuItem( m_mapMenu, wxID_ANY, wxString( wxT("Build Map") ) , wxEmptyString, wxITEM_NORMAL );
	m_mapMenu->Append( m_buildMapItem );

	wxMenuItem* m_rebuildCsgItem;
	m_rebuildCsgItem = new wxMenuItem( m_mapMenu, wxID_ANY, wxString( wxT("Rebuild Csg") ) , wxEmptyString, wxITEM_NORMAL );
	m_mapMenu->Append( m_rebuildCsgItem );

	wxMenuItem* m_runItem;
	m_runItem = new wxMenuItem( m_mapMenu, ID_RUN, wxString( wxT("Run Map") ) , wxEmptyString, wxITEM_NORMAL );
	m_mapMenu->Append( m_runItem );
	m_runItem->Enable( false );

	m_menubar->Append( m_mapMenu, wxT("Map") );

	m_helpMenu = new wxMenu();
	m_menubar->Append( m_helpMenu, wxT("Help") );

	this->SetMenuBar( m_menubar );

	m_toolBar = this->CreateToolBar( wxTB_VERTICAL|wxBORDER_SIMPLE, wxID_ANY );
	m_toolBar->SetToolBitmapSize( wxSize( 32,32 ) );
	m_toolBar->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_3DLIGHT ) );
	m_toolBar->SetMaxSize( wxSize( 39,-1 ) );

	m_addTool = m_toolBar->AddTool( wxID_ANY, wxT("add block"), wxBitmap( wxT("Editor/addblock.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_subTool = m_toolBar->AddTool( wxID_ANY, wxT("subtruct block"), wxBitmap( wxT("Editor/subblock.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_solidTool = m_toolBar->AddTool( wxID_ANY, wxT("solid block"), wxBitmap( wxT("Editor/solidblock.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_toolBar->AddSeparator();

	m_moveTool = m_toolBar->AddTool( wxID_ANY, wxT("move"), wxBitmap( wxT("Editor/objmove.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_RADIO, wxEmptyString, wxEmptyString, NULL );

	m_rotateTool = m_toolBar->AddTool( wxID_ANY, wxT("rotate"), wxBitmap( wxT("Editor/objrot.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_RADIO, wxEmptyString, wxEmptyString, NULL );

	m_scaleTool = m_toolBar->AddTool( wxID_ANY, wxT("scale"), wxBitmap( wxT("Editor/objscale.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_RADIO, wxEmptyString, wxEmptyString, NULL );

	m_toolBar->AddSeparator();

	m_vertTool = m_toolBar->AddTool( wxID_ANY, wxT("vertices"), wxBitmap( wxT("Editor/vedit.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_RADIO, wxEmptyString, wxEmptyString, NULL );

	m_blockTool = m_toolBar->AddTool( ID_BLOCKS, wxT("blocks"), wxBitmap( wxT("Editor/block.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_RADIO, wxEmptyString, wxEmptyString, NULL );

	m_objectTool = m_toolBar->AddTool( wxID_ANY, wxT("objects"), wxBitmap( wxT("Editor/object.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_RADIO, wxEmptyString, wxEmptyString, NULL );

	m_polyTool = m_toolBar->AddTool( wxID_ANY, wxT("polygon edit"), wxBitmap( wxT("Editor/texedit.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_RADIO, wxEmptyString, wxEmptyString, NULL );

	m_decalTool = m_toolBar->AddTool( wxID_ANY, wxT("tool"), wxBitmap( wxT("Editor/decal.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_RADIO, wxEmptyString, wxEmptyString, NULL );

	m_displaceTool = m_toolBar->AddTool( wxID_ANY, wxT("surface displace"), wxBitmap( wxT("Editor/terra.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_RADIO, wxEmptyString, wxEmptyString, NULL );

	m_surfaceEditTool = m_toolBar->AddTool( wxID_ANY, wxT("parametric surfaces"), wxBitmap( wxT("Editor/surf_edit.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_RADIO, wxEmptyString, wxEmptyString, NULL );

	m_cpTool = m_toolBar->AddTool( wxID_ANY, wxT("control points"), wxBitmap( wxT("Editor/cpedit.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_RADIO, wxEmptyString, wxEmptyString, NULL );

	m_toolBar->AddSeparator();

	m_surfaceTool = m_toolBar->AddTool( wxID_ANY, wxT("create surface"), wxBitmap( wxT("Editor/surf.png"), wxBITMAP_TYPE_ANY ), wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString, NULL );

	m_toolBar->Realize();


	this->Centre( wxBOTH );

	// Connect Events
	m_fileMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuNew ), this, m_newItem->GetId());
	m_fileMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuOpen ), this, m_openItem->GetId());
	m_fileMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuSave ), this, m_saveItem->GetId());
	m_fileMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuSaveAs ), this, m_saveasItem->GetId());
	m_fileMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuPreferences ), this, m_prefItem->GetId());
	m_fileMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuQuit ), this, m_quitItem->GetId());
	m_editMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onDuplicate ), this, m_duplicateItem->GetId());
	m_viewMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuPortals ), this, m_portalsItem->GetId());
	m_objectsMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuPlane ), this, m_menuPlane->GetId());
	m_objectsMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuBox ), this, m_menuBox->GetId());
	m_objectsMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuCylinder ), this, m_menuCylinder->GetId());
	m_objectsMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuCone ), this, m_menuCone->GetId());
	m_objectsMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuSphere ), this, m_menuSphere->GetId());
	m_objectsMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuSurface ), this, m_menuSurface->GetId());
	m_objectsMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuCp ), this, m_menuCp->GetId());
	m_mapMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuMapInfo ), this, m_mapPropsItem->GetId());
	m_mapMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuBuildMap ), this, m_buildMapItem->GetId());
	m_mapMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuRebuildCsg ), this, m_rebuildCsgItem->GetId());
	m_mapMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( MainFrame::onMenuRunMap ), this, m_runItem->GetId());
	this->Connect( m_addTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onAddBlock ) );
	this->Connect( m_subTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onSubBlock ) );
	this->Connect( m_solidTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onSolidBlock ) );
	this->Connect( m_moveTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onMoveMode ) );
	this->Connect( m_rotateTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onRotateMode ) );
	this->Connect( m_scaleTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onScaleMode ) );
	this->Connect( m_vertTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onVertType ) );
	this->Connect( m_blockTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onBlockType ) );
	this->Connect( m_objectTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onObjectType ) );
	this->Connect( m_polyTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onPolyType ) );
	this->Connect( m_decalTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onDecalType ) );
	this->Connect( m_displaceTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onDisplaceType ) );
	this->Connect( m_surfaceEditTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onSurfaceType ) );
	this->Connect( m_cpTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onCpType ) );
	this->Connect( m_surfaceTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onCreateSurface ) );
}

MainFrame::~MainFrame()
{
	// Disconnect Events
	this->Disconnect( m_addTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onAddBlock ) );
	this->Disconnect( m_subTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onSubBlock ) );
	this->Disconnect( m_solidTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onSolidBlock ) );
	this->Disconnect( m_moveTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onMoveMode ) );
	this->Disconnect( m_rotateTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onRotateMode ) );
	this->Disconnect( m_scaleTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onScaleMode ) );
	this->Disconnect( m_vertTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onVertType ) );
	this->Disconnect( m_blockTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onBlockType ) );
	this->Disconnect( m_objectTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onObjectType ) );
	this->Disconnect( m_polyTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onPolyType ) );
	this->Disconnect( m_decalTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onDecalType ) );
	this->Disconnect( m_displaceTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onDisplaceType ) );
	this->Disconnect( m_surfaceEditTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onSurfaceType ) );
	this->Disconnect( m_cpTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onCpType ) );
	this->Disconnect( m_surfaceTool->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( MainFrame::onCreateSurface ) );

}

PlaneDlg::PlaneDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxVERTICAL );

	m_bitmap2 = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("Editor/plane.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize( 100,100 ), wxBORDER_SUNKEN );
	bSizer35->Add( m_bitmap2, 0, wxALIGN_CENTER|wxALL, 5 );

	wxBoxSizer* bSizer36;
	bSizer36 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Width:"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_staticText1->Wrap( -1 );
	bSizer36->Add( m_staticText1, 0, wxALL, 5 );

	m_widthEdit = new wxTextCtrl( this, wxID_ANY, wxT("2"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizer36->Add( m_widthEdit, 0, wxALL, 5 );


	bSizer35->Add( bSizer36, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer37;
	bSizer37 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Height:"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_staticText2->Wrap( -1 );
	bSizer37->Add( m_staticText2, 0, wxALL, 5 );

	m_heightEdit = new wxTextCtrl( this, wxID_ANY, wxT("2"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizer37->Add( m_heightEdit, 0, wxALL, 5 );


	bSizer35->Add( bSizer37, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer38;
	bSizer38 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer39;
	bSizer39 = new wxBoxSizer( wxVERTICAL );

	m_xyRadio = new wxRadioButton( this, wxID_ANY, wxT("XY plane"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer39->Add( m_xyRadio, 0, wxALL, 5 );

	m_xzRadio = new wxRadioButton( this, wxID_ANY, wxT("XZ plane"), wxDefaultPosition, wxDefaultSize, 0 );
	m_xzRadio->SetValue( true );
	bSizer39->Add( m_xzRadio, 0, wxALL, 5 );

	m_yzRadio = new wxRadioButton( this, wxID_ANY, wxT("YZ plane"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer39->Add( m_yzRadio, 0, wxALL, 5 );


	bSizer38->Add( bSizer39, 1, wxEXPAND, 5 );

	m_triangleChbox = new wxCheckBox( this, wxID_ANY, wxT("Triangle"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer38->Add( m_triangleChbox, 0, wxALL, 5 );


	bSizer35->Add( bSizer38, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxHORIZONTAL );

	m_okBtn = new wxButton( this, wxID_ANY, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer41->Add( m_okBtn, 0, wxALL, 5 );


	bSizer41->Add( 0, 0, 1, wxEXPAND, 5 );

	m_cancelBtn = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer41->Add( m_cancelBtn, 0, wxALL, 5 );


	bSizer35->Add( bSizer41, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer35 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_okBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlaneDlg::onOkBtn ), NULL, this );
	m_cancelBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlaneDlg::onCancelBtn ), NULL, this );
}

PlaneDlg::~PlaneDlg()
{
	// Disconnect Events
	m_okBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlaneDlg::onOkBtn ), NULL, this );
	m_cancelBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PlaneDlg::onCancelBtn ), NULL, this );

}

BoxDlg::BoxDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer30;
	bSizer30 = new wxBoxSizer( wxVERTICAL );

	m_bitmap1 = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("Editor/box.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize( 100,100 ), wxBORDER_SUNKEN );
	bSizer30->Add( m_bitmap1, 0, wxALIGN_CENTER|wxALL, 5 );

	wxBoxSizer* bSizer31;
	bSizer31 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Width:"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_staticText1->Wrap( -1 );
	bSizer31->Add( m_staticText1, 0, wxALL, 5 );

	m_widthEdit = new wxTextCtrl( this, wxID_ANY, wxT("2"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizer31->Add( m_widthEdit, 0, wxALL, 5 );


	bSizer30->Add( bSizer31, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer33;
	bSizer33 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Height:"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_staticText2->Wrap( -1 );
	bSizer33->Add( m_staticText2, 0, wxALL, 5 );

	m_heightEdit = new wxTextCtrl( this, wxID_ANY, wxT("2"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizer33->Add( m_heightEdit, 0, wxALL, 5 );


	bSizer30->Add( bSizer33, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer34;
	bSizer34 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Depth:"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_staticText3->Wrap( -1 );
	bSizer34->Add( m_staticText3, 0, wxALL, 5 );

	m_depthEdit = new wxTextCtrl( this, wxID_ANY, wxT("2"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizer34->Add( m_depthEdit, 0, wxALL, 5 );


	bSizer30->Add( bSizer34, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer35;
	bSizer35 = new wxBoxSizer( wxHORIZONTAL );

	m_okBtn = new wxButton( this, wxID_ANY, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer35->Add( m_okBtn, 0, wxALL, 5 );


	bSizer35->Add( 0, 0, 1, wxEXPAND, 5 );

	m_cancelBtn = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer35->Add( m_cancelBtn, 0, wxALL, 5 );


	bSizer30->Add( bSizer35, 0, wxEXPAND, 5 );


	this->SetSizer( bSizer30 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_okBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BoxDlg::onOkBtn ), NULL, this );
	m_cancelBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BoxDlg::onCancelBtn ), NULL, this );
}

BoxDlg::~BoxDlg()
{
	// Disconnect Events
	m_okBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BoxDlg::onOkBtn ), NULL, this );
	m_cancelBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( BoxDlg::onCancelBtn ), NULL, this );

}

CylinderDlg::CylinderDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxVERTICAL );

	m_bitmap3 = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("Editor/cylinder.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize( 100,100 ), wxBORDER_SUNKEN );
	bSizer41->Add( m_bitmap3, 0, wxALIGN_CENTER|wxALL, 5 );

	wxBoxSizer* bSizer42;
	bSizer42 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Height:"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_staticText1->Wrap( -1 );
	bSizer42->Add( m_staticText1, 0, wxALL, 5 );

	m_heightEdit = new wxTextCtrl( this, wxID_ANY, wxT("2"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizer42->Add( m_heightEdit, 0, wxALL, 5 );


	bSizer41->Add( bSizer42, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer43;
	bSizer43 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Radius:"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_staticText2->Wrap( -1 );
	bSizer43->Add( m_staticText2, 0, wxALL, 5 );

	m_radiusEdit = new wxTextCtrl( this, wxID_ANY, wxT("1"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizer43->Add( m_radiusEdit, 0, wxALL, 5 );


	bSizer41->Add( bSizer43, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer44;
	bSizer44 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Sides:"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_staticText3->Wrap( -1 );
	bSizer44->Add( m_staticText3, 0, wxALL, 5 );

	m_sidesEdit = new wxTextCtrl( this, wxID_ANY, wxT("6"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizer44->Add( m_sidesEdit, 0, wxALL, 5 );


	bSizer41->Add( bSizer44, 0, wxEXPAND, 5 );

	m_smoothBox = new wxCheckBox( this, wxID_ANY, wxT("Smooth"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer41->Add( m_smoothBox, 0, wxALL, 5 );

	m_halfBox = new wxCheckBox( this, wxID_ANY, wxT("Half"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer41->Add( m_halfBox, 0, wxALL, 5 );

	wxBoxSizer* bSizer45;
	bSizer45 = new wxBoxSizer( wxHORIZONTAL );

	m_okBtn = new wxButton( this, wxID_ANY, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer45->Add( m_okBtn, 0, wxALL, 5 );


	bSizer45->Add( 0, 0, 1, wxEXPAND, 5 );

	m_cancelBtn = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer45->Add( m_cancelBtn, 0, wxALL, 5 );


	bSizer41->Add( bSizer45, 0, wxEXPAND, 5 );


	this->SetSizer( bSizer41 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_okBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CylinderDlg::onOkBtn ), NULL, this );
	m_cancelBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CylinderDlg::onCancelBtn ), NULL, this );
}

CylinderDlg::~CylinderDlg()
{
	// Disconnect Events
	m_okBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CylinderDlg::onOkBtn ), NULL, this );
	m_cancelBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CylinderDlg::onCancelBtn ), NULL, this );

}

ConeDlg::ConeDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxVERTICAL );

	m_bitmap3 = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("Editor/cone.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize( 100,100 ), wxBORDER_SUNKEN );
	bSizer41->Add( m_bitmap3, 0, wxALIGN_CENTER|wxALL, 5 );

	wxBoxSizer* bSizer42;
	bSizer42 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Height:"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_staticText1->Wrap( -1 );
	bSizer42->Add( m_staticText1, 0, wxALL, 5 );

	m_heightEdit = new wxTextCtrl( this, wxID_ANY, wxT("2"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizer42->Add( m_heightEdit, 0, wxALL, 5 );


	bSizer41->Add( bSizer42, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer43;
	bSizer43 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Radius:"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_staticText2->Wrap( -1 );
	bSizer43->Add( m_staticText2, 0, wxALL, 5 );

	m_radiusEdit = new wxTextCtrl( this, wxID_ANY, wxT("1"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizer43->Add( m_radiusEdit, 0, wxALL, 5 );


	bSizer41->Add( bSizer43, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer44;
	bSizer44 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("Sides:"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_staticText3->Wrap( -1 );
	bSizer44->Add( m_staticText3, 0, wxALL, 5 );

	m_sidesEdit = new wxTextCtrl( this, wxID_ANY, wxT("6"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizer44->Add( m_sidesEdit, 0, wxALL, 5 );


	bSizer41->Add( bSizer44, 0, wxEXPAND, 5 );

	m_smoothBox = new wxCheckBox( this, wxID_ANY, wxT("Smooth"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer41->Add( m_smoothBox, 0, wxALL, 5 );

	wxBoxSizer* bSizer45;
	bSizer45 = new wxBoxSizer( wxHORIZONTAL );

	m_okBtn = new wxButton( this, wxID_ANY, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer45->Add( m_okBtn, 0, wxALL, 5 );


	bSizer45->Add( 0, 0, 1, wxEXPAND, 5 );

	m_cancelBtn = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer45->Add( m_cancelBtn, 0, wxALL, 5 );


	bSizer41->Add( bSizer45, 0, wxEXPAND, 5 );


	this->SetSizer( bSizer41 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_okBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConeDlg::onOkBtn ), NULL, this );
	m_cancelBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConeDlg::onCancelBtn ), NULL, this );
}

ConeDlg::~ConeDlg()
{
	// Disconnect Events
	m_okBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConeDlg::onOkBtn ), NULL, this );
	m_cancelBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ConeDlg::onCancelBtn ), NULL, this );

}

SphereDlg::SphereDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer51;
	bSizer51 = new wxBoxSizer( wxVERTICAL );

	m_bitmap5 = new wxStaticBitmap( this, wxID_ANY, wxBitmap( wxT("Editor/sphere.bmp"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize( 100,100 ), wxBORDER_SUNKEN );
	bSizer51->Add( m_bitmap5, 0, wxALIGN_CENTER|wxALL, 5 );

	wxBoxSizer* bSizer52;
	bSizer52 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("Radius:"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_staticText1->Wrap( -1 );
	bSizer52->Add( m_staticText1, 0, wxALL, 5 );

	m_radiusEdit = new wxTextCtrl( this, wxID_ANY, wxT("2"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizer52->Add( m_radiusEdit, 0, wxALL, 5 );


	bSizer51->Add( bSizer52, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer54;
	bSizer54 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("Sides:"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_staticText2->Wrap( -1 );
	bSizer54->Add( m_staticText2, 0, wxALL, 5 );

	m_sidesEdit = new wxTextCtrl( this, wxID_ANY, wxT("8"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizer54->Add( m_sidesEdit, 0, wxALL, 5 );


	bSizer51->Add( bSizer54, 0, wxEXPAND, 5 );

	m_smoothBox = new wxCheckBox( this, wxID_ANY, wxT("Smooth"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer51->Add( m_smoothBox, 0, wxALL, 5 );

	m_halfBox = new wxCheckBox( this, wxID_ANY, wxT("Hemisphere"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer51->Add( m_halfBox, 0, wxALL, 5 );

	wxBoxSizer* bSizer53;
	bSizer53 = new wxBoxSizer( wxHORIZONTAL );

	m_okBtn = new wxButton( this, wxID_ANY, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer53->Add( m_okBtn, 0, wxALL, 5 );


	bSizer53->Add( 0, 0, 1, wxEXPAND, 5 );

	m_cancelBtn = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer53->Add( m_cancelBtn, 0, wxALL, 5 );


	bSizer51->Add( bSizer53, 0, wxEXPAND, 5 );


	this->SetSizer( bSizer51 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_okBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SphereDlg::onOkBtn ), NULL, this );
	m_cancelBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SphereDlg::onCancelBtn ), NULL, this );
}

SphereDlg::~SphereDlg()
{
	// Disconnect Events
	m_okBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SphereDlg::onOkBtn ), NULL, this );
	m_cancelBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SphereDlg::onCancelBtn ), NULL, this );

}

Console::Console( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	this->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_previewPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SUNKEN|wxTAB_TRAVERSAL );
	m_previewPanel->SetMinSize( wxSize( -1,270 ) );
	m_previewPanel->SetMaxSize( wxSize( -1,270 ) );

	bSizer1->Add( m_previewPanel, 1, wxALL|wxEXPAND, 5 );

	m_tabPanel = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxNB_LEFT );
	m_tabPanel->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	m_tabPanel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	m_materialPanel = new wxPanel( m_tabPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_materialPanel->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );
	m_materialPanel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	m_materialTree = new wxTreeCtrl( m_materialPanel, wxID_ANY, wxDefaultPosition, wxSize( -1,300 ), wxTR_DEFAULT_STYLE );
	m_materialTree->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOWTEXT ) );

	bSizer2->Add( m_materialTree, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	m_matRefreshBtn = new wxButton( m_materialPanel, wxID_ANY, wxT("Refresh"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_matRefreshBtn, 0, wxALL, 5 );


	bSizer4->Add( 0, 0, 1, wxEXPAND, 5 );

	m_matApplyBtn = new wxButton( m_materialPanel, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_matApplyBtn, 0, wxALL, 5 );


	bSizer2->Add( bSizer4, 1, wxEXPAND, 5 );


	m_materialPanel->SetSizer( bSizer2 );
	m_materialPanel->Layout();
	bSizer2->Fit( m_materialPanel );
	m_tabPanel->AddPage( m_materialPanel, wxT("Materials"), true );
	m_meshPanel = new wxPanel( m_tabPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_meshPanel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxVERTICAL );

	m_meshTree = new wxTreeCtrl( m_meshPanel, wxID_ANY, wxDefaultPosition, wxSize( -1,300 ), wxTR_DEFAULT_STYLE );
	bSizer19->Add( m_meshTree, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxHORIZONTAL );

	m_meshRefreshBtn = new wxButton( m_meshPanel, wxID_ANY, wxT("Refresh"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_meshRefreshBtn, 0, wxALL, 5 );


	bSizer20->Add( 0, 0, 1, wxEXPAND, 5 );

	m_meshCreateBtn = new wxButton( m_meshPanel, wxID_ANY, wxT("Create"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_meshCreateBtn, 0, wxALL, 5 );


	bSizer19->Add( bSizer20, 1, wxEXPAND, 5 );


	m_meshPanel->SetSizer( bSizer19 );
	m_meshPanel->Layout();
	bSizer19->Fit( m_meshPanel );
	m_tabPanel->AddPage( m_meshPanel, wxT("Meshes"), false );
	m_lightPanel = new wxPanel( m_tabPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_lightPanel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer114;
	bSizer114 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer117;
	bSizer117 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer113;
	bSizer113 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText56 = new wxStaticText( m_lightPanel, wxID_ANY, wxT("Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText56->Wrap( -1 );
	m_staticText56->SetMinSize( wxSize( 45,-1 ) );

	bSizer113->Add( m_staticText56, 0, wxALL, 5 );

	wxString m_lightTypeBoxChoices[] = { wxT("Omni"), wxT("Spot") };
	int m_lightTypeBoxNChoices = sizeof( m_lightTypeBoxChoices ) / sizeof( wxString );
	m_lightTypeBox = new wxChoice( m_lightPanel, wxID_ANY, wxDefaultPosition, wxSize( 100,-1 ), m_lightTypeBoxNChoices, m_lightTypeBoxChoices, 0 );
	m_lightTypeBox->SetSelection( 0 );
	bSizer113->Add( m_lightTypeBox, 0, wxALL, 5 );


	bSizer117->Add( bSizer113, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer118;
	bSizer118 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText57 = new wxStaticText( m_lightPanel, wxID_ANY, wxT("Radius:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText57->Wrap( -1 );
	m_staticText57->SetMinSize( wxSize( 45,-1 ) );

	bSizer118->Add( m_staticText57, 0, wxALL, 5 );

	m_lightRadiusEdt = new wxTextCtrl( m_lightPanel, wxID_ANY, wxT("5"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizer118->Add( m_lightRadiusEdt, 0, wxALL, 5 );


	bSizer117->Add( bSizer118, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer119;
	bSizer119 = new wxBoxSizer( wxHORIZONTAL );

	m_lightAngleCaption = new wxStaticText( m_lightPanel, wxID_ANY, wxT("Angles:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lightAngleCaption->Wrap( -1 );
	m_lightAngleCaption->Hide();
	m_lightAngleCaption->SetMinSize( wxSize( 45,-1 ) );

	bSizer119->Add( m_lightAngleCaption, 0, wxALL, 5 );

	m_lightAngleEdt = new wxTextCtrl( m_lightPanel, wxID_ANY, wxT("60"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_lightAngleEdt->Hide();

	bSizer119->Add( m_lightAngleEdt, 0, wxALL, 5 );

	m_lightAngleInnerEdt = new wxTextCtrl( m_lightPanel, wxID_ANY, wxT("50"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_lightAngleInnerEdt->Hide();

	bSizer119->Add( m_lightAngleInnerEdt, 0, wxALL, 5 );


	bSizer117->Add( bSizer119, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer23;
	bSizer23 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText11 = new wxStaticText( m_lightPanel, wxID_ANY, wxT("Falloff:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	m_staticText11->SetMinSize( wxSize( 45,-1 ) );

	bSizer23->Add( m_staticText11, 0, wxALL, 5 );

	m_lightFalloffEdt = new wxTextCtrl( m_lightPanel, wxID_ANY, wxT("3"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizer23->Add( m_lightFalloffEdt, 0, wxALL, 5 );


	bSizer117->Add( bSizer23, 0, 0, 5 );

	wxBoxSizer* bSizer26;
	bSizer26 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText12 = new wxStaticText( m_lightPanel, wxID_ANY, wxT("Power:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	m_staticText12->SetMinSize( wxSize( 45,-1 ) );

	bSizer26->Add( m_staticText12, 0, wxALL, 5 );

	m_lightPowerEdt = new wxTextCtrl( m_lightPanel, wxID_ANY, wxT("1"), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	bSizer26->Add( m_lightPowerEdt, 0, wxALL, 5 );


	bSizer117->Add( bSizer26, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer24;
	bSizer24 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText55 = new wxStaticText( m_lightPanel, wxID_ANY, wxT("Shadow:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText55->Wrap( -1 );
	m_staticText55->SetMinSize( wxSize( 45,-1 ) );

	bSizer24->Add( m_staticText55, 0, wxALL, 5 );

	wxString m_lightShadowBoxChoices[] = { wxT("None"), wxT("Static"), wxT("Dynamic") };
	int m_lightShadowBoxNChoices = sizeof( m_lightShadowBoxChoices ) / sizeof( wxString );
	m_lightShadowBox = new wxChoice( m_lightPanel, wxID_ANY, wxDefaultPosition, wxSize( 100,-1 ), m_lightShadowBoxNChoices, m_lightShadowBoxChoices, 0 );
	m_lightShadowBox->SetSelection( 0 );
	bSizer24->Add( m_lightShadowBox, 0, wxALL, 5 );


	bSizer117->Add( bSizer24, 0, wxEXPAND, 5 );


	bSizer114->Add( bSizer117, 1, wxEXPAND, 5 );

	m_lightColorPanel = new wxPanel( m_lightPanel, wxID_ANY, wxDefaultPosition, wxSize( 50,50 ), wxBORDER_RAISED|wxTAB_TRAVERSAL );
	m_lightColorPanel->SetForegroundColour( wxColour( 255, 255, 255 ) );
	m_lightColorPanel->SetBackgroundColour( wxColour( 255, 255, 255 ) );

	bSizer114->Add( m_lightColorPanel, 0, wxALIGN_BOTTOM|wxALL, 5 );


	bSizer21->Add( bSizer114, 0, wxEXPAND, 5 );

	m_createLightBtn = new wxButton( m_lightPanel, wxID_ANY, wxT("Create"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer21->Add( m_createLightBtn, 0, wxALIGN_CENTER|wxALL, 5 );


	m_lightPanel->SetSizer( bSizer21 );
	m_lightPanel->Layout();
	bSizer21->Fit( m_lightPanel );
	m_tabPanel->AddPage( m_lightPanel, wxT("Lights"), false );
	m_entityPanel = new wxPanel( m_tabPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	m_entityPanel->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_WINDOW ) );

	wxBoxSizer* bSizer28;
	bSizer28 = new wxBoxSizer( wxVERTICAL );

	m_entityTree = new wxTreeCtrl( m_entityPanel, wxID_ANY, wxDefaultPosition, wxSize( -1,300 ), wxTR_DEFAULT_STYLE );
	bSizer28->Add( m_entityTree, 0, wxALL|wxEXPAND, 5 );

	m_createEntityBtn = new wxButton( m_entityPanel, wxID_ANY, wxT("Create"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer28->Add( m_createEntityBtn, 0, wxALIGN_CENTER|wxALL, 5 );


	m_entityPanel->SetSizer( bSizer28 );
	m_entityPanel->Layout();
	bSizer28->Fit( m_entityPanel );
	m_tabPanel->AddPage( m_entityPanel, wxT("Entities"), false );
	m_surfacePanel = new wxPanel( m_tabPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer56;
	bSizer56 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer57;
	bSizer57 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText27 = new wxStaticText( m_surfacePanel, wxID_ANY, wxT("Radius:"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_staticText27->Wrap( -1 );
	bSizer57->Add( m_staticText27, 0, wxALIGN_CENTER|wxALL, 5 );

	m_dispRadiusEdit = new wxSpinCtrlDouble( m_surfacePanel, wxID_ANY, wxT("0.5"), wxDefaultPosition, wxSize( 65,-1 ), wxSP_ARROW_KEYS, 0, 30, 0.500000, 0.1 );
	m_dispRadiusEdit->SetDigits( 1 );
	bSizer57->Add( m_dispRadiusEdit, 0, wxALL, 5 );


	bSizer56->Add( bSizer57, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer58;
	bSizer58 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText28 = new wxStaticText( m_surfacePanel, wxID_ANY, wxT("Power:"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_staticText28->Wrap( -1 );
	bSizer58->Add( m_staticText28, 0, wxALIGN_CENTER|wxALL, 5 );

	m_dispPowerEdit = new wxSpinCtrlDouble( m_surfacePanel, wxID_ANY, wxT("1"), wxDefaultPosition, wxSize( 65,-1 ), wxSP_ARROW_KEYS, 0, 100, 1, 1 );
	m_dispPowerEdit->SetDigits( 0 );
	bSizer58->Add( m_dispPowerEdit, 0, wxALL, 5 );


	bSizer56->Add( bSizer58, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer99;
	bSizer99 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText48 = new wxStaticText( m_surfacePanel, wxID_ANY, wxT("Layer:"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_staticText48->Wrap( -1 );
	bSizer99->Add( m_staticText48, 0, wxALIGN_CENTER|wxALL, 5 );

	m_layerCtrl = new wxSpinCtrl( m_surfacePanel, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 10, 1 );
	m_layerCtrl->Enable( false );

	bSizer99->Add( m_layerCtrl, 0, wxALIGN_CENTER|wxALL, 5 );


	bSizer56->Add( bSizer99, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer59;
	bSizer59 = new wxBoxSizer( wxHORIZONTAL );

	m_dispUpBtn = new wxBitmapToggleButton( m_surfacePanel, wxID_ANY, wxBitmap( wxT("Editor/surf_up.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize( 55,55 ), 0 );
	m_dispUpBtn->SetValue( true );

	m_dispUpBtn->SetBitmap( wxBitmap( wxT("Editor/surf_up.png"), wxBITMAP_TYPE_ANY ) );
	m_dispUpBtn->SetToolTip( wxT("Up") );

	bSizer59->Add( m_dispUpBtn, 0, wxALL, 5 );

	m_dispDownBtn = new wxBitmapToggleButton( m_surfacePanel, wxID_ANY, wxBitmap( wxT("Editor/surf_dn.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize( 55,55 ), 0 );

	m_dispDownBtn->SetBitmap( wxBitmap( wxT("Editor/surf_dn.png"), wxBITMAP_TYPE_ANY ) );
	m_dispDownBtn->SetToolTip( wxT("Down") );

	bSizer59->Add( m_dispDownBtn, 0, wxALL, 5 );

	m_dispSmoothBtn = new wxBitmapToggleButton( m_surfacePanel, wxID_ANY, wxBitmap( wxT("Editor/surf_smooth.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize( 55,55 ), 0 );

	m_dispSmoothBtn->SetBitmap( wxBitmap( wxT("Editor/surf_smooth.png"), wxBITMAP_TYPE_ANY ) );
	m_dispSmoothBtn->SetToolTip( wxT("Smooth") );

	bSizer59->Add( m_dispSmoothBtn, 0, wxALL, 5 );

	m_dispPaintBtn = new wxBitmapToggleButton( m_surfacePanel, wxID_ANY, wxBitmap( wxT("Editor/surf_paint.png"), wxBITMAP_TYPE_ANY ), wxDefaultPosition, wxSize( 55,55 ), 0 );

	m_dispPaintBtn->SetBitmap( wxBitmap( wxT("Editor/surf_paint.png"), wxBITMAP_TYPE_ANY ) );
	m_dispPaintBtn->SetToolTip( wxT("Paint") );

	bSizer59->Add( m_dispPaintBtn, 0, wxALL, 5 );


	bSizer56->Add( bSizer59, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer103;
	bSizer103 = new wxBoxSizer( wxVERTICAL );

	m_staticText50 = new wxStaticText( m_surfacePanel, wxID_ANY, wxT("Details:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText50->Wrap( -1 );
	bSizer103->Add( m_staticText50, 0, wxALL, 5 );

	wxBoxSizer* bSizer104;
	bSizer104 = new wxBoxSizer( wxHORIZONTAL );

	m_addDetailBtn = new wxButton( m_surfacePanel, wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer104->Add( m_addDetailBtn, 0, wxALL, 5 );

	m_editDetailBtn = new wxButton( m_surfacePanel, wxID_ANY, wxT("Edit"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer104->Add( m_editDetailBtn, 0, wxALL, 5 );

	m_clearDetailBtn = new wxButton( m_surfacePanel, wxID_ANY, wxT("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer104->Add( m_clearDetailBtn, 0, wxALL, 5 );


	bSizer103->Add( bSizer104, 1, wxEXPAND, 5 );


	bSizer56->Add( bSizer103, 1, wxEXPAND, 5 );


	m_surfacePanel->SetSizer( bSizer56 );
	m_surfacePanel->Layout();
	bSizer56->Fit( m_surfacePanel );
	m_tabPanel->AddPage( m_surfacePanel, wxT("Surface Edit"), false );

	bSizer1->Add( m_tabPanel, 1, wxALL|wxEXPAND, 0 );


	this->SetSizer( bSizer1 );
	this->Layout();

	// Connect Events
	m_materialTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( Console::onMatTreeSelected ), NULL, this );
	m_matRefreshBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onRefreshMaterials ), NULL, this );
	m_matApplyBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onApplyMaterial ), NULL, this );
	m_meshTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( Console::onMeshTreeSelected ), NULL, this );
	m_meshRefreshBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onRefreshMesh ), NULL, this );
	m_meshCreateBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onCreateMesh ), NULL, this );
	m_lightTypeBox->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( Console::onChangeLightType ), NULL, this );
	m_lightColorPanel->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( Console::onSelectLightColor ), NULL, this );
	m_createLightBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onCreateLight ), NULL, this );
	m_entityTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( Console::onEntityTreeSelected ), NULL, this );
	m_createEntityBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onCreateObject ), NULL, this );
	m_dispRadiusEdit->Connect( wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler( Console::onDispRadiusChange ), NULL, this );
	m_dispRadiusEdit->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Console::onDispRadiusChange ), NULL, this );
	m_dispPowerEdit->Connect( wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler( Console::onDispPowerChange ), NULL, this );
	m_dispPowerEdit->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Console::onDispPowerChange ), NULL, this );
	m_layerCtrl->Connect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( Console::onLayerChange ), NULL, this );
	m_layerCtrl->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Console::onLayerChange ), NULL, this );
	m_dispUpBtn->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Console::onDispUpBtn ), NULL, this );
	m_dispDownBtn->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Console::onDispDownBtn ), NULL, this );
	m_dispSmoothBtn->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Console::onDispSmoothBtn ), NULL, this );
	m_dispPaintBtn->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Console::onDispPaintBtn ), NULL, this );
	m_addDetailBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onAddDetailBtn ), NULL, this );
	m_editDetailBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onEditDetailBtn ), NULL, this );
	m_clearDetailBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onClearDetailBtn ), NULL, this );
}

Console::~Console()
{
	// Disconnect Events
	m_materialTree->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( Console::onMatTreeSelected ), NULL, this );
	m_matRefreshBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onRefreshMaterials ), NULL, this );
	m_matApplyBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onApplyMaterial ), NULL, this );
	m_meshTree->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( Console::onMeshTreeSelected ), NULL, this );
	m_meshRefreshBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onRefreshMesh ), NULL, this );
	m_meshCreateBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onCreateMesh ), NULL, this );
	m_lightTypeBox->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( Console::onChangeLightType ), NULL, this );
	m_lightColorPanel->Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( Console::onSelectLightColor ), NULL, this );
	m_createLightBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onCreateLight ), NULL, this );
	m_entityTree->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( Console::onEntityTreeSelected ), NULL, this );
	m_createEntityBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onCreateObject ), NULL, this );
	m_dispRadiusEdit->Disconnect( wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler( Console::onDispRadiusChange ), NULL, this );
	m_dispRadiusEdit->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Console::onDispRadiusChange ), NULL, this );
	m_dispPowerEdit->Disconnect( wxEVT_COMMAND_SPINCTRLDOUBLE_UPDATED, wxSpinDoubleEventHandler( Console::onDispPowerChange ), NULL, this );
	m_dispPowerEdit->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Console::onDispPowerChange ), NULL, this );
	m_layerCtrl->Disconnect( wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler( Console::onLayerChange ), NULL, this );
	m_layerCtrl->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Console::onLayerChange ), NULL, this );
	m_dispUpBtn->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Console::onDispUpBtn ), NULL, this );
	m_dispDownBtn->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Console::onDispDownBtn ), NULL, this );
	m_dispSmoothBtn->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Console::onDispSmoothBtn ), NULL, this );
	m_dispPaintBtn->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Console::onDispPaintBtn ), NULL, this );
	m_addDetailBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onAddDetailBtn ), NULL, this );
	m_editDetailBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onEditDetailBtn ), NULL, this );
	m_clearDetailBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Console::onClearDetailBtn ), NULL, this );

}

DetailDlg::DetailDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer105;
	bSizer105 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer110;
	bSizer110 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer106;
	bSizer106 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText51 = new wxStaticText( this, wxID_ANY, wxT("Layer:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText51->Wrap( -1 );
	bSizer106->Add( m_staticText51, 0, wxALL, 5 );

	m_layerCtrl = new wxSpinCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, 32, 0 );
	bSizer106->Add( m_layerCtrl, 0, wxALL, 5 );


	bSizer110->Add( bSizer106, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer108;
	bSizer108 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText53 = new wxStaticText( this, wxID_ANY, wxT("Model:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText53->Wrap( -1 );
	bSizer108->Add( m_staticText53, 0, wxALL, 5 );

	m_modelCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer108->Add( m_modelCtrl, 0, wxALL, 5 );


	bSizer110->Add( bSizer108, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer107;
	bSizer107 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText52 = new wxStaticText( this, wxID_ANY, wxT("Material:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText52->Wrap( -1 );
	bSizer107->Add( m_staticText52, 0, wxALL, 5 );

	m_materialCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer107->Add( m_materialCtrl, 0, wxALL, 5 );


	bSizer110->Add( bSizer107, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer109;
	bSizer109 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText54 = new wxStaticText( this, wxID_ANY, wxT("Density:"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	m_staticText54->Wrap( -1 );
	bSizer109->Add( m_staticText54, 0, wxALL, 5 );

	m_densityCtrl = new wxSpinCtrlDouble( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 100, 5, 1 );
	m_densityCtrl->SetDigits( 0 );
	bSizer109->Add( m_densityCtrl, 0, wxALL, 5 );


	bSizer110->Add( bSizer109, 0, wxEXPAND, 5 );

	m_addBtn = new wxButton( this, wxID_ANY, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer110->Add( m_addBtn, 0, wxALL, 5 );


	bSizer105->Add( bSizer110, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxVERTICAL );

	m_treeCtrl5 = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	bSizer111->Add( m_treeCtrl5, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer112;
	bSizer112 = new wxBoxSizer( wxHORIZONTAL );

	m_addDetailBtn = new wxButton( this, wxID_ANY, wxT("+"), wxDefaultPosition, wxSize( 15,15 ), 0 );
	bSizer112->Add( m_addDetailBtn, 0, wxALL, 5 );

	m_removeDetailBtn = new wxButton( this, wxID_ANY, wxT("-"), wxDefaultPosition, wxSize( 15,15 ), 0 );
	bSizer112->Add( m_removeDetailBtn, 0, wxALL, 5 );


	bSizer111->Add( bSizer112, 0, wxEXPAND, 5 );


	bSizer105->Add( bSizer111, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer105 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_addBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DetailDlg::onAddBtn ), NULL, this );
	m_addDetailBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DetailDlg::onAddDetailBtn ), NULL, this );
	m_removeDetailBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DetailDlg::onRemoveDetailDlg ), NULL, this );
}

DetailDlg::~DetailDlg()
{
	// Disconnect Events
	m_addBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DetailDlg::onAddBtn ), NULL, this );
	m_addDetailBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DetailDlg::onAddDetailBtn ), NULL, this );
	m_removeDetailBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DetailDlg::onRemoveDetailDlg ), NULL, this );

}

PolyDlg::PolyDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );

	m_portalBox = new wxCheckBox( this, wxID_ANY, wxT("Portal"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_portalBox, 0, wxALL, 5 );

	m_skyBox = new wxCheckBox( this, wxID_ANY, wxT("Sky"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_skyBox, 0, wxALL, 5 );

	m_invisibleBox = new wxCheckBox( this, wxID_ANY, wxT("Invisible"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_invisibleBox, 0, wxALL, 5 );

	m_transparentBox = new wxCheckBox( this, wxID_ANY, wxT("Transparent"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_transparentBox, 0, wxALL, 5 );

	m_nocollisionBox = new wxCheckBox( this, wxID_ANY, wxT("No collision"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_nocollisionBox, 0, wxALL, 5 );

	m_twosideBox = new wxCheckBox( this, wxID_ANY, wxT("Two side"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_twosideBox, 0, wxALL, 5 );

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText6 = new wxStaticText( this, wxID_ANY, wxT("Smooth groop:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText6->Wrap( -1 );
	bSizer11->Add( m_staticText6, 0, wxALL, 5 );

	m_smgroopEdit = new wxTextCtrl( this, wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER );
	m_smgroopEdit->SetMaxSize( wxSize( 30,-1 ) );

	bSizer11->Add( m_smgroopEdit, 0, wxALL, 5 );


	bSizer5->Add( bSizer11, 1, wxEXPAND, 5 );


	bSizer4->Add( bSizer5, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );

	wxGridSizer* gSizer1;
	gSizer1 = new wxGridSizer( 3, 2, 0, 0 );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("U move"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer6->Add( m_staticText1, 0, wxALL, 5 );

	m_umoveBtn = new wxSpinButton( this, wxID_ANY, wxDefaultPosition, wxSize( -1,23 ), wxSP_WRAP );
	bSizer6->Add( m_umoveBtn, 0, wxALL, 5 );

	m_umoveEdit = new wxTextCtrl( this, wxID_ANY, wxT("0.1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_umoveEdit->SetMaxSize( wxSize( 50,-1 ) );

	bSizer6->Add( m_umoveEdit, 0, wxALL, 5 );


	gSizer1->Add( bSizer6, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("U scale"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer7->Add( m_staticText2, 0, wxALL, 5 );

	m_uscaleBtn = new wxSpinButton( this, wxID_ANY, wxDefaultPosition, wxSize( -1,23 ), wxSP_WRAP );
	bSizer7->Add( m_uscaleBtn, 0, wxALL, 5 );

	m_uscaleEdit = new wxTextCtrl( this, wxID_ANY, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_uscaleEdit->SetMaxSize( wxSize( 50,-1 ) );

	bSizer7->Add( m_uscaleEdit, 0, wxALL, 5 );


	gSizer1->Add( bSizer7, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText3 = new wxStaticText( this, wxID_ANY, wxT("V move"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer8->Add( m_staticText3, 0, wxALL, 5 );

	m_vmoveBtn = new wxSpinButton( this, wxID_ANY, wxDefaultPosition, wxSize( -1,23 ), wxSP_WRAP );
	bSizer8->Add( m_vmoveBtn, 0, wxALL, 5 );

	m_vmoveEdit = new wxTextCtrl( this, wxID_ANY, wxT("0.1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_vmoveEdit->SetMaxSize( wxSize( 50,-1 ) );

	bSizer8->Add( m_vmoveEdit, 0, wxALL, 5 );


	gSizer1->Add( bSizer8, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText4 = new wxStaticText( this, wxID_ANY, wxT("V scale"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer9->Add( m_staticText4, 0, wxALL, 5 );

	m_vscaleBtn = new wxSpinButton( this, wxID_ANY, wxDefaultPosition, wxSize( -1,23 ), wxSP_WRAP );
	bSizer9->Add( m_vscaleBtn, 0, wxALL, 5 );

	m_vscaleEdit = new wxTextCtrl( this, wxID_ANY, wxT("2"), wxDefaultPosition, wxDefaultSize, 0 );
	m_vscaleEdit->SetMaxSize( wxSize( 50,-1 ) );

	bSizer9->Add( m_vscaleEdit, 0, wxALL, 5 );


	gSizer1->Add( bSizer9, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer10;
	bSizer10 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText5 = new wxStaticText( this, wxID_ANY, wxT("Rotate  "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer10->Add( m_staticText5, 0, wxALL, 5 );

	m_rotateBtn = new wxSpinButton( this, wxID_ANY, wxDefaultPosition, wxSize( -1,23 ), wxSP_WRAP );
	bSizer10->Add( m_rotateBtn, 0, wxALL, 5 );

	m_rotateEdit = new wxTextCtrl( this, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_rotateEdit->SetMaxSize( wxSize( 50,-1 ) );

	bSizer10->Add( m_rotateEdit, 0, wxALL, 5 );


	gSizer1->Add( bSizer10, 1, wxEXPAND, 5 );


	bSizer12->Add( gSizer1, 1, 0, 5 );

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer101;
	bSizer101 = new wxBoxSizer( wxVERTICAL );

	m_layersCaption = new wxStaticText( this, wxID_ANY, wxT("Layers:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_layersCaption->Wrap( -1 );
	bSizer101->Add( m_layersCaption, 0, wxALL, 5 );

	m_layerList = new wxListBox( this, wxID_ANY, wxDefaultPosition, wxSize( 140,40 ), 0, NULL, 0 );
	bSizer101->Add( m_layerList, 0, wxALL, 5 );

	wxBoxSizer* bSizer102;
	bSizer102 = new wxBoxSizer( wxHORIZONTAL );

	m_addLayerBtn = new wxButton( this, wxID_ANY, wxT("+"), wxDefaultPosition, wxSize( 20,20 ), 0 );
	bSizer102->Add( m_addLayerBtn, 0, wxALL, 5 );

	m_delLayerBtn = new wxButton( this, wxID_ANY, wxT("-"), wxDefaultPosition, wxSize( 20,20 ), 0 );
	bSizer102->Add( m_delLayerBtn, 0, wxALL, 5 );

	m_editLayerBtn = new wxButton( this, wxID_ANY, wxT("edit"), wxDefaultPosition, wxSize( 40,20 ), 0 );
	bSizer102->Add( m_editLayerBtn, 0, wxALL, 5 );


	bSizer101->Add( bSizer102, 1, wxEXPAND, 5 );


	bSizer13->Add( bSizer101, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer100;
	bSizer100 = new wxBoxSizer( wxHORIZONTAL );

	m_subdivideBtn = new wxButton( this, wxID_ANY, wxT("Subdivide"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer100->Add( m_subdivideBtn, 0, wxALIGN_BOTTOM|wxALL, 5 );

	m_resEdit = new wxTextCtrl( this, wxID_ANY, wxT("20"), wxDefaultPosition, wxDefaultSize, 0 );
	m_resEdit->SetMaxSize( wxSize( 50,-1 ) );

	bSizer100->Add( m_resEdit, 0, wxALIGN_BOTTOM|wxALL, 5 );


	bSizer13->Add( bSizer100, 1, wxALIGN_BOTTOM, 5 );


	bSizer12->Add( bSizer13, 1, wxEXPAND, 5 );


	bSizer4->Add( bSizer12, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer4 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_portalBox->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PolyDlg::onPortalBox ), NULL, this );
	m_skyBox->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PolyDlg::onSkyBox ), NULL, this );
	m_invisibleBox->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PolyDlg::onInvisibleBox ), NULL, this );
	m_transparentBox->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PolyDlg::onTransparentBox ), NULL, this );
	m_nocollisionBox->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PolyDlg::onNocollisionBox ), NULL, this );
	m_twosideBox->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PolyDlg::onTwosideBox ), NULL, this );
	m_smgroopEdit->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( PolyDlg::onSmoothGroopEnter ), NULL, this );
	m_umoveBtn->Connect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( PolyDlg::onUMoveDown ), NULL, this );
	m_umoveBtn->Connect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( PolyDlg::onUMoveUp ), NULL, this );
	m_umoveEdit->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PolyDlg::onUMoveRateChanged ), NULL, this );
	m_uscaleBtn->Connect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( PolyDlg::onUScaleDown ), NULL, this );
	m_uscaleBtn->Connect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( PolyDlg::onUScaleUp ), NULL, this );
	m_uscaleEdit->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PolyDlg::onUScaleRateChanged ), NULL, this );
	m_vmoveBtn->Connect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( PolyDlg::onVMoveDown ), NULL, this );
	m_vmoveBtn->Connect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( PolyDlg::onVMoveUp ), NULL, this );
	m_vmoveEdit->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PolyDlg::onVMoveRateChanged ), NULL, this );
	m_vscaleBtn->Connect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( PolyDlg::onVScaleDown ), NULL, this );
	m_vscaleBtn->Connect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( PolyDlg::onVScaleUp ), NULL, this );
	m_vscaleEdit->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PolyDlg::onVScaleRateChanged ), NULL, this );
	m_rotateBtn->Connect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( PolyDlg::onRotateDown ), NULL, this );
	m_rotateBtn->Connect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( PolyDlg::onRotateUp ), NULL, this );
	m_rotateEdit->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PolyDlg::onRotateRateChanged ), NULL, this );
	m_addLayerBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PolyDlg::onAddLayer ), NULL, this );
	m_delLayerBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PolyDlg::onDeleteLayer ), NULL, this );
	m_editLayerBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PolyDlg::onEditLayer ), NULL, this );
	m_subdivideBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PolyDlg::onSubdivide ), NULL, this );
}

PolyDlg::~PolyDlg()
{
	// Disconnect Events
	m_portalBox->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PolyDlg::onPortalBox ), NULL, this );
	m_skyBox->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PolyDlg::onSkyBox ), NULL, this );
	m_invisibleBox->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PolyDlg::onInvisibleBox ), NULL, this );
	m_transparentBox->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PolyDlg::onTransparentBox ), NULL, this );
	m_nocollisionBox->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PolyDlg::onNocollisionBox ), NULL, this );
	m_twosideBox->Disconnect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( PolyDlg::onTwosideBox ), NULL, this );
	m_smgroopEdit->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( PolyDlg::onSmoothGroopEnter ), NULL, this );
	m_umoveBtn->Disconnect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( PolyDlg::onUMoveDown ), NULL, this );
	m_umoveBtn->Disconnect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( PolyDlg::onUMoveUp ), NULL, this );
	m_umoveEdit->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PolyDlg::onUMoveRateChanged ), NULL, this );
	m_uscaleBtn->Disconnect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( PolyDlg::onUScaleDown ), NULL, this );
	m_uscaleBtn->Disconnect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( PolyDlg::onUScaleUp ), NULL, this );
	m_uscaleEdit->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PolyDlg::onUScaleRateChanged ), NULL, this );
	m_vmoveBtn->Disconnect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( PolyDlg::onVMoveDown ), NULL, this );
	m_vmoveBtn->Disconnect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( PolyDlg::onVMoveUp ), NULL, this );
	m_vmoveEdit->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PolyDlg::onVMoveRateChanged ), NULL, this );
	m_vscaleBtn->Disconnect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( PolyDlg::onVScaleDown ), NULL, this );
	m_vscaleBtn->Disconnect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( PolyDlg::onVScaleUp ), NULL, this );
	m_vscaleEdit->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PolyDlg::onVScaleRateChanged ), NULL, this );
	m_rotateBtn->Disconnect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( PolyDlg::onRotateDown ), NULL, this );
	m_rotateBtn->Disconnect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( PolyDlg::onRotateUp ), NULL, this );
	m_rotateEdit->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( PolyDlg::onRotateRateChanged ), NULL, this );
	m_addLayerBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PolyDlg::onAddLayer ), NULL, this );
	m_delLayerBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PolyDlg::onDeleteLayer ), NULL, this );
	m_editLayerBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PolyDlg::onEditLayer ), NULL, this );
	m_subdivideBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PolyDlg::onSubdivide ), NULL, this );

}

MapInfoDlg::MapInfoDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer55;
	bSizer55 = new wxBoxSizer( wxVERTICAL );

	m_propertyGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE);
	bSizer55->Add( m_propertyGrid, 1, wxALL|wxEXPAND, 5 );


	this->SetSizer( bSizer55 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_SHOW, wxShowEventHandler( MapInfoDlg::onShow ) );
	m_propertyGrid->Connect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( MapInfoDlg::onChanged ), NULL, this );
}

MapInfoDlg::~MapInfoDlg()
{
	// Disconnect Events
	this->Disconnect( wxEVT_SHOW, wxShowEventHandler( MapInfoDlg::onShow ) );
	m_propertyGrid->Disconnect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( MapInfoDlg::onChanged ), NULL, this );

}

PreferencesDlg::PreferencesDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText7 = new wxStaticText( this, wxID_ANY, wxT("Move step:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	m_staticText7->SetMinSize( wxSize( 65,-1 ) );

	bSizer15->Add( m_staticText7, 0, wxALL, 5 );

	m_moveEdt = new wxTextCtrl( this, wxID_ANY, wxT("0.1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_moveEdt->SetMaxSize( wxSize( 60,-1 ) );

	bSizer15->Add( m_moveEdt, 0, wxALL, 5 );

	m_moveBox = new wxCheckBox( this, wxID_ANY, wxT("discrete"), wxDefaultPosition, wxDefaultSize, 0 );
	m_moveBox->SetValue(true);
	bSizer15->Add( m_moveBox, 0, wxALL, 5 );


	bSizer14->Add( bSizer15, 1, 0, 5 );

	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText8 = new wxStaticText( this, wxID_ANY, wxT("Rotate step:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	m_staticText8->SetMinSize( wxSize( 65,-1 ) );

	bSizer16->Add( m_staticText8, 0, wxALL, 5 );

	m_rotateEdt = new wxTextCtrl( this, wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, 0 );
	m_rotateEdt->SetMaxSize( wxSize( 60,-1 ) );

	bSizer16->Add( m_rotateEdt, 0, wxALL, 5 );

	m_rotateBox = new wxCheckBox( this, wxID_ANY, wxT("discrete"), wxDefaultPosition, wxDefaultSize, 0 );
	m_rotateBox->SetValue(true);
	bSizer16->Add( m_rotateBox, 0, wxALL, 5 );


	bSizer14->Add( bSizer16, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText9 = new wxStaticText( this, wxID_ANY, wxT("Scale step:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	m_staticText9->SetMinSize( wxSize( 65,-1 ) );

	bSizer17->Add( m_staticText9, 0, wxALL, 5 );

	m_scaleEdt = new wxTextCtrl( this, wxID_ANY, wxT("0.1"), wxDefaultPosition, wxDefaultSize, 0 );
	m_scaleEdt->SetMaxSize( wxSize( 60,-1 ) );

	bSizer17->Add( m_scaleEdt, 0, wxALL, 5 );

	m_scaleBox = new wxCheckBox( this, wxID_ANY, wxT("discrete"), wxDefaultPosition, wxDefaultSize, 0 );
	m_scaleBox->SetValue(true);
	bSizer17->Add( m_scaleBox, 0, wxALL, 5 );


	bSizer14->Add( bSizer17, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer29;
	bSizer29 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText13 = new wxStaticText( this, wxID_ANY, wxT("Fly speed:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	m_staticText13->SetMinSize( wxSize( 65,-1 ) );

	bSizer29->Add( m_staticText13, 0, wxALL, 5 );

	m_flySpeedEdt = new wxTextCtrl( this, wxID_ANY, wxT("0.3"), wxDefaultPosition, wxDefaultSize, 0 );
	m_flySpeedEdt->SetMaxSize( wxSize( 60,-1 ) );

	bSizer29->Add( m_flySpeedEdt, 0, wxALL, 5 );


	bSizer14->Add( bSizer29, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxHORIZONTAL );

	m_cancelBtn = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_cancelBtn, 0, wxALL, 5 );

	m_okBtn = new wxButton( this, wxID_ANY, wxT("Ok"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( m_okBtn, 0, wxALL, 5 );


	bSizer14->Add( bSizer18, 1, wxALIGN_RIGHT, 5 );


	this->SetSizer( bSizer14 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_cancelBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreferencesDlg::onCancel ), NULL, this );
	m_okBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreferencesDlg::onOk ), NULL, this );
}

PreferencesDlg::~PreferencesDlg()
{
	// Disconnect Events
	m_cancelBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreferencesDlg::onCancel ), NULL, this );
	m_okBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PreferencesDlg::onOk ), NULL, this );

}

ObjectDlg::ObjectDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer27;
	bSizer27 = new wxBoxSizer( wxVERTICAL );

	m_propertyGrid = new wxPropertyGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE);
	bSizer27->Add( m_propertyGrid, 1, wxALL|wxEXPAND, 5 );


	this->SetSizer( bSizer27 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_propertyGrid->Connect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( ObjectDlg::onPropertyChange ), NULL, this );
}

ObjectDlg::~ObjectDlg()
{
	// Disconnect Events
	m_propertyGrid->Disconnect( wxEVT_PG_CHANGED, wxPropertyGridEventHandler( ObjectDlg::onPropertyChange ), NULL, this );

}

SurfaceDlg::SurfaceDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer60;
	bSizer60 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer62;
	bSizer62 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer63;
	bSizer63 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText29 = new wxStaticText( this, wxID_ANY, wxT("X power:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText29->Wrap( -1 );
	bSizer63->Add( m_staticText29, 0, wxALIGN_CENTER|wxALL, 5 );

	m_xPowerEdit = new wxTextCtrl( this, wxID_ANY, wxT("3"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	bSizer63->Add( m_xPowerEdit, 0, wxALIGN_CENTER|wxALL, 5 );


	bSizer62->Add( bSizer63, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer64;
	bSizer64 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText30 = new wxStaticText( this, wxID_ANY, wxT("Y power:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText30->Wrap( -1 );
	bSizer64->Add( m_staticText30, 0, wxALIGN_CENTER|wxALL, 5 );

	m_yPowerEdit = new wxTextCtrl( this, wxID_ANY, wxT("3"), wxDefaultPosition, wxSize( 30,-1 ), 0 );
	bSizer64->Add( m_yPowerEdit, 0, wxALIGN_CENTER|wxALL, 5 );


	bSizer62->Add( bSizer64, 0, wxEXPAND, 5 );


	bSizer61->Add( bSizer62, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer68;
	bSizer68 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer69;
	bSizer69 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText33 = new wxStaticText( this, wxID_ANY, wxT("X size:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText33->Wrap( -1 );
	bSizer69->Add( m_staticText33, 0, wxALIGN_CENTER|wxALL, 5 );

	m_xSizeEdit = new wxTextCtrl( this, wxID_ANY, wxT("6"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_xSizeEdit->Enable( false );

	bSizer69->Add( m_xSizeEdit, 0, wxALIGN_CENTER|wxALL, 5 );


	bSizer68->Add( bSizer69, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer70;
	bSizer70 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText34 = new wxStaticText( this, wxID_ANY, wxT("Y size:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText34->Wrap( -1 );
	bSizer70->Add( m_staticText34, 0, wxALIGN_CENTER|wxALL, 5 );

	m_ySizeEdit = new wxTextCtrl( this, wxID_ANY, wxT("6"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_ySizeEdit->Enable( false );

	bSizer70->Add( m_ySizeEdit, 0, wxALIGN_CENTER|wxALL, 5 );


	bSizer68->Add( bSizer70, 0, wxEXPAND, 5 );


	bSizer61->Add( bSizer68, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer65;
	bSizer65 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer66;
	bSizer66 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText31 = new wxStaticText( this, wxID_ANY, wxT("X resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText31->Wrap( -1 );
	bSizer66->Add( m_staticText31, 0, wxALIGN_CENTER|wxALL, 5 );

	m_xResEdit = new wxTextCtrl( this, wxID_ANY, wxT("20"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	bSizer66->Add( m_xResEdit, 0, wxALIGN_CENTER|wxALL, 5 );


	bSizer65->Add( bSizer66, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer67;
	bSizer67 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText32 = new wxStaticText( this, wxID_ANY, wxT("Y resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText32->Wrap( -1 );
	bSizer67->Add( m_staticText32, 0, wxALIGN_CENTER|wxALL, 5 );

	m_yResEdit = new wxTextCtrl( this, wxID_ANY, wxT("20"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	bSizer67->Add( m_yResEdit, 0, wxALIGN_CENTER|wxALL, 5 );


	bSizer65->Add( bSizer67, 0, wxEXPAND, 5 );


	bSizer61->Add( bSizer65, 1, wxEXPAND, 5 );


	bSizer60->Add( bSizer61, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer71;
	bSizer71 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText35 = new wxStaticText( this, wxID_ANY, wxT("Mapping Mode:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText35->Wrap( -1 );
	bSizer71->Add( m_staticText35, 0, wxALIGN_CENTER|wxALL, 5 );

	wxString m_mappingComboChoices[] = { wxT("Texture Coordinates"), wxT("Texture Space") };
	int m_mappingComboNChoices = sizeof( m_mappingComboChoices ) / sizeof( wxString );
	m_mappingCombo = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_mappingComboNChoices, m_mappingComboChoices, 0 );
	m_mappingCombo->SetSelection( 0 );
	bSizer71->Add( m_mappingCombo, 0, wxALL, 5 );


	bSizer71->Add( 0, 0, 1, wxEXPAND, 5 );

	m_staticText36 = new wxStaticText( this, wxID_ANY, wxT("Type:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText36->Wrap( -1 );
	bSizer71->Add( m_staticText36, 0, wxALIGN_CENTER|wxALL, 5 );

	wxString m_typeComboChoices[] = { wxT("Bezier"), wxT("B-Spline") };
	int m_typeComboNChoices = sizeof( m_typeComboChoices ) / sizeof( wxString );
	m_typeCombo = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_typeComboNChoices, m_typeComboChoices, 0 );
	m_typeCombo->SetSelection( 0 );
	bSizer71->Add( m_typeCombo, 0, wxALL, 5 );


	bSizer60->Add( bSizer71, 0, wxEXPAND, 5 );


	bSizer60->Add( 0, 0, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer72;
	bSizer72 = new wxBoxSizer( wxHORIZONTAL );


	bSizer72->Add( 0, 0, 1, wxEXPAND, 5 );

	m_createBtn = new wxButton( this, wxID_ANY, wxT("Create"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer72->Add( m_createBtn, 0, wxALL, 5 );

	m_cancelBtn = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer72->Add( m_cancelBtn, 0, wxALL, 5 );


	bSizer60->Add( bSizer72, 0, wxEXPAND, 5 );


	this->SetSizer( bSizer60 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_typeCombo->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SurfaceDlg::onTypeChange ), NULL, this );
	m_createBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SurfaceDlg::onCreate ), NULL, this );
	m_cancelBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SurfaceDlg::onCancel ), NULL, this );
}

SurfaceDlg::~SurfaceDlg()
{
	// Disconnect Events
	m_typeCombo->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( SurfaceDlg::onTypeChange ), NULL, this );
	m_createBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SurfaceDlg::onCreate ), NULL, this );
	m_cancelBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SurfaceDlg::onCancel ), NULL, this );

}

SurfaceCpDlg::SurfaceCpDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer73;
	bSizer73 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer74;
	bSizer74 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText37 = new wxStaticText( this, wxID_ANY, wxT("Weight:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText37->Wrap( -1 );
	bSizer74->Add( m_staticText37, 0, wxALIGN_CENTER|wxALL, 5 );

	m_weightEdit = new wxTextCtrl( this, wxID_ANY, wxT("1"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	bSizer74->Add( m_weightEdit, 0, wxALIGN_CENTER|wxALL, 5 );


	bSizer73->Add( bSizer74, 0, wxEXPAND, 5 );


	bSizer73->Add( 0, 0, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer75;
	bSizer75 = new wxBoxSizer( wxHORIZONTAL );

	m_setWeightBtn = new wxButton( this, wxID_ANY, wxT("Set Weight"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer75->Add( m_setWeightBtn, 0, wxALL, 5 );


	bSizer75->Add( 0, 0, 1, wxEXPAND, 5 );

	m_closeBtn = new wxButton( this, wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer75->Add( m_closeBtn, 0, wxALL, 5 );


	bSizer73->Add( bSizer75, 0, wxEXPAND, 5 );


	this->SetSizer( bSizer73 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_setWeightBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SurfaceCpDlg::onSetWeight ), NULL, this );
	m_closeBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SurfaceCpDlg::onClose ), NULL, this );
}

SurfaceCpDlg::~SurfaceCpDlg()
{
	// Disconnect Events
	m_setWeightBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SurfaceCpDlg::onSetWeight ), NULL, this );
	m_closeBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SurfaceCpDlg::onClose ), NULL, this );

}

SurfaceEditDlg::SurfaceEditDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer76;
	bSizer76 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer77;
	bSizer77 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer79;
	bSizer79 = new wxBoxSizer( wxVERTICAL );

	bSizer79->SetMinSize( wxSize( 280,-1 ) );
	wxBoxSizer* bSizer81;
	bSizer81 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText38 = new wxStaticText( this, wxID_ANY, wxT("X resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText38->Wrap( -1 );
	bSizer81->Add( m_staticText38, 0, wxALIGN_CENTER|wxALL, 5 );

	m_xresEdit = new wxTextCtrl( this, wxID_ANY, wxT("20"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	bSizer81->Add( m_xresEdit, 0, wxALIGN_CENTER|wxALL, 5 );


	bSizer79->Add( bSizer81, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer82;
	bSizer82 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText39 = new wxStaticText( this, wxID_ANY, wxT("Y resolution:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText39->Wrap( -1 );
	bSizer82->Add( m_staticText39, 0, wxALIGN_CENTER|wxALL, 5 );

	m_yresEdit = new wxTextCtrl( this, wxID_ANY, wxT("20"), wxDefaultPosition, wxSize( 60,-1 ), 0 );
	bSizer82->Add( m_yresEdit, 0, wxALIGN_CENTER|wxALL, 5 );


	bSizer79->Add( bSizer82, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer83;
	bSizer83 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText40 = new wxStaticText( this, wxID_ANY, wxT("Mapping Mode:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText40->Wrap( -1 );
	bSizer83->Add( m_staticText40, 0, wxALIGN_CENTER|wxALL, 5 );

	wxString m_mappingComboChoices[] = { wxT("Texture Coordinates"), wxT("Texture Space") };
	int m_mappingComboNChoices = sizeof( m_mappingComboChoices ) / sizeof( wxString );
	m_mappingCombo = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_mappingComboNChoices, m_mappingComboChoices, 0 );
	m_mappingCombo->SetSelection( 0 );
	bSizer83->Add( m_mappingCombo, 0, wxALIGN_CENTER|wxALL, 5 );


	bSizer79->Add( bSizer83, 1, wxEXPAND, 5 );


	bSizer77->Add( bSizer79, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("Texture Edit\n") ), wxVERTICAL );

	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 3, 2, 0, 0 );

	wxBoxSizer* bSizer84;
	bSizer84 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText43 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("U move:"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_staticText43->Wrap( -1 );
	bSizer84->Add( m_staticText43, 0, wxALIGN_CENTER|wxALL, 5 );

	m_umoveBtn = new wxSpinButton( sbSizer2->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize( 30,25 ), wxSP_WRAP );
	bSizer84->Add( m_umoveBtn, 0, wxALIGN_CENTER|wxALL, 5 );

	m_umoveEdit = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxT("0.1"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	bSizer84->Add( m_umoveEdit, 0, wxALIGN_CENTER|wxALL, 5 );


	gSizer2->Add( bSizer84, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer85;
	bSizer85 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText44 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("U scale:"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_staticText44->Wrap( -1 );
	bSizer85->Add( m_staticText44, 0, wxALIGN_CENTER|wxALL, 5 );

	m_uscaleBtn = new wxSpinButton( sbSizer2->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize( 30,25 ), wxSP_WRAP );
	bSizer85->Add( m_uscaleBtn, 0, wxALIGN_CENTER|wxALL, 5 );

	m_uscaleEdit = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxT("2"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	bSizer85->Add( m_uscaleEdit, 0, wxALIGN_CENTER|wxALL, 5 );


	gSizer2->Add( bSizer85, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer86;
	bSizer86 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText45 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("V move:"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_staticText45->Wrap( -1 );
	bSizer86->Add( m_staticText45, 0, wxALIGN_CENTER|wxALL, 5 );

	m_vmoveBtn = new wxSpinButton( sbSizer2->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_WRAP );
	m_vmoveBtn->SetMinSize( wxSize( 30,25 ) );

	bSizer86->Add( m_vmoveBtn, 0, wxALIGN_CENTER|wxALL, 5 );

	m_vmoveEdit = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxT("0.1"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	bSizer86->Add( m_vmoveEdit, 0, wxALL, 5 );


	gSizer2->Add( bSizer86, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer87;
	bSizer87 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText46 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("V scale:"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	m_staticText46->Wrap( -1 );
	bSizer87->Add( m_staticText46, 0, wxALIGN_CENTER|wxALL, 5 );

	m_vscaleBtn = new wxSpinButton( sbSizer2->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize( 30,25 ), wxSP_WRAP );
	bSizer87->Add( m_vscaleBtn, 0, wxALIGN_CENTER|wxALL, 5 );

	m_vscaleEdit = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxT("2"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	bSizer87->Add( m_vscaleEdit, 0, wxALL, 5 );


	gSizer2->Add( bSizer87, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer88;
	bSizer88 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText47 = new wxStaticText( sbSizer2->GetStaticBox(), wxID_ANY, wxT("Rotate:"), wxDefaultPosition, wxSize( 45,-1 ), 0 );
	m_staticText47->Wrap( -1 );
	bSizer88->Add( m_staticText47, 0, wxALIGN_CENTER|wxALL, 5 );

	m_rotateBtn = new wxSpinButton( sbSizer2->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize( 30,25 ), wxSP_WRAP );
	bSizer88->Add( m_rotateBtn, 0, wxALIGN_CENTER|wxALL, 5 );

	m_rotateEdit = new wxTextCtrl( sbSizer2->GetStaticBox(), wxID_ANY, wxT("5"), wxDefaultPosition, wxSize( 40,-1 ), 0 );
	bSizer88->Add( m_rotateEdit, 0, wxALL, 5 );


	gSizer2->Add( bSizer88, 1, wxEXPAND, 5 );


	sbSizer2->Add( gSizer2, 0, wxEXPAND, 5 );


	bSizer77->Add( sbSizer2, 0, 0, 5 );


	bSizer76->Add( bSizer77, 0, wxEXPAND, 5 );


	bSizer76->Add( 0, 0, 1, wxEXPAND, 5 );

	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer( 0, 3, 0, 0 );

	wxBoxSizer* bSizer881;
	bSizer881 = new wxBoxSizer( wxVERTICAL );

	m_collisionBox = new wxCheckBox( this, wxID_ANY, wxT("Collision"), wxDefaultPosition, wxDefaultSize, 0 );
	m_collisionBox->SetValue(true);
	bSizer881->Add( m_collisionBox, 0, wxALL, 5 );

	m_normalsBox = new wxCheckBox( this, wxID_ANY, wxT("Accurate Normals"), wxDefaultPosition, wxDefaultSize, 0 );
	m_normalsBox->SetValue(true);
	bSizer881->Add( m_normalsBox, 0, wxALL, 5 );


	gSizer3->Add( bSizer881, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer89;
	bSizer89 = new wxBoxSizer( wxVERTICAL );

	m_xcycleBox = new wxCheckBox( this, wxID_ANY, wxT("Cyclic X"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer89->Add( m_xcycleBox, 0, wxALL, 5 );

	m_ycycleBox = new wxCheckBox( this, wxID_ANY, wxT("Cyclic Y"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer89->Add( m_ycycleBox, 0, wxALL, 5 );


	gSizer3->Add( bSizer89, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer90;
	bSizer90 = new wxBoxSizer( wxVERTICAL );

	m_xendBox = new wxCheckBox( this, wxID_ANY, wxT("X endpoint"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer90->Add( m_xendBox, 0, wxALL, 5 );

	m_yendBox = new wxCheckBox( this, wxID_ANY, wxT("Y endpoint"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer90->Add( m_yendBox, 0, wxALL, 5 );


	gSizer3->Add( bSizer90, 1, wxEXPAND, 5 );


	bSizer76->Add( gSizer3, 0, wxEXPAND, 5 );


	bSizer76->Add( 0, 0, 1, wxEXPAND, 5 );

	wxBoxSizer* bSizer78;
	bSizer78 = new wxBoxSizer( wxHORIZONTAL );


	bSizer78->Add( 0, 0, 1, wxEXPAND, 5 );

	m_applyBtn = new wxButton( this, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer78->Add( m_applyBtn, 0, wxALL, 5 );

	m_closeBtn = new wxButton( this, wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer78->Add( m_closeBtn, 0, wxALL, 5 );


	bSizer76->Add( bSizer78, 0, wxEXPAND, 5 );


	this->SetSizer( bSizer76 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_umoveBtn->Connect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( SurfaceEditDlg::onUMoveDown ), NULL, this );
	m_umoveBtn->Connect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( SurfaceEditDlg::onUMoveUp ), NULL, this );
	m_umoveEdit->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SurfaceEditDlg::onUMoveRateChanged ), NULL, this );
	m_uscaleBtn->Connect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( SurfaceEditDlg::onUScaleDown ), NULL, this );
	m_uscaleBtn->Connect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( SurfaceEditDlg::onUScaleUp ), NULL, this );
	m_uscaleEdit->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SurfaceEditDlg::onUScaleRateChanged ), NULL, this );
	m_vmoveBtn->Connect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( SurfaceEditDlg::onVMoveDown ), NULL, this );
	m_vmoveBtn->Connect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( SurfaceEditDlg::onVMoveUp ), NULL, this );
	m_vmoveEdit->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SurfaceEditDlg::onVMoveRateChanged ), NULL, this );
	m_vscaleBtn->Connect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( SurfaceEditDlg::onVScaleDown ), NULL, this );
	m_vscaleBtn->Connect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( SurfaceEditDlg::onVScaleUp ), NULL, this );
	m_vscaleEdit->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SurfaceEditDlg::onVScaleRateChanged ), NULL, this );
	m_rotateBtn->Connect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( SurfaceEditDlg::onRotateDown ), NULL, this );
	m_rotateBtn->Connect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( SurfaceEditDlg::onRotateUp ), NULL, this );
	m_rotateEdit->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SurfaceEditDlg::onRotateRateChanged ), NULL, this );
	m_applyBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SurfaceEditDlg::onApply ), NULL, this );
	m_closeBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SurfaceEditDlg::onClose ), NULL, this );
}

SurfaceEditDlg::~SurfaceEditDlg()
{
	// Disconnect Events
	m_umoveBtn->Disconnect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( SurfaceEditDlg::onUMoveDown ), NULL, this );
	m_umoveBtn->Disconnect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( SurfaceEditDlg::onUMoveUp ), NULL, this );
	m_umoveEdit->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SurfaceEditDlg::onUMoveRateChanged ), NULL, this );
	m_uscaleBtn->Disconnect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( SurfaceEditDlg::onUScaleDown ), NULL, this );
	m_uscaleBtn->Disconnect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( SurfaceEditDlg::onUScaleUp ), NULL, this );
	m_uscaleEdit->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SurfaceEditDlg::onUScaleRateChanged ), NULL, this );
	m_vmoveBtn->Disconnect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( SurfaceEditDlg::onVMoveDown ), NULL, this );
	m_vmoveBtn->Disconnect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( SurfaceEditDlg::onVMoveUp ), NULL, this );
	m_vmoveEdit->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SurfaceEditDlg::onVMoveRateChanged ), NULL, this );
	m_vscaleBtn->Disconnect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( SurfaceEditDlg::onVScaleDown ), NULL, this );
	m_vscaleBtn->Disconnect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( SurfaceEditDlg::onVScaleUp ), NULL, this );
	m_vscaleEdit->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SurfaceEditDlg::onVScaleRateChanged ), NULL, this );
	m_rotateBtn->Disconnect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( SurfaceEditDlg::onRotateDown ), NULL, this );
	m_rotateBtn->Disconnect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( SurfaceEditDlg::onRotateUp ), NULL, this );
	m_rotateEdit->Disconnect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SurfaceEditDlg::onRotateRateChanged ), NULL, this );
	m_applyBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SurfaceEditDlg::onApply ), NULL, this );
	m_closeBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SurfaceEditDlg::onClose ), NULL, this );

}

BlockDlg::BlockDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer91;
	bSizer91 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer95;
	bSizer95 = new wxBoxSizer( wxHORIZONTAL );

	wxBoxSizer* bSizer94;
	bSizer94 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer92;
	bSizer92 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText46 = new wxStaticText( this, wxID_ANY, wxT("Type:"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_staticText46->Wrap( -1 );
	bSizer92->Add( m_staticText46, 0, wxALL, 5 );

	wxString m_typeBoxChoices[] = { wxT("Add"), wxT("Subtract"), wxT("Static"), wxT("Solid") };
	int m_typeBoxNChoices = sizeof( m_typeBoxChoices ) / sizeof( wxString );
	m_typeBox = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxSize( 100,-1 ), m_typeBoxNChoices, m_typeBoxChoices, 0 );
	m_typeBox->SetSelection( 0 );
	bSizer92->Add( m_typeBox, 0, wxALL, 5 );


	bSizer94->Add( bSizer92, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer93;
	bSizer93 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText47 = new wxStaticText( this, wxID_ANY, wxT("Class:"), wxDefaultPosition, wxSize( 50,-1 ), 0 );
	m_staticText47->Wrap( -1 );
	bSizer93->Add( m_staticText47, 0, wxALL, 5 );

	wxArrayString m_classBoxChoices;
	m_classBox = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxSize( 100,-1 ), m_classBoxChoices, 0 );
	m_classBox->SetSelection( 0 );
	m_classBox->Enable( false );

	bSizer93->Add( m_classBox, 0, wxALL, 5 );


	bSizer94->Add( bSizer93, 0, wxEXPAND, 5 );


	bSizer95->Add( bSizer94, 1, wxEXPAND, 5 );

	m_orderBtn = new wxSpinButton( this, wxID_ANY, wxDefaultPosition, wxSize( -1,50 ), 0 );
	m_orderBtn->SetToolTip( wxT("change order") );

	bSizer95->Add( m_orderBtn, 0, wxALIGN_CENTER|wxALL, 5 );


	bSizer91->Add( bSizer95, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer96;
	bSizer96 = new wxBoxSizer( wxVERTICAL );


	bSizer91->Add( bSizer96, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer91 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_typeBox->Connect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( BlockDlg::onChangeType ), NULL, this );
	m_orderBtn->Connect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( BlockDlg::onOrderDown ), NULL, this );
	m_orderBtn->Connect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( BlockDlg::onOrderUp ), NULL, this );
}

BlockDlg::~BlockDlg()
{
	// Disconnect Events
	m_typeBox->Disconnect( wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( BlockDlg::onChangeType ), NULL, this );
	m_orderBtn->Disconnect( wxEVT_SCROLL_LINEDOWN, wxSpinEventHandler( BlockDlg::onOrderDown ), NULL, this );
	m_orderBtn->Disconnect( wxEVT_SCROLL_LINEUP, wxSpinEventHandler( BlockDlg::onOrderUp ), NULL, this );

}

ResourceDlg::ResourceDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer97;
	bSizer97 = new wxBoxSizer( wxVERTICAL );

	m_resourceTree = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	bSizer97->Add( m_resourceTree, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer98;
	bSizer98 = new wxBoxSizer( wxHORIZONTAL );


	bSizer98->Add( 0, 0, 1, wxEXPAND, 5 );

	m_closeBtn = new wxButton( this, wxID_ANY, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer98->Add( m_closeBtn, 0, wxALL, 5 );

	m_selectBtn = new wxButton( this, wxID_ANY, wxT("Select"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer98->Add( m_selectBtn, 0, wxALL, 5 );


	bSizer97->Add( bSizer98, 0, wxEXPAND, 5 );


	this->SetSizer( bSizer97 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_resourceTree->Connect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( ResourceDlg::onItemActivated ), NULL, this );
	m_resourceTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( ResourceDlg::onSelectionChanged ), NULL, this );
	m_closeBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ResourceDlg::onClose ), NULL, this );
	m_selectBtn->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ResourceDlg::onSelect ), NULL, this );
}

ResourceDlg::~ResourceDlg()
{
	// Disconnect Events
	m_resourceTree->Disconnect( wxEVT_COMMAND_TREE_ITEM_ACTIVATED, wxTreeEventHandler( ResourceDlg::onItemActivated ), NULL, this );
	m_resourceTree->Disconnect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( ResourceDlg::onSelectionChanged ), NULL, this );
	m_closeBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ResourceDlg::onClose ), NULL, this );
	m_selectBtn->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( ResourceDlg::onSelect ), NULL, this );

}
