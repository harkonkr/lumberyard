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

#include "LmbrCentral_precompiled.h"
#include "EditorPolygonPrismShapeComponent.h"
#include "EditorPolygonPrismShapeComponentMode.h"
#include "PolygonPrismShapeComponent.h"

#include <AzCore/RTTI/ReflectContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzToolsFramework/Entity/EditorEntityInfoBus.h>
#include <AzToolsFramework/Viewport/VertexContainerDisplay.h>

#include "EditorShapeComponentConverters.h"
#include "ShapeDisplay.h"

namespace LmbrCentral
{
    void EditorPolygonPrismShapeComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        EditorBaseShapeComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC("PolygonPrismShapeService", 0x1cbc4ed4));
        provided.push_back(AZ_CRC("VariableVertexContainerService", 0x70c58740));
        provided.push_back(AZ_CRC("FixedVertexContainerService", 0x83f1bbf2));
    }

    void EditorPolygonPrismShapeComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        EditorBaseShapeComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC("VariableVertexContainerService", 0x70c58740));
        incompatible.push_back(AZ_CRC("FixedVertexContainerService", 0x83f1bbf2));
    }

    void EditorPolygonPrismShapeComponent::Init()
    {
        EditorBaseShapeComponent::Init();

        SetShapeComponentConfig(&m_polygonShapeConfig);
    }

    void EditorPolygonPrismShapeComponent::Activate()
    {
        EditorBaseShapeComponent::Activate();

        AzFramework::EntityDebugDisplayEventBus::Handler::BusConnect(GetEntityId());
        EditorPolygonPrismShapeComponentRequestsBus::Handler::BusConnect(GetEntityId());

        m_polygonPrismShape.Activate(GetEntityId());

        bool selected = false;
        AzToolsFramework::EditorEntityInfoRequestBus::EventResult(
            selected, GetEntityId(), &AzToolsFramework::EditorEntityInfoRequestBus::Events::IsSelected);

        // placeholder - create initial polygon prism shape if empty
        AZ::VertexContainer<AZ::Vector2>& vertexContainer = m_polygonPrismShape.GetPolygonPrism()->m_vertexContainer;
        if (selected && vertexContainer.Empty())
        {
            vertexContainer.AddVertex(AZ::Vector2(-2.0f, -2.0f));
            vertexContainer.AddVertex(AZ::Vector2(2.0f, -2.0f));
            vertexContainer.AddVertex(AZ::Vector2(2.0f, 2.0f));
            vertexContainer.AddVertex(AZ::Vector2(-2.0f, 2.0f));
        }

        const auto shapeModified = [this]()
        {
            GenerateVertices();
            m_polygonPrismShape.ShapeChanged();
        };

        const auto vertexAdded = [this, shapeModified](size_t index)
        {
            PolygonPrismShapeComponentNotificationBus::Event(
                GetEntityId(), &PolygonPrismShapeComponentNotification::OnVertexAdded, index);

            shapeModified();
        };

        const auto vertexRemoved = [this, shapeModified](size_t index)
        {
            PolygonPrismShapeComponentNotificationBus::Event(
                GetEntityId(), &PolygonPrismShapeComponentNotification::OnVertexRemoved, index);

            shapeModified();
        };

        const auto vertexUpdated = [this, shapeModified](size_t index)
        {
            PolygonPrismShapeComponentNotificationBus::Event(
                GetEntityId(), &PolygonPrismShapeComponentNotification::OnVertexUpdated, index);

            shapeModified();
        };

        const auto verticesSet = [this, shapeModified]()
        {
            PolygonPrismShapeComponentNotificationBus::Event(
                GetEntityId(), &PolygonPrismShapeComponentNotification::OnVerticesSet,
                m_polygonPrismShape.GetPolygonPrism()->m_vertexContainer.GetVertices());

            shapeModified();
        };

        const auto verticesCleared = [this, shapeModified]()
        {
            PolygonPrismShapeComponentNotificationBus::Event(
                GetEntityId(), &PolygonPrismShapeComponentNotification::OnVerticesCleared);

            shapeModified();
        };

        m_polygonPrismShape.GetPolygonPrism()->SetCallbacks(
            vertexAdded,
            vertexRemoved,
            vertexUpdated,
            verticesSet,
            verticesCleared,
            shapeModified);

        GenerateVertices();

        // ComponentMode
        m_componentModeDelegate.ConnectWithSingleComponentMode<
            EditorPolygonPrismShapeComponent, EditorPolygonPrismShapeComponentMode>(
                AZ::EntityComponentIdPair(GetEntityId(), GetId()), this);
    }

    void EditorPolygonPrismShapeComponent::Deactivate()
    {
        EditorBaseShapeComponent::Deactivate();

        m_componentModeDelegate.Disconnect();

        EditorPolygonPrismShapeComponentRequestsBus::Handler::BusDisconnect();
        AzFramework::EntityDebugDisplayEventBus::Handler::BusDisconnect();

        m_polygonPrismShape.Deactivate();
    }

    void EditorPolygonPrismShapeComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<EditorPolygonPrismShapeComponent, EditorBaseShapeComponent>()
                ->Version(3, &ClassConverters::UpgradeEditorPolygonPrismShapeComponent)
                ->Field("Configuration", &EditorPolygonPrismShapeComponent::m_polygonPrismShape)
                ->Field("ShapeConfiguration", &EditorPolygonPrismShapeComponent::m_polygonShapeConfig)
                ->Field("ComponentMode", &EditorPolygonPrismShapeComponent::m_componentModeDelegate)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<EditorPolygonPrismShapeComponent>(
                    "Polygon Prism Shape", "Provides polygon prism shape")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Category, "Shape")
                        ->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/PolygonPrism.svg")
                        ->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/PolygonPrism.png")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
                        ->Attribute(AZ::Edit::Attributes::HelpPageURL, "http://docs.aws.amazon.com/console/lumberyard/userguide/polygon-prism-component")
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &EditorPolygonPrismShapeComponent::m_polygonPrismShape, "Configuration", "PolygonPrism Shape Configuration")
                        // ->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly) // disabled - prevents ChangeNotify attribute firing correctly
                    ->DataElement(AZ::Edit::UIHandlers::Default, &EditorPolygonPrismShapeComponent::m_componentModeDelegate, "Component Mode", "PolygonPrism Component Mode")
                        ->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly)
                        ;
            }
        }
    }

    void EditorPolygonPrismShapeComponent::DisplayEntityViewport(
        const AzFramework::ViewportInfo& viewportInfo,
        AzFramework::DebugDisplayRequests& debugDisplay)
    {
        DisplayShape(
            debugDisplay, [this]() { return CanDraw(); },
            [this](AzFramework::DebugDisplayRequests& debugDisplay)
            {
                DrawPolygonPrismShape(
                    { m_shapeColor, m_shapeWireColor, m_displayFilled },
                    m_polygonPrismMesh, debugDisplay);

                debugDisplay.SetColor(m_shapeWireColor);

                if (m_componentModeDelegate.AddedToComponentMode())
                {
                    AzToolsFramework::VertexContainerDisplay::DisplayVertexContainerIndices(
                        debugDisplay, AzToolsFramework::VariableVerticesVertexContainer<AZ::Vector2>(
                            m_polygonPrismShape.GetPolygonPrism()->m_vertexContainer),
                        AzToolsFramework::TransformUniformScale(m_polygonPrismShape.GetCurrentTransform()),
                        IsSelected());
                }
            },
            m_polygonPrismShape.GetCurrentTransform());
    }

    void EditorPolygonPrismShapeComponent::BuildGameEntity(AZ::Entity* gameEntity)
    {
        if (auto component = gameEntity->CreateComponent<PolygonPrismShapeComponent>())
        {
            const bool isActive = GetEntity() ? (GetEntity()->GetState() == AZ::Entity::ES_ACTIVE) : false;
            if (isActive)
            {
                m_polygonPrismShape.Deactivate();
            }

            component->m_polygonPrismShape = m_polygonPrismShape;

            if (isActive)
            {
                m_polygonPrismShape.Activate(GetEntityId());
            }
        }

        if (m_visibleInGameView)
        {
            PolygonPrismShapeDebugDisplayComponent* debugDisplayComponent = gameEntity->CreateComponent<PolygonPrismShapeDebugDisplayComponent>(*m_polygonPrismShape.GetPolygonPrism());
            debugDisplayComponent->SetShapeConfig(m_polygonShapeConfig);
        }
    }

    void EditorPolygonPrismShapeComponent::GenerateVertices()
    {
        GeneratePolygonPrismMesh(
            m_polygonPrismShape.GetPolygonPrism()->m_vertexContainer.GetVertices(),
            m_polygonPrismShape.GetPolygonPrism()->GetHeight(), m_polygonPrismMesh);
    }
} // namespace LmbrCentral