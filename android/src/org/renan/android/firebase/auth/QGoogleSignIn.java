package org.renan.android.firebase.auth;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;

import org.renan.android.dev.gsi.R;

/**
 * Created by stan on 10/03/2018.
 */

public class QGoogleSignIn
{
    public final static String WAIT_FOR_ASYNC_ANSWER = "wait_async";
    public final static int ERROR_INVALID_ACCOUNT = 0x10001;
    public final static int ERROR_INVALID_TOKEN = 0x10002;

    // TODO : return a more complex answer, if necessary
    public static String getTokenIdFromSignedAccount(Context context)
    {
        assert (null != context);
        String token = "";
        final GoogleSignInAccount account = GoogleSignIn.getLastSignedInAccount(context);
        if (null != account)
        {
            token = account.getIdToken();
            if (null == token)
                token = "";
        }
        assert (null != token);
        return token;
    }

    // TODO : return a more complex answer, if necessary
    public static String refreshToken(final Context context, final boolean silently)
    {
        String token = "";
        // TODO : extend request to other things
        final GoogleSignInOptions gso = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN)
                .requestEmail()
                .requestIdToken(context.getString(R.string.default_web_client_id))
                .build();
        if (null == gso)
            return "";
        final GoogleSignInClient gsiClient = GoogleSignIn.getClient(context, gso);
        if (null == gsiClient)
            return "";
        final Task<GoogleSignInAccount> task = gsiClient.silentSignIn();
        if (task.isSuccessful())
        {
            Log.i("refreshToken", "Immediate successful silentSignIn()");
            final GoogleSignInAccount account = task.getResult();
            if (null != account)
            {
                token = account.getIdToken();
                if (null == token)
                    token = "";
            }
        }
        else
        {
            task.addOnCompleteListener(new OnCompleteListener<GoogleSignInAccount>()
            {
                @Override
                public void onComplete(Task<GoogleSignInAccount> task)
                {
                    try
                    {
                        Log.i("refreshToken.onComplete", "Delayed result from silentSignIn()");
                        final GoogleSignInAccount account = task.getResult(ApiException.class);
                        if (null == account)
                            onFailedRefresh(ERROR_INVALID_ACCOUNT, silently);
                        else
                        {
                            final String token = account.getIdToken();
                            if (null == token)
                                onFailedRefresh(ERROR_INVALID_TOKEN, silently);
                            else
                                onSuccessfulRefresh(token);
                        }
                    } catch (ApiException apiException)
                    {
                        Log.w("refreshToken.onComplete", "error, statusCode=" + apiException.getStatusCode());
                        // Please refer to GoogleSignInStatusCodes Javadoc for details
                        onFailedRefresh(apiException.getStatusCode(), silently);
                    }
                }
            });
            // We inform the caller that an answer is to be received
            // Important : the answer could be received before this method
            // returns. Consequently it should be interpreted as "do nothing,
            // it is handled elsewhere".
            token = WAIT_FOR_ASYNC_ANSWER;
        }
        assert (null != token);
        return token;
    }

//    public static boolean tokenIsWaitForAnswer(String token)
//    {
//        return 0 == WAIT_FOR_ASYNC_ANSWER.compareTo(token);
//    }
//
    public static native void onSuccessfulRefresh(String token);
    public static native void onFailedRefresh(int statusCode, boolean silently);
    public static native void onSuccessfulSignOut();

    public static Intent getGSIIntent(Context context)
    {
        assert (null != context);
        assert (context instanceof Activity);
        // TODO : extend request to other things
        final GoogleSignInOptions gso = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN)
                .requestEmail()
                .requestIdToken(context.getString(R.string.default_web_client_id))
                .build();
        if (null == gso)
            return null;
        final GoogleSignInClient gsiClient = GoogleSignIn.getClient(context, gso);
        if (null == gsiClient)
            return null;
        final Intent signInIntent = gsiClient.getSignInIntent();
        return signInIntent;
    }

    // TODO : return a more complex answer, if necessary
    // 1- data
    // 2- error code from the exception in order to handle various cases
    // https://stackoverflow.com/questions/12325860/jni-getting-exception-info-attempted-to-read-or-write-protected-memory
    // or a big object including an error code
    public static String handleSignInResult(Intent data)
    {
        assert (null != data);
        assert (data instanceof Intent);
        String token = "";
        // The Task returned from this call is always completed, no need to attach
        // a listener.
        // See: https://developers.google.com/identity/sign-in/android/
        final Task<GoogleSignInAccount> task = GoogleSignIn.getSignedInAccountFromIntent(data);
        try
        {
            final GoogleSignInAccount account = task.getResult(ApiException.class);
            token = account.getIdToken();

        }
        catch (ApiException e)
        {
            // The ApiException status code indicates the detailed failure reason.
            // Please refer to the GoogleSignInStatusCodes class reference for more information.
            Log.w("handleSignInResult", "signInResult:failed code=" + e.getStatusCode());
            Log.w("handleSignInResult", e);
        }

        assert (null != token);
        return token;
    }

    // return false if it did not succeed
    // return true if sign out of GSI completed
    // may return true if sign out did not succeed yet, but is still in progress
    // we do not distinguish the two cases.
    // Behaviour intended : do nothing until it returns true either directly
    // or with its callback when the Task is finished.
    public static boolean signOut(final Context context)
    {
        String token = "";
        // TODO : extend request to other things
        final GoogleSignInOptions gso = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN)
                .requestEmail()
                .requestIdToken(context.getString(R.string.default_web_client_id))
                .build();
        if (null == gso)
            return false;
        final GoogleSignInClient gsiClient = GoogleSignIn.getClient(context, gso);
        if (null == gsiClient)
            return false;
        final Task<Void> task = gsiClient.signOut();
        if (task.isSuccessful())
        {
            Log.i("signOut", "Immediate successful signOut() from GSI");
            return true;
        }
        else
        {
            task.addOnCompleteListener(new OnCompleteListener<Void>()
            {
                @Override
                public void onComplete(Task<Void> task)
                {
                    Log.i("signOut.onComplete", "Future signOut() completed");
                    onSuccessfulSignOut();
                }
            });
        }
        return false;
    }
}
