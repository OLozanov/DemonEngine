#pragma once

#include "View/View.h"

enum ViewType
{
    TopView = 0,
    SideView = 1,
    FrontView = 2,
    ViewTypesCount
};

class OrthoView : public View
{
public:
    OrthoView(wxWindow* parent, Editor& editor, ViewType type);

    const vec2& offset() const;
    void setOffset(const vec2& offset);

    float scale() const { return m_scale; }
    void setScale(float scale) { m_scale = scale; }

    void reset();

private:
    void rotateVertices(float x, float y);
    void rotateBlocks(float x, float y);
    void rotateObjects(float x, float y);
    void rotateSurfaces(float x, float y);
    void rotateControlPoints(float x, float y);

    void onRightMouseDown(wxMouseEvent*);
    void onRightMouseUp(wxMouseEvent*);
    void onLeftMouseDown(wxMouseEvent*);
    void onLeftMouseUp(wxMouseEvent*);
    void onMouseMove(wxMouseEvent*);
    void onMouseWheel(wxMouseEvent*);
    void onKeyDown(wxKeyEvent*);
    void onKeyUp(wxKeyEvent* event);
    void onResize(wxSizeEvent*);
    void onPaint(wxPaintEvent*);

private:

    ViewType m_type;

    float m_aspectRatio;

    vec3 m_offset;
    float m_scale;

    bool m_mousePan;
    bool m_mouseMove;

    bool m_xlock;
    bool m_ylock;
    bool m_appendSelection;

    int m_curX;
    int m_curY;

    mat4 m_projMat;

    vec3 m_pickPoint;
    vec2 m_scaleCenter;
    float m_scaleDist;

    static const mat4 ViewMat[ViewTypesCount];
    static const mat4 GridMat[ViewTypesCount];

    static int ViewAxes[ViewTypesCount][3];

    static constexpr float PanSpeed = 0.01;
    static constexpr float ScaleFactor = 1.5;
};