# Delete stale gcov .gcda files before a coverage-enabled test run.
#
# gcov/--coverage merges execution counts into existing .gcda files on every
# process exit. Because catch_discover_tests runs each Catch2 TEST_CASE as its
# own process of the same instrumented binary, a process that is killed
# mid-write (a ctest timeout, a sanitizer abort) leaves a truncated .gcda
# behind. Any later run then crashes in llvm_gcda_emit_function while trying to
# merge that corrupt file ("cannot merge previous GCDA file: corrupt arc tag"),
# and stays broken on every subsequent run until the files are deleted by hand.
#
# Removing the .gcda files up front guarantees each run starts from clean,
# self-consistent coverage data. Counts still accumulate correctly across the
# per-TEST_CASE processes of a single ctest invocation.
if(NOT DEFINED GCDA_ROOT)
  message(FATAL_ERROR "ResetCoverageData: GCDA_ROOT must be defined")
endif()

file(GLOB_RECURSE gcda_files "${GCDA_ROOT}/*.gcda")
list(LENGTH gcda_files gcda_count)
if(gcda_count GREATER 0)
  file(REMOVE ${gcda_files})
endif()
message(STATUS "ResetCoverageData: removed ${gcda_count} stale .gcda file(s)")
