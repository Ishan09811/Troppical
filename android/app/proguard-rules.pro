-keepclasseswithmembernames class * {
    native <methods>;
}

-keep class * {
    native <methods>;
}

-dontwarn org.slf4j.impl.StaticLoggerBinder
-dontwarn org.slf4j.impl.StaticMarkerBinder
-dontwarn org.slf4j.impl.StaticMDCBinder
-dontwarn java.lang.management.RuntimeMXBean
-dontwarn java.lang.management.ManagementFactory

# Keep all classes in the specified package and its subpackages
-keep class io.github.troppical.** {*;}
