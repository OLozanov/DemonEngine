#include "MapInfoDlg.h"

MapInfoDlgImpl::MapInfoDlgImpl(wxWindow* parent, Editor& editor)
: MapInfoDlg(parent)
, m_editor(editor)
{
    m_skyProp = new wxStringProperty("Sky", "sky", "sky01");
    m_globalLightEnableProp = new wxBoolProperty("Enable", "enable_dir_light", false);
    m_xdirProp = new wxFloatProperty("x", "dir_light_x", 1);
    m_ydirProp = new wxFloatProperty("y", "dir_light_y", 1);
    m_zdirProp = new wxFloatProperty("z", "dir_light_z", 1);
    m_dirLightColorProp = new wxColourProperty("Color", "dir_light_color", wxColour(255, 255, 255));
    m_giEnableProp = new wxBoolProperty("Sky Illumination", "enable_gi", false);
    m_giColorProp = new wxColourProperty("Sky Color", "gi_color", wxColour(31, 31, 51));
    
    m_propertyGrid->Append(new wxPropertyCategory("Properties"));

    m_propertyGrid->Append(m_skyProp);

    // Directional Light
    m_propertyGrid->Append(new wxPropertyCategory("Directional Light"));

    m_propertyGrid->Append(m_globalLightEnableProp);

    wxPGProperty* vecProp = m_propertyGrid->Append(new wxStringProperty("Direction", "Direction", "<composed>"));

    m_propertyGrid->AppendIn(vecProp, m_xdirProp);
    m_propertyGrid->AppendIn(vecProp, m_ydirProp);
    m_propertyGrid->AppendIn(vecProp, m_zdirProp);

    vecProp->SetFlagRecursively(wxPG_PROP_COLLAPSED, true);

    m_propertyGrid->Append(m_dirLightColorProp);

    // GI
    m_propertyGrid->Append(new wxPropertyCategory("Global Illumination"));

    m_propertyGrid->Append(m_giEnableProp);
    m_propertyGrid->Append(m_giColorProp);
}

void MapInfoDlgImpl::onShow(wxShowEvent& event)
{
    const MapInfo& mapInfo = m_editor.mapInfo();

    wxVariant areaColor = wxColour(mapInfo.dirLightColor.x * 255, mapInfo.dirLightColor.y * 255, mapInfo.dirLightColor.z * 255);
    wxVariant giColor = wxColour(mapInfo.giColor.x * 255, mapInfo.giColor.y * 255, mapInfo.giColor.z * 255);

    m_skyProp->SetValue(wxString(mapInfo.mapSky.c_str()));
    m_globalLightEnableProp->SetValue(mapInfo.enableGlobalLight);
    m_xdirProp->SetValue(mapInfo.dirLightDirection.x);
    m_ydirProp->SetValue(mapInfo.dirLightDirection.y);
    m_zdirProp->SetValue(mapInfo.dirLightDirection.z);
    m_dirLightColorProp->SetValue(areaColor);
    m_giEnableProp->SetValue(mapInfo.enableGi);
    m_giColorProp->SetValue(giColor);
}

void MapInfoDlgImpl::onChanged(wxPropertyGridEvent& event)
{
    wxPGProperty* property = event.GetProperty();
    wxAny value = property->GetValue();

    MapInfo& mapInfo = m_editor.mapInfo();

    if (property == m_skyProp)
    {
        wxString str = value.As<wxString>();
        mapInfo.mapSky = str.ToStdString();
    }

    if (property == m_globalLightEnableProp)
        mapInfo.enableGlobalLight = value.As<bool>();

    if (property == m_xdirProp || property == m_ydirProp || property == m_zdirProp)
    {
        wxAny x = m_xdirProp->GetValue();
        wxAny y = m_ydirProp->GetValue();
        wxAny z = m_zdirProp->GetValue();

        mapInfo.dirLightDirection = { x.As<float>(), y.As<float>(), z.As<float>() };
    }

    if (property == m_dirLightColorProp)
    {
        wxColour color = value.As<wxColour>();
        mapInfo.dirLightColor = { color.Red() / 255.0f, color.Green() / 255.0f, color.Blue() / 255.0f };
    }
    
    if (property == m_giEnableProp)
        mapInfo.enableGi = value.As<bool>();
    
    if (property == m_giColorProp)
    {
        wxColour color = value.As<wxColour>();
        mapInfo.giColor = { color.Red() / 255.0f, color.Green() / 255.0f, color.Blue() / 255.0f };
    }
}