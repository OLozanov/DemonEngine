#include "PerspectiveView.h"

#include "Geometry/Geometry.h"

const long PerspectiveView::ID_TIMER = wxNewId();

PerspectiveView::PerspectiveView(wxWindow* parent, Editor& editor)
: View(parent, editor, true)
, m_mouseLook(false)
, m_moveForward(false)
, m_moveBackward(false)
, m_mouseMove(false)
, m_axisLock(false)
, m_appendSelection(false)
, m_moveSpeed(0.3)
{
    m_camera.setPos({ -3, 3, -3 });
    m_camera.rotate(45, 45);

    m_timer.SetOwner(this, ID_TIMER);

    Connect(wxEVT_RIGHT_DOWN, (wxObjectEventFunction)&PerspectiveView::onRightMouseDown);
    Connect(wxEVT_RIGHT_UP, (wxObjectEventFunction)&PerspectiveView::onRightMouseUp);
    Connect(wxEVT_LEFT_DOWN, (wxObjectEventFunction)&PerspectiveView::onLeftMouseDown);
    Connect(wxEVT_LEFT_UP, (wxObjectEventFunction)&PerspectiveView::onLeftMouseUp);
    Connect(wxEVT_MOTION, (wxObjectEventFunction)&PerspectiveView::onMouseMove);
    Connect(wxEVT_KEY_DOWN, (wxObjectEventFunction)&PerspectiveView::onKeyDown);
    Connect(wxEVT_KEY_UP, (wxObjectEventFunction)&PerspectiveView::onKeyUp);
    Connect(wxEVT_PAINT, (wxObjectEventFunction)&PerspectiveView::onPaint);
    Connect(wxEVT_SIZE, (wxObjectEventFunction)&PerspectiveView::onResize);
    Connect(ID_TIMER, wxEVT_TIMER, (wxObjectEventFunction)&PerspectiveView::onTimer);

    m_timer.Start(Framerate, false);
}

void PerspectiveView::reset()
{
    m_camera.setPos({ -3, 3, -3 });
    m_camera.setAngles(45, 45);
}

vec3 PerspectiveView::getCursorRay(int x, int y)
{
    mat4 worldMat = m_camera.rotMat().transpose();

    vec3 topleft = worldMat * vec3(-m_fovx, m_fovy, 1.0f);
    vec3 xdir = worldMat[0] * m_fovx * (2.0 / m_width);
    vec3 ydir = -(worldMat[1] * m_fovy * (2.0 / m_height));

    vec3 ray = topleft + xdir * x + ydir * y;
    ray.normalize();

    return ray;
}

void PerspectiveView::determineLockAxis()
{
    if (m_xlock)
    {
        m_axisLock = true;
        m_axis = XAxis;
    }
    else if (m_ylock)
    {
        m_axisLock = true;
        m_axis = YAxis;
    }
    else if (m_zlock)
    {
        m_axisLock = true;
        m_axis = ZAxis;
    }
    else m_axisLock = false;
}

void PerspectiveView::onRightMouseDown(wxMouseEvent* event)
{
    m_mouseLook = true;
    wxPoint cursorPos = event->GetPosition();

    m_curX = cursorPos.x;
    m_curY = cursorPos.y;

    SetFocus();
}

void PerspectiveView::onRightMouseUp(wxMouseEvent*)
{
    m_mouseLook = false;
}

void PerspectiveView::onLeftMouseDown(wxMouseEvent* event)
{
    SetFocus();

    wxPoint cursorPos = event->GetPosition();

    EditType editType = m_editor.getEditType();

    if (editType == EditType::Vertices)
    {
        bool select = m_editor.selectVertices(m_camera.pos(), getCursorRay(cursorPos.x, cursorPos.y), m_pickPoint, m_appendSelection);
        m_mouseMove = select && !m_appendSelection;

        if (m_mouseMove && m_editor.getEditMode() == EditMode::Rotate)
        {
            int axis = m_axisLock ? m_axis : 1;

            switch (axis)
            {
            case XAxis:
                m_editor.calcVertsXAngle(m_pickPoint.y, m_pickPoint.z);
            break;
            case YAxis:
                m_editor.calcVertsYAngle(m_pickPoint.x, m_pickPoint.z);
            break;
            case ZAxis:
                m_editor.calcVertsZAngle(m_pickPoint.x, m_pickPoint.y);
            break;
            }
        }

        if (m_editor.getEditMode() == EditMode::Scale)
        {
            vec2 clickPoint = vec2(cursorPos.x, cursorPos.y);

            mat4 mat = m_projMat * m_camera.viewMat();

            const vec3& center = m_editor.vertsSelectionCenter();
            vec4 projection = mat * vec4(center, 1.0);

            if (fabs(projection.w) < math::eps) return;

            m_scaleCenter.x = (projection.x / projection.w + 1.0) * 0.5 * m_width;
            m_scaleCenter.y = (1.0 - projection.y / projection.w) * 0.5 * m_height;

            m_scaleDist = (clickPoint - m_scaleCenter).length();
        }
    }

    if (editType == EditType::Blocks)
    {
        bool select = m_editor.selectBlocks(m_camera.pos(), getCursorRay(cursorPos.x, cursorPos.y), m_pickPoint, m_pickDist, m_appendSelection);
        m_mouseMove = select && !m_appendSelection;

        if (m_mouseMove && m_editor.getEditMode() == EditMode::Rotate)
        {
            int axis = m_axisLock ? m_axis : 1;

            switch (axis)
            {
            case XAxis:
                m_editor.calcBlockXAngle(m_pickPoint.y, m_pickPoint.z);
                break;
            case YAxis:
                m_editor.calcBlockYAngle(m_pickPoint.x, m_pickPoint.z);
                break;
            case ZAxis:
                m_editor.calcBlockZAngle(m_pickPoint.x, m_pickPoint.y);
                break;
            }
        }

        if (m_mouseMove && m_editor.getEditMode() == EditMode::Scale)
        {
            vec2 clickPoint = vec2(cursorPos.x, cursorPos.y);

            mat4 mat = m_projMat * m_camera.viewMat();

            const vec3& center = m_editor.blockSelectionCenter();
            vec4 projection = mat * vec4(center, 1.0);

            if (fabs(projection.w) < math::eps) return;

            m_scaleCenter.x = (projection.x / projection.w + 1.0) * 0.5 * m_width;
            m_scaleCenter.y = (1.0 - projection.y / projection.w) * 0.5 * m_height;

            m_scaleDist = (clickPoint - m_scaleCenter).length();
        }

        if (m_mouseMove) m_editor.collectUpdatedBlocks(true);
    }

    if (editType == EditType::Polygons)
    {
        m_editor.selectPolygon(m_camera.pos(), getCursorRay(cursorPos.x, cursorPos.y), m_appendSelection);
        Refresh();
    }

    if (editType == EditType::Decals)
    {
        m_editor.addDecal(m_camera.pos(), getCursorRay(cursorPos.x, cursorPos.y));
        Refresh();
    }

    if (editType == EditType::Objects)
    {
        bool select = m_editor.selectObjects(m_camera.pos(), getCursorRay(cursorPos.x, cursorPos.y), m_pickPoint, m_pickDist, m_appendSelection);
        m_mouseMove = select && !m_appendSelection;

        if (m_mouseMove && m_editor.getEditMode() == EditMode::Rotate)
        {
            int axis = m_axisLock ? m_axis : 1;

            switch (axis)
            {
            case XAxis:
                m_editor.calcObjectXAngle(m_pickPoint.y, m_pickPoint.z);
                break;
            case YAxis:
                m_editor.calcObjectYAngle(m_pickPoint.x, m_pickPoint.z);
                break;
            case ZAxis:
                m_editor.calcObjectZAngle(m_pickPoint.x, m_pickPoint.y);
                break;
            }
        }
    }

    if (editType == EditType::Surfaces)
    {
        bool select = m_editor.selectSurfaces(m_camera.pos(), getCursorRay(cursorPos.x, cursorPos.y), m_pickPoint, m_pickDist, m_appendSelection);
        m_mouseMove = select && !m_appendSelection;

        if (m_mouseMove && m_editor.getEditMode() == EditMode::Rotate)
        {
            int axis = m_axisLock ? m_axis : 1;

            switch (axis)
            {
            case XAxis:
                m_editor.calcSurfaceXAngle(m_pickPoint.y, m_pickPoint.z);
                break;
            case YAxis:
                m_editor.calcSurfaceYAngle(m_pickPoint.x, m_pickPoint.z);
                break;
            case ZAxis:
                m_editor.calcSurfaceZAngle(m_pickPoint.x, m_pickPoint.y);
                break;
            }
        }

        if (m_editor.getEditMode() == EditMode::Scale)
        {
            vec2 clickPoint = vec2(cursorPos.x, cursorPos.y);

            mat4 mat = m_projMat * m_camera.viewMat();

            const vec3& center = m_editor.surfaceSelectionCenter();
            vec4 projection = mat * vec4(center, 1.0);

            if (fabs(projection.w) < math::eps) return;

            m_scaleCenter.x = (projection.x / projection.w + 1.0) * 0.5 * m_width;
            m_scaleCenter.y = (1.0 - projection.y / projection.w) * 0.5 * m_height;

            m_scaleDist = (clickPoint - m_scaleCenter).length();
        }
    }

    if (editType == EditType::ControlPoints)
    {
        bool select = m_editor.selectCp(m_camera.pos(), getCursorRay(cursorPos.x, cursorPos.y), m_pickPoint, m_appendSelection);
        m_mouseMove = select && !m_appendSelection;

        if (m_mouseMove && m_editor.getEditMode() == EditMode::Rotate)
        {
            int axis = m_axisLock ? m_axis : 1;

            switch (axis)
            {
            case XAxis:
                m_editor.calcCpXAngle(m_pickPoint.y, m_pickPoint.z);
                break;
            case YAxis:
                m_editor.calcCpYAngle(m_pickPoint.x, m_pickPoint.z);
                break;
            case ZAxis:
                m_editor.calcCpZAngle(m_pickPoint.x, m_pickPoint.y);
                break;
            }
        }

        if (m_editor.getEditMode() == EditMode::Scale)
        {
            vec2 clickPoint = vec2(cursorPos.x, cursorPos.y);

            mat4 mat = m_projMat * m_camera.viewMat();

            const vec3& center = m_editor.cpSelectionCenter();
            vec4 projection = mat * vec4(center, 1.0);

            if (fabs(projection.w) < math::eps) return;

            m_scaleCenter.x = (projection.x / projection.w + 1.0) * 0.5 * m_width;
            m_scaleCenter.y = (1.0 - projection.y / projection.w) * 0.5 * m_height;

            m_scaleDist = (clickPoint - m_scaleCenter).length();
        }
    }

    if (editType == EditType::Displace)
    {
        m_mouseMove = true;
    }
}

void PerspectiveView::onLeftMouseUp(wxMouseEvent* event)
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
        m_editor.collectUpdatedBlocks();
        m_editor.updateCsg();
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

void PerspectiveView::rotateBlocks(int axis, const vec3& newPt)
{
    switch (axis)
    {
    case XAxis:
        m_editor.rotateBlocksX(newPt);
        break;
    case YAxis:
        m_editor.rotateBlocksY(newPt);
        break;
    case ZAxis:
        m_editor.rotateBlocksZ(newPt);
        break;
    }
}

void PerspectiveView::rotateVertices(int axis, const vec3& newPt)
{
    switch (axis)
    {
    case XAxis:
        m_editor.rotateVertsX(newPt);
        break;
    case YAxis:
        m_editor.rotateVertsY(newPt);
        break;
    case ZAxis:
        m_editor.rotateVertsZ(newPt);
        break;
    }
}

void PerspectiveView::rotateObjects(int axis, const vec3& newPt)
{
    switch (axis)
    {
    case XAxis:
        m_editor.rotateObjectsX(newPt);
        break;
    case YAxis:
        m_editor.rotateObjectsY(newPt);
        break;
    case ZAxis:
        m_editor.rotateObjectsZ(newPt);
        break;
    }
}

void PerspectiveView::rotateSurfaces(int axis, const vec3& newPt)
{
    switch (axis)
    {
    case XAxis:
        m_editor.rotateSurfacesX(newPt);
        break;
    case YAxis:
        m_editor.rotateSurfacesY(newPt);
        break;
    case ZAxis:
        m_editor.rotateSurfacesZ(newPt);
        break;
    }
}

void PerspectiveView::rotateControlPoints(int axis, const vec3& newPt)
{
    switch (axis)
    {
    case XAxis:
        m_editor.rotateCpX(newPt);
        break;
    case YAxis:
        m_editor.rotateCpY(newPt);
        break;
    case ZAxis:
        m_editor.rotateCpZ(newPt);
        break;
    }
}

void PerspectiveView::onMouseMove(wxMouseEvent* event)
{
    if (m_mouseMove)
    {
        wxPoint cursorPos = event->GetPosition();
        
        EditType type = m_editor.getEditType();
        EditMode mode = m_editor.getEditMode();

        m_curX = cursorPos.x;
        m_curY = cursorPos.y;

        vec3 ray = getCursorRay(cursorPos.x, cursorPos.y);

        if (type == EditType::Displace)
        {
            m_editor.surfaceDisplace(m_camera.pos(), ray);
            Refresh();

            return;
        }

        if (mode == EditMode::Move)
        {
            if (m_axisLock)
            {
                if (fabs(1.0 - ray[m_axis]) < math::eps) return;

                vec3 axisvec;
                axisvec[m_axis] = 1.0;

                float s, t;

                RayIntersect(m_camera.pos(), m_pickPoint, ray, axisvec, s, t);
                vec3 newPt = m_pickPoint;
                newPt[m_axis] += t;

                switch (type)
                {
                case EditType::Vertices:
                    m_editor.moveVertices(newPt);
                break;
                case EditType::Blocks:
                    m_editor.moveBlocks(newPt);
                break;
                case EditType::Objects:
                    m_editor.moveObjects(newPt);
                break;
                case EditType::Surfaces:
                    m_editor.moveSurfaces(newPt);
                break;
                case EditType::ControlPoints:
                    m_editor.moveCp(newPt);
                break;
                }
            }
            else
            {
                if (fabs(ray.y) < math::eps) return;

                float dist = (m_pickPoint.y - m_camera.pos().y) / ray.y;
                vec3 newPt = m_camera.pos() + ray * dist;

                switch (type)
                {
                case EditType::Vertices:
                    m_editor.moveVertices(newPt);
                break;
                case EditType::Blocks:
                    m_editor.moveBlocks(newPt);
                break;
                case EditType::Objects:
                    m_editor.moveObjects(newPt);
                break;
                case EditType::Surfaces:
                    m_editor.moveSurfaces(newPt);
                break;
                case EditType::ControlPoints:
                    m_editor.moveCp(newPt);
                break;
                }
            }
        }

        if (mode == EditMode::Rotate)
        {
            int axis = m_axisLock ? m_axis : 1;

            if (fabs(ray[axis]) < math::eps) return;

            float dist = (m_pickPoint[axis] - m_camera.pos()[axis]) / ray[axis];
            vec3 newPt = m_camera.pos() + ray * dist;

            switch (type)
            {
            case EditType::Vertices:
                rotateVertices(axis, newPt);
            break;
            case EditType::Blocks:
                rotateBlocks(axis, newPt);
            break;
            case EditType::Objects:
                rotateObjects(axis, newPt);
            break;
            case EditType::Surfaces:
                rotateSurfaces(axis, newPt);
            break;
            case EditType::ControlPoints:
                rotateControlPoints(axis, newPt);
            break;
            }
        }

        if (mode == EditMode::Scale)
        {
            vec2 diff = vec2(cursorPos.x, cursorPos.y) - m_scaleCenter;
            float dist = diff.length() / m_scaleDist;

            vec3 scale;

            if (m_axisLock)
            {
                scale = { 1.0, 1.0, 1.0 };
                scale[m_axis] = dist;
            }
            else scale = { dist, dist, dist };

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
    else if (m_editor.getEditType() == EditType::Displace)
    {
        wxPoint cursorPos = event->GetPosition();
        vec3 ray = getCursorRay(cursorPos.x, cursorPos.y);

        m_editor.surfaceIntersect(m_camera.pos(), ray);

        if (!m_mouseLook) Refresh();
    }
}

void PerspectiveView::onKeyDown(wxKeyEvent* event)
{
    switch (event->GetKeyCode())
    {
    case 'W': m_moveForward = true; break;
    case 'S': m_moveBackward = true; break;
    case 'X': m_xlock = true; break;
    case 'Y': m_ylock = true; break;
    case 'Z': m_zlock = true; break;
    case WXK_SHIFT: m_appendSelection = true; break;
    case WXK_DELETE: m_editor.deleteSelected(); break;
    //case WXK_CONTROL: mstate &= ~ACT_CONTROL; break;
    }

    determineLockAxis();
}

void PerspectiveView::onKeyUp(wxKeyEvent* event)
{
    switch (event->GetKeyCode())
    {
    case 'W': m_moveForward = false; break;
    case 'S': m_moveBackward = false; break;
    case 'X': m_xlock = false; break;
    case 'Y': m_ylock = false; break;
    case 'Z': m_zlock = false; break;
    case 'P':
        event->ResumePropagation(wxEVENT_PROPAGATE_MAX);
        event->Skip();
    break;
    case WXK_SHIFT: m_appendSelection = false; break;
    }

    determineLockAxis();
}

void PerspectiveView::onResize(wxSizeEvent*)
{
    DoGetClientSize(&m_width, &m_height);

    m_swapChain.resize(m_width, m_height);

    float aspectRatio = (float)m_width / (float)m_height;

    constexpr float znear = 0.1f;
    constexpr float zfar = 5000.0f;

    m_projMat = mat4::Projection(75, aspectRatio, znear, zfar);

    m_fovx = 1.0 / m_projMat[0][0];
    m_fovy = 1.0 / m_projMat[1][1];

    Refresh();
}

void PerspectiveView::onPaint(wxPaintEvent*)
{
    m_camera.update();

    m_editor.display(m_swapChain.getFrameBuffer(), m_camera, m_projMat, m_width, m_height);
    m_swapChain.present();
}

void PerspectiveView::onTimer(wxTimerEvent*)
{
    if (m_mouseLook)
    {
        int x, y;

        wxGetMousePosition(&x, &y);
        ScreenToClient(&x, &y);

        m_camera.rotate(y - m_curY, x - m_curX);
        
        m_curX = x;
        m_curY = y;

        Refresh();
    }

    if (m_moveForward)
    {
        m_camera.move(m_camera.direction() * m_moveSpeed);
        Refresh();
    }

    if (m_moveBackward)
    {
        m_camera.move(-m_camera.direction() * m_moveSpeed);
        Refresh();
    }
}