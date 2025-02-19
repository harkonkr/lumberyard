/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
// Original file Copyright Crytek GMBH or its affiliates, used under license.

#pragma once
#include <QWidget>

#include <Cry_Math.h>
#include <Cry_Matrix34.h>
#include "EditorCommonAPI.h"
#include "QViewportEvents.h"

class CImageEx;
struct DisplayContext;
class CCamera;
struct SRenderingPassInfo;
struct SRendParams;
struct Ray;
struct IRenderer;
struct I3DEngine;
struct SSystemGlobalEnvironment;
namespace Serialization {
    class IArchive;
}
using Serialization::IArchive;
using std::unique_ptr;

struct SKeyEvent;
struct SMouseEvent;
struct SViewportSettings;
struct SViewportState;
class QElapsedTimer;

class QViewport;
struct SRenderContext
{
    CCamera* camera;
    QViewport* viewport;
    SRendParams* renderParams;
    SRenderingPassInfo* passInfo;
};

enum class CameraControlMode
{
    NONE,
    PAN,
    ROTATE,
    ZOOM,
    ORBIT
};

class QViewportConsumer;
class EDITOR_COMMON_API QViewport
    : public QWidget
{
    Q_OBJECT
public:

    enum StartupMode {
        StartupMode_Immediate = 1, // Startup() will be called by QViewport's CTOR
        StartupMode_Manual // Startup() will be called by the derived class
    };

    explicit QViewport(QWidget* parent, StartupMode startupMode = StartupMode_Immediate);
    ~QViewport();
    void Startup();

    void AddConsumer(QViewportConsumer* consumer);
    void RemoveConsumer(QViewportConsumer* consumer);

    void CaptureMouse();
    void ReleaseMouse();
    void SetForegroundUpdateMode(bool foregroundUpdate);
    CCamera* Camera() const { return m_camera.get(); }
    void ResetCamera();
    void Serialize(IArchive& ar);

    void SetUseArrowsForNavigation(bool useArrowsForNavigation);
    void SetSceneDimensions(const Vec3& size) { m_sceneDimensions = size; }
    void SetSettings(const SViewportSettings& settings);
    const SViewportSettings& GetSettings() const {return *m_settings; }
    void SetState(const SViewportState& state);
    const SViewportState& GetState() const {return *m_state; }
    bool ScreenToWorldRay(Ray* ray, int x, int y);
    QPoint ProjectToScreen(const Vec3& point);
    void LookAt(const Vec3& target, float radius, bool snap);

    int Width() const;
    int Height() const;
    void SetSize(const QSize& size){ m_width = size.width(); m_height = size.height(); }
    void GetImageOffscreen(CImageEx& image, const QSize& customSize);

public slots:
    void Update();
protected slots:
    void RenderInternal();
    void ForceRebuildRenderContext();
signals:
    void SignalPreRender(const SRenderContext&);
    void SignalRender(const SRenderContext&);
    void SignalKey(const SKeyEvent&);
    void SignalMouse(const SMouseEvent&);
    void SignalUpdate();
    void SignalCameraMoved(const QuatT& qt);
protected:
    void mousePressEvent(QMouseEvent* ev) override;
    void mouseReleaseEvent(QMouseEvent* ev) override;
    void wheelEvent(QWheelEvent* ev) override;
    void mouseMoveEvent(QMouseEvent* ev) override;
    void keyPressEvent(QKeyEvent* ev) override;
    void keyReleaseEvent(QKeyEvent* ev) override;
    void resizeEvent(QResizeEvent* ev) override;
    void showEvent(QShowEvent* ev) override;
    void moveEvent(QMoveEvent* ev) override;
    void paintEvent(QPaintEvent* ev) override;
    bool event(QEvent* ev) override;

    void CameraMoved(QuatT qt, bool snap); //Confetti: Jurecka ... making this protected so can adjust camera to focus on items in derived class.

    float GetLastFrameTime() { return m_lastFrameTime; }
private:
    struct SPrivate;

    bool CreateRenderContext();
    void DestroyRenderContext();
    void StorePreviousContext();
protected:
    void SetCurrentContext();
    void RestorePreviousContext();
private:
    void UpdateBackgroundColor();

    void ProcessMouse();
    void ProcessKeys();
    void PreRender();
    void Render();
    void OnMouseEvent(const SMouseEvent& ev);
    void OnKeyEvent(const SKeyEvent& ev);
    float CalculateMoveSpeed(bool shiftPressed, bool ctrlPressed, bool scaleWithOrbitDistance = false) const;
    void CreateLookAt(const Vec3& target, float radius, QuatT& cameraTarget) const;
    void UpdateCameraControlMode(QMouseEvent* ev);

    struct SPreviousContext;
    std::vector<SPreviousContext> m_previousContexts;
    std::auto_ptr<CCamera> m_camera;
    QElapsedTimer* m_frameTimer;
    QTimer* m_timer;
    int m_width;
    int m_height;
    QPoint m_mousePressPos;
    int64 m_lastTime;
    float m_lastFrameTime;
    float m_averageFrameTime;
    bool m_useArrowsForNavigation;
    bool m_renderContextCreated;
    bool m_creatingRenderContext;
    bool m_updating;
    bool m_fastMode;
    bool m_slowMode;
    CameraControlMode m_cameraControlMode;

    Vec3 m_cameraSmoothPosRate;
    float m_cameraSmoothRotRate;
    int m_mouseMovementsSinceLastFrame;
    f32 m_LightRotationRadian;
    SMouseEvent m_pendingMouseMoveEvent;

    Vec3 m_sceneDimensions;
    std::unique_ptr<SPrivate> m_private;
    std::unique_ptr<SViewportSettings> m_settings;
    std::unique_ptr<SViewportState> m_state;
    std::vector<QViewportConsumer*> m_consumers;
};
