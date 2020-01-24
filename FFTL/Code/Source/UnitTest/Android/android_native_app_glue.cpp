/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "android_native_app_glue.h"

#include "Core/Platform/Log.h"
#include "Core/Platform/Thread.h"

#include <android/log.h>
#include <unistd.h>


static void free_saved_state(Android_App* pApp) {
    pthread_mutex_lock(&pApp->mutex);
    if (pApp->savedState != NULL) {
        free(pApp->savedState);
        pApp->savedState = NULL;
        pApp->savedStateSize = 0;
    }
    pthread_mutex_unlock(&pApp->mutex);
}

int8_t android_app_read_cmd(Android_App* pApp) {
    int8_t cmd;
    if (read(pApp->msgread, &cmd, sizeof(cmd)) == sizeof(cmd)) {
        switch (cmd) {
            case APP_CMD_SAVE_STATE:
                free_saved_state(pApp);
                break;
        }
        return cmd;
    } else {
        FFTL_LOG_ERR("No data on command pipe!");
    }
    return -1;
}

static void print_cur_config(Android_App* pApp) {
    char lang[2], country[2];
    AConfiguration_getLanguage(pApp->config, lang);
    AConfiguration_getCountry(pApp->config, country);

    FFTL_LOG_VRB("Config: mcc=%d mnc=%d lang=%c%c cnt=%c%c orien=%d touch=%d dens=%d "
            "keys=%d nav=%d keysHid=%d navHid=%d sdk=%d size=%d long=%d "
            "modetype=%d modenight=%d",
            AConfiguration_getMcc(pApp->config),
            AConfiguration_getMnc(pApp->config),
            lang[0], lang[1], country[0], country[1],
            AConfiguration_getOrientation(pApp->config),
            AConfiguration_getTouchscreen(pApp->config),
            AConfiguration_getDensity(pApp->config),
            AConfiguration_getKeyboard(pApp->config),
            AConfiguration_getNavigation(pApp->config),
            AConfiguration_getKeysHidden(pApp->config),
            AConfiguration_getNavHidden(pApp->config),
            AConfiguration_getSdkVersion(pApp->config),
            AConfiguration_getScreenSize(pApp->config),
            AConfiguration_getScreenLong(pApp->config),
            AConfiguration_getUiModeType(pApp->config),
            AConfiguration_getUiModeNight(pApp->config));
}

void android_app_pre_exec_cmd(Android_App* pApp, int8_t cmd) {
    switch (cmd) {
        case APP_CMD_INPUT_CHANGED:
            FFTL_LOG_VRB("APP_CMD_INPUT_CHANGED\n");
            pthread_mutex_lock(&pApp->mutex);
            if (pApp->inputQueue != NULL) {
                AInputQueue_detachLooper(pApp->inputQueue);
            }
            pApp->inputQueue = pApp->pendingInputQueue;
            if (pApp->inputQueue != NULL) {
                FFTL_LOG_VRB("Attaching input queue to looper");
                AInputQueue_attachLooper(pApp->inputQueue,
                        pApp->looper, LOOPER_ID_INPUT, NULL,
                        &pApp->inputPollSource);
            }
            pthread_cond_broadcast(&pApp->cond);
            pthread_mutex_unlock(&pApp->mutex);
            break;

        case APP_CMD_INIT_WINDOW:
            FFTL_LOG_VRB("APP_CMD_INIT_WINDOW\n");
            pthread_mutex_lock(&pApp->mutex);
            pApp->window = pApp->pendingWindow;
            pthread_cond_broadcast(&pApp->cond);
            pthread_mutex_unlock(&pApp->mutex);
            break;

        case APP_CMD_TERM_WINDOW:
            FFTL_LOG_VRB("APP_CMD_TERM_WINDOW\n");
            pthread_cond_broadcast(&pApp->cond);
            break;

        case APP_CMD_RESUME:
        case APP_CMD_START:
        case APP_CMD_PAUSE:
        case APP_CMD_STOP:
            FFTL_LOG_VRB("activityState=%d\n", cmd);
            pthread_mutex_lock(&pApp->mutex);
            pApp->activityState = cmd;
            pthread_cond_broadcast(&pApp->cond);
            pthread_mutex_unlock(&pApp->mutex);
            break;

        case APP_CMD_CONFIG_CHANGED:
            FFTL_LOG_VRB("APP_CMD_CONFIG_CHANGED\n");
            AConfiguration_fromAssetManager(pApp->config,
                    pApp->activity->assetManager);
            print_cur_config(pApp);
            break;

        case APP_CMD_DESTROY:
            FFTL_LOG_VRB("APP_CMD_DESTROY\n");
            pApp->destroyRequested = 1;
            break;
    }
}

void android_app_post_exec_cmd(Android_App* pApp, int8_t cmd) {
    switch (cmd) {
        case APP_CMD_TERM_WINDOW:
            FFTL_LOG_VRB("APP_CMD_TERM_WINDOW\n");
            pthread_mutex_lock(&pApp->mutex);
            pApp->window = NULL;
            pthread_cond_broadcast(&pApp->cond);
            pthread_mutex_unlock(&pApp->mutex);
            break;

        case APP_CMD_SAVE_STATE:
            FFTL_LOG_VRB("APP_CMD_SAVE_STATE\n");
            pthread_mutex_lock(&pApp->mutex);
            pApp->stateSaved = 1;
            pthread_cond_broadcast(&pApp->cond);
            pthread_mutex_unlock(&pApp->mutex);
            break;

        case APP_CMD_RESUME:
            free_saved_state(pApp);
            break;
    }
}

static void android_app_destroy(Android_App* pApp) {
    FFTL_LOG_VRB("android_app_destroy!");
    free_saved_state(pApp);
    pthread_mutex_lock(&pApp->mutex);
    if (pApp->inputQueue != NULL) {
        AInputQueue_detachLooper(pApp->inputQueue);
    }
    AConfiguration_delete(pApp->config);
	pApp->destroyed = 1;
    pthread_cond_broadcast(&pApp->cond);
    pthread_mutex_unlock(&pApp->mutex);
    // Can't touch android_app object after this.
}

static void process_input(Android_App* app, struct android_poll_source* source) {
    AInputEvent* event = NULL;
    while (AInputQueue_getEvent(app->inputQueue, &event) >= 0) {
        FFTL_LOG_VRB("New input event: type=%d\n", AInputEvent_getType(event));
        if (AInputQueue_preDispatchEvent(app->inputQueue, event)) {
            continue;
        }
        int32_t handled = 0;
        if (app->onInputEvent != NULL) handled = app->onInputEvent(app, event);
        AInputQueue_finishEvent(app->inputQueue, event, handled);
    }
}

static void process_cmd(Android_App* app, android_poll_source* source) {
    int8_t cmd = android_app_read_cmd(app);
    android_app_pre_exec_cmd(app, cmd);
    if (app->onAppCmd != NULL) app->onAppCmd(app, cmd);
    android_app_post_exec_cmd(app, cmd);
}

static void* android_app_entry(void* param) {
	Android_App* pApp = (Android_App*)param;

	pApp->config = AConfiguration_new();
    AConfiguration_fromAssetManager(pApp->config, pApp->activity->assetManager);

    print_cur_config(pApp);

    pApp->cmdPollSource.id = LOOPER_ID_MAIN;
    pApp->cmdPollSource.app = pApp;
    pApp->cmdPollSource.process = process_cmd;
    pApp->inputPollSource.id = LOOPER_ID_INPUT;
    pApp->inputPollSource.app = pApp;
    pApp->inputPollSource.process = process_input;

    ALooper* looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    ALooper_addFd(looper, pApp->msgread, LOOPER_ID_MAIN, ALOOPER_EVENT_INPUT, NULL,
            &pApp->cmdPollSource);
	pApp->looper = looper;

    pthread_mutex_lock(&pApp->mutex);
	pApp->running = 1;
    pthread_cond_broadcast(&pApp->cond);
    pthread_mutex_unlock(&pApp->mutex);

    android_main(pApp);

    android_app_destroy(pApp);
    return NULL;
}

// --------------------------------------------------------------------
// Native activity interaction (called from main thread)
// --------------------------------------------------------------------

static Android_App* android_app_create(ANativeActivity* activity,
        void* savedState, size_t savedStateSize) {
    Android_App* pApp = (Android_App*)malloc(sizeof(Android_App));
    memset(pApp, 0, sizeof(Android_App));
	pApp->activity = activity;

    pthread_mutex_init(&pApp->mutex, NULL);
    pthread_cond_init(&pApp->cond, NULL);

    if (savedState != NULL) {
		pApp->savedState = malloc(savedStateSize);
		pApp->savedStateSize = savedStateSize;
        memcpy(pApp->savedState, savedState, savedStateSize);
    }

    int msgpipe[2];
    if (pipe(msgpipe)) {
        FFTL_LOG_ERR("could not create pipe: %s", strerror(errno));
        return NULL;
    }
	pApp->msgread = msgpipe[0];
	pApp->msgwrite = msgpipe[1];

    pthread_attr_t attr; 
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&pApp->thread, &attr, android_app_entry, pApp);

    // Wait for thread to start.
    pthread_mutex_lock(&pApp->mutex);
    while (!pApp->running) {
        pthread_cond_wait(&pApp->cond, &pApp->mutex);
    }
    pthread_mutex_unlock(&pApp->mutex);

    return pApp;
}

static void android_app_write_cmd(Android_App* pApp, int8_t cmd) {
    if (write(pApp->msgwrite, &cmd, sizeof(cmd)) != sizeof(cmd)) {
        FFTL_LOG_ERR("Failure writing android_app cmd: %s\n", strerror(errno));
    }
}

static void android_app_set_input(Android_App* pApp, AInputQueue* inputQueue) {
    pthread_mutex_lock(&pApp->mutex);
	pApp->pendingInputQueue = inputQueue;
    android_app_write_cmd(pApp, APP_CMD_INPUT_CHANGED);
    while (pApp->inputQueue != pApp->pendingInputQueue) {
        pthread_cond_wait(&pApp->cond, &pApp->mutex);
    }
    pthread_mutex_unlock(&pApp->mutex);
}

static void android_app_set_window(Android_App* pApp, ANativeWindow* window) {
    pthread_mutex_lock(&pApp->mutex);
    if (pApp->pendingWindow != NULL) {
        android_app_write_cmd(pApp, APP_CMD_TERM_WINDOW);
    }
	pApp->pendingWindow = window;
    if (window != NULL) {
        android_app_write_cmd(pApp, APP_CMD_INIT_WINDOW);
    }
    while (pApp->window != pApp->pendingWindow) {
        pthread_cond_wait(&pApp->cond, &pApp->mutex);
    }
    pthread_mutex_unlock(&pApp->mutex);
}

static void android_app_set_activity_state(Android_App* pApp, int8_t cmd) {
    pthread_mutex_lock(&pApp->mutex);
    android_app_write_cmd(pApp, cmd);
    while (pApp->activityState != cmd) {
        pthread_cond_wait(&pApp->cond, &pApp->mutex);
    }
    pthread_mutex_unlock(&pApp->mutex);
}

static void android_app_free(Android_App* pApp) {
    pthread_mutex_lock(&pApp->mutex);
    android_app_write_cmd(pApp, APP_CMD_DESTROY);
    while (!pApp->destroyed) {
        pthread_cond_wait(&pApp->cond, &pApp->mutex);
    }
    pthread_mutex_unlock(&pApp->mutex);

    close(pApp->msgread);
    close(pApp->msgwrite);
    pthread_cond_destroy(&pApp->cond);
    pthread_mutex_destroy(&pApp->mutex);
    free(pApp);
}

static void onDestroy(ANativeActivity* activity) {
    FFTL_LOG_VRB("Destroy: %p\n", activity);
    android_app_free((Android_App*)activity->instance);
}

static void onStart(ANativeActivity* activity) {
    FFTL_LOG_VRB("Start: %p\n", activity);
    android_app_set_activity_state((Android_App*)activity->instance, APP_CMD_START);
}

static void onResume(ANativeActivity* activity) {
    FFTL_LOG_VRB("Resume: %p\n", activity);
    android_app_set_activity_state((Android_App*)activity->instance, APP_CMD_RESUME);
}

static void* onSaveInstanceState(ANativeActivity* activity, size_t* outLen) {
    Android_App* pApp = (Android_App*)activity->instance;
    void* savedState = NULL;

    FFTL_LOG_VRB("SaveInstanceState: %p\n", activity);
    pthread_mutex_lock(&pApp->mutex);
    pApp->stateSaved = 0;
    android_app_write_cmd(pApp, APP_CMD_SAVE_STATE);
    while (!pApp->stateSaved) {
        pthread_cond_wait(&pApp->cond, &pApp->mutex);
    }

    if (pApp->savedState != NULL) {
        savedState = pApp->savedState;
        *outLen = pApp->savedStateSize;
        pApp->savedState = NULL;
        pApp->savedStateSize = 0;
    }

    pthread_mutex_unlock(&pApp->mutex);

    return savedState;
}

static void onPause(ANativeActivity* activity) {
    FFTL_LOG_VRB("Pause: %p\n", activity);
    android_app_set_activity_state((Android_App*)activity->instance, APP_CMD_PAUSE);
}

static void onStop(ANativeActivity* activity) {
    FFTL_LOG_VRB("Stop: %p\n", activity);
    android_app_set_activity_state((Android_App*)activity->instance, APP_CMD_STOP);
}

static void onConfigurationChanged(ANativeActivity* activity) {
    Android_App* pApp = (Android_App*)activity->instance;
    FFTL_LOG_VRB("ConfigurationChanged: %p\n", activity);
    android_app_write_cmd(pApp, APP_CMD_CONFIG_CHANGED);
}

static void onLowMemory(ANativeActivity* activity) {
    Android_App* pApp = (Android_App*)activity->instance;
    FFTL_LOG_VRB("LowMemory: %p\n", activity);
    android_app_write_cmd(pApp, APP_CMD_LOW_MEMORY);
}

static void onWindowFocusChanged(ANativeActivity* activity, int focused) {
    FFTL_LOG_VRB("WindowFocusChanged: %p -- %d\n", activity, focused);
    android_app_write_cmd((Android_App*)activity->instance,
            focused ? APP_CMD_GAINED_FOCUS : APP_CMD_LOST_FOCUS);
}

static void onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window) {
    FFTL_LOG_VRB("NativeWindowCreated: %p -- %p\n", activity, window);
    android_app_set_window((Android_App*)activity->instance, window);
}

static void onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window) {
    FFTL_LOG_VRB("NativeWindowDestroyed: %p -- %p\n", activity, window);
    android_app_set_window((Android_App*)activity->instance, NULL);
}

static void onInputQueueCreated(ANativeActivity* activity, AInputQueue* queue) {
    FFTL_LOG_VRB("InputQueueCreated: %p -- %p\n", activity, queue);
    android_app_set_input((Android_App*)activity->instance, queue);
}

static void onInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue) {
    FFTL_LOG_VRB("InputQueueDestroyed: %p -- %p\n", activity, queue);
    android_app_set_input((Android_App*)activity->instance, NULL);
}

void ANativeActivity_onCreate(ANativeActivity* activity,
        void* savedState, size_t savedStateSize) {
    FFTL_LOG_VRB("Creating: %p\n", activity);
    activity->callbacks->onDestroy = onDestroy;
    activity->callbacks->onStart = onStart;
    activity->callbacks->onResume = onResume;
    activity->callbacks->onSaveInstanceState = onSaveInstanceState;
    activity->callbacks->onPause = onPause;
    activity->callbacks->onStop = onStop;
    activity->callbacks->onConfigurationChanged = onConfigurationChanged;
    activity->callbacks->onLowMemory = onLowMemory;
    activity->callbacks->onWindowFocusChanged = onWindowFocusChanged;
    activity->callbacks->onNativeWindowCreated = onNativeWindowCreated;
    activity->callbacks->onNativeWindowDestroyed = onNativeWindowDestroyed;
    activity->callbacks->onInputQueueCreated = onInputQueueCreated;
    activity->callbacks->onInputQueueDestroyed = onInputQueueDestroyed;

    activity->instance = android_app_create(activity, savedState, savedStateSize);
}











//	Gimmick color animation code, which we don't really need
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <android/sensor.h>

/**
* Our saved state data.
*/
struct saved_state {
	float angle;
	int32_t x;
	int32_t y;
};

/**
* Shared state for our app.
*/
struct Engine {
	struct Android_App* app;

	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	ASensorEventQueue* sensorEventQueue;

	int animating;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32_t width;
	int32_t height;
	struct saved_state state;
};

/**
* Initialize an EGL context for the current display.
*/
static int engine_init_display(struct Engine* pEngine) {
	// initialize OpenGL ES and EGL

	/*
	* Here specify the attributes of the desired configuration.
	* Below, we select an EGLConfig with at least 8 bits per color
	* component compatible with on-screen windows
	*/
	const EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
	};
	EGLint w, h, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	/* Here, the application chooses the configuration it desires. In this
	* sample, we have a very simplified selection process, where we pick
	* the first EGLConfig that matches our criteria */
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	* guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	* As soon as we picked a EGLConfig, we can safely reconfigure the
	* ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(pEngine->app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, pEngine->app->window, NULL);
	context = eglCreateContext(display, config, NULL, NULL);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		FFTL_LOG_WRN("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	pEngine->display = display;
	pEngine->context = context;
	pEngine->surface = surface;
	pEngine->width = w;
	pEngine->height = h;
	pEngine->state.angle = 0;

	// Initialize GL state.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_DEPTH_TEST);

	return 0;
}

/**
* Just the current frame in the display.
*/
static void engine_draw_frame(Engine* pEngine) {
	if (pEngine->display == NULL) {
		// No display.
		return;
	}

	// Just fill the screen with a color.
	glClearColor(((float)pEngine->state.x) / pEngine->width, pEngine->state.angle,
		((float)pEngine->state.y) / pEngine->height, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	eglSwapBuffers(pEngine->display, pEngine->surface);
}

/**
* Tear down the EGL context currently associated with the display.
*/
static void engine_term_display(Engine* pEngine) {
	if (pEngine->display != EGL_NO_DISPLAY) {
		eglMakeCurrent(pEngine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (pEngine->context != EGL_NO_CONTEXT) {
			eglDestroyContext(pEngine->display, pEngine->context);
		}
		if (pEngine->surface != EGL_NO_SURFACE) {
			eglDestroySurface(pEngine->display, pEngine->surface);
		}
		eglTerminate(pEngine->display);
	}
	pEngine->animating = 0;
	pEngine->display = EGL_NO_DISPLAY;
	pEngine->context = EGL_NO_CONTEXT;
	pEngine->surface = EGL_NO_SURFACE;
}

/**
* Process the next input event.
*/
static int32_t engine_handle_input(Android_App* app, AInputEvent* event) {
	Engine* pEngine = (Engine*)app->userData;
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		pEngine->state.x = AMotionEvent_getX(event, 0);
		pEngine->state.y = AMotionEvent_getY(event, 0);
		return 1;
	}
	return 0;
}

/**
* Process the next main command.
*/
static void engine_handle_cmd(struct Android_App* app, int32_t cmd) {
	Engine* pEngine = (Engine*)app->userData;
	switch (cmd) {
		case APP_CMD_SAVE_STATE:
			// The system has asked us to save our current state.  Do so.
			pEngine->app->savedState = malloc(sizeof(struct saved_state));
			*((struct saved_state*)pEngine->app->savedState) = pEngine->state;
			pEngine->app->savedStateSize = sizeof(struct saved_state);
			break;
		case APP_CMD_INIT_WINDOW:
			// The window is being shown, get it ready.
			if (pEngine->app->window != NULL) {
				engine_init_display(pEngine);
				engine_draw_frame(pEngine);
			}
			break;
		case APP_CMD_TERM_WINDOW:
			// The window is being hidden or closed, clean it up.
			engine_term_display(pEngine);
			break;
		case APP_CMD_GAINED_FOCUS:
			// When our app gains focus, we start monitoring the accelerometer.
			if (pEngine->accelerometerSensor != NULL) {
				ASensorEventQueue_enableSensor(pEngine->sensorEventQueue,
					pEngine->accelerometerSensor);
				// We'd like to get 60 events per second (in us).
				ASensorEventQueue_setEventRate(pEngine->sensorEventQueue,
					pEngine->accelerometerSensor, (1000L / 60) * 1000);
			}
			break;
		case APP_CMD_LOST_FOCUS:
			// When our app loses focus, we stop monitoring the accelerometer.
			// This is to avoid consuming battery while not being used.
			if (pEngine->accelerometerSensor != NULL) {
				ASensorEventQueue_disableSensor(pEngine->sensorEventQueue,
					pEngine->accelerometerSensor);
			}
			// Also stop animating.
			pEngine->animating = 0;
			engine_draw_frame(pEngine);
			break;
	}
}

/**
* This is the main entry point of a native application that is using
* android_native_app_glue.  It runs in its own thread, with its own
* event loop for receiving input events and doing other things.
*/
extern int main();
void android_main(struct Android_App* state) {
	Engine engine;

	//	My app
	FFTL::CreateAndStartThread(nullptr, [](void*) -> FFTL_THREAD_RETURNTYPE { main(); return FFTL_THREAD_RETURNVALUE; });

	memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	engine.app = state;

	// Prepare to monitor accelerometer
	engine.sensorManager = ASensorManager_getInstance();
	engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
		ASENSOR_TYPE_ACCELEROMETER);
	engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,
		state->looper, LOOPER_ID_USER, NULL, NULL);

	if (state->savedState != NULL) {
		// We are starting with a previous saved state; restore from it.
		engine.state = *(struct saved_state*)state->savedState;
	}

	engine.animating = 1;

	// loop waiting for stuff to do.

	while (1) {
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
			(void**)&source)) >= 0) {

			// Process this event.
			if (source != NULL) {
				source->process(state, source);
			}

			// If a sensor has data, process it now.
			if (ident == LOOPER_ID_USER) {
				if (engine.accelerometerSensor != NULL) {
					ASensorEvent event;
					while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
						&event, 1) > 0) {
						FFTL_LOG_MSG("accelerometer: x=%f y=%f z=%f",
							event.acceleration.x, event.acceleration.y,
							event.acceleration.z);
					}
				}
			}

			// Check if we are exiting.
			if (state->destroyRequested != 0) {
				engine_term_display(&engine);
				return;
			}
		}

		if (engine.animating) {
			// Done with events; draw next animation frame.
			engine.state.angle += .01f;
			if (engine.state.angle > 1) {
				engine.state.angle = 0;
			}

			// Drawing is throttled to the screen update rate, so there
			// is no need to do timing here.
			engine_draw_frame(&engine);
		}
	}
}
