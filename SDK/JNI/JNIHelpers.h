#pragma once
#include <jni.h>

jfieldID TryFieldN(JNIEnv *e, jclass c, const char **names, int count,
                   const char *sig);

jmethodID TryMethodN(JNIEnv *e, jclass c, const char **names, int count,
                     const char *sig);

jmethodID TryStaticMethodN(JNIEnv *e, jclass c, const char **names, int count,
                           const char *sig);

jclass LoadMCClass(JNIEnv *env, const char **dotNames, int nameCount);

bool FindMCClassLoader(JNIEnv *env, const char *testClassName);

bool InitJNIReach(JNIEnv **envOut);
