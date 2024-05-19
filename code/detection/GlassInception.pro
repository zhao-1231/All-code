QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
#QMAKE_CXXFLAGS += /utf-8
#msvc {    QMAKE_CFLAGS += /utf-8    QMAKE_CXXFLAGS += /utf-8}
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    basicitem.cpp \
    camerabasic.cpp \
    cameradetectdialog.cpp \
    centerwidget.cpp \
    defectdetectionthread.cpp \
    defectinceptiondialog.cpp \
    grascaletunerdia.cpp \
    imageprocessthread.cpp \
    inceptionresultmodel.cpp \
    main.cpp \
    mainwindow.cpp \
    measurementsettingdialog.cpp \
    signaltuner.cpp

HEADERS += \
    basicitem.h \
    camerabasic.h \
    cameradetectdialog.h \
    centerwidget.h \
    cudaHeader.cuh \
    defectdetectionthread.h \
    defectinceptiondialog.h \
    grascaletunerdia.h \
    imageprocessthread.h \
    inceptionresultmodel.h \
    mainwindow.h \
    measurementsettingdialog.h \
    signaltuner.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    cameradetectdialog.ui \
    defectinceptiondialog.ui \
    grascaletunerdia.ui \
    measurementsettingdialog.ui \
    signaltuner.ui



win32{
CUDA_SOURCES += kernel.cu

CUDA_DIR = "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.2"
SYSTEM_NAME = x64
SYSTEM_TYPE = 64
CUDA_ARCH = compute_86
CUDA_CODE = sm_86
NVCC_OPTIONS = --use_fast_math

# header include path
INCLUDEPATH += "$$CUDA_DIR/include" \
"C:\ProgramData\NVIDIA Corporation\CUDA Samples\v11.2\common\inc"

# lib path
QMAKE_LIBDIR += "$$CUDA_DIR/lib/x64"
CUDA_INC = $$join(INCLUDEPATH,'" -I"','-I"','"')

# libs
CUDA_LIB_NAMES += \cuda \cudadevrt \cudart \cufft \cublas \

for(lib, CUDA_LIB_NAMES) {
    CUDA_LIBS += $$lib.lib
}
for(lib, CUDA_LIB_NAMES) {
    NVCC_LIBS += -l$$lib
}
LIBS += $$NVCC_LIBS

MSVCRT_LINK_FLAG_DEBUG   = "/MDd"
MSVCRT_LINK_FLAG_RELEASE = "/MD"

CONFIG(debug, debug|release) {
    # Debug mode
    OBJECTS_DIR = debug/obj
    CUDA_OBJECTS_DIR = debug/cuda
    cuda_d.input = CUDA_SOURCES
    cuda_d.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}_cuda.o
    cuda_d.commands = $$CUDA_DIR/bin/nvcc.exe -D_DEBUG $$NVCC_OPTIONS $$CUDA_INC $$LIBS \
                      --machine $$SYSTEM_TYPE -arch=$$CUDA_ARCH -code=$$CUDA_CODE --fmad=true \
                      --compile -cudart static -g -DWIN32 -D_MBCS -use_fast_math \
                      -Xptxas -dlcm=cg \
                      -Xcompiler "/wd4819,/EHsc,/W3,/nologo,/Od,/Zi,/RTC1" \
                      -Xcompiler $$MSVCRT_LINK_FLAG_DEBUG \
                      -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
    cuda_d.dependency_type = TYPE_C
    QMAKE_EXTRA_COMPILERS += cuda_d
}
else {
    # Release mode
    OBJECTS_DIR = release/obj
    CUDA_OBJECTS_DIR = release/cuda
    cuda.input = CUDA_SOURCES
    cuda.output = $$CUDA_OBJECTS_DIR/${QMAKE_FILE_BASE}_cuda.o
    cuda.commands = $$CUDA_DIR/bin/nvcc.exe $$NVCC_OPTIONS $$CUDA_INC $$LIBS \
                    --machine $$SYSTEM_TYPE -arch=$$CUDA_ARCH -code=$$CUDA_CODE --fmad=true \
                    --compile -cudart static -D_MBCS -use_fast_math \
                    -Xptxas -dlcm=cg \  #cg:禁用一级缓存,粒度为32,ca:启用一级缓存,粒度为128
                    -Xcompiler "/wd4819,/EHsc,/W3,/nologo,/O3,/Zi" \
                    -Xcompiler $$MSVCRT_LINK_FLAG_RELEASE \
                    -c -o ${QMAKE_FILE_OUT} ${QMAKE_FILE_NAME}
    cuda.dependency_type = TYPE_C
    QMAKE_EXTRA_COMPILERS += cuda
}
}

INCLUDEPATH += D:\opencvBuildVS\install\include
win32:CONFIG(release, debug|release):{
LIBS += D:\opencvBuildVS\install\x64\vc16\lib\opencv_*
}
else:win32:CONFIG(debug, debug|release):{
LIBS += D:\opencvBuildVS\install\x64\vc16\lib\opencv_*d.lib
}

INCLUDEPATH += D:\CamExpert\Include
INCLUDEPATH += D:\CamExpert\Examples\Classes\Common
INCLUDEPATH += D:\CamExpert\Classes\Basic

LIBS += D:\CamExpert\Lib\Win64\corapi.lib
LIBS += D:\CamExpert\Lib\Win64\SapClassBasic.lib

RESOURCES += \
    iconSource.qrc

DISTFILES += \
        kernel.cu

