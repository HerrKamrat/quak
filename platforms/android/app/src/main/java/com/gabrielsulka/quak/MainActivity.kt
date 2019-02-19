package com.gabrielsulka.quak

import android.content.Context
import android.content.res.AssetManager
import android.os.Bundle
import android.util.Log
import org.libsdl.app.SDLActivity
import java.io.File
import java.io.FileWriter


class MainActivity : SDLActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setNativeAssetManger(getAssets())
    }

    override fun onDestroy() {
        setNativeAssetManger(null)
        super.onDestroy()
    }

    external fun setNativeAssetManger(assets: AssetManager?);

    override fun getLibraries(): Array<String> {
        return arrayOf(
            "SDL2",
            // "SDL2_image",
            // "SDL2_mixer",
            // "SDL2_net",
            // "SDL2_ttf",
            // "main"
            "quak"
        )
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String
}
