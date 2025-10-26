#include "OrthoView.h"

const mat4 OrthoView::ViewMat[ViewTypesCount] = { //Top view
                                                  {{1, 0, 0, 0}, 
                                                   {0, 0, 1, 0},
                                                   {0, 1, 0, 0},
                                                   {0, 0, 0, 1}},
                                                  
                                                  //SideView
                                                  {{0, 0, 1, 0},
                                                   {0, 1, 0, 0},
                                                   {1, 0, 0, 0},
                                                   {0, 0, 0, 1}},

                                                  //FronView
                                                  {{1, 0, 0, 0},
                                                   {0, 1, 0, 0},
                                                   {0, 0, 1, 0},
                                                   {0, 0, 0, 1}} };

const mat4 OrthoView::GridMat[ViewTypesCount] = { //Top view
                                                  {{1, 0, 0, 0},
                                                   {0, 1, 0, 0},
                                                   {0, 0, 1, 0},
                                                   {0, 0, 0, 1}},

                                                   //SideView
                                                  {{0, 1, 0, 0},
                                                   {1, 0, 0, 0},
                                                   {0, 0, 1, 0},
                                                   {0, 0, 0, 1}},

                                                    //FronView
                                                  {{1, 0, 0, 0},
                                                   {0, 0, 1, 0},
                                                   {0, 1, 0, 0},
                                                   {0, 0, 0, 1}} };

int OrthoView::ViewAxes[ViewTypesCount][3] = { {0, 2, 1},
                                               {2, 1, 0},
                                               {0, 1, 2} };

OrthoView::OrthoView(wxWindow* parent, Editor& editor, ViewType type)
: View(parent, editor)
, m_type(type)
, m_scale(0.5)
, m_mousePan(false)
, m_mouseMove(false)
, m_xlock(false)
, m_ylock(false)
, m_appendSelection(false)
{
    Connect(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&OrthoView::onRightMouseDown);
    Connect(wxEVT_RIGHT_UP, (wxObjectEventFunction)&OrthoView::onRightMouseUp);
    Connect(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&OrthoView::onLeftMouseDown);
    Connect(wxEVT_LEFT_UP, (wxObjectEventFunction)&OrthoView::onLeftMouseUp);
    Connect(wxEVT_MOTION, (wxObjectEventFunction)&OrthoView::onMouseMove);
    Connect(wxEVT_MOUSEWHEEL, (wxObjectEventFunction)&OrthoView::onMouseWheel);
    Connect(wxEVT_KEY_DOWN, (wxObjectEventFunction)&OrthoView::onKeyDown);
    Connect(wxEVT_KEY_UP, (wxObjectEventFunction)&OrthoView::onKeyUp);
    Connect(wxEVT_PAINT, (wxObjectEventFunction)&OrthoView::onPaint);
    Connect(wxEVT_SIZE, (wxObjectEventFunction)&OrthoView::onResize);
}

const vec2& OrthoView::offset() const
{
    int* axes = ViewAxes[m_type];

    return { m_offset[axes[0]], m_offset[axes[1]] };
}

void OrthoView::setOffset(const vec2& offset)
{
    int* axes = ViewAxes[m_type];

    m_offset[axes[0]] = offset.x;
    m_offset[axes[1]] = offset.y;
    m_offset[axes[2]] = 0;
}

void OrthoView::reset()
{
    m_offset = { 0, 0, 0 };
    m_scale = 0.5;
}

void OrthoView::rotateVertices(float x, float y)
{
    switch (m_type)
    {
    case ViewType::TopView:
        m_editor.rotateVertsY({ x, 0, y });
        break;
    case ViewType::SideView:
        m_editor.rotateVertsX({ 0, y, x });
        break;
    case ViewType::FrontView:
        m_editor.rotateVertsZ({ x, y, 0 });
        break;
    }
}

void OrthoView::rotateBlocks(float x, float y)
{
    switch (m_type)
    {
    case ViewType::TopView:
        m_editor.rotateBlocksY({ x, 0, y });
    break;
    case ViewType::SideView:
        m_editor.rotateBlocksX({ 0, y, x });
    break;
    case ViewType::FrontView:
        m_editor.rotateBlocksZ({ x, y, 0 });
    break;
    }
}

void OrthoView::rotateObjects(float x, float y)
{
    switch (m_type)
    {
    case ViewType::TopView:
        m_editor.rotateObjectsY({ x, 0, y });
        break;
    case ViewType::SideView:
        m_editor.rotateObjectsX({ 0, y, x });
        break;
    case ViewType::FrontView:
        m_editor.rotateObjectsZ({ x, y, 0 });
        break;
    }
}

void OrthoView::rotateSurfaces(float x, float y)
{
    switch (m_type)
    {
    case ViewType::TopView:
        m_editor.rotateSurfacesY({ x, 0, y });
        break;
    case ViewType::SideView:
        m_editor.rotateSurfacesX({ 0, y, x });
        break;
    case ViewType::FrontView:
        m_editor.rotateSurfacesZ({ x, y, 0 });
        break;
    }
}

void OrthoView::rotateControlPoints(float x, float y)
{
    switch (m_type)
    {
    case ViewType::TopView:
        m_editor.rotateCpY({ x, 0, y });
        break;
    case ViewType::SideView:
        m_editor.rotateCpX({ 0, y, x });
        break;
    case ViewType::FrontView:
        m_editor.rotateCpZ({ x, y, 0 });
        break;
    }
}

void OrthoView::onRightMouseDown(wxMouseEvent* event)
{
    m_mousePan = true;
    wxPoint cursorPos = event->GetPosition();

    m_curX = cursorPos.x;
    m_curY = cursorPos.y;
}

void OrthoView::onRightMouseUp(wxMouseEvent*)
{
    m_mousePan = false;
}

void OrthoView::onLeftMouseDown(wxMouseEvent* event)
{
    SetFocus();

    wxPoint cursorPos = event->GetPosition();

    float x = (float)cursorPos.x / m_width * 2.0 - 1.0;
    float y = -(float)cursorPos.y / m_height * 2.0 + 1.0;

    x /= m_scale / m_aspectRatio;
    y /= m_scale;

    int* axes = ViewAxes[m_type];

    x -= m_offset[axes[0]];
    y -= m_offset[axes[1]];

    EditType editType = m_editor.getEditType();

    if (editType == EditType::Blocks)
    {
        bool select = m_editor.selectBlocks2d(x, y, m_scale, m_appendSelection, m_pickPoint, axes[0], axes[1], axes[2]);
        m_mouseMove = select && !m_appendSelection;

        if (m_mouseMove && m_editor.getEditMode() == EditMode::Rotate)
        {
            switch (m_type)
            {
            case ViewType::TopView:
                m_editor.calcBlockYAngle(m_pickPoint.x, m_pickPoint.z);
            break;
            case ViewType::SideView:
                m_editor.calcBlockXAngle(m_pickPoint.y, m_pickPoint.z);
            break;
            case ViewType::FrontView:
                m_editor.calcBlockZAngle(m_pickPoint.x, m_pickPoint.y);
            break;
            }
        }

        if (m_mouseMove && m_editor.getEditMode() == EditMode::Scale)
        {
            vec2 clickPoint = vec2(cursorPos.x, cursorPos.y);

            const vec3& center = m_editor.blockSelectionCenter();

            float posx = (center[axes[0]] + m_offset[axes[0]]) * m_scale / m_aspectRatio;
            float posy = (center[axes[1]] + m_offset[axes[1]]) * m_scale;

            m_scaleCenter.x = (posx + 1.0) * 0.5 * m_width;
            m_scaleCenter.y = (1.0 - posy) * 0.5 * m_height;

            m_scaleDist = (clickPoint - m_scaleCenter).length();
        }

        if (m_mouseMove) m_editor.collectUpdatedBlocks(true);
    }

    if (editType == EditType::Vertices)
    {
        bool select = m_editor.selectVertices2d(x, y, m_scale, m_appendSelection, axes[0], axes[1]);
        m_mouseMove = select && !m_appendSelection;

        m_pickPoint.x = x;
        m_pickPoint.y = y;

        if (m_mouseMove && m_editor.getEditMode() == EditMode::Rotate)
        {
            switch (m_type)
            {
            case ViewType::TopView:
                m_editor.calcVertsYAngle(x, y);
            break;
            case ViewType::SideView:
                m_editor.calcVertsXAngle(y, x);
            break;
            case ViewType::FrontView:
                m_editor.calcVertsZAngle(x, y);
            break;
            }
        }

        if (m_editor.getEditMode() == EditMode::Scale)
        {
            vec2 clickPoint = vec2(cursorPos.x, cursorPos.y);

            const vec3& center = m_editor.vertsSelectionCenter();

            float posx = (center[axes[0]] + m_offset[axes[0]]) * m_scale / m_aspectRatio;
            float posy = (center[axes[1]] + m_offset[axes[1]]) * m_scale;

            m_scaleCenter.x = (posx + 1.0) * 0.5 * m_width;
            m_scaleCenter.y = (1.0 - posy) * 0.5 * m_height;

            m_scaleDist = (clickPoint - m_scaleCenter).length();
        }
    }

    if (editType == EditType::Objects)
    {
        bool select = m_editor.selectObjects2d(x, y, m_scale, m_appendSelection, m_pickPoint, axes[0], axes[1], axes[2]);
        m_mouseMove = select && !m_appendSelection;

        if (m_mouseMove && m_editor.getEditMode() == EditMode::Rotate)
        {
            switch (m_type)
            {
            case ViewType::TopView:
                m_editor.calcObjectYAngle(m_pickPoint.x, m_pickPoint.z);
                break;
            case ViewType::SideView:
                m_editor.calcObjectXAngle(m_pickPoint.y, m_pickPoint.z);
                break;
            case ViewType::FrontView:
                m_editor.calcObjectZAngle(m_pickPoint.x, m_pickPoint.y);
                break;
            }
        }
    }

    if (editType == EditType::Surfaces)
    {
        bool select = m_editor.selectSurfaces2d(x, y, m_scale, m_appendSelection, m_pickPoint, axes[0], axes[1], axes[2]);
        m_mouseMove = select && !m_appendSelection;

        if (m_mouseMove && m_editor.getEditMode() == EditMode::Rotate)
        {
            switch (m_type)
            {
            case ViewType::TopView:
                m_editor.calcSurfaceYAngle(m_pickPoint.x, m_pickPoint.z);
                break;
            case ViewType::SideView:
                m_editor.calcSurfaceXAngle(m_pickPoint.y, m_pickPoint.z);
                break;
            case ViewType::FrontView:
                m_editor.calcSurfaceZAngle(m_pickPoint.x, m_pickPoint.y);
                break;
            }
        }

        if (m_mouseMove && m_editor.getEditMode() == EditMode::Scale)
        {
            vec2 clickPoint = vec2(cursorPos.x, cursorPos.y);

            const vec3& center = m_editor.surfaceSelectionCenter();

            float posx = (center[axes[0]] + m_offset[axes[0]]) * m_scale / m_aspectRatio;
            float posy = (center[axes[1]] + m_offset[axes[1]]) * m_scale;

            m_scaleCenter.x = (posx + 1.0) * 0.5 * m_width;
            m_scaleCenter.y = (1.0 - posy) * 0.5 * m_height;

            m_scaleDist = (clickPoint - m_scaleCenter).length();
        }
    }

    if (editType == EditType::ControlPoints)
    {
        bool select = m_editor.selectCp2d(x, y, m_scale, m_appendSelection, axes[0], axes[1]);
        m_mouseMove = select && !m_appendSelection;

        m_pickPoint.x = x;
        m_pickPoint.y = y;

        if (m_mouseMove && m_editor.getEditMode() == EditMode::Rotate)
        {
            switch (m_type)
            {
            case ViewType::TopView:
                m_editor.calcCpYAngle(x, y);
            break;
            case ViewType::SideView:
                m_editor.calcCpXAngle(y, x);
            break;
            case ViewType::FrontView:
                m_editor.calcCpZAngle(x, y);
            break;
            }
        }

        if (m_editor.getEditMode() == EditMode::Scale)
        {
            vec2 clickPoint = vec2(cursorPos.x, cursorPos.y);

            const vec3& center = m_editor.cpSelectionCenter();

            float posx = (center[axes[0]] + m_offset[axes[0]]) * m_scale / m_aspectRatio;
            float posy = (center[axes[1]] + m_offset[axes[1]]) * m_scale;

            m_scaleCenter.x = (posx + 1.0) * 0.5 * m_width;
            m_scaleCenter.y = (1.0 - posy) * 0.5 * m_height;

            m_scaleDist = (clickPoint - m_scaleCenter).length();
        }
    }
}

void OrthoView::onLeftMouseUp(wxMouseEvent* event)
{
    EditType editType = m_editor.getEditType();

    if (editType == EditType::Blocks)
    {
        if (m_editor.getEditMode() == EditMode::Rotate) m_editor.applyBlockRotation();
        if (m_editor.getEditMode() == EditMode::Scale) m_editor.applyBlockScale();

        m_editor.updateBlockBBoxes();
        m_editor.collectUpdatedBlocks();
        m_editor.updateCsg();
    }

    if (editType == EditType::Vertices)
    {
        m_editor.updateBlockBBoxes();
        //m_editor.collectUpdatedBlocks();
        //m_editor.updateCsg();
    }

    if (editType == EditType::Objects)
    {
        if (m_editor.getEditMode() == EditMode::Rotate) m_editor.applyObjectRotation();
    }

    if (editType == EditType::Surfaces)
    {
        if (m_editor.getEditMode() == EditMode::Rotate) m_editor.applySurfaceRotation();
        if (m_editor.getEditMode() == EditMode::Scale) m_editor.applySurfaceScale();
    }

    m_mouseMove = false;

    onEditFinish();
}

void OrthoView::onMouseMove(wxMouseEvent* event)
{
    if (m_mousePan)
    {
        wxPoint cursorPos = event->GetPosition();

        switch (m_type)
        {
        case ViewType::TopView:
            m_offset.x += (cursorPos.x - m_curX) * PanSpeed / m_scale;
            m_offset.z -= (cursorPos.y - m_curY) * PanSpeed / m_scale;
        break;
        case ViewType::SideView:
            m_offset.z += (cursorPos.x - m_curX) * PanSpeed / m_scale;
            m_offset.y -= (cursorPos.y - m_curY) * PanSpeed / m_scale;
        break;
        case ViewType::FrontView:
            m_offset.x += (cursorPos.x - m_curX) * PanSpeed / m_scale;
            m_offset.y -= (cursorPos.y - m_curY) * PanSpeed / m_scale;
        break;
        }

        m_curX = cursorPos.x;
        m_curY = cursorPos.y;

        Refresh();
    }

    if (m_mouseMove)
    {
        wxPoint cursorPos = event->GetPosition();

        EditType type = m_editor.getEditType();
        EditMode mode = m_editor.getEditMode();

        float x = (float)cursorPos.x / m_width * 2.0 - 1.0;
        float y = -(float)cursorPos.y / m_height * 2.0 + 1.0;

        x /= m_scale / m_aspectRatio;
        y /= m_scale;

        int* axes = ViewAxes[m_type];

        x -= m_offset[axes[0]];
        y -= m_offset[axes[1]];

        if (mode == EditMode::Move)
        {
            float newX = m_ylock ? m_pickPoint[axes[0]] : x;
            float newY = m_xlock ? m_pickPoint[axes[1]] : y;

            switch (type)
            {
            case EditType::Vertices:
                m_editor.moveVertices2d(newX, newY, axes[0], axes[1]);
            break;
            case EditType::Blocks:
                m_editor.moveBlocks2d(newX, newY, axes[0], axes[1]);
            break;
            case EditType::Objects:
                m_editor.moveObjects2d(newX, newY, axes[0], axes[1]);
            break;
            case EditType::Surfaces:
                m_editor.moveSurfaces2d(newX, newY, axes[0], axes[1]);
            break;
            case EditType::ControlPoints:
                m_editor.moveCp2d(newX, newY, axes[0], axes[1]);
            break;
            }
        }

        if (mode == EditMode::Rotate)
        {
            switch (type)
            {
            case EditType::Vertices:
                rotateVertices(x, y);
            break;
            case EditType::Blocks:
                rotateBlocks(x, y);
            break;
            case EditType::Objects:
                rotateObjects(x, y);
            break;
            case EditType::Surfaces:
                rotateSurfaces(x, y);
            break;
            case EditType::ControlPoints:
                rotateControlPoints(x, y);
            break;
            }
        }

        if (mode == EditMode::Scale)
        {
            vec2 diff = vec2(cursorPos.x, cursorPos.y) - m_scaleCenter;
            float dist = diff.length() / m_scaleDist;

            vec3 scale;
           
            scale[axes[0]] = m_ylock ? 1.0 : dist;
            scale[axes[1]] = m_xlock ? 1.0 : dist;
            scale[axes[2]] = 1.0;

            switch (type)
            {
            case EditType::Vertices:
                m_editor.scaleVertices(scale);
            break;
            case EditType::Blocks:
                m_editor.scaleBlocks(scale);
            break;
            case EditType::Surfaces:
                m_editor.scaleSurfaces(scale);
            break;
            case EditType::ControlPoints:
                m_editor.scaleCp(scale);
            break;
            }
        }

        Refresh();
    }
}

void OrthoView::onMouseWheel(wxMouseEvent* event)
{
    int d = event->GetWheelRotation();

    if (d > 0) m_scale *= ScaleFactor;
    else m_scale /= ScaleFactor;

    Refresh();
}

void OrthoView::onKeyDown(wxKeyEvent* event)
{
    switch (event->GetKeyCode())
    {
    case 'X': m_xlock = true; break;
    case 'Y': m_ylock = true; break;
    case WXK_SHIFT: m_appendSelection = true; break;
    case WXK_DELETE: m_editor.deleteSelected(); break;
        //case WXK_CONTROL: mstate &= ~ACT_CONTROL; break;
    }
}

void OrthoView::onKeyUp(wxKeyEvent* event)
{
    switch (event->GetKeyCode())
    {
    case 'X': m_xlock = false; break;
    case 'Y': m_ylock = false; break;
    case WXK_SHIFT: m_appendSelection = false; break;
    }
}

void OrthoView::onResize(wxSizeEvent*)
{
    DoGetClientSize(&m_width, &m_height);

    m_swapChain.resize(m_width, m_height);

    m_aspectRatio = (float)m_width / (float)m_height;

    m_projMat = mat4::OrthoProjection(-m_aspectRatio, m_aspectRatio, -1, 1, -500, 500) * ViewMat[m_type];
}

void OrthoView::onPaint(wxPaintEvent*)
{
    m_editor.displayOrtho(m_swapChain.getFrameBuffer(), m_projMat, ViewMat[m_type], GridMat[m_type], m_offset, m_scale, m_width, m_height);
    m_swapChain.present();
}