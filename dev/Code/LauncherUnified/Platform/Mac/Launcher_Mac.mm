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
#include <Launcher_precompiled.h>
#include <Launcher.h>

#include <LumberyardApplication_Mac.h>
#include <../Common/Apple/Launcher_Apple.h>
#include <../Common/UnixLike/Launcher_UnixLike.h>


int main(int argc, char* argv[])
{
    // Ensure the process is a foreground application. Must be done before creating the application.
    ProcessSerialNumber processSerialNumber = { 0, kCurrentProcess };
    TransformProcessType(&processSerialNumber, kProcessTransformToForegroundApplication);

    // Create a memory pool, a custom AppKit application, and a custom AppKit application delegate.
    NSAutoreleasePool* autoreleasePool = [[NSAutoreleasePool alloc] init];
    [LumberyardApplication_Mac sharedApplication];
    [NSApp setDelegate: [[LumberyardApplicationDelegate_Mac alloc] init]];

    // Register some default application behaviours
    [[NSUserDefaults standardUserDefaults] registerDefaults:
        [[NSDictionary alloc] initWithObjectsAndKeys:
            [NSNumber numberWithBool: FALSE], @"AppleMomentumScrollSupported",
            [NSNumber numberWithBool: FALSE], @"ApplePressAndHoldEnabled",
            nil]];

    // Launch the AppKit application and release the memory pool.
    [NSApp finishLaunching];
    [autoreleasePool release];

    // run the Lumberyard application
    using namespace LumberyardLauncher;

    PlatformMainInfo mainInfo;
    mainInfo.m_updateResourceLimits = IncreaseResourceLimits;
    mainInfo.m_appResourcesPath = GetAppResourcePath();

    bool ret = mainInfo.CopyCommandLine(argc, argv);

    ReturnCode status = ret ? 
        Run(mainInfo) : 
        ReturnCode::ErrCommandLine;

    return static_cast<int>(status);
}
