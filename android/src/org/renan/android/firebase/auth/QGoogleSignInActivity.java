package org.renan.android.firebase.auth;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import org.qtproject.qt5.android.bindings.QtActivity;

/**
 * Created by stan on 18/04/2018.
 */

public class QGoogleSignInActivity extends QtActivity
{
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data)
    {
        super.onActivityResult(requestCode, resultCode, data);
        Log.i("onActivityResult", "requestCode="+requestCode+", int resultCode="+resultCode+", Intent data="+data.toString());
    }
}
