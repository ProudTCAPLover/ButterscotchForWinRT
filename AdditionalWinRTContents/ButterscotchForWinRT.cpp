//// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
//// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
//// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//// PARTICULAR PURPOSE.
////
//// Copyright (c) Microsoft Corporation. All rights reserved

#include "pch.h"
#include "ButterscotchForWinRT.h"
#include "BasicTimer.h"
#include "winrt\winrt_platform.h"
#include "winrt/winrt_log.h"
#include <string>

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

static bool g_activated = false;

ButterscotchForWinRT::ButterscotchForWinRT() :
    m_windowClosed(false)
{
}

void ButterscotchForWinRT::Initialize(CoreApplicationView^ applicationView)
{
	WinRT_LogRaw("Initalize");
    applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &ButterscotchForWinRT::OnActivated);

    CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>(this, &ButterscotchForWinRT::OnSuspending);

    CoreApplication::Resuming +=
        ref new EventHandler<Platform::Object^>(this, &ButterscotchForWinRT::OnResuming);

    m_renderer = ref new CubeRenderer();
}

void ButterscotchForWinRT::SetWindow(CoreWindow^ window)
{
	WinRT_LogRaw("Beginning of SetWindow initialization");
    window->SizeChanged += 
        ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &ButterscotchForWinRT::OnWindowSizeChanged);
	
    window->Closed += 
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &ButterscotchForWinRT::OnWindowClosed);
	
	window->VisibilityChanged +=
		    ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &ButterscotchForWinRT::OnVisibilityChanged);

	WinRT_SetCoreWindow(window);

    m_renderer->Initialize(CoreWindow::GetForCurrentThread());
	WinRT_LogRaw("SetWindow: Direct3D is ready");
	WinRT_SetDirect3D(m_renderer);
}

void ButterscotchForWinRT::Load(Platform::String^ entryPoint)
{
}

void ButterscotchForWinRT::Run()
{
	WinRT_LogRaw("Beginning of Run() initialization");
	WinRT_FileLog("Run() start");
    std::wstring path = std::wstring(Windows::Storage::ApplicationData::Current->LocalFolder->Path->Data()) + L"\\game\\data.win";
    OutputDebugStringW((L"Butterscotch: data.win expected at " + path + L"\n").c_str());

    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &fad))
    {
		   CoreWindow^ win = CoreWindow::GetForCurrentThread();
   while (!g_activated && !m_windowClosed)
       win->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
   WinRT_FileLog("activated, starting game");
        WinRT_RunGame(path.c_str());
        OutputDebugStringW(L"Butterscotch: game loop ended\n");
        return;
    }
    OutputDebugStringW(L"Butterscotch: data.win not found, have a blue background instead m8\n");

    BasicTimer^ timer = ref new BasicTimer();

    while (!m_windowClosed)
    {
        timer->Update();
        CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
        m_renderer->Update(timer->Total, timer->Delta);
        m_renderer->Render();
        m_renderer->Present(); // This call is synchronized to the display frame rate.
    }
}

void ButterscotchForWinRT::Uninitialize() 
{
}

void ButterscotchForWinRT::OnWindowSizeChanged(CoreWindow^ sender, WindowSizeChangedEventArgs^ args) 
{
    m_renderer->UpdateForWindowSizeChange();
}

void ButterscotchForWinRT::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
    m_windowClosed = true;
	WinRT_RequestExit();
}

void ButterscotchForWinRT::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
    CoreWindow::GetForCurrentThread()->Activate();
	g_activated = true;
}

void ButterscotchForWinRT::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	    WinRT_SetVisible(args->Visible);
}

void ButterscotchForWinRT::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
    // Save application state after requesting a deferral. Holding a deferral
    // indicates that the application is busy performing suspending operations.
    // Be aware that a deferral may not be held indefinitely. After about five
    // seconds, the application will be forced to exit.
     SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();

     // Insert your code here

	 WinRT_FileLog("suspending");
     deferral->Complete();
}
 
void ButterscotchForWinRT::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
	WinRT_FileLog("resuming");
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
    return ref new ButterscotchForWinRT();
}

[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
    WinRT_LogInit(Windows::Storage::ApplicationData::Current->LocalFolder->Path->Data());
    WinRT_InstallCrashHandlers();
    WinRT_LogRaw("main() entered");

    auto direct3DApplicationSource = ref new Direct3DApplicationSource();
    CoreApplication::Run(direct3DApplicationSource);
    return 0;
}
