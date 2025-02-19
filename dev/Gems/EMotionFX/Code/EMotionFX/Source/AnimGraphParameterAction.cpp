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

#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <EMotionFX/Source/AnimGraph.h>
#include <EMotionFX/Source/AnimGraphBus.h>
#include <EMotionFX/Source/AnimGraphInstance.h>
#include <EMotionFX/Source/AnimGraphManager.h>
#include <EMotionFX/Source/AnimGraphParameterAction.h>
#include <EMotionFX/Source/EMotionFXConfig.h>
#include <MCore/Source/AttributeBool.h>
#include <MCore/Source/AttributeFloat.h>

namespace EMotionFX
{
    AZ_CLASS_ALLOCATOR_IMPL(AnimGraphParameterAction, AnimGraphAllocator, 0)

    AnimGraphParameterAction::AnimGraphParameterAction()
        : AnimGraphTriggerAction()
        , m_parameterIndex(AZ::Failure())
        , m_valueParameter(nullptr)
        , m_triggerValue(0.0f)
    {
    }


    AnimGraphParameterAction::AnimGraphParameterAction(AnimGraph* animGraph)
        : AnimGraphParameterAction()
    {
        InitAfterLoading(animGraph);
    }


    AnimGraphParameterAction::~AnimGraphParameterAction()
    {

    }


    void AnimGraphParameterAction::Reinit()
    {
        // Find the parameter index for the given parameter name, to prevent string based lookups every frame
        m_parameterIndex = mAnimGraph->FindValueParameterIndexByName(m_parameterName);
        if (m_parameterIndex.IsSuccess())
        {
            m_valueParameter = mAnimGraph->FindValueParameter(m_parameterIndex.GetValue());
        }
        else
        {
            m_valueParameter = nullptr;
        }
    }


    bool AnimGraphParameterAction::InitAfterLoading(AnimGraph* animGraph)
    {
        if (!AnimGraphTriggerAction::InitAfterLoading(animGraph))
        {
            return false;
        }

        InitInternalAttributesForAllInstances();

        Reinit();
        return true;
    }


    const char* AnimGraphParameterAction::GetPaletteName() const
    {
        return "Parameter Action";
    }


    void AnimGraphParameterAction::TriggerAction(AnimGraphInstance* animGraphInstance) const
    {
        {
            MCore::Attribute* attribute = animGraphInstance->GetParameterValue(static_cast<uint32>(m_parameterIndex.GetValue()));
            if (attribute)
            {
                switch (attribute->GetType())
                {
                    case MCore::AttributeBool::TYPE_ID:
                    {
                        MCore::AttributeBool* atrBool = static_cast<MCore::AttributeBool*>(attribute);
                        atrBool->SetValue(m_triggerValue != 0.0f);
                        break;
                    }
                    case MCore::AttributeFloat::TYPE_ID:
                    {
                        MCore::AttributeFloat* atrFloat = static_cast<MCore::AttributeFloat*>(attribute);
                        atrFloat->SetValue(m_triggerValue);
                        break;
                    }
                    default:
                    {
                        AZ_Assert(false, "Type %d of attribute %s are not supported", attribute->GetType(), m_parameterName.c_str());
                        break;
                    }
                }

                AnimGraphNotificationBus::Broadcast(&AnimGraphNotificationBus::Events::OnParameterActionTriggered, m_valueParameter);
            }
        }
    }


    void AnimGraphParameterAction::SetParameterName(const AZStd::string& parameterName)
    {
        m_parameterName = parameterName;
        if (mAnimGraph)
        {
            Reinit();
        }
    }


    const AZStd::string& AnimGraphParameterAction::GetParameterName() const
    {
        return m_parameterName;
    }


    AZ::TypeId AnimGraphParameterAction::GetParameterType() const
    {
        // if there is no parameter selected yet, return invalid type
        if (m_parameterIndex.IsSuccess())
        {
            // get access to the parameter info and return the type of its default value
            const ValueParameter* valueParameter = mAnimGraph->FindValueParameter(m_parameterIndex.GetValue());
            return azrtti_typeid(valueParameter);
        }
        else
        {
            return AZ::TypeId();
        }
    }


    // construct and output the information summary string for this object
    void AnimGraphParameterAction::GetSummary(AZStd::string* outResult) const
    {
        *outResult = AZStd::string::format("%s: Parameter Name='%s", RTTI_GetTypeName(), m_parameterName.c_str());
    }


    // construct and output the tooltip for this object
    void AnimGraphParameterAction::GetTooltip(AZStd::string* outResult) const
    {
        AZStd::string columnName, columnValue;

        // add the action type
        columnName = "Action Type: ";
        columnValue = RTTI_GetTypeName();
        *outResult = AZStd::string::format("<table border=\"0\"><tr><td width=\"120\"><b>%s</b></td><td><nobr>%s</nobr></td>", columnName.c_str(), columnValue.c_str());

        // add the parameter
        columnName = "Parameter Name: ";
        *outResult += AZStd::string::format("</tr><tr><td><b><nobr>%s</nobr></b></td><td><nobr>%s</nobr></td>", columnName.c_str(), m_parameterName.c_str());
    }


    void AnimGraphParameterAction::Reflect(AZ::ReflectContext* context)
    {
        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context);
        if (!serializeContext)
        {
            return;
        }

        serializeContext->Class<AnimGraphParameterAction, AnimGraphTriggerAction>()
            ->Version(1)
            ->Field("parameterName", &AnimGraphParameterAction::m_parameterName)
            ->Field("triggerValue", &AnimGraphParameterAction::m_triggerValue)
            ;

        AZ::EditContext* editContext = serializeContext->GetEditContext();
        if (!editContext)
        {
            return;
        }

        editContext->Class<AnimGraphParameterAction>("Parameter Action", "Parameter action attributes")
            ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                ->Attribute(AZ::Edit::Attributes::AutoExpand, "")
                ->Attribute(AZ::Edit::Attributes::Visibility, AZ::Edit::PropertyVisibility::ShowChildrenOnly)
            ->DataElement(AZ_CRC("AnimGraphParameter", 0x778af55a), &AnimGraphParameterAction::m_parameterName, "Parameter", "The parameter name to apply the action on.")
                ->Attribute(AZ::Edit::Attributes::ChangeNotify, &AnimGraphParameterAction::Reinit)
                ->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
                ->Attribute(AZ_CRC("AnimGraph", 0x0d53d4b3), &AnimGraphParameterAction::GetAnimGraph)
            ->DataElement(AZ::Edit::UIHandlers::ComboBox, &AnimGraphParameterAction::m_triggerValue, "Trigger Value", "The value that the parameter will be override to.")
            ->Attribute(AZ::Edit::Attributes::ChangeNotify, &AnimGraphParameterAction::Reinit)
            ->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
            ;
    }
} // namespace EMotionFX
