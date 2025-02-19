param (
    [Parameter(Mandatory=$false)]
    [Switch]$debug_so,
    [Parameter(Mandatory=$false)]
    [Switch]$log,
    [Parameter(Mandatory=$false)]
    [Switch]$NoStart
)

& $PSScriptRoot/build.ps1
if (-not ($LastExitCode -eq 0)) {
    echo "build failed, not copying"
    exit
}

if ($debug_so.IsPresent) {
    & adb push build/debug/libmpchat.so /sdcard/ModData/com.beatgames.beatsaber/Modloader/mods/libmpchat.so
} else {
    & adb push build/libmpchat.so /sdcard/ModData/com.beatgames.beatsaber/Modloader/mods/libmpchat.so
}

& adb shell am force-stop com.beatgames.beatsaber
if ($NoStart.IsPresent) {
    exit
}
& adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
Start-Sleep -Seconds 1.0
& adb shell am start com.beatgames.beatsaber/com.unity3d.player.UnityPlayerActivity
if ($log.IsPresent) {
    & $PSScriptRoot/log.ps1
}
