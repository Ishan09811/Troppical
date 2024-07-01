-keepclasseswithmembernames class * {
    native <methods>;
}

-keep class * {
    native <methods>;
}

-dontwarn org.slf4j.impl.StaticLoggerBinder

# Keep all classes in the specified package and its subpackages
-keep class io.github.troppical.** {*;}
