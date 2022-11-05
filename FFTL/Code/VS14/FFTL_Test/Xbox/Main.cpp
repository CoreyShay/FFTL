//
// Main.cpp
//

#include "pch.h"
#include "Game.h"

#include <appnotify.h>
#include <stdio.h>


#include "../../../Source/AudioEngine/WaveAsset.h"
#include "../../../Source/AudioEngine/AudioEngine.h"
#include "../../../Source/AudioEngine/EventFactory.h"
#include "../../../Source/AudioEngine/Events/EventMetadataDefs.h"
#include "../../../Source/AudioEngine/Entity.h"
#include "../../../Source/AudioEngine/EventOwner.h"
#include "../../../Source/AudioEngine/VoiceManager.h"
#include "../../../Source/AudioEngine/Codec/CodecNode.h"
#include "../../../Source/AudioEngine/DSP/ConvolutionNode.h"
#include "../../../Source/AudioEngine/DSP/IIRFilterNode.h"
#include "../../../Source/AudioEngine/Mixer/WaveRecorderNode.h"
#include "../../../Source/AudioEngine/System/Device.h"

#include "../../../Source/UnitTest/rlutil.h"

namespace FFTL
{

int MultitrackTest()
{
#if defined(FFTL_PLATFORM_ANDROID)
	constexpr char MEDIA_PATH[] = "/sdcard/Media";
#elif defined(FFTL_PLATFORM_XBOX)
	constexpr char MEDIA_PATH[] = "Media";
#elif defined(FFTL_PLATFORM_WINDOWS)
	constexpr char MEDIA_PATH[] = "../../../../Media";
#elif defined(FFTL_PLATFORM_PLAYSTATION)
	constexpr char MEDIA_PATH[] = MEDIA_PATH "/app0/Media";
#endif
	constexpr const char* WAV_FILES[] =
	{
//		"StemTest/die.L64i.wav",
//		"StemTest/Die.wav",
//		"StemTest/die.L.wav",
//		"StemTest/die.R.wav",
		"StemTest/Wither Drum Stem.wav",
		"StemTest/Wither Bass Stem.wav",
		"StemTest/Wither Guitar Stem.wav",
		"StemTest/Wither Keys Stem.wav",
		"StemTest/Wither Solo Stem.wav",
		"StemTest/Wither Vocal Stem.wav",
	};

	static constexpr int WAV_COUNT = sizeof(WAV_FILES) / sizeof(WAV_FILES[0]);

#if 1
	WaveAsset asset[WAV_COUNT];
//	CodecPcmNode pcm[WAV_COUNT];
//	SampleRateConverterNode src[WAV_COUNT];
//	PannerNode panner[WAV_COUNT];
//	Voice* pVoice[WAV_COUNT];

	EngineInitParams params;
	params.uMixBufferLength = 1 << 11;
//	params.uPhysicalVoiceCount = 5;
	const bool bSuccess = g_AudioEngine.Init(&params);
	if (!bSuccess)
	{
		FFTL_LOG_MSG("AudioEngine failed to initialize\n");
		return -1;
	}

	auto& mgr = g_AudioEngine.GetEventFactory().GetMetadataManager();

	mgr.AllocateForFakeData(1, 1024 * 1024, 512);


	for (int i = 0; i < WAV_COUNT; ++i)
	{
		StringFixed<256> szPath;
		StringFormat(szPath.data(), "%s/%s", MEDIA_PATH, WAV_FILES[i]);
		asset[i].SetFileName(szPath);
	}

	std::shared_ptr<MetaEventPhysical> pEvtMetaPhysical[WAV_COUNT];
	std::shared_ptr<MetaEventMultitrack> pEvtMetaMultitrack = std::make_shared<MetaEventMultitrack>();
	for (uint i = 0; i < WAV_COUNT; ++i)
	{
		auto& pEvt = pEvtMetaPhysical[i];
		pEvt = std::make_shared<MetaEventPhysical>();
		pEvt->m_WaveHash = StringHash(asset[i].GetName());
#if FFTL_USE_COMPACT_METADATA
		const auto evtId = mgr.AddObjectToMetadata(1, pEvt.get(), pEvt->m_WaveHash, sizeof(*pEvt), alignof(decltype(*pEvt)));
		pEvtMetaMultitrack->AddChildEventId(evtId);
#else
		pEvtMetaMultitrack->AddChildEvent(pEvt.get());
#endif
		pEvtMetaMultitrack->m_PropagationDelayScale_Cents = 0;
	}

	//	pEvtMetaMultitrack->m_PrepareTimeLimit_mS = 1000;
	pEvtMetaMultitrack->m_fWidth_Meters = 1.f;
	pEvtMetaMultitrack->m_Volume_mB = -600;
#if FFTL_USE_COMPACT_METADATA
	const auto evtId = mgr.AddObjectToMetadata(1, pEvtMetaMultitrack.get(), StringHash("WitherMulti"), sizeof(*pEvtMetaMultitrack), alignof(decltype(*pEvtMetaMultitrack)));
#else
	const auto evtId = pEvtMetaMultitrack.get();
#endif

	WaveRecorderNode wavRecorder;
	wavRecorder.SetStopOnConsecutiveZeroCount(48000);

	StringFixed<256> szWavPath;
	StringFormat(szWavPath.data(), "%s/test.wav", MEDIA_PATH);
	wavRecorder.SetFileName(szWavPath);

	SignalNodeLink* pInput = g_AudioEngine.GetDevice().GetInput();
	g_AudioEngine.GetDevice().Disconnect();
	wavRecorder.SetInput(pInput);
	g_AudioEngine.GetDevice().SetInput(wavRecorder.GetOutput(0));

	g_AudioEngine.StartAudioThread();

	EventOwnerHandle hndl;
	Entity entity;
	entity.Init();
	EventInitParams evtParams;
	const vec3 vPos(0, 1, 0);
	evtParams.pInitPosition = &vPos;
	entity.InitAndPlayEventPersistent(evtId, &hndl, &evtParams);

	const auto& voices = g_AudioEngine.GetVoiceManager().GetActiveVoices();

	g_AudioEngine.UpdateMainThread(2000);
	ThreadSleep(2000);
	//g_AudioEngine.StopAudioThreadAndWait();

	const SignalNode* pNodeToTrack = nullptr;
	//for (Voice* v : voices)
	//{
	//	if (v && v->GetPhysicalVoice())
	//	{
	//		SignalNode* pPanner = v->GetPhysicalVoice();
	//		auto* pNode = new ConvolutionNode();
	//		pPanner->InsertInput(pNode->GetOutput(0));

	//		sprintf(szWavPath, "%s/ImpulseReponses/ExerciseAndNutritionSciences.wav", MEDIA_PATH);
	//		pNode->ImportImpulseResponse(szWavPath);
	//		pNodeToTrack = pNode;

	//		break;
	//	}
	//}

	//for (Voice* v : voices)
	//{
	//	if (v && v->GetPhysicalVoice())
	//	{
	//		SignalNode* pPanner = v->GetPhysicalVoice();
	//		auto* pNode = new IIRFilterNode();
	//		pPanner->InsertInput(pNode->GetOutput(0));

	//		pNode->SetupAdjustableSlope(750.f, 1.f, /*INV_SQRT2, 1.f,*/ IIRFilterType::LowPass);
	//		pNodeToTrack = pNode;

	//		break;
	//	}
	//}

	//g_AudioEngine.StartAudioThread();

	std::atomic<int> nAtomicKey = 0;
	auto testFunc = [&nAtomicKey]()
	{
		while (const int myKey = rlutil::getkey())
		{
			int zero = 0;
			while (!nAtomicKey.compare_exchange_weak(zero, myKey))
			{
				ThreadSleep(10);
			}
		}
	};

	ThreadFunctor funcTest{ testFunc };
	funcTest.Start();


#if 1
	int cursorPos = 0;
#endif

	while (hndl && voices.size() > 0)
	{
		g_AudioEngine.UpdateMainThread(125);

#if defined(FFTL_ENABLE_PROFILING)
		rlutil::cls();
		for (Voice* v : voices)
		{
			SignalNode* pNode = v->GetPhysicalVoice() ? v->GetPhysicalVoice()->GetInput()->GetOwner() : nullptr;
			const tchar* pName = pNode ? pNode->GetName() : FFTL_T("VIRTUAL");

			if (IIRFilterNode* pFilter = dynamic_cast<IIRFilterNode*>(pNode))
			{
				FFTL_LOG_MSG(FFTL_T("\t%-24s vol: %-4.1f exc: %-8.4f inc: %-8.4f ord: %-8.4f\n"), pName, LinearToDecibels(v->GetPlaybackVolume()),
					pFilter->GetTimerExclusive().GetAvgMicroseconds(),
					pFilter->GetTimerInclusive().GetAvgMicroseconds(),
					pFilter->GetAdjustableOrder()
				);
			}
			else
			{
				FFTL_LOG_MSG(FFTL_T("\t%-24s vol: %-4.1f exc: %-8.4f inc: %-8.4f\n"), pName, LinearToDecibels(v->GetPlaybackVolume()),
					pNode ? pNode->GetTimerExclusive().GetAvgMicroseconds() : 0.0,
					pNode ? pNode->GetTimerInclusive().GetAvgMicroseconds() : 0.0);
			}
		}
#endif
#if 1
		rlutil::locate(1, cursorPos + 1);
		rlutil::setChar('>');

		const int nKey = nAtomicKey.exchange(0);
		Voice* pVoice = voices[cursorPos];

		switch (nKey)
		{
		case 14: cursorPos = Clamp(cursorPos - 1, 0, safestatic_cast<int>(voices.size()) - 1); break;
		case 15: cursorPos = Clamp(cursorPos + 1, 0, safestatic_cast<int>(voices.size()) - 1); break;
		case 16:
		{
			SignalNode* pNode = pVoice->GetPhysicalVoice() ? pVoice->GetPhysicalVoice()->GetInput()->GetOwner() : nullptr;
			if (IIRFilterNode* pFilter = dynamic_cast<IIRFilterNode*>(pNode))
			{
				IIRFilterOrder o = pFilter->GetOrder();
				o = Wrap(o - 1, IIRFilterOrder::k1Pole, IIRFilterOrder::kCount);
				pFilter->SetupOrder(o);
			}
			const f32 fVol = pVoice->GetPlaybackVolume();
			const f32 dB = LinearToDecibels(fVol);
			const f32 fOut = DecibelsToLinear(dB - 1);
			pVoice->SetVolume(fOut);
		}
		break;
		case 17:
		{
			SignalNode* pNode = pVoice->GetPhysicalVoice() ? pVoice->GetPhysicalVoice()->GetInput()->GetOwner() : nullptr;
			if (IIRFilterNode* pFilter = dynamic_cast<IIRFilterNode*>(pNode))
			{
				IIRFilterOrder o = pFilter->GetOrder();
				o = Wrap(o + 1, IIRFilterOrder::k1Pole, IIRFilterOrder::kCount);
				pFilter->SetupOrder(o);
			}
			const f32 fVol = pVoice->GetPlaybackVolume();
			const f32 dB = LinearToDecibels(fVol);
			const f32 fOut = DecibelsToLinear(dB + 1);
			pVoice->SetVolume(fOut);
		}
		break;
		case '-':
		{
			SignalNode* pNode = pVoice->GetPhysicalVoice() ? pVoice->GetPhysicalVoice()->GetInput()->GetOwner() : nullptr;
			if (IIRFilterNode* pFilter = dynamic_cast<IIRFilterNode*>(pNode))
			{
				const f32 f = pFilter->GetAdjustableOrder();
				pFilter->SetupAdjustableSlope(750.f, f - 0.125f, IIRFilterType::LowPass);
			}
			const f32 fPitch = pVoice->GetPitchRatio();
			const f32 fSemitones = LinearToSemitones(fPitch);
			const f32 fOut = SemitonesToLinear(fSemitones - 1);
			pVoice->SetPitchRatio(fOut);
		}
		break;
		case '+':
		{
			SignalNode* pNode = pVoice->GetPhysicalVoice() ? pVoice->GetPhysicalVoice()->GetInput()->GetOwner() : nullptr;
			if (IIRFilterNode* pFilter = dynamic_cast<IIRFilterNode*>(pNode))
			{
				const f32 f = pFilter->GetAdjustableOrder();
				pFilter->SetupAdjustableSlope(750.f, f + 0.125f, IIRFilterType::LowPass);
			}
			const f32 fPitch = pVoice->GetPitchRatio();
			const f32 fSemitones = LinearToSemitones(fPitch);
			const f32 fOut = SemitonesToLinear(fSemitones + 1);
			pVoice->SetPitchRatio(fOut);
		}
		break;
		default:
			break;
		}
#endif

		ThreadSleep(250);

		if (!pNodeToTrack)
		{
			for (Voice* v : voices)
			{
				if (v && v->GetPhysicalVoice())
				{
					const SignalNode* pNode = v->GetPhysicalVoice()->GetInput()->GetOwner()->GetInput()->GetOwner();
					if (const CodecNode* pCodecNode = dynamic_cast<const CodecNode*>(pNode); pCodecNode&& pCodecNode->GetCodecFormat() == CodecFormat::PcmFixed24)
						pNodeToTrack = pNode;
					break;
				}
			}
		}
		else
		{
//			rlutil::cls();
//			FFTL_LOG_MSG("Speed: %f\n", pNodeToTrack->GetTimerExclusive().GetAvgMicroseconds());
		}
	}

	if (hndl)
		hndl->StopAndForget();

	funcTest.Terminate();
	g_AudioEngine.Shutdown();
#endif

	return 0;
}

}


using namespace DirectX;

namespace
{
std::unique_ptr<Game> g_game;
HANDLE g_plmSuspendComplete = nullptr;
HANDLE g_plmSignalResume = nullptr;
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Entry point
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (!XMVerifyCPUSupport())
	{
#ifdef _DEBUG
		OutputDebugStringA("ERROR: This hardware does not support the required instruction set.\n");
#ifdef __AVX2__
		OutputDebugStringA("This may indicate a Gaming.Xbox.Scarlett.x64 binary is being run on an Xbox One.\n");
#endif
#endif
		return 1;
	}

	if (FAILED(XGameRuntimeInitialize()))
		return 1;

	FFTL::MultitrackTest();

	// Microsoft Game Core on Xbox supports UTF-8 everywhere
	assert(GetACP() == CP_UTF8);

	g_game = std::make_unique<Game>();

	// Register class and create window
	PAPPSTATE_REGISTRATION hPLM = {};
	{
		// Register class
		WNDCLASSEXA wcex = {};
		wcex.cbSize = sizeof(WNDCLASSEXA);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.hInstance = hInstance;
		wcex.lpszClassName = "Direct3DGame2WindowClass";
		wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		if (!RegisterClassExA(&wcex))
			return 1;

		// Create window
		HWND hwnd = CreateWindowExA(0, "FFTLTestWindowClass", "FFTL.Test", WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, nullptr, nullptr, hInstance,
			nullptr);
		if (!hwnd)
			return 1;

		ShowWindow(hwnd, nCmdShow);

		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_game.get()));

		g_game->Initialize(hwnd);

		g_plmSuspendComplete = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
		g_plmSignalResume = CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE);
		if (!g_plmSuspendComplete || !g_plmSignalResume)
			return 1;

		if (RegisterAppStateChangeNotification([](BOOLEAN quiesced, PVOID context)
		{
			if (quiesced)
			{
				ResetEvent(g_plmSuspendComplete);
				ResetEvent(g_plmSignalResume);

				// To ensure we use the main UI thread to process the notification, we self-post a message
				PostMessage(reinterpret_cast<HWND>(context), WM_USER, 0, 0);

				// To defer suspend, you must wait to exit this callback
				(void)WaitForSingleObject(g_plmSuspendComplete, INFINITE);
			}
			else
			{
				SetEvent(g_plmSignalResume);
			}
		}, hwnd, &hPLM))
			return 1;
	}

	// Main message loop
	MSG msg = {};
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_game->Tick();
		}
	}

	g_game.reset();

	UnregisterAppStateChangeNotification(hPLM);

	CloseHandle(g_plmSuspendComplete);
	CloseHandle(g_plmSignalResume);

	XGameRuntimeUninitialize();

	return static_cast<int>(msg.wParam);
}

// Windows procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto game = reinterpret_cast<Game*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (message)
	{
	case WM_CREATE:
		break;

	case WM_ACTIVATEAPP:
		break;

	case WM_USER:
		if (game)
		{
			game->OnSuspending();

			// Complete deferral
			SetEvent(g_plmSuspendComplete);

			(void)WaitForSingleObject(g_plmSignalResume, INFINITE);

			game->OnResuming();
		}
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

// Exit helper
void ExitGame() noexcept
{
	PostQuitMessage(0);
}
