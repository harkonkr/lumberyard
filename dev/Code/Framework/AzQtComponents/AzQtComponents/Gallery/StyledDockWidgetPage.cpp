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

#include "StyledDockWidgetPage.h"
#include <Gallery/ui_StyledDockWidgetPage.h>

#include <AzQtComponents/Components/DockMainWindow.h>
#include <AzQtComponents/Components/FancyDocking.h>
#include <AzQtComponents/Components/WindowDecorationWrapper.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QSettings>
#include <QPushButton>

StyledDockWidgetPage::StyledDockWidgetPage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::StyledDockWidgetPage)
    , m_mainWindow(nullptr)
{
    ui->setupUi(this);
    connect(ui->button, &QPushButton::clicked, this, &StyledDockWidgetPage::showMainWindow);

    QString exampleText = R"(
This example demonstrates the minimum required to use Amazon's fancy docking and styled dock
widgets. AzQtComponents::DockMainWindow and AzQtComponents::StyledDockWidget use the same API as
QMainWindow and QDockWidget respectively. To use the fancy docking, simply construct an
AzQtComponents::FancyDocking with an AzQtComponents::DockMainWindow as the parent.<br/>
<br/>
QMainWindow docs: <a href="http://doc.qt.io/qt-5/qmainwindow.html">http://doc.qt.io/qt-5/qmainwindow.html</a><br/>
QDockWidget docs: <a href="http://doc.qt.io/qt-5/qdockwidget.html">http://doc.qt.io/qt-5/qdockwidget.html</a><br/>
    <pre>
#include &lt;AzQtComponents/Components/DockMainWindow.h&gt;
#include &lt;AzQtComponents/Components/StyledDockWidget.h&gt;

#include &lt;QVBoxLayout&gt;
#include &lt;QPushButton&gt;
#include &lt;QSettings&gt;

// Construct a main window and apply fancy docking.
AzQtComponents::DockMainWindow mainWindow;
auto fancyDocking = new AzQtComponents::FancyDocking(mainWindow);

// Add one AzQtComponents::StyledDockWidget per edge
auto leftDockWidget = new AzQtComponents::StyledDockWidget(&quot;Left Dock Widget&quot;, mainWindow);
leftDockWidget-&gt;setObjectName(leftDockWidget-&gt;windowTitle());
leftDockWidget-&gt;setWidget(new QLabel(&quot;StyledDockWidget&quot;));
mainWindow-&gt;addDockWidget(Qt::LeftDockWidgetArea, leftDockWidget);

auto topDockWidget = new AzQtComponents::StyledDockWidget(&quot;Top Dock Widget&quot;, mainWindow);
topDockWidget-&gt;setObjectName(topDockWidget-&gt;windowTitle());
topDockWidget-&gt;setWidget(new QLabel(&quot;StyledDockWidget&quot;));
mainWindow-&gt;addDockWidget(Qt::TopDockWidgetArea, topDockWidget);

auto rightDockWidget = new AzQtComponents::StyledDockWidget(&quot;Right Dock Widget&quot;, mainWindow);
rightDockWidget-&gt;setObjectName(rightDockWidget-&gt;windowTitle());
rightDockWidget-&gt;setWidget(new QLabel(&quot;StyledDockWidget&quot;));
mainWindow-&gt;addDockWidget(Qt::RightDockWidgetArea, rightDockWidget);

auto bottomDockWidget = new AzQtComponents::StyledDockWidget(&quot;Bottom Dock Widget&quot;, mainWindow);
bottomDockWidget-&gt;setObjectName(bottomDockWidget-&gt;windowTitle());
bottomDockWidget-&gt;setWidget(new QLabel(&quot;StyledDockWidget&quot;));
mainWindow-&gt;addDockWidget(Qt::BottomDockWidgetArea, bottomDockWidget);

QWidget* centralWidget = new QWidget;
QVBoxLayout* vl = new QVBoxLayout(centralWidget);
vl-&gt;addWidget(new QLabel(&quot;Central Widget&quot;));
mainWindow-&gt;setCentralWidget(centralWidget);

QSettings* settings = new QSettings(fancyDocking);

// Is is even possible to save and restore docks state
QPushButton* load = new QPushButton(&quot;Restore layout&quot;);
QPushButton* save = new QPushButton(&quot;Save layout&quot;);
vl-&gt;addWidget(load);
vl-&gt;addWidget(save);

connect(load, &amp;QPushButton::clicked, fancyDocking, [mainWindow, settings, fancyDocking]()
{
    const QByteArray windowGeometry = settings-&gt;value(&quot;geometry&quot;).toByteArray();
    const QByteArray state = settings-&gt;value(&quot;docks&quot;).toByteArray();
    if (!windowGeometry.isEmpty())
    {
        mainWindow-&gt;restoreGeometry(windowGeometry);
    }
    if (!state.isEmpty())
    {
        fancyDocking-&gt;restoreState(state);
    }
});

connect(save, &amp;QPushButton::clicked, fancyDocking, [mainWindow, settings, fancyDocking]()
{
    const QByteArray windowGeometry = mainWindow-&gt;saveGeometry();
    const QByteArray state = fancyDocking-&gt;saveState();
    if (!windowGeometry.isEmpty())
    {
        settings-&gt;setValue(&quot;geometry&quot;, windowGeometry);
    }
    if (!state.isEmpty())
    {
        settings-&gt;setValue(&quot;docks&quot;, state);
    }
});

// or to tabify or split docks
QPushButton* split = new QPushButton(&quot;Split docks&quot;);
QPushButton* tabs = new QPushButton(&quot;Tabs docks&quot;);
vl-&gt;addWidget(split);
vl-&gt;addWidget(tabs);

connect(split, &amp;QPushButton::clicked, fancyDocking, [mainWindow, fancyDocking, leftDockWidget, rightDockWidget]()
{
    fancyDocking-&gt;splitDockWidget(mainWindow, leftDockWidget, rightDockWidget, Qt::Vertical);
});

connect(tabs, &amp;QPushButton::clicked, fancyDocking, [mainWindow, fancyDocking, topDockWidget, bottomDockWidget]()
{
    fancyDocking-&gt;tabifyDockWidget(topDockWidget, bottomDockWidget, mainWindow);
});

// Show the main window
mainWindow->show();
    </pre>
    )";

    ui->exampleText->setHtml(exampleText);
}

StyledDockWidgetPage::~StyledDockWidgetPage()
{
}

void StyledDockWidgetPage::showMainWindow()
{
    if (m_mainWindow)
    {
        auto window = m_mainWindow->window();
        window->show();
        window->raise();
        window->activateWindow();
        return;
    }
    // Supply our own WindowDecorationWrapper, otherwise one gets automatically applied, forcing the
    // DockMainWindow to have Qt::Tool window flags. This is annoying on macOS, as the main window
    // is then always displayed on top, and also appears to prevent GammaRay from working until the
    // main window has been closed.
    auto wrapper = new AzQtComponents::WindowDecorationWrapper(AzQtComponents::WindowDecorationWrapper::OptionNone);
    wrapper->setObjectName("wrapper");
    m_mainWindow = new AzQtComponents::DockMainWindow(this);
    m_mainWindow->setObjectName("mainWindow");
    wrapper->setGuest(m_mainWindow);
    m_mainWindow->resize(800, 600);
    m_mainWindow->setAttribute(Qt::WA_DeleteOnClose);
    auto fancyDocking = new AzQtComponents::FancyDocking(m_mainWindow);

    QWidget* centralWidget = new QWidget;
    QPushButton* load = new QPushButton("Restore layout");
    QPushButton* save = new QPushButton("Save layout");
    QPushButton* split = new QPushButton("Split docks");
    QPushButton* tabs = new QPushButton("Tabs docks");
    QVBoxLayout* vl = new QVBoxLayout(centralWidget);

    vl->addWidget(new QLabel("Central Widget"));
    vl->addWidget(load);
    vl->addWidget(save);
    vl->addWidget(split);
    vl->addWidget(tabs);

    m_mainWindow->setCentralWidget(centralWidget);

    auto leftDockWidget = new AzQtComponents::StyledDockWidget("Left Dock Widget", m_mainWindow);
    leftDockWidget->setObjectName(leftDockWidget->windowTitle());
    leftDockWidget->setWidget(new QLabel("StyledDockWidget"));
    m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, leftDockWidget);

    auto topDockWidget = new AzQtComponents::StyledDockWidget("Top Dock Widget", m_mainWindow);
    topDockWidget->setObjectName(topDockWidget->windowTitle());
    topDockWidget->setWidget(new QLabel("StyledDockWidget"));
    m_mainWindow->addDockWidget(Qt::TopDockWidgetArea, topDockWidget);

    auto rightDockWidget = new AzQtComponents::StyledDockWidget("Right Dock Widget", m_mainWindow);
    rightDockWidget->setObjectName(rightDockWidget->windowTitle());
    rightDockWidget->setWidget(new QLabel("StyledDockWidget"));
    m_mainWindow->addDockWidget(Qt::RightDockWidgetArea, rightDockWidget);

    auto bottomDockWidget = new AzQtComponents::StyledDockWidget("Bottom Dock Widget", m_mainWindow);
    bottomDockWidget->setObjectName(bottomDockWidget->windowTitle());
    bottomDockWidget->setWidget(new QLabel("StyledDockWidget"));
    m_mainWindow->addDockWidget(Qt::BottomDockWidgetArea, bottomDockWidget);

    auto galleryCenter = mapToGlobal(frameGeometry().center());
    auto mainWindowCenter = m_mainWindow->mapToGlobal(m_mainWindow->frameGeometry().center());
    m_mainWindow->window()->move(galleryCenter - mainWindowCenter);
    m_mainWindow->show();

    QSettings* settings = new QSettings(fancyDocking);

    connect(load, &QPushButton::clicked, this, [wrapper, settings, fancyDocking]()
    {
        const QByteArray windowGeometry = settings->value("geometry").toByteArray();
        const QByteArray state = settings->value("docks").toByteArray();
        if (!windowGeometry.isEmpty())
        {
            wrapper->restoreGeometry(windowGeometry);
        }
        if (!state.isEmpty())
        {
            fancyDocking->restoreState(state);
        }
    });

    connect(save, &QPushButton::clicked, this, [wrapper, settings, fancyDocking]()
    {
        const QByteArray windowGeometry = wrapper->saveGeometry();
        const QByteArray state = fancyDocking->saveState();
        if (!windowGeometry.isEmpty())
        {
            settings->setValue("geometry", windowGeometry);
        }
        if (!state.isEmpty())
        {
            settings->setValue("docks", state);
        }
    });

    connect(split, &QPushButton::clicked, this, [this, fancyDocking, leftDockWidget, rightDockWidget]()
    {
        fancyDocking->splitDockWidget(m_mainWindow, leftDockWidget, rightDockWidget, Qt::Vertical);
    });

    connect(tabs, &QPushButton::clicked, this, [this, fancyDocking, topDockWidget, bottomDockWidget]()
    {
        fancyDocking->tabifyDockWidget(topDockWidget, bottomDockWidget, m_mainWindow);
    });
}

#include <Gallery/StyledDockWidgetPage.moc>
