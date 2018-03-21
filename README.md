# QGoogleSignIn
This project prototypes a Qt application (in QML) using the Google Sign In authentication with Firebase, on Android.
It uses a bunch of simple techniques in order to put pieces of the authentication workflow alltogether.

# What can I do with it ?
Try it :)

Below, this is **NOT** a step by step build procedure. It should be, but it isn't.

Prerequisites :
* Qt (tested with 5.10.1)
* Android SDK (I use Android Studio and its SDK manager to pick the versions)
* Android NDK (version r10e, from the archives)

Useful :
* QtCreator for C++ editing (tested with 4.5.1)
* Android Studio for Java editing (tested with 3.0.1)

Steps to build :
* download firebase SDK, install it anywhere
* gradle should download automatically Java dependencies
* you need to create a Firebase project, and add your application to it
* you need to allow Google Sign In authentication on your project
* you have to download you `google-services.json` file in the android directory
* you have to set the correct paths and versions in `local.properties` and `gradle.properties` (yes, these files should not be as is in the VCS)
* you also have to set the correct path in the `.pro` file


