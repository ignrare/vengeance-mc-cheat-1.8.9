#include "JNIHelpers.h"
#include "../Globals/Globals.h"

jfieldID TryFieldN(JNIEnv *e, jclass c, const char **names, int count,
                   const char *sig) {
  for (int i = 0; i < count; i++) {
    jfieldID f = e->GetFieldID(c, names[i], sig);
    if (f)
      return f;
    e->ExceptionClear();
  }
  return nullptr;
}

jmethodID TryMethodN(JNIEnv *e, jclass c, const char **names, int count,
                     const char *sig) {
  for (int i = 0; i < count; i++) {
    jmethodID m = e->GetMethodID(c, names[i], sig);
    if (m)
      return m;
    e->ExceptionClear();
  }
  return nullptr;
}

jmethodID TryStaticMethodN(JNIEnv *e, jclass c, const char **names, int count,
                           const char *sig) {
  for (int i = 0; i < count; i++) {
    jmethodID m = e->GetStaticMethodID(c, names[i], sig);
    if (m)
      return m;
    e->ExceptionClear();
  }
  return nullptr;
}

jclass LoadMCClass(JNIEnv *env, const char **dotNames, int nameCount) {
  if (!g_mcClassLoader || !g_loadClassMethod)
    return nullptr;
  for (int i = 0; i < nameCount; i++) {
    jstring jName = env->NewStringUTF(dotNames[i]);
    jclass cls = (jclass)env->CallObjectMethod(g_mcClassLoader,
                                               g_loadClassMethod, jName);
    env->DeleteLocalRef(jName);
    if (cls && !env->ExceptionCheck())
      return cls;
    env->ExceptionClear();
  }
  return nullptr;
}

bool FindMCClassLoader(JNIEnv *env, const char *testClassName) {
  jclass threadClass = env->FindClass("java/lang/Thread");
  jclass mapClass = env->FindClass("java/util/Map");
  jclass setClass = env->FindClass("java/util/Set");
  jclass clLoaderClass = env->FindClass("java/lang/ClassLoader");
  if (!threadClass || !mapClass || !setClass || !clLoaderClass) {
    env->ExceptionClear();
    return false;
  }

  jmethodID getAllTraces = env->GetStaticMethodID(
      threadClass, "getAllStackTraces", "()Ljava/util/Map;");
  jmethodID keySet = env->GetMethodID(mapClass, "keySet", "()Ljava/util/Set;");
  jmethodID toArray =
      env->GetMethodID(setClass, "toArray", "()[Ljava/lang/Object;");
  jmethodID getContextCL = env->GetMethodID(
      threadClass, "getContextClassLoader", "()Ljava/lang/ClassLoader;");
  g_loadClassMethod = env->GetMethodID(clLoaderClass, "loadClass",
                                       "(Ljava/lang/String;)Ljava/lang/Class;");

  if (!getAllTraces || !keySet || !toArray || !getContextCL ||
      !g_loadClassMethod) {
    env->ExceptionClear();
    return false;
  }

  jobject traceMap = env->CallStaticObjectMethod(threadClass, getAllTraces);
  if (!traceMap || env->ExceptionCheck()) {
    env->ExceptionClear();
    return false;
  }

  jobject keys = env->CallObjectMethod(traceMap, keySet);
  jobjectArray threadArr = (jobjectArray)env->CallObjectMethod(keys, toArray);
  env->DeleteLocalRef(keys);
  env->DeleteLocalRef(traceMap);
  if (!threadArr) {
    env->ExceptionClear();
    return false;
  }

  jstring testName = env->NewStringUTF(testClassName);
  jint threadCount = env->GetArrayLength(threadArr);

  for (jint i = 0; i < threadCount; i++) {
    jobject thread = env->GetObjectArrayElement(threadArr, i);
    if (!thread)
      continue;

    jobject cl = env->CallObjectMethod(thread, getContextCL);
    env->DeleteLocalRef(thread);
    if (!cl || env->ExceptionCheck()) {
      env->ExceptionClear();
      continue;
    }

    jclass mcTest =
        (jclass)env->CallObjectMethod(cl, g_loadClassMethod, testName);
    if (mcTest && !env->ExceptionCheck()) {
      g_mcClassLoader = env->NewGlobalRef(cl);
      g_mcClass = (jclass)env->NewGlobalRef(mcTest);
      env->DeleteLocalRef(mcTest);
      env->DeleteLocalRef(cl);
      env->DeleteLocalRef(testName);
      env->DeleteLocalRef(threadArr);
      env->DeleteLocalRef(threadClass);
      env->DeleteLocalRef(mapClass);
      env->DeleteLocalRef(setClass);
      env->DeleteLocalRef(clLoaderClass);
      return true;
    }
    env->ExceptionClear();
    env->DeleteLocalRef(cl);
  }

  env->DeleteLocalRef(testName);
  env->DeleteLocalRef(threadArr);
  env->DeleteLocalRef(threadClass);
  env->DeleteLocalRef(mapClass);
  env->DeleteLocalRef(setClass);
  env->DeleteLocalRef(clLoaderClass);
  return false;
}
