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

#include "precompiled.h"
#include "Settings.h"

#include <QLineEdit>
#include <QPushButton>
#include <QKeyEvent>

#include <AzCore/UserSettings/UserSettingsComponent.h>
#include <AzCore/Component/ComponentApplicationBus.h>
#include <AzToolsFramework/UI/PropertyEditor/ReflectedPropertyEditor.hxx>

#include <GraphCanvas/Editor/AssetEditorBus.h>

#include <Editor/GraphCanvas/GraphCanvasEditorNotificationBusId.h>

#include "Editor/View/Dialogs/ui_Settings.h"

namespace ScriptCanvasEditor
{
    SettingsDialog::SettingsDialog(const QString& title, AZ::EntityId graphId, QWidget* pParent /*=nullptr*/)
        : QDialog(pParent)
        , ui(new Ui::SettingsDialog)
        , m_graphId(graphId)
    {
        ui->setupUi(this);

        setWindowTitle(title);

        QObject::connect(ui->ok, &QPushButton::clicked, this, &SettingsDialog::OnOK);
        QObject::connect(ui->cancel, &QPushButton::clicked, this, &SettingsDialog::OnCancel);

        if (m_graphId.IsValid())
        {
            SetType(SettingsType::Graph);
        }
        else
        {
            SetType(SettingsType::General);
        }        

        m_revertOnClose = true;
    }

    void SettingsDialog::ConfigurePropertyEditor(AzToolsFramework::ReflectedPropertyEditor* editor)
    {
        editor->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        editor->SetHideRootProperties(false);
        editor->SetDynamicEditDataProvider(nullptr);
        editor->ExpandAll();
        editor->InvalidateAll();
        editor->setFixedHeight(editor->GetContentHeight());
    }

    SettingsDialog::~SettingsDialog()
    {
        if (m_revertOnClose)
        {
            RevertSettings();
        }

        ui->propertyEditor->ClearInstances();
    }

    void SettingsDialog::OnTextChanged(const QString& text)
    {
        ui->ok->setEnabled(!text.isEmpty());
    }

    void SettingsDialog::OnOK()
    {
        m_revertOnClose = false;
        AZ::UserSettingsOwnerRequestBus::Event(AZ::UserSettings::CT_LOCAL, &AZ::UserSettingsOwnerRequests::SaveSettings);
        GraphCanvas::AssetEditorSettingsNotificationBus::Event(ScriptCanvasEditor::AssetEditorId, &GraphCanvas::AssetEditorSettingsNotifications::OnSettingsChanged);
        accept();
    }

    void Settings::Reflect(AZ::ReflectContext* reflection)
    {
        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
        if (serializeContext)
        {
            serializeContext->Class<Settings, AZ::UserSettings >()
                ->Version(0)
                ->Field("EnableLogging", &Settings::m_enableLogging)
                ;

            AZ::EditContext* editContext = serializeContext->GetEditContext();
            if (editContext)
            {
                editContext->Class<Settings>("Script Canvas Settings", "Per-graph Script Canvas settings")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->Attribute(AZ::Edit::Attributes::Visibility, AZ_CRC("PropertyVisibility_ShowChildrenOnly", 0xef428f20))
                    ->DataElement(AZ::Edit::UIHandlers::Default, &Settings::m_enableLogging, "Logging", "Will enable logging for this Script Canvas graph")
                ;
            }
        }
    }

    void SettingsDialog::OnCancel()
    {        
        RevertSettings();
        close();
    }

    void SettingsDialog::SetType(SettingsType settingsType)
    {
        AZ::SerializeContext* context = nullptr;
        {
            AZ::ComponentApplicationBus::BroadcastResult(context, &AZ::ComponentApplicationRequests::GetSerializeContext);
            AZ_Assert(context, "We should have a valid context!");
        }

        AZ_Warning("SetingsDialog", settingsType != SettingsType::None, 
            "Cannot set up settings for None type. Please choose a valid type.");

        // SettingsType::None
        ui->generalLabel->setVisible(false);
        ui->previewSettingsPropertyEditor->setVisible(false);
        ui->previewSettingsPropertyEditor->SetAutoResizeLabels(true);

        ui->graphLabel->setVisible(false);
        ui->propertyEditor->setVisible(false);
        ui->propertyEditor->SetAutoResizeLabels(true);

        if (settingsType == SettingsType::Graph || settingsType == SettingsType::All)
        {
            ui->graphLabel->setVisible(true);
            ui->propertyEditor->setVisible(true);
            SetupGraphSettings(context);
        }

        if (settingsType == SettingsType::General || settingsType == SettingsType::All)
        {
            ui->generalLabel->setVisible(true);
            ui->previewSettingsPropertyEditor->setVisible(true);
            SetupGeneralSettings(context);
        }

        m_settingsType = settingsType;
    }

    void SettingsDialog::SetupGeneralSettings(AZ::SerializeContext* context)
    {
        // General properties
        AZStd::intrusive_ptr<EditorSettings::ScriptCanvasEditorSettings> previewSettings =
            AZ::UserSettings::CreateFind<EditorSettings::ScriptCanvasEditorSettings>(
                AZ_CRC("ScriptCanvasPreviewSettings", 0x1c5a2965), AZ::UserSettings::CT_LOCAL);

        // Store a copy to revert if needed.
        m_originalEditorSettings = *previewSettings;

        ui->previewSettingsPropertyEditor->Setup(context, nullptr, false, 210);
        ui->previewSettingsPropertyEditor->AddInstance(previewSettings.get(), previewSettings->RTTI_GetType());
        ui->previewSettingsPropertyEditor->setObjectName("ui->previewSettingsPropertyEditor");
        ConfigurePropertyEditor(ui->previewSettingsPropertyEditor);
    }

    void SettingsDialog::SetupGraphSettings(AZ::SerializeContext* context)
    {
        if (m_graphId.IsValid())
        {
            AZStd::intrusive_ptr<Settings> settings = 
                AZ::UserSettings::CreateFind<Settings>(AZ::Crc32(m_graphId.ToString().c_str()),
                    AZ::UserSettings::CT_LOCAL);

            // Store a copy to revert if needed.
            m_originalSettings = *settings;

            ui->propertyEditor->setDisabled(false);
            ui->propertyEditor->Setup(context, nullptr, false, 210);
            ui->propertyEditor->AddInstance(settings.get(), settings->RTTI_GetType());
            ui->propertyEditor->setObjectName("ui->propertyEditor");
            ui->propertyEditor->SetSavedStateKey(AZ::Crc32(m_graphId.ToString().c_str()));
            ConfigurePropertyEditor(ui->propertyEditor);
        }
        else
        {
            ui->propertyEditor->setDisabled(true);
        }
    }

    void SettingsDialog::RevertSettings()
    {
        if (m_settingsType == SettingsType::Graph || m_settingsType == SettingsType::All)
        {
            if (m_graphId.IsValid())
            {
                AZStd::intrusive_ptr<Settings> settings =
                    AZ::UserSettings::CreateFind<Settings>(AZ::Crc32(m_graphId.ToString().c_str()),
                        AZ::UserSettings::CT_LOCAL);
                // Revert the stored copy, no changes will be stored.
                *settings = m_originalSettings;
            }
        }

        if (m_settingsType == SettingsType::General || m_settingsType == SettingsType::All)
        {
            // General properties
            AZStd::intrusive_ptr<EditorSettings::ScriptCanvasEditorSettings> previewSettings =
                AZ::UserSettings::CreateFind<EditorSettings::ScriptCanvasEditorSettings>(
                    AZ_CRC("ScriptCanvasPreviewSettings", 0x1c5a2965), AZ::UserSettings::CT_LOCAL);
            *previewSettings = m_originalEditorSettings;
        }

        m_revertOnClose = false;
    }


    #include <Editor/View/Dialogs/Settings.moc>
}