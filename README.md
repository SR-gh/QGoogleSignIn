# QGoogleSignIn
QGoogleSignIn is an Android application demonstrating Google Sign In authentication in Firebase.
It is a prototype built in C++ using Qt and QML.
Parts of QGoogleSignIn are written in Java, in order to directly access the Google Sign In API.
All Firebase code is based upon the Firebase C++ SDK.

# What can I do with it ?
This code allows you to use the following authentication methods with Firebase in C++ :
  * Google Sign In
  * Email + password
  * Anonymous

Linking accounts is also handled.

# Is it production-ready ?
No. Not at all. It is a Work In Progress.
Many use cases can be handled, but not all. There's still work to do.

# How can I use it ?
It can be useful to add Google Sign In authentication to your Android mobile app made with Qt. If you do not code in C++, or not for Android, it may not be useful.

Below, this is **NOT** a step by step build procedure. It should be sufficient to have you getting the app running.

Prerequisites :
* Qt (tested with 5.10.1)
* Android SDK (I use Android Studio and its SDK manager to pick the versions)
* Android NDK (version r10e, from the archives)

Useful :
* QtCreator for C++ editing (tested with 4.6.0-rc1)
* Android Studio for Java editing (tested with 3.1.3)

Steps to build :
* download firebase SDK, install it anywhere (tested with 5.1.0)
* gradle should download automatically Java dependencies
* you need to create a Firebase project, and add your client application to it
* you need to allow Google Sign In authentication on your project
* you have to download you `google-services.json` file in the android directory
* you have to set the correct paths and versions in `local.properties` and `gradle.properties` (using `local.properties.template` and `gradle.properties.template` if you want)
* you also have to set the correct path in the `local_defines.pri` file (using `local_defines.pri.template` if you want)

When updating from this repository, check for changes in the template files, to see if you should modify something.
