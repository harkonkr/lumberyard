/*
* All or portions of this file Copyright(c) Amazon.com, Inc.or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
*or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
*WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#include "GradientSignal_precompiled.h"
#include "EditorImageProcessingSystemComponent.h"

#include <AzCore/Asset/AssetManager.h>
#include <AzCore/IO/FileIO.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/std/string/wildcard.h>
#include <AzFramework/Asset/AssetSystemBus.h>
#include <AzFramework/StringFunc/StringFunc.h>
#include <AzToolsFramework/API/EditorAssetSystemAPI.h>
#include <AzToolsFramework/AssetBrowser/EBusFindAssetTypeByName.h>
#include <AzToolsFramework/AssetEditor/AssetEditorBus.h>
#include <GradientSignal/ImageSettings.h>
#include <QMenu>
#include <AzCore/IO/SystemFile.h>
#include <GradientSignalSystemComponent.h>

namespace GradientSignal
{
    void EditorImageProcessingSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<EditorImageProcessingSystemComponent, AZ::Component>()
                ->Version(0)
                ;

            if (auto editContext = serializeContext->GetEditContext())
            {
                editContext->Class<EditorImageProcessingSystemComponent>("EditorImageProcessingSystemComponent", "Handles adding gradient image setting context menus")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System", 0xc94d118b))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ;
            }
        }
    }

    void EditorImageProcessingSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& services)
    {
        services.push_back(AZ_CRC("GradientImageBuilderService", 0x00cea88a));
    }

    void EditorImageProcessingSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& services)
    {
        services.push_back(AZ_CRC("GradientImageBuilderService", 0x00cea88a));
    }

    void EditorImageProcessingSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& services)
    {
        (void)services;
    }

    void EditorImageProcessingSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& services)
    {
        (void)services;
    }

    void EditorImageProcessingSystemComponent::Init()
    {

    }

    void EditorImageProcessingSystemComponent::Activate()
    {
        AzToolsFramework::AssetBrowser::AssetBrowserInteractionNotificationBus::Handler::BusConnect();
    }

    void EditorImageProcessingSystemComponent::Deactivate()
    {
        AzToolsFramework::AssetBrowser::AssetBrowserInteractionNotificationBus::Handler::BusDisconnect();
    }

    void EditorImageProcessingSystemComponent::AddContextMenuActions(QWidget* /*caller*/, QMenu* menu, const AZStd::vector<AzToolsFramework::AssetBrowser::AssetBrowserEntry*>& entries)
    {
        // Register right click menu
        using namespace AzToolsFramework::AssetBrowser;
        auto entryIt = AZStd::find_if(entries.begin(), entries.end(), [](const AssetBrowserEntry* entry)
        {
            return entry->GetEntryType() == AssetBrowserEntry::AssetEntryType::Source;
        });

        if (entryIt == entries.end())
        {
            return;
        }

        auto source = azrtti_cast<SourceAssetBrowserEntry*>(*entryIt);
        if (!HandlesSource(source))
        {
            return;
        }

        AZStd::string settingsPath = source->GetFullPath() + "." + s_gradientImageSettingsExtension;
        bool settingsExist = AZ::IO::SystemFile::Exists(settingsPath.data());

        if(settingsExist)
        {
            menu->addAction("Edit Gradient Image Settings...", [this, source, settingsPath]()
            {
                bool result = false;
                AZ::Data::AssetInfo assetInfo;
                AZStd::string rootPath;
                AzToolsFramework::AssetSystemRequestBus::BroadcastResult(result, &AzToolsFramework::AssetSystem::AssetSystemRequest::GetSourceInfoBySourcePath, settingsPath.data(), assetInfo, rootPath);
                if (result)
                {
                    AZ::Data::Asset<GradientSignal::ImageSettings> asset = AZ::Data::AssetManager::Instance().GetAsset<GradientSignal::ImageSettings>(assetInfo.m_assetId, false);
                    AzToolsFramework::AssetEditor::AssetEditorRequestsBus::Broadcast(&AzToolsFramework::AssetEditor::AssetEditorRequests::OpenAssetEditor, asset);
                }
            });
        }
        else
        {
            menu->addAction("Enable Gradient Image Settings", [this, source, settingsPath]()
            {
                GradientSignal::ImageSettings imageSettings;
                AZ::Utils::SaveObjectToFile(settingsPath, AZ::DataStream::ST_XML, &imageSettings);
            });
        }
    }

    bool EditorImageProcessingSystemComponent::HandlesSource(const AzToolsFramework::AssetBrowser::SourceAssetBrowserEntry* entry) const
    {
        AZStd::string targetExtension = entry->GetExtension();

        return AZStd::wildcard_match("*.tif", targetExtension.data())
            || AZStd::wildcard_match("*.tiff", targetExtension.data())
            || AZStd::wildcard_match("*.png", targetExtension.data())
            || AZStd::wildcard_match("*.bmp", targetExtension.data())
            || AZStd::wildcard_match("*.jpg", targetExtension.data())
            || AZStd::wildcard_match("*.jpeg", targetExtension.data())
            || AZStd::wildcard_match("*.tga", targetExtension.data())
            || AZStd::wildcard_match("*.gif", targetExtension.data());
    }
} // namespace GradientSignal
