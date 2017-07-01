#ifndef xportalH
#define xportalH

/******************************************************************

E:\include\general\xportal.h

******************************************************************/

//#include  //include altered by xmake 2007 May 23. 21:53:57
#include "../gl/gl.h"
//#include  //include altered by xmake 2007 May 23. 21:54:00
#include "../gl/glu.h"
#include "../gl/glx.h"
#include <dlfcn.h>
#include <pthread.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


#ifndef NO_PORTAL
    #undef glAccum
    #undef glAlphaFunc
    #undef glAreTexturesResident
    #undef glArrayElement
    #undef glBegin
    #undef glBindTexture
    #undef glBitmap
    #undef glBlendFunc
    #undef glCallList
    #undef glCallLists
    #undef glClear
    #undef glClearAccum
    #undef glClearColor
    #undef glClearDepth
    #undef glClearIndex
    #undef glClearStencil
    #undef glClipPlane
    #undef glColor3b
    #undef glColor3bv
    #undef glColor3d
    #undef glColor3dv
    #undef glColor3f
    #undef glColor3fv
    #undef glColor3i
    #undef glColor3iv
    #undef glColor3s
    #undef glColor3sv
    #undef glColor3ub
    #undef glColor3ubv
    #undef glColor3ui
    #undef glColor3uiv
    #undef glColor3us
    #undef glColor3usv
    #undef glColor4b
    #undef glColor4bv
    #undef glColor4d
    #undef glColor4dv
    #undef glColor4f
    #undef glColor4fv
    #undef glColor4i
    #undef glColor4iv
    #undef glColor4s
    #undef glColor4sv
    #undef glColor4ub
    #undef glColor4ubv
    #undef glColor4ui
    #undef glColor4uiv
    #undef glColor4us
    #undef glColor4usv
    #undef glColorMask
    #undef glColorMaterial
    #undef glColorPointer
    #undef glCopyPixels
    #undef glCopyTexImage1D
    #undef glCopyTexImage2D
    #undef glCopyTexSubImage1D
    #undef glCopyTexSubImage2D
    #undef glCullFace
    #undef glDeleteLists
    #undef glDeleteTextures
    #undef glDepthFunc
    #undef glDepthMask
    #undef glDepthRange
    #undef glDisable
    #undef glDisableClientState
    #undef glDrawArrays
    #undef glDrawBuffer
    #undef glDrawElements
    #undef glDrawPixels
    #undef glEdgeFlag
    #undef glEdgeFlagPointer
    #undef glEdgeFlagv
    #undef glEnable
    #undef glEnableClientState
    #undef glEnd
    #undef glEndList
    #undef glEvalCoord1d
    #undef glEvalCoord1dv
    #undef glEvalCoord1f
    #undef glEvalCoord1fv
    #undef glEvalCoord2d
    #undef glEvalCoord2dv
    #undef glEvalCoord2f
    #undef glEvalCoord2fv
    #undef glEvalMesh1
    #undef glEvalMesh2
    #undef glEvalPoint1
    #undef glEvalPoint2
    #undef glFeedbackBuffer
    #undef glFinish
    #undef glFlush
    #undef glFogf
    #undef glFogfv
    #undef glFogi
    #undef glFogiv
    #undef glFrontFace
    #undef glFrustum
    #undef glGenLists
    #undef glGenTextures
    #undef glGetBooleanv
    #undef glGetClipPlane
    #undef glGetDoublev
    #undef glGetError
    #undef glGetFloatv
    #undef glGetIntegerv
    #undef glGetLightfv
    #undef glGetLightiv
    #undef glGetMapdv
    #undef glGetMapfv
    #undef glGetMapiv
    #undef glGetMaterialfv
    #undef glGetMaterialiv
    #undef glGetPixelMapfv
    #undef glGetPixelMapuiv
    #undef glGetPixelMapusv
    #undef glGetPointerv
    #undef glGetPolygonStipple
    #undef glGetString
    #undef glGetTexEnvfv
    #undef glGetTexEnviv
    #undef glGetTexGendv
    #undef glGetTexGenfv
    #undef glGetTexGeniv
    #undef glGetTexImage
    #undef glGetTexLevelParameterfv
    #undef glGetTexLevelParameteriv
    #undef glGetTexParameterfv
    #undef glGetTexParameteriv
    #undef glHint
    #undef glIndexMask
    #undef glIndexPointer
    #undef glIndexd
    #undef glIndexdv
    #undef glIndexf
    #undef glIndexfv
    #undef glIndexi
    #undef glIndexiv
    #undef glIndexs
    #undef glIndexsv
    #undef glIndexub
    #undef glIndexubv
    #undef glInitNames
    #undef glInterleavedArrays
    #undef glIsEnabled
    #undef glIsList
    #undef glIsTexture
    #undef glLightModelf
    #undef glLightModelfv
    #undef glLightModeli
    #undef glLightModeliv
    #undef glLightf
    #undef glLightfv
    #undef glLighti
    #undef glLightiv
    #undef glLineStipple
    #undef glLineWidth
    #undef glListBase
    #undef glLoadIdentity
    #undef glLoadMatrixd
    #undef glLoadMatrixf
    #undef glLoadName
    #undef glLogicOp
    #undef glMap1d
    #undef glMap1f
    #undef glMap2d
    #undef glMap2f
    #undef glMapGrid1d
    #undef glMapGrid1f
    #undef glMapGrid2d
    #undef glMapGrid2f
    #undef glMaterialf
    #undef glMaterialfv
    #undef glMateriali
    #undef glMaterialiv
    #undef glMatrixMode
    #undef glMultMatrixd
    #undef glMultMatrixf
    #undef glNewList
    #undef glNormal3b
    #undef glNormal3bv
    #undef glNormal3d
    #undef glNormal3dv
    #undef glNormal3f
    #undef glNormal3fv
    #undef glNormal3i
    #undef glNormal3iv
    #undef glNormal3s
    #undef glNormal3sv
    #undef glNormalPointer
    #undef glOrtho
    #undef glPassThrough
    #undef glPixelMapfv
    #undef glPixelMapuiv
    #undef glPixelMapusv
    #undef glPixelStoref
    #undef glPixelStorei
    #undef glPixelTransferf
    #undef glPixelTransferi
    #undef glPixelZoom
    #undef glPointSize
    #undef glPolygonMode
    #undef glPolygonOffset
    #undef glPolygonStipple
    #undef glPopAttrib
    #undef glPopClientAttrib
    #undef glPopMatrix
    #undef glPopName
    #undef glPrioritizeTextures
    #undef glPushAttrib
    #undef glPushClientAttrib
    #undef glPushMatrix
    #undef glPushName
    #undef glRasterPos2d
    #undef glRasterPos2dv
    #undef glRasterPos2f
    #undef glRasterPos2fv
    #undef glRasterPos2i
    #undef glRasterPos2iv
    #undef glRasterPos2s
    #undef glRasterPos2sv
    #undef glRasterPos3d
    #undef glRasterPos3dv
    #undef glRasterPos3f
    #undef glRasterPos3fv
    #undef glRasterPos3i
    #undef glRasterPos3iv
    #undef glRasterPos3s
    #undef glRasterPos3sv
    #undef glRasterPos4d
    #undef glRasterPos4dv
    #undef glRasterPos4f
    #undef glRasterPos4fv
    #undef glRasterPos4i
    #undef glRasterPos4iv
    #undef glRasterPos4s
    #undef glRasterPos4sv
    #undef glReadBuffer
    #undef glReadPixels
    #undef glRectd
    #undef glRectdv
    #undef glRectf
    #undef glRectfv
    #undef glRecti
    #undef glRectiv
    #undef glRects
    #undef glRectsv
    #undef glRenderMode
    #undef glRotated
    #undef glRotatef
    #undef glScaled
    #undef glScalef
    #undef glScissor
    #undef glSelectBuffer
    #undef glShadeModel
    #undef glStencilFunc
    #undef glStencilMask
    #undef glStencilOp
    #undef glTexCoord1d
    #undef glTexCoord1dv
    #undef glTexCoord1f
    #undef glTexCoord1fv
    #undef glTexCoord1i
    #undef glTexCoord1iv
    #undef glTexCoord1s
    #undef glTexCoord1sv
    #undef glTexCoord2d
    #undef glTexCoord2dv
    #undef glTexCoord2f
    #undef glTexCoord2fv
    #undef glTexCoord2i
    #undef glTexCoord2iv
    #undef glTexCoord2s
    #undef glTexCoord2sv
    #undef glTexCoord3d
    #undef glTexCoord3dv
    #undef glTexCoord3f
    #undef glTexCoord3fv
    #undef glTexCoord3i
    #undef glTexCoord3iv
    #undef glTexCoord3s
    #undef glTexCoord3sv
    #undef glTexCoord4d
    #undef glTexCoord4dv
    #undef glTexCoord4f
    #undef glTexCoord4fv
    #undef glTexCoord4i
    #undef glTexCoord4iv
    #undef glTexCoord4s
    #undef glTexCoord4sv
    #undef glTexCoordPointer
    #undef glTexEnvf
    #undef glTexEnvfv
    #undef glTexEnvi
    #undef glTexEnviv
    #undef glTexGend
    #undef glTexGendv
    #undef glTexGenf
    #undef glTexGenfv
    #undef glTexGeni
    #undef glTexGeniv
    #undef glTexImage1D
    #undef glTexImage2D
    #undef glTexParameterf
    #undef glTexParameterfv
    #undef glTexParameteri
    #undef glTexParameteriv
    #undef glTexSubImage1D
    #undef glTexSubImage2D
    #undef glTranslated
    #undef glTranslatef
    #undef glVertex2d
    #undef glVertex2dv
    #undef glVertex2f
    #undef glVertex2fv
    #undef glVertex2i
    #undef glVertex2iv
    #undef glVertex2s
    #undef glVertex2sv
    #undef glVertex3d
    #undef glVertex3dv
    #undef glVertex3f
    #undef glVertex3fv
    #undef glVertex3i
    #undef glVertex3iv
    #undef glVertex3s
    #undef glVertex3sv
    #undef glVertex4d
    #undef glVertex4dv
    #undef glVertex4f
    #undef glVertex4fv
    #undef glVertex4i
    #undef glVertex4iv
    #undef glVertex4s
    #undef glVertex4sv
    #undef glVertexPointer
    #undef glViewport
    #undef gluBeginCurve
    #undef gluBeginPolygon
    #undef gluBeginSurface
    #undef gluBeginTrim
    #undef gluBuild1DMipmaps
    #undef gluBuild2DMipmaps
    #undef gluCylinder
    #undef gluDeleteNurbsRenderer
    #undef gluDeleteQuadric
    #undef gluDeleteTess
    #undef gluDisk
    #undef gluEndCurve
    #undef gluEndPolygon
    #undef gluEndSurface
    #undef gluEndTrim
    #undef gluErrorString
    #undef gluGetNurbsProperty
    #undef gluGetString
    #undef gluGetTessProperty
    #undef gluLoadSamplingMatrices
    #undef gluLookAt
    #undef gluNewNurbsRenderer
    #undef gluNewQuadric
    #undef gluNewTess
    #undef gluNextContour
    #undef gluNurbsCallback
    #undef gluNurbsCurve
    #undef gluNurbsProperty
    #undef gluNurbsSurface
    #undef gluOrtho2D
    #undef gluPartialDisk
    #undef gluPerspective
    #undef gluPickMatrix
    #undef gluProject
    #undef gluPwlCurve
    #undef gluQuadricCallback
    #undef gluQuadricDrawStyle
    #undef gluQuadricNormals
    #undef gluQuadricOrientation
    #undef gluQuadricTexture
    #undef gluScaleImage
    #undef gluSphere
    #undef gluTessBeginContour
    #undef gluTessBeginPolygon
    #undef gluTessCallback
    #undef gluTessEndContour
    #undef gluTessEndPolygon
    #undef gluTessNormal
    #undef gluTessProperty
    #undef gluTessVertex
    #undef gluUnProject
    #undef gluUnProject4
    #undef glXChooseVisual
    #undef glXCopyContext
    #undef glXCreateContext
    #undef glXCreateGLXPixmap
    #undef glXDestroyContext
    #undef glXDestroyGLXPixmap
    #undef glXGetConfig
    #undef glXGetCurrentContext
    #undef glXGetCurrentDrawable
    #undef glXIsDirect
    #undef glXMakeCurrent
    #undef glXQueryExtension
    #undef glXQueryVersion
    #undef glXSwapBuffers
    #undef glXUseXFont
    #undef glXWaitGL
    #undef glXWaitX
    #undef glXGetClientString
    #undef glXQueryServerString
    #undef glXQueryExtensionsString
    #undef glXGetCurrentDisplay
    #undef glXChooseFBConfig
    #undef glXCreateNewContext
    #undef glXCreatePbuffer
    #undef glXCreatePixmap
    #undef glXCreateWindow
    #undef glXDestroyPbuffer
    #undef glXDestroyPixmap
    #undef glXDestroyWindow
    #undef glXGetCurrentReadDrawable
    #undef glXGetFBConfigAttrib
    #undef glXGetFBConfigs
    #undef glXGetSelectedEvent
    #undef glXGetVisualFromFBConfig
    #undef glXMakeContextCurrent
    #undef glXQueryContext
    #undef glXQueryDrawable
    #undef glXSelectEvent
    #undef glXFreeContextEXT
    #undef glXGetContextIDEXT
    #undef glXImportContextEXT
    #undef glXQueryContextInfoEXT
    #undef glXAllocateMemoryNV
    #undef glXFreeMemoryNV
    #undef glXGetVideoSyncSGI
    #undef glXWaitVideoSyncSGI
    #undef glXGetRefreshRateSGI
    #undef glXSwapIntervalSGI
    #undef glXJoinSwapGroupNV
    #undef glXBindSwapBarrierNV
    #undef glXQuerySwapGroupNV
    #undef glXQueryMaxSwapGroupsNV
    #undef glXQueryFrameCountNV
    #undef glXResetFrameCountNV
    #undef glXGetVideoDeviceNV
    #undef glXReleaseVideoDeviceNV
    #undef glXBindVideoImageNV
    #undef glXReleaseVideoImageNV
    #undef glXSendPbufferToVideoNV
    #undef glXGetVideoInfoNV
    #undef glXGetFBConfigAttribSGIX
    #undef glXChooseFBConfigSGIX
    #undef glXCreateGLXPixmapWithConfigSGIX
    #undef glXCreateContextWithConfigSGIX
    #undef glXGetVisualFromFBConfigSGIX
    #undef glXGetFBConfigFromVisualSGIX
    #undef glXCreateGLXPbufferSGIX
    #undef glXDestroyGLXPbufferSGIX
    #undef glXQueryGLXPbufferSGIX
    #undef glXSelectEventSGIX
    #undef glXGetSelectedEventSGIX
    #undef dlopen
    #undef dlclose
    #undef dlsym
    #undef dlmopen
    #undef dlvsym
    #undef dlerror
    #undef dladdr
    #undef dladdr1
    #undef dlinfo
    #undef pthread_sigmask
    #undef pthread_kill
    #undef pthread_self
    #undef pthread_equal
    #undef pthread_exit
    #undef pthread_join
    #undef pthread_detach
    #undef pthread_attr_init
    #undef pthread_attr_destroy
    #undef pthread_attr_setdetachstate
    #undef pthread_attr_getdetachstate
    #undef pthread_attr_setschedparam
    #undef pthread_attr_getschedparam
    #undef pthread_attr_setschedpolicy
    #undef pthread_attr_getschedpolicy
    #undef pthread_attr_setinheritsched
    #undef pthread_attr_getinheritsched
    #undef pthread_attr_setscope
    #undef pthread_attr_getscope
    #undef pthread_attr_setguardsize
    #undef pthread_attr_getguardsize
    #undef pthread_attr_setstackaddr
    #undef pthread_attr_getstackaddr
    #undef pthread_attr_setstack
    #undef pthread_attr_getstack
    #undef pthread_attr_setstacksize
    #undef pthread_attr_getstacksize
    #undef pthread_getattr_np
    #undef pthread_setschedparam
    #undef pthread_getschedparam
    #undef pthread_getconcurrency
    #undef pthread_setconcurrency
    #undef pthread_yield
    #undef pthread_mutex_init
    #undef pthread_mutex_destroy
    #undef pthread_mutex_trylock
    #undef pthread_mutex_lock
    #undef pthread_mutex_timedlock
    #undef pthread_mutex_unlock
    #undef pthread_mutexattr_init
    #undef pthread_mutexattr_destroy
    #undef pthread_mutexattr_getpshared
    #undef pthread_mutexattr_setpshared
    #undef pthread_mutexattr_settype
    #undef pthread_mutexattr_gettype
    #undef pthread_cond_init
    #undef pthread_cond_destroy
    #undef pthread_cond_signal
    #undef pthread_cond_broadcast
    #undef pthread_cond_wait
    #undef pthread_cond_timedwait
    #undef pthread_condattr_init
    #undef pthread_condattr_destroy
    #undef pthread_condattr_getpshared
    #undef pthread_condattr_setpshared
    #undef pthread_rwlock_init
    #undef pthread_rwlock_destroy
    #undef pthread_rwlock_rdlock
    #undef pthread_rwlock_tryrdlock
    #undef pthread_rwlock_timedrdlock
    #undef pthread_rwlock_wrlock
    #undef pthread_rwlock_trywrlock
    #undef pthread_rwlock_timedwrlock
    #undef pthread_rwlock_unlock
    #undef pthread_rwlockattr_init
    #undef pthread_rwlockattr_destroy
    #undef pthread_rwlockattr_getpshared
    #undef pthread_rwlockattr_setpshared
    #undef pthread_rwlockattr_getkind_np
    #undef pthread_rwlockattr_setkind_np
    #undef pthread_spin_init
    #undef pthread_spin_destroy
    #undef pthread_spin_lock
    #undef pthread_spin_trylock
    #undef pthread_spin_unlock
    #undef pthread_barrier_init
    #undef pthread_barrier_destroy
    #undef pthread_barrierattr_init
    #undef pthread_barrierattr_destroy
    #undef pthread_barrierattr_setpshared
    #undef pthread_barrier_wait
    #undef pthread_key_delete
    #undef pthread_setspecific
    #undef pthread_getspecific
    #undef pthread_setcancelstate
    #undef pthread_setcanceltype
    #undef pthread_cancel
    #undef pthread_testcancel
    #undef _pthread_cleanup_pop
    #undef _pthread_cleanup_pop_restore
    #undef pthread_getcpuclockid
    #undef pthread_kill_other_threads_np
    #undef XRRQueryExtension
    #undef XRRQueryVersion
    #undef XRRGetScreenInfo
    #undef XRRFreeScreenConfigInfo
    #undef XRRSetScreenConfig
    #undef XRRSetScreenConfigAndRate
    #undef XRRConfigRotations
    #undef XRRConfigTimes
    #undef XRRConfigSizes
    #undef XRRConfigRates
    #undef XRRConfigCurrentConfiguration
    #undef XRRConfigCurrentRate
    #undef XRRRootToScreen
    #undef XRRSelectInput
    #undef XRRRotations
    #undef XRRSizes
    #undef XRRRates
    #undef XRRTimes
    #undef XRRUpdateConfiguration
    #undef XLoadQueryFont
    #undef XQueryFont
    #undef XGetMotionEvents
    #undef XDeleteModifiermapEntry
    #undef XGetModifierMapping
    #undef XInsertModifiermapEntry
    #undef XNewModifiermap
    #undef XCreateImage
    #undef XInitImage
    #undef XGetImage
    #undef XGetSubImage
    #undef XOpenDisplay
    #undef XrmInitialize
    #undef XFetchBytes
    #undef XFetchBuffer
    #undef XGetAtomName
    #undef XGetAtomNames
    #undef XGetDefault
    #undef XDisplayName
    #undef XKeysymToString
    #undef XInternAtom
    #undef XInternAtoms
    #undef XCopyColormapAndFree
    #undef XCreateColormap
    #undef XCreatePixmapCursor
    #undef XCreateGlyphCursor
    #undef XCreateFontCursor
    #undef XLoadFont
    #undef XCreateGC
    #undef XGContextFromGC
    #undef XFlushGC
    #undef XCreatePixmap
    #undef XCreateBitmapFromData
    #undef XCreatePixmapFromBitmapData
    #undef XCreateSimpleWindow
    #undef XGetSelectionOwner
    #undef XCreateWindow
    #undef XListInstalledColormaps
    #undef XListFonts
    #undef XListFontsWithInfo
    #undef XGetFontPath
    #undef XListExtensions
    #undef XListProperties
    #undef XListHosts
    #undef XKeycodeToKeysym
    #undef XLookupKeysym
    #undef XGetKeyboardMapping
    #undef XStringToKeysym
    #undef XMaxRequestSize
    #undef XExtendedMaxRequestSize
    #undef XResourceManagerString
    #undef XScreenResourceString
    #undef XDisplayMotionBufferSize
    #undef XVisualIDFromVisual
    #undef XInitThreads
    #undef XLockDisplay
    #undef XUnlockDisplay
    #undef XInitExtension
    #undef XAddExtension
    #undef XFindOnExtensionList
    #undef XEHeadOfExtensionList
    #undef XRootWindow
    #undef XDefaultRootWindow
    #undef XRootWindowOfScreen
    #undef XDefaultVisual
    #undef XDefaultVisualOfScreen
    #undef XDefaultGC
    #undef XDefaultGCOfScreen
    #undef XBlackPixel
    #undef XWhitePixel
    #undef XAllPlanes
    #undef XBlackPixelOfScreen
    #undef XWhitePixelOfScreen
    #undef XNextRequest
    #undef XLastKnownRequestProcessed
    #undef XServerVendor
    #undef XDisplayString
    #undef XDefaultColormap
    #undef XDefaultColormapOfScreen
    #undef XDisplayOfScreen
    #undef XScreenOfDisplay
    #undef XDefaultScreenOfDisplay
    #undef XEventMaskOfScreen
    #undef XScreenNumberOfScreen
    #undef XSetErrorHandler
    #undef XSetIOErrorHandler
    #undef XListPixmapFormats
    #undef XListDepths
    #undef XReconfigureWMWindow
    #undef XGetWMProtocols
    #undef XSetWMProtocols
    #undef XIconifyWindow
    #undef XWithdrawWindow
    #undef XGetCommand
    #undef XGetWMColormapWindows
    #undef XSetWMColormapWindows
    #undef XFreeStringList
    #undef XSetTransientForHint
    #undef XActivateScreenSaver
    #undef XAddHost
    #undef XAddHosts
    #undef XAddToExtensionList
    #undef XAddToSaveSet
    #undef XAllocColor
    #undef XAllocColorCells
    #undef XAllocColorPlanes
    #undef XAllocNamedColor
    #undef XAllowEvents
    #undef XAutoRepeatOff
    #undef XAutoRepeatOn
    #undef XBell
    #undef XBitmapBitOrder
    #undef XBitmapPad
    #undef XBitmapUnit
    #undef XCellsOfScreen
    #undef XChangeActivePointerGrab
    #undef XChangeGC
    #undef XChangeKeyboardControl
    #undef XChangeKeyboardMapping
    #undef XChangePointerControl
    #undef XChangeProperty
    #undef XChangeSaveSet
    #undef XChangeWindowAttributes
    #undef XCheckMaskEvent
    #undef XCheckTypedEvent
    #undef XCheckTypedWindowEvent
    #undef XCheckWindowEvent
    #undef XCirculateSubwindows
    #undef XCirculateSubwindowsDown
    #undef XCirculateSubwindowsUp
    #undef XClearArea
    #undef XClearWindow
    #undef XCloseDisplay
    #undef XConfigureWindow
    #undef XConnectionNumber
    #undef XConvertSelection
    #undef XCopyArea
    #undef XCopyGC
    #undef XCopyPlane
    #undef XDefaultDepth
    #undef XDefaultDepthOfScreen
    #undef XDefaultScreen
    #undef XDefineCursor
    #undef XDeleteProperty
    #undef XDestroyWindow
    #undef XDestroySubwindows
    #undef XDoesBackingStore
    #undef XDoesSaveUnders
    #undef XDisableAccessControl
    #undef XDisplayCells
    #undef XDisplayHeight
    #undef XDisplayHeightMM
    #undef XDisplayKeycodes
    #undef XDisplayPlanes
    #undef XDisplayWidth
    #undef XDisplayWidthMM
    #undef XDrawArc
    #undef XDrawArcs
    #undef XDrawImageString
    #undef XDrawImageString16
    #undef XDrawLine
    #undef XDrawLines
    #undef XDrawPoint
    #undef XDrawPoints
    #undef XDrawRectangle
    #undef XDrawRectangles
    #undef XDrawSegments
    #undef XDrawString
    #undef XDrawString16
    #undef XDrawText
    #undef XDrawText16
    #undef XEnableAccessControl
    #undef XEventsQueued
    #undef XFetchName
    #undef XFillArc
    #undef XFillArcs
    #undef XFillPolygon
    #undef XFillRectangle
    #undef XFillRectangles
    #undef XFlush
    #undef XForceScreenSaver
    #undef XFree
    #undef XFreeColormap
    #undef XFreeColors
    #undef XFreeCursor
    #undef XFreeExtensionList
    #undef XFreeFont
    #undef XFreeFontInfo
    #undef XFreeFontNames
    #undef XFreeFontPath
    #undef XFreeGC
    #undef XFreeModifiermap
    #undef XFreePixmap
    #undef XGeometry
    #undef XGetErrorDatabaseText
    #undef XGetErrorText
    #undef XGetFontProperty
    #undef XGetGCValues
    #undef XGetGeometry
    #undef XGetIconName
    #undef XGetInputFocus
    #undef XGetKeyboardControl
    #undef XGetPointerControl
    #undef XGetPointerMapping
    #undef XGetScreenSaver
    #undef XGetTransientForHint
    #undef XGetWindowProperty
    #undef XGetWindowAttributes
    #undef XGrabButton
    #undef XGrabKey
    #undef XGrabKeyboard
    #undef XGrabPointer
    #undef XGrabServer
    #undef XHeightMMOfScreen
    #undef XHeightOfScreen
    #undef XImageByteOrder
    #undef XInstallColormap
    #undef XKeysymToKeycode
    #undef XKillClient
    #undef XLookupColor
    #undef XLowerWindow
    #undef XMapRaised
    #undef XMapSubwindows
    #undef XMapWindow
    #undef XMaskEvent
    #undef XMaxCmapsOfScreen
    #undef XMinCmapsOfScreen
    #undef XMoveResizeWindow
    #undef XMoveWindow
    #undef XNextEvent
    #undef XNoOp
    #undef XParseColor
    #undef XParseGeometry
    #undef XPeekEvent
    #undef XPending
    #undef XPlanesOfScreen
    #undef XProtocolRevision
    #undef XProtocolVersion
    #undef XPutBackEvent
    #undef XPutImage
    #undef XQLength
    #undef XQueryBestCursor
    #undef XQueryBestSize
    #undef XQueryBestStipple
    #undef XQueryBestTile
    #undef XQueryColor
    #undef XQueryColors
    #undef XQueryExtension
    #undef XQueryKeymap
    #undef XQueryPointer
    #undef XQueryTextExtents
    #undef XQueryTextExtents16
    #undef XQueryTree
    #undef XRaiseWindow
    #undef XReadBitmapFile
    #undef XReadBitmapFileData
    #undef XRebindKeysym
    #undef XRecolorCursor
    #undef XRefreshKeyboardMapping
    #undef XRemoveFromSaveSet
    #undef XRemoveHost
    #undef XRemoveHosts
    #undef XReparentWindow
    #undef XResetScreenSaver
    #undef XResizeWindow
    #undef XRestackWindows
    #undef XRotateBuffers
    #undef XRotateWindowProperties
    #undef XScreenCount
    #undef XSelectInput
    #undef XSendEvent
    #undef XSetAccessControl
    #undef XSetArcMode
    #undef XSetBackground
    #undef XSetClipMask
    #undef XSetClipOrigin
    #undef XSetClipRectangles
    #undef XSetCloseDownMode
    #undef XSetCommand
    #undef XSetDashes
    #undef XSetFillRule
    #undef XSetFillStyle
    #undef XSetFont
    #undef XSetFontPath
    #undef XSetForeground
    #undef XSetFunction
    #undef XSetGraphicsExposures
    #undef XSetIconName
    #undef XSetInputFocus
    #undef XSetLineAttributes
    #undef XSetModifierMapping
    #undef XSetPlaneMask
    #undef XSetPointerMapping
    #undef XSetScreenSaver
    #undef XSetSelectionOwner
    #undef XSetState
    #undef XSetStipple
    #undef XSetSubwindowMode
    #undef XSetTSOrigin
    #undef XSetTile
    #undef XSetWindowBackground
    #undef XSetWindowBackgroundPixmap
    #undef XSetWindowBorder
    #undef XSetWindowBorderPixmap
    #undef XSetWindowBorderWidth
    #undef XSetWindowColormap
    #undef XStoreBuffer
    #undef XStoreBytes
    #undef XStoreColor
    #undef XStoreColors
    #undef XStoreName
    #undef XStoreNamedColor
    #undef XSync
    #undef XTextExtents
    #undef XTextExtents16
    #undef XTextWidth
    #undef XTextWidth16
    #undef XTranslateCoordinates
    #undef XUndefineCursor
    #undef XUngrabButton
    #undef XUngrabKey
    #undef XUngrabKeyboard
    #undef XUngrabPointer
    #undef XUngrabServer
    #undef XUninstallColormap
    #undef XUnloadFont
    #undef XUnmapSubwindows
    #undef XUnmapWindow
    #undef XVendorRelease
    #undef XWarpPointer
    #undef XWidthMMOfScreen
    #undef XWidthOfScreen
    #undef XWindowEvent
    #undef XWriteBitmapFile
    #undef XSupportsLocale
    #undef XSetLocaleModifiers
    #undef XOpenOM
    #undef XCloseOM
    #undef XDisplayOfOM
    #undef XLocaleOfOM
    #undef XDestroyOC
    #undef XOMOfOC
    #undef XCreateFontSet
    #undef XFreeFontSet
    #undef XFontsOfFontSet
    #undef XBaseFontNameListOfFontSet
    #undef XLocaleOfFontSet
    #undef XContextDependentDrawing
    #undef XDirectionalDependentDrawing
    #undef XContextualDrawing
    #undef XExtentsOfFontSet
    #undef XmbTextEscapement
    #undef XwcTextEscapement
    #undef Xutf8TextEscapement
    #undef XmbTextExtents
    #undef XwcTextExtents
    #undef Xutf8TextExtents
    #undef XmbTextPerCharExtents
    #undef XwcTextPerCharExtents
    #undef Xutf8TextPerCharExtents
    #undef XmbDrawText
    #undef XwcDrawText
    #undef Xutf8DrawText
    #undef XmbDrawString
    #undef XwcDrawString
    #undef Xutf8DrawString
    #undef XmbDrawImageString
    #undef XwcDrawImageString
    #undef Xutf8DrawImageString
    #undef XOpenIM
    #undef XCloseIM
    #undef XDisplayOfIM
    #undef XLocaleOfIM
    #undef XDestroyIC
    #undef XSetICFocus
    #undef XUnsetICFocus
    #undef XwcResetIC
    #undef XmbResetIC
    #undef Xutf8ResetIC
    #undef XIMOfIC
    #undef XFilterEvent
    #undef XmbLookupString
    #undef XwcLookupString
    #undef Xutf8LookupString
    #undef XRegisterIMInstantiateCallback
    #undef XUnregisterIMInstantiateCallback
    #undef XInternalConnectionNumbers
    #undef XProcessInternalConnection
    #undef XAddConnectionWatch
    #undef XRemoveConnectionWatch
    #undef XSetAuthorization
    #undef _Xwctomb
    #undef XDestroyImage
    #undef XGetPixel
    #undef XPutPixel
    #undef XSubImage
    #undef XAddPixel
    #undef XAllocClassHint
    #undef XAllocIconSize
    #undef XAllocSizeHints
    #undef XAllocStandardColormap
    #undef XAllocWMHints
    #undef XClipBox
    #undef XCreateRegion
    #undef XDefaultString
    #undef XDeleteContext
    #undef XDestroyRegion
    #undef XEmptyRegion
    #undef XEqualRegion
    #undef XFindContext
    #undef XGetClassHint
    #undef XGetIconSizes
    #undef XGetNormalHints
    #undef XGetRGBColormaps
    #undef XGetSizeHints
    #undef XGetStandardColormap
    #undef XGetTextProperty
    #undef XGetVisualInfo
    #undef XGetWMClientMachine
    #undef XGetWMHints
    #undef XGetWMIconName
    #undef XGetWMName
    #undef XGetWMNormalHints
    #undef XGetWMSizeHints
    #undef XGetZoomHints
    #undef XIntersectRegion
    #undef XConvertCase
    #undef XLookupString
    #undef XMatchVisualInfo
    #undef XOffsetRegion
    #undef XPointInRegion
    #undef XPolygonRegion
    #undef XRectInRegion
    #undef XSaveContext
    #undef XSetClassHint
    #undef XSetIconSizes
    #undef XSetNormalHints
    #undef XSetRGBColormaps
    #undef XSetSizeHints
    #undef XSetStandardProperties
    #undef XSetTextProperty
    #undef XSetWMClientMachine
    #undef XSetWMHints
    #undef XSetWMIconName
    #undef XSetWMName
    #undef XSetWMNormalHints
    #undef XSetWMProperties
    #undef XmbSetWMProperties
    #undef Xutf8SetWMProperties
    #undef XSetWMSizeHints
    #undef XSetRegion
    #undef XSetStandardColormap
    #undef XSetZoomHints
    #undef XShrinkRegion
    #undef XStringListToTextProperty
    #undef XSubtractRegion
    #undef XmbTextListToTextProperty
    #undef XwcTextListToTextProperty
    #undef Xutf8TextListToTextProperty
    #undef XwcFreeStringList
    #undef XTextPropertyToStringList
    #undef XmbTextPropertyToTextList
    #undef XwcTextPropertyToTextList
    #undef Xutf8TextPropertyToTextList
    #undef XUnionRectWithRegion
    #undef XUnionRegion
    #undef XWMGeometry
    #undef XXorRegion
    #undef glXGetProcAddressARB
#endif

namespace Portal
{
    void glAccum (GLenum op, GLfloat value);
    void glAlphaFunc (GLenum func, GLclampf ref);
    GLboolean glAreTexturesResident (GLsizei n, const GLuint *textures, GLboolean *residences);
    void glArrayElement (GLint i);
    void glBegin (GLenum mode);
    void glBindTexture (GLenum target, GLuint texture);
    void glBitmap (GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap);
    void glBlendFunc (GLenum sfactor, GLenum dfactor);
    void glCallList (GLuint list);
    void glCallLists (GLsizei n, GLenum type, const GLvoid *lists);
    void glClear (GLbitfield mask);
    void glClearAccum (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    void glClearColor (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
    void glClearDepth (GLclampd depth);
    void glClearIndex (GLfloat c);
    void glClearStencil (GLint s);
    void glClipPlane (GLenum plane, const GLdouble *equation);
    void glColor3b (GLbyte red, GLbyte green, GLbyte blue);
    void glColor3bv (const GLbyte *v);
    void glColor3d (GLdouble red, GLdouble green, GLdouble blue);
    void glColor3dv (const GLdouble *v);
    void glColor3f (GLfloat red, GLfloat green, GLfloat blue);
    void glColor3fv (const GLfloat *v);
    void glColor3i (GLint red, GLint green, GLint blue);
    void glColor3iv (const GLint *v);
    void glColor3s (GLshort red, GLshort green, GLshort blue);
    void glColor3sv (const GLshort *v);
    void glColor3ub (GLubyte red, GLubyte green, GLubyte blue);
    void glColor3ubv (const GLubyte *v);
    void glColor3ui (GLuint red, GLuint green, GLuint blue);
    void glColor3uiv (const GLuint *v);
    void glColor3us (GLushort red, GLushort green, GLushort blue);
    void glColor3usv (const GLushort *v);
    void glColor4b (GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
    void glColor4bv (const GLbyte *v);
    void glColor4d (GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
    void glColor4dv (const GLdouble *v);
    void glColor4f (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    void glColor4fv (const GLfloat *v);
    void glColor4i (GLint red, GLint green, GLint blue, GLint alpha);
    void glColor4iv (const GLint *v);
    void glColor4s (GLshort red, GLshort green, GLshort blue, GLshort alpha);
    void glColor4sv (const GLshort *v);
    void glColor4ub (GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
    void glColor4ubv (const GLubyte *v);
    void glColor4ui (GLuint red, GLuint green, GLuint blue, GLuint alpha);
    void glColor4uiv (const GLuint *v);
    void glColor4us (GLushort red, GLushort green, GLushort blue, GLushort alpha);
    void glColor4usv (const GLushort *v);
    void glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
    void glColorMaterial (GLenum face, GLenum mode);
    void glColorPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
    void glCopyPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
    void glCopyTexImage1D (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLint border);
    void glCopyTexImage2D (GLenum target, GLint level, GLenum internalFormat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
    void glCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
    void glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
    void glCullFace (GLenum mode);
    void glDeleteLists (GLuint list, GLsizei range);
    void glDeleteTextures (GLsizei n, const GLuint *textures);
    void glDepthFunc (GLenum func);
    void glDepthMask (GLboolean flag);
    void glDepthRange (GLclampd zNear, GLclampd zFar);
    void glDisable (GLenum cap);
    void glDisableClientState (GLenum array);
    void glDrawArrays (GLenum mode, GLint first, GLsizei count);
    void glDrawBuffer (GLenum mode);
    void glDrawElements (GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
    void glDrawPixels (GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
    void glEdgeFlag (GLboolean flag);
    void glEdgeFlagPointer (GLsizei stride, const GLvoid *pointer);
    void glEdgeFlagv (const GLboolean *flag);
    void glEnable (GLenum cap);
    void glEnableClientState (GLenum array);
    void glEnd (void);
    void glEndList (void);
    void glEvalCoord1d (GLdouble u);
    void glEvalCoord1dv (const GLdouble *u);
    void glEvalCoord1f (GLfloat u);
    void glEvalCoord1fv (const GLfloat *u);
    void glEvalCoord2d (GLdouble u, GLdouble v);
    void glEvalCoord2dv (const GLdouble *u);
    void glEvalCoord2f (GLfloat u, GLfloat v);
    void glEvalCoord2fv (const GLfloat *u);
    void glEvalMesh1 (GLenum mode, GLint i1, GLint i2);
    void glEvalMesh2 (GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
    void glEvalPoint1 (GLint i);
    void glEvalPoint2 (GLint i, GLint j);
    void glFeedbackBuffer (GLsizei size, GLenum type, GLfloat *buffer);
    void glFinish (void);
    void glFlush (void);
    void glFogf (GLenum pname, GLfloat param);
    void glFogfv (GLenum pname, const GLfloat *params);
    void glFogi (GLenum pname, GLint param);
    void glFogiv (GLenum pname, const GLint *params);
    void glFrontFace (GLenum mode);
    void glFrustum (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
    GLuint glGenLists (GLsizei range);
    void glGenTextures (GLsizei n, GLuint *textures);
    void glGetBooleanv (GLenum pname, GLboolean *params);
    void glGetClipPlane (GLenum plane, GLdouble *equation);
    void glGetDoublev (GLenum pname, GLdouble *params);
    GLenum glGetError (void);
    void glGetFloatv (GLenum pname, GLfloat *params);
    void glGetIntegerv (GLenum pname, GLint *params);
    void glGetLightfv (GLenum light, GLenum pname, GLfloat *params);
    void glGetLightiv (GLenum light, GLenum pname, GLint *params);
    void glGetMapdv (GLenum target, GLenum query, GLdouble *v);
    void glGetMapfv (GLenum target, GLenum query, GLfloat *v);
    void glGetMapiv (GLenum target, GLenum query, GLint *v);
    void glGetMaterialfv (GLenum face, GLenum pname, GLfloat *params);
    void glGetMaterialiv (GLenum face, GLenum pname, GLint *params);
    void glGetPixelMapfv (GLenum map, GLfloat *values);
    void glGetPixelMapuiv (GLenum map, GLuint *values);
    void glGetPixelMapusv (GLenum map, GLushort *values);
    void glGetPointerv (GLenum pname, GLvoid* *params);
    void glGetPolygonStipple (GLubyte *mask);
    const GLubyte * glGetString (GLenum name);
    void glGetTexEnvfv (GLenum target, GLenum pname, GLfloat *params);
    void glGetTexEnviv (GLenum target, GLenum pname, GLint *params);
    void glGetTexGendv (GLenum coord, GLenum pname, GLdouble *params);
    void glGetTexGenfv (GLenum coord, GLenum pname, GLfloat *params);
    void glGetTexGeniv (GLenum coord, GLenum pname, GLint *params);
    void glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
    void glGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat *params);
    void glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params);
    void glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params);
    void glGetTexParameteriv (GLenum target, GLenum pname, GLint *params);
    void glHint (GLenum target, GLenum mode);
    void glIndexMask (GLuint mask);
    void glIndexPointer (GLenum type, GLsizei stride, const GLvoid *pointer);
    void glIndexd (GLdouble c);
    void glIndexdv (const GLdouble *c);
    void glIndexf (GLfloat c);
    void glIndexfv (const GLfloat *c);
    void glIndexi (GLint c);
    void glIndexiv (const GLint *c);
    void glIndexs (GLshort c);
    void glIndexsv (const GLshort *c);
    void glIndexub (GLubyte c);
    void glIndexubv (const GLubyte *c);
    void glInitNames (void);
    void glInterleavedArrays (GLenum format, GLsizei stride, const GLvoid *pointer);
    GLboolean glIsEnabled (GLenum cap);
    GLboolean glIsList (GLuint list);
    GLboolean glIsTexture (GLuint texture);
    void glLightModelf (GLenum pname, GLfloat param);
    void glLightModelfv (GLenum pname, const GLfloat *params);
    void glLightModeli (GLenum pname, GLint param);
    void glLightModeliv (GLenum pname, const GLint *params);
    void glLightf (GLenum light, GLenum pname, GLfloat param);
    void glLightfv (GLenum light, GLenum pname, const GLfloat *params);
    void glLighti (GLenum light, GLenum pname, GLint param);
    void glLightiv (GLenum light, GLenum pname, const GLint *params);
    void glLineStipple (GLint factor, GLushort pattern);
    void glLineWidth (GLfloat width);
    void glListBase (GLuint base);
    void glLoadIdentity (void);
    void glLoadMatrixd (const GLdouble *m);
    void glLoadMatrixf (const GLfloat *m);
    void glLoadName (GLuint name);
    void glLogicOp (GLenum opcode);
    void glMap1d (GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points);
    void glMap1f (GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points);
    void glMap2d (GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points);
    void glMap2f (GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points);
    void glMapGrid1d (GLint un, GLdouble u1, GLdouble u2);
    void glMapGrid1f (GLint un, GLfloat u1, GLfloat u2);
    void glMapGrid2d (GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
    void glMapGrid2f (GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
    void glMaterialf (GLenum face, GLenum pname, GLfloat param);
    void glMaterialfv (GLenum face, GLenum pname, const GLfloat *params);
    void glMateriali (GLenum face, GLenum pname, GLint param);
    void glMaterialiv (GLenum face, GLenum pname, const GLint *params);
    void glMatrixMode (GLenum mode);
    void glMultMatrixd (const GLdouble *m);
    void glMultMatrixf (const GLfloat *m);
    void glNewList (GLuint list, GLenum mode);
    void glNormal3b (GLbyte nx, GLbyte ny, GLbyte nz);
    void glNormal3bv (const GLbyte *v);
    void glNormal3d (GLdouble nx, GLdouble ny, GLdouble nz);
    void glNormal3dv (const GLdouble *v);
    void glNormal3f (GLfloat nx, GLfloat ny, GLfloat nz);
    void glNormal3fv (const GLfloat *v);
    void glNormal3i (GLint nx, GLint ny, GLint nz);
    void glNormal3iv (const GLint *v);
    void glNormal3s (GLshort nx, GLshort ny, GLshort nz);
    void glNormal3sv (const GLshort *v);
    void glNormalPointer (GLenum type, GLsizei stride, const GLvoid *pointer);
    void glOrtho (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
    void glPassThrough (GLfloat token);
    void glPixelMapfv (GLenum map, GLsizei mapsize, const GLfloat *values);
    void glPixelMapuiv (GLenum map, GLsizei mapsize, const GLuint *values);
    void glPixelMapusv (GLenum map, GLsizei mapsize, const GLushort *values);
    void glPixelStoref (GLenum pname, GLfloat param);
    void glPixelStorei (GLenum pname, GLint param);
    void glPixelTransferf (GLenum pname, GLfloat param);
    void glPixelTransferi (GLenum pname, GLint param);
    void glPixelZoom (GLfloat xfactor, GLfloat yfactor);
    void glPointSize (GLfloat size);
    void glPolygonMode (GLenum face, GLenum mode);
    void glPolygonOffset (GLfloat factor, GLfloat units);
    void glPolygonStipple (const GLubyte *mask);
    void glPopAttrib (void);
    void glPopClientAttrib (void);
    void glPopMatrix (void);
    void glPopName (void);
    void glPrioritizeTextures (GLsizei n, const GLuint *textures, const GLclampf *priorities);
    void glPushAttrib (GLbitfield mask);
    void glPushClientAttrib (GLbitfield mask);
    void glPushMatrix (void);
    void glPushName (GLuint name);
    void glRasterPos2d (GLdouble x, GLdouble y);
    void glRasterPos2dv (const GLdouble *v);
    void glRasterPos2f (GLfloat x, GLfloat y);
    void glRasterPos2fv (const GLfloat *v);
    void glRasterPos2i (GLint x, GLint y);
    void glRasterPos2iv (const GLint *v);
    void glRasterPos2s (GLshort x, GLshort y);
    void glRasterPos2sv (const GLshort *v);
    void glRasterPos3d (GLdouble x, GLdouble y, GLdouble z);
    void glRasterPos3dv (const GLdouble *v);
    void glRasterPos3f (GLfloat x, GLfloat y, GLfloat z);
    void glRasterPos3fv (const GLfloat *v);
    void glRasterPos3i (GLint x, GLint y, GLint z);
    void glRasterPos3iv (const GLint *v);
    void glRasterPos3s (GLshort x, GLshort y, GLshort z);
    void glRasterPos3sv (const GLshort *v);
    void glRasterPos4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
    void glRasterPos4dv (const GLdouble *v);
    void glRasterPos4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void glRasterPos4fv (const GLfloat *v);
    void glRasterPos4i (GLint x, GLint y, GLint z, GLint w);
    void glRasterPos4iv (const GLint *v);
    void glRasterPos4s (GLshort x, GLshort y, GLshort z, GLshort w);
    void glRasterPos4sv (const GLshort *v);
    void glReadBuffer (GLenum mode);
    void glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
    void glRectd (GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
    void glRectdv (const GLdouble *v1, const GLdouble *v2);
    void glRectf (GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
    void glRectfv (const GLfloat *v1, const GLfloat *v2);
    void glRecti (GLint x1, GLint y1, GLint x2, GLint y2);
    void glRectiv (const GLint *v1, const GLint *v2);
    void glRects (GLshort x1, GLshort y1, GLshort x2, GLshort y2);
    void glRectsv (const GLshort *v1, const GLshort *v2);
    GLint glRenderMode (GLenum mode);
    void glRotated (GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
    void glRotatef (GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
    void glScaled (GLdouble x, GLdouble y, GLdouble z);
    void glScalef (GLfloat x, GLfloat y, GLfloat z);
    void glScissor (GLint x, GLint y, GLsizei width, GLsizei height);
    void glSelectBuffer (GLsizei size, GLuint *buffer);
    void glShadeModel (GLenum mode);
    void glStencilFunc (GLenum func, GLint ref, GLuint mask);
    void glStencilMask (GLuint mask);
    void glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
    void glTexCoord1d (GLdouble s);
    void glTexCoord1dv (const GLdouble *v);
    void glTexCoord1f (GLfloat s);
    void glTexCoord1fv (const GLfloat *v);
    void glTexCoord1i (GLint s);
    void glTexCoord1iv (const GLint *v);
    void glTexCoord1s (GLshort s);
    void glTexCoord1sv (const GLshort *v);
    void glTexCoord2d (GLdouble s, GLdouble t);
    void glTexCoord2dv (const GLdouble *v);
    void glTexCoord2f (GLfloat s, GLfloat t);
    void glTexCoord2fv (const GLfloat *v);
    void glTexCoord2i (GLint s, GLint t);
    void glTexCoord2iv (const GLint *v);
    void glTexCoord2s (GLshort s, GLshort t);
    void glTexCoord2sv (const GLshort *v);
    void glTexCoord3d (GLdouble s, GLdouble t, GLdouble r);
    void glTexCoord3dv (const GLdouble *v);
    void glTexCoord3f (GLfloat s, GLfloat t, GLfloat r);
    void glTexCoord3fv (const GLfloat *v);
    void glTexCoord3i (GLint s, GLint t, GLint r);
    void glTexCoord3iv (const GLint *v);
    void glTexCoord3s (GLshort s, GLshort t, GLshort r);
    void glTexCoord3sv (const GLshort *v);
    void glTexCoord4d (GLdouble s, GLdouble t, GLdouble r, GLdouble q);
    void glTexCoord4dv (const GLdouble *v);
    void glTexCoord4f (GLfloat s, GLfloat t, GLfloat r, GLfloat q);
    void glTexCoord4fv (const GLfloat *v);
    void glTexCoord4i (GLint s, GLint t, GLint r, GLint q);
    void glTexCoord4iv (const GLint *v);
    void glTexCoord4s (GLshort s, GLshort t, GLshort r, GLshort q);
    void glTexCoord4sv (const GLshort *v);
    void glTexCoordPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
    void glTexEnvf (GLenum target, GLenum pname, GLfloat param);
    void glTexEnvfv (GLenum target, GLenum pname, const GLfloat *params);
    void glTexEnvi (GLenum target, GLenum pname, GLint param);
    void glTexEnviv (GLenum target, GLenum pname, const GLint *params);
    void glTexGend (GLenum coord, GLenum pname, GLdouble param);
    void glTexGendv (GLenum coord, GLenum pname, const GLdouble *params);
    void glTexGenf (GLenum coord, GLenum pname, GLfloat param);
    void glTexGenfv (GLenum coord, GLenum pname, const GLfloat *params);
    void glTexGeni (GLenum coord, GLenum pname, GLint param);
    void glTexGeniv (GLenum coord, GLenum pname, const GLint *params);
    void glTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
    void glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
    void glTexParameterf (GLenum target, GLenum pname, GLfloat param);
    void glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params);
    void glTexParameteri (GLenum target, GLenum pname, GLint param);
    void glTexParameteriv (GLenum target, GLenum pname, const GLint *params);
    void glTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
    void glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
    void glTranslated (GLdouble x, GLdouble y, GLdouble z);
    void glTranslatef (GLfloat x, GLfloat y, GLfloat z);
    void glVertex2d (GLdouble x, GLdouble y);
    void glVertex2dv (const GLdouble *v);
    void glVertex2f (GLfloat x, GLfloat y);
    void glVertex2fv (const GLfloat *v);
    void glVertex2i (GLint x, GLint y);
    void glVertex2iv (const GLint *v);
    void glVertex2s (GLshort x, GLshort y);
    void glVertex2sv (const GLshort *v);
    void glVertex3d (GLdouble x, GLdouble y, GLdouble z);
    void glVertex3dv (const GLdouble *v);
    void glVertex3f (GLfloat x, GLfloat y, GLfloat z);
    void glVertex3fv (const GLfloat *v);
    void glVertex3i (GLint x, GLint y, GLint z);
    void glVertex3iv (const GLint *v);
    void glVertex3s (GLshort x, GLshort y, GLshort z);
    void glVertex3sv (const GLshort *v);
    void glVertex4d (GLdouble x, GLdouble y, GLdouble z, GLdouble w);
    void glVertex4dv (const GLdouble *v);
    void glVertex4f (GLfloat x, GLfloat y, GLfloat z, GLfloat w);
    void glVertex4fv (const GLfloat *v);
    void glVertex4i (GLint x, GLint y, GLint z, GLint w);
    void glVertex4iv (const GLint *v);
    void glVertex4s (GLshort x, GLshort y, GLshort z, GLshort w);
    void glVertex4sv (const GLshort *v);
    void glVertexPointer (GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
    void glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
    void gluBeginCurve (GLUnurbs* nurb);
    void gluBeginPolygon (GLUtesselator* tess);
    void gluBeginSurface (GLUnurbs* nurb);
    void gluBeginTrim (GLUnurbs* nurb);
    GLint gluBuild1DMipmaps (GLenum target, GLint internalFormat, GLsizei width, GLenum format, GLenum type, const void *data);
    GLint gluBuild2DMipmaps (GLenum target, GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *data);
    void gluCylinder (GLUquadric* quad, GLdouble base, GLdouble top, GLdouble height, GLint slices, GLint stacks);
    void gluDeleteNurbsRenderer (GLUnurbs* nurb);
    void gluDeleteQuadric (GLUquadric* quad);
    void gluDeleteTess (GLUtesselator* tess);
    void gluDisk (GLUquadric* quad, GLdouble inner, GLdouble outer, GLint slices, GLint loops);
    void gluEndCurve (GLUnurbs* nurb);
    void gluEndPolygon (GLUtesselator* tess);
    void gluEndSurface (GLUnurbs* nurb);
    void gluEndTrim (GLUnurbs* nurb);
    const GLubyte * gluErrorString (GLenum error);
    void gluGetNurbsProperty (GLUnurbs* nurb, GLenum property, GLfloat* data);
    const GLubyte * gluGetString (GLenum name);
    void gluGetTessProperty (GLUtesselator* tess, GLenum which, GLdouble* data);
    void gluLoadSamplingMatrices (GLUnurbs* nurb, const GLfloat *model, const GLfloat *perspective, const GLint *view);
    void gluLookAt (GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ, GLdouble centerX, GLdouble centerY, GLdouble centerZ, GLdouble upX, GLdouble upY, GLdouble upZ);
    GLUnurbs* gluNewNurbsRenderer (void);
    GLUquadric* gluNewQuadric (void);
    GLUtesselator* gluNewTess (void);
    void gluNextContour (GLUtesselator* tess, GLenum type);
    void gluNurbsCallback (GLUnurbs* nurb, GLenum which, _GLUfuncptr CallBackFunc);
    void gluNurbsCurve (GLUnurbs* nurb, GLint knotCount, GLfloat *knots, GLint stride, GLfloat *control, GLint order, GLenum type);
    void gluNurbsProperty (GLUnurbs* nurb, GLenum property, GLfloat value);
    void gluNurbsSurface (GLUnurbs* nurb, GLint sKnotCount, GLfloat* sKnots, GLint tKnotCount, GLfloat* tKnots, GLint sStride, GLint tStride, GLfloat* control, GLint sOrder, GLint tOrder, GLenum type);
    void gluOrtho2D (GLdouble left, GLdouble right, GLdouble bottom, GLdouble top);
    void gluPartialDisk (GLUquadric* quad, GLdouble inner, GLdouble outer, GLint slices, GLint loops, GLdouble start, GLdouble sweep);
    void gluPerspective (GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
    void gluPickMatrix (GLdouble x, GLdouble y, GLdouble delX, GLdouble delY, GLint *viewport);
    GLint gluProject (GLdouble objX, GLdouble objY, GLdouble objZ, const GLdouble *model, const GLdouble *proj, const GLint *view, GLdouble* winX, GLdouble* winY, GLdouble* winZ);
    void gluPwlCurve (GLUnurbs* nurb, GLint count, GLfloat* data, GLint stride, GLenum type);
    void gluQuadricCallback (GLUquadric* quad, GLenum which, _GLUfuncptr CallBackFunc);
    void gluQuadricDrawStyle (GLUquadric* quad, GLenum draw);
    void gluQuadricNormals (GLUquadric* quad, GLenum normal);
    void gluQuadricOrientation (GLUquadric* quad, GLenum orientation);
    void gluQuadricTexture (GLUquadric* quad, GLboolean texture);
    GLint gluScaleImage (GLenum format, GLsizei wIn, GLsizei hIn, GLenum typeIn, const void *dataIn, GLsizei wOut, GLsizei hOut, GLenum typeOut, GLvoid* dataOut);
    void gluSphere (GLUquadric* quad, GLdouble radius, GLint slices, GLint stacks);
    void gluTessBeginContour (GLUtesselator* tess);
    void gluTessBeginPolygon (GLUtesselator* tess, GLvoid* data);
    void gluTessCallback (GLUtesselator* tess, GLenum which, _GLUfuncptr CallBackFunc);
    void gluTessEndContour (GLUtesselator* tess);
    void gluTessEndPolygon (GLUtesselator* tess);
    void gluTessNormal (GLUtesselator* tess, GLdouble valueX, GLdouble valueY, GLdouble valueZ);
    void gluTessProperty (GLUtesselator* tess, GLenum which, GLdouble data);
    void gluTessVertex (GLUtesselator* tess, GLdouble *location, GLvoid* data);
    GLint gluUnProject (GLdouble winX, GLdouble winY, GLdouble winZ, const GLdouble *model, const GLdouble *proj, const GLint *view, GLdouble* objX, GLdouble* objY, GLdouble* objZ);
    GLint gluUnProject4 (GLdouble winX, GLdouble winY, GLdouble winZ, GLdouble clipW, const GLdouble *model, const GLdouble *proj, const GLint *view, GLdouble nearVal, GLdouble farVal, GLdouble* objX, GLdouble* objY, GLdouble* objZ, GLdouble* objW);
    XVisualInfo* glXChooseVisual (Display *dpy, int screen,int *attrib_list);
    void glXCopyContext (Display *dpy, GLXContext src,GLXContext dst, unsigned long mask);
    GLXContext glXCreateContext (Display *dpy, XVisualInfo *vis,GLXContext share_list, Bool direct);
    GLXPixmap glXCreateGLXPixmap (Display *dpy, XVisualInfo *vis,Pixmap pixmap);
    void glXDestroyContext (Display *dpy, GLXContext ctx);
    void glXDestroyGLXPixmap (Display *dpy, GLXPixmap pix);
    int glXGetConfig (Display *dpy, XVisualInfo *vis,int attrib, int *value);
    GLXContext glXGetCurrentContext (void);
    GLXDrawable glXGetCurrentDrawable (void);
    Bool glXIsDirect (Display *dpy, GLXContext ctx);
    Bool glXMakeCurrent (Display *dpy, GLXDrawable drawable,GLXContext ctx);
    Bool glXQueryExtension (Display *dpy, int *error_base, int *event_base);
    Bool glXQueryVersion (Display *dpy, int *major, int *minor);
    void glXSwapBuffers (Display *dpy, GLXDrawable drawable);
    void glXUseXFont (Font font, int first, int count, int list_base);
    void glXWaitGL (void);
    void glXWaitX (void);
    const char * glXGetClientString (Display *dpy, int name);
    const char * glXQueryServerString (Display *dpy, int screen, int name);
    const char * glXQueryExtensionsString (Display *dpy, int screen);
    Display * glXGetCurrentDisplay (void);
    GLXFBConfig * glXChooseFBConfig (Display *dpy, int screen,const int *attrib_list, int *nelements);
    GLXContext glXCreateNewContext (Display *dpy, GLXFBConfig config,int render_type, GLXContext share_list,Bool direct);
    GLXPbuffer glXCreatePbuffer (Display *dpy, GLXFBConfig config,const int *attrib_list);
    GLXPixmap glXCreatePixmap (Display *dpy, GLXFBConfig config,Pixmap pixmap, const int *attrib_list);
    GLXWindow glXCreateWindow (Display *dpy, GLXFBConfig config,Window win, const int *attrib_list);
    void glXDestroyPbuffer (Display *dpy, GLXPbuffer pbuf);
    void glXDestroyPixmap (Display *dpy, GLXPixmap pixmap);
    void glXDestroyWindow (Display *dpy, GLXWindow win);
    GLXDrawable glXGetCurrentReadDrawable (void);
    int glXGetFBConfigAttrib (Display *dpy, GLXFBConfig config,int attribute, int *value);
    GLXFBConfig * glXGetFBConfigs (Display *dpy, int screen, int *nelements);
    void glXGetSelectedEvent (Display *dpy, GLXDrawable draw,unsigned long *event_mask);
    XVisualInfo * glXGetVisualFromFBConfig (Display *dpy, GLXFBConfig config);
    Bool glXMakeContextCurrent (Display *display, GLXDrawable draw,GLXDrawable read, GLXContext ctx);
    int glXQueryContext (Display *dpy, GLXContext ctx,int attribute, int *value);
    void glXQueryDrawable (Display *dpy, GLXDrawable draw,int attribute, unsigned int *value);
    void glXSelectEvent (Display *dpy, GLXDrawable draw,unsigned long event_mask);
    void glXFreeContextEXT (Display *dpy, GLXContext ctx);
    GLXContextID glXGetContextIDEXT (const GLXContext ctx);
    GLXContext glXImportContextEXT (Display *dpy, GLXContextID contextID);
    int glXQueryContextInfoEXT (Display *dpy, GLXContext ctx,int attribute, int *value);
    void * glXAllocateMemoryNV (GLsizei size, GLfloat readfreq,				 GLfloat writefreq, GLfloat priority);
    void glXFreeMemoryNV (GLvoid *pointer);
    int glXGetVideoSyncSGI (unsigned int *count);
    int glXWaitVideoSyncSGI (int divisor, int remainder,unsigned int *count);
    int glXGetRefreshRateSGI (unsigned int *rate);
    int glXSwapIntervalSGI (int interval);
    Bool glXJoinSwapGroupNV (Display *dpy, GLXDrawable drawable,GLuint group);
    Bool glXBindSwapBarrierNV (Display *dpy, GLuint group, GLuint barrier);
    Bool glXQuerySwapGroupNV (Display *dpy, GLXDrawable drawable,GLuint *group, GLuint *barrier);
    Bool glXQueryMaxSwapGroupsNV (Display *dpy, int screen,GLuint *maxGroups, GLuint *maxBarriers);
    Bool glXQueryFrameCountNV (Display *dpy, int screen, GLuint *count);
    Bool glXResetFrameCountNV (Display *dpy, int screen);
    int glXGetVideoDeviceNV (Display *dpy, int screen, int numVideoDevices,GLXVideoDeviceNV *pVideoDevice);
    int glXReleaseVideoDeviceNV (Display *dpy, int screen,GLXVideoDeviceNV VideoDevice);
    int glXBindVideoImageNV (Display *dpy, GLXVideoDeviceNV VideoDevice,GLXPbuffer pbuf, int iVideoBuffer);
    int glXReleaseVideoImageNV (Display *dpy, GLXPbuffer pbuf);
    int glXSendPbufferToVideoNV (Display *dpy, GLXPbuffer pbuf,int iBufferType,unsigned long *pulCounterPbuffer,GLboolean bBlock);
    int glXGetVideoInfoNV (Display *dpy, int screen,GLXVideoDeviceNV VideoDevice,unsigned long *pulCounterOutputVideo,unsigned long *pulCounterOutputPbuffer);
    int glXGetFBConfigAttribSGIX (Display *dpy, GLXFBConfigSGIX config,int attribute, int *value_return);
    GLXFBConfigSGIX * glXChooseFBConfigSGIX (Display *dpy, int screen,const int *attrib_list,int *nelements);
    GLXPixmap glXCreateGLXPixmapWithConfigSGIX (Display *dpy,GLXFBConfigSGIX config,Pixmap pixmap);
    GLXContext glXCreateContextWithConfigSGIX (Display *dpy,GLXFBConfigSGIX config,int render_type,GLXContext share_list,Bool direct);
    XVisualInfo * glXGetVisualFromFBConfigSGIX (Display *dpy,GLXFBConfigSGIX config);
    GLXFBConfigSGIX glXGetFBConfigFromVisualSGIX (Display *dpy,XVisualInfo *vis);
    GLXPbuffer glXCreateGLXPbufferSGIX (Display *dpy, GLXFBConfig config,unsigned int width,unsigned int height,const int *attrib_list);
    void glXDestroyGLXPbufferSGIX (Display *dpy, GLXPbuffer pbuf);
    void glXQueryGLXPbufferSGIX (Display *dpy, GLXPbuffer pbuf,int attribute, unsigned int *value);
    void glXSelectEventSGIX (Display *dpy, GLXDrawable drawable,unsigned long mask);
    void glXGetSelectedEventSGIX (Display *dpy, GLXDrawable drawable,unsigned long *mask);
    void * dlopen (__const char *__file, int __mode);
    int dlclose (void *__handle);
    void * dlsym (void *__restrict __handle,		    __const char *__restrict __name);
    void * dlmopen (Lmid_t __nsid, __const char *__file, int __mode);
    void * dlvsym (void *__restrict __handle,		     __const char *__restrict __name,		     __const char *__restrict __version);
    char * dlerror (void);
    int dladdr (__const void *__address, Dl_info *__info);
    int dladdr1 (__const void *__address, Dl_info *__info,		    void **__extra_info, int __flags);
    int dlinfo (void *__restrict __handle,		   int __request, void *__restrict __arg);
    int pthread_sigmask (int __how,			    __const __sigset_t *__restrict __newmask,			    __sigset_t *__restrict __oldmask);
    int pthread_kill (pthread_t __threadid, int __signo);
    pthread_t pthread_self (void);
    int pthread_equal (pthread_t __thread1, pthread_t __thread2);
    void pthread_exit (void *__retval);
    int pthread_join (pthread_t __th, void **__thread_return);
    int pthread_detach (pthread_t __th);
    int pthread_attr_init (pthread_attr_t *__attr);
    int pthread_attr_destroy (pthread_attr_t *__attr);
    int pthread_attr_setdetachstate (pthread_attr_t *__attr,					int __detachstate);
    int pthread_attr_getdetachstate (__const pthread_attr_t *__attr,					int *__detachstate);
    int pthread_attr_setschedparam (pthread_attr_t *__restrict __attr,				       __const struct sched_param *__restrict				       __param);
    int pthread_attr_getschedparam (__const pthread_attr_t *__restrict				       __attr,				       struct sched_param *__restrict __param);
    int pthread_attr_setschedpolicy (pthread_attr_t *__attr, int __policy);
    int pthread_attr_getschedpolicy (__const pthread_attr_t *__restrict					__attr, int *__restrict __policy);
    int pthread_attr_setinheritsched (pthread_attr_t *__attr,					 int __inherit);
    int pthread_attr_getinheritsched (__const pthread_attr_t *__restrict					 __attr, int *__restrict __inherit);
    int pthread_attr_setscope (pthread_attr_t *__attr, int __scope);
    int pthread_attr_getscope (__const pthread_attr_t *__restrict __attr,				  int *__restrict __scope);
    int pthread_attr_setguardsize (pthread_attr_t *__attr,				      size_t __guardsize);
    int pthread_attr_getguardsize (__const pthread_attr_t *__restrict				      __attr, size_t *__restrict __guardsize);
    int pthread_attr_setstackaddr (pthread_attr_t *__attr,				      void *__stackaddr);
    int pthread_attr_getstackaddr (__const pthread_attr_t *__restrict				      __attr, void **__restrict __stackaddr);
    int pthread_attr_setstack (pthread_attr_t *__attr, void *__stackaddr,				  size_t __stacksize);
    int pthread_attr_getstack (__const pthread_attr_t *__restrict __attr,				  void **__restrict __stackaddr,				  size_t *__restrict __stacksize);
    int pthread_attr_setstacksize (pthread_attr_t *__attr,				      size_t __stacksize);
    int pthread_attr_getstacksize (__const pthread_attr_t *__restrict				      __attr, size_t *__restrict __stacksize);
    int pthread_getattr_np (pthread_t __th, pthread_attr_t *__attr);
    int pthread_setschedparam (pthread_t __target_thread, int __policy,				  __const struct sched_param *__param);
    int pthread_getschedparam (pthread_t __target_thread,				  int *__restrict __policy,				  struct sched_param *__restrict __param);
    int pthread_getconcurrency (void);
    int pthread_setconcurrency (int __level);
    int pthread_yield (void);
    int pthread_mutex_init (pthread_mutex_t *__restrict __mutex,			       __const pthread_mutexattr_t *__restrict			       __mutex_attr);
    int pthread_mutex_destroy (pthread_mutex_t *__mutex);
    int pthread_mutex_trylock (pthread_mutex_t *__mutex);
    int pthread_mutex_lock (pthread_mutex_t *__mutex);
    int pthread_mutex_timedlock (pthread_mutex_t *__restrict __mutex,				    __const struct timespec *__restrict				    __abstime);
    int pthread_mutex_unlock (pthread_mutex_t *__mutex);
    int pthread_mutexattr_init (pthread_mutexattr_t *__attr);
    int pthread_mutexattr_destroy (pthread_mutexattr_t *__attr);
    int pthread_mutexattr_getpshared (__const pthread_mutexattr_t *					 __restrict __attr,					 int *__restrict __pshared);
    int pthread_mutexattr_setpshared (pthread_mutexattr_t *__attr,					 int __pshared);
    int pthread_mutexattr_settype (pthread_mutexattr_t *__attr, int __kind);
    int pthread_mutexattr_gettype (__const pthread_mutexattr_t *__restrict				      __attr, int *__restrict __kind);
    int pthread_cond_init (pthread_cond_t *__restrict __cond,			      __const pthread_condattr_t *__restrict			      __cond_attr);
    int pthread_cond_destroy (pthread_cond_t *__cond);
    int pthread_cond_signal (pthread_cond_t *__cond);
    int pthread_cond_broadcast (pthread_cond_t *__cond);
    int pthread_cond_wait (pthread_cond_t *__restrict __cond,			      pthread_mutex_t *__restrict __mutex);
    int pthread_cond_timedwait (pthread_cond_t *__restrict __cond,				   pthread_mutex_t *__restrict __mutex,				   __const struct timespec *__restrict				   __abstime);
    int pthread_condattr_init (pthread_condattr_t *__attr);
    int pthread_condattr_destroy (pthread_condattr_t *__attr);
    int pthread_condattr_getpshared (__const pthread_condattr_t *					__restrict __attr,					int *__restrict __pshared);
    int pthread_condattr_setpshared (pthread_condattr_t *__attr,					int __pshared);
    int pthread_rwlock_init (pthread_rwlock_t *__restrict __rwlock,				__const pthread_rwlockattr_t *__restrict				__attr);
    int pthread_rwlock_destroy (pthread_rwlock_t *__rwlock);
    int pthread_rwlock_rdlock (pthread_rwlock_t *__rwlock);
    int pthread_rwlock_tryrdlock (pthread_rwlock_t *__rwlock);
    int pthread_rwlock_timedrdlock (pthread_rwlock_t *__restrict __rwlock,				       __const struct timespec *__restrict				       __abstime);
    int pthread_rwlock_wrlock (pthread_rwlock_t *__rwlock);
    int pthread_rwlock_trywrlock (pthread_rwlock_t *__rwlock);
    int pthread_rwlock_timedwrlock (pthread_rwlock_t *__restrict __rwlock,				       __const struct timespec *__restrict				       __abstime);
    int pthread_rwlock_unlock (pthread_rwlock_t *__rwlock);
    int pthread_rwlockattr_init (pthread_rwlockattr_t *__attr);
    int pthread_rwlockattr_destroy (pthread_rwlockattr_t *__attr);
    int pthread_rwlockattr_getpshared (__const pthread_rwlockattr_t *					  __restrict __attr,					  int *__restrict __pshared);
    int pthread_rwlockattr_setpshared (pthread_rwlockattr_t *__attr,					  int __pshared);
    int pthread_rwlockattr_getkind_np (__const pthread_rwlockattr_t *__attr,					  int *__pref);
    int pthread_rwlockattr_setkind_np (pthread_rwlockattr_t *__attr,					  int __pref);
    int pthread_spin_init (pthread_spinlock_t *__lock, int __pshared);
    int pthread_spin_destroy (pthread_spinlock_t *__lock);
    int pthread_spin_lock (pthread_spinlock_t *__lock);
    int pthread_spin_trylock (pthread_spinlock_t *__lock);
    int pthread_spin_unlock (pthread_spinlock_t *__lock);
    int pthread_barrier_init (pthread_barrier_t *__restrict __barrier,				 __const pthread_barrierattr_t *__restrict				 __attr, unsigned int __count);
    int pthread_barrier_destroy (pthread_barrier_t *__barrier);
    int pthread_barrierattr_init (pthread_barrierattr_t *__attr);
    int pthread_barrierattr_destroy (pthread_barrierattr_t *__attr);
    int pthread_barrierattr_setpshared (pthread_barrierattr_t *__attr,					   int __pshared);
    int pthread_barrier_wait (pthread_barrier_t *__barrier);
    int pthread_key_delete (pthread_key_t __key);
    int pthread_setspecific (pthread_key_t __key,				__const void *__pointer);
    void * pthread_getspecific (pthread_key_t __key);
    int pthread_setcancelstate (int __state, int *__oldstate);
    int pthread_setcanceltype (int __type, int *__oldtype);
    int pthread_cancel (pthread_t __cancelthread);
    void pthread_testcancel (void);
    void _pthread_cleanup_pop (struct _pthread_cleanup_buffer *__buffer,				  int __execute);
    void _pthread_cleanup_pop_restore (struct _pthread_cleanup_buffer *__buffer,					  int __execute);
    int pthread_getcpuclockid (pthread_t __thread_id,				  __clockid_t *__clock_id);
    void pthread_kill_other_threads_np (void);
    Bool XRRQueryExtension (Display *dpy, int *event_basep, int *error_basep);
    Status XRRQueryVersion (Display *dpy,			    int     *major_versionp,			    int     *minor_versionp);
    XRRScreenConfiguration * XRRGetScreenInfo (Display *dpy,					  Drawable draw);
    void XRRFreeScreenConfigInfo (XRRScreenConfiguration *config);
    Status XRRSetScreenConfig (Display *dpy, 			   XRRScreenConfiguration *config,			   Drawable draw,			   int size_index,			   Rotation rotation,			   Time timestamp);
    Status XRRSetScreenConfigAndRate (Display *dpy, 				  XRRScreenConfiguration *config,				  Drawable draw,				  int size_index,				  Rotation rotation,				  short rate,				  Time timestamp);
    Rotation XRRConfigRotations (XRRScreenConfiguration *config, Rotation *current_rotation);
    Time XRRConfigTimes (XRRScreenConfiguration *config, Time *config_timestamp);
    XRRScreenSize * XRRConfigSizes (XRRScreenConfiguration *config, int *nsizes);
    short * XRRConfigRates (XRRScreenConfiguration *config, int sizeID, int *nrates);
    SizeID XRRConfigCurrentConfiguration (XRRScreenConfiguration *config, 			      Rotation *rotation);
    short XRRConfigCurrentRate (XRRScreenConfiguration *config);
    int XRRRootToScreen (Display *dpy, Window root);
    void XRRSelectInput (Display *dpy, Window window, int mask);
    Rotation XRRRotations (Display *dpy, int screen, Rotation *current_rotation);
    XRRScreenSize * XRRSizes (Display *dpy, int screen, int *nsizes);
    short * XRRRates (Display *dpy, int screen, int sizeID, int *nrates);
    Time XRRTimes (Display *dpy, int screen, Time *config_timestamp);
    int XRRUpdateConfiguration (XEvent *event);
    XFontStruct * XLoadQueryFont (Display*		,_Xconst char*	);
    XFontStruct * XQueryFont (Display*		,XID			);
    XTimeCoord * XGetMotionEvents (Display*		,Window		,Time		,Time		,int*		);
    XModifierKeymap * XDeleteModifiermapEntry (XModifierKeymap *modmap, KeyCode keycode_entry, int modifier);
    XModifierKeymap	* XGetModifierMapping (Display*		);
    XModifierKeymap * XInsertModifiermapEntry (XModifierKeymap *modmap, KeyCode keycode_entry, int modifier);
    XModifierKeymap * XNewModifiermap (int			);
    XImage * XCreateImage (Display*		,Visual*		,unsigned int	,int			,int			,char*		,unsigned int	,unsigned int	,int			,int			);
    Status XInitImage (XImage*		);
    XImage * XGetImage (Display*		,Drawable		,int			,int			,unsigned int	,unsigned int	,unsigned long	,int			);
    XImage * XGetSubImage (Display*		,Drawable		,int			,int			,unsigned int	,unsigned int	,unsigned long	,int			,XImage*		,int			,int			);
    Display * XOpenDisplay (_Xconst char*	);
    void XrmInitialize (void);
    char * XFetchBytes (Display*		,int*		);
    char * XFetchBuffer (Display*		,int*		,int			);
    char * XGetAtomName (Display*		,Atom		);
    Status XGetAtomNames (Display*		,Atom*		,int			,char**		);
    char * XGetDefault (Display*		,_Xconst char*	,_Xconst char*			  );
    char * XDisplayName (_Xconst char*	);
    char * XKeysymToString (KeySym		);
    Atom XInternAtom (Display*		,_Xconst char*	,Bool				 );
    Status XInternAtoms (Display*		,char**		,int			,Bool		,Atom*		);
    Colormap XCopyColormapAndFree (Display*		,Colormap		);
    Colormap XCreateColormap (Display*		,Window		,Visual*		,int						 );
    Cursor XCreatePixmapCursor (Display*		,Pixmap		,Pixmap		,XColor*		,XColor*		,unsigned int	,unsigned int				   );
    Cursor XCreateGlyphCursor (Display*		,Font		,Font		,unsigned int	,unsigned int	,XColor _Xconst *	,XColor _Xconst *	);
    Cursor XCreateFontCursor (Display*		,unsigned int	);
    Font XLoadFont (Display*		,_Xconst char*	);
    GC XCreateGC (Display*		,Drawable		,unsigned long	,XGCValues*		);
    GContext XGContextFromGC (GC			);
    void XFlushGC (Display*		,GC			);
    Pixmap XCreatePixmap (Display*		,Drawable		,unsigned int	,unsigned int	,unsigned int			        );
    Pixmap XCreateBitmapFromData (Display*		,Drawable		,_Xconst char*	,unsigned int	,unsigned int	);
    Pixmap XCreatePixmapFromBitmapData (Display*		,Drawable		,char*		,unsigned int	,unsigned int	,unsigned long	,unsigned long	,unsigned int	);
    Window XCreateSimpleWindow (Display*		,Window		,int			,int			,unsigned int	,unsigned int	,unsigned int	,unsigned long	,unsigned long	);
    Window XGetSelectionOwner (Display*		,Atom		);
    Window XCreateWindow (Display*		,Window		,int			,int			,unsigned int	,unsigned int	,unsigned int	,int			,unsigned int	,Visual*		,unsigned long	,XSetWindowAttributes*	);
    Colormap * XListInstalledColormaps (Display*		,Window		,int*		);
    char ** XListFonts (Display*		,_Xconst char*	,int			,int*		);
    char ** XListFontsWithInfo (Display*		,_Xconst char*	,int			,int*		,XFontStruct**	);
    char ** XGetFontPath (Display*		,int*		);
    char ** XListExtensions (Display*		,int*		);
    Atom * XListProperties (Display*		,Window		,int*		);
    XHostAddress * XListHosts (Display*		,int*		,Bool*		);
    KeySym XKeycodeToKeysym (Display *display, KeyCode keycode, int index);
    KeySym XLookupKeysym (XKeyEvent*		,int			);
    KeySym * XGetKeyboardMapping (Display *display, KeyCode keycode, int index,int *keysyms_per_keycode_return);
    KeySym XStringToKeysym (_Xconst char*	);
    long XMaxRequestSize (Display*		);
    long XExtendedMaxRequestSize (Display*		);
    char * XResourceManagerString (Display*		);
    char * XScreenResourceString (	Screen*		);
    unsigned long XDisplayMotionBufferSize (Display*		);
    VisualID XVisualIDFromVisual (Visual*		);
    Status XInitThreads (void);
    void XLockDisplay (Display*		);
    void XUnlockDisplay (Display*		);
    XExtCodes * XInitExtension (Display*		,_Xconst char*	);
    XExtCodes * XAddExtension (Display*		);
    XExtData * XFindOnExtensionList (XExtData**		,int			);
    XExtData ** XEHeadOfExtensionList (XEDataObject	);
    Window XRootWindow (Display*		,int			);
    Window XDefaultRootWindow (Display*		);
    Window XRootWindowOfScreen (Screen*		);
    Visual * XDefaultVisual (Display*		,int			);
    Visual * XDefaultVisualOfScreen (Screen*		);
    GC XDefaultGC (Display*		,int			);
    GC XDefaultGCOfScreen (Screen*		);
    unsigned long XBlackPixel (Display*		,int			);
    unsigned long XWhitePixel (Display*		,int			);
    unsigned long XAllPlanes (void);
    unsigned long XBlackPixelOfScreen (Screen*		);
    unsigned long XWhitePixelOfScreen (Screen*		);
    unsigned long XNextRequest (Display*		);
    unsigned long XLastKnownRequestProcessed (Display*		);
    char * XServerVendor (Display*		);
    char * XDisplayString (Display*		);
    Colormap XDefaultColormap (Display*		,int			);
    Colormap XDefaultColormapOfScreen (Screen*		);
    Display * XDisplayOfScreen (Screen*		);
    Screen * XScreenOfDisplay (Display*		,int			);
    Screen * XDefaultScreenOfDisplay (Display*		);
    long XEventMaskOfScreen (Screen*		);
    int XScreenNumberOfScreen (Screen*		);
    XErrorHandler XSetErrorHandler (XErrorHandler	);
    XIOErrorHandler XSetIOErrorHandler (XIOErrorHandler	);
    XPixmapFormatValues * XListPixmapFormats (Display*		,int*		);
    int * XListDepths (Display*		,int			,int*		);
    Status XReconfigureWMWindow (Display*		,Window		,int			,unsigned int	,XWindowChanges*	);
    Status XGetWMProtocols (Display*		,Window		,Atom**		,int*		);
    Status XSetWMProtocols (Display*		,Window		,Atom*		,int			);
    Status XIconifyWindow (Display*		,Window		,int			);
    Status XWithdrawWindow (Display*		,Window		,int			);
    Status XGetCommand (Display*		,Window		,char***		,int*		);
    Status XGetWMColormapWindows (Display*		,Window		,Window**		,int*		);
    Status XSetWMColormapWindows (Display*		,Window		,Window*		,int			);
    void XFreeStringList (char**		);
    int XSetTransientForHint (Display*		,Window		,Window		);
    int XActivateScreenSaver (Display*		);
    int XAddHost (Display*		,XHostAddress*	);
    int XAddHosts (Display*		,XHostAddress*	,int			    );
    int XAddToExtensionList (struct _XExtData**	,XExtData*		);
    int XAddToSaveSet (Display*		,Window		);
    Status XAllocColor (Display*		,Colormap		,XColor*		);
    Status XAllocColorCells (Display*		,Colormap		,Bool	        ,unsigned long*	,unsigned int	,unsigned long*	,unsigned int 	);
    Status XAllocColorPlanes (Display*		,Colormap		,Bool		,unsigned long*	,int			,int			,int			,int			,unsigned long*	,unsigned long*	,unsigned long*	);
    Status XAllocNamedColor (Display*		,Colormap		,_Xconst char*	,XColor*		,XColor*		);
    int XAllowEvents (Display*		,int			,Time		);
    int XAutoRepeatOff (Display*		);
    int XAutoRepeatOn (Display*		);
    int XBell (Display*		,int			);
    int XBitmapBitOrder (Display*		);
    int XBitmapPad (Display*		);
    int XBitmapUnit (Display*		);
    int XCellsOfScreen (Screen*		);
    int XChangeActivePointerGrab (Display*		,unsigned int	,Cursor		,Time		);
    int XChangeGC (Display*		,GC			,unsigned long	,XGCValues*		);
    int XChangeKeyboardControl (Display*		,unsigned long	,XKeyboardControl*	);
    int XChangeKeyboardMapping (Display*		,int			,int			,KeySym*		,int			);
    int XChangePointerControl (Display*		,Bool		,Bool		,int			,int			,int			);
    int XChangeProperty (Display*		,Window		,Atom		,Atom		,int			,int			,_Xconst unsigned char*	,int			);
    int XChangeSaveSet (Display*		,Window		,int			);
    int XChangeWindowAttributes (Display*		,Window		,unsigned long	,XSetWindowAttributes* );
    Bool XCheckMaskEvent (Display*		,long		,XEvent*		);
    Bool XCheckTypedEvent (Display*		,int			,XEvent*		);
    Bool XCheckTypedWindowEvent (Display*		,Window		,int			,XEvent*		);
    Bool XCheckWindowEvent (Display*		,Window		,long		,XEvent*		);
    int XCirculateSubwindows (Display*		,Window		,int			);
    int XCirculateSubwindowsDown (Display*		,Window		);
    int XCirculateSubwindowsUp (Display*		,Window		);
    int XClearArea (Display*		,Window		,int			,int			,unsigned int	,unsigned int	,Bool		);
    int XClearWindow (Display*		,Window		);
    int XCloseDisplay (Display*		);
    int XConfigureWindow (Display*		,Window		,unsigned int	,XWindowChanges*			 );
    int XConnectionNumber (Display*		);
    int XConvertSelection (Display*		,Atom		,Atom 		,Atom		,Window		,Time		);
    int XCopyArea (Display*		,Drawable		,Drawable		,GC			,int			,int			,unsigned int	,unsigned int	,int			,int			);
    int XCopyGC (Display*		,GC			,unsigned long	,GC			);
    int XCopyPlane (Display*		,Drawable		,Drawable		,GC			,int			,int			,unsigned int	,unsigned int	,int			,int			,unsigned long	);
    int XDefaultDepth (Display*		,int			);
    int XDefaultDepthOfScreen (Screen*		);
    int XDefaultScreen (Display*		);
    int XDefineCursor (Display*		,Window		,Cursor		);
    int XDeleteProperty (Display*		,Window		,Atom		);
    int XDestroyWindow (Display*		,Window		);
    int XDestroySubwindows (Display*		,Window		);
    int XDoesBackingStore (Screen*		    );
    Bool XDoesSaveUnders (Screen*		);
    int XDisableAccessControl (Display*		);
    int XDisplayCells (Display*		,int			);
    int XDisplayHeight (Display*		,int			);
    int XDisplayHeightMM (Display*		,int			);
    int XDisplayKeycodes (Display*		,int*		,int*		);
    int XDisplayPlanes (Display*		,int			);
    int XDisplayWidth (Display*		,int			);
    int XDisplayWidthMM (Display*		,int			);
    int XDrawArc (Display*		,Drawable		,GC			,int			,int			,unsigned int	,unsigned int	,int			,int			);
    int XDrawArcs (Display*		,Drawable		,GC			,XArc*		,int			);
    int XDrawImageString (Display*		,Drawable		,GC			,int			,int			,_Xconst char*	,int			);
    int XDrawImageString16 (Display*		,Drawable		,GC			,int			,int			,_Xconst XChar2b*	,int			);
    int XDrawLine (Display*		,Drawable		,GC			,int			,int			,int			,int			);
    int XDrawLines (Display*		,Drawable		,GC			,XPoint*		,int			,int			);
    int XDrawPoint (Display*		,Drawable		,GC			,int			,int			);
    int XDrawPoints (Display*		,Drawable		,GC			,XPoint*		,int			,int			);
    int XDrawRectangle (Display*		,Drawable		,GC			,int			,int			,unsigned int	,unsigned int	);
    int XDrawRectangles (Display*		,Drawable		,GC			,XRectangle*		,int			);
    int XDrawSegments (Display*		,Drawable		,GC			,XSegment*		,int			);
    int XDrawString (Display*		,Drawable		,GC			,int			,int			,_Xconst char*	,int			);
    int XDrawString16 (Display*		,Drawable		,GC			,int			,int			,_Xconst XChar2b*	,int			);
    int XDrawText (Display*		,Drawable		,GC			,int			,int			,XTextItem*		,int			);
    int XDrawText16 (Display*		,Drawable		,GC			,int			,int			,XTextItem16*	,int			);
    int XEnableAccessControl (Display*		);
    int XEventsQueued (Display*		,int			);
    Status XFetchName (Display*		,Window		,char**		);
    int XFillArc (Display*		,Drawable		,GC			,int			,int			,unsigned int	,unsigned int	,int			,int			);
    int XFillArcs (Display*		,Drawable		,GC			,XArc*		,int			);
    int XFillPolygon (Display*		,Drawable		,GC			,XPoint*		,int			,int			,int			);
    int XFillRectangle (Display*		,Drawable		,GC			,int			,int			,unsigned int	,unsigned int	);
    int XFillRectangles (Display*		,Drawable		,GC			,XRectangle*		,int			);
    int XFlush (Display*		);
    int XForceScreenSaver (Display*		,int			);
    int XFree (void*		);
    int XFreeColormap (Display*		,Colormap		);
    int XFreeColors (Display*		,Colormap		,unsigned long*	,int			,unsigned long	);
    int XFreeCursor (Display*		,Cursor		);
    int XFreeExtensionList (char**		    );
    int XFreeFont (Display*		,XFontStruct*	);
    int XFreeFontInfo (char**		,XFontStruct*	,int			);
    int XFreeFontNames (char**		);
    int XFreeFontPath (char**		);
    int XFreeGC (Display*		,GC			);
    int XFreeModifiermap (XModifierKeymap*	);
    int XFreePixmap (Display*		,Pixmap		);
    int XGeometry (Display*		,int			,_Xconst char*	,_Xconst char*	,unsigned int	,unsigned int	,unsigned int	,int			,int			,int*		,int*		,int*		,int*		);
    int XGetErrorDatabaseText (Display*		,_Xconst char*	,_Xconst char*	,_Xconst char*	,char*		,int			);
    int XGetErrorText (Display*		,int			,char*		,int			);
    Bool XGetFontProperty (XFontStruct*	,Atom		,unsigned long*	);
    Status XGetGCValues (Display*		,GC			,unsigned long	,XGCValues*		);
    Status XGetGeometry (Display*		,Drawable		,Window*		,int*		,int*		,unsigned int*	,unsigned int*	,unsigned int*	,unsigned int*	);
    Status XGetIconName (Display*		,Window		,char**		);
    int XGetInputFocus (Display*		,Window*		,int*		);
    int XGetKeyboardControl (Display*		,XKeyboardState*	);
    int XGetPointerControl (Display*		,int*		,int*		,int*		);
    int XGetPointerMapping (Display*		,unsigned char*	,int			);
    int XGetScreenSaver (Display*		,int*		,int*		,int*		,int*		);
    Status XGetTransientForHint (Display*		,Window		,Window*		);
    int XGetWindowProperty (Display*		,Window		,Atom		,long		,long		,Bool		,Atom		,Atom*		,int*		,unsigned long*	,unsigned long*	,unsigned char**	);
    Status XGetWindowAttributes (Display*		,Window		,XWindowAttributes*	);
    int XGrabButton (Display*		,unsigned int	,unsigned int	,Window		,Bool		,unsigned int	,int			,int			,Window		,Cursor		);
    int XGrabKey (Display*		,int			,unsigned int	,Window		,Bool		,int			,int			);
    int XGrabKeyboard (Display*		,Window		,Bool		,int			,int			,Time		);
    int XGrabPointer (Display*		,Window		,Bool		,unsigned int	,int			,int			,Window		,Cursor		,Time		);
    int XGrabServer (Display*		);
    int XHeightMMOfScreen (Screen*		);
    int XHeightOfScreen (Screen*		);
    int XImageByteOrder (Display*		);
    int XInstallColormap (Display*		,Colormap		);
    KeyCode XKeysymToKeycode (Display*		,KeySym		);
    int XKillClient (Display*		,XID			);
    Status XLookupColor (Display*		,Colormap		,_Xconst char*	,XColor*		,XColor*		);
    int XLowerWindow (Display*		,Window		);
    int XMapRaised (Display*		,Window		);
    int XMapSubwindows (Display*		,Window		);
    int XMapWindow (Display*		,Window		);
    int XMaskEvent (Display*		,long		,XEvent*		);
    int XMaxCmapsOfScreen (Screen*		);
    int XMinCmapsOfScreen (Screen*		);
    int XMoveResizeWindow (Display*		,Window		,int			,int			,unsigned int	,unsigned int	);
    int XMoveWindow (Display*		,Window		,int			,int			);
    int XNextEvent (Display*		,XEvent*		);
    int XNoOp (Display*		);
    Status XParseColor (Display*		,Colormap		,_Xconst char*	,XColor*		);
    int XParseGeometry (_Xconst char*	,int*		,int*		,unsigned int*	,unsigned int*	);
    int XPeekEvent (Display*		,XEvent*		);
    int XPending (Display*		);
    int XPlanesOfScreen (Screen*		);
    int XProtocolRevision (Display*		);
    int XProtocolVersion (Display*		);
    int XPutBackEvent (Display*		,XEvent*		);
    int XPutImage (Display*		,Drawable		,GC			,XImage*		,int			,int			,int			,int			,unsigned int	,unsigned int		  );
    int XQLength (Display*		);
    Status XQueryBestCursor (Display*		,Drawable		,unsigned int        ,unsigned int	,unsigned int*	,unsigned int*	);
    Status XQueryBestSize (Display*		,int			,Drawable		,unsigned int	,unsigned int	,unsigned int*	,unsigned int*	);
    Status XQueryBestStipple (Display*		,Drawable		,unsigned int	,unsigned int	,unsigned int*	,unsigned int*	);
    Status XQueryBestTile (Display*		,Drawable		,unsigned int	,unsigned int	,unsigned int*	,unsigned int*	);
    int XQueryColor (Display*		,Colormap		,XColor*		);
    int XQueryColors (Display*		,Colormap		,XColor*		,int			);
    Bool XQueryExtension (Display*		,_Xconst char*	,int*		,int*		,int*		);
    int XQueryKeymap (Display*		,char [32]		);
    Bool XQueryPointer (Display*		,Window		,Window*		,Window*		,int*		,int*		,int*		,int*		,unsigned int*       );
    int XQueryTextExtents (Display*		,XID			,_Xconst char*	,int			,int*		,int*		,int*		,XCharStruct*	    );
    int XQueryTextExtents16 (Display*		,XID			,_Xconst XChar2b*	,int			,int*		,int*		,int*		,XCharStruct*	);
    Status XQueryTree (Display*		,Window		,Window*		,Window*		,Window**		,unsigned int*	);
    int XRaiseWindow (Display*		,Window		);
    int XReadBitmapFile (Display*		,Drawable 		,_Xconst char*	,unsigned int*	,unsigned int*	,Pixmap*		,int*		,int*		);
    int XReadBitmapFileData (_Xconst char*	,unsigned int*	,unsigned int*	,unsigned char**	,int*		,int*		);
    int XRebindKeysym (Display*		,KeySym		,KeySym*		,int			,_Xconst unsigned char*	,int			);
    int XRecolorCursor (Display*		,Cursor		,XColor*		,XColor*		);
    int XRefreshKeyboardMapping (XMappingEvent*	    );
    int XRemoveFromSaveSet (Display*		,Window		);
    int XRemoveHost (Display*		,XHostAddress*	);
    int XRemoveHosts (Display*		,XHostAddress*	,int			);
    int XReparentWindow (Display*		,Window		,Window		,int			,int			);
    int XResetScreenSaver (Display*		);
    int XResizeWindow (Display*		,Window		,unsigned int	,unsigned int	);
    int XRestackWindows (Display*		,Window*		,int			);
    int XRotateBuffers (Display*		,int			);
    int XRotateWindowProperties (Display*		,Window		,Atom*		,int			,int			);
    int XScreenCount (Display*		);
    int XSelectInput (Display*		,Window		,long		);
    Status XSendEvent (Display*		,Window		,Bool		,long		,XEvent*		);
    int XSetAccessControl (Display*		,int			);
    int XSetArcMode (Display*		,GC			,int			);
    int XSetBackground (Display*		,GC			,unsigned long	);
    int XSetClipMask (Display*		,GC			,Pixmap		);
    int XSetClipOrigin (Display*		,GC			,int			,int			);
    int XSetClipRectangles (Display*		,GC			,int			,int			,XRectangle*		,int			,int			);
    int XSetCloseDownMode (Display*		,int			);
    int XSetCommand (Display*		,Window		,char**		,int			);
    int XSetDashes (Display*		,GC			,int			,_Xconst char*	,int			);
    int XSetFillRule (Display*		,GC			,int			);
    int XSetFillStyle (Display*		,GC			,int			);
    int XSetFont (Display*		,GC			,Font		);
    int XSetFontPath (Display*		,char**		,int				     );
    int XSetForeground (Display*		,GC			,unsigned long	);
    int XSetFunction (Display*		,GC			,int			);
    int XSetGraphicsExposures (Display*		,GC			,Bool		);
    int XSetIconName (Display*		,Window		,_Xconst char*	);
    int XSetInputFocus (Display*		,Window		,int			,Time		);
    int XSetLineAttributes (Display*		,GC			,unsigned int	,int			,int			,int			);
    int XSetModifierMapping (Display*		,XModifierKeymap*	);
    int XSetPlaneMask (Display*		,GC			,unsigned long	);
    int XSetPointerMapping (Display*		,_Xconst unsigned char*	,int			);
    int XSetScreenSaver (Display*		,int			,int			,int			,int			);
    int XSetSelectionOwner (Display*		,Atom	        ,Window		,Time		);
    int XSetState (Display*		,GC			,unsigned long 	,unsigned long	,int			,unsigned long	);
    int XSetStipple (Display*		,GC			,Pixmap		);
    int XSetSubwindowMode (Display*		,GC			,int			);
    int XSetTSOrigin (Display*		,GC			,int			,int			);
    int XSetTile (Display*		,GC			,Pixmap		);
    int XSetWindowBackground (Display*		,Window		,unsigned long	);
    int XSetWindowBackgroundPixmap (Display*		,Window		,Pixmap		);
    int XSetWindowBorder (Display*		,Window		,unsigned long	);
    int XSetWindowBorderPixmap (Display*		,Window		,Pixmap		);
    int XSetWindowBorderWidth (Display*		,Window		,unsigned int	);
    int XSetWindowColormap (Display*		,Window		,Colormap		);
    int XStoreBuffer (Display*		,_Xconst char*	,int			,int			);
    int XStoreBytes (Display*		,_Xconst char*	,int			);
    int XStoreColor (Display*		,Colormap		,XColor*		);
    int XStoreColors (Display*		,Colormap		,XColor*		,int			);
    int XStoreName (Display*		,Window		,_Xconst char*	);
    int XStoreNamedColor (Display*		,Colormap		,_Xconst char*	,unsigned long	,int			);
    int XSync (Display*		,Bool		);
    int XTextExtents (XFontStruct*	,_Xconst char*	,int			,int*		,int*		,int*		,XCharStruct*	);
    int XTextExtents16 (XFontStruct*	,_Xconst XChar2b*	,int			,int*		,int*		,int*		,XCharStruct*	);
    int XTextWidth (XFontStruct*	,_Xconst char*	,int			);
    int XTextWidth16 (XFontStruct*	,_Xconst XChar2b*	,int			);
    Bool XTranslateCoordinates (Display*		,Window		,Window		,int			,int			,int*		,int*		,Window*		);
    int XUndefineCursor (Display*		,Window		);
    int XUngrabButton (Display*		,unsigned int	,unsigned int	,Window		);
    int XUngrabKey (Display*		,int			,unsigned int	,Window		);
    int XUngrabKeyboard (Display*		,Time		);
    int XUngrabPointer (Display*		,Time		);
    int XUngrabServer (Display*		);
    int XUninstallColormap (Display*		,Colormap		);
    int XUnloadFont (Display*		,Font		);
    int XUnmapSubwindows (Display*		,Window		);
    int XUnmapWindow (Display*		,Window		);
    int XVendorRelease (Display*		);
    int XWarpPointer (Display*		,Window		,Window		,int			,int			,unsigned int	,unsigned int	,int			,int				     );
    int XWidthMMOfScreen (Screen*		);
    int XWidthOfScreen (Screen*		);
    int XWindowEvent (Display*		,Window		,long		,XEvent*		);
    int XWriteBitmapFile (Display*		,_Xconst char*	,Pixmap		,unsigned int	,unsigned int	,int			,int					     );
    Bool XSupportsLocale (void);
    char * XSetLocaleModifiers (const char*		);
    XOM XOpenOM (Display*			,struct _XrmHashBucketRec*	,_Xconst char*		,_Xconst char*		);
    Status XCloseOM (XOM			);
    Display * XDisplayOfOM (XOM			);
    char * XLocaleOfOM (XOM			);
    void XDestroyOC (XOC			);
    XOM XOMOfOC (XOC			);
    XFontSet XCreateFontSet (Display*		,_Xconst char*	,char***		,int*		,char**		);
    void XFreeFontSet (Display*		,XFontSet		);
    int XFontsOfFontSet (XFontSet		,XFontStruct***	,char***		);
    char * XBaseFontNameListOfFontSet (XFontSet		);
    char * XLocaleOfFontSet (XFontSet		);
    Bool XContextDependentDrawing (XFontSet		);
    Bool XDirectionalDependentDrawing (XFontSet		);
    Bool XContextualDrawing (XFontSet		);
    XFontSetExtents * XExtentsOfFontSet (XFontSet		);
    int XmbTextEscapement (XFontSet		,_Xconst char*	,int			);
    int XwcTextEscapement (XFontSet		,_Xconst wchar_t*	,int			);
    int Xutf8TextEscapement (XFontSet		,_Xconst char*	,int			);
    int XmbTextExtents (XFontSet		,_Xconst char*	,int			,XRectangle*		,XRectangle*		);
    int XwcTextExtents (XFontSet		,_Xconst wchar_t*	,int			,XRectangle*		,XRectangle*		);
    int Xutf8TextExtents (XFontSet		,_Xconst char*	,int			,XRectangle*		,XRectangle*		);
    Status XmbTextPerCharExtents (XFontSet		,_Xconst char*	,int			,XRectangle*		,XRectangle*		,int			,int*		,XRectangle*		,XRectangle*		);
    Status XwcTextPerCharExtents (XFontSet		,_Xconst wchar_t*	,int			,XRectangle*		,XRectangle*		,int			,int*		,XRectangle*		,XRectangle*		);
    Status Xutf8TextPerCharExtents (XFontSet		,_Xconst char*	,int			,XRectangle*		,XRectangle*		,int			,int*		,XRectangle*		,XRectangle*		);
    void XmbDrawText (Display*		,Drawable		,GC			,int			,int			,XmbTextItem*	,int			);
    void XwcDrawText (Display*		,Drawable		,GC			,int			,int			,XwcTextItem*	,int			);
    void Xutf8DrawText (Display*		,Drawable		,GC			,int			,int			,XmbTextItem*	,int			);
    void XmbDrawString (Display*		,Drawable		,XFontSet		,GC			,int			,int			,_Xconst char*	,int			);
    void XwcDrawString (Display*		,Drawable		,XFontSet		,GC			,int			,int			,_Xconst wchar_t*	,int			);
    void Xutf8DrawString (Display*		,Drawable		,XFontSet		,GC			,int			,int			,_Xconst char*	,int			);
    void XmbDrawImageString (Display*		,Drawable		,XFontSet		,GC			,int			,int			,_Xconst char*	,int			);
    void XwcDrawImageString (Display*		,Drawable		,XFontSet		,GC			,int			,int			,_Xconst wchar_t*	,int			);
    void Xutf8DrawImageString (Display*		,Drawable		,XFontSet		,GC			,int			,int			,_Xconst char*	,int			);
    XIM XOpenIM (Display*			,struct _XrmHashBucketRec*	,char*			,char*			);
    Status XCloseIM (XIM );
    Display * XDisplayOfIM (XIM );
    char * XLocaleOfIM (XIM );
    void XDestroyIC (XIC );
    void XSetICFocus (XIC );
    void XUnsetICFocus (XIC );
    wchar_t * XwcResetIC (XIC );
    char * XmbResetIC (XIC );
    char * Xutf8ResetIC (XIC );
    XIM XIMOfIC (XIC );
    Bool XFilterEvent (XEvent*	,Window	);
    int XmbLookupString (XIC			,XKeyPressedEvent*	,char*		,int			,KeySym*		,Status*		);
    int XwcLookupString (XIC			,XKeyPressedEvent*	,wchar_t*		,int			,KeySym*		,Status*		);
    int Xutf8LookupString (XIC			,XKeyPressedEvent*	,char*		,int			,KeySym*		,Status*		);
    Bool XRegisterIMInstantiateCallback (Display*			,struct _XrmHashBucketRec*	,char*			,char*			,XIDProc			,XPointer			);
    Bool XUnregisterIMInstantiateCallback (Display*			,struct _XrmHashBucketRec*	,char*			,char*			,XIDProc			,XPointer			);
    Status XInternalConnectionNumbers (Display*			,int**			,int*			);
    void XProcessInternalConnection (Display*			,int				);
    Status XAddConnectionWatch (Display*			,XConnectionWatchProc	,XPointer			);
    void XRemoveConnectionWatch (Display*			,XConnectionWatchProc	,XPointer			);
    void XSetAuthorization (char *			,int				, char *			,int				);
    int _Xwctomb (char *			,wchar_t			);
    int XDestroyImage (XImage *ximage);
    unsigned long XGetPixel (XImage *ximage,int x, int y);
    int XPutPixel (XImage *ximage,int x, int y,unsigned long pixel);
    XImage * XSubImage (XImage *ximage,int x, int y,unsigned int width, unsigned int height);
    int XAddPixel (XImage *ximage,long value);
    XClassHint * XAllocClassHint (void);
    XIconSize * XAllocIconSize (void);
    XSizeHints * XAllocSizeHints (void);
    XStandardColormap * XAllocStandardColormap (void);
    XWMHints * XAllocWMHints (void);
    int XClipBox (Region		,XRectangle*		);
    Region XCreateRegion (void);
    const char * XDefaultString (void);
    int XDeleteContext (Display*		,XID			,XContext		);
    int XDestroyRegion (Region		);
    int XEmptyRegion (Region		);
    int XEqualRegion (Region		,Region		);
    int XFindContext (Display*		,XID			,XContext		,XPointer*		);
    Status XGetClassHint (Display*		,Window		,XClassHint*		);
    Status XGetIconSizes (Display*		,Window		,XIconSize**		,int*		);
    Status XGetNormalHints (Display*		,Window		,XSizeHints*		);
    Status XGetRGBColormaps (Display*		,Window		,XStandardColormap** ,int*		,Atom		);
    Status XGetSizeHints (Display*		,Window		,XSizeHints*		,Atom		);
    Status XGetStandardColormap (Display*		,Window		,XStandardColormap*	,Atom					    );
    Status XGetTextProperty (Display*		,Window		,XTextProperty*	,Atom		);
    XVisualInfo * XGetVisualInfo (Display*		,long		,XVisualInfo*	,int*		);
    Status XGetWMClientMachine (Display*		,Window		,XTextProperty*	);
    XWMHints * XGetWMHints (Display*		,Window				      );
    Status XGetWMIconName (Display*		,Window		,XTextProperty*	);
    Status XGetWMName (Display*		,Window		,XTextProperty*	);
    Status XGetWMNormalHints (Display*		,Window		,XSizeHints*		,long*		 );
    Status XGetWMSizeHints (Display*		,Window		,XSizeHints*		,long*		,Atom		);
    Status XGetZoomHints (Display*		,Window		,XSizeHints*		);
    int XIntersectRegion (Region		,Region		,Region		);
    void XConvertCase (KeySym		,KeySym*		,KeySym*		);
    int XLookupString (XKeyEvent*		,char*		,int			,KeySym*		,XComposeStatus*	);
    Status XMatchVisualInfo (Display*		,int			,int			,int			,XVisualInfo*	);
    int XOffsetRegion (Region		,int			,int			);
    Bool XPointInRegion (Region		,int			,int			);
    Region XPolygonRegion (XPoint*		,int			,int			);
    int XRectInRegion (Region		,int			,int			,unsigned int	,unsigned int	);
    int XSaveContext (Display*		,XID			,XContext		,_Xconst char*	);
    int XSetClassHint (Display*		,Window		,XClassHint*		);
    int XSetIconSizes (Display*		,Window		,XIconSize*		,int			    );
    int XSetNormalHints (Display*		,Window		,XSizeHints*		);
    void XSetRGBColormaps (Display*		,Window		,XStandardColormap*	,int			,Atom		);
    int XSetSizeHints (Display*		,Window		,XSizeHints*		,Atom		);
    int XSetStandardProperties (Display*		,Window		,_Xconst char*	,_Xconst char*	,Pixmap		,char**		,int			,XSizeHints*		);
    void XSetTextProperty (Display*		,Window		,XTextProperty*	,Atom		);
    void XSetWMClientMachine (Display*		,Window		,XTextProperty*	);
    int XSetWMHints (Display*		,Window		,XWMHints*		);
    void XSetWMIconName (Display*		,Window		,XTextProperty*	);
    void XSetWMName (Display*		,Window		,XTextProperty*	);
    void XSetWMNormalHints (Display*		,Window		,XSizeHints*		);
    void XSetWMProperties (Display*		,Window		,XTextProperty*	,XTextProperty*	,char**		,int			,XSizeHints*		,XWMHints*		,XClassHint*		);
    void XmbSetWMProperties (Display*		,Window		,_Xconst char*	,_Xconst char*	,char**		,int			,XSizeHints*		,XWMHints*		,XClassHint*		);
    void Xutf8SetWMProperties (Display*		,Window		,_Xconst char*	,_Xconst char*	,char**		,int			,XSizeHints*		,XWMHints*		,XClassHint*		);
    void XSetWMSizeHints (Display*		,Window		,XSizeHints*		,Atom		);
    int XSetRegion (Display*		,GC			,Region		);
    void XSetStandardColormap (Display*		,Window		,XStandardColormap*	,Atom		);
    int XSetZoomHints (Display*		,Window		,XSizeHints*		);
    int XShrinkRegion (Region		,int			,int			);
    Status XStringListToTextProperty (char**		,int			,XTextProperty*	);
    int XSubtractRegion (Region		,Region		,Region		);
    int XmbTextListToTextProperty (Display*		display,char**		list,int			count,XICCEncodingStyle	style,XTextProperty*	text_prop_return);
    int XwcTextListToTextProperty (Display*		display,wchar_t**		list,int			count,XICCEncodingStyle	style,XTextProperty*	text_prop_return);
    int Xutf8TextListToTextProperty (Display*		display,char**		list,int			count,XICCEncodingStyle	style,XTextProperty*	text_prop_return);
    void XwcFreeStringList (wchar_t**		list);
    Status XTextPropertyToStringList (XTextProperty*	,char***		,int*		);
    int XmbTextPropertyToTextList (Display*		display,const XTextProperty* text_prop,char***		list_return,int*		count_return);
    int XwcTextPropertyToTextList (Display*		display,const XTextProperty* text_prop,wchar_t***		list_return,int*		count_return);
    int Xutf8TextPropertyToTextList (Display*		display,const XTextProperty* text_prop,char***		list_return,int*		count_return);
    int XUnionRectWithRegion (XRectangle*		,Region		,Region		);
    int XUnionRegion (Region		,Region		,Region		);
    int XWMGeometry (Display*		,int			,_Xconst char*	,_Xconst char*	,unsigned int	,XSizeHints*		,int*		,int*		,int*		,int*		,int*		);
    int XXorRegion (Region		,Region		,Region		);
    void* glXGetProcAddressARB (const GLubyte*procName);
}

#ifndef NO_PORTAL
    #define glAccum Portal::glAccum
    #define glAlphaFunc Portal::glAlphaFunc
    #define glAreTexturesResident Portal::glAreTexturesResident
    #define glArrayElement Portal::glArrayElement
    #define glBegin Portal::glBegin
    #define glBindTexture Portal::glBindTexture
    #define glBitmap Portal::glBitmap
    #define glBlendFunc Portal::glBlendFunc
    #define glCallList Portal::glCallList
    #define glCallLists Portal::glCallLists
    #define glClear Portal::glClear
    #define glClearAccum Portal::glClearAccum
    #define glClearColor Portal::glClearColor
    #define glClearDepth Portal::glClearDepth
    #define glClearIndex Portal::glClearIndex
    #define glClearStencil Portal::glClearStencil
    #define glClipPlane Portal::glClipPlane
    #define glColor3b Portal::glColor3b
    #define glColor3bv Portal::glColor3bv
    #define glColor3d Portal::glColor3d
    #define glColor3dv Portal::glColor3dv
    #define glColor3f Portal::glColor3f
    #define glColor3fv Portal::glColor3fv
    #define glColor3i Portal::glColor3i
    #define glColor3iv Portal::glColor3iv
    #define glColor3s Portal::glColor3s
    #define glColor3sv Portal::glColor3sv
    #define glColor3ub Portal::glColor3ub
    #define glColor3ubv Portal::glColor3ubv
    #define glColor3ui Portal::glColor3ui
    #define glColor3uiv Portal::glColor3uiv
    #define glColor3us Portal::glColor3us
    #define glColor3usv Portal::glColor3usv
    #define glColor4b Portal::glColor4b
    #define glColor4bv Portal::glColor4bv
    #define glColor4d Portal::glColor4d
    #define glColor4dv Portal::glColor4dv
    #define glColor4f Portal::glColor4f
    #define glColor4fv Portal::glColor4fv
    #define glColor4i Portal::glColor4i
    #define glColor4iv Portal::glColor4iv
    #define glColor4s Portal::glColor4s
    #define glColor4sv Portal::glColor4sv
    #define glColor4ub Portal::glColor4ub
    #define glColor4ubv Portal::glColor4ubv
    #define glColor4ui Portal::glColor4ui
    #define glColor4uiv Portal::glColor4uiv
    #define glColor4us Portal::glColor4us
    #define glColor4usv Portal::glColor4usv
    #define glColorMask Portal::glColorMask
    #define glColorMaterial Portal::glColorMaterial
    #define glColorPointer Portal::glColorPointer
    #define glCopyPixels Portal::glCopyPixels
    #define glCopyTexImage1D Portal::glCopyTexImage1D
    #define glCopyTexImage2D Portal::glCopyTexImage2D
    #define glCopyTexSubImage1D Portal::glCopyTexSubImage1D
    #define glCopyTexSubImage2D Portal::glCopyTexSubImage2D
    #define glCullFace Portal::glCullFace
    #define glDeleteLists Portal::glDeleteLists
    #define glDeleteTextures Portal::glDeleteTextures
    #define glDepthFunc Portal::glDepthFunc
    #define glDepthMask Portal::glDepthMask
    #define glDepthRange Portal::glDepthRange
    #define glDisable Portal::glDisable
    #define glDisableClientState Portal::glDisableClientState
    #define glDrawArrays Portal::glDrawArrays
    #define glDrawBuffer Portal::glDrawBuffer
    #define glDrawElements Portal::glDrawElements
    #define glDrawPixels Portal::glDrawPixels
    #define glEdgeFlag Portal::glEdgeFlag
    #define glEdgeFlagPointer Portal::glEdgeFlagPointer
    #define glEdgeFlagv Portal::glEdgeFlagv
    #define glEnable Portal::glEnable
    #define glEnableClientState Portal::glEnableClientState
    #define glEnd Portal::glEnd
    #define glEndList Portal::glEndList
    #define glEvalCoord1d Portal::glEvalCoord1d
    #define glEvalCoord1dv Portal::glEvalCoord1dv
    #define glEvalCoord1f Portal::glEvalCoord1f
    #define glEvalCoord1fv Portal::glEvalCoord1fv
    #define glEvalCoord2d Portal::glEvalCoord2d
    #define glEvalCoord2dv Portal::glEvalCoord2dv
    #define glEvalCoord2f Portal::glEvalCoord2f
    #define glEvalCoord2fv Portal::glEvalCoord2fv
    #define glEvalMesh1 Portal::glEvalMesh1
    #define glEvalMesh2 Portal::glEvalMesh2
    #define glEvalPoint1 Portal::glEvalPoint1
    #define glEvalPoint2 Portal::glEvalPoint2
    #define glFeedbackBuffer Portal::glFeedbackBuffer
    #define glFinish Portal::glFinish
    #define glFlush Portal::glFlush
    #define glFogf Portal::glFogf
    #define glFogfv Portal::glFogfv
    #define glFogi Portal::glFogi
    #define glFogiv Portal::glFogiv
    #define glFrontFace Portal::glFrontFace
    #define glFrustum Portal::glFrustum
    #define glGenLists Portal::glGenLists
    #define glGenTextures Portal::glGenTextures
    #define glGetBooleanv Portal::glGetBooleanv
    #define glGetClipPlane Portal::glGetClipPlane
    #define glGetDoublev Portal::glGetDoublev
    #define glGetError Portal::glGetError
    #define glGetFloatv Portal::glGetFloatv
    #define glGetIntegerv Portal::glGetIntegerv
    #define glGetLightfv Portal::glGetLightfv
    #define glGetLightiv Portal::glGetLightiv
    #define glGetMapdv Portal::glGetMapdv
    #define glGetMapfv Portal::glGetMapfv
    #define glGetMapiv Portal::glGetMapiv
    #define glGetMaterialfv Portal::glGetMaterialfv
    #define glGetMaterialiv Portal::glGetMaterialiv
    #define glGetPixelMapfv Portal::glGetPixelMapfv
    #define glGetPixelMapuiv Portal::glGetPixelMapuiv
    #define glGetPixelMapusv Portal::glGetPixelMapusv
    #define glGetPointerv Portal::glGetPointerv
    #define glGetPolygonStipple Portal::glGetPolygonStipple
    #define glGetString Portal::glGetString
    #define glGetTexEnvfv Portal::glGetTexEnvfv
    #define glGetTexEnviv Portal::glGetTexEnviv
    #define glGetTexGendv Portal::glGetTexGendv
    #define glGetTexGenfv Portal::glGetTexGenfv
    #define glGetTexGeniv Portal::glGetTexGeniv
    #define glGetTexImage Portal::glGetTexImage
    #define glGetTexLevelParameterfv Portal::glGetTexLevelParameterfv
    #define glGetTexLevelParameteriv Portal::glGetTexLevelParameteriv
    #define glGetTexParameterfv Portal::glGetTexParameterfv
    #define glGetTexParameteriv Portal::glGetTexParameteriv
    #define glHint Portal::glHint
    #define glIndexMask Portal::glIndexMask
    #define glIndexPointer Portal::glIndexPointer
    #define glIndexd Portal::glIndexd
    #define glIndexdv Portal::glIndexdv
    #define glIndexf Portal::glIndexf
    #define glIndexfv Portal::glIndexfv
    #define glIndexi Portal::glIndexi
    #define glIndexiv Portal::glIndexiv
    #define glIndexs Portal::glIndexs
    #define glIndexsv Portal::glIndexsv
    #define glIndexub Portal::glIndexub
    #define glIndexubv Portal::glIndexubv
    #define glInitNames Portal::glInitNames
    #define glInterleavedArrays Portal::glInterleavedArrays
    #define glIsEnabled Portal::glIsEnabled
    #define glIsList Portal::glIsList
    #define glIsTexture Portal::glIsTexture
    #define glLightModelf Portal::glLightModelf
    #define glLightModelfv Portal::glLightModelfv
    #define glLightModeli Portal::glLightModeli
    #define glLightModeliv Portal::glLightModeliv
    #define glLightf Portal::glLightf
    #define glLightfv Portal::glLightfv
    #define glLighti Portal::glLighti
    #define glLightiv Portal::glLightiv
    #define glLineStipple Portal::glLineStipple
    #define glLineWidth Portal::glLineWidth
    #define glListBase Portal::glListBase
    #define glLoadIdentity Portal::glLoadIdentity
    #define glLoadMatrixd Portal::glLoadMatrixd
    #define glLoadMatrixf Portal::glLoadMatrixf
    #define glLoadName Portal::glLoadName
    #define glLogicOp Portal::glLogicOp
    #define glMap1d Portal::glMap1d
    #define glMap1f Portal::glMap1f
    #define glMap2d Portal::glMap2d
    #define glMap2f Portal::glMap2f
    #define glMapGrid1d Portal::glMapGrid1d
    #define glMapGrid1f Portal::glMapGrid1f
    #define glMapGrid2d Portal::glMapGrid2d
    #define glMapGrid2f Portal::glMapGrid2f
    #define glMaterialf Portal::glMaterialf
    #define glMaterialfv Portal::glMaterialfv
    #define glMateriali Portal::glMateriali
    #define glMaterialiv Portal::glMaterialiv
    #define glMatrixMode Portal::glMatrixMode
    #define glMultMatrixd Portal::glMultMatrixd
    #define glMultMatrixf Portal::glMultMatrixf
    #define glNewList Portal::glNewList
    #define glNormal3b Portal::glNormal3b
    #define glNormal3bv Portal::glNormal3bv
    #define glNormal3d Portal::glNormal3d
    #define glNormal3dv Portal::glNormal3dv
    #define glNormal3f Portal::glNormal3f
    #define glNormal3fv Portal::glNormal3fv
    #define glNormal3i Portal::glNormal3i
    #define glNormal3iv Portal::glNormal3iv
    #define glNormal3s Portal::glNormal3s
    #define glNormal3sv Portal::glNormal3sv
    #define glNormalPointer Portal::glNormalPointer
    #define glOrtho Portal::glOrtho
    #define glPassThrough Portal::glPassThrough
    #define glPixelMapfv Portal::glPixelMapfv
    #define glPixelMapuiv Portal::glPixelMapuiv
    #define glPixelMapusv Portal::glPixelMapusv
    #define glPixelStoref Portal::glPixelStoref
    #define glPixelStorei Portal::glPixelStorei
    #define glPixelTransferf Portal::glPixelTransferf
    #define glPixelTransferi Portal::glPixelTransferi
    #define glPixelZoom Portal::glPixelZoom
    #define glPointSize Portal::glPointSize
    #define glPolygonMode Portal::glPolygonMode
    #define glPolygonOffset Portal::glPolygonOffset
    #define glPolygonStipple Portal::glPolygonStipple
    #define glPopAttrib Portal::glPopAttrib
    #define glPopClientAttrib Portal::glPopClientAttrib
    #define glPopMatrix Portal::glPopMatrix
    #define glPopName Portal::glPopName
    #define glPrioritizeTextures Portal::glPrioritizeTextures
    #define glPushAttrib Portal::glPushAttrib
    #define glPushClientAttrib Portal::glPushClientAttrib
    #define glPushMatrix Portal::glPushMatrix
    #define glPushName Portal::glPushName
    #define glRasterPos2d Portal::glRasterPos2d
    #define glRasterPos2dv Portal::glRasterPos2dv
    #define glRasterPos2f Portal::glRasterPos2f
    #define glRasterPos2fv Portal::glRasterPos2fv
    #define glRasterPos2i Portal::glRasterPos2i
    #define glRasterPos2iv Portal::glRasterPos2iv
    #define glRasterPos2s Portal::glRasterPos2s
    #define glRasterPos2sv Portal::glRasterPos2sv
    #define glRasterPos3d Portal::glRasterPos3d
    #define glRasterPos3dv Portal::glRasterPos3dv
    #define glRasterPos3f Portal::glRasterPos3f
    #define glRasterPos3fv Portal::glRasterPos3fv
    #define glRasterPos3i Portal::glRasterPos3i
    #define glRasterPos3iv Portal::glRasterPos3iv
    #define glRasterPos3s Portal::glRasterPos3s
    #define glRasterPos3sv Portal::glRasterPos3sv
    #define glRasterPos4d Portal::glRasterPos4d
    #define glRasterPos4dv Portal::glRasterPos4dv
    #define glRasterPos4f Portal::glRasterPos4f
    #define glRasterPos4fv Portal::glRasterPos4fv
    #define glRasterPos4i Portal::glRasterPos4i
    #define glRasterPos4iv Portal::glRasterPos4iv
    #define glRasterPos4s Portal::glRasterPos4s
    #define glRasterPos4sv Portal::glRasterPos4sv
    #define glReadBuffer Portal::glReadBuffer
    #define glReadPixels Portal::glReadPixels
    #define glRectd Portal::glRectd
    #define glRectdv Portal::glRectdv
    #define glRectf Portal::glRectf
    #define glRectfv Portal::glRectfv
    #define glRecti Portal::glRecti
    #define glRectiv Portal::glRectiv
    #define glRects Portal::glRects
    #define glRectsv Portal::glRectsv
    #define glRenderMode Portal::glRenderMode
    #define glRotated Portal::glRotated
    #define glRotatef Portal::glRotatef
    #define glScaled Portal::glScaled
    #define glScalef Portal::glScalef
    #define glScissor Portal::glScissor
    #define glSelectBuffer Portal::glSelectBuffer
    #define glShadeModel Portal::glShadeModel
    #define glStencilFunc Portal::glStencilFunc
    #define glStencilMask Portal::glStencilMask
    #define glStencilOp Portal::glStencilOp
    #define glTexCoord1d Portal::glTexCoord1d
    #define glTexCoord1dv Portal::glTexCoord1dv
    #define glTexCoord1f Portal::glTexCoord1f
    #define glTexCoord1fv Portal::glTexCoord1fv
    #define glTexCoord1i Portal::glTexCoord1i
    #define glTexCoord1iv Portal::glTexCoord1iv
    #define glTexCoord1s Portal::glTexCoord1s
    #define glTexCoord1sv Portal::glTexCoord1sv
    #define glTexCoord2d Portal::glTexCoord2d
    #define glTexCoord2dv Portal::glTexCoord2dv
    #define glTexCoord2f Portal::glTexCoord2f
    #define glTexCoord2fv Portal::glTexCoord2fv
    #define glTexCoord2i Portal::glTexCoord2i
    #define glTexCoord2iv Portal::glTexCoord2iv
    #define glTexCoord2s Portal::glTexCoord2s
    #define glTexCoord2sv Portal::glTexCoord2sv
    #define glTexCoord3d Portal::glTexCoord3d
    #define glTexCoord3dv Portal::glTexCoord3dv
    #define glTexCoord3f Portal::glTexCoord3f
    #define glTexCoord3fv Portal::glTexCoord3fv
    #define glTexCoord3i Portal::glTexCoord3i
    #define glTexCoord3iv Portal::glTexCoord3iv
    #define glTexCoord3s Portal::glTexCoord3s
    #define glTexCoord3sv Portal::glTexCoord3sv
    #define glTexCoord4d Portal::glTexCoord4d
    #define glTexCoord4dv Portal::glTexCoord4dv
    #define glTexCoord4f Portal::glTexCoord4f
    #define glTexCoord4fv Portal::glTexCoord4fv
    #define glTexCoord4i Portal::glTexCoord4i
    #define glTexCoord4iv Portal::glTexCoord4iv
    #define glTexCoord4s Portal::glTexCoord4s
    #define glTexCoord4sv Portal::glTexCoord4sv
    #define glTexCoordPointer Portal::glTexCoordPointer
    #define glTexEnvf Portal::glTexEnvf
    #define glTexEnvfv Portal::glTexEnvfv
    #define glTexEnvi Portal::glTexEnvi
    #define glTexEnviv Portal::glTexEnviv
    #define glTexGend Portal::glTexGend
    #define glTexGendv Portal::glTexGendv
    #define glTexGenf Portal::glTexGenf
    #define glTexGenfv Portal::glTexGenfv
    #define glTexGeni Portal::glTexGeni
    #define glTexGeniv Portal::glTexGeniv
    #define glTexImage1D Portal::glTexImage1D
    #define glTexImage2D Portal::glTexImage2D
    #define glTexParameterf Portal::glTexParameterf
    #define glTexParameterfv Portal::glTexParameterfv
    #define glTexParameteri Portal::glTexParameteri
    #define glTexParameteriv Portal::glTexParameteriv
    #define glTexSubImage1D Portal::glTexSubImage1D
    #define glTexSubImage2D Portal::glTexSubImage2D
    #define glTranslated Portal::glTranslated
    #define glTranslatef Portal::glTranslatef
    #define glVertex2d Portal::glVertex2d
    #define glVertex2dv Portal::glVertex2dv
    #define glVertex2f Portal::glVertex2f
    #define glVertex2fv Portal::glVertex2fv
    #define glVertex2i Portal::glVertex2i
    #define glVertex2iv Portal::glVertex2iv
    #define glVertex2s Portal::glVertex2s
    #define glVertex2sv Portal::glVertex2sv
    #define glVertex3d Portal::glVertex3d
    #define glVertex3dv Portal::glVertex3dv
    #define glVertex3f Portal::glVertex3f
    #define glVertex3fv Portal::glVertex3fv
    #define glVertex3i Portal::glVertex3i
    #define glVertex3iv Portal::glVertex3iv
    #define glVertex3s Portal::glVertex3s
    #define glVertex3sv Portal::glVertex3sv
    #define glVertex4d Portal::glVertex4d
    #define glVertex4dv Portal::glVertex4dv
    #define glVertex4f Portal::glVertex4f
    #define glVertex4fv Portal::glVertex4fv
    #define glVertex4i Portal::glVertex4i
    #define glVertex4iv Portal::glVertex4iv
    #define glVertex4s Portal::glVertex4s
    #define glVertex4sv Portal::glVertex4sv
    #define glVertexPointer Portal::glVertexPointer
    #define glViewport Portal::glViewport
    #define gluBeginCurve Portal::gluBeginCurve
    #define gluBeginPolygon Portal::gluBeginPolygon
    #define gluBeginSurface Portal::gluBeginSurface
    #define gluBeginTrim Portal::gluBeginTrim
    #define gluBuild1DMipmaps Portal::gluBuild1DMipmaps
    #define gluBuild2DMipmaps Portal::gluBuild2DMipmaps
    #define gluCylinder Portal::gluCylinder
    #define gluDeleteNurbsRenderer Portal::gluDeleteNurbsRenderer
    #define gluDeleteQuadric Portal::gluDeleteQuadric
    #define gluDeleteTess Portal::gluDeleteTess
    #define gluDisk Portal::gluDisk
    #define gluEndCurve Portal::gluEndCurve
    #define gluEndPolygon Portal::gluEndPolygon
    #define gluEndSurface Portal::gluEndSurface
    #define gluEndTrim Portal::gluEndTrim
    #define gluErrorString Portal::gluErrorString
    #define gluGetNurbsProperty Portal::gluGetNurbsProperty
    #define gluGetString Portal::gluGetString
    #define gluGetTessProperty Portal::gluGetTessProperty
    #define gluLoadSamplingMatrices Portal::gluLoadSamplingMatrices
    #define gluLookAt Portal::gluLookAt
    #define gluNewNurbsRenderer Portal::gluNewNurbsRenderer
    #define gluNewQuadric Portal::gluNewQuadric
    #define gluNewTess Portal::gluNewTess
    #define gluNextContour Portal::gluNextContour
    #define gluNurbsCallback Portal::gluNurbsCallback
    #define gluNurbsCurve Portal::gluNurbsCurve
    #define gluNurbsProperty Portal::gluNurbsProperty
    #define gluNurbsSurface Portal::gluNurbsSurface
    #define gluOrtho2D Portal::gluOrtho2D
    #define gluPartialDisk Portal::gluPartialDisk
    #define gluPerspective Portal::gluPerspective
    #define gluPickMatrix Portal::gluPickMatrix
    #define gluProject Portal::gluProject
    #define gluPwlCurve Portal::gluPwlCurve
    #define gluQuadricCallback Portal::gluQuadricCallback
    #define gluQuadricDrawStyle Portal::gluQuadricDrawStyle
    #define gluQuadricNormals Portal::gluQuadricNormals
    #define gluQuadricOrientation Portal::gluQuadricOrientation
    #define gluQuadricTexture Portal::gluQuadricTexture
    #define gluScaleImage Portal::gluScaleImage
    #define gluSphere Portal::gluSphere
    #define gluTessBeginContour Portal::gluTessBeginContour
    #define gluTessBeginPolygon Portal::gluTessBeginPolygon
    #define gluTessCallback Portal::gluTessCallback
    #define gluTessEndContour Portal::gluTessEndContour
    #define gluTessEndPolygon Portal::gluTessEndPolygon
    #define gluTessNormal Portal::gluTessNormal
    #define gluTessProperty Portal::gluTessProperty
    #define gluTessVertex Portal::gluTessVertex
    #define gluUnProject Portal::gluUnProject
    #define gluUnProject4 Portal::gluUnProject4
    #define glXChooseVisual Portal::glXChooseVisual
    #define glXCopyContext Portal::glXCopyContext
    #define glXCreateContext Portal::glXCreateContext
    #define glXCreateGLXPixmap Portal::glXCreateGLXPixmap
    #define glXDestroyContext Portal::glXDestroyContext
    #define glXDestroyGLXPixmap Portal::glXDestroyGLXPixmap
    #define glXGetConfig Portal::glXGetConfig
    #define glXGetCurrentContext Portal::glXGetCurrentContext
    #define glXGetCurrentDrawable Portal::glXGetCurrentDrawable
    #define glXIsDirect Portal::glXIsDirect
    #define glXMakeCurrent Portal::glXMakeCurrent
    #define glXQueryExtension Portal::glXQueryExtension
    #define glXQueryVersion Portal::glXQueryVersion
    #define glXSwapBuffers Portal::glXSwapBuffers
    #define glXUseXFont Portal::glXUseXFont
    #define glXWaitGL Portal::glXWaitGL
    #define glXWaitX Portal::glXWaitX
    #define glXGetClientString Portal::glXGetClientString
    #define glXQueryServerString Portal::glXQueryServerString
    #define glXQueryExtensionsString Portal::glXQueryExtensionsString
    #define glXGetCurrentDisplay Portal::glXGetCurrentDisplay
    #define glXChooseFBConfig Portal::glXChooseFBConfig
    #define glXCreateNewContext Portal::glXCreateNewContext
    #define glXCreatePbuffer Portal::glXCreatePbuffer
    #define glXCreatePixmap Portal::glXCreatePixmap
    #define glXCreateWindow Portal::glXCreateWindow
    #define glXDestroyPbuffer Portal::glXDestroyPbuffer
    #define glXDestroyPixmap Portal::glXDestroyPixmap
    #define glXDestroyWindow Portal::glXDestroyWindow
    #define glXGetCurrentReadDrawable Portal::glXGetCurrentReadDrawable
    #define glXGetFBConfigAttrib Portal::glXGetFBConfigAttrib
    #define glXGetFBConfigs Portal::glXGetFBConfigs
    #define glXGetSelectedEvent Portal::glXGetSelectedEvent
    #define glXGetVisualFromFBConfig Portal::glXGetVisualFromFBConfig
    #define glXMakeContextCurrent Portal::glXMakeContextCurrent
    #define glXQueryContext Portal::glXQueryContext
    #define glXQueryDrawable Portal::glXQueryDrawable
    #define glXSelectEvent Portal::glXSelectEvent
    #define glXFreeContextEXT Portal::glXFreeContextEXT
    #define glXGetContextIDEXT Portal::glXGetContextIDEXT
    #define glXImportContextEXT Portal::glXImportContextEXT
    #define glXQueryContextInfoEXT Portal::glXQueryContextInfoEXT
    #define glXAllocateMemoryNV Portal::glXAllocateMemoryNV
    #define glXFreeMemoryNV Portal::glXFreeMemoryNV
    #define glXGetVideoSyncSGI Portal::glXGetVideoSyncSGI
    #define glXWaitVideoSyncSGI Portal::glXWaitVideoSyncSGI
    #define glXGetRefreshRateSGI Portal::glXGetRefreshRateSGI
    #define glXSwapIntervalSGI Portal::glXSwapIntervalSGI
    #define glXJoinSwapGroupNV Portal::glXJoinSwapGroupNV
    #define glXBindSwapBarrierNV Portal::glXBindSwapBarrierNV
    #define glXQuerySwapGroupNV Portal::glXQuerySwapGroupNV
    #define glXQueryMaxSwapGroupsNV Portal::glXQueryMaxSwapGroupsNV
    #define glXQueryFrameCountNV Portal::glXQueryFrameCountNV
    #define glXResetFrameCountNV Portal::glXResetFrameCountNV
    #define glXGetVideoDeviceNV Portal::glXGetVideoDeviceNV
    #define glXReleaseVideoDeviceNV Portal::glXReleaseVideoDeviceNV
    #define glXBindVideoImageNV Portal::glXBindVideoImageNV
    #define glXReleaseVideoImageNV Portal::glXReleaseVideoImageNV
    #define glXSendPbufferToVideoNV Portal::glXSendPbufferToVideoNV
    #define glXGetVideoInfoNV Portal::glXGetVideoInfoNV
    #define glXGetFBConfigAttribSGIX Portal::glXGetFBConfigAttribSGIX
    #define glXChooseFBConfigSGIX Portal::glXChooseFBConfigSGIX
    #define glXCreateGLXPixmapWithConfigSGIX Portal::glXCreateGLXPixmapWithConfigSGIX
    #define glXCreateContextWithConfigSGIX Portal::glXCreateContextWithConfigSGIX
    #define glXGetVisualFromFBConfigSGIX Portal::glXGetVisualFromFBConfigSGIX
    #define glXGetFBConfigFromVisualSGIX Portal::glXGetFBConfigFromVisualSGIX
    #define glXCreateGLXPbufferSGIX Portal::glXCreateGLXPbufferSGIX
    #define glXDestroyGLXPbufferSGIX Portal::glXDestroyGLXPbufferSGIX
    #define glXQueryGLXPbufferSGIX Portal::glXQueryGLXPbufferSGIX
    #define glXSelectEventSGIX Portal::glXSelectEventSGIX
    #define glXGetSelectedEventSGIX Portal::glXGetSelectedEventSGIX
    #define dlopen Portal::dlopen
    #define dlclose Portal::dlclose
    #define dlsym Portal::dlsym
    #define dlmopen Portal::dlmopen
    #define dlvsym Portal::dlvsym
    #define dlerror Portal::dlerror
    #define dladdr Portal::dladdr
    #define dladdr1 Portal::dladdr1
    #define dlinfo Portal::dlinfo
    #define pthread_sigmask Portal::pthread_sigmask
    #define pthread_kill Portal::pthread_kill
    #define pthread_self Portal::pthread_self
    #define pthread_equal Portal::pthread_equal
    #define pthread_exit Portal::pthread_exit
    #define pthread_join Portal::pthread_join
    #define pthread_detach Portal::pthread_detach
    #define pthread_attr_init Portal::pthread_attr_init
    #define pthread_attr_destroy Portal::pthread_attr_destroy
    #define pthread_attr_setdetachstate Portal::pthread_attr_setdetachstate
    #define pthread_attr_getdetachstate Portal::pthread_attr_getdetachstate
    #define pthread_attr_setschedparam Portal::pthread_attr_setschedparam
    #define pthread_attr_getschedparam Portal::pthread_attr_getschedparam
    #define pthread_attr_setschedpolicy Portal::pthread_attr_setschedpolicy
    #define pthread_attr_getschedpolicy Portal::pthread_attr_getschedpolicy
    #define pthread_attr_setinheritsched Portal::pthread_attr_setinheritsched
    #define pthread_attr_getinheritsched Portal::pthread_attr_getinheritsched
    #define pthread_attr_setscope Portal::pthread_attr_setscope
    #define pthread_attr_getscope Portal::pthread_attr_getscope
    #define pthread_attr_setguardsize Portal::pthread_attr_setguardsize
    #define pthread_attr_getguardsize Portal::pthread_attr_getguardsize
    #define pthread_attr_setstackaddr Portal::pthread_attr_setstackaddr
    #define pthread_attr_getstackaddr Portal::pthread_attr_getstackaddr
    #define pthread_attr_setstack Portal::pthread_attr_setstack
    #define pthread_attr_getstack Portal::pthread_attr_getstack
    #define pthread_attr_setstacksize Portal::pthread_attr_setstacksize
    #define pthread_attr_getstacksize Portal::pthread_attr_getstacksize
    #define pthread_getattr_np Portal::pthread_getattr_np
    #define pthread_setschedparam Portal::pthread_setschedparam
    #define pthread_getschedparam Portal::pthread_getschedparam
    #define pthread_getconcurrency Portal::pthread_getconcurrency
    #define pthread_setconcurrency Portal::pthread_setconcurrency
    #define pthread_yield Portal::pthread_yield
    #define pthread_mutex_init Portal::pthread_mutex_init
    #define pthread_mutex_destroy Portal::pthread_mutex_destroy
    #define pthread_mutex_trylock Portal::pthread_mutex_trylock
    #define pthread_mutex_lock Portal::pthread_mutex_lock
    #define pthread_mutex_timedlock Portal::pthread_mutex_timedlock
    #define pthread_mutex_unlock Portal::pthread_mutex_unlock
    #define pthread_mutexattr_init Portal::pthread_mutexattr_init
    #define pthread_mutexattr_destroy Portal::pthread_mutexattr_destroy
    #define pthread_mutexattr_getpshared Portal::pthread_mutexattr_getpshared
    #define pthread_mutexattr_setpshared Portal::pthread_mutexattr_setpshared
    #define pthread_mutexattr_settype Portal::pthread_mutexattr_settype
    #define pthread_mutexattr_gettype Portal::pthread_mutexattr_gettype
    #define pthread_cond_init Portal::pthread_cond_init
    #define pthread_cond_destroy Portal::pthread_cond_destroy
    #define pthread_cond_signal Portal::pthread_cond_signal
    #define pthread_cond_broadcast Portal::pthread_cond_broadcast
    #define pthread_cond_wait Portal::pthread_cond_wait
    #define pthread_cond_timedwait Portal::pthread_cond_timedwait
    #define pthread_condattr_init Portal::pthread_condattr_init
    #define pthread_condattr_destroy Portal::pthread_condattr_destroy
    #define pthread_condattr_getpshared Portal::pthread_condattr_getpshared
    #define pthread_condattr_setpshared Portal::pthread_condattr_setpshared
    #define pthread_rwlock_init Portal::pthread_rwlock_init
    #define pthread_rwlock_destroy Portal::pthread_rwlock_destroy
    #define pthread_rwlock_rdlock Portal::pthread_rwlock_rdlock
    #define pthread_rwlock_tryrdlock Portal::pthread_rwlock_tryrdlock
    #define pthread_rwlock_timedrdlock Portal::pthread_rwlock_timedrdlock
    #define pthread_rwlock_wrlock Portal::pthread_rwlock_wrlock
    #define pthread_rwlock_trywrlock Portal::pthread_rwlock_trywrlock
    #define pthread_rwlock_timedwrlock Portal::pthread_rwlock_timedwrlock
    #define pthread_rwlock_unlock Portal::pthread_rwlock_unlock
    #define pthread_rwlockattr_init Portal::pthread_rwlockattr_init
    #define pthread_rwlockattr_destroy Portal::pthread_rwlockattr_destroy
    #define pthread_rwlockattr_getpshared Portal::pthread_rwlockattr_getpshared
    #define pthread_rwlockattr_setpshared Portal::pthread_rwlockattr_setpshared
    #define pthread_rwlockattr_getkind_np Portal::pthread_rwlockattr_getkind_np
    #define pthread_rwlockattr_setkind_np Portal::pthread_rwlockattr_setkind_np
    #define pthread_spin_init Portal::pthread_spin_init
    #define pthread_spin_destroy Portal::pthread_spin_destroy
    #define pthread_spin_lock Portal::pthread_spin_lock
    #define pthread_spin_trylock Portal::pthread_spin_trylock
    #define pthread_spin_unlock Portal::pthread_spin_unlock
    #define pthread_barrier_init Portal::pthread_barrier_init
    #define pthread_barrier_destroy Portal::pthread_barrier_destroy
    #define pthread_barrierattr_init Portal::pthread_barrierattr_init
    #define pthread_barrierattr_destroy Portal::pthread_barrierattr_destroy
    #define pthread_barrierattr_setpshared Portal::pthread_barrierattr_setpshared
    #define pthread_barrier_wait Portal::pthread_barrier_wait
    #define pthread_key_delete Portal::pthread_key_delete
    #define pthread_setspecific Portal::pthread_setspecific
    #define pthread_getspecific Portal::pthread_getspecific
    #define pthread_setcancelstate Portal::pthread_setcancelstate
    #define pthread_setcanceltype Portal::pthread_setcanceltype
    #define pthread_cancel Portal::pthread_cancel
    #define pthread_testcancel Portal::pthread_testcancel
    #define _pthread_cleanup_pop Portal::_pthread_cleanup_pop
    #define _pthread_cleanup_pop_restore Portal::_pthread_cleanup_pop_restore
    #define pthread_getcpuclockid Portal::pthread_getcpuclockid
    #define pthread_kill_other_threads_np Portal::pthread_kill_other_threads_np
    #define XRRQueryExtension Portal::XRRQueryExtension
    #define XRRQueryVersion Portal::XRRQueryVersion
    #define XRRGetScreenInfo Portal::XRRGetScreenInfo
    #define XRRFreeScreenConfigInfo Portal::XRRFreeScreenConfigInfo
    #define XRRSetScreenConfig Portal::XRRSetScreenConfig
    #define XRRSetScreenConfigAndRate Portal::XRRSetScreenConfigAndRate
    #define XRRConfigRotations Portal::XRRConfigRotations
    #define XRRConfigTimes Portal::XRRConfigTimes
    #define XRRConfigSizes Portal::XRRConfigSizes
    #define XRRConfigRates Portal::XRRConfigRates
    #define XRRConfigCurrentConfiguration Portal::XRRConfigCurrentConfiguration
    #define XRRConfigCurrentRate Portal::XRRConfigCurrentRate
    #define XRRRootToScreen Portal::XRRRootToScreen
    #define XRRSelectInput Portal::XRRSelectInput
    #define XRRRotations Portal::XRRRotations
    #define XRRSizes Portal::XRRSizes
    #define XRRRates Portal::XRRRates
    #define XRRTimes Portal::XRRTimes
    #define XRRUpdateConfiguration Portal::XRRUpdateConfiguration
    #define XLoadQueryFont Portal::XLoadQueryFont
    #define XQueryFont Portal::XQueryFont
    #define XGetMotionEvents Portal::XGetMotionEvents
    #define XDeleteModifiermapEntry Portal::XDeleteModifiermapEntry
    #define XGetModifierMapping Portal::XGetModifierMapping
    #define XInsertModifiermapEntry Portal::XInsertModifiermapEntry
    #define XNewModifiermap Portal::XNewModifiermap
    #define XCreateImage Portal::XCreateImage
    #define XInitImage Portal::XInitImage
    #define XGetImage Portal::XGetImage
    #define XGetSubImage Portal::XGetSubImage
    #define XOpenDisplay Portal::XOpenDisplay
    #define XrmInitialize Portal::XrmInitialize
    #define XFetchBytes Portal::XFetchBytes
    #define XFetchBuffer Portal::XFetchBuffer
    #define XGetAtomName Portal::XGetAtomName
    #define XGetAtomNames Portal::XGetAtomNames
    #define XGetDefault Portal::XGetDefault
    #define XDisplayName Portal::XDisplayName
    #define XKeysymToString Portal::XKeysymToString
    #define XInternAtom Portal::XInternAtom
    #define XInternAtoms Portal::XInternAtoms
    #define XCopyColormapAndFree Portal::XCopyColormapAndFree
    #define XCreateColormap Portal::XCreateColormap
    #define XCreatePixmapCursor Portal::XCreatePixmapCursor
    #define XCreateGlyphCursor Portal::XCreateGlyphCursor
    #define XCreateFontCursor Portal::XCreateFontCursor
    #define XLoadFont Portal::XLoadFont
    #define XCreateGC Portal::XCreateGC
    #define XGContextFromGC Portal::XGContextFromGC
    #define XFlushGC Portal::XFlushGC
    #define XCreatePixmap Portal::XCreatePixmap
    #define XCreateBitmapFromData Portal::XCreateBitmapFromData
    #define XCreatePixmapFromBitmapData Portal::XCreatePixmapFromBitmapData
    #define XCreateSimpleWindow Portal::XCreateSimpleWindow
    #define XGetSelectionOwner Portal::XGetSelectionOwner
    #define XCreateWindow Portal::XCreateWindow
    #define XListInstalledColormaps Portal::XListInstalledColormaps
    #define XListFonts Portal::XListFonts
    #define XListFontsWithInfo Portal::XListFontsWithInfo
    #define XGetFontPath Portal::XGetFontPath
    #define XListExtensions Portal::XListExtensions
    #define XListProperties Portal::XListProperties
    #define XListHosts Portal::XListHosts
    #define XKeycodeToKeysym Portal::XKeycodeToKeysym
    #define XLookupKeysym Portal::XLookupKeysym
    #define XGetKeyboardMapping Portal::XGetKeyboardMapping
    #define XStringToKeysym Portal::XStringToKeysym
    #define XMaxRequestSize Portal::XMaxRequestSize
    #define XExtendedMaxRequestSize Portal::XExtendedMaxRequestSize
    #define XResourceManagerString Portal::XResourceManagerString
    #define XScreenResourceString Portal::XScreenResourceString
    #define XDisplayMotionBufferSize Portal::XDisplayMotionBufferSize
    #define XVisualIDFromVisual Portal::XVisualIDFromVisual
    #define XInitThreads Portal::XInitThreads
    #define XLockDisplay Portal::XLockDisplay
    #define XUnlockDisplay Portal::XUnlockDisplay
    #define XInitExtension Portal::XInitExtension
    #define XAddExtension Portal::XAddExtension
    #define XFindOnExtensionList Portal::XFindOnExtensionList
    #define XEHeadOfExtensionList Portal::XEHeadOfExtensionList
    #define XRootWindow Portal::XRootWindow
    #define XDefaultRootWindow Portal::XDefaultRootWindow
    #define XRootWindowOfScreen Portal::XRootWindowOfScreen
    #define XDefaultVisual Portal::XDefaultVisual
    #define XDefaultVisualOfScreen Portal::XDefaultVisualOfScreen
    #define XDefaultGC Portal::XDefaultGC
    #define XDefaultGCOfScreen Portal::XDefaultGCOfScreen
    #define XBlackPixel Portal::XBlackPixel
    #define XWhitePixel Portal::XWhitePixel
    #define XAllPlanes Portal::XAllPlanes
    #define XBlackPixelOfScreen Portal::XBlackPixelOfScreen
    #define XWhitePixelOfScreen Portal::XWhitePixelOfScreen
    #define XNextRequest Portal::XNextRequest
    #define XLastKnownRequestProcessed Portal::XLastKnownRequestProcessed
    #define XServerVendor Portal::XServerVendor
    #define XDisplayString Portal::XDisplayString
    #define XDefaultColormap Portal::XDefaultColormap
    #define XDefaultColormapOfScreen Portal::XDefaultColormapOfScreen
    #define XDisplayOfScreen Portal::XDisplayOfScreen
    #define XScreenOfDisplay Portal::XScreenOfDisplay
    #define XDefaultScreenOfDisplay Portal::XDefaultScreenOfDisplay
    #define XEventMaskOfScreen Portal::XEventMaskOfScreen
    #define XScreenNumberOfScreen Portal::XScreenNumberOfScreen
    #define XSetErrorHandler Portal::XSetErrorHandler
    #define XSetIOErrorHandler Portal::XSetIOErrorHandler
    #define XListPixmapFormats Portal::XListPixmapFormats
    #define XListDepths Portal::XListDepths
    #define XReconfigureWMWindow Portal::XReconfigureWMWindow
    #define XGetWMProtocols Portal::XGetWMProtocols
    #define XSetWMProtocols Portal::XSetWMProtocols
    #define XIconifyWindow Portal::XIconifyWindow
    #define XWithdrawWindow Portal::XWithdrawWindow
    #define XGetCommand Portal::XGetCommand
    #define XGetWMColormapWindows Portal::XGetWMColormapWindows
    #define XSetWMColormapWindows Portal::XSetWMColormapWindows
    #define XFreeStringList Portal::XFreeStringList
    #define XSetTransientForHint Portal::XSetTransientForHint
    #define XActivateScreenSaver Portal::XActivateScreenSaver
    #define XAddHost Portal::XAddHost
    #define XAddHosts Portal::XAddHosts
    #define XAddToExtensionList Portal::XAddToExtensionList
    #define XAddToSaveSet Portal::XAddToSaveSet
    #define XAllocColor Portal::XAllocColor
    #define XAllocColorCells Portal::XAllocColorCells
    #define XAllocColorPlanes Portal::XAllocColorPlanes
    #define XAllocNamedColor Portal::XAllocNamedColor
    #define XAllowEvents Portal::XAllowEvents
    #define XAutoRepeatOff Portal::XAutoRepeatOff
    #define XAutoRepeatOn Portal::XAutoRepeatOn
    #define XBell Portal::XBell
    #define XBitmapBitOrder Portal::XBitmapBitOrder
    #define XBitmapPad Portal::XBitmapPad
    #define XBitmapUnit Portal::XBitmapUnit
    #define XCellsOfScreen Portal::XCellsOfScreen
    #define XChangeActivePointerGrab Portal::XChangeActivePointerGrab
    #define XChangeGC Portal::XChangeGC
    #define XChangeKeyboardControl Portal::XChangeKeyboardControl
    #define XChangeKeyboardMapping Portal::XChangeKeyboardMapping
    #define XChangePointerControl Portal::XChangePointerControl
    #define XChangeProperty Portal::XChangeProperty
    #define XChangeSaveSet Portal::XChangeSaveSet
    #define XChangeWindowAttributes Portal::XChangeWindowAttributes
    #define XCheckMaskEvent Portal::XCheckMaskEvent
    #define XCheckTypedEvent Portal::XCheckTypedEvent
    #define XCheckTypedWindowEvent Portal::XCheckTypedWindowEvent
    #define XCheckWindowEvent Portal::XCheckWindowEvent
    #define XCirculateSubwindows Portal::XCirculateSubwindows
    #define XCirculateSubwindowsDown Portal::XCirculateSubwindowsDown
    #define XCirculateSubwindowsUp Portal::XCirculateSubwindowsUp
    #define XClearArea Portal::XClearArea
    #define XClearWindow Portal::XClearWindow
    #define XCloseDisplay Portal::XCloseDisplay
    #define XConfigureWindow Portal::XConfigureWindow
    #define XConnectionNumber Portal::XConnectionNumber
    #define XConvertSelection Portal::XConvertSelection
    #define XCopyArea Portal::XCopyArea
    #define XCopyGC Portal::XCopyGC
    #define XCopyPlane Portal::XCopyPlane
    #define XDefaultDepth Portal::XDefaultDepth
    #define XDefaultDepthOfScreen Portal::XDefaultDepthOfScreen
    #define XDefaultScreen Portal::XDefaultScreen
    #define XDefineCursor Portal::XDefineCursor
    #define XDeleteProperty Portal::XDeleteProperty
    #define XDestroyWindow Portal::XDestroyWindow
    #define XDestroySubwindows Portal::XDestroySubwindows
    #define XDoesBackingStore Portal::XDoesBackingStore
    #define XDoesSaveUnders Portal::XDoesSaveUnders
    #define XDisableAccessControl Portal::XDisableAccessControl
    #define XDisplayCells Portal::XDisplayCells
    #define XDisplayHeight Portal::XDisplayHeight
    #define XDisplayHeightMM Portal::XDisplayHeightMM
    #define XDisplayKeycodes Portal::XDisplayKeycodes
    #define XDisplayPlanes Portal::XDisplayPlanes
    #define XDisplayWidth Portal::XDisplayWidth
    #define XDisplayWidthMM Portal::XDisplayWidthMM
    #define XDrawArc Portal::XDrawArc
    #define XDrawArcs Portal::XDrawArcs
    #define XDrawImageString Portal::XDrawImageString
    #define XDrawImageString16 Portal::XDrawImageString16
    #define XDrawLine Portal::XDrawLine
    #define XDrawLines Portal::XDrawLines
    #define XDrawPoint Portal::XDrawPoint
    #define XDrawPoints Portal::XDrawPoints
    #define XDrawRectangle Portal::XDrawRectangle
    #define XDrawRectangles Portal::XDrawRectangles
    #define XDrawSegments Portal::XDrawSegments
    #define XDrawString Portal::XDrawString
    #define XDrawString16 Portal::XDrawString16
    #define XDrawText Portal::XDrawText
    #define XDrawText16 Portal::XDrawText16
    #define XEnableAccessControl Portal::XEnableAccessControl
    #define XEventsQueued Portal::XEventsQueued
    #define XFetchName Portal::XFetchName
    #define XFillArc Portal::XFillArc
    #define XFillArcs Portal::XFillArcs
    #define XFillPolygon Portal::XFillPolygon
    #define XFillRectangle Portal::XFillRectangle
    #define XFillRectangles Portal::XFillRectangles
    #define XFlush Portal::XFlush
    #define XForceScreenSaver Portal::XForceScreenSaver
    #define XFree Portal::XFree
    #define XFreeColormap Portal::XFreeColormap
    #define XFreeColors Portal::XFreeColors
    #define XFreeCursor Portal::XFreeCursor
    #define XFreeExtensionList Portal::XFreeExtensionList
    #define XFreeFont Portal::XFreeFont
    #define XFreeFontInfo Portal::XFreeFontInfo
    #define XFreeFontNames Portal::XFreeFontNames
    #define XFreeFontPath Portal::XFreeFontPath
    #define XFreeGC Portal::XFreeGC
    #define XFreeModifiermap Portal::XFreeModifiermap
    #define XFreePixmap Portal::XFreePixmap
    #define XGeometry Portal::XGeometry
    #define XGetErrorDatabaseText Portal::XGetErrorDatabaseText
    #define XGetErrorText Portal::XGetErrorText
    #define XGetFontProperty Portal::XGetFontProperty
    #define XGetGCValues Portal::XGetGCValues
    #define XGetGeometry Portal::XGetGeometry
    #define XGetIconName Portal::XGetIconName
    #define XGetInputFocus Portal::XGetInputFocus
    #define XGetKeyboardControl Portal::XGetKeyboardControl
    #define XGetPointerControl Portal::XGetPointerControl
    #define XGetPointerMapping Portal::XGetPointerMapping
    #define XGetScreenSaver Portal::XGetScreenSaver
    #define XGetTransientForHint Portal::XGetTransientForHint
    #define XGetWindowProperty Portal::XGetWindowProperty
    #define XGetWindowAttributes Portal::XGetWindowAttributes
    #define XGrabButton Portal::XGrabButton
    #define XGrabKey Portal::XGrabKey
    #define XGrabKeyboard Portal::XGrabKeyboard
    #define XGrabPointer Portal::XGrabPointer
    #define XGrabServer Portal::XGrabServer
    #define XHeightMMOfScreen Portal::XHeightMMOfScreen
    #define XHeightOfScreen Portal::XHeightOfScreen
    #define XImageByteOrder Portal::XImageByteOrder
    #define XInstallColormap Portal::XInstallColormap
    #define XKeysymToKeycode Portal::XKeysymToKeycode
    #define XKillClient Portal::XKillClient
    #define XLookupColor Portal::XLookupColor
    #define XLowerWindow Portal::XLowerWindow
    #define XMapRaised Portal::XMapRaised
    #define XMapSubwindows Portal::XMapSubwindows
    #define XMapWindow Portal::XMapWindow
    #define XMaskEvent Portal::XMaskEvent
    #define XMaxCmapsOfScreen Portal::XMaxCmapsOfScreen
    #define XMinCmapsOfScreen Portal::XMinCmapsOfScreen
    #define XMoveResizeWindow Portal::XMoveResizeWindow
    #define XMoveWindow Portal::XMoveWindow
    #define XNextEvent Portal::XNextEvent
    #define XNoOp Portal::XNoOp
    #define XParseColor Portal::XParseColor
    #define XParseGeometry Portal::XParseGeometry
    #define XPeekEvent Portal::XPeekEvent
    #define XPending Portal::XPending
    #define XPlanesOfScreen Portal::XPlanesOfScreen
    #define XProtocolRevision Portal::XProtocolRevision
    #define XProtocolVersion Portal::XProtocolVersion
    #define XPutBackEvent Portal::XPutBackEvent
    #define XPutImage Portal::XPutImage
    #define XQLength Portal::XQLength
    #define XQueryBestCursor Portal::XQueryBestCursor
    #define XQueryBestSize Portal::XQueryBestSize
    #define XQueryBestStipple Portal::XQueryBestStipple
    #define XQueryBestTile Portal::XQueryBestTile
    #define XQueryColor Portal::XQueryColor
    #define XQueryColors Portal::XQueryColors
    #define XQueryExtension Portal::XQueryExtension
    #define XQueryKeymap Portal::XQueryKeymap
    #define XQueryPointer Portal::XQueryPointer
    #define XQueryTextExtents Portal::XQueryTextExtents
    #define XQueryTextExtents16 Portal::XQueryTextExtents16
    #define XQueryTree Portal::XQueryTree
    #define XRaiseWindow Portal::XRaiseWindow
    #define XReadBitmapFile Portal::XReadBitmapFile
    #define XReadBitmapFileData Portal::XReadBitmapFileData
    #define XRebindKeysym Portal::XRebindKeysym
    #define XRecolorCursor Portal::XRecolorCursor
    #define XRefreshKeyboardMapping Portal::XRefreshKeyboardMapping
    #define XRemoveFromSaveSet Portal::XRemoveFromSaveSet
    #define XRemoveHost Portal::XRemoveHost
    #define XRemoveHosts Portal::XRemoveHosts
    #define XReparentWindow Portal::XReparentWindow
    #define XResetScreenSaver Portal::XResetScreenSaver
    #define XResizeWindow Portal::XResizeWindow
    #define XRestackWindows Portal::XRestackWindows
    #define XRotateBuffers Portal::XRotateBuffers
    #define XRotateWindowProperties Portal::XRotateWindowProperties
    #define XScreenCount Portal::XScreenCount
    #define XSelectInput Portal::XSelectInput
    #define XSendEvent Portal::XSendEvent
    #define XSetAccessControl Portal::XSetAccessControl
    #define XSetArcMode Portal::XSetArcMode
    #define XSetBackground Portal::XSetBackground
    #define XSetClipMask Portal::XSetClipMask
    #define XSetClipOrigin Portal::XSetClipOrigin
    #define XSetClipRectangles Portal::XSetClipRectangles
    #define XSetCloseDownMode Portal::XSetCloseDownMode
    #define XSetCommand Portal::XSetCommand
    #define XSetDashes Portal::XSetDashes
    #define XSetFillRule Portal::XSetFillRule
    #define XSetFillStyle Portal::XSetFillStyle
    #define XSetFont Portal::XSetFont
    #define XSetFontPath Portal::XSetFontPath
    #define XSetForeground Portal::XSetForeground
    #define XSetFunction Portal::XSetFunction
    #define XSetGraphicsExposures Portal::XSetGraphicsExposures
    #define XSetIconName Portal::XSetIconName
    #define XSetInputFocus Portal::XSetInputFocus
    #define XSetLineAttributes Portal::XSetLineAttributes
    #define XSetModifierMapping Portal::XSetModifierMapping
    #define XSetPlaneMask Portal::XSetPlaneMask
    #define XSetPointerMapping Portal::XSetPointerMapping
    #define XSetScreenSaver Portal::XSetScreenSaver
    #define XSetSelectionOwner Portal::XSetSelectionOwner
    #define XSetState Portal::XSetState
    #define XSetStipple Portal::XSetStipple
    #define XSetSubwindowMode Portal::XSetSubwindowMode
    #define XSetTSOrigin Portal::XSetTSOrigin
    #define XSetTile Portal::XSetTile
    #define XSetWindowBackground Portal::XSetWindowBackground
    #define XSetWindowBackgroundPixmap Portal::XSetWindowBackgroundPixmap
    #define XSetWindowBorder Portal::XSetWindowBorder
    #define XSetWindowBorderPixmap Portal::XSetWindowBorderPixmap
    #define XSetWindowBorderWidth Portal::XSetWindowBorderWidth
    #define XSetWindowColormap Portal::XSetWindowColormap
    #define XStoreBuffer Portal::XStoreBuffer
    #define XStoreBytes Portal::XStoreBytes
    #define XStoreColor Portal::XStoreColor
    #define XStoreColors Portal::XStoreColors
    #define XStoreName Portal::XStoreName
    #define XStoreNamedColor Portal::XStoreNamedColor
    #define XSync Portal::XSync
    #define XTextExtents Portal::XTextExtents
    #define XTextExtents16 Portal::XTextExtents16
    #define XTextWidth Portal::XTextWidth
    #define XTextWidth16 Portal::XTextWidth16
    #define XTranslateCoordinates Portal::XTranslateCoordinates
    #define XUndefineCursor Portal::XUndefineCursor
    #define XUngrabButton Portal::XUngrabButton
    #define XUngrabKey Portal::XUngrabKey
    #define XUngrabKeyboard Portal::XUngrabKeyboard
    #define XUngrabPointer Portal::XUngrabPointer
    #define XUngrabServer Portal::XUngrabServer
    #define XUninstallColormap Portal::XUninstallColormap
    #define XUnloadFont Portal::XUnloadFont
    #define XUnmapSubwindows Portal::XUnmapSubwindows
    #define XUnmapWindow Portal::XUnmapWindow
    #define XVendorRelease Portal::XVendorRelease
    #define XWarpPointer Portal::XWarpPointer
    #define XWidthMMOfScreen Portal::XWidthMMOfScreen
    #define XWidthOfScreen Portal::XWidthOfScreen
    #define XWindowEvent Portal::XWindowEvent
    #define XWriteBitmapFile Portal::XWriteBitmapFile
    #define XSupportsLocale Portal::XSupportsLocale
    #define XSetLocaleModifiers Portal::XSetLocaleModifiers
    #define XOpenOM Portal::XOpenOM
    #define XCloseOM Portal::XCloseOM
    #define XDisplayOfOM Portal::XDisplayOfOM
    #define XLocaleOfOM Portal::XLocaleOfOM
    #define XDestroyOC Portal::XDestroyOC
    #define XOMOfOC Portal::XOMOfOC
    #define XCreateFontSet Portal::XCreateFontSet
    #define XFreeFontSet Portal::XFreeFontSet
    #define XFontsOfFontSet Portal::XFontsOfFontSet
    #define XBaseFontNameListOfFontSet Portal::XBaseFontNameListOfFontSet
    #define XLocaleOfFontSet Portal::XLocaleOfFontSet
    #define XContextDependentDrawing Portal::XContextDependentDrawing
    #define XDirectionalDependentDrawing Portal::XDirectionalDependentDrawing
    #define XContextualDrawing Portal::XContextualDrawing
    #define XExtentsOfFontSet Portal::XExtentsOfFontSet
    #define XmbTextEscapement Portal::XmbTextEscapement
    #define XwcTextEscapement Portal::XwcTextEscapement
    #define Xutf8TextEscapement Portal::Xutf8TextEscapement
    #define XmbTextExtents Portal::XmbTextExtents
    #define XwcTextExtents Portal::XwcTextExtents
    #define Xutf8TextExtents Portal::Xutf8TextExtents
    #define XmbTextPerCharExtents Portal::XmbTextPerCharExtents
    #define XwcTextPerCharExtents Portal::XwcTextPerCharExtents
    #define Xutf8TextPerCharExtents Portal::Xutf8TextPerCharExtents
    #define XmbDrawText Portal::XmbDrawText
    #define XwcDrawText Portal::XwcDrawText
    #define Xutf8DrawText Portal::Xutf8DrawText
    #define XmbDrawString Portal::XmbDrawString
    #define XwcDrawString Portal::XwcDrawString
    #define Xutf8DrawString Portal::Xutf8DrawString
    #define XmbDrawImageString Portal::XmbDrawImageString
    #define XwcDrawImageString Portal::XwcDrawImageString
    #define Xutf8DrawImageString Portal::Xutf8DrawImageString
    #define XOpenIM Portal::XOpenIM
    #define XCloseIM Portal::XCloseIM
    #define XDisplayOfIM Portal::XDisplayOfIM
    #define XLocaleOfIM Portal::XLocaleOfIM
    #define XDestroyIC Portal::XDestroyIC
    #define XSetICFocus Portal::XSetICFocus
    #define XUnsetICFocus Portal::XUnsetICFocus
    #define XwcResetIC Portal::XwcResetIC
    #define XmbResetIC Portal::XmbResetIC
    #define Xutf8ResetIC Portal::Xutf8ResetIC
    #define XIMOfIC Portal::XIMOfIC
    #define XFilterEvent Portal::XFilterEvent
    #define XmbLookupString Portal::XmbLookupString
    #define XwcLookupString Portal::XwcLookupString
    #define Xutf8LookupString Portal::Xutf8LookupString
    #define XRegisterIMInstantiateCallback Portal::XRegisterIMInstantiateCallback
    #define XUnregisterIMInstantiateCallback Portal::XUnregisterIMInstantiateCallback
    #define XInternalConnectionNumbers Portal::XInternalConnectionNumbers
    #define XProcessInternalConnection Portal::XProcessInternalConnection
    #define XAddConnectionWatch Portal::XAddConnectionWatch
    #define XRemoveConnectionWatch Portal::XRemoveConnectionWatch
    #define XSetAuthorization Portal::XSetAuthorization
    #define _Xwctomb Portal::_Xwctomb
    #define XDestroyImage Portal::XDestroyImage
    #define XGetPixel Portal::XGetPixel
    #define XPutPixel Portal::XPutPixel
    #define XSubImage Portal::XSubImage
    #define XAddPixel Portal::XAddPixel
    #define XAllocClassHint Portal::XAllocClassHint
    #define XAllocIconSize Portal::XAllocIconSize
    #define XAllocSizeHints Portal::XAllocSizeHints
    #define XAllocStandardColormap Portal::XAllocStandardColormap
    #define XAllocWMHints Portal::XAllocWMHints
    #define XClipBox Portal::XClipBox
    #define XCreateRegion Portal::XCreateRegion
    #define XDefaultString Portal::XDefaultString
    #define XDeleteContext Portal::XDeleteContext
    #define XDestroyRegion Portal::XDestroyRegion
    #define XEmptyRegion Portal::XEmptyRegion
    #define XEqualRegion Portal::XEqualRegion
    #define XFindContext Portal::XFindContext
    #define XGetClassHint Portal::XGetClassHint
    #define XGetIconSizes Portal::XGetIconSizes
    #define XGetNormalHints Portal::XGetNormalHints
    #define XGetRGBColormaps Portal::XGetRGBColormaps
    #define XGetSizeHints Portal::XGetSizeHints
    #define XGetStandardColormap Portal::XGetStandardColormap
    #define XGetTextProperty Portal::XGetTextProperty
    #define XGetVisualInfo Portal::XGetVisualInfo
    #define XGetWMClientMachine Portal::XGetWMClientMachine
    #define XGetWMHints Portal::XGetWMHints
    #define XGetWMIconName Portal::XGetWMIconName
    #define XGetWMName Portal::XGetWMName
    #define XGetWMNormalHints Portal::XGetWMNormalHints
    #define XGetWMSizeHints Portal::XGetWMSizeHints
    #define XGetZoomHints Portal::XGetZoomHints
    #define XIntersectRegion Portal::XIntersectRegion
    #define XConvertCase Portal::XConvertCase
    #define XLookupString Portal::XLookupString
    #define XMatchVisualInfo Portal::XMatchVisualInfo
    #define XOffsetRegion Portal::XOffsetRegion
    #define XPointInRegion Portal::XPointInRegion
    #define XPolygonRegion Portal::XPolygonRegion
    #define XRectInRegion Portal::XRectInRegion
    #define XSaveContext Portal::XSaveContext
    #define XSetClassHint Portal::XSetClassHint
    #define XSetIconSizes Portal::XSetIconSizes
    #define XSetNormalHints Portal::XSetNormalHints
    #define XSetRGBColormaps Portal::XSetRGBColormaps
    #define XSetSizeHints Portal::XSetSizeHints
    #define XSetStandardProperties Portal::XSetStandardProperties
    #define XSetTextProperty Portal::XSetTextProperty
    #define XSetWMClientMachine Portal::XSetWMClientMachine
    #define XSetWMHints Portal::XSetWMHints
    #define XSetWMIconName Portal::XSetWMIconName
    #define XSetWMName Portal::XSetWMName
    #define XSetWMNormalHints Portal::XSetWMNormalHints
    #define XSetWMProperties Portal::XSetWMProperties
    #define XmbSetWMProperties Portal::XmbSetWMProperties
    #define Xutf8SetWMProperties Portal::Xutf8SetWMProperties
    #define XSetWMSizeHints Portal::XSetWMSizeHints
    #define XSetRegion Portal::XSetRegion
    #define XSetStandardColormap Portal::XSetStandardColormap
    #define XSetZoomHints Portal::XSetZoomHints
    #define XShrinkRegion Portal::XShrinkRegion
    #define XStringListToTextProperty Portal::XStringListToTextProperty
    #define XSubtractRegion Portal::XSubtractRegion
    #define XmbTextListToTextProperty Portal::XmbTextListToTextProperty
    #define XwcTextListToTextProperty Portal::XwcTextListToTextProperty
    #define Xutf8TextListToTextProperty Portal::Xutf8TextListToTextProperty
    #define XwcFreeStringList Portal::XwcFreeStringList
    #define XTextPropertyToStringList Portal::XTextPropertyToStringList
    #define XmbTextPropertyToTextList Portal::XmbTextPropertyToTextList
    #define XwcTextPropertyToTextList Portal::XwcTextPropertyToTextList
    #define Xutf8TextPropertyToTextList Portal::Xutf8TextPropertyToTextList
    #define XUnionRectWithRegion Portal::XUnionRectWithRegion
    #define XUnionRegion Portal::XUnionRegion
    #define XWMGeometry Portal::XWMGeometry
    #define XXorRegion Portal::XXorRegion
    #define glXGetProcAddressARB Portal::glXGetProcAddressARB
#endif

#endif
