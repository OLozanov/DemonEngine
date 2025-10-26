#pragma once

#include "Forms.h"

#include <filesystem>

class ResourceDlgImpl : public ResourceDlg
{
    struct ResourceInfo : public wxTreeItemData
    {
        wxString path;
    };

public:
    ResourceDlgImpl(wxWindow* parent);

    int open(const wxString& restype, const wxString& extension, bool showExtension = true);
    const wxString& getPath();

private:
    void onClose(wxCommandEvent& event) override;
    void onSelect(wxCommandEvent& event) override;
    void onSelectionChanged(wxTreeEvent& event) override;
    void onItemActivated(wxTreeEvent& event) override;

    void initResourceList(const wxString& restype);
    void addResourceDirectory(wxTreeItemId root, const std::filesystem::path& path);

private:
    std::filesystem::path m_root;
    wxString m_extension;
    bool m_showExtension;
    wxTreeItemId m_resourceId;
};