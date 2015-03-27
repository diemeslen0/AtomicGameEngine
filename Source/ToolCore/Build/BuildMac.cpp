// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Atomic/IO/FileSystem.h>

#include "../ToolSystem.h"
#include "../Project/Project.h"
#include "BuildMac.h"
#include "BuildSystem.h"

namespace ToolCore
{

BuildMac::BuildMac(Context * context, Project *project) : BuildBase(context, project)
{

}

BuildMac::~BuildMac()
{

}

void BuildMac::Initialize()
{
    ToolSystem* tools = GetSubsystem<ToolSystem>();
    Project* project = tools->GetProject();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

#ifdef ATOMIC_PLATFORM_WINDOWS
    String bundleResources = fileSystem->GetProgramDir();
#else
    String bundleResources = fileSystem->GetAppBundleResourceFolder();
#endif

    String projectResources = project->GetResourcePath();
    String coreDataFolder = bundleResources + "CoreData/";

    AddResourceDir(coreDataFolder);
    AddResourceDir(projectResources);

    BuildResourceEntries();

}

void BuildMac::Build(const String& buildPath)
{
    buildPath_ = buildPath + "/Mac-Build";

    Initialize();

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    if (fileSystem->DirExists(buildPath_))
        fileSystem->RemoveDir(buildPath_, true);

 #ifdef ATOMIC_PLATFORM_WINDOWS
    String buildSourceDir = fileSystem->GetProgramDir();
 #else
    String buildSourceDir = fileSystem->GetAppBundleResourceFolder();
 #endif

    buildSourceDir += "Deployment/MacOS/AtomicPlayer.app";

    fileSystem->CreateDir(buildPath_);

    buildPath_ += "/AtomicPlayer.app";

    fileSystem->CreateDir(buildPath_);

    fileSystem->CreateDir(buildPath_ + "/Contents");
    fileSystem->CreateDir(buildPath_ + "/Contents/MacOS");
    fileSystem->CreateDir(buildPath_ + "/Contents/Resources");

    String resourcePackagePath = buildPath_ + "/Contents/Resources/AtomicResources.pak";
    GenerateResourcePackage(resourcePackagePath);

    fileSystem->Copy(buildSourceDir + "/Contents/Resources/Atomic.icns", buildPath_ + "/Contents/Resources/Atomic.icns");

    fileSystem->Copy(buildSourceDir + "/Contents/Info.plist", buildPath_ + "/Contents/Info.plist");
    fileSystem->Copy(buildSourceDir + "/Contents/MacOS/AtomicPlayer", buildPath_ + "/Contents/MacOS/AtomicPlayer");

#ifdef ATOMIC_PLATFORM_OSX
    Vector<String> args;
    args.Push("+x");
    args.Push(buildPath_ + "/Contents/MacOS/AtomicPlayer");
    fileSystem->SystemRun("chmod", args);
#endif

    buildPath_ = buildPath + "/Mac-Build";    
    buildSystem->BuildComplete(PLATFORMID_MAC, buildPath_);

}

}
