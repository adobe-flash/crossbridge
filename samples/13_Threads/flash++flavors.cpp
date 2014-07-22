// Example of the functionality in <Flash++.h>

#include <pthread.h>

#include <Flash++.h>

static void *tryGetClipboardLocal(void *) {
  using namespace AS3::local; // use Worker-local wrapper classes

  try {
    flash::desktop::Clipboard clip = flash::desktop::Clipboard::generalClipboard;
    return (void *)internal::utf8_toString(clip);
  } catch(var e) {
    return (void *)internal::utf8_toString(e);
  }
  // unreachable
}

static void *tryGetClipboardUI(void *) {
  using namespace AS3::ui; // use auto-delegating wrapper classes

  try {
    flash::desktop::Clipboard clip = flash::desktop::Clipboard::generalClipboard;
    return (void *)internal::utf8_toString(clip);
  } catch(var e) {
    return (void *)internal::utf8_toString(e);
  }
  // unreachable
}

static void *tryGetClipboardLocalThunk(void *) {
  return avm2_ui_thunk(tryGetClipboardLocal, NULL);
}

static void printAndFreeStatus(const char *context, void *result) {
  printf("%s trying to get flash.desktop.Clipboard.generalClipboard: %s\n", context, (char *)result);
  free(result);
}

int main() {
  // try to get Clipboard in our thread -- should fail!
  printAndFreeStatus("main (local)", tryGetClipboardLocal(NULL));
  // try to get Clipboard in our thread using "AS3::ui" -- should succeed!
  printAndFreeStatus("main (ui)", tryGetClipboardUI(NULL));
  // try to get Clipboard in the ui Worker -- should succeed!
  printAndFreeStatus("main via avm2_ui_thunk (local)", tryGetClipboardLocalThunk(NULL));

  void *threadResult;
  pthread_t thread;

  // try to get Clipboard in a new pthread -- should fail!
  pthread_create(&thread, NULL, tryGetClipboardLocal, NULL);
  pthread_join(thread, &threadResult);
  printAndFreeStatus("pthread (local)", threadResult);
  // try to get Clipboard in a new pthread using "AS3::ui" -- should succeed!
  pthread_create(&thread, NULL, tryGetClipboardUI, NULL);
  pthread_join(thread, &threadResult);
  printAndFreeStatus("pthread (ui)", threadResult);
  // try to get Clipboard in a new pthread using "AS3::local" via avm2_ui_thunk -- should succeed!
  pthread_create(&thread, NULL, tryGetClipboardLocalThunk, NULL);
  pthread_join(thread, &threadResult);
  printAndFreeStatus("pthread via avm2_ui_thunk (local)", threadResult);
  puts("done!");
  return 0;
}
