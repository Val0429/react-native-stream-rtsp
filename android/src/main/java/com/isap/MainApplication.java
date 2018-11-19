package com.isap;

import android.app.Activity;
import android.content.Context;

/**
 * Created by Neo on 2018/11/9.
 */

public class MainApplication {

    private static Activity mActivity;
    private static Context mContext;

    public static void setCurrentActivity(Activity activity) {
        mActivity = activity;
    }

    public static Activity getCurrentActivity() {
        return mActivity;
    }

    public static void setActivity(Activity activity) {
        mActivity = activity;
    }

    public static Activity getActivity() {
        return mActivity;
    }

    public static Context getAppContext() {
        return MainApplication.mContext;
    }
}
