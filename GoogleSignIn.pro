QT += quick androidextras svg
CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    qgooglesigninapplication.cpp \
    qmaincontroller.cpp \
    controller.cpp \
    qfirebase.cpp \
    qauthgsi.cpp \
    qgsijnicallbacks.cpp \
    androidhelper.cpp \
    quser.cpp

RESOURCES += qml.qrc

# Locally defined variables.
include(local_defines.pri)

# Additional import path used to resolve QML modules in Qt Creator's code model
# MODIFY this value according to your configuration
QML_IMPORT_PATH = $${LOCAL_QML_IMPORT_PATH}

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/gradle.properties \
    android/local.properties \
    android/src/org/renan/android/firebase/auth/QGoogleSignIn.java \
    android/src/org/renan/android/firebase/auth/QGoogleSignInActivity.java

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

HEADERS += \
    qgooglesigninapplication.h \
    qmaincontroller.h \
    controller.h \
    qfirebase.h \
    qauthgsi.h \
    androidhelper.h \
    helper.h \
    quser.h \
    androidmessagehandler.h

# Firebase
# MODIFY this value according to your configuration
FIREBASE_VERSION=$${LOCAL_FIREBASE_VERSION}
# MODIFY this value according to your configuration
FIREBASE_SDK_PATH=$${LOCAL_FIREBASE_SDK_PATH}

isEmpty(FIREBASE_SDK_PATH) {
    warning("Please set FIREBASE_SDK_PATH to the Firebase SDK path")
}

LIBRARY_INFIX=""
versionAtLeast(FIREBASE_VERSION, 5) {
LIBRARY_INFIX="firebase_"
}

INCLUDEPATH += \
    $$FIREBASE_SDK_PATH/include \
    \

# NOTE the order of linking is important!
android {
    FIREBASE_SDK_LIBS_PATH = $$FIREBASE_SDK_PATH/libs/android/$$ANDROID_TARGET_ARCH/gnustl
    DEPENDPATH += $$FIREBASE_SDK_LIBS_PATH
}

# Firebase common
PRE_TARGETDEPS += $$FIREBASE_SDK_LIBS_PATH/lib$${LIBRARY_INFIX}app.a
LIBS += -L$$FIREBASE_SDK_LIBS_PATH -l$${LIBRARY_INFIX}app

# Firebase auth
PRE_TARGETDEPS += $$FIREBASE_SDK_LIBS_PATH/lib$${LIBRARY_INFIX}auth.a
LIBS += -L$$FIREBASE_SDK_LIBS_PATH -l$${LIBRARY_INFIX}auth

# Once again (only needed when having xref libraries)
#LIBS += -L$$FIREBASE_SDK_LIBS_PATH -l$${LIBRARY_INFIX}app

# Clumsy optionnal OpenSSL library inclusion
contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    POTENTIAL_ANDROID_EXTRA_LIBS = "$$OPEN_SSL_LIBS_PATH/libssl.so" "$$OPEN_SSL_LIBS_PATH/libcrypto.so"
    ACTUAL=true
    for (file, POTENTIAL_ANDROID_EXTRA_LIBS) {
        !exists($$file) {
            message("$$file does not exist")
            ACTUAL=false
        }
    }
    if ($$ACTUAL) {
            # This « solution » for Openssl warnings makes my App crash.
            # Commented out until I find out why and can fix it.
            #ANDROID_EXTRA_LIBS += $$POTENTIAL_ANDROID_EXTRA_LIBS
            message("Using Android extra libs : $$ANDROID_EXTRA_LIBS")
    }
}
