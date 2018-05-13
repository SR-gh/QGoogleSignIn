QT += quick androidextras
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
    androidhelper.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH = D:/Qt/5.10.1

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
    helper.h

# Firebase
QTFIREBASE_SDK_PATH=D:/Android/firebase_cpp_sdk_4.4.1

isEmpty(QTFIREBASE_SDK_PATH) {
    warning("Please set QTFIREBASE_SDK_PATH to the Firebase SDK path")
}

INCLUDEPATH += \
    $$QTFIREBASE_SDK_PATH/include \
    \

# NOTE the order of linking is important!
android {
    QTFIREBASE_SDK_LIBS_PATH = $$QTFIREBASE_SDK_PATH/libs/android/$$ANDROID_TARGET_ARCH/gnustl
    DEPENDPATH += $$QTFIREBASE_SDK_LIBS_PATH
}

# Firebase common
PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/libapp.a
LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -lapp

# Firebase auth
PRE_TARGETDEPS += $$QTFIREBASE_SDK_LIBS_PATH/libauth.a
LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -lauth

# Once again (je ne sais pas si c'est obligatoire, mais ça ne fait pas de mal)
LIBS += -L$$QTFIREBASE_SDK_LIBS_PATH -lapp
