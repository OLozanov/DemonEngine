#pragma once

#include "Forms.h"
#include "ResourceDlg.h"
#include "Editor.h"

#include "Utils/EventHandler.h"

class ObjectDlgImpl : public ObjectDlg
{
    class ObjBoolProperty : public wxBoolProperty
    {
        bool* m_ptr;

    public:
        ObjBoolProperty(const char* name, bool* ptr)
        : wxBoolProperty(name, name, *ptr)
        , m_ptr(ptr)
        {
        }

        void OnSetValue() override
        {
            *m_ptr = m_value;
        }
    };

    class ObjIntProperty : public wxIntProperty
    {
        long* m_ptr;

    public:
        ObjIntProperty(const char* name, long* ptr)
        : wxIntProperty(name, name, *ptr)
        , m_ptr(ptr)
        {
        }

        void OnSetValue() override
        {
            *m_ptr = m_value;
        }
    };

    class ObjFloatProperty : public wxFloatProperty
    {
        float* m_ptr;

    public:
        ObjFloatProperty(const char* name, float* ptr)
        : wxFloatProperty(name, name, *ptr)
        , m_ptr(ptr)
        {
        }

        void OnSetValue() override
        {
            *m_ptr = static_cast<double>(m_value);
        }
    };

    class ObjStringProperty : public wxStringProperty
    {
        std::string* m_ptr;

    public:
        ObjStringProperty(const char* name, std::string* ptr)
        : wxStringProperty(name, name, ptr->c_str())
        , m_ptr(ptr)
        {
        }

        void OnSetValue() override
        {
            wxString wxstr = m_value;
            *m_ptr = wxstr.char_str();
        }
    };

    class ObjColorProperty : public wxColourProperty
    {
        vec3* m_ptr;

    public:
        ObjColorProperty(const char* name, vec3* ptr)
        : wxColourProperty(name, name, wxColour(ptr->x * 255, ptr->y * 255, ptr->z * 255))
        , m_ptr(ptr)
        {
        }

        void OnSetValue() override
        {
            wxColour color;
            color << m_value;

            m_ptr->x = color.Red() / 255.0f;
            m_ptr->y = color.Green() / 255.0f;
            m_ptr->z = color.Blue() / 255.0f;
        }
    };

    class ObjAngleProperty : public wxFloatProperty
    {
        float* m_ptr;

    public:
        ObjAngleProperty(const char* name, float* ptr)
        : wxFloatProperty(name, name, *ptr / math::pi * 180.0)
        , m_ptr(ptr)
        {
        }

        void OnSetValue() override
        {
            *m_ptr = static_cast<double>(m_value) / 180.0 * math::pi;
        }
    };

    class ObjEulerProperty : public wxFloatProperty
    {
        mat3* m_ptr;
        std::shared_ptr<vec3> m_euler;
        size_t m_axis;

    public:
        ObjEulerProperty(const char* name, mat3* ptr, std::shared_ptr<vec3>& euler, size_t axis)
        : wxFloatProperty(name, name, (*euler)[axis])
        , m_ptr(ptr)
        , m_euler(euler)
        , m_axis(axis)
        {
        }

        void OnSetValue() override
        {
            (*m_euler)[m_axis] = static_cast<double>(m_value);
            
            *m_ptr = mat3::Rotate(m_euler->x / 180.0 * math::pi, 
                                  m_euler->y / 180.0 * math::pi, 
                                  m_euler->z / 180.0 * math::pi);
        }
    };

    class ObjModelProperty : public wxLongStringProperty
    {
        ResourceDlgImpl* m_dialog;

        std::string* m_str;
        Model** m_mdl;

    public:
        ObjModelProperty(const char* name, std::string* str, Model** mdl, ResourceDlgImpl* dlg)
        : wxLongStringProperty(name, name, str->c_str())
        , m_dialog(dlg)
        , m_str(str)
        , m_mdl(mdl)
        {
        }

        void OnSetValue() override
        {
            wxString path = m_value;

            *m_str = path.char_str();
            *m_mdl = ResourceManager::GetModel(*m_str);
        }

        bool DisplayEditorDialog(wxPropertyGrid* propGrid, wxVariant& value) override
        {
            if (m_dialog->open("Models", ".msh") == wxID_OK)
            {
                value = m_dialog->getPath();
                return true;
            }

            return false;
        }
    };

    class ObjSoundProperty : public wxLongStringProperty
    {
        ResourceDlgImpl* m_dialog;
        std::string* m_ptr;

    public:
        ObjSoundProperty(const char* name, std::string* ptr, ResourceDlgImpl* dlg)
        : wxLongStringProperty(name, name, ptr->c_str())
        , m_dialog(dlg)
        , m_ptr(ptr)
        {
        }

        void OnSetValue() override
        {
            wxString wxstr = m_value;
            *m_ptr = wxstr.char_str();
        }

        bool DisplayEditorDialog(wxPropertyGrid* propGrid, wxVariant& value) override
        {              
            if (m_dialog->open("Sounds", ".wav") == wxID_OK)
            {
                value = m_dialog->getPath();
                return true;
            }

            return false;
        }
    };

    class ObjEnumProperty : public wxEnumProperty
    {
        uint8_t* m_ptr;

    public:
        ObjEnumProperty(const char* name, uint8_t* ptr,
                        const wxArrayString& valueNames, const wxArrayInt& values)
        : wxEnumProperty(name, name, valueNames, values, *ptr)
        , m_ptr(ptr)
        {
        }

        void OnSetValue() override
        {
            long value = m_value;
            *m_ptr = value;
        }
    };

public:
    ObjectDlgImpl(wxWindow* parent, Editor& editor);

    void fill(Object* object, const TypeInfo& typeInfo);
    void update();

    Event<void()> onPropertyUpdate;

private:
    void onPropertyChange(wxPropertyGridEvent& event) override;

private:
    ResourceDlgImpl* m_resourceDlg;
    Editor& m_editor;
};