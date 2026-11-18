# Help Coverage Test

`mkdocs build --strict` with `check_paths: true` proves that every stub's include exists. It does not prove that the C++ side and the site agree: a new settings page whose `helpTopicId()` has no page, or a remap to a path where nothing is published, still builds. A **help coverage test** closes that gap. aIDE provides the list of topics (`Application::declaredHelpTopics()`); the test itself lives in your project.

The reference is the demo's test, [`demo/tests/demohelptest.cpp`](https://github.com/mrpilot2/aide/blob/develop/demo/tests/demohelptest.cpp), test case *"Every demo Help Topic has a page in the demo help site"*.

---

## The recipe

1. **Build the application through the same setup as `main()`.** Factor the configuration and setup out of `main()` into functions the test can call, so the test sees exactly the settings pages, declared topics, remaps and disabled topics the real application has. The demo uses `demo::demoConfig()` for the `ApplicationConfig` and `demo::setUpDemo(app)` for everything after construction:

    ```cpp
    // main.cpp
    const aide::Application app(argc, argv, demo::demoConfig());
    const auto colorSchemeReactor = demo::setUpDemo(app);
    return aide::Application::exec();
    ```

    Pass a test URL launcher in tests (the demo's `demoConfig()` takes an optional `aide::UrlLauncherPtr`) so no test opens a real browser.

2. **Check every ID in `declaredHelpTopics()` has a page in the built site.** With directory URLs, topic `<id>` is served from `<site dir>/<id>/index.html`. Call `declaredHelpTopics()` after your settings pages are registered, and remember to [declare every widget topic](index.md#declaring-widget-topics).

3. **Skip when no site directory is given.** Read the site directory from an environment variable. When it is unset, skip, so ordinary test runs don't need Python and mkdocs. In CI, build the site first and set the variable.

```cpp
TEST_CASE("Every Help Topic has a page in the help site", "[Help]")
{
    const auto siteDir = qEnvironmentVariable("MYAPP_HELP_SITE_DIR");
    if (siteDir.isEmpty()) {
        SKIP("MYAPP_HELP_SITE_DIR is not set; build the help site first");
    }

    // Built exactly like main() builds it.
    int argc{1};
    std::array<char*, 1> argv{{const_cast<char*>("myapp")}};
    aide::Application app(argc, argv.data(), myapp::config(testLauncher));
    const auto setup = myapp::setUp(app);

    QStringList missing;
    for (const auto& topic : app.declaredHelpTopics()) {
        if (!QFileInfo(QDir(siteDir).filePath(topic.value() + "/index.html"))
                 .isFile()) {
            missing << topic.value();
        }
    }

    INFO("Help Topics without a page: " << missing.join(", ").toStdString());
    REQUIRE(missing.isEmpty());
}
```

`aide::Application` is itself a `QApplication`, so put this test in an executable that does not create its own `QApplication` (the demo uses a separate `ut.aide.demo.application` executable for this). Set the application and organization names before constructing it, as `main()` does.

---

## Running it

Build the site, then run the test with the site directory set. For the demo, from the aIDE repository root:

```bash
AIDE_HELP_DIR="$PWD/aide/help" build/help-venv/bin/mkdocs build --strict \
  -f demo/help/mkdocs.yml -d "$PWD/build/demo-help-site"
AIDE_DEMO_HELP_SITE_DIR="$PWD/build/demo-help-site" \
  ./build/client-unix-static-debug/bin/ut.aide.demo.application
```

A missing page fails with the list of topics, e.g. `Help Topics without a page: keymap`.

!!! warning "Guard against a silent skip in CI"
    A skipped test is green. In the CI job that builds the site, run the test binary directly on this test case and fail on any non-zero exit code: Catch2 exits with code 4 when every selected test was skipped. Under `ctest`, `catch_discover_tests` reports the skip as *Skipped*, not as a failure.
