#include "ObjectDlg.h"

ObjectDlgImpl::ObjectDlgImpl(wxWindow* parent, Editor& editor)
: ObjectDlg(parent)
, m_resourceDlg(new ResourceDlgImpl(this))
, m_editor(editor)
{
}

void ObjectDlgImpl::fill(Object* object, const TypeInfo& typeInfo)
{
    if (typeInfo.parentTypeInfo) fill(object, *typeInfo.parentTypeInfo);

    m_propertyGrid->Append(new wxPropertyCategory(typeInfo.name));

    for (auto member : typeInfo.members)
    {
        std::string val;

        char* ptr = reinterpret_cast<char*>(object) + member.offset;

        switch (member.type)
        {
        case TypeInfoMember::Type::Bool:
        {
            m_propertyGrid->Append(new ObjBoolProperty(member.name, reinterpret_cast<bool*>(ptr)));
        }
        break;

        case TypeInfoMember::Type::Integer:
        {
            m_propertyGrid->Append(new ObjIntProperty(member.name, reinterpret_cast<long*>(ptr)));
        }
        break;

        case TypeInfoMember::Type::Float:
        {
            m_propertyGrid->Append(new ObjFloatProperty(member.name, reinterpret_cast<float*>(ptr)));
        }
        break;

        case TypeInfoMember::Type::String:
        {
            m_propertyGrid->Append(new ObjStringProperty(member.name, reinterpret_cast<std::string*>(ptr)));
        }
        break;

        case TypeInfoMember::Type::Vec3:
        {
            vec3* vec = reinterpret_cast<vec3*>(ptr);

            wxPGProperty* vecProp = m_propertyGrid->Append(new wxStringProperty(member.name, member.name, "<composed>"));

            m_propertyGrid->AppendIn(vecProp, new ObjFloatProperty("x", &(vec->x)));
            m_propertyGrid->AppendIn(vecProp, new ObjFloatProperty("y", &(vec->y)));
            m_propertyGrid->AppendIn(vecProp, new ObjFloatProperty("z", &(vec->z)));

            vecProp->SetFlagRecursively(wxPG_PROP_COLLAPSED, true);
        }
        break;

        case TypeInfoMember::Type::Angle:
        {
            m_propertyGrid->Append(new ObjAngleProperty(member.name, reinterpret_cast<float*>(ptr)));
        }
        break;

        case TypeInfoMember::Type::Orientation:
        {
            mat3* mat = reinterpret_cast<mat3*>(ptr);
            std::shared_ptr<vec3> euler = std::make_shared<vec3>();

            *euler = ToEuler(*mat);

            euler->x = euler->x / math::pi * 180.0;
            euler->y = euler->y / math::pi * 180.0;
            euler->z = euler->z / math::pi * 180.0;

            wxPGProperty* vecProp = m_propertyGrid->Append(new wxStringProperty(member.name, member.name, "<composed>"));

            m_propertyGrid->AppendIn(vecProp, new ObjEulerProperty("x", mat, euler, 0));
            m_propertyGrid->AppendIn(vecProp, new ObjEulerProperty("y", mat, euler, 1));
            m_propertyGrid->AppendIn(vecProp, new ObjEulerProperty("z", mat, euler, 2));

            vecProp->SetFlagRecursively(wxPG_PROP_COLLAPSED, true);
        }
        break;

        case TypeInfoMember::Type::Color:
        {
            m_propertyGrid->Append(new ObjColorProperty(member.name, reinterpret_cast<vec3*>(ptr)));
        }
        break;

        case TypeInfoMember::Type::Model:
        {
            char* mdl_ptr = reinterpret_cast<char*>(object) + member.offset2;
            m_propertyGrid->Append(new ObjModelProperty(member.name,
                                   reinterpret_cast<std::string*>(ptr),
                                   reinterpret_cast<Model**>(mdl_ptr),
                                   m_resourceDlg));
        }
        break;

        case TypeInfoMember::Type::Sound:
        {
            m_propertyGrid->Append(new ObjSoundProperty(member.name, reinterpret_cast<std::string*>(ptr), m_resourceDlg));
        }
        break;

        case TypeInfoMember::Type::Enum:
        {
            wxArrayString valueNames;
            wxArrayInt values;

            for (const auto& option : *(member.enumInfo))
            {
                valueNames.Add(option.name);
                values.Add(option.value);
            }

            m_propertyGrid->Append(new ObjEnumProperty(member.name,
                                   reinterpret_cast<uint8_t*>(ptr),
                                   valueNames,
                                   values));
        }
        break;

        }
    }
}

void ObjectDlgImpl::update()
{
    m_propertyGrid->Clear();

    Object* sobject = m_editor.selectedObject();

    if (sobject)
    {
        m_propertyGrid->Enable();
        fill(sobject, sobject->getTypeInfo());
    }
    else
    {
        m_propertyGrid->Disable();
    }
}

void ObjectDlgImpl::onPropertyChange(wxPropertyGridEvent& event)
{
    onPropertyUpdate();
}