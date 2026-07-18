// Default sanitizer runtime options baked into the binary itself, so they
// apply no matter how it is invoked (ctest --preset, bare ctest, or running
// the executable directly) rather than only when the CMakePresets.json
// environment happens to be in effect.
//
// LeakSanitizer's exit-time "stop the world" thread-suspension deadlocks
// whenever a QApplication in this binary spins up GLib helper threads (dconf
// worker, gdbus, pool-spawner) to watch the live desktop theme - which only
// happens on a host with a real desktop session (see GitHub issue #140,
// where this hung the demo app on exit). detect_leaks=0 matches the fact
// that aide_ENABLE_SANITIZER_LEAK is already OFF: this project never wanted
// a leak sanitizer, but ASan's built-in LSan integration runs by default
// unless told otherwise.
#if defined(__has_feature)
#if __has_feature(address_sanitizer)
#define AIDE_ASAN_BUILD
#endif
#endif
#if defined(__SANITIZE_ADDRESS__)
#define AIDE_ASAN_BUILD
#endif

#if defined(AIDE_ASAN_BUILD)

extern "C" const char* __asan_default_options() // NOLINT
{
    return "detect_odr_violation=0:detect_stack_use_after_return=1:detect_"
           "leaks=0";
}

#endif
