#include "SurfaceDlg.h"
#include "Surfaces/BezierSurface.h"
#include "Surfaces/BSplineSurface.h"
#include "Surfaces/BezierTriangleSurface.h"

SurfaceDlgImpl::SurfaceDlgImpl(wxWindow* parent, Editor& editor)
: SurfaceDlg(parent)
, m_editor(editor)
{

}

void SurfaceDlgImpl::onCreate(wxCommandEvent& event)
{
    int xpower;
    int ypower;

    int xsize;
    int ysize;

    int xres;
    int yres;

    m_xPowerEdit->GetValue().ToInt(&xpower);
    m_yPowerEdit->GetValue().ToInt(&ypower);

    m_xSizeEdit->GetValue().ToInt(&xsize);
    m_ySizeEdit->GetValue().ToInt(&ysize);

    m_xResEdit->GetValue().ToInt(&xres);
    m_yResEdit->GetValue().ToInt(&yres);

    int type = m_typeCombo->GetSelection();

    Block& block = m_editor.editBlock();

    if (block.polygonsNum() != 1) return;

    BlockPolygon& poly = block.polygon(0);

    TextureMapping mapping = m_mappingCombo->GetSelection() == 0 ? TextureMapping::TCoords : TextureMapping::TSpace;

    switch (type)
    {
    case surf_bezier:
    {
        if (poly.vertnum == 4)
        {
            Surface* surface = new BezierSurface(&block, &poly, xpower, ypower, xres, yres, mapping);

            surface->setMapMode(mapping);
            surface->setMaterial(m_editor.getCurrentMaterial());
            m_editor.addSurface(surface);
        }

        if (poly.vertnum == 3)
        {
            Surface* surface = new BezierTriangleSurface(&block, &poly, xpower, xres, mapping);

            surface->setMapMode(mapping);
            surface->setMaterial(m_editor.getCurrentMaterial());
            m_editor.addSurface(surface);
        }
    }
    break;
    case surf_bspline:
    {
        if (poly.vertnum == 4)
        {
            Surface* surface = new BSplineSurface(&block, &poly, xpower, ypower, xsize, ysize, xres, yres, mapping);

            surface->setMapMode(mapping);
            surface->setMaterial(m_editor.getCurrentMaterial());
            m_editor.addSurface(surface);
        }
    }
    break;
    }

    Close();
}

void SurfaceDlgImpl::onCancel(wxCommandEvent& event)
{
    Close();
}

void SurfaceDlgImpl::onTypeChange(wxCommandEvent& event)
{
    int type = m_typeCombo->GetSelection();

    if (type == surf_bspline)
    {
        m_xSizeEdit->Enable(true);
        m_ySizeEdit->Enable(true);
    }
    else
    {
        m_xSizeEdit->Enable(false);
        m_ySizeEdit->Enable(false);
    }
}