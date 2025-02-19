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


#include "EMotionFX_precompiled.h"

#include <AzCore/Component/Entity.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Math/Transform.h>
#include <AzCore/Asset/AssetManager.h>
#include <AzToolsFramework/API/ToolsApplicationAPI.h>
#include <AzToolsFramework/API/EditorAssetSystemAPI.h>

#include <LmbrCentral/Physics/CryCharacterPhysicsBus.h>

#include <Integration/Editor/Components/EditorActorComponent.h>
#include <Integration/AnimGraphComponentBus.h>

#include <EMotionFX/Tools/EMotionStudio/EMStudioSDK/Source/EMStudioManager.h>
#include <EMotionFX/Tools/EMotionStudio/EMStudioSDK/Source/MainWindow.h>
#include <EMotionFX/Tools/EMotionStudio/EMStudioSDK/Source/NodeSelectionWindow.h>
#include <EMotionFX/CommandSystem/Source/SelectionList.h>
#include <MCore/Source/AzCoreConversions.h>

#include <MathConversion.h>
#include <IRenderAuxGeom.h>
#include <Material/Material.h>


namespace EMotionFX
{
    namespace Integration
    {
        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::Reflect(AZ::ReflectContext* context)
        {
            auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
            if (serializeContext)
            {
                serializeContext->Class<EditorActorComponent, AzToolsFramework::Components::EditorComponentBase>()
                    ->Version(3)
                    ->Field("ActorAsset", &EditorActorComponent::m_actorAsset)
                    ->Field("MaterialPerLOD", &EditorActorComponent::m_materialPerLOD)
                    ->Field("MaterialPerActor", &EditorActorComponent::m_materialPerActor)
                    ->Field("AttachmentType", &EditorActorComponent::m_attachmentType)
                    ->Field("AttachmentTarget", &EditorActorComponent::m_attachmentTarget)
                    ->Field("RenderSkeleton", &EditorActorComponent::m_renderSkeleton)
                    ->Field("RenderCharacter", &EditorActorComponent::m_renderCharacter)
                    ->Field("RenderBounds", &EditorActorComponent::m_renderBounds)
                    ->Field("SkinningMethod", &EditorActorComponent::m_skinningMethod)
                    ->Field("LodLevel", &EditorActorComponent::m_lodLevel)
                ;

                AZ::EditContext* editContext = serializeContext->GetEditContext();
                if (editContext)
                {
                    editContext->Class<EditorActorComponent>("Actor", "The Actor component manages an instance of an Actor")
                        ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Category, "Animation")
                        ->Attribute(AZ::Edit::Attributes::Icon, ":/EMotionFX/ActorComponent.svg")
                        ->Attribute(AZ::Edit::Attributes::PrimaryAssetType, azrtti_typeid<ActorAsset>())
                        ->Attribute(AZ::Edit::Attributes::ViewportIcon, ":/EMotionFX/ActorComponent.svg")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                        ->Attribute(AZ::Edit::Attributes::HelpPageURL, "https://docs.aws.amazon.com/lumberyard/latest/userguide/component-actor.html")
                        ->DataElement(0, &EditorActorComponent::m_actorAsset,
                        "Actor asset", "Assigned actor asset")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorActorComponent::OnAssetSelected)
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                        ->Attribute("EditButton", "Gems/EMotionFX/Assets/Editor/Images/Icons/EMFX_icon_32x32")
                        ->Attribute("EditDescription", "Open in Animation Editor")
                        ->Attribute("EditCallback", &EditorActorComponent::LaunchAnimationEditor)
                        ->DataElement(0, &EditorActorComponent::m_materialPerActor,
                        "Material", "Material assignment for this actor")
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorActorComponent::OnMaterialPerActorChanged)
                        ->ClassElement(AZ::Edit::ClassElements::Group, "Options")
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                        ->DataElement(0, &EditorActorComponent::m_renderCharacter,
                        "Draw character", "Toggles rendering of character mesh.")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorActorComponent::OnDebugDrawFlagChanged)
                        ->DataElement(0, &EditorActorComponent::m_renderSkeleton,
                        "Draw skeleton", "Toggles rendering of skeleton.")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorActorComponent::OnDebugDrawFlagChanged)
                        ->DataElement(0, &EditorActorComponent::m_renderBounds, "Draw bounds", "Toggles rendering of world space bounding boxes.")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorActorComponent::OnDebugDrawFlagChanged)
                        ->DataElement(AZ::Edit::UIHandlers::ComboBox, &EditorActorComponent::m_skinningMethod,
                        "Skinning method", "Choose the skinning method this actor is using")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorActorComponent::OnSkinningMethodChanged)
                        ->EnumAttribute(SkinningMethod::DualQuat, "Dual quat skinning")
                        ->EnumAttribute(SkinningMethod::Linear, "Linear skinning")
                        ->ClassElement(AZ::Edit::ClassElements::Group, "Attach To")
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                        ->DataElement(AZ::Edit::UIHandlers::ComboBox, &EditorActorComponent::m_attachmentType,
                        "Attachment type", "Type of attachment to use when attaching to the target entity.")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorActorComponent::OnAttachmentTypeChanged)
                        ->EnumAttribute(AttachmentType::None, "None")
                        ->EnumAttribute(AttachmentType::SkinAttachment, "Skin attachment")
                        ->DataElement(0, &EditorActorComponent::m_attachmentTarget,
                        "Target entity", "Entity Id whose actor instance we should attach to.")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
                        ->Attribute(AZ::Edit::Attributes::RequiredService, AZ_CRC("EMotionFXActorService", 0xd6e8f48d))
                        ->Attribute(AZ::Edit::Attributes::Visibility, &EditorActorComponent::AttachmentTargetVisibility)
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorActorComponent::OnAttachmentTargetChanged)
                        ->ClassElement(AZ::Edit::ClassElements::Group, "Preview")
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                        ->DataElement(0, &EditorActorComponent::m_lodLevel,
                        "LOD Level", "Preview the LOD Level of the current actor.")
                        ->Attribute(AZ::Edit::Attributes::ChangeNotify, &EditorActorComponent::OnLODLevelChanged)
                    ;
                }
            }

            AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context);
            if (behaviorContext)
            {
                behaviorContext->Class<EditorActorComponent>()->RequestBus("ActorComponentRequestBus");
            }
        }

        //////////////////////////////////////////////////////////////////////////
        EditorActorComponent::EditorActorComponent()
            : m_renderCharacter(true)
            , m_renderSkeleton(false)
            , m_renderBounds(false)
            , m_entityVisible(true)
            , m_skinningMethod(SkinningMethod::DualQuat)
            , m_attachmentType(AttachmentType::None)
            , m_attachmentJointIndex(0)
            , m_lodLevel(0)
            , m_actorAsset(AZ::Data::AssetLoadBehavior::NoLoad)
        {
        }

        //////////////////////////////////////////////////////////////////////////
        EditorActorComponent::~EditorActorComponent()
        {
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::Init()
        {
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::Activate()
        {
            CreateActorInstance();

            AzToolsFramework::EditorVisibilityRequestBus::EventResult(m_entityVisible, GetEntityId(), &AzToolsFramework::EditorVisibilityRequests::GetCurrentVisibility);

            ActorComponentRequestBus::Handler::BusConnect(GetEntityId());
            EditorActorComponentRequestBus::Handler::BusConnect(GetEntityId());
            LmbrCentral::MeshComponentRequestBus::Handler::BusConnect(GetEntityId());
            LmbrCentral::RenderNodeRequestBus::Handler::BusConnect(GetEntityId());
            LmbrCentral::MaterialOwnerRequestBus::Handler::BusConnect(GetEntityId());
            LmbrCentral::AttachmentComponentNotificationBus::Handler::BusConnect(GetEntityId());
            AzToolsFramework::EditorComponentSelectionRequestsBus::Handler::BusConnect(GetEntityId());
            AzToolsFramework::EditorVisibilityNotificationBus::Handler::BusConnect(GetEntityId());
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::Deactivate()
        {
            AzToolsFramework::EditorVisibilityNotificationBus::Handler::BusDisconnect();
            AzToolsFramework::EditorComponentSelectionRequestsBus::Handler::BusDisconnect();
            LmbrCentral::AttachmentComponentNotificationBus::Handler::BusDisconnect();
            LmbrCentral::MaterialOwnerRequestBus::Handler::BusDisconnect();
            LmbrCentral::RenderNodeRequestBus::Handler::BusDisconnect();
            LmbrCentral::MeshComponentRequestBus::Handler::BusDisconnect();
            EditorActorComponentRequestBus::Handler::BusDisconnect();
            ActorComponentRequestBus::Handler::BusDisconnect();

            AZ::TransformNotificationBus::Handler::BusDisconnect();
            AZ::TickBus::Handler::BusDisconnect();
            AZ::Data::AssetBus::Handler::BusDisconnect();

            DestroyActorInstance();
            m_actorAsset.Release();
        }

        //////////////////////////////////////////////////////////////////////////
        bool EditorActorComponent::GetRenderCharacter() const
        {
            return m_renderCharacter;
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::SetRenderCharacter(bool enable)
        {
            if (m_renderCharacter != enable)
            {
                m_renderCharacter = enable;
                OnEntityVisibilityChanged(m_renderCharacter);
            }
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::CreateActorInstance()
        {
            AZ::Data::AssetBus::Handler::BusDisconnect();

            // Queue actor asset load. Instantiation occurs in OnAssetReady.
            if (m_actorAsset.GetId().IsValid())
            {
                AZ::Data::AssetBus::Handler::BusConnect(m_actorAsset.GetId());
                m_actorAsset.QueueLoad();
            }
            else
            {
                DestroyActorInstance();
            }
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::DestroyActorInstance()
        {
            if (m_actorInstance)
            {
                ActorComponentNotificationBus::Event(
                    GetEntityId(),
                    &ActorComponentNotificationBus::Events::OnActorInstanceDestroyed,
                    m_actorInstance.get());
            }

            m_actorInstance = nullptr;
            m_renderNode = nullptr;
        }

        //////////////////////////////////////////////////////////////////////////
        // ActorComponentRequestBus::Handler
        //////////////////////////////////////////////////////////////////////////
        const AZ::Data::AssetId& EditorActorComponent::GetActorAssetId()
        {
            return m_actorAsset.GetId();
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::OnEntityVisibilityChanged(bool visibility)
        {
            m_entityVisible = visibility;
            if (m_renderNode)
            {
                m_renderNode->Hide(!m_entityVisible || !m_renderCharacter);
            }
        }
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        AZ::Crc32 EditorActorComponent::OnAssetSelected()
        {
            CreateActorInstance();

            if (!m_actorAsset.GetId().IsValid())
            {
                m_materialPerLOD.clear();

                return AZ::Edit::PropertyRefreshLevels::EntireTree;
            }

            return AZ::Edit::PropertyRefreshLevels::None;
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::OnMaterialChanged()
        {
            if (m_renderNode)
            {
                m_renderNode->SetMaterials(m_materialPerLOD);
            }
        }

        void EditorActorComponent::OnMaterialPerActorChanged()
        {
            if (m_actorInstance)
            {
                m_materialPerLOD.resize(m_actorInstance->GetActor()->GetNumLODLevels());
                for (auto& materialPath : m_materialPerLOD)
                {
                    materialPath.SetAssetPath(m_materialPerActor.GetAssetPath().c_str());
                }
            }
            OnMaterialChanged();
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::OnLODLevelChanged()
        {
            if (m_actorInstance)
            {
                m_actorInstance->SetLODLevel(m_lodLevel);
            }
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::OnDebugDrawFlagChanged()
        {
            if (m_renderSkeleton || m_renderBounds)
            {
                AZ::TickBus::Handler::BusConnect();
            }
            else
            {
                AZ::TickBus::Handler::BusDisconnect();
            }

            if (m_renderNode)
            {
                m_renderNode->Hide(!m_entityVisible || !m_renderCharacter);
            }
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::OnSkinningMethodChanged()
        {
            if (m_renderNode)
            {
                m_renderNode->SetSkinningMethod(m_skinningMethod);
            }
        }

        //////////////////////////////////////////////////////////////////////////
        bool EditorActorComponent::AttachmentTargetVisibility()
        {
            return (m_attachmentType != AttachmentType::None);
        }

        //////////////////////////////////////////////////////////////////////////
        bool EditorActorComponent::AttachmentTargetJointVisibility()
        {
            return (m_attachmentType == AttachmentType::ActorAttachment);
        }

        //////////////////////////////////////////////////////////////////////////
        AZStd::string EditorActorComponent::AttachmentJointButtonText()
        {
            return m_attachmentJointName.empty() ?
                   AZStd::string("(No joint selected)") : m_attachmentJointName;
        }

        //////////////////////////////////////////////////////////////////////////
        AZ::Crc32 EditorActorComponent::OnAttachmentTypeChanged()
        {
            if (m_attachmentType == AttachmentType::None)
            {
                m_attachmentTarget.SetInvalid();
                m_attachmentJointName.clear();
            }
            return AZ::Edit::PropertyRefreshLevels::AttributesAndValues;
        }

        //////////////////////////////////////////////////////////////////////////
        AZ::Crc32 EditorActorComponent::OnAttachmentTargetChanged()
        {
            if (!IsValidAttachment(GetEntityId(), m_attachmentTarget))
            {
                m_attachmentTarget.SetInvalid();
                AZ_Error("EMotionFX", false, "You cannot attach to yourself or create circular dependencies! Attachment cannot be performed.");
            }
            return AZ::Edit::PropertyRefreshLevels::AttributesAndValues;
        }

        //////////////////////////////////////////////////////////////////////////
        AZ::Crc32 EditorActorComponent::OnAttachmentTargetJointSelect()
        {
            // Grab actor instance and invoke UI for joint selection.
            EMotionFXPtr<EMotionFX::ActorInstance> actorInstance;
            ActorComponentRequestBus::EventResult(
                actorInstance,
                m_attachmentTarget,
                &ActorComponentRequestBus::Events::GetActorInstance);

            AZ::Crc32 refreshLevel = AZ::Edit::PropertyRefreshLevels::None;

            if (actorInstance)
            {
                EMStudio::NodeSelectionWindow* nodeSelectWindow = new EMStudio::NodeSelectionWindow(nullptr, true);
                nodeSelectWindow->setWindowTitle(nodeSelectWindow->tr("Select Target Joint"));

                CommandSystem::SelectionList selection;

                // If a joint was previously selected, ensure it's pre-selected in the UI.
                if (!m_attachmentJointName.empty())
                {
                    EMotionFX::Node* node = actorInstance->GetActor()->GetSkeleton()->FindNodeByName(m_attachmentJointName.c_str());
                    if (node)
                    {
                        selection.AddNode(node);
                    }
                }

                QObject::connect(nodeSelectWindow, &EMStudio::NodeSelectionWindow::accepted,
                    [this, nodeSelectWindow, &refreshLevel, &actorInstance]()
                    {
                        auto& selectedItems = nodeSelectWindow->GetNodeHierarchyWidget()->GetSelectedItems();
                        if (!selectedItems.empty())
                        {
                            const char* jointName = selectedItems[0].GetNodeName();
                            EMotionFX::Node* node = actorInstance->GetActor()->GetSkeleton()->FindNodeByName(jointName);
                            if (node)
                            {
                                m_attachmentJointName = jointName;
                                m_attachmentJointIndex = node->GetNodeIndex();

                                refreshLevel = AZ::Edit::PropertyRefreshLevels::AttributesAndValues;
                            }
                        }
                    });

                nodeSelectWindow->Update(actorInstance->GetID(), &selection);
                nodeSelectWindow->exec();
                delete nodeSelectWindow;
            }

            return refreshLevel;
        }

        void EditorActorComponent::LaunchAnimationEditor(const AZ::Data::AssetId& assetId, const AZ::Data::AssetType&)
        {
            if (assetId.IsValid())
            {
                AZ::Data::AssetId animgraphAssetId;
                animgraphAssetId.SetInvalid();
                EditorAnimGraphComponentRequestBus::EventResult(animgraphAssetId, GetEntityId(), &EditorAnimGraphComponentRequestBus::Events::GetAnimGraphAssetId);
                AZ::Data::AssetId motionSetAssetId;
                motionSetAssetId.SetInvalid();
                EditorAnimGraphComponentRequestBus::EventResult(motionSetAssetId, GetEntityId(), &EditorAnimGraphComponentRequestBus::Events::GetMotionSetAssetId);

                // call to open must be done before LoadCharacter
                const char* panelName = EMStudio::MainWindow::GetEMotionFXPaneName();
                EBUS_EVENT(AzToolsFramework::EditorRequests::Bus, OpenViewPane, panelName);

                EMStudio::MainWindow* mainWindow = EMStudio::GetMainWindow();
                if (mainWindow)
                {
                    mainWindow->LoadCharacter(assetId, animgraphAssetId, motionSetAssetId);
                }
            }
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::OnAssetReloaded(AZ::Data::Asset<AZ::Data::AssetData> asset)
        {
            OnAssetReady(asset);
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::OnAssetError(AZ::Data::Asset<AZ::Data::AssetData> asset)
        {
            // Escalate the job if the asset failed.
            AZ::Outcome<AzToolsFramework::AssetSystem::JobInfoContainer> jobOutcome = AZ::Failure();
            AzToolsFramework::AssetSystemJobRequestBus::BroadcastResult(jobOutcome, &AzToolsFramework::AssetSystemJobRequestBus::Events::GetAssetJobsInfoByAssetID, asset.GetId(), true, true);
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset)
        {
            m_actorAsset = asset;

            // Enable/disable debug drawing.
            OnDebugDrawFlagChanged();

            // Create actor instance.
            auto* actorAsset = m_actorAsset.GetAs<ActorAsset>();
            AZ_Error("EMotionFX", actorAsset, "Actor asset is not valid.");
            if (!actorAsset)
            {
                return;
            }

            if (m_actorInstance)
            {
                ActorComponentNotificationBus::Event(
                    GetEntityId(),
                    &ActorComponentNotificationBus::Events::OnActorInstanceDestroyed,
                    m_actorInstance.get());
            }

            m_actorInstance = actorAsset->CreateInstance(GetEntity());
            if (!m_actorInstance)
            {
                AZ_Error("EMotionFX", actorAsset, "Failed to create actor instance.");
                return;
            }

            // If we are loading the actor for the first time, automatically add the material
            // per lod information. If the amount of lods between different actors that are assigned
            // to this component differ, then reinit the materials.
            if (m_materialPerActor.GetAssetPath().empty())
            {
                InitializeMaterial(*actorAsset);
            }
            OnMaterialPerActorChanged();

            // Assign entity Id to user data field, so we can extract owning entity from an EMFX actor pointer.
            m_actorInstance->SetCustomData(reinterpret_cast<void*>(static_cast<AZ::u64>(GetEntityId())));

            // Notify listeners that an actor instance has been created.
            ActorComponentNotificationBus::Event(
                GetEntityId(),
                &ActorComponentNotificationBus::Events::OnActorInstanceCreated,
                m_actorInstance.get());

            // Setup initial transform and listen for transform changes.
            AZ::Transform transform;
            AZ::TransformBus::EventResult(transform, GetEntityId(), &AZ::TransformBus::Events::GetWorldTM);
            OnTransformChanged(transform, transform);
            AZ::TransformNotificationBus::Handler::BusConnect(GetEntityId());

            // Force an update of node transforms so we can get an accurate bounding box.
            m_actorInstance->UpdateTransformations(0.0f, true, false);

            m_renderNode = AZStd::make_unique<ActorRenderNode>(GetEntityId(), m_actorInstance, m_actorAsset, transform);
            m_renderNode->SetMaterials(m_materialPerLOD);
            m_renderNode->RegisterWithRenderer();
            m_renderNode->Hide(!m_renderCharacter || !m_entityVisible);
            m_renderNode->SetSkinningMethod(m_skinningMethod);

            // Reattach all attachments
            for (AZ::EntityId& attachment : m_attachments)
            {
                LmbrCentral::AttachmentComponentRequestBus::Event(attachment, &LmbrCentral::AttachmentComponentRequestBus::Events::Reattach, true);
            }

            // Send general mesh creation notification to interested parties.
            LmbrCentral::MeshComponentNotificationBus::Event(GetEntityId(), &LmbrCentral::MeshComponentNotifications::OnMeshCreated, m_actorAsset);
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::InitializeMaterial(ActorAsset& actorAsset)
        {
            if (!m_materialPerLOD.empty())
            {
                // If the materialPerLOD exist, it means that we previously stored the path to the material. Use it.
                m_materialPerActor.SetAssetPath(m_materialPerLOD[0].GetAssetPath().c_str());
            }
            else
            {
                // If a material exists next to the actor, pre - initialize LOD material slot with that material.
                // This is merely an accelerator for the user, and is isolated to tools-only code (the editor actor component).
                AZStd::string materialAssetPath;
                EBUS_EVENT_RESULT(materialAssetPath, AZ::Data::AssetCatalogRequestBus, GetAssetPathById, actorAsset.GetId());
                if (!materialAssetPath.empty())
                {
                    // Query the catalog for a material of the same name as the actor.
                    AzFramework::StringFunc::Path::ReplaceExtension(materialAssetPath, "mtl");
                    AZ::Data::AssetId materialAssetId;
                    EBUS_EVENT_RESULT(materialAssetId, AZ::Data::AssetCatalogRequestBus, GetAssetIdByPath, materialAssetPath.c_str(), AZ::Data::s_invalidAssetType, false);

                    // If found, initialize all empty material slots with the material.
                    if (materialAssetId.IsValid())
                    {
                        m_materialPerActor.SetAssetPath(materialAssetPath.c_str());
                    }
                }
            }

            using namespace AzToolsFramework;
            ToolsApplicationEvents::Bus::Broadcast(&ToolsApplicationEvents::InvalidatePropertyDisplay, Refresh_EntireTree);
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world)
        {
            AZ_UNUSED(local);
            if (!m_actorInstance)
            {
                return;
            }

            const AZ::Quaternion entityOrientation = AZ::Quaternion::CreateRotationFromScaledTransform(world);
            const AZ::Vector3 entityPosition = world.GetTranslation();
            const AZ::Transform worldTransformNoScale = AZ::Transform::CreateFromQuaternionAndTranslation(entityOrientation, entityPosition);
            m_actorInstance->SetLocalSpaceTransform(MCore::AzTransformToEmfxTransform(worldTransformNoScale));
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::SetPrimaryAsset(const AZ::Data::AssetId& assetId)
        {
            AZ::Data::Asset<ActorAsset> asset = AZ::Data::AssetManager::Instance().GetAsset<ActorAsset>(assetId, false);
            if (asset)
            {
                m_actorAsset = asset;
                OnAssetSelected();
            }
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::OnTick(float deltaTime, AZ::ScriptTimePoint time)
        {
            if (!m_actorInstance)
            {
                return;
            }

            if (m_renderSkeleton)
            {
                ActorComponent::DrawSkeleton(m_actorInstance);
            }

            if (m_renderBounds)
            {
                ActorComponent::DrawBounds(m_actorInstance);
            }
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::SetMaterial(_smart_ptr<IMaterial> material)
        {
            ActorAsset* actor = m_actorAsset.GetAs<ActorAsset>();
            if (!actor)
            {
                return;
            }

            // Set m_materialPerActor and m_materialPerLOD, which contains the material asset references
            if (material)
            {
                if (material->IsSubMaterial())
                {
                    // Attempt to apply the parent material if material is a sub-material
                    CMaterial* editorMaterial = static_cast<CMaterial*>(material->GetUserData());
                    if (editorMaterial && editorMaterial->GetParent() && editorMaterial->GetParent()->GetMatInfo())
                    {
                        material = editorMaterial->GetParent()->GetMatInfo();
                        AZ_Warning("EMotionFX", false, "Cannot apply a sub-material directly to an actor. Applying the parent material group '%s' instead.", material->GetName());
                    }
                    else
                    {
                        AZ_Error("EMotionFX", false, "Cannot apply sub-material '%s' directly to an actor. Try applying the parent material group instead.", material->GetName());
                        return;
                    }
                }

                // Apply the material to the actor
                m_materialPerLOD.clear();
                m_materialPerActor.SetAssetPath(material->GetName());
            }
            else
            {
                // If material is nullptr, re-set m_materialPerLOD to the default for this actor
                m_materialPerLOD.clear();
                m_materialPerActor.SetAssetPath("");
                InitializeMaterial(*m_actorAsset.GetAs<ActorAsset>());
            }

            // Update the rendernode and the property grid
            OnMaterialPerActorChanged();
            AzToolsFramework::ToolsApplicationEvents::Bus::Broadcast(
                &AzToolsFramework::ToolsApplicationEvents::InvalidatePropertyDisplay,
                AzToolsFramework::Refresh_AttributesAndValues);
        }

        //////////////////////////////////////////////////////////////////////////
        _smart_ptr<IMaterial> EditorActorComponent::GetMaterial()
        {
            if (m_renderNode)
            {
                return m_renderNode->GetMaterial();
            }

            return nullptr;
        }

        //////////////////////////////////////////////////////////////////////////
        AZ::Aabb EditorActorComponent::GetWorldBounds()
        {
            if (m_renderNode)
            {
                const AABB bbox = m_renderNode->GetBBox();
                return AZ::Aabb::CreateFromMinMax(AZ::Vector3(bbox.min.x, bbox.min.y, bbox.min.z), AZ::Vector3(bbox.max.x, bbox.max.y, bbox.max.z));
            }
            else
            {
                return AZ::Aabb::CreateNull();
            }
        }

        //////////////////////////////////////////////////////////////////////////
        AZ::Aabb EditorActorComponent::GetLocalBounds()
        {
            if (m_renderNode)
            {
                AABB bbox;
                m_renderNode->GetLocalBounds(bbox);
                return AZ::Aabb::CreateFromMinMax(AZ::Vector3(bbox.min.x, bbox.min.y, bbox.min.z), AZ::Vector3(bbox.max.x, bbox.max.y, bbox.max.z));
            }
            else
            {
                return AZ::Aabb::CreateNull();
            }
        }

        //////////////////////////////////////////////////////////////////////////
        bool EditorActorComponent::GetVisibility()
        {
            return true;
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::SetVisibility(bool isVisible)
        {
            (void)isVisible;
        }

        //////////////////////////////////////////////////////////////////////////
        void EditorActorComponent::BuildGameEntity(AZ::Entity* gameEntity)
        {
            ActorComponent::Configuration cfg;
            cfg.m_actorAsset = m_actorAsset;
            cfg.m_materialPerLOD = m_materialPerLOD;
            cfg.m_renderSkeleton = m_renderSkeleton;
            cfg.m_renderCharacter = m_renderCharacter;
            cfg.m_renderBounds = m_renderBounds;
            cfg.m_attachmentType = m_attachmentType;
            cfg.m_attachmentTarget = m_attachmentTarget;
            cfg.m_attachmentJointIndex = m_attachmentJointIndex;
            cfg.m_lodLevel = m_lodLevel;
            cfg.m_skinningMethod = m_skinningMethod;

            gameEntity->AddComponent(aznew ActorComponent(&cfg));
        }

        //////////////////////////////////////////////////////////////////////////
        IRenderNode* EditorActorComponent::GetRenderNode()
        {
            return m_renderNode.get();
        }

        //////////////////////////////////////////////////////////////////////////
        const float EditorActorComponent::s_renderNodeRequestBusOrder = 100.f;
        float EditorActorComponent::GetRenderNodeRequestBusOrder() const
        {
            return s_renderNodeRequestBusOrder;
        }
        AZ::EntityId EditorActorComponent::GetAttachedToEntityId() const
        {
            return m_attachmentTarget;
        }


        AZ::Aabb EditorActorComponent::GetEditorSelectionBoundsViewport(
            const AzFramework::ViewportInfo& /*viewportInfo*/)
        {
            return GetWorldBounds();
        }

        bool EditorActorComponent::EditorSelectionIntersectRayViewport(
            const AzFramework::ViewportInfo& /*viewportInfo*/,
            const AZ::Vector3& src, const AZ::Vector3& dir, AZ::VectorFloat& distance)
        {
            if (!m_actorAsset.Get() || !m_actorAsset.Get()->GetActor() || !m_actorInstance || !m_actorInstance->GetTransformData() || !m_renderCharacter)
            {
                return false;
            }

            distance = std::numeric_limits<float>::max();
            bool isHit = false;

            // Get the MCore::Ray used by EMotionFX::Mesh::Intersects
            // Convert the input source + direction to make a line segment, since that's the format that is used for an MCore::Ray
            AZ::Vector3 dest = src + dir;
            MCore::Ray ray(src, dest);

            // Update the mesh deformers so the intersection test will hit the actor if it is being
            // animated by a motion component that is previewing the animation in the editor
            m_actorInstance->UpdateMeshDeformers(0.0f, true);

            // Get the MCore::Matrix used by EmotionFX::Mesh::Intersects
            const EMotionFX::TransformData* transformData = m_actorInstance->GetTransformData();
            const Pose* currentPose = transformData->GetCurrentPose();

            // Iterate through the meshes in the actor, looking for the closest hit
            EMotionFX::Actor* actor = m_actorAsset.Get()->GetActor().get();
            const uint32 numNodes = actor->GetNumNodes();
            const uint32 numLods = actor->GetNumLODLevels();
            for (uint32 lod = 0; lod < numLods; ++lod)
            {
                for (uint32 nodeIndex = 0; nodeIndex < numNodes; ++nodeIndex)
                {
                    EMotionFX::Mesh* mesh = actor->GetMesh(lod, nodeIndex);
                    if (!mesh || mesh->GetIsCollisionMesh())
                    {
                        continue;
                    }

                    AZ::Vector3 hitPoint;
                    if (mesh->Intersects(currentPose->GetWorldSpaceTransform(nodeIndex), ray, &hitPoint))
                    {
                        isHit = true;
                        float hitDistance = (src - hitPoint).GetLength();
                        if (hitDistance < distance)
                        {
                            distance = hitDistance;
                        }
                    }
                }
            }

            return isHit;
        }
        //////////////////////////////////////////////////////////////////////////
        // Check if the given attachment is valid.
        bool EditorActorComponent::IsValidAttachment(const AZ::EntityId& attachment, const AZ::EntityId& attachTo) const
        {
            // Cannot attach to yourself.
            if (attachment == attachTo)
            {
                return false;
            }

            // Walk our way up to the root.
            AZ::EntityId resultId;
            EditorActorComponentRequestBus::EventResult(resultId, attachTo, &EditorActorComponentRequestBus::Events::GetAttachedToEntityId);
            while (resultId.IsValid())
            {
                AZ::EntityId localResult;
                EditorActorComponentRequestBus::EventResult(localResult, resultId, &EditorActorComponentRequestBus::Events::GetAttachedToEntityId);

                // We detected a loop.
                if (localResult == attachment)
                {
                    return false;
                }

                resultId = localResult;
            }

            return true;
        }



        // The entity has attached to the target.
        void EditorActorComponent::OnAttached(AZ::EntityId targetId)
        {
            const AZ::EntityId* busIdPtr = LmbrCentral::AttachmentComponentNotificationBus::GetCurrentBusId();
            if (busIdPtr)
            {
                const auto result = AZStd::find(m_attachments.begin(), m_attachments.end(), *busIdPtr);
                if (result == m_attachments.end())
                {
                    m_attachments.emplace_back(*busIdPtr);
                }
            }

            if (!m_actorInstance)
            {
                return;
            }

            EMotionFX::ActorInstance* targetActorInstance = nullptr;
            ActorComponentRequestBus::EventResult(targetActorInstance, targetId, &ActorComponentRequestBus::Events::GetActorInstance);

            const char* jointName = nullptr;
            LmbrCentral::AttachmentComponentRequestBus::EventResult(jointName, GetEntityId(), &LmbrCentral::AttachmentComponentRequestBus::Events::GetJointName);
            if (targetActorInstance)
            {
                EMotionFX::Node* node = jointName ? targetActorInstance->GetActor()->GetSkeleton()->FindNodeByName(jointName) : targetActorInstance->GetActor()->GetSkeleton()->GetNode(0);
                if (node)
                {
                    const AZ::u32 jointIndex = node->GetNodeIndex();
                    EMotionFX::Attachment* attachment = EMotionFX::AttachmentNode::Create(targetActorInstance, jointIndex, m_actorInstance.get(), true /* Managed externally, by this component. */);
                    targetActorInstance->AddAttachment(attachment);
                }
            }
        }


        // The entity is detaching from the target.
        void EditorActorComponent::OnDetached(AZ::EntityId targetId)
        {
            // Remove the targetId from the attachment list
            const AZ::EntityId* busIdPtr = LmbrCentral::AttachmentComponentNotificationBus::GetCurrentBusId();
            if (busIdPtr)
            {
                m_attachments.erase(AZStd::remove(m_attachments.begin(), m_attachments.end(), *busIdPtr), m_attachments.end());
            }

            if (!m_actorInstance)
            {
                return;
            }

            EMotionFX::ActorInstance* targetActorInstance = nullptr;
            ActorComponentRequestBus::EventResult(targetActorInstance, targetId, &ActorComponentRequestBus::Events::GetActorInstance);
            if (targetActorInstance)
            {
                targetActorInstance->RemoveAttachment(m_actorInstance.get());
            }
        }
    } //namespace Integration
} // namespace EMotionFX

