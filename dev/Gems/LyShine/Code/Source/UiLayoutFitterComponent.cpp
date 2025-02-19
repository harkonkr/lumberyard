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
#include "LyShine_precompiled.h"
#include "UiLayoutFitterComponent.h"
#include "UiLayoutHelpers.h"

#include <LyShine/Bus/UiLayoutCellBus.h>
#include <LyShine/Bus/UiLayoutCellDefaultBus.h>
#include <LyShine/Bus/UiTransformBus.h>
#include <LyShine/Bus/UiTransform2dBus.h>
#include <LyShine/Bus/UiEditorChangeNotificationBus.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
UiLayoutFitterComponent::UiLayoutFitterComponent()
    : m_horizontalFit(false)
    , m_verticalFit(false)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
UiLayoutFitterComponent::~UiLayoutFitterComponent()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void UiLayoutFitterComponent::ApplyLayoutWidth()
{
    if (m_horizontalFit)
    {
        float targetWidth = LyShine::UiLayoutCellUnspecifiedSize;

        // First check for overriden cell width
        EBUS_EVENT_ID_RESULT(targetWidth, GetEntityId(), UiLayoutCellBus, GetTargetWidth);

        // Check if there is a max width
        float maxWidth = LyShine::UiLayoutCellUnspecifiedSize;
        EBUS_EVENT_ID_RESULT(maxWidth, GetEntityId(), UiLayoutCellBus, GetMaxWidth);

        // If not overriden, get the default cell width
        if (!LyShine::IsUiLayoutCellSizeSpecified(targetWidth))
        {
            targetWidth = 0.0f;

            AZ::EBusAggregateResults<float> results;
            EBUS_EVENT_ID_RESULT(results, GetEntityId(), UiLayoutCellDefaultBus, GetTargetWidth, maxWidth);

            if (!results.values.empty())
            {
                for (float value : results.values)
                {
                    if (targetWidth < value)
                    {
                        targetWidth = value;
                    }
                }
            }
        }

        // Make sure target width does not exceed max width
        if (LyShine::IsUiLayoutCellSizeSpecified(maxWidth) && maxWidth < targetWidth)
        {
            targetWidth = maxWidth;
        }

        // Recalculate the new horizontal offsets using the pivot
        UiTransform2dInterface::Offsets offsets;
        EBUS_EVENT_ID_RESULT(offsets, GetEntityId(), UiTransform2dBus, GetOffsets);
        UiTransform2dInterface::Anchors anchors;
        EBUS_EVENT_ID_RESULT(anchors, GetEntityId(), UiTransform2dBus, GetAnchors);

        // If anchors are separate
        if (anchors.m_left != anchors.m_right)
        {
            // Bring them back together in their midpoint
            float midPoint = (anchors.m_left + anchors.m_right) / 2.0f;
            anchors.m_left = anchors.m_right = midPoint;

            EBUS_EVENT_ID(GetEntityId(), UiTransform2dBus, SetAnchors, anchors, false, true);
        }

        float oldWidth = offsets.m_right - offsets.m_left;
        float widthDiff = targetWidth - oldWidth;

        if (widthDiff != 0.0f)
        {
            AZ::Vector2 pivot;
            EBUS_EVENT_ID_RESULT(pivot, GetEntityId(), UiTransformBus, GetPivot);

            offsets.m_left -= widthDiff * pivot.GetX();
            offsets.m_right += widthDiff * (1.0f - pivot.GetX());

            EBUS_EVENT_ID(GetEntityId(), UiTransform2dBus, SetOffsets, offsets);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void UiLayoutFitterComponent::ApplyLayoutHeight()
{
    if (m_verticalFit)
    {
        float targetHeight = LyShine::UiLayoutCellUnspecifiedSize;

        // First check for overriden cell height
        EBUS_EVENT_ID_RESULT(targetHeight, GetEntityId(), UiLayoutCellBus, GetTargetHeight);

        // Check if there is a max height
        float maxHeight = LyShine::UiLayoutCellUnspecifiedSize;
        EBUS_EVENT_ID_RESULT(maxHeight, GetEntityId(), UiLayoutCellBus, GetMaxHeight);

        // If not overriden, get the default cell height
        if (!LyShine::IsUiLayoutCellSizeSpecified(targetHeight))
        {
            targetHeight = 0.0f;

            AZ::EBusAggregateResults<float> results;
            EBUS_EVENT_ID_RESULT(results, GetEntityId(), UiLayoutCellDefaultBus, GetTargetHeight, maxHeight);

            if (!results.values.empty())
            {
                for (float value : results.values)
                {
                    if (targetHeight < value)
                    {
                        targetHeight = value;
                    }
                }
            }
        }

        // Make sure target height does not exceed max height
        if (LyShine::IsUiLayoutCellSizeSpecified(maxHeight) && maxHeight < targetHeight)
        {
            targetHeight = maxHeight;
        }

        // Recalculate the new vertical offsets using the pivot
        UiTransform2dInterface::Offsets offsets;
        EBUS_EVENT_ID_RESULT(offsets, GetEntityId(), UiTransform2dBus, GetOffsets);
        UiTransform2dInterface::Anchors anchors;
        EBUS_EVENT_ID_RESULT(anchors, GetEntityId(), UiTransform2dBus, GetAnchors);

        // If anchors are separate
        if (anchors.m_top != anchors.m_bottom)
        {
            // Bring them back together in their midpoint
            float midPoint = (anchors.m_top + anchors.m_bottom) / 2.0f;
            anchors.m_top = anchors.m_bottom = midPoint;

            EBUS_EVENT_ID(GetEntityId(), UiTransform2dBus, SetAnchors, anchors, false, true);
        }

        float oldHeight = offsets.m_bottom - offsets.m_top;
        float heightDiff = targetHeight - oldHeight;

        if (heightDiff != 0.0f)
        {
            AZ::Vector2 pivot;
            EBUS_EVENT_ID_RESULT(pivot, GetEntityId(), UiTransformBus, GetPivot);

            offsets.m_top -= heightDiff * pivot.GetY();
            offsets.m_bottom += heightDiff * (1.0f - pivot.GetY());

            EBUS_EVENT_ID(GetEntityId(), UiTransform2dBus, SetOffsets, offsets);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool UiLayoutFitterComponent::GetHorizontalFit()
{
    return m_horizontalFit;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void UiLayoutFitterComponent::SetHorizontalFit(bool horizontalFit)
{
    m_horizontalFit = horizontalFit;

    CheckFitterAndInvalidateLayout();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool UiLayoutFitterComponent::GetVerticalFit()
{
    return m_verticalFit;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void UiLayoutFitterComponent::SetVerticalFit(bool verticalFit)
{
    m_verticalFit = verticalFit;

    CheckFitterAndInvalidateLayout();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
UiLayoutFitterInterface::FitType UiLayoutFitterComponent::GetFitType()
{
    if (m_horizontalFit && m_verticalFit)
    {
        return UiLayoutFitterInterface::FitType::HorizontalAndVertical;
    }
    else if (m_horizontalFit)
    {
        return UiLayoutFitterInterface::FitType::HorizontalOnly;
    }
    else if (m_verticalFit)
    {
        return UiLayoutFitterInterface::FitType::VerticalOnly;
    }
    else
    {
        return UiLayoutFitterInterface::FitType::None;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC STATIC MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////

void UiLayoutFitterComponent::Reflect(AZ::ReflectContext* context)
{
    AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);

    if (serializeContext)
    {
        serializeContext->Class<UiLayoutFitterComponent, AZ::Component>()
            ->Version(0)
            ->Field("HorizontalFit", &UiLayoutFitterComponent::m_horizontalFit)
            ->Field("VerticalFit", &UiLayoutFitterComponent::m_verticalFit);

        AZ::EditContext* ec = serializeContext->GetEditContext();
        if (ec)
        {
            auto editInfo = ec->Class<UiLayoutFitterComponent>("LayoutFitter", "A component that resizes its element to its content.");

            editInfo->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::Category, "UI")
                ->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/UiLayoutFitter.png")
                ->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/UiLayoutFitter.png")
                ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("UI", 0x27ff46b0))
                ->Attribute(AZ::Edit::Attributes::AutoExpand, true);

            editInfo->DataElement(AZ::Edit::UIHandlers::CheckBox, &UiLayoutFitterComponent::m_horizontalFit, "Horizontal Fit",
                "When checked, this element will be resized according to the target width of its content.")
                ->Attribute(AZ::Edit::Attributes::ChangeNotify, &UiLayoutFitterComponent::CheckFitterAndInvalidateLayout)
                ->Attribute(AZ::Edit::Attributes::ChangeNotify, &UiLayoutFitterComponent::RefreshEditorTransformProperties);

            editInfo->DataElement(AZ::Edit::UIHandlers::CheckBox, &UiLayoutFitterComponent::m_verticalFit, "Vertical Fit",
                "When checked, this element will be resized according to the target height of its content.")
                ->Attribute(AZ::Edit::Attributes::ChangeNotify, &UiLayoutFitterComponent::CheckFitterAndInvalidateLayout)
                ->Attribute(AZ::Edit::Attributes::ChangeNotify, &UiLayoutFitterComponent::RefreshEditorTransformProperties);
        }
    }

    AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context);
    if (behaviorContext)
    {
        behaviorContext->EBus<UiLayoutFitterBus>("UiLayoutFitterBus")
            ->Event("GetHorizontalFit", &UiLayoutFitterBus::Events::GetHorizontalFit)
            ->Event("SetHorizontalFit", &UiLayoutFitterBus::Events::SetHorizontalFit)
            ->Event("GetVerticalFit", &UiLayoutFitterBus::Events::GetVerticalFit)
            ->Event("SetVerticalFit", &UiLayoutFitterBus::Events::SetVerticalFit);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// PROTECTED MEMBER FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////
void UiLayoutFitterComponent::Activate()
{
    UiLayoutControllerBus::Handler::BusConnect(m_entity->GetId());
    UiLayoutFitterBus::Handler::BusConnect(m_entity->GetId());

    // If this is the first time the entity has been activated this has no effect since the canvas
    // is not known. But if a LayoutFitter component has just been pasted onto an existing entity
    // we need to invalidate the layout in case that affects things.
    CheckFitterAndInvalidateLayout();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void UiLayoutFitterComponent::Deactivate()
{
    UiLayoutControllerBus::Handler::BusDisconnect();
    UiLayoutFitterBus::Handler::BusDisconnect();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int UiLayoutFitterComponent::GetPriority() const
{
    // Priority should be lower (called earlier) than components that modify their children size
    // Default prioritiy is 100
    return 50;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void UiLayoutFitterComponent::CheckFitterAndInvalidateLayout()
{
    if (m_horizontalFit || m_verticalFit)
    {
        UiLayoutHelpers::InvalidateLayout(GetEntityId());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void UiLayoutFitterComponent::RefreshEditorTransformProperties()
{
    EBUS_EVENT(UiEditorChangeNotificationBus, OnEditorTransformPropertiesNeedRefresh);
}
