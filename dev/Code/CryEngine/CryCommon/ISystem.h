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

// In Mac, including ISystem without including platform.h first fails because platform.h
// includes CryThread.h which includes CryThread_pthreads.h which uses ISystem (gEnv).
// So plaform.h needs the contents of ISystem.h.
// By including platform.h outside of the guard, we give platform.h the right include order
#include <platform.h> // Needed for LARGE_INTEGER (for consoles).

#ifndef CRYINCLUDE_CRYCOMMON_ISYSTEM_H
#define CRYINCLUDE_CRYCOMMON_ISYSTEM_H
#pragma once

#ifdef CRYSYSTEM_EXPORTS
#define CRYSYSTEM_API DLL_EXPORT
#else
#define CRYSYSTEM_API DLL_IMPORT
#endif

#include "CryAssert.h"
#include "CompileTimeAssert.h"

#include <AzCore/IO/SystemFile.h>

#if defined(AZ_RESTRICTED_PLATFORM)
#undef AZ_RESTRICTED_SECTION
#define ISYSTEM_H_SECTION_1 1
#define ISYSTEM_H_SECTION_2 2
#define ISYSTEM_H_SECTION_3 3
#define ISYSTEM_H_SECTION_4 4
#define ISYSTEM_H_SECTION_5 5
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////////////////////
#include <IXml.h> // <> required for Interfuscator
#include "IValidator.h" // <> required for Interfuscator
#include <ILog.h> // <> required for Interfuscator
#include "CryVersion.h"
#include "smartptr.h"
#include <IMemory.h> // <> required for Interfuscator
#include <ISystemScheduler.h> // <> required for Interfuscator
#include <memory> // shared_ptr
#include <IFilePathManager.h>
#include <CrySystemBus.h>

struct ISystem;
struct ILog;
struct IProfileLogSystem;
struct IEntitySystem;
struct IEntity;
struct ICryPak;
struct IKeyboard;
struct IMouse;
struct IConsole;
struct IRemoteConsole;
struct IInput;
struct IRenderer;
struct IConsole;
namespace Telemetry {
    struct ITelemetrySystem;
}
struct IProcess;
struct I3DEngine;
struct ITimer;
struct IGame;
struct IGameStartup;
struct IScriptSystem;
struct IAISystem;
struct INetwork;
struct IOnline;
struct ICryLobby;
struct ICryFont;
class ICrypto;
struct IMovieSystem;
struct IPhysicalWorld;
struct IMemoryManager;
namespace Audio
{
    struct IAudioSystem;
} // namespace Audio
struct IFrameProfileSystem;
struct IStatoscope;
class IDiskProfiler;
struct FrameProfiler;
struct IStreamEngine;
struct ICharacterManager;
struct SFileVersion;
struct INameTable;
struct IBudgetingSystem;
struct ILevelSystem;
struct IViewSystem;
struct IFlowSystem;
struct IDialogSystem;
namespace DRS {
    struct IDynamicResponseSystem;
}
struct IMaterialEffects;
struct IParticleManager;
class IOpticsManager;
struct IHardwareMouse;
class ICrySizer;
struct ITestSystem;
class IXMLBinarySerializer;
struct IReadWriteXMLSink;
struct IThreadTaskManager;
struct IResourceManager;
struct ITextModeConsole;
struct IAVI_Reader;
class CPNoise3;
struct IFileChangeMonitor;
struct IVisualLog;
struct ILocalizationManager;
struct ICryFactoryRegistry;
struct ICodeCheckpointMgr;
struct ISoftCodeMgr;
struct IZLibCompressor;
struct IZLibDecompressor;
struct ILZ4Decompressor;
class IZStdDecompressor;
struct IOutputPrintSink;
struct IPhysicsDebugRenderer;
struct IPhysRenderer;
struct IOverloadSceneManager;
struct IThreadManager;
struct IServiceNetwork;
struct IRemoteCommandManager;
struct IWindowMessageHandler;
struct IImageHandler;
class IResourceCompilerHelper;
class ILmbrAWS;

namespace AZ
{
    namespace IO
    {
        class FileIOBase;
    }
}

class IResourceCompilerHelper;

class CBootProfilerRecord;

namespace Serialization {
    struct IArchiveHost;
}

struct ILocalMemoryUsage;

typedef void* WIN_HWND;

class CCamera;
struct CLoadingTimeProfiler;

class ICmdLine;
class CFrameProfilerSection;

struct INotificationNetwork;
struct IPlatformOS;
struct ICryPerfHUD;
class ILyShine;
class IGemManager;

namespace JobManager {
    struct IJobManager;
}

#define PROC_MENU       1
#define PROC_3DENGINE   2

// Summary:
//   IDs for script userdata typing.
// Remarks:
//   Maybe they should be moved into the game.dll .
//##@{
#define USER_DATA_SOUND         1
#define USER_DATA_TEXTURE       2
#define USER_DATA_OBJECT        3
#define USER_DATA_LIGHT         4
#define USER_DATA_BONEHANDLER   5
#define USER_DATA_POINTER       6
//##@}

enum ESystemUpdateFlags
{
    ESYSUPDATE_IGNORE_AI = 0x0001,
    ESYSUPDATE_IGNORE_PHYSICS = 0x0002,
    // Summary:
    //   Special update mode for editor.
    ESYSUPDATE_EDITOR = 0x0004,
    ESYSUPDATE_MULTIPLAYER = 0x0008,
    ESYSUPDATE_EDITOR_AI_PHYSICS = 0x0010,
    ESYSUPDATE_EDITOR_ONLY = 0x0020,
    ESYSUPDATE_UPDATE_VIEW_ONLY = 0x0040
};

// Description:
//   Configuration specification, depends on user selected machine specification.
enum ESystemConfigSpec
{
    CONFIG_AUTO_SPEC = 0,
    CONFIG_LOW_SPEC = 1,
    CONFIG_MEDIUM_SPEC = 2,
    CONFIG_HIGH_SPEC = 3,
    CONFIG_VERYHIGH_SPEC = 4,

    END_CONFIG_SPEC_ENUM, // MUST BE LSAT VALUE. USED FOR ERROR CHECKING.
};

// Description:
//   Configuration platform. Autodetected at start, can be modified through the editor.
enum ESystemConfigPlatform
{
    CONFIG_INVALID_PLATFORM = 0,
    CONFIG_PC = 1,
    CONFIG_OSX_GL = 2,
    CONFIG_OSX_METAL = 3,
    CONFIG_ANDROID = 4,
    CONFIG_IOS = 5,
    CONFIG_XBONE = 6,
    CONFIG_PS4 = 7,
    CONFIG_APPLETV = 8,

    END_CONFIG_PLATFORM_ENUM, // MUST BE LSAT VALUE. USED FOR ERROR CHECKING.
};

enum ESubsystem
{
    ESubsys_3DEngine = 0,
    ESubsys_AI = 1,
    ESubsys_Physics = 2,
    ESubsys_Renderer = 3,
    ESubsys_Script = 4
};

// Summary:
//   Collates cycles taken per update.
struct sUpdateTimes
{
    uint32 PhysYields;
    uint64 SysUpdateTime;
    uint64 PhysStepTime;
    uint64 RenderTime;
    //extended yimes info
    uint64 physWaitTime;
    uint64 streamingWaitTime;
    uint64 animationWaitTime;
};

enum ESystemGlobalState
{
    ESYSTEM_GLOBAL_STATE_UNKNOWN,
    ESYSTEM_GLOBAL_STATE_INIT,
    ESYSTEM_GLOBAL_STATE_RUNNING,
    ESYSTEM_GLOBAL_STATE_LEVEL_LOAD_START_PREPARE,
    ESYSTEM_GLOBAL_STATE_LEVEL_LOAD_START,
    ESYSTEM_GLOBAL_STATE_LEVEL_LOAD_START_MATERIALS,
    ESYSTEM_GLOBAL_STATE_LEVEL_LOAD_START_OBJECTS,
    ESYSTEM_GLOBAL_STATE_LEVEL_LOAD_START_CHARACTERS,
    ESYSTEM_GLOBAL_STATE_LEVEL_LOAD_START_STATIC_WORLD,
    ESYSTEM_GLOBAL_STATE_LEVEL_LOAD_START_ENTITIES,
    ESYSTEM_GLOBAL_STATE_LEVEL_LOAD_START_PRECACHE,
    ESYSTEM_GLOBAL_STATE_LEVEL_LOAD_START_TEXTURES,
    ESYSTEM_GLOBAL_STATE_LEVEL_LOAD_END,
    ESYSTEM_GLOBAL_STATE_LEVEL_LOAD_COMPLETE
};

// Summary:
//   System wide events.
enum ESystemEvent
{
    // Description:
    // Seeds all random number generators to the same seed number, WParam will hold seed value.
    //##@{
    ESYSTEM_EVENT_RANDOM_SEED = 1,
    ESYSTEM_EVENT_RANDOM_ENABLE,
    ESYSTEM_EVENT_RANDOM_DISABLE,
    //##@}

    // Description:
    //   Changes to main window focus.
    //   wparam is not 0 is focused, 0 if not focused
    ESYSTEM_EVENT_CHANGE_FOCUS = 10,

    // Description:
    //   Moves of the main window.
    //   wparam=x, lparam=y
    ESYSTEM_EVENT_MOVE = 11,

    // Description:
    //   Resizes of the main window.
    //   wparam=width, lparam=height
    ESYSTEM_EVENT_RESIZE = 12,

    // Description:
    //   Activation of the main window.
    //   wparam=1/0, 1=active 0=inactive
    ESYSTEM_EVENT_ACTIVATE = 13,

    // Description:
    //   Main window position changed.
    ESYSTEM_EVENT_POS_CHANGED = 14,

    // Description:
    //   Main window style changed.
    ESYSTEM_EVENT_STYLE_CHANGED = 15,

    // Description:
    //   Sent before the loading movie is begun
    ESYSTEM_EVENT_LEVEL_LOAD_START_PRELOADINGSCREEN,

    // Description:
    //   Sent before the loading last save
    ESYSTEM_EVENT_LEVEL_LOAD_RESUME_GAME,

    // Description:
    //   Sent before starting level, before game rules initialization and before ESYSTEM_EVENT_LEVEL_LOAD_START event
    //   Used mostly for level loading profiling
    ESYSTEM_EVENT_LEVEL_LOAD_PREPARE,

    // Description:
    //   Sent to start the active loading screen rendering.
    ESYSTEM_EVENT_LEVEL_LOAD_START_LOADINGSCREEN,

    // Description:
    //   Sent when loading screen is active
    ESYSTEM_EVENT_LEVEL_LOAD_LOADINGSCREEN_ACTIVE,

    // Description:
    //   Sent before starting loading a new level.
    //   Used for a more efficient resource management.
    ESYSTEM_EVENT_LEVEL_LOAD_START,

    // Description:
    //   Sent after loading a level finished.
    //   Used for a more efficient resource management.
    ESYSTEM_EVENT_LEVEL_LOAD_END,

    // Description:
    //   Sent after trying to load a level failed.
    //   Used for resetting the front end.
    ESYSTEM_EVENT_LEVEL_LOAD_ERROR,

    // Description:
    //   Sent in case the level was requested to load, but it's not ready
    //   Used in streaming install scenario for notifying the front end.
    ESYSTEM_EVENT_LEVEL_NOT_READY,

    // Description:
    //   Sent after precaching of the streaming system has been done
    ESYSTEM_EVENT_LEVEL_PRECACHE_START,

    // Description:
    //   Sent before object/texture precache stream requests are submitted
    ESYSTEM_EVENT_LEVEL_PRECACHE_FIRST_FRAME,

    // Description:
    //  Sent when level loading is completely finished with no more onscreen
    //  movie or info rendering, and when actual gameplay can start
    ESYSTEM_EVENT_LEVEL_GAMEPLAY_START,

    // Level is unloading.
    ESYSTEM_EVENT_LEVEL_UNLOAD,

    // Summary:
    //   Sent after level have been unloaded. For cleanup code.
    ESYSTEM_EVENT_LEVEL_POST_UNLOAD,

    // Summary:
    //   Called when the game framework has been initialized.
    ESYSTEM_EVENT_GAME_POST_INIT,

    // Summary:
    //   Called when the game framework has been initialized, not loading should happen in this event.
    ESYSTEM_EVENT_GAME_POST_INIT_DONE,

    // Summary:
    //   Sent when the system is doing a full shutdown.
    ESYSTEM_EVENT_FULL_SHUTDOWN,

    // Summary:
    //   Sent when the system is doing a fast shutdown.
    ESYSTEM_EVENT_FAST_SHUTDOWN,

    // Summary:
    //   When keyboard layout changed.
    ESYSTEM_EVENT_LANGUAGE_CHANGE,

    // Description:
    //   Toggled fullscreen.
    //   wparam is 1 means we switched to fullscreen, 0 if for windowed
    ESYSTEM_EVENT_TOGGLE_FULLSCREEN,
    ESYSTEM_EVENT_SHARE_SHADER_COMBINATIONS,

    // Summary:
    //   Start 3D post rendering
    ESYSTEM_EVENT_3D_POST_RENDERING_START,

    // Summary:
    //   End 3D post rendering
    ESYSTEM_EVENT_3D_POST_RENDERING_END,

    // Summary:
    //   Called before switching to level memory heap
    ESYSTEM_EVENT_SWITCHING_TO_LEVEL_HEAP_DEPRECATED,

    // Summary:
    //   Called after switching to level memory heap
    ESYSTEM_EVENT_SWITCHED_TO_LEVEL_HEAP_DEPRECATED,

    // Summary:
    //   Called before switching to global memory heap
    ESYSTEM_EVENT_SWITCHING_TO_GLOBAL_HEAP_DEPRECATED,

    // Summary:
    //   Called after switching to global memory heap
    ESYSTEM_EVENT_SWITCHED_TO_GLOBAL_HEAP_DEPRECATED,

    // Description:
    //   Sent after precaching of the streaming system has been done
    ESYSTEM_EVENT_LEVEL_PRECACHE_END,

    // Description:
    //      Sent when game mode switch begins
    ESYSTEM_EVENT_GAME_MODE_SWITCH_START,

    // Description:
    //      Sent when game mode switch ends
    ESYSTEM_EVENT_GAME_MODE_SWITCH_END,

    // Description:
    //   Video notifications
    //   wparam=[0/1/2/3] : [stop/play/pause/resume]
    ESYSTEM_EVENT_VIDEO,

    // Description:
    //   Sent if the game is paused
    ESYSTEM_EVENT_GAME_PAUSED,

    // Description:
    //   Sent if the game is resumed
    ESYSTEM_EVENT_GAME_RESUMED,

    // Description:
    //      Sent when time of day is set
    ESYSTEM_EVENT_TIME_OF_DAY_SET,

    // Description:
    //      Sent once the Editor finished initialization.
    ESYSTEM_EVENT_EDITOR_ON_INIT,

    // Description:
    //      Sent when frontend is initialised
    ESYSTEM_EVENT_FRONTEND_INITIALISED,

    // Description:
    //      Sent once the Editor switches between in-game and editing mode.
    ESYSTEM_EVENT_EDITOR_GAME_MODE_CHANGED,

    // Description:
    //      Sent once the Editor switches simulation mode (AI/Physics).
    ESYSTEM_EVENT_EDITOR_SIMULATION_MODE_CHANGED,

    // Description:
    //      Sent when frontend is reloaded
    ESYSTEM_EVENT_FRONTEND_RELOADED,

#if defined(AZ_RESTRICTED_PLATFORM)
#define AZ_RESTRICTED_SECTION ISYSTEM_H_SECTION_1
    #if defined(AZ_PLATFORM_XENIA)
        #include "Xenia/ISystem_h_xenia.inl"
    #elif defined(AZ_PLATFORM_PROVO)
        #include "Provo/ISystem_h_provo.inl"
    #endif
#endif

#if defined(AZ_RESTRICTED_PLATFORM)
#define AZ_RESTRICTED_SECTION ISYSTEM_H_SECTION_2
    #if defined(AZ_PLATFORM_XENIA)
        #include "Xenia/ISystem_h_xenia.inl"
    #elif defined(AZ_PLATFORM_PROVO)
        #include "Provo/ISystem_h_provo.inl"
    #endif
#endif
    ESYSTEM_EVENT_STREAMING_INSTALL_ERROR,

    // Description:
    //      Sent when the online services are initialized.
    ESYSTEM_EVENT_ONLINE_SERVICES_INITIALISED,

    // Description:
    //      Sent when a new audio implementation is loaded
    ESYSTEM_EVENT_AUDIO_IMPLEMENTATION_LOADED,

    // Description:
    //      Sent when optional modules (like gems) should register their flow nodes
    ESYSTEM_EVENT_FLOW_SYSTEM_REGISTER_EXTERNAL_NODES,

    ESYSTEM_EVENT_USER = 0x1000,

    ESYSTEM_BEAM_PLAYER_TO_CAMERA_POS
};

// Description:
//   User defined callback, which can be passed to ISystem.
struct ISystemUserCallback
{
    // <interfuscator:shuffle>
    virtual ~ISystemUserCallback() {}
    // Description:
    //   This method is called at the earliest point the ISystem pointer can be used
    //   the log might not be yet there.
    virtual void OnSystemConnect(ISystem* pSystem) {}

    // Summary:
    //   Signals to User that engine error occurred.
    // Return Value:
    //      True to Halt execution or false to ignore this error
    virtual bool OnError(const char* szErrorString) = 0;

    // Notes:
    //   If working in Editor environment notify user that engine want to Save current document.
    //   This happens if critical error have occurred and engine gives a user way to save data and not lose it
    //   due to crash.
    virtual bool OnSaveDocument() = 0;

    // Notes:
    //  If working in Editor environment and a critical error occurs notify the user to backup
    //  the current document to prevent data loss due to crash.
    virtual bool OnBackupDocument() = 0;

    // Description:
    //   Notifies user that system wants to switch out of current process.
    // Example:
    //  Called when pressing ESC in game mode to go to Menu.
    virtual void OnProcessSwitch() = 0;

    // Description:
    //   Notifies user, usually editor, about initialization progress in system.
    virtual void OnInitProgress(const char* sProgressMsg) = 0;

    // Description:
    //   Initialization callback.  This is called early in CSystem::Init(), before
    //   any of the other callback methods is called.
    // See also:
    //   CSystem::Init()
    virtual void OnInit(ISystem*) { }

    // Summary:
    //   Shutdown callback.
    virtual void OnShutdown() { }

    // Summary:
    //   Quit callback.
    // See also:
    //   CSystem::Quit()
    virtual void OnQuit() { }

    // Description:
    //   Notify user of an update iteration.  Called in the update loop.
    virtual void OnUpdate() { }

    // Description:
    //   Show message by provider.
    virtual int ShowMessage(const char* text, const char* caption, unsigned int uType) { return CryMessageBox(text, caption, uType); }

    // Description:
    //   Collects the memory information in the user program/application.
    virtual void GetMemoryUsage(ICrySizer* pSizer) = 0;
    // </interfuscator:shuffle>

    //   Post console load, for cvar setting
    virtual void OnConsoleCreated(IConsole* pConsole) {}
};

// Description:
//   Interface used for getting notified when a system event occurs.
struct ISystemEventListener
{
    // <interfuscator:shuffle>
    virtual ~ISystemEventListener() {}
    virtual void OnSystemEventAnyThread(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) {}
    virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) { }
    // </interfuscator:shuffle>
};

// Description:
//   Structure used for getting notified when a system event occurs.
struct ISystemEventDispatcher
{
    // <interfuscator:shuffle>
    virtual ~ISystemEventDispatcher() {}
    virtual bool RegisterListener(ISystemEventListener* pListener) = 0;
    virtual bool RemoveListener(ISystemEventListener* pListener) = 0;

    virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) = 0;
    virtual void Update() = 0;

    //virtual void OnLocaleChange() = 0;
    // </interfuscator:shuffle>
};

struct IErrorObserver
{
    // <interfuscator:shuffle>
    virtual ~IErrorObserver() {}
    virtual void OnAssert(const char* condition, const char* message, const char* fileName, unsigned int fileLineNumber) = 0;
    virtual void OnFatalError(const char* message) = 0;
    // </interfuscator:shuffle>
};

enum ESystemProtectedFunctions
{
    eProtectedFunc_Save = 0,
    eProtectedFunc_Load = 1,
    eProtectedFuncsLast = 10,
};

struct SCvarsDefault
{
    SCvarsDefault()
    {
        sz_r_DriverDef = NULL;
    }

    const char* sz_r_DriverDef;
};

#if defined(CVARS_WHITELIST)
struct ICVarsWhitelist
{
    // <interfuscator:shuffle>
    virtual ~ICVarsWhitelist() {};
    virtual bool IsWhiteListed(const string& command, bool silent) = 0;
    // </interfuscator:shuffle>
};
#endif // defined(CVARS_WHITELIST)

#if defined(AZ_RESTRICTED_PLATFORM)
#define AZ_RESTRICTED_SECTION ISYSTEM_H_SECTION_3
    #if defined(AZ_PLATFORM_XENIA)
        #include "Xenia/ISystem_h_xenia.inl"
    #elif defined(AZ_PLATFORM_PROVO)
        #include "Provo/ISystem_h_provo.inl"
    #endif
#endif

namespace AZ
{
    namespace Internal
    {
        class EnvironmentInterface;
    } // namespace Internal
} // namespace AZ

typedef AZ::Internal::EnvironmentInterface SharedEnvironmentInstance;

// Description:
//  Structure passed to Init method of ISystem interface.
struct SSystemInitParams
{
    void* hInstance;                                //
    void* hWnd;                                     //
    void* hWndForInputSystem;                       // the HWND for the input devices, distinct from the hWnd, which the rendering system overrides anyways

    char remoteIP[256];
    int  remotePort;
    bool remoteFileIO;
    bool remoteResourceCompiler;
    bool connectToRemote;
    bool waitForConnection; // if true, wait for the remote connection to be established before proceeding to system init.
    char assetsPlatform[64]; // what flavor of assets to load.  ("pc" / "es3" / "ps4" / "ios" / "xboxone").  Corresponds to those in rc.ini and asset processor ini // ACCEPTED_USE
    char gameFolderName[256]; // just the name.  Not the full path.
    char gameDLLName[256]; // just the name.  Not the full path.  ("ExampleGame") - does not include extension
    char branchToken[12]; // information written by the assetprocessor which help determine whether the game/editor are running from the same branch or not

    ILog* pLog;                                     // You can specify your own ILog to be used by System.
    ILogCallback* pLogCallback;                     // You can specify your own ILogCallback to be added on log creation (used by Editor).
    ISystemUserCallback* pUserCallback;
    const char* sLogFileName;                       // File name to use for log.
    bool autoBackupLogs;                            // if true, logs will be automatically backed up each startup
    IValidator* pValidator;                         // You can specify different validator object to use by System.
    IOutputPrintSink* pPrintSync;               // Print Sync which can be used to catch all output from engine
    char szSystemCmdLine[2048];                     // Command line.


                                                    // set some paths before you create the system.

                                                    // rootPath - (REQUIRED) folder containing root.  Must contain system.cfg or bootstrap.cfg basically.
                                                    // the remainder are optional and if specified should contain prefixes that can be prepended to any file to get to that location:
                                                    // READ ONLY!
    char rootPath[256];
    char rootPathCache[256];

    // assetsPath - (REQUIRED) - where you assets live.  The engine config parser will default this to @root@/gamename
    // READ ONLY!
    char assetsPath[256];
    char assetsPathCache[256];

    // userPath - (OPTIONAL) User path contains a folder for preferences persistent storage.  May be persisted to the cloud (by things like IOS)
    // If not specified, this is assumed @root@/User/
    // WRITABLE
    char userPath[256];

    // cachePath - (OPTIONAL) a temporary store that can be erased at any time and does not need to be persisted
    // on the cloud or anything like that. if not specified, this will be @user@/Cache
    // WRITABLE
    char cachePath[256];

    // logPath - (OPTIONAL) a log path folder.
    // If not specified, it will be @cache@/Logs
    // WRITABLE
    char logPath[256];

    // the game should never use these values instead, the game should be using crypak or fileio with aliases:
    // @root@ To get to the folder where system.cfg lives
    // @assets@ to get to the folder containing game assets (textures and such) - by default, this is @root@/Gamename/
    // @devroot@ to get to source files that are checked into source control (PC EDITOR ONLY!)
    // @engroot@ to get to path to the engine root folder
    // @user@ to access user store
    // @cache@ to access temporary cache
    // @log@ to access log file and other forensic storage
    char szBinariesDir[256];

    bool bEditor;                                   // When running in Editor mode.
    bool bPreview;                                  // When running in Preview mode (Minimal initialization).
    bool bTestMode;                                 // When running in Automated testing mode.
    bool bDedicatedServer;                          // When running a dedicated server.
    bool bExecuteCommandLine;                       // can be switched of to suppress the feature or do it later during the initialization.
    bool bSkipFont;                                     // Don't load CryFont.dll
    bool bSkipRenderer;                                 // Don't load Renderer
    bool bSkipConsole;                                  // Don't create console
    bool bSkipNetwork;                                  // Don't create Network
    bool bSkipWebsocketServer;      // Don't create the WebSocket server
    bool bMinimal;                              // Don't load banks
    bool bTesting;                              // CryUnit
    bool bNoRandom;                             //use fixed generator init/seed
    bool bShaderCacheGen;                   // When running in shadercache gen mode
    bool bUnattendedMode;                           // When running as part of a build on build-machines: Prevent popping up of any dialog
    bool bSkipMovie;            // Don't load movie
    bool bSkipAnimation;        // Don't load animation
    bool bSkipScriptSystem;     // Don't load script system

    bool bToolMode;                                 // System is running inside a tool. Will not create USER directory or anything else that the game needs to do

    bool bSkipPhysics; // Don't initialize CryPhysics.

    ISystem* pSystem;                                           // Pointer to existing ISystem interface, it will be reused if not NULL.
    IGameStartup* pGameStartup;                     // Pointer to the calling GameStartup instance, to allow use of some game specific data during engine init.
                                                    //  char szLocalIP[256];                                    // local IP address (needed if we have several servers on one machine)

    typedef void* (*ProtectedFunction)(void* param1, void* param2);
    ProtectedFunction pProtectedFunctions[eProtectedFuncsLast];         // Protected functions.

    SCvarsDefault* pCvarsDefault;               // to override the default value of some cvar

#if defined(CVARS_WHITELIST)
    ICVarsWhitelist* pCVarsWhitelist;       // CVars whitelist callback
#endif // defined(CVARS_WHITELIST)

    SharedEnvironmentInstance* pSharedEnvironment;

    // Summary:
    //  Initialization defaults.
    SSystemInitParams()
    {
        hInstance = NULL;
        hWnd = NULL;
        hWndForInputSystem = NULL;

        memset(rootPath, 0, sizeof(rootPath));
        memset(rootPathCache, 0, sizeof(rootPathCache));
        memset(userPath, 0, sizeof(userPath));
        memset(assetsPath, 0, sizeof(assetsPath));
        memset(assetsPathCache, 0, sizeof(assetsPathCache));
        memset(cachePath, 0, sizeof(cachePath));
        memset(logPath, 0, sizeof(logPath));
        memset(gameFolderName, 0, sizeof(gameFolderName));
        memset(gameDLLName, 0, sizeof(gameDLLName));
        memset(branchToken, 0, sizeof(branchToken));

        memset(remoteIP, 0, sizeof(remoteIP));
        azstrcpy(remoteIP, sizeof(remoteIP), "127.0.0.1");
        memset(assetsPlatform, 0, sizeof(assetsPlatform));
        azstrcpy(assetsPlatform, sizeof(assetsPlatform), "pc");

        remotePort = 45643;
        remoteFileIO = false;
        remoteResourceCompiler = false;
        connectToRemote = false;
        waitForConnection = false;

        pLog = NULL;
        pLogCallback = NULL;
        pUserCallback = NULL;
#if defined(CVARS_WHITELIST)
        pCVarsWhitelist = NULL;
#endif // defined(CVARS_WHITELIST)
        sLogFileName = NULL;
        autoBackupLogs = true;
        pValidator = NULL;
        pPrintSync = NULL;
        memset(szSystemCmdLine, 0, sizeof(szSystemCmdLine));
        memset(szBinariesDir, 0, sizeof(szBinariesDir));

        bEditor = false;
        bPreview = false;
        bTestMode = false;
        bDedicatedServer = false;
        bExecuteCommandLine = true;
        bExecuteCommandLine = true;
        bSkipFont = false;
        bSkipRenderer = false;
        bSkipConsole = false;
        bSkipNetwork = false;
#if defined(WIN32) || defined(WIN64)
        // create websocket server by default. bear in mind that USE_HTTP_WEBSOCKETS is not defined in release.
        bSkipWebsocketServer = false;
#else
        // CTCPStreamSocket only seems to fully support Win32 and 64
        bSkipWebsocketServer = true;
#endif
        bMinimal = false;
        bTesting = false;
        bNoRandom = false;
        bShaderCacheGen = false;
        bUnattendedMode = false;
        bSkipMovie = false;
        bSkipAnimation = false;
        bSkipScriptSystem = false;
        bToolMode = false;
        bSkipPhysics = false;

        pSystem = NULL;
        pGameStartup = NULL;

        memset(pProtectedFunctions, 0, sizeof(pProtectedFunctions));
        pCvarsDefault = NULL;

        pSharedEnvironment = nullptr;
    }

    bool UseAssetCache() const
    {
#if defined(AZ_PLATFORM_WINDOWS) || defined(AZ_PLATFORM_MAC) || defined(AZ_PLATFORM_LINUX)
        char checkPath[AZ_MAX_PATH_LEN] = { 0 };
        azsnprintf(checkPath, AZ_MAX_PATH_LEN, "%s/engine.json", rootPathCache);
        return AZ::IO::SystemFile::Exists(checkPath);
#else
        return false;
#endif // defined(AZ_PLATFORM_WINDOWS) || AZ_TRAIT_OS_PLATFORM_APPLE
    }
};

// Summary:
//   Typedef for frame profile callback function.
typedef void(*FrameProfilerSectionCallback)(class CFrameProfilerSection* pSection);

// Notes:
//   Can be used for LoadConfiguration().
// See also:
//   LoadConfiguration()
struct ILoadConfigurationEntrySink
{
    // <interfuscator:shuffle>
    virtual ~ILoadConfigurationEntrySink() {}
    virtual void OnLoadConfigurationEntry(const char* szKey, const char* szValue, const char* szGroup) = 0;
    virtual void OnLoadConfigurationEntry_End() {}
    // </interfuscator:shuffle>
};

struct SPlatformInfo
{
    unsigned int numCoresAvailableToProcess;
    unsigned int numLogicalProcessors;

#if defined(WIN32) || defined(WIN64)
    enum EWinVersion
    {
        WinUndetected,
        Win2000,
        WinXP,
        WinSrv2003,
        WinVista,
        Win7,
        Win8,
        Win81,
        Win10
    };

    EWinVersion winVer;
    bool win64Bit;
    bool vistaKB940105Required;
#endif
};

// Description:
//  Holds info about system update stats over perior of time (cvar-tweakable)

struct SSystemUpdateStats
{
    SSystemUpdateStats()
        : avgUpdateTime(0.0f)
        , minUpdateTime(0.0f)
        , maxUpdateTime(0.0f) {}
    float avgUpdateTime;
    float minUpdateTime;
    float maxUpdateTime;
};

// Description:
//   Global environment.
//   Contains pointers to all global often needed interfaces.
//    This is a faster way to get interface pointer then calling ISystem interface to retrieve one.
// Notes:
//   Some pointers can be NULL, use with care.
// See also:
//   ISystem
struct SSystemGlobalEnvironment
{
    IDialogSystem*             pDialogSystem;
    I3DEngine*                 p3DEngine;
    INetwork*                  pNetwork;
    // **************************************************************************************
    // DEPRECATED: This object is currently deprecated and will be removed in release 1.13
    // **************************************************************************************
    IScriptSystem*             pScriptSystem;
    IPhysicalWorld*            pPhysicalWorld;
    IFlowSystem*               pFlowSystem;
    IInput*                    pInput;
    IStatoscope*        pStatoscope;
    ICryPak*                   pCryPak;
    AZ::IO::FileIOBase*        pFileIO;
    IFileChangeMonitor*        pFileChangeMonitor;
    IProfileLogSystem*         pProfileLogSystem;
    IParticleManager*          pParticleManager;
    IOpticsManager*                      pOpticsManager;
    IFrameProfileSystem*       pFrameProfileSystem;
    ITimer*                    pTimer;
    ICryFont*                  pCryFont;
    IGame*                     pGame;
    ILocalMemoryUsage*     pLocalMemoryUsage;
    IEntitySystem*             pEntitySystem;
    IConsole*                  pConsole;
    Telemetry::ITelemetrySystem* pTelemetrySystem;
    ISystem*                   pSystem = nullptr;
    ICharacterManager*         pCharacterManager;
    IAISystem*                 pAISystem;
    ILog*                      pLog;
    ICodeCheckpointMgr*     pCodeCheckpointMgr;
    IMovieSystem*              pMovieSystem;
    INameTable*                pNameTable;
    IVisualLog*                pVisualLog;
    IRenderer*                 pRenderer;
    IHardwareMouse*            pHardwareMouse;
    IMaterialEffects*          pMaterialEffects;
    ISoftCodeMgr*                            pSoftCodeMgr;
    IOverloadSceneManager*       pOverloadSceneManager;
    IServiceNetwork*              pServiceNetwork;
    IRemoteCommandManager*        pRemoteCommandManager;
    ILyShine*                      pLyShine;
    IResourceCompilerHelper*      pResourceCompilerHelper;
    DRS::IDynamicResponseSystem*  pDynamicResponseSystem;
    SharedEnvironmentInstance*      pSharedEnvironment;
    IFilePathManager* pFilePathManager;
    IThreadManager*               pThreadManager;

#if defined(AZ_RESTRICTED_PLATFORM)
#define AZ_RESTRICTED_SECTION ISYSTEM_H_SECTION_4
    #if defined(AZ_PLATFORM_XENIA)
        #include "Xenia/ISystem_h_xenia.inl"
    #elif defined(AZ_PLATFORM_PROVO)
        #include "Provo/ISystem_h_provo.inl"
    #endif
#endif

    ISystemScheduler*          pSystemScheduler;

    threadID                                 mMainThreadId;     //The main thread ID is used in multiple systems so should be stored globally

                                                                //////////////////////////////////////////////////////////////////////////
    uint32                     nMainFrameID;

    //////////////////////////////////////////////////////////////////////////
    const char*                szCmdLine;  // Startup command line.

                                           //////////////////////////////////////////////////////////////////////////
                                           // Generic debug string which can be easily updated by any system and output by the debug handler
    enum
    {
        MAX_DEBUG_STRING_LENGTH = 128
    };
    char                                            szDebugStatus[MAX_DEBUG_STRING_LENGTH];

    //////////////////////////////////////////////////////////////////////////
    // Used to tell if this is a server/multiplayer instance
    bool                       bServer;
    bool                                             bMultiplayer;
    bool                       bHostMigrating;
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Used by frame profiler.
    bool                       bProfilerEnabled;
    FrameProfilerSectionCallback callbackStartSection;
    FrameProfilerSectionCallback callbackEndSection;
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Indicate Editor status.
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // Used by CRY_ASSERT
    bool                                            bIgnoreAllAsserts;
    bool                                            bNoAssertDialog;
    bool                                            bTesting;
    //////////////////////////////////////////////////////////////////////////

    bool                                            bNoRandomSeed;

    SPlatformInfo pi;

    // Protected functions.
    SSystemInitParams::ProtectedFunction pProtectedFunctions[eProtectedFuncsLast];  // Protected functions.

                                                                                    //////////////////////////////////////////////////////////////////////////
                                                                                    // Flag to able to print out of memory conditon
    bool                                            bIsOutOfMemory;
    bool                                            bIsOutOfVideoMemory;

    bool                                            bToolMode;

    ILINE const bool IsClient() const
    {
#if defined(CONSOLE)
        return true;
#else
        return bClient;
#endif
    }

    ILINE const bool IsDedicated() const
    {
#if defined(CONSOLE)
        return false;
#else
        return bDedicated;
#endif
    }

#if !defined(CONSOLE)
    ILINE void SetIsEditor(bool isEditor)
    {
        bEditor = isEditor;
    }

    ILINE void SetIsEditorGameMode(bool isEditorGameMode)
    {
        bEditorGameMode = isEditorGameMode;
    }

    ILINE void SetIsEditorSimulationMode(bool isEditorSimulationMode)
    {
        bEditorSimulationMode = isEditorSimulationMode;
    }

    ILINE void SetIsDedicated(bool isDedicated)
    {
        bDedicated = isDedicated;
    }

    ILINE void SetIsClient(bool isClient)
    {
        bClient = isClient;
    }
#endif

    //this way the compiler can strip out code for consoles
    ILINE const bool IsEditor() const
    {
#if defined(CONSOLE)
        return false;
#else
        return bEditor;
#endif
    }

    ILINE const bool IsEditorGameMode() const
    {
#if defined(CONSOLE)
        return false;
#else
        return bEditorGameMode;
#endif
    }

    ILINE const bool IsEditorSimulationMode() const
    {
#if defined(CONSOLE)
        return false;
#else
        return bEditorSimulationMode;
#endif
    }

    ILINE const bool IsEditing() const
    {
#if defined(CONSOLE)
        return false;
#else
        return bEditor && !bEditorGameMode;
#endif
    }

    ILINE const bool IsFMVPlaying() const
    {
        return m_isFMVPlaying;
    }

    ILINE void SetFMVIsPlaying(const bool isPlaying)
    {
        m_isFMVPlaying = isPlaying;
    }

    ILINE const bool IsCutscenePlaying() const
    {
        return m_isCutscenePlaying;
    }

    ILINE void SetCutsceneIsPlaying(const bool isPlaying)
    {
        m_isCutscenePlaying = isPlaying;
    }

    ILINE bool IsInToolMode() const
    {
        return bToolMode;
    }

    ILINE void SetToolMode(bool bNewToolMode)
    {
        bToolMode = bNewToolMode;
    }

    ILINE void SetDynamicMergedMeshGenerationEnabled(bool mmgenEnable)
    {
        m_bDynamicMergedMeshGenerationEnabled = mmgenEnable;
    }

    ILINE const bool IsDynamicMergedMeshGenerationEnabled() const
    {
        return m_bDynamicMergedMeshGenerationEnabled;
    }

#if !defined(CONSOLE)
private:
    bool bClient;
    bool bEditor;          // Engine is running under editor.
    bool bEditorGameMode;  // Engine is in editor game mode.
    bool bEditorSimulationMode;  // Engine is in editor simulation mode.
    bool bDedicated;             // Engine is in dedicated
#endif

    bool m_isFMVPlaying;
    bool m_isCutscenePlaying;
    bool m_bDynamicMergedMeshGenerationEnabled;

public:
    SSystemGlobalEnvironment()
        : pSystemScheduler(nullptr)
        , szCmdLine("")
        , bToolMode(false)
        , m_bDynamicMergedMeshGenerationEnabled(false)
    {
    };
};

// NOTE Nov 25, 2008: <pvl> the ISystem interface that follows has a member function
// called 'GetUserName'.  If we don't #undef'ine the same-named Win32 symbol here
// ISystem wouldn't even compile.
// TODO Nov 25, 2008: <pvl> there might be a better place for this?
#ifdef GetUserName
#undef GetUserName
#endif


struct IProfilingSystem
{
    // <interfuscator:shuffle>
    virtual ~IProfilingSystem() {}
    //////////////////////////////////////////////////////////////////////////
    // VTune Profiling interface.

    // Summary:
    //   Resumes vtune data collection.
    virtual void VTuneResume() = 0;
    // Summary:
    //   Pauses vtune data collection.
    virtual void VTunePause() = 0;
    //////////////////////////////////////////////////////////////////////////
    // </interfuscator:shuffle>
};

////////////////////////////////////////////////////////////////////////////////////////////////

// Description:
//   Main Engine Interface.
//   Initialize and dispatch all engine's subsystems.
struct ISystem
{
    struct ILoadingProgressListener
    {
        // <interfuscator:shuffle>
        virtual ~ILoadingProgressListener() {}
        virtual void OnLoadingProgress(int steps) = 0;
        // </interfuscator:shuffle>
    };

#ifndef _RELEASE
    enum LevelLoadOrigin
    {
        eLLO_Unknown,
        eLLO_NewLevel,
        eLLO_Level2Level,
        eLLO_Resumed,
        eLLO_MapCmd,
    };

    struct ICheckpointData
    {
        int                         m_totalLoads;
        LevelLoadOrigin m_loadOrigin;
    };
#endif

    // <interfuscator:shuffle>
    virtual ~ISystem() {}
    // Summary:
    //   Releases ISystem.
    virtual void Release() = 0;
    virtual ILoadConfigurationEntrySink* GetCVarsWhiteListConfigSink() const = 0; // will return NULL if no whitelisting

                                                                                  // Summary:
                                                                                  //   Returns pointer to the global environment structure.
    virtual SSystemGlobalEnvironment* GetGlobalEnvironment() = 0;

    // Summary:
    //   Updates all subsystems (including the ScriptSink() )
    // Arguments:
    //   flags      - One or more flags from ESystemUpdateFlags structure.
    //   nPauseMode - 0=normal(no pause), 1=menu/pause, 2=cutscene
    virtual bool UpdatePreTickBus(int updateFlags = 0, int nPauseMode = 0) = 0;

    // Summary:
    //   Updates all subsystems (including the ScriptSink() )
    // Arguments:
    //   flags      - One or more flags from ESystemUpdateFlags structure.
    //   nPauseMode - 0=normal(no pause), 1=menu/pause, 2=cutscene
    virtual bool UpdatePostTickBus(int updateFlags = 0, int nPauseMode = 0) = 0;

    // Summary:
    //   Updates only require components during loading
    virtual bool UpdateLoadtime() = 0;

    // Summary:
    //   Optimisation: do part of the update while waiting for occlusion queries to complete
    virtual void DoWorkDuringOcclusionChecks() = 0;
    virtual bool NeedDoWorkDuringOcclusionChecks() = 0;

    // Summary:
    //   Renders subsystems.
    virtual void    Render() = 0;
    // Summary:
    //   Begins rendering frame.
    virtual void    RenderBegin() = 0;
    // Summary:
    //   Ends rendering frame and swap back buffer.
    virtual void    RenderEnd(bool bRenderStats = true, bool bMainWindow = true) = 0;

    //! Update screen and call some important tick functions during loading.
    virtual void SynchronousLoadingTick(const char* pFunc, int line) = 0;

    // Description:
    //   Renders the statistics; this is called from RenderEnd, but if the
    //   Host application (Editor) doesn't employ the Render cycle in ISystem,
    //   it may call this method to render the essential statistics.
    virtual void RenderStatistics() = 0;
    virtual void RenderPhysicsHelpers() = 0;
    virtual void RenderPhysicsStatistics(IPhysicalWorld* pWorld) = 0;

    // Summary:
    //   Returns the current used memory.
    virtual uint32 GetUsedMemory() = 0;

    // Summary:
    //   Retrieve the name of the user currently logged in to the computer.
    virtual const char* GetUserName() = 0;

    // Summary:
    //   Gets current supported CPU features flags. (CPUF_SSE, CPUF_SSE2, CPUF_3DNOW, CPUF_MMX)
    virtual int GetCPUFlags() = 0;

    // Summary:
    //   Gets number of CPUs
    virtual int GetLogicalCPUCount() = 0;

    //! Get the 'kind' of assets you need to load - this describes the flavor of assets you are going to load
    //! based on the platform you're on - so for example, android on ES3 will be 'es3' but android on opengl might load PC assets or others...
    //! This is defined in bootstrap.cfg and is read-only during runtime.
    virtual const char* GetAssetsPlatform() const = 0;

    // Summary:
    //   Return the rendering driver name. GL or Metal
    virtual const char* GetRenderingDriverName() const = 0;

    // Summary:
    //   Dumps the memory usage statistics to the logging default MB. (can be KB)
    virtual void DumpMemoryUsageStatistics(bool bUseKB = false) = 0;

    // Summary:
    //   Quits the application.
    virtual void    Quit() = 0;
    // Summary:
    //   Tells the system if it is relaunching or not.
    virtual void    Relaunch(bool bRelaunch) = 0;
    // Summary:
    //   Returns true if the application is in the shutdown phase.
    virtual bool    IsQuitting() const = 0;
    // Summary:
    //   Returns true if the application was initialized to generate the shader cache.
    virtual bool    IsShaderCacheGenMode() const = 0;
    // Summary:
    //   Tells the system in which way we are using the serialization system.
    virtual void  SerializingFile(int mode) = 0;
    virtual int IsSerializingFile() const = 0;

    virtual bool IsRelaunch() const = 0;
    virtual bool LaunchAssetProcessor() = 0;

    // Summary:
    //   Displays an error message to display info for certain time
    // Arguments:
    //   acMessage - Message to show
    //   fTime - Amount of seconds to show onscreen
    virtual void DisplayErrorMessage(const char* acMessage, float fTime, const float* pfColor = 0, bool bHardError = true) = 0;

    // Description:
    //   Displays error message.
    //   Logs it to console and file and error message box then terminates execution.
    virtual void FatalError(const char* sFormat, ...) PRINTF_PARAMS(2, 3) = 0;

    // Description:
    //   Reports a bug using the crash handler.
    //   Logs an error to the console and launches the crash handler, then continues execution.
    virtual void ReportBug(const char* sFormat, ...) PRINTF_PARAMS(2, 3) = 0;

    // Description:
    //   Report warning to current Validator object.
    //   Doesn't terminate the execution.
    //##@{
    virtual void WarningV(EValidatorModule module, EValidatorSeverity severity, int flags, const char* file, const char* format, va_list args) = 0;
    virtual void Warning(EValidatorModule module, EValidatorSeverity severity, int flags, const char* file, const char* format, ...) = 0;
    //##@}

    // Description:
    //   Report message by provider or by using CryMessageBox.
    //   Doesn't terminate the execution.
    virtual int ShowMessage(const char* text, const char* caption, unsigned int uType) = 0;

    // Summary:
    //   Compare specified verbosity level to the one currently set.
    virtual bool CheckLogVerbosity(int verbosity) = 0;

    // return the related subsystem interface

    //
    virtual IZLibCompressor* GetIZLibCompressor() = 0;
    virtual IZLibDecompressor* GetIZLibDecompressor() = 0;
    virtual ILZ4Decompressor* GetLZ4Decompressor() = 0;
    virtual IZStdDecompressor* GetZStdDecompressor() = 0;
    virtual ICryPerfHUD* GetPerfHUD() = 0;
    virtual IPlatformOS* GetPlatformOS() = 0;
    virtual INotificationNetwork* GetINotificationNetwork() = 0;
    virtual IHardwareMouse* GetIHardwareMouse() = 0;
    virtual IDialogSystem* GetIDialogSystem() = 0;
    virtual IFlowSystem* GetIFlowSystem() = 0;
    virtual IViewSystem* GetIViewSystem() = 0;
    virtual ILevelSystem* GetILevelSystem() = 0;
    virtual IBudgetingSystem* GetIBudgetingSystem() = 0;
    virtual INameTable* GetINameTable() = 0;
    virtual IDiskProfiler* GetIDiskProfiler() = 0;
    virtual IFrameProfileSystem* GetIProfileSystem() = 0;
    virtual IValidator* GetIValidator() = 0;
    virtual IPhysicsDebugRenderer* GetIPhysicsDebugRenderer() = 0;
    virtual IPhysRenderer* GetIPhysRenderer() = 0;
    virtual ICharacterManager* GetIAnimationSystem() = 0;
    virtual IStreamEngine* GetStreamEngine() = 0;
    virtual ICmdLine* GetICmdLine() = 0;
    virtual ILog* GetILog() = 0;
    virtual ICryPak* GetIPak() = 0;
    virtual ICryFont* GetICryFont() = 0;
    virtual IEntitySystem* GetIEntitySystem() = 0;
    virtual IMemoryManager* GetIMemoryManager() = 0;
    virtual IAISystem* GetAISystem() = 0;
    virtual IMovieSystem* GetIMovieSystem() = 0;
    virtual IPhysicalWorld* GetIPhysicalWorld() = 0;
    virtual I3DEngine* GetI3DEngine() = 0;
    // **************************************************************************************
    // DEPRECATED: This method is currently deprecated and will be removed in release 1.13
    // **************************************************************************************
    virtual IScriptSystem* GetIScriptSystem() = 0;
    virtual IConsole* GetIConsole() = 0;
    virtual IRemoteConsole* GetIRemoteConsole() = 0;
    virtual ICrypto* GetCrypto() = 0;
    // Returns:
    //   Can be NULL, because it only exists when running through the editor, not in pure game mode.
    virtual IResourceManager* GetIResourceManager() = 0;
    virtual IThreadTaskManager* GetIThreadTaskManager() = 0;
    virtual IProfilingSystem* GetIProfilingSystem() = 0;
    virtual ISystemEventDispatcher* GetISystemEventDispatcher() = 0;
    virtual IVisualLog* GetIVisualLog() = 0;
    virtual IFileChangeMonitor* GetIFileChangeMonitor() = 0;

    virtual WIN_HWND GetHWND() = 0;

    virtual IGame* GetIGame() = 0;
    virtual IRenderer* GetIRenderer() = 0;
    virtual IInput* GetIInput() = 0;
    virtual ITimer* GetITimer() = 0;
    virtual IGemManager* GetGemManager() = 0;
    virtual IThreadManager* GetIThreadManager() = 0;

    //irtual IThreadManager* GetIThreadManager() = 0;


    virtual INetwork* GetINetwork() = 0;

    virtual void SetLoadingProgressListener(ILoadingProgressListener* pListener) = 0;
    virtual ISystem::ILoadingProgressListener* GetLoadingProgressListener() const = 0;

    // Summary:
    //   Game is created after System init, so has to be set explicitly.
    virtual void                        SetIGame(IGame* pGame) = 0;
    virtual void            SetIFlowSystem(IFlowSystem* pFlowSystem) = 0;
    virtual void SetIDialogSystem(IDialogSystem* pDialogSystem) = 0;
    virtual void SetIMaterialEffects(IMaterialEffects* pMaterialEffects) = 0;
    virtual void SetIParticleManager(IParticleManager* pParticleManager) = 0;
    virtual void SetIOpticsManager(IOpticsManager* pOpticsManager) = 0;
    virtual void SetIFileChangeMonitor(IFileChangeMonitor* pFileChangeMonitor) = 0;
    virtual void SetIVisualLog(IVisualLog* pVisualLog) = 0;

    //virtual   const char          *GetGamePath()=0;

    virtual void DebugStats(bool checkpoint, bool leaks) = 0;
    virtual void DumpWinHeaps() = 0;
    virtual int DumpMMStats(bool log) = 0;

    // Arguments:
    //   bValue - Set to true when running on a cheat protected server or a client that is connected to it (not used in singleplayer).
    virtual void SetForceNonDevMode(const bool bValue) = 0;
    // Return Value:
    //   True when running on a cheat protected server or a client that is connected to it (not used in singleplayer).
    virtual bool GetForceNonDevMode() const = 0;
    virtual bool WasInDevMode() const = 0;
    virtual bool IsDevMode() const = 0;
    virtual bool IsMODValid(const char* szMODName) const = 0;
    virtual bool IsMinimalMode() const = 0;
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    // IXmlNode interface.
    //////////////////////////////////////////////////////////////////////////

    // Summary:
    //   Creates new xml node.
    virtual XmlNodeRef CreateXmlNode(const char* sNodeName = "", bool bReuseStrings = false, bool bIsProcessingInstruction = false) = 0;
    // Summary:
    //   Loads xml from memory buffer, returns 0 if load failed.
    virtual XmlNodeRef LoadXmlFromBuffer(const char* buffer, size_t size, bool bReuseStrings = false, bool bSuppressWarnings = false) = 0;
    // Summary:
    //   Loads xml file, returns 0 if load failed.
    virtual XmlNodeRef LoadXmlFromFile(const char* sFilename, bool bReuseStrings = false) = 0;
    // Summary:
    //   Retrieves access to XML utilities interface.
    virtual IXmlUtils* GetXmlUtils() = 0;

    // Summary:
    //   Interface to access different implementations of Serialization::IArchive in a centralized way.
    virtual Serialization::IArchiveHost* GetArchiveHost() const = 0;

    virtual void SetViewCamera(CCamera& Camera) = 0;
    virtual CCamera& GetViewCamera() = 0;

    // Description:
    //   When ignore update sets to true, system will ignore and updates and render calls.
    virtual void IgnoreUpdates(bool bIgnore) = 0;

    // Summary:
    //   Sets the active process
    // Arguments:
    //   process - A pointer to a class that implement the IProcess interface.
    virtual void SetIProcess(IProcess* process) = 0;

    // Summary:
    //   Gets the active process.
    // Return Value:
    //   A pointer to the current active process.
    virtual IProcess* GetIProcess() = 0;

    // Return Value:
    //   True if system running in Test mode.
    virtual bool IsTestMode() const = 0;

    // Summary:
    //   Frame profiler functions
    virtual void SetFrameProfiler(bool on, bool display, char* prefix) = 0;


    //////////////////////////////////////////////////////////////////////////
    // Loading time/memory profiling
    //////////////////////////////////////////////////////////////////////////

    // Summary:
    //   Starts function loading stats profiling.
    virtual struct SLoadingTimeContainer* StartLoadingSectionProfiling(CLoadingTimeProfiler* pProfiler, const char* szFuncName) = 0;

    // Summary:
    //   Ends function loading stats profiling.
    virtual void EndLoadingSectionProfiling(CLoadingTimeProfiler* pProfiler) = 0;

    // Summary:
    //   Starts function profiling with bootprofiler (session must be started).
    virtual CBootProfilerRecord* StartBootSectionProfiler(const char* name, const char* args) = 0;

    // Summary:
    //   Ends function profiling with bootprofiler.
    virtual void StopBootSectionProfiler(CBootProfilerRecord* record) = 0;


    // Summary:
    //   Starts frame session
    virtual void StartBootProfilerSessionFrames(const char* pName) = 0;

    // Summary:
    //   Stops frame session
    virtual void StopBootProfilerSessionFrames() = 0;

    // Summary:
    //   Prints loading stats into log.
    virtual void OutputLoadingTimeStats() = 0;

    // Summary:
    //   Starts function loading stats profiling.
    virtual const char* GetLoadingProfilerCallstack() = 0;

    //////////////////////////////////////////////////////////////////////////
    // File version.
    //////////////////////////////////////////////////////////////////////////

    // Summary:
    //   Gets file version.
    virtual const SFileVersion& GetFileVersion() = 0;
    // Summary:
    //   Gets product version.
    virtual const SFileVersion& GetProductVersion() = 0;
    // Summary:
    //   Gets build version.
    virtual const SFileVersion& GetBuildVersion() = 0;

    // Summary:
    //   Data compression
    //##@{
    virtual bool CompressDataBlock(const void* input, size_t inputSize, void* output, size_t& outputSize, int level = 3) = 0;
    virtual bool DecompressDataBlock(const void* input, size_t inputSize, void* output, size_t& outputSize) = 0;
    //##@}

    //////////////////////////////////////////////////////////////////////////
    // Configuration.
    //////////////////////////////////////////////////////////////////////////

    // Summary:
    //   Loads configurations from CVarGroup directory recursively
    //   If m_GraphicsSettingsMap is defined (in Graphics Settings Dialog box), fills in mapping based on sys_spec_Full
    // Arguments:
    //   sPath - e.g. "Game/Config/CVarGroups"
    virtual void AddCVarGroupDirectory(const string& sPath) = 0;

    // Summary:
    //   Saves system configuration.
    virtual void SaveConfiguration() = 0;

    // Summary:
    //   Loads system configuration
    // Arguments:
    //   pCallback - 0 means normal LoadConfigVar behaviour is used
    virtual void LoadConfiguration(const char* sFilename, ILoadConfigurationEntrySink* pSink = 0, bool warnIfMissing = true) = 0;

    // Summary:
    //   Retrieves current configuration specification for client or server.
    // Arguments:
    //   bClient - If true returns local client config spec, if false returns server config spec.
    virtual ESystemConfigSpec GetConfigSpec(bool bClient = true) = 0;

    virtual ESystemConfigSpec GetMaxConfigSpec() const = 0;

    // Summary:
    //   Changes current configuration specification for client or server.
    // Arguments:
    //   bClient - If true changes client config spec (sys_spec variable changed),
    //             if false changes only server config spec (as known on the client).
    virtual void SetConfigSpec(ESystemConfigSpec spec, ESystemConfigPlatform platform, bool bClient) = 0;

    //////////////////////////////////////////////////////////////////////////

    // Summary:
    //   Retrieves current configuration platform
    virtual ESystemConfigPlatform GetConfigPlatform() const = 0;

    // Summary:
    //   Changes current configuration platform.
    virtual void SetConfigPlatform(const ESystemConfigPlatform platform) = 0;
    //////////////////////////////////////////////////////////////////////////

    // Summary:
    //   Detects and set optimal spec.
    virtual void AutoDetectSpec(const bool detectResolution) = 0;

    // Summary:
    //   Thread management for subsystems
    // Return Value:
    //   Non-0 if the state was indeed changed, 0 if already in that state.
    virtual int SetThreadState(ESubsystem subsys, bool bActive) = 0;

    // Summary:
    //   Creates and returns a usable object implementing ICrySizer interface.
    virtual ICrySizer* CreateSizer() = 0;

    // Summary:
    //   Query if system is now paused.
    //   Pause flag is set when calling system update with pause mode.
    virtual bool IsPaused() const = 0;

    // Summary:
    //   Retrieves localized strings manager interface.
    virtual ILocalizationManager* GetLocalizationManager() = 0;

    // Summary:
    //   Creates an instance of the AVI Reader class.
    virtual IAVI_Reader* CreateAVIReader() = 0;
    // Release the AVI reader
    virtual void ReleaseAVIReader(IAVI_Reader* pAVIReader) = 0;

    virtual ITextModeConsole* GetITextModeConsole() = 0;

    // Summary:
    //   Retrieves the perlin noise singleton instance.
    virtual CPNoise3* GetNoiseGen() = 0;

    // Summary:
    //   Retrieves system update counter.
    virtual uint64 GetUpdateCounter() = 0;

    // Summary:
    //   Gets access to all registered factories.
    virtual ICryFactoryRegistry* GetCryFactoryRegistry() const = 0;

    //////////////////////////////////////////////////////////////////////////
    // Error callback handling

    // Summary:
    //  Registers listeners to CryAssert and error messages. (may not be called if asserts are disabled)
    //  Each pointer can be registered only once. (stl::push_back_unique)
    //  It will return false if the pointer is already registered. Returns true, otherwise.
    virtual bool RegisterErrorObserver(IErrorObserver* errorObserver) = 0;

    // Summary:
    //  Unregisters listeners to CryAssert and error messages.
    //  It will return false if the pointer is not registered. Otherwise, returns true.
    virtual bool UnregisterErrorObserver(IErrorObserver* errorObserver) = 0;

    // Summary:
    //  Called after the processing of the assert message box(Windows or Xbox). // ACCEPTED_USE
    //  It will be called even when asserts are disabled by the console variables.
    virtual void OnAssert(const char* condition, const char* message, const char* fileName, unsigned int fileLineNumber) = 0;

    // Summary:
    // Returns if the assert window from CryAssert is visible.
    // OBS1: needed by the editor, as in some cases it can freeze if during an assert engine it will handle
    // some events such as mouse movement in a CryPhysics assert.
    // OBS2: it will always return false, if asserts are disabled or ignored.
    virtual bool IsAssertDialogVisible() const = 0;

    // Summary:
    // Sets the AssertVisisble internal variable.
    // Typically it should only be called by CryAssert.
    virtual void SetAssertVisible(bool bAssertVisble) = 0;
    //////////////////////////////////////////////////////////////////////////

    // Summary:
    //  Enable/Disable drawing the console 
    virtual void SetConsoleDrawEnabled(bool enabled) = 0;

    //  Enable/Disable drawing the UI 
    virtual void SetUIDrawEnabled(bool enabled) = 0;

    // Summary:
    //   Get the index of the currently running Crytek application. (0 = first instance, 1 = second instance, etc)
    virtual int GetApplicationInstance() = 0;

    // Summary:
    //      Retrieves the current stats for systems to update the respective time taken
    virtual sUpdateTimes& GetCurrentUpdateTimeStats() = 0;

    // Summary:
    //      Retrieves the array of update times and the number of entries
    virtual const sUpdateTimes* GetUpdateTimeStats(uint32&, uint32&) = 0;

    // Summary:
    //      Clear all currently logged and drawn on screen error messages
    virtual void ClearErrorMessages() = 0;
    //////////////////////////////////////////////////////////////////////////
    // For debugging use only!, query current C++ call stack.
    //////////////////////////////////////////////////////////////////////////

    // Notes:
    //   Pass nCount to indicate maximum number of functions to get.
    //   For debugging use only, query current C++ call stack.
    // Description:
    //   Fills array of function pointers, nCount return number of functions.
    virtual void debug_GetCallStack(const char** pFunctions, int& nCount) = 0;
    // Summary:
    //   Logs current callstack.
    // Notes:
    //   For debugging use only!, query current C++ call stack.
    virtual void debug_LogCallStack(int nMaxFuncs = 32, int nFlags = 0) = 0;

    // Summary:
    //   Can be called through console
    // Example:
    //   #System.ApplicationTest("testcase0")
    // Arguments:
    //   szParam - 0 generates error
    virtual void ApplicationTest(const char* szParam) = 0;
    // Return Value:
    //   0 if not activated, activate through #System.ApplicationTest
    virtual ITestSystem* GetITestSystem() = 0;

    // Description:
    //   Execute command line arguments.
    //   Should be after init game.
    // Example:
    //   +g_gametype ASSAULT +map "testy"
    virtual void ExecuteCommandLine(bool deferred=true) = 0;

    // Description:
    //  GetSystemUpdate stats (all systems update without except console)
    //  very useful on dedicated server as we throttle it to fixed frequency
    //  returns zeroes if no updates happened yet
    virtual void GetUpdateStats(SSystemUpdateStats& stats) = 0;

    // Description:
    //   Useful to investigate memory fragmentation.
    //   Every time you call this from the console: #System.DumpMemoryCoverage()
    //   it adds a line to "MemoryCoverage.bmp" (generated the first time, there is a max line count).
    virtual void DumpMemoryCoverage() = 0;
    virtual ESystemGlobalState  GetSystemGlobalState(void) = 0;
    virtual void SetSystemGlobalState(const ESystemGlobalState systemGlobalState) = 0;

    // Summary:
    //      Asynchronous memcpy
    // Note sync variable will be incremented (in calling thread) before job starts
    // and decremented when job finishes. Multiple async copies can therefore be
    // tied to the same sync variable, therefore it's advised to wait for completion with
    // while(*sync) (yield());
    virtual void AsyncMemcpy(void* dst, const void* src, size_t size, int nFlags, volatile int* sync) = 0;
    // </interfuscator:shuffle>


#if defined(CVARS_WHITELIST)
    virtual ICVarsWhitelist* GetCVarsWhiteList() const = 0;
#endif // defined(CVARS_WHITELIST)

#ifndef _RELEASE
    virtual void GetCheckpointData(ICheckpointData& data) = 0;
    virtual void IncreaseCheckpointLoadCount() = 0;
    virtual void SetLoadOrigin(LevelLoadOrigin origin) = 0;
#endif

#if !defined(_RELEASE)
    virtual bool IsSavingResourceList() const = 0;
#endif

    // Initializes Steam if needed and returns if it was successful
    virtual bool SteamInit() = 0;

    virtual const IImageHandler* GetImageHandler() const = 0;

    // Summary:
    //      Loads a dynamic library, creates and initializes an instance of the module class

    virtual bool InitializeEngineModule(const char* dllName, const char* moduleClassName, const SSystemInitParams& initParams) = 0;

    // Summary:
    //      Unloads a dynamic library as well as the corresponding instance of the module class
    virtual bool UnloadEngineModule(const char* dllName, const char* moduleClassName) = 0;

    // Summary:
    //      Gets the root window message handler function
    //      The returned pointer is platform-specific:
    //      For Windows OS, the pointer is of type WNDPROC
    virtual void* GetRootWindowMessageHandler() = 0;

    // Summary:
    //      Register a IWindowMessageHandler that will be informed about window messages
    //      The delivered messages are platform-specific
    virtual void RegisterWindowMessageHandler(IWindowMessageHandler* pHandler) = 0;

    // Summary:
    //      Unregister an IWindowMessageHandler that was previously registered using RegisterWindowMessageHandler
    virtual void UnregisterWindowMessageHandler(IWindowMessageHandler* pHandler) = 0;

    // Deprecated, use AzFramework::ApplicationRequests::PumpSystemEventLoopUntilEmpty instead
    AZ_DEPRECATED(virtual int PumpWindowMessage(bool bAll, WIN_HWND hWnd = 0),
        "PumpWindowMessage has been deprecated, use AzFramework::ApplicationRequests::PumpSystemEventLoopUntilEmpty instead.")
    {
        // AzFramework::ApplicationRequests::Bus::Broadcast(&AzFramework::ApplicationRequests::PumpSystemEventLoopUntilEmpty);
        return 0;
    }

    // Create an instance of a Local File IO object (which reads directly off the local filesystem, instead of,
    // for example, reading from the network or a pack or USB or such.
    virtual std::shared_ptr<AZ::IO::FileIOBase> CreateLocalFileIO() = 0;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // EBus interface used to listen for cry system notifications
    class CrySystemNotifications : public AZ::EBusTraits
    {
    public:
        virtual ~CrySystemNotifications() = default;

        // Override to be notified right before the call to ISystem::Render
        virtual void OnPreRender() {}
    };
    using CrySystemNotificationBus = AZ::EBus<CrySystemNotifications>;
};

//JAT - this is a very important function for the dedicated server - it lets us run >1000 players per piece of server hardware
//JAT - this saves us lots of money on the dedicated server hardware
#define SYNCHRONOUS_LOADING_TICK() do { if (gEnv && gEnv->pSystem) {gEnv->pSystem->SynchronousLoadingTick(__FUNC__, __LINE__); } \
} while (0)

#if defined(ENABLE_LOADING_PROFILER)

struct DiskOperationInfo
{
    DiskOperationInfo()
        : m_nSeeksCount(0)
        , m_nFileOpenCount(0)
        , m_nFileReadCount(0)
        , m_dOperationSize(0.)
        , m_dOperationTime(0.) {}
    int m_nSeeksCount;
    int m_nFileOpenCount;
    int m_nFileReadCount;
    double m_dOperationTime;
    double m_dOperationSize;

    DiskOperationInfo& operator -= (const DiskOperationInfo& rv)
    {
        m_nSeeksCount -= rv.m_nSeeksCount;
        m_nFileOpenCount -= rv.m_nFileOpenCount;
        m_nFileReadCount -= rv.m_nFileReadCount;
        m_dOperationSize -= rv.m_dOperationSize;
        m_dOperationTime -= rv.m_dOperationTime;
        return *this;
    }

    DiskOperationInfo& operator += (const DiskOperationInfo& rv)
    {
        m_nSeeksCount += rv.m_nSeeksCount;
        m_nFileOpenCount += rv.m_nFileOpenCount;
        m_nFileReadCount += rv.m_nFileReadCount;
        m_dOperationSize += rv.m_dOperationSize;
        m_dOperationTime += rv.m_dOperationTime;
        return *this;
    }

    DiskOperationInfo& operator - (const DiskOperationInfo& rv)
    {
        DiskOperationInfo res(*this);
        return res -= rv;
    }

    DiskOperationInfo& operator + (const DiskOperationInfo& rv)
    {
        DiskOperationInfo res(*this);
        return res += rv;
    }
};

struct CLoadingTimeProfiler
{
    CLoadingTimeProfiler(ISystem* pSystem, const char* szFuncName)
        : m_pSystem(pSystem)
    {
        m_pSystem = pSystem;
        m_pTimeContainer = m_pSystem->StartLoadingSectionProfiling(this, szFuncName);
    }

    ~CLoadingTimeProfiler()
    {
        m_pSystem->EndLoadingSectionProfiling(this);
    }

    struct SLoadingTimeContainer* m_pTimeContainer;
    double m_fConstructorTime;
    double m_fConstructorMemUsage;

    DiskOperationInfo m_constructorInfo;

    ISystem* m_pSystem;
};

class CSYSBootProfileBlock
{
    ISystem* m_pSystem;
    CBootProfilerRecord* m_pRecord;
public:
    CSYSBootProfileBlock(ISystem* pSystem, const char* name, const char* args = NULL)
        : m_pSystem(pSystem)
    {
        m_pRecord = m_pSystem->StartBootSectionProfiler(name, args);
    }

    ~CSYSBootProfileBlock()
    {
        m_pSystem->StopBootSectionProfiler(m_pRecord);
    }
};

#ifdef AZ_PROFILE_TELEMETRY

#define LOADING_TIME_PROFILE_SECTION AZ_PROFILE_FUNCTION(AZ::Debug::ProfileCategory::AzCore)
#define LOADING_TIME_PROFILE_SECTION_ARGS(...) AZ_PROFILE_SCOPE_DYNAMIC(AZ::Debug::ProfileCategory::AzCore, __VA_ARGS__)
#define LOADING_TIME_PROFILE_SECTION_NAMED(sectionName) AZ_PROFILE_SCOPE(AZ::Debug::ProfileCategory::AzCore, sectionName)
#define LOADING_TIME_PROFILE_SECTION_NAMED_ARGS(sectionName, args) AZ_PROFILE_SCOPE_DYNAMIC(AZ::Debug::ProfileCategory::AzCore, sectionName, args)

#else

#define LOADING_TIME_PROFILE_SECTION CSYSBootProfileBlock AZ_JOIN(_profileBlockLine, __LINE__)(gEnv->pSystem, __FUNCTION__);
#define LOADING_TIME_PROFILE_SECTION_ARGS(args) CSYSBootProfileBlock AZ_JOIN(_profileBlockLine, __LINE__)(gEnv->pSystem, __FUNCTION__, args);
#define LOADING_TIME_PROFILE_SECTION_NAMED(sectionName) CSYSBootProfileBlock AZ_JOIN(_profileBlockLine, __LINE__)(gEnv->pSystem, sectionName);
#define LOADING_TIME_PROFILE_SECTION_NAMED_ARGS(sectionName, args) CSYSBootProfileBlock AZ_JOIN(_profileBlockLine, __LINE__)(gEnv->pSystem, sectionName, args);

#endif // AZ_PROFILE_TELEMETRY

#else

#define LOADING_TIME_PROFILE_SECTION
#define LOADING_TIME_PROFILE_SECTION_ARGS(args)
#define LOADING_TIME_PROFILE_SECTION_NAMED(sectionName)
#define LOADING_TIME_PROFILE_SECTION_NAMED_ARGS(sectionName, args)
#define LOADING_TIME_PROFILE_SESSION_SECTION(sessionName)
#define LOADING_TIME_PROFILE_SESSION_START(sessionName)
#define LOADING_TIME_PROFILE_SESSION_STOP(sessionName)

#endif

//////////////////////////////////////////////////////////////////////////
// CrySystem DLL Exports.
//////////////////////////////////////////////////////////////////////////
typedef ISystem* (*PFNCREATESYSTEMINTERFACE)(SSystemInitParams& initParams);


//////////////////////////////////////////////////////////////////////////
// Global environment variable.
//////////////////////////////////////////////////////////////////////////
extern SC_API SSystemGlobalEnvironment* gEnv;


// Summary:
//   Gets the system interface.
inline ISystem* GetISystem()
{
    // Some unit tests temporarily install and then uninstall ISystem* mocks.
    // It is generally okay for runtime and tool systems which call this function to cache the returned pointer, 
    // because their lifetime is usually shorter than the lifetime of the ISystem* implementation.
    // It is NOT safe for this function to cache it as a static itself, though, as the static it would cache
    // it inside may outlive the the actual instance implementing ISystem* when unit tests are torn down and then restarted.
    ISystem* systemInterface = gEnv ? gEnv->pSystem : nullptr;
    if (!systemInterface)
    {
        CrySystemRequestBus::BroadcastResult(systemInterface, &CrySystemRequests::GetCrySystem);
    }
    return systemInterface;
};

inline ISystemScheduler* GetISystemScheduler(void)
{
    return gEnv->pSystemScheduler;
};
//////////////////////////////////////////////////////////////////////////

// Description:
//   This function must be called once by each module at the beginning, to setup global pointers.
extern "C" AZ_DLL_EXPORT void ModuleInitISystem(ISystem* pSystem, const char* moduleName);
extern bool g_bProfilerEnabled;
extern int g_iTraceAllocations;

// Summary:
//   Interface of the DLL.
extern "C"
{
    CRYSYSTEM_API ISystem* CreateSystemInterface(const SSystemInitParams& initParams);
}

// Description:
//   Displays error message.
//   Logs it to console and file and error message box.
//   Then terminates execution.
void CryFatalError(const char*, ...) PRINTF_PARAMS(1, 2);
inline void CryFatalError(const char* format, ...)
{
    if (!gEnv || !gEnv->pSystem)
    {
        return;
    }

    va_list ArgList;
    char szBuffer[MAX_WARNING_LENGTH];
    va_start(ArgList, format);
    int count = azvsnprintf(szBuffer, sizeof(szBuffer), format, ArgList);
    if (count == -1 || count >= sizeof(szBuffer))
    {
        szBuffer[sizeof(szBuffer) - 1] = '\0';
    }
    va_end(ArgList);

    gEnv->pSystem->FatalError("%s", szBuffer);
}

//////////////////////////////////////////////////////////////////////////

// Description:
//   Displays warning message.
//   Logs it to console and file and display a warning message box.
//   Doesn't terminate execution.
void CryWarning(EValidatorModule, EValidatorSeverity, const char*, ...) PRINTF_PARAMS(3, 4);
inline void CryWarning(EValidatorModule module, EValidatorSeverity severity, const char* format, ...)
{
    if (!gEnv || !gEnv->pSystem || !format)
    {
        return;
    }

    va_list args;
    va_start(args, format);
    GetISystem()->WarningV(module, severity, 0, 0, format, args);
    va_end(args);
}

#ifdef EXCLUDE_CVARHELP
#define CVARHELP(_comment)  0
#else
#define CVARHELP(_comment)  _comment
#endif

//Provide macros for fixing cvars for release mode on consoles to enums to allow for code stripping
//Do not enable for PC, apply VF_CHEAT there if required
#if defined(CONSOLE)
#define CONST_CVAR_FLAGS (VF_CHEAT)
#else
#define CONST_CVAR_FLAGS (VF_NULL)
#endif

#if defined(AZ_RESTRICTED_PLATFORM)
#define AZ_RESTRICTED_SECTION ISYSTEM_H_SECTION_5
    #if defined(AZ_PLATFORM_XENIA)
        #include "Xenia/ISystem_h_xenia.inl"
    #elif defined(AZ_PLATFORM_PROVO)
        #include "Provo/ISystem_h_provo.inl"
    #endif
#endif
#if defined(_RELEASE) && defined(IS_CONSOLE_PLATFORM)
#ifndef LOG_CONST_CVAR_ACCESS
#error LOG_CONST_CVAR_ACCESS should be defined in ProjectDefines.h
#endif

#include "IConsole.h"
namespace Detail
{
    template<typename T>
    struct SQueryTypeEnum;
    template<>
    struct SQueryTypeEnum<int>
    {
        static const int type = CVAR_INT;
        static int ParseString(const char* s) { return atoi(s); }
    };
    template<>
    struct SQueryTypeEnum<float>
    {
        static const int type = CVAR_FLOAT;
        static float ParseString(const char* s) { return (float)atof(s); }
    };

    template<typename T>
    struct SDummyCVar
        : ICVar
    {
        const T value;
#if LOG_CONST_CVAR_ACCESS
        mutable bool bWasRead;
        mutable bool bWasChanged;
        SDummyCVar(T val)
            : value(val)
            , bWasChanged(false)
            , bWasRead(false) {}
#else
        SDummyCVar(T val)
            : value(val) {}
#endif

        void WarnUse() const
        {
#if LOG_CONST_CVAR_ACCESS
            if (!bWasRead)
            {
                CryWarning(VALIDATOR_MODULE_SYSTEM, VALIDATOR_WARNING, "[CVAR] Read from const CVar '%s' via name look-up, this is non-optimal", GetName());
                bWasRead = true;
            }
#endif
        }

        void InvalidAccess() const
        {
#if LOG_CONST_CVAR_ACCESS
            if (!bWasChanged)
            {
                CryWarning(VALIDATOR_MODULE_SYSTEM, VALIDATOR_ERROR, "[CVAR] Write to const CVar '%s' with wrong value '%f' was ignored. This indicates a bug in code or a config file", GetName(), GetFVal());
                bWasChanged = true;
            }
#endif
        }

        void Release() {}
        int GetIVal() const { WarnUse(); return static_cast<int>(value); }
        int64 GetI64Val() const { WarnUse(); return static_cast<int64>(value); }
        float GetFVal() const { WarnUse(); return static_cast<float>(value); }
        const char* GetString() const { return ""; }
        const char* GetDataProbeString() const { return ""; }
        void Set(const char* s)
        {
            if (SQueryTypeEnum<T>::ParseString(s) != value)
            {
                InvalidAccess();
            }
        }
        void ForceSet(const char* s) { Set(s); }
        void Set(const float f)
        {
            if (static_cast<T>(f) != value)
            {
                InvalidAccess();
            }
        }
        void Set(const int i)
        {
            if (static_cast<T>(i) != value)
            {
                InvalidAccess();
            }
        }
        void ClearFlags(int flags) {}
        int GetFlags() const { return VF_CONST_CVAR | VF_READONLY; }
        int SetFlags(int flags) { return 0; }
        int GetType() { return SQueryTypeEnum<T>::type; }
        const char* GetHelp() { return NULL; }
        bool IsConstCVar() const { return true; }
        void SetOnChangeCallback(ConsoleVarFunc pChangeFunc) { (void)pChangeFunc; }
        uint64 AddOnChangeFunctor(const SFunctor& pChangeFunctor) { (void)pChangeFunctor; return 0; }
        uint64 GetNumberOfOnChangeFunctors() const { return 0; }
        const SFunctor& GetOnChangeFunctor(uint64 nFunctorIndex) const { InvalidAccess(); SFunctor* pNull = nullptr; return *pNull; }
        bool RemoveOnChangeFunctor(const uint64 nElement) { return true; }
        ConsoleVarFunc GetOnChangeCallback() const { InvalidAccess(); return NULL; }
        void GetMemoryUsage(class ICrySizer* pSizer) const {}
        int GetRealIVal() const { return GetIVal(); }
        void SetLimits(float min, float max) { return; }
        void GetLimits(float& min, float& max) { return; }
        bool HasCustomLimits() { return false; }
        void SetDataProbeString(const char* pDataProbeString) { InvalidAccess(); }
    };
}

#define REGISTER_DUMMY_CVAR(type, name, value)                                           \
    do {                                                                                 \
        static struct DummyCVar                                                          \
            : Detail::SDummyCVar<type>                                                   \
        {                                                                                \
            DummyCVar()                                                                  \
                : Detail::SDummyCVar<type>(value) {}                                     \
            const char* GetName() const { return name; }                                 \
        } DummyStaticInstance;                                                           \
        if (!(gEnv->pConsole != 0 ? gEnv->pConsole->Register(&DummyStaticInstance) : 0)) \
        {                                                                                \
            DEBUG_BREAK;                                                                 \
            CryFatalError("Can not register dummy CVar");                                \
        }                                                                                \
    } while (0)

# define CONSOLE_CONST_CVAR_MODE
# define DeclareConstIntCVar(name, defaultValue) enum { name = (defaultValue) }
# define DeclareStaticConstIntCVar(name, defaultValue) enum { name = (defaultValue) }

# define DefineConstIntCVarName(strname, name, defaultValue, flags, help) { COMPILE_TIME_ASSERT((int)(defaultValue) == (int)(name)); REGISTER_DUMMY_CVAR(int, strname, defaultValue); }
# define DefineConstIntCVar(name, defaultValue, flags, help) { COMPILE_TIME_ASSERT((int)(defaultValue) == (int)(name)); REGISTER_DUMMY_CVAR(int, (#name), defaultValue); }
// DefineConstIntCVar2 is deprecated, any such instance can be converted to the 3 variant by removing the quotes around the first parameter
# define DefineConstIntCVar3(name, _var_, defaultValue, flags, help) { COMPILE_TIME_ASSERT((int)(defaultValue) == (int)(_var_)); REGISTER_DUMMY_CVAR(int, name, defaultValue); }
# define AllocateConstIntCVar(scope, name)

# define DefineConstFloatCVar(name, flags, help) { REGISTER_DUMMY_CVAR(float, (#name), name ## Default); }
# define DeclareConstFloatCVar(name)
# define DeclareStaticConstFloatCVar(name)
# define AllocateConstFloatCVar(scope, name)

#else

# define DeclareConstIntCVar(name, defaultValue) int name
# define DeclareStaticConstIntCVar(name, defaultValue) static int name
# define DefineConstIntCVarName(strname, name, defaultValue, flags, help) \
    (gEnv->pConsole == 0 ? 0 : gEnv->pConsole->Register(strname, &name, defaultValue, flags | CONST_CVAR_FLAGS, CVARHELP(help)))
# define DefineConstIntCVar(name, defaultValue, flags, help) \
    (gEnv->pConsole == 0 ? 0 : gEnv->pConsole->Register((#name), &name, defaultValue, flags | CONST_CVAR_FLAGS, CVARHELP(help), 0, false))
// DefineConstIntCVar2 is deprecated, any such instance can be converted to the 3 variant by removing the quotes around the first parameter
# define DefineConstIntCVar3(_name, _var, _def_val, _flags, help) \
    (gEnv->pConsole == 0 ? 0 : gEnv->pConsole->Register(_name, &(_var), (_def_val), (_flags) | CONST_CVAR_FLAGS, CVARHELP(help), 0, false))
# define AllocateConstIntCVar(scope, name) int scope:: name

# define DefineConstFloatCVar(name, flags, help) \
    (gEnv->pConsole == 0 ? 0 : gEnv->pConsole->Register((#name), &name, name ## Default, flags | CONST_CVAR_FLAGS, CVARHELP(help), 0, false))
# define DeclareConstFloatCVar(name) float name
# define DeclareStaticConstFloatCVar(name) static float name
# define AllocateConstFloatCVar(scope, name) float scope:: name
#endif

#if defined(USE_CRY_ASSERT)
static void AssertConsoleExists(void)
{
    CRY_ASSERT(gEnv->pConsole != NULL);
}
#define ASSERT_CONSOLE_EXISTS AssertConsoleExists()
#else
#define ASSERT_CONSOLE_EXISTS 0
#endif // defined(USE_CRY_ASSERT)

// the following macros allow the help text to be easily stripped out

// Summary:
//   Preferred way to register a CVar
#define REGISTER_CVAR(_var, _def_val, _flags, _comment)                                                            (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? 0 : gEnv->pConsole->Register((#_var), &(_var), (_def_val), (_flags), CVARHELP(_comment)))
// Summary:
//   Preferred way to register a CVar with a callback
#define REGISTER_CVAR_CB(_var, _def_val, _flags, _comment, _onchangefunction)                   (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? 0 : gEnv->pConsole->Register((#_var), &(_var), (_def_val), (_flags), CVARHELP(_comment), _onchangefunction))
// Summary:
//   Preferred way to register a string CVar
#define REGISTER_STRING(_name, _def_val, _flags, _comment)                                                     (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? 0 : gEnv->pConsole->RegisterString(_name, (_def_val), (_flags), CVARHELP(_comment)))
// Summary:
//   Preferred way to register a string CVar with a callback
#define REGISTER_STRING_CB(_name, _def_val, _flags, _comment, _onchangefunction)            (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? 0 : gEnv->pConsole->RegisterString(_name, (_def_val), (_flags), CVARHELP(_comment), _onchangefunction))
// Summary:
//   Preferred way to register an int CVar
#define REGISTER_INT(_name, _def_val, _flags, _comment)                                                            (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? 0 : gEnv->pConsole->RegisterInt(_name, (_def_val), (_flags), CVARHELP(_comment)))
// Summary:
//   Preferred way to register an int CVar with a callback
#define REGISTER_INT_CB(_name, _def_val, _flags, _comment, _onchangefunction)                   (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? 0 : gEnv->pConsole->RegisterInt(_name, (_def_val), (_flags), CVARHELP(_comment), _onchangefunction))
// Summary:
//   Preferred way to register an int64 CVar
#define REGISTER_INT64(_name, _def_val, _flags, _comment)                                                      (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? 0 : gEnv->pConsole->RegisterInt64(_name, (_def_val), (_flags), CVARHELP(_comment)))
// Summary:
//   Preferred way to register an int64 CVar with a callback
#define REGISTER_INT64_CB(_name, _def_val, _flags, _comment, _onchangefunction)                                                     (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? 0 : gEnv->pConsole->RegisterInt64(_name, (_def_val), (_flags), CVARHELP(_comment), _onchangefunction))
// Summary:
//   Preferred way to register a float CVar
#define REGISTER_FLOAT(_name, _def_val, _flags, _comment)                                                      (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? 0 : gEnv->pConsole->RegisterFloat(_name, (_def_val), (_flags), CVARHELP(_comment)))
// Summary:
//   Preferred way to register a float CVar with a callback
#define REGISTER_FLOAT_CB(_name, _def_val, _flags, _comment, _onchangefunction)                 (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? 0 : gEnv->pConsole->RegisterFloat(_name, (_def_val), (_flags), CVARHELP(_comment), _onchangefunction))
// Summary:
//   Offers more flexibility but more code is required
#define REGISTER_CVAR2(_name, _var, _def_val, _flags, _comment)                                             (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? 0 : gEnv->pConsole->Register(_name, _var, (_def_val), (_flags), CVARHELP(_comment)))
// Summary:
//   Offers more flexibility but more code is required
#define REGISTER_CVAR2_CB(_name, _var, _def_val, _flags, _comment, _onchangefunction)    (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? 0 : gEnv->pConsole->Register(_name, _var, (_def_val), (_flags), CVARHELP(_comment), _onchangefunction))
// Summary:
//   Offers more flexibility but more code is required, explicit address taking of destination variable
#define REGISTER_CVAR3(_name, _var, _def_val, _flags, _comment)                                             (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? 0 : gEnv->pConsole->Register(_name, &(_var), (_def_val), (_flags), CVARHELP(_comment)))
// Summary:
//   Offers more flexibility but more code is required, explicit address taking of destination variable
#define REGISTER_CVAR3_CB(_name, _var, _def_val, _flags, _comment, _onchangefunction)    (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? 0 : gEnv->pConsole->Register(_name, &(_var), (_def_val), (_flags), CVARHELP(_comment), _onchangefunction))
// Summary:
//   Preferred way to register a console command
#define REGISTER_COMMAND(_name, _func, _flags, _comment)                                                           (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? false : gEnv->pConsole->AddCommand(_name, _func, (_flags), CVARHELP(_comment)))
// Summary:
//   Preferred way to unregister a CVar
#define UNREGISTER_CVAR(_name)                                                      (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? (void)0 : gEnv->pConsole->UnregisterVariable(_name))
//   Preferred way to unregister a console command
#define UNREGISTER_COMMAND(_name)                                                           (ASSERT_CONSOLE_EXISTS, gEnv->pConsole == 0 ? (void)0 : gEnv->pConsole->RemoveCommand(_name))

////////////////////////////////////////////////////////////////////////////////
//
// Development only cvars
//
// N.B:
// (1) Registered as real cvars *in non release builds only*.
// (2) Can still be manipulated in release by the mapped variable, so not the same as const cvars.
// (3) Any 'OnChanged' callback will need guarding against in release builds since the cvar won't exist
// (4) Any code that tries to get ICVar* will need guarding against in release builds since the cvar won't exist
//
// ILLEGAL_DEV_FLAGS is a mask of all those flags which make no sense in a _DEV_ONLY or _DEDI_ONLY cvar since the
// cvar potentially won't exist in a release build.
//
#define ILLEGAL_DEV_FLAGS (VF_NET_SYNCED | VF_CHEAT | VF_CHEAT_ALWAYS_CHECK | VF_CHEAT_NOCHECK | VF_READONLY | VF_CONST_CVAR)

#if defined(_RELEASE)
#define REGISTER_CVAR_DEV_ONLY(_var, _def_val, _flags, _comment)                                                               NULL; COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0); _var = _def_val
#define REGISTER_CVAR_CB_DEV_ONLY(_var, _def_val, _flags, _comment, _onchangefunction)                  NULL; COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0); _var = _def_val /* _onchangefunction consumed; callback not available */
#define REGISTER_STRING_DEV_ONLY(_name, _def_val, _flags, _comment)                                                        NULL; COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)                                  /* consumed; pure cvar not available */
#define REGISTER_STRING_CB_DEV_ONLY(_name, _def_val, _flags, _comment, _onchangefunction)               NULL; COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)                                  /* consumed; pure cvar not available */
#define REGISTER_INT_DEV_ONLY(_name, _def_val, _flags, _comment)                                                               NULL; COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)                                  /* consumed; pure cvar not available */
#define REGISTER_INT_CB_DEV_ONLY(_name, _def_val, _flags, _comment, _onchangefunction)                  NULL; COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)                                  /* consumed; pure cvar not available */
#define REGISTER_INT64_DEV_ONLY(_name, _def_val, _flags, _comment)                                                         NULL; COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)                                  /* consumed; pure cvar not available */
#define REGISTER_FLOAT_DEV_ONLY(_name, _def_val, _flags, _comment)                                                         NULL; COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)                                  /* consumed; pure cvar not available */
#define REGISTER_CVAR2_DEV_ONLY(_name, _var, _def_val, _flags, _comment)                                                NULL; COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0); *(_var) = _def_val
#define REGISTER_CVAR2_CB_DEV_ONLY(_name, _var, _def_val, _flags, _comment, _onchangefunction)       NULL; COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0); *(_var) = _def_val
#define REGISTER_CVAR3_DEV_ONLY(_name, _var, _def_val, _flags, _comment)                                                NULL; COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0); _var = _def_val
#define REGISTER_CVAR3_CB_DEV_ONLY(_name, _var, _def_val, _flags, _comment, _onchangefunction)       NULL; COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0); _var = _def_val
#define REGISTER_COMMAND_DEV_ONLY(_name, _func, _flags, _comment)                                                  /* consumed; command not available */
#else
#define REGISTER_CVAR_DEV_ONLY(_var, _def_val, _flags, _comment)                                                               REGISTER_CVAR(_var, _def_val, ((_flags) | VF_DEV_ONLY), _comment); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_CVAR_CB_DEV_ONLY(_var, _def_val, _flags, _comment, _onchangefunction)                  REGISTER_CVAR_CB(_var, _def_val, ((_flags) | VF_DEV_ONLY), _comment, _onchangefunction); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_STRING_DEV_ONLY(_name, _def_val, _flags, _comment)                                                        REGISTER_STRING(_name, _def_val, ((_flags) | VF_DEV_ONLY), _comment); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_STRING_CB_DEV_ONLY(_name, _def_val, _flags, _comment, _onchangefunction)               REGISTER_STRING_CB(_name, _def_val, ((_flags) | VF_DEV_ONLY), _comment, _onchangefunction); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_INT_DEV_ONLY(_name, _def_val, _flags, _comment)                                                               REGISTER_INT(_name, _def_val, ((_flags) | VF_DEV_ONLY), _comment); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_INT_CB_DEV_ONLY(_name, _def_val, _flags, _comment, _onchangefunction)                  REGISTER_INT_CB(_name, _def_val, ((_flags) | VF_DEV_ONLY), _comment, _onchangefunction); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_INT64_DEV_ONLY(_name, _def_val, _flags, _comment)                                                         REGISTER_INT64(_name, _def_val, ((_flags) | VF_DEV_ONLY), _comment); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_FLOAT_DEV_ONLY(_name, _def_val, _flags, _comment)                                                         REGISTER_FLOAT(_name, _def_val, ((_flags) | VF_DEV_ONLY), _comment); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_CVAR2_DEV_ONLY(_name, _var, _def_val, _flags, _comment)                                                REGISTER_CVAR2(_name, _var, _def_val, ((_flags) | VF_DEV_ONLY), _comment); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_CVAR2_CB_DEV_ONLY(_name, _var, _def_val, _flags, _comment, _onchangefunction)       REGISTER_CVAR2_CB(_name, _var, _def_val, ((_flags) | VF_DEV_ONLY), _comment, _onchangefunction); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_CVAR3_DEV_ONLY(_name, _var, _def_val, _flags, _comment)                                                REGISTER_CVAR3(_name, _var, _def_val, ((_flags) | VF_DEV_ONLY), _comment); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_CVAR3_CB_DEV_ONLY(_name, _var, _def_val, _flags, _comment, _onchangefunction)       REGISTER_CVAR3_CB(_name, _var, _def_val, ((_flags) | VF_DEV_ONLY), _comment, _onchangefunction); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_COMMAND_DEV_ONLY(_name, _func, _flags, _comment)                                                          REGISTER_COMMAND(_name, _func, ((_flags) | VF_DEV_ONLY), _comment); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#endif // defined(_RELEASE)
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// Dedicated server only cvars
//
// N.B:
// (1) Registered as real cvars in all non release builds
// (2) Registered as real cvars in release on dedi servers only, otherwise treated as DEV_ONLY type cvars (see above)
//
#if defined(_RELEASE) && defined(DEDICATED_SERVER)
#define REGISTER_CVAR_DEDI_ONLY(_var, _def_val, _flags, _comment)                                                          REGISTER_CVAR(_var, _def_val, ((_flags) | VF_DEDI_ONLY), _comment); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_CVAR_CB_DEDI_ONLY(_var, _def_val, _flags, _comment, _onchangefunction)                 REGISTER_CVAR_CB(_var, _def_val, ((_flags) | VF_DEDI_ONLY), _comment, _onchangefunction); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_STRING_DEDI_ONLY(_name, _def_val, _flags, _comment)                                                       REGISTER_STRING(_name, _def_val, ((_flags) | VF_DEDI_ONLY), _comment); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_STRING_CB_DEDI_ONLY(_name, _def_val, _flags, _comment, _onchangefunction)          REGISTER_STRING_CB(_name, _def_val, ((_flags) | VF_DEDI_ONLY), _comment, _onchangefunction); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_INT_DEDI_ONLY(_name, _def_val, _flags, _comment)                                                          REGISTER_INT(_name, _def_val, ((_flags) | VF_DEDI_ONLY), _comment); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_INT_CB_DEDI_ONLY(_name, _def_val, _flags, _comment, _onchangefunction)                 REGISTER_INT_CB(_name, _def_val, ((_flags) | VF_DEDI_ONLY), _comment, _onchangefunction); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_INT64_DEDI_ONLY(_name, _def_val, _flags, _comment)                                                        REGISTER_INT64(_name, _def_val, ((_flags) | VF_DEDI_ONLY), _comment); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_FLOAT_DEDI_ONLY(_name, _def_val, _flags, _comment)                                                        REGISTER_FLOAT(_name, _def_val, ((_flags) | VF_DEDI_ONLY), _comment); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_CVAR2_DEDI_ONLY(_name, _var, _def_val, _flags, _comment)                                               REGISTER_CVAR2(_name, _var, _def_val, ((_flags) | VF_DEDI_ONLY), _comment); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_CVAR2_CB_DEDI_ONLY(_name, _var, _def_val, _flags, _comment, _onchangefunction)  REGISTER_CVAR2_CB(_name, _var, _def_val, ((_flags) | VF_DEDI_ONLY), _comment, _onchangefunction); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_CVAR3_DEDI_ONLY(_name, _var, _def_val, _flags, _comment)                                               REGISTER_CVAR3(_name, _var, _def_val, ((_flags) | VF_DEDI_ONLY), _comment); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_CVAR3_CB_DEDI_ONLY(_name, _var, _def_val, _flags, _comment, _onchangefunction)  REGISTER_CVAR3_CB(_name, _var, _def_val, ((_flags) | VF_DEDI_ONLY), _comment, _onchangefunction); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#define REGISTER_COMMAND_DEDI_ONLY(_name, _func, _flags, _comment)                                                         REGISTER_COMMAND(_name, _func, ((_flags) | VF_DEDI_ONLY), _comment); COMPILE_TIME_ASSERT(((_flags) & ILLEGAL_DEV_FLAGS) == 0)
#else
#define REGISTER_CVAR_DEDI_ONLY(_var, _def_val, _flags, _comment)                                                          REGISTER_CVAR_DEV_ONLY(_var, _def_val, ((_flags) | VF_DEDI_ONLY), _comment)
#define REGISTER_CVAR_CB_DEDI_ONLY(_var, _def_val, _flags, _comment, _onchangefunction)                 REGISTER_CVAR_CB_DEV_ONLY(_var, _def_val, ((_flags) | VF_DEDI_ONLY), _comment, _onchangefunction)
#define REGISTER_STRING_DEDI_ONLY(_name, _def_val, _flags, _comment)                                                       REGISTER_STRING_DEV_ONLY(_name, _def_val, ((_flags) | VF_DEDI_ONLY), _comment)
#define REGISTER_STRING_CB_DEDI_ONLY(_name, _def_val, _flags, _comment, _onchangefunction)          REGISTER_STRING_CB_DEV_ONLY(_name, _def_val, ((_flags) | VF_DEDI_ONLY), _comment, _onchangefunction)
#define REGISTER_INT_DEDI_ONLY(_name, _def_val, _flags, _comment)                                                          REGISTER_INT_DEV_ONLY(_name, _def_val, ((_flags) | VF_DEDI_ONLY), _comment)
#define REGISTER_INT_CB_DEDI_ONLY(_name, _def_val, _flags, _comment, _onchangefunction)                 REGISTER_INT_CB_DEV_ONLY(_name, _def_val, ((_flags) | VF_DEDI_ONLY), _comment, _onchangefunction)
#define REGISTER_INT64_DEDI_ONLY(_name, _def_val, _flags, _comment)                                                        REGISTER_INT64_DEV_ONLY(_name, _def_val, ((_flags) | VF_DEDI_ONLY), _comment)
#define REGISTER_FLOAT_DEDI_ONLY(_name, _def_val, _flags, _comment)                                                        REGISTER_FLOAT_DEV_ONLY(_name, _def_val, ((_flags) | VF_DEDI_ONLY), _comment)
#define REGISTER_CVAR2_DEDI_ONLY(_name, _var, _def_val, _flags, _comment)                                               REGISTER_CVAR2_DEV_ONLY(_name, _var, _def_val, ((_flags) | VF_DEDI_ONLY), _comment)
#define REGISTER_CVAR2_CB_DEDI_ONLY(_name, _var, _def_val, _flags, _comment, _onchangefunction)  REGISTER_CVAR2_CB_DEV_ONLY(_name, _var, _def_val, ((_flags) | VF_DEDI_ONLY), _comment, _onchangefunction)
#define REGISTER_CVAR3_DEDI_ONLY(_name, _var, _def_val, _flags, _comment)                                               REGISTER_CVAR3_DEV_ONLY(_name, _var, _def_val, ((_flags) | VF_DEDI_ONLY), _comment)
#define REGISTER_CVAR3_CB_DEDI_ONLY(_name, _var, _def_val, _flags, _comment, _onchangefunction)  REGISTER_CVAR3_CB_DEV_ONLY(_name, _var, _def_val, ((_flags) | VF_DEDI_ONLY), _comment, _onchangefunction)
#define REGISTER_COMMAND_DEDI_ONLY(_name, _func, _flags, _comment)                                                         REGISTER_COMMAND_DEV_ONLY(_name, _func, ((_flags) | VF_DEDI_ONLY), _comment)
#endif // defined(_RELEASE)
//
////////////////////////////////////////////////////////////////////////////////

#ifdef EXCLUDE_NORMAL_LOG               // setting this removes a lot of logging to reduced code size (useful for consoles)

#define CryLog(...) ((void)0)
#define CryComment(...) ((void)0)
#define CryLogAlways(...) ((void)0)

#else // EXCLUDE_NORMAL_LOG

// Summary:
//   Simple logs of data with low verbosity.
void CryLog(const char*, ...) PRINTF_PARAMS(1, 2);
inline void CryLog(const char* format, ...)
{
    // Fran: we need these guards for the testing framework to work
    if (gEnv && gEnv->pSystem && gEnv->pLog)
    {
        va_list args;
        va_start(args, format);
        gEnv->pLog->LogV(ILog::eMessage, format, args);
        va_end(args);
    }
}
// Notes:
//   Very rarely used log comment.
void CryComment(const char*, ...) PRINTF_PARAMS(1, 2);
inline void CryComment(const char* format, ...)
{
    // Fran: we need these guards for the testing framework to work
    if (gEnv && gEnv->pSystem && gEnv->pLog)
    {
        va_list args;
        va_start(args, format);
        gEnv->pLog->LogV(ILog::eComment, format, args);
        va_end(args);
    }
}
// Summary:
//   Logs important data that must be printed regardless verbosity.
void CryLogAlways(const char*, ...) PRINTF_PARAMS(1, 2);
inline void CryLogAlways(const char* format, ...)
{
    // log should not be used before system is ready
    // error before system init should be handled explicitly

    // Fran: we need these guards for the testing framework to work

    if (gEnv && gEnv->pSystem && gEnv->pLog)
    {
        //      assert(gEnv);
        //      assert(gEnv->pSystem);

        va_list args;
        va_start(args, format);
        gEnv->pLog->LogV(ILog::eAlways, format, args);
        va_end(args);
    }
}

#endif // EXCLUDE_NORMAL_LOG

/*****************************************************
ASYNC MEMCPY FUNCTIONS
*****************************************************/

// Complex delegation required because it is not really easy to
// export a external standalone symbol like a memcpy function when
// building with modules. Dll pay an extra indirection cost for calling this
// function.
#if !defined(AZ_MONOLITHIC_BUILD)
# define CRY_ASYNC_MEMCPY_DELEGATE_TO_CRYSYSTEM
#endif
#define CRY_ASYNC_MEMCPY_API extern "C"

// Note sync variable will be incremented (in calling thread) before job starts
// and decremented when job finishes. Multiple async copies can therefore be
// tied to the same sync variable, therefore wait for completion with
// while(*sync) (yield());
#if defined(CRY_ASYNC_MEMCPY_DELEGATE_TO_CRYSYSTEM)
inline void cryAsyncMemcpy(
    void* dst
    , const void* src
    , size_t size
    , int nFlags
    , volatile int* sync)
{
    GetISystem()->AsyncMemcpy(dst, src, size, nFlags, sync);
}
# else
CRY_ASYNC_MEMCPY_API void cryAsyncMemcpy(
    void* dst
    , const void* src
    , size_t size
    , int nFlags
    , volatile int* sync);
#endif



//////////////////////////////////////////////////////////////////////////
// Additional headers.
//////////////////////////////////////////////////////////////////////////
#include <FrameProfiler.h>

#endif // CRYINCLUDE_CRYCOMMON_ISYSTEM_H

