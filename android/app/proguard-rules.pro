-keepclasseswithmembernames class * {
    native <methods>;
}

-keep class * {
    native <methods>;
}

# Keep all classes in the specified package and its subpackages
-keep class io.github.troppical.** {*;}
