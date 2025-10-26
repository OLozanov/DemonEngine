#include "ResourceDlg.h"

ResourceDlgImpl::ResourceDlgImpl(wxWindow* parent)
: ResourceDlg(parent)
{
}

void ResourceDlgImpl::onClose(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}

void ResourceDlgImpl::onSelect(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}

void ResourceDlgImpl::onSelectionChanged(wxTreeEvent& event)
{
    m_resourceId = event.GetItem();

    ResourceInfo* resinfo = (ResourceInfo*)m_resourceTree->GetItemData(m_resourceId);
    m_selectBtn->Enable(resinfo != nullptr);
}

void ResourceDlgImpl::onItemActivated(wxTreeEvent& event)
{
    m_resourceId = event.GetItem();
    ResourceInfo* resinfo = (ResourceInfo*)m_resourceTree->GetItemData(m_resourceId);
    
    if(resinfo != nullptr) EndModal(wxID_OK);
}

int ResourceDlgImpl::open(const wxString& restype, const wxString& extension, bool showExtension)
{
    SetTitle(wxString("Select ") + restype.SubString(0, restype.size() - 2));

    m_selectBtn->Enable(false);

    m_extension = extension;
    m_showExtension = showExtension;
    m_resourceTree->DeleteAllItems();
    initResourceList(restype);

    return ShowModal();
}

const wxString& ResourceDlgImpl::getPath()
{
    ResourceInfo* resinfo = (ResourceInfo*)m_resourceTree->GetItemData(m_resourceId);

    if (!resinfo) return {};

    return resinfo->path;
}

void ResourceDlgImpl::initResourceList(const wxString& restype)
{
    wxString rootdir = wxString("./") + restype + "/";
    m_root = rootdir.ToStdString();

    wxTreeItemId root = m_resourceTree->AddRoot(restype, -1, -1);
    addResourceDirectory(root, rootdir.ToStdString());

    m_resourceTree->Expand(root);
}

void ResourceDlgImpl::addResourceDirectory(wxTreeItemId root, const std::filesystem::path& path)
{
    std::vector<std::filesystem::path> folders;
    std::vector<std::filesystem::path> files;

    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (entry.is_directory())
        {
            folders.push_back(entry.path());
        }
        else
        {
            wxString ext = entry.path().extension().c_str();
            if (ext == m_extension) files.push_back(entry.path());
        }
    }

    for (const std::filesystem::path& path : folders)
    {
        wxTreeItemId dir = m_resourceTree->AppendItem(root, path.filename().c_str(), -1, -1, nullptr);
        addResourceDirectory(dir, path);
    }

    for (const std::filesystem::path& path : files)
    {
        auto fname = m_showExtension ? path.filename() : path.filename().replace_extension();

        ResourceInfo* resinfo = new ResourceInfo;
        resinfo->path = m_showExtension ? path.lexically_relative(m_root).generic_string() :
                                          path.lexically_relative(m_root).replace_extension().generic_string();

        m_resourceTree->AppendItem(root, fname.c_str(), -1, -1, resinfo);
    }
}