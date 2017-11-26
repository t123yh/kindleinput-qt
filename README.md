# kindleinput-qt
A PlatformInputContextPlugin to use Kindle's native keyboard on Qt apps.

Inspired by https://www.kdab.com/qt-input-method-virtual-keyboard/.

## Build
* Install Qt SDK for Kindle.
* Make sure there is `openlipc.h` available for your Kindle toolchain.
* Import the project to Qt Creator.
* Build it.
* You can find the plugin at `<qt-sdk-path>/plugins/platforminputcontexts/libkindleplatforminputcontextplugin.so`. Copy it to the corresponding directory on Kindle.
