#pragma once

#include "View/View.h"

#include <wx/timer.h>

#include "View/ViewCamera.h"
#include "math/math3d.h"

class PerspectiveView : public View
{
public:
    PerspectiveView(wxWindow* parent, Editor& editor);

    void setMoveSpeed(float speed) { m_moveSpeed = speed; }
    float moveSpeed() { return m_moveSpeed; }

    void reset();

    const ViewCamera& camera() const { return m_camera; }
    ViewCamera& camera() { return m_camera; }

protected:
    vec3 getCursorRay(int x, int y);
    void determineLockAxis();

    void rotateBlocks(int axis, const vec3& newPt);
    void rotateVertices(int axis, const vec3& newPt);
    void rotateObjects(int axis, const vec3& newPt);
    void rotateSurfaces(int axis, const vec3& newPt);
    void rotateControlPoints(int axis, const vec3& newPt);

    void onKeyDown(wxKeyEvent*);
    void onKeyUp(wxKeyEvent*);
    void onRightMouseDown(wxMouseEvent*);
    void onRightMouseUp(wxMouseEvent*);
    void onLeftMouseDown(wxMouseEvent*);
    void onLeftMouseUp(wxMouseEvent*);
    void onMouseMove(wxMouseEvent*);
    void onResize(wxSizeEvent*);
    void onPaint(wxPaintEvent*);
    void onTimer(wxTimerEvent*);

private:
    wxTimer m_timer;

    mat4 m_projMat;
    float m_fovx;
    float m_fovy;

    ViewCamera m_camera;
    bool m_mouseLook;
    bool m_moveForward;
    bool m_moveBackward;
    bool m_mouseMove;

    int m_axis;
    bool m_axisLock;
    bool m_xlock;
    bool m_ylock;
    bool m_zlock;
    bool m_appendSelection;

    int m_curX;
    int m_curY;

    vec3 m_pickPoint;
    vec2 m_scaleCenter;
    float m_scaleDist;
    float m_pickDist;

    float m_moveSpeed;

    static const long ID_TIMER;
    static constexpr float Framerate = 1.0 / 60;
};