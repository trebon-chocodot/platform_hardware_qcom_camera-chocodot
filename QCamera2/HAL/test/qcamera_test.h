/* Copyright (c) 2012-2014, The Linux Foundataion. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef QCAMERA_TEST_H
#define QCAMERA_TEST_H

#include <SkData.h>
#include <SkBitmap.h>
#include <SkStream.h>

namespace qcamera {

using namespace android;

class CameraContext : public CameraListener {
public:
    typedef enum {
        READ_METADATA = 1,
        READ_IMAGE = 2,
        READ_ALL = 3
    } ReadMode_t;

    // This structure is used to store jpeg file sections in memory.
    typedef struct {
        unsigned char *  Data;
        int      Type;
        unsigned Size;
    } Sections_t;

public:

    CameraContext(int cameraIndex);
    virtual ~CameraContext();

    status_t openCamera();
    status_t closeCamera();

    status_t startPreview();
    status_t stopPreview();
    status_t resumePreview();
    status_t autoFocus();
    status_t enablePreviewCallbacks();
    status_t takePicture();
    status_t startRecording();
    status_t stopRecording();

    status_t nextPreviewSize();
    status_t setPreviewSize(const char *format);
    status_t getCurrentPreviewSize(Size &previewSize);

    status_t nextPictureSize();
    status_t getCurrentPictureSize(Size &pictureSize);
    status_t setPictureSize(const char *format);

    status_t nextVideoSize();
    status_t setVideoSize(const char *format);
    status_t getCurrentVideoSize(Size &videoSize);
    status_t configureRecorder();
    status_t unconfigureRecorder();
    Sections_t *FindSection(int SectionType);
    status_t ReadSectionsFromBuffer (unsigned char *buffer,
        unsigned int buffer_size, ReadMode_t ReadMode);

    void printMenu(sp<CameraContext> currentCamera);
    void printSupportedParams();

    int getCameraIndex() { return mCameraIndex; }
    int getNumberOfCameras();
    void enablePrintPreview();
    void disablePrintPreview();
    void enablePiPCapture();
    void disablePiPCapture();
    void CheckSectionsAllocated();
    void DiscardData();
    void DiscardSections();

    virtual void notify(int32_t msgType, int32_t ext1, int32_t ext2);
    virtual void postData(int32_t msgType,
                          const sp<IMemory>& dataPtr,
                          camera_frame_metadata_t *metadata);

    virtual void postDataTimestamp(nsecs_t timestamp,
                                   int32_t msgType,
                                   const sp<IMemory>& dataPtr);

private:

    status_t createPreviewSurface(unsigned int width,
                                  unsigned int height,
                                  int32_t pixFormat);
    status_t destroyPreviewSurface();

    status_t saveFile(const sp<IMemory>& mem, String8 path);
    SkBitmap * PiPCopyToOneFile(SkBitmap *bitmap0, SkBitmap *bitmap1);
    SkBitmap *decodeJPEG(const sp<IMemory>& mem);
    status_t encodeJPEG(SkWStream * stream, const SkBitmap *bitmap,
        String8 path);
    void previewCallback(const sp<IMemory>& mem);

    static int JpegIdx;
    int mCameraIndex;
    bool mResizePreview;
    bool mHardwareActive;
    bool mPreviewRunning;
    bool mRecordRunning;
    int  mVideoFd;
    int  mVideoIdx;
    bool mRecordingHint;
    bool mDoPrintMenu;
    bool mPiPCapture;
    static int mPiPIdx;
    int mfmtMultiplier;
    int mWidthTmp[2];
    int mHeightTmp[2];
    int mSectionsRead;
    int mSectionsAllocated;
    Sections_t * mSections;
    Sections_t * mJEXIFTmp;
    Sections_t mJEXIFSection;
    int mHaveAll;

    sp<Camera> mCamera;
    sp<SurfaceComposerClient> mClient;
    sp<SurfaceControl> mSurfaceControl;
    sp<Surface> mPreviewSurface;
    sp<MediaRecorder> mRecorder;
    CameraParameters mParams;
    SkBitmap *skBMDec;
    SkImageEncoder* skJpegEnc;
    static SkBitmap *skBMtmp[2];
    static sp<IMemory> PiPPtrTmp[2];

    int mCurrentPreviewSizeIdx;
    int mCurrentPictureSizeIdx;
    int mCurrentVideoSizeIdx;
    Vector<Size> mSupportedPreviewSizes;
    Vector<Size> mSupportedPictureSizes;
    Vector<Size> mSupportedVideoSizes;

    bool mInUse;
    Mutex mLock;
    Condition mCond;

    void useLock();
    void signalFinished();

    //------------------------------------------------------------------------
    // JPEG markers consist of one or more 0xFF bytes, followed by a marker
    // code byte (which is not an FF).  Here are the marker codes of interest
    // in this program.  (See jdmarker.c for a more complete list.)
    //------------------------------------------------------------------------
    #define M_SOF0  0xC0          // Start Of Frame N
    #define M_SOF1  0xC1          // N indicates which compression process
    #define M_SOF2  0xC2          // Only SOF0-SOF2 are now in common use
    #define M_SOF3  0xC3
    #define M_SOF5  0xC5          // NB: codes C4 and CC are NOT SOF markers
    #define M_SOF6  0xC6
    #define M_SOF7  0xC7
    #define M_SOF9  0xC9
    #define M_SOF10 0xCA
    #define M_SOF11 0xCB
    #define M_SOF13 0xCD
    #define M_SOF14 0xCE
    #define M_SOF15 0xCF
    #define M_SOI   0xD8          // Start Of Image (beginning of datastream)
    #define M_EOI   0xD9          // End Of Image (end of datastream)
    #define M_SOS   0xDA          // Start Of Scan (begins compressed data)
    #define M_JFIF  0xE0          // Jfif marker
    #define M_EXIF  0xE1          // Exif marker.  Also used for XMP data!
    #define M_XMP   0x10E1        // Not a real tag same value as Exif!
    #define M_COM   0xFE          // COMment
    #define M_DQT   0xDB
    #define M_DHT   0xC4
    #define M_DRI   0xDD
    #define M_IPTC  0xED          // IPTC marker
    #define PSEUDO_IMAGE_MARKER 0x123; // Extra value.
};


}; //namespace qcamera

using namespace qcamera;

class Interpreter
{
public:
    enum Commands_e {
        SWITCH_CAMERA_CMD = 'A',
        RESUME_PREVIEW_CMD = '[',
        START_PREVIEW_CMD = '1',
        STOP_PREVIEW_CMD = '2',
        CHANGE_PREVIEW_SIZE_CMD = '4',
        CHANGE_PICTURE_SIZE_CMD = '5',
        START_RECORD_CMD = '7',
        STOP_RECORD_CMD = '8',
        DUMP_CAPS_CMD = 'E',
        AUTOFOCUS_CMD = 'f',
        TAKEPICTURE_CMD = 'p',
        TAKEPICTURE_IN_PICTURE_CMD = 'P',
        ENABLE_PRV_CALLBACKS_CMD = '&',
        EXIT_CMD = 'q',
        DELAY = 'd',
        INVALID_CMD = '0'
    };

    struct Command {
        Command( Commands_e cmd, char *arg = NULL)
            : cmd(cmd)
            , arg(arg) {}
        Command()
            : cmd(INVALID_CMD)
            , arg(NULL) {}
        Commands_e cmd;
        char *arg;
    };

    /* API */
    Interpreter()
        : mUseScript(false)
        , mScript(NULL) {}

    Interpreter(const char *file);
    ~Interpreter();

    Command getCommand(sp<CameraContext> currentCamera);
private:
    static const int numberOfCommands;

    bool mUseScript;
    int mCmdIndex;
    char *mScript;
    Vector<Command> mCommands;

};

class TestContext
{
public:
    TestContext();
    ~TestContext();

    int32_t GetCamerasNum();
    status_t FunctionalTest();
    status_t AddScriptFromFile(const char *scriptFile);
private:
    char GetNextCmd(sp<qcamera::CameraContext> currentCamera);

    Vector< sp<qcamera::CameraContext> > mAvailableCameras;
    bool mTestRunning;
    int  mCurrentCameraIndex;
    int  mSaveCurrentCameraIndex;
    Interpreter *mInterpreter;
};

#endif
