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

#include "stdafx.h"

// All plugins suffer from the following warning:
//  warning C4273: 'GetIEditor' : inconsistent dll linkage
// GetIEditor() is forward-declared using EDITOR_CORE_API, which without EDITOR_CORE set,
// results in dllimport rather than dllexport. This define ensure it's consistently and
// properly defined for export.
#define EDITOR_CORE

#include <platform.h>
#include <platform_impl.h>

#include <IEditor.h>
#include <Include/IPlugin.h>
#include <Include/IEditorClassFactory.h>
#include "ComponentEntityEditorPlugin.h"

#if AZ_TRAIT_OS_PLATFORM_APPLE
typedef HANDLE HINSTANCE;
#define DLL_PROCESS_ATTACH 1
#endif

IEditor* g_pEditor = nullptr;

//------------------------------------------------------------------
PLUGIN_API IPlugin* CreatePluginInstance(PLUGIN_INIT_PARAM* pInitParam)
{
    g_pEditor = pInitParam->pIEditorInterface;
    ISystem* pSystem = pInitParam->pIEditorInterface->GetSystem();
    ModuleInitISystem(pSystem, "ComponentEntityEditorPlugin");
    return new ComponentEntityEditorPlugin(g_pEditor);
}

//------------------------------------------------------------------
HINSTANCE g_hInstance = 0;
BOOL __stdcall DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH)
    {
        g_hInstance = hinstDLL;
    }

    return TRUE;
}
