# Keep native methods
-keepclasseswithmembernames class * {
    native <methods>;
}

# Keep all native methods
-keep class * {
    native <methods>;
}

# Don't warn specific classes
-dontwarn org.slf4j.impl.StaticLoggerBinder
-dontwarn org.slf4j.impl.StaticMarkerBinder
-dontwarn org.slf4j.impl.StaticMDCBinder
-dontwarn java.lang.management.RuntimeMXBean
-dontwarn java.lang.management.ManagementFactory

# Keep all classes in the specified package and its subpackages
-keep class io.github.troppical.** { *; }

# Keep specific interface methods
-keep class io.github.troppical.network.APKDownloader$OnCompleteCallback {
    void onComplete(boolean);
}

-keep class io.github.troppical.network.APKDownloader$ProgressCallback {
    void onProgress(int);
}
