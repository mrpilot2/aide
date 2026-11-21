# Changelog

## [1.0.0](https://github.com/mrpilot2/aide/compare/v0.3.1...v1.0.0) (2026-11-21)


### ⚠ BREAKING CHANGES

* **notifications:** aide::Notification (aide/include/aide/notification.hpp) now declares Q_DECLARE_METATYPE(aide::Notification), changing the public header's ABI surface for downstream consumers.
* ActionRegistryInterface gains a new pure-virtual menus() accessor (mirroring the existing actions() accessor), needed so KeyMapTreeModel can resolve a group node's QMenu title. Any existing implementation of ActionRegistryInterface must add this method to keep compiling.
* Qt5 is no longer supported. Consumers must build against Qt 6.5 or newer.
* consumers must update includes from <aide/loggerfactory.hpp> to <aide/logger/loggerfactory.hpp> and from <aide/log_helper_macros.hpp> to <aide/logger/log_helper_macros.hpp>.
* **settings:** SettingsPage::matches(const QString&) is inserted as a new virtual method ahead of reset()/apply() in the vtable, shifting the slot index of every virtual member declared after it. Consumer binaries linking against a prebuilt aIDE that subclass SettingsPage must be recompiled against this header; mixing old and new binaries is an ABI break, not just a source-compatible addition.
* SettingsPageRegistry is no longer a static singleton. It is now an instance owned by ApplicationBuilder; access it via Application::settingsPageRegistry(). The static addPage()/settingsPages() methods and deleteAllPages() have been removed.
* **Logger:** C++20 in external header
* const functions in SearchFilterWidget and Application

### Features

* **AboutDialog:** use standard link color ([e24bc82](https://github.com/mrpilot2/aide/commit/e24bc8249bd1dc78b722ca3dcc0f9b67d5696da9))
* add opt-in "Show Log in File Manager" Help-menu action ([9013ec5](https://github.com/mrpilot2/aide/commit/9013ec5817f599ad124fdfdf139dd059b65fd320))
* **appearance:** add AppearanceManager, themes, icon integration, and settings page ([f05e27e](https://github.com/mrpilot2/aide/commit/f05e27ebb2e80a048bc59a587e356e25a4789ecf))
* **appearance:** add AppearancePage settings page ([6a09ed7](https://github.com/mrpilot2/aide/commit/6a09ed733a41f9f9143b53f7f08cb0d4045117fd))
* **appearance:** add icon integration and MainWindow icon refresh ([91dcb2c](https://github.com/mrpilot2/aide/commit/91dcb2c183c25be2f668bb9476a98c395786c6ac))
* **appearance:** add Theme struct and AppearanceManager core ([fe09f28](https://github.com/mrpilot2/aide/commit/fe09f28116aa0d1e2a9e5dc62d60b0527112d7d0))
* **appearance:** react to OS dark/light mode changes at runtime ([a64d27a](https://github.com/mrpilot2/aide/commit/a64d27ac7befa18e50b7d3ed2a7897f40cd4ea26))
* **appearance:** update demo app to use instance-based appearance and settings APIs ([c89cb83](https://github.com/mrpilot2/aide/commit/c89cb83a00be0988a9ab4711f554a7dd7d057b2a))
* **appearance:** wire AppearanceManager into ApplicationBuilder and Application ([bc751e0](https://github.com/mrpilot2/aide/commit/bc751e0bb9c4fbb155b1edf50cdfdf528de52a34))
* **Bug Report:** fix ([0dd77a9](https://github.com/mrpilot2/aide/commit/0dd77a9daf4eda4646fa90ee35a0be599a71d53b))
* **Bug Report:** issue template config ([5dea15e](https://github.com/mrpilot2/aide/commit/5dea15e77012444aa8296f0215c050e6696446b1))
* **Bug Report:** Update bug.yml ([86ceea9](https://github.com/mrpilot2/aide/commit/86ceea9f8360bcb364e45e1d4b8bcdd0418a94bc))
* **config:** add ApplicationConfig feature toggles and gated Full Screen action ([e5aa07b](https://github.com/mrpilot2/aide/commit/e5aa07ba5180aa5b07bc725d25db34aeff94ea56)), closes [#113](https://github.com/mrpilot2/aide/issues/113) [#114](https://github.com/mrpilot2/aide/issues/114) [#115](https://github.com/mrpilot2/aide/issues/115)
* **demo:** add "Got it" tooltip controls to the notification launcher ([6733391](https://github.com/mrpilot2/aide/commit/6733391a46a23f8aa19f212319744e1398a6aa5c))
* **demo:** add dialog banner controls to the notification launcher ([ee3e3a9](https://github.com/mrpilot2/aide/commit/ee3e3a9c4551d20e84f46f2e92b4beacae9a5c18))
* **demo:** add editor banner controls to the notification launcher ([c904f0a](https://github.com/mrpilot2/aide/commit/c904f0a10f121fec8fa907756970e01a441d1e68))
* **demo:** add Notifications launcher dialog with balloon controls ([625dcfd](https://github.com/mrpilot2/aide/commit/625dcfd3a0fd29d0d9fe2f0a28b61187d93efdea)), closes [#164](https://github.com/mrpilot2/aide/issues/164)
* **demo:** preview Report Bug tickets instead of filing real ones ([70151ee](https://github.com/mrpilot2/aide/commit/70151ee45adc5dca8d8c2a6cc17658a70797137a)), closes [#139](https://github.com/mrpilot2/aide/issues/139)
* **demo:** ship a distinct icon set for the Demo Blue theme ([d93f5ee](https://github.com/mrpilot2/aide/commit/d93f5eed83939e03a36ca8398975c44ce2653840))
* **demo:** translate Report Bug preview dialog strings ([0fa4146](https://github.com/mrpilot2/aide/commit/0fa4146b1f04ed937b39591f457fb812a9aabafd))
* exact file manager naming on Windows, macOS, and Linux ([4b31868](https://github.com/mrpilot2/aide/commit/4b3186876df197300c1f614ecb385b9df27805f0))
* expose resolved log file path from LoggerFactory ([c1a31a6](https://github.com/mrpilot2/aide/commit/c1a31a6ce5e5fe9bb6b17e6403a37bc47f4020bd)), closes [#129](https://github.com/mrpilot2/aide/issues/129)
* **gui:** add aIDE-owned notification log view with wired settings link ([e3c33d5](https://github.com/mrpilot2/aide/commit/e3c33d52850ab19200f7cc40006e95fdf9760d8b))
* **gui:** widen MainWindowInterface with addBanner/removeBanner ([9ed38dd](https://github.com/mrpilot2/aide/commit/9ed38dd1e831ac564a8672f3600c3f2ae27e3841))
* **help:** add gated Report Bug in aIDE action ([e0a2ac4](https://github.com/mrpilot2/aide/commit/e0a2ac4f6610d2172d27b64f367073cc726c8ac7))
* **help:** let consumers override the Report Bug launcher ([53fa838](https://github.com/mrpilot2/aide/commit/53fa8387c6a9ecbd3afaf6445dc94257fa21c9e7))
* **i18n:** finish German translation and add top 5 world languages ([c770b73](https://github.com/mrpilot2/aide/commit/c770b7346a0c251a6c158cba939507ff5b22e69b))
* **i18n:** finish unfinished translations for notification system ([49aab35](https://github.com/mrpilot2/aide/commit/49aab358ea8a14981f1220b11aa2d631ce0b8044))
* **i18n:** translate "Show Log in %1" for the file manager action ([d9a393b](https://github.com/mrpilot2/aide/commit/d9a393b78df55fc8f514b68c39a7afc7f2ac5ddd))
* **Logger:** add option to disable console logging application wide ([38d06dc](https://github.com/mrpilot2/aide/commit/38d06dc9e7c205d2f92cc493f92fa549084b0eae))
* **Logger:** add option to override log level ([599b423](https://github.com/mrpilot2/aide/commit/599b42358819c65a6ca73b108536f4f64e47de59))
* **Logger:** switch to C++20 std::format ([43c7065](https://github.com/mrpilot2/aide/commit/43c7065fdecef08b858bab937b84f5dbe0450a37))
* **notifications:** add "Got it" one-shot onboarding tooltip ([f56b3cb](https://github.com/mrpilot2/aide/commit/f56b3cbba7fd37a89d906cde0dd9f1e3e2b04feb))
* **notifications:** add AideDialog base with dialog banner slot ([647d570](https://github.com/mrpilot2/aide/commit/647d5703b36321ab91de54699307c9760e99c220)), closes [#151](https://github.com/mrpilot2/aide/issues/151)
* **notifications:** add Banner widget and editor mount ([a83a8c0](https://github.com/mrpilot2/aide/commit/a83a8c07aa3feda006fc43de7ba8c0baf81e684b))
* **notifications:** add core notification model & manager ([5b4ed9f](https://github.com/mrpilot2/aide/commit/5b4ed9fd77539df4d342b3be175e7215529e2d03))
* **notifications:** add notification balloon widget + global placement ([880e5b8](https://github.com/mrpilot2/aide/commit/880e5b8f53b86f1d7f8c06ac6705d42623695695))
* **notifications:** add notification settings page ([0254c03](https://github.com/mrpilot2/aide/commit/0254c03955e8012e6eceb65146e465cbd2d3c32c))
* **notifications:** add notification-view widget ([c07750d](https://github.com/mrpilot2/aide/commit/c07750d0f1914a71c64cf40e4baef31a8e04c433))
* **notifications:** add severity icons + severityColor accessor ([039ee32](https://github.com/mrpilot2/aide/commit/039ee32c719907fa6e7ff04ba89ac2f0fd653015)), closes [#157](https://github.com/mrpilot2/aide/issues/157)
* require Qt 6.5+, drop Qt5 support ([6a11be3](https://github.com/mrpilot2/aide/commit/6a11be334c75da6d75f7eebbd286b5c4f71ef278))
* **settings:** add magnifying-glass history dropdown to search field ([313805a](https://github.com/mrpilot2/aide/commit/313805ad3433aa68ff9fa9b290106d3c34f86fae))
* **settings:** add search field and route pattern to core ([a5a4623](https://github.com/mrpilot2/aide/commit/a5a4623183b24086fd4a682dcd841bbb4d993e73))
* **settings:** also gray out non-text controls on the selected page ([762ce4e](https://github.com/mrpilot2/aide/commit/762ce4e189d524ba5e65546848a54fc6fab2462c))
* **settings:** auto-select best-matching page as the user types ([4f6db34](https://github.com/mrpilot2/aide/commit/4f6db3458c59cd9875aea428a667309a32535212))
* **settings:** clear page highlight when a control is edited ([467332b](https://github.com/mrpilot2/aide/commit/467332bf693fc33a43eb3c139cc9ab58d9ca0c65))
* **settings:** filter settings tree by page content ([1406df4](https://github.com/mrpilot2/aide/commit/1406df45849f5a6577f939b98fc4a0aec912c234))
* **settings:** filter settings tree by page name ([f547c0c](https://github.com/mrpilot2/aide/commit/f547c0cfe509a5e1198a461d079d958f09e6790f))
* **settings:** gray out every non-matching widget by traversal ([e01c053](https://github.com/mrpilot2/aide/commit/e01c053bab3343884b9067459fa0f967f9775246))
* **settings:** gray out non-matching content on the selected page ([4142a5c](https://github.com/mrpilot2/aide/commit/4142a5cf307b2bf9fda2b1c13a0ffd880afd6358))
* **settings:** highlight matching GUI elements on settings page ([1c7961c](https://github.com/mrpilot2/aide/commit/1c7961c8b7badbbba2642931423721505317833f))
* **settings:** match search tree by independent words (OR) ([ad11697](https://github.com/mrpilot2/aide/commit/ad11697c237d1d3d44b05067228879d487b64727))
* **settings:** persist and commit settings-dialog search history ([8d8729e](https://github.com/mrpilot2/aide/commit/8d8729ed3d2d39879124a685b6cbfae0bde7b375))
* **settings:** pin the current settings page while it still matches ([cc0cdd4](https://github.com/mrpilot2/aide/commit/cc0cdd497a02ce8b93c373499ec0cbbf7a842ce7))
* translate settings-page group tree and keymap tree labels ([4d03158](https://github.com/mrpilot2/aide/commit/4d031589c5e13eb16b0c7a4e994b49b2894094ef)), closes [#120](https://github.com/mrpilot2/aide/issues/120)


### Bug Fixes

* **appearance:** honor persisted theme registered after construction ([48e0c3c](https://github.com/mrpilot2/aide/commit/48e0c3c583411cf803709dba798b922edd4362a9))
* **appearance:** initialize icon_themes resource so menu icons render ([1eed0a9](https://github.com/mrpilot2/aide/commit/1eed0a96304a2d894573fcf811b35ca4dc55945e))
* **appearance:** keep built-in icon path searchable for consumer themes ([b638981](https://github.com/mrpilot2/aide/commit/b638981acf6cfbbbee363c90bfc8d52e521516d0))
* **appearance:** own icon_themes resource in AideCore so installed lib resolves icons ([d8c6fe8](https://github.com/mrpilot2/aide/commit/d8c6fe8346b5a57e6ca302249e277c7e465d7c04))
* **appearance:** preserve platform icon paths and fallback so window decorations render ([5bbbc1a](https://github.com/mrpilot2/aide/commit/5bbbc1aa1a6c787ae4c286fab942694ebe4b10c9))
* **appearance:** satisfy newly-enabled clang-tidy checks ([8eb085a](https://github.com/mrpilot2/aide/commit/8eb085a6ace155a85bd81f5e674dd96adf6ebefb))
* block standard locations via file collision, not permission bits ([1de0992](https://github.com/mrpilot2/aide/commit/1de09926b38bcae55cbd79fb8dd8963e7a8e18e0))
* **ci-static:** suppress cppcheck false positive in balloon host test ([b2af991](https://github.com/mrpilot2/aide/commit/b2af991d166720b7b883173e81e6f82f36e9e303))
* **ci-static:** suppress cppcheck false positive in demo main ([c1c5112](https://github.com/mrpilot2/aide/commit/c1c5112b0cd3d5d60c12e2eab82fcfb1afa3e26d))
* **ci:** scope clang-format bot commit to hpp/cpp files ([4a7ac6b](https://github.com/mrpilot2/aide/commit/4a7ac6bafd9f2d0eb8e62722fb3022392455a62a))
* **ci:** scope license-check bot commit to 3rd-party-licenses.md ([efaf2de](https://github.com/mrpilot2/aide/commit/efaf2dee4894456f3cb0228906c81b820ecf6c8b))
* **clang-tidy:** resolve conflict between anonymous-namespace checks ([a99f2be](https://github.com/mrpilot2/aide/commit/a99f2bef77c1ad134fe603237ae64d777e63021d))
* **cmake:** correct malformed BUILD_LOCAL_INTERFACE generator expressions ([f15fa85](https://github.com/mrpilot2/aide/commit/f15fa85c1ff48437f8fdecbd518c792ef2a853ab))
* **cmake:** resolve dev license path via global property instead of relative guess ([724e332](https://github.com/mrpilot2/aide/commit/724e3324335970d0c0b7883e4009674b7e918a58))
* **demo:** resolve sanitizerdefaults.cpp relative to the CMakeLists.txt ([463765b](https://github.com/mrpilot2/aide/commit/463765b0d178d8ce2e1333befb734d2fdc63467a))
* **demo:** set text roles in Demo Blue palette so item views stay readable ([bb05db3](https://github.com/mrpilot2/aide/commit/bb05db33d3cc2c25c09052f0d8a2117d7a344167))
* **demo:** stop LeakSanitizer from hanging the demo app on exit ([630b9f1](https://github.com/mrpilot2/aide/commit/630b9f17648aef47e28da394a2e7dff41c48932d))
* demote noisy internal log lines from info/trace mismatch to trace ([88c8dca](https://github.com/mrpilot2/aide/commit/88c8dca41d636a132ca6e31c8b5f2bc1bf1ddb67))
* **gui:** export AideDialog's Q_OBJECT symbols from AideWidgets ([40e7d4c](https://github.com/mrpilot2/aide/commit/40e7d4cd4ea6189a0ad840a6ccb9d86433ff19ad))
* **gui:** fix GotItTooltip contrast and app-window attachment ([dead25b](https://github.com/mrpilot2/aide/commit/dead25b79ab1ee0af3ddeaa0edbc96703ba257ce))
* **gui:** hide notification log view until a consumer embeds it ([bce438a](https://github.com/mrpilot2/aide/commit/bce438ab3df8ae3ed719d648c76c031c6fb9e4bc))
* **gui:** stop pinning balloon/tooltip popups to Wayland's active window ([fd4d20b](https://github.com/mrpilot2/aide/commit/fd4d20b3d3b0d2c17ae81dc53dec7d236cece333))
* **help:** stop leaking log path in bug reports, drop broken OS prefill ([5f0be44](https://github.com/mrpilot2/aide/commit/5f0be44d7ce22f12a20a0182008ba0f9c2f50c87))
* logger macros on windows shared library ([18af4bd](https://github.com/mrpilot2/aide/commit/18af4bd66eb19ca8c20bd4150f38a2b79755057c))
* **mainwindow:** keep the window maximized when leaving full screen ([65d20b3](https://github.com/mrpilot2/aide/commit/65d20b3299f48d9efdcbc506a077b716d5fde24d))
* make the "no writable log location" tests actually isolate CacheLocation on Windows/macOS ([4e2a2d0](https://github.com/mrpilot2/aide/commit/4e2a2d00509015210f96162409277ca1e5f7b96b))
* **notifications:** anchor balloons to the app window, not the desktop ([07a79d4](https://github.com/mrpilot2/aide/commit/07a79d47412c799a7c5e72e911130cbb6da5b5ca))
* **notifications:** fix unreadable balloon text on dark theme ([c0bc873](https://github.com/mrpilot2/aide/commit/c0bc87300ed8e198420e364b3de170f231dbc0f0))
* **notifications:** use exact hex dark-theme background colors for Banner ([22315d4](https://github.com/mrpilot2/aide/commit/22315d47b7a1c7c0944c3e32ed09d6ce15e61c6b))
* possible double free issues ([826bca7](https://github.com/mrpilot2/aide/commit/826bca7a05d0ed8540ad606d51556b1230619056))
* **settings:** do not abort when filtering empties the page selection ([2930880](https://github.com/mrpilot2/aide/commit/2930880b60a390788c51d6e48273c160a0aa1f62))
* **settings:** keep the matched page shown when filtering drops the selection ([734a6f4](https://github.com/mrpilot2/aide/commit/734a6f434837820380fd41332a1016ae37c16e67))
* **settings:** refresh theme list each time the appearance page opens ([a3e268d](https://github.com/mrpilot2/aide/commit/a3e268da54e3555a044830751650844bb110d522))
* **settings:** select a single settings page and display it on auto-select ([a3964fa](https://github.com/mrpilot2/aide/commit/a3964fafb3ba60f7f706b51a03f84c49f14bedfd))
* **settings:** ship edit-find icon so search magnifier is visible ([f831a1a](https://github.com/mrpilot2/aide/commit/f831a1a8357d8c10769b6c5a01a056bd01654cf9))
* **settings:** show the auto-selected page widget instead of an empty panel ([04b8a27](https://github.com/mrpilot2/aide/commit/04b8a276e4902c75ed8ee97f4699d9da51f26e84))
* **settings:** stop double-free of settings page on dialog close ([12733e9](https://github.com/mrpilot2/aide/commit/12733e93924566efb6788454d1987850069cdb41))
* **settings:** stop infinite recursion crash when selecting a notification group ([b942d71](https://github.com/mrpilot2/aide/commit/b942d717d690957427cc96ec259053d09795f0f1))
* **settings:** tokenize search words without a static QRegularExpression ([e7e10c7](https://github.com/mrpilot2/aide/commit/e7e10c734bc24bd4cc6a7da24a0fc64d768ab6b1))
* **settings:** use Qt5-compatible findChildren overload ([6567b60](https://github.com/mrpilot2/aide/commit/6567b60580133fd786cd1687da4affc01eb12e10))
* **static-analysis:** raise begin/endFilterChange guard to Qt 6.10 ([ceb5a11](https://github.com/mrpilot2/aide/commit/ceb5a1139811fe39eac64c906dbbf86dbf989447))
* **static-analysis:** resolve dev build clang-tidy and cppcheck failures ([0e37ac2](https://github.com/mrpilot2/aide/commit/0e37ac25a697031566065bb5f5f1c5432ac156d0))
* **static-analysis:** use designated initializers and ranges algorithms ([e011ea8](https://github.com/mrpilot2/aide/commit/e011ea8c0578a4b51f437eeef7fa474edbf6de89))
* **test:** find GotItTooltip via findChildren, not topLevelWidgets() ([374422e](https://github.com/mrpilot2/aide/commit/374422efbaa550984be1353fa47fcefc5cee4382))
* **test:** key MockSettings on HierarchicalId::name(), not the id itself ([59eae09](https://github.com/mrpilot2/aide/commit/59eae0998a422e34e1286f7862ca8500a70b63cf))
* **tests:** build mockurllauncher.cpp unconditionally ([780e15f](https://github.com/mrpilot2/aide/commit/780e15f7f777550ff0c0e08c9409098cb2d8294b))
* **tests:** default QT_QPA_PLATFORM to offscreen in test binaries ([22faeec](https://github.com/mrpilot2/aide/commit/22faeec5555814bd46192073c0d87c2bc19c2a63))
* **tests:** stop LSan exit-scan deadlock on plain ctest/binary runs ([5e7e6b4](https://github.com/mrpilot2/aide/commit/5e7e6b4d5f72060b00ce05fe4860d03346569fda))
* translate settings-page title and keymap root menu label ([7a23c1c](https://github.com/mrpilot2/aide/commit/7a23c1cedc86dc41438c0bedfe3f1f15bd5f1f7d))
* use string-based connect in KeyMapPageWidgetController test ([6e53eaa](https://github.com/mrpilot2/aide/commit/6e53eaa73c7e40ac427460b1c28a4e94bd2517bf))
* **widgets:** bind SearchFilterWidget's deferred-visibility timer to `this` ([6e8a2a9](https://github.com/mrpilot2/aide/commit/6e8a2a9c21225c271163b306be1069eed66de7c7))


### Performance Improvements

* **Logger:** store already created loggers for reuse ([aef0866](https://github.com/mrpilot2/aide/commit/aef086676e12c56a77ba916931b5bf0037a60fbc))


### Code Refactoring

* move public logger headers into logger/ subdirectory ([2ca916e](https://github.com/mrpilot2/aide/commit/2ca916e2c9808e13c3d73dfaa2e334cd55d8bd18))
* reduce static analyzer warnings ([337d6c1](https://github.com/mrpilot2/aide/commit/337d6c1f964deff14474423946153663400e94e2))
* SettingsPageRegistry instance-based API ([a90b171](https://github.com/mrpilot2/aide/commit/a90b1717d333d3a2b546fea97ccaafbc32f3d026))

## [0.3.0](https://github.com/mrpilot2/aide/compare/v0.2.1...v0.3.0) (2023-06-03)


### Features

* **SearchLineEdit:** add highlight delegates ([7cbfb46](https://github.com/mrpilot2/aide/commit/7cbfb4675a06b98b00a05f3cb1689eda0de2bf07))
* **SearchLineEdit:** add options to match case and for regex ([a641aab](https://github.com/mrpilot2/aide/commit/a641aabc08fc726b7284f00ed5431af20ff0b1b8))
* **SearchLineEdit:** first useful implementation ([6d5bc8a](https://github.com/mrpilot2/aide/commit/6d5bc8ab0f638bf35d00996009fa29855ce16713))
* **SearchLineEdit:** gui settings might be null ([7eff1ff](https://github.com/mrpilot2/aide/commit/7eff1ff426d9858f1ed4bfec47399d13b17113d4))
* **SearchLineEdit:** save/load GUI state ([548cd19](https://github.com/mrpilot2/aide/commit/548cd19ba97e6a7b0c05034bd9c5f15e7e340600))
* **SearchLineEdit:** set visibility of widget after construction ([16cbff9](https://github.com/mrpilot2/aide/commit/16cbff9ec5f6f5973f5e365120380854b6ffb2eb))
* **SearchLineEdit:** wildcard matching not allowed for Qt &lt; 5.12 ([922ea32](https://github.com/mrpilot2/aide/commit/922ea32884436277f7f6260bf9b768b16246c929))


### Bug Fixes

* **about:** correct representation of build date ([d4a2d71](https://github.com/mrpilot2/aide/commit/d4a2d71dbb33b90847f96a5c247452119e931294))
* remove memory leaks ([1155d13](https://github.com/mrpilot2/aide/commit/1155d137e6e94ea1bb18179e2ade6fe4253a4d8b))

## [0.2.1](https://github.com/mrpilot2/aide/compare/v0.2.0...v0.2.1) (2022-12-28)


### Bug Fixes

* run github-release on main branch ([97316a2](https://github.com/mrpilot2/aide/commit/97316a2d8972168faddf7408f11e0ae79c28952b))

## [0.2.0](https://github.com/mrpilot2/aide/compare/v0.1.1...v0.2.0) (2022-12-28)


### Features

* **about:** add logo ([a362268](https://github.com/mrpilot2/aide/commit/a3622687bf8c44319cccd07533bc3fe40b16df03))
* **about:** add menu item to select about dialog ([c8ab3e2](https://github.com/mrpilot2/aide/commit/c8ab3e24f61c00ba5c50b4e2ca881025f6c88b85))
* **about:** add What's new link ([a14a5a1](https://github.com/mrpilot2/aide/commit/a14a5a170dce9fb3ef976cd7d4454e059f9a2756))
* **about:** implementation of about dialog ([f43dd37](https://github.com/mrpilot2/aide/commit/f43dd3784bbd8d12e5f98b19b2f5cec689bfb517))
* **about:** set parent window ([e57231c](https://github.com/mrpilot2/aide/commit/e57231c82a22dcc0d4ace7873589d3e82d5aace0))
* **Logger:** make log level configurable ([e522e61](https://github.com/mrpilot2/aide/commit/e522e614eb68530ef1a7d45859dd0651e04cdf41))


### Bug Fixes

* release-please shall run on default branch ([ffeefae](https://github.com/mrpilot2/aide/commit/ffeefaebc25b7b4176eb1bded60a15f1ecc490be))

## [0.2.0](https://github.com/mrpilot2/aide/compare/v0.1.1...v0.2.0) (2022-12-28)


### Features

* **about:** add logo ([a362268](https://github.com/mrpilot2/aide/commit/a3622687bf8c44319cccd07533bc3fe40b16df03))
* **about:** add menu item to select about dialog ([c8ab3e2](https://github.com/mrpilot2/aide/commit/c8ab3e24f61c00ba5c50b4e2ca881025f6c88b85))
* **about:** add What's new link ([a14a5a1](https://github.com/mrpilot2/aide/commit/a14a5a170dce9fb3ef976cd7d4454e059f9a2756))
* **about:** implementation of about dialog ([f43dd37](https://github.com/mrpilot2/aide/commit/f43dd3784bbd8d12e5f98b19b2f5cec689bfb517))
* **about:** set parent window ([e57231c](https://github.com/mrpilot2/aide/commit/e57231c82a22dcc0d4ace7873589d3e82d5aace0))

## [0.1.1](https://github.com/mrpilot2/aide/compare/v0.1.0...v0.1.1) (2022-07-30)


### Bug Fixes

* remove wrong if condition ([33aff8e](https://github.com/mrpilot2/aide/commit/33aff8e74af9b663a9a2c118b0679ff652c556c5))

## [0.1.0](https://github.com/mrpilot2/aide/compare/v0.0.2...v0.1.0) (2022-07-30)


### Features

* create deploy workflow ([9b9ad1b](https://github.com/mrpilot2/aide/commit/9b9ad1bc6d5bf22576d5a7585a3f10610d57c749))

## [0.0.2](https://github.com/mrpilot2/aide/compare/v0.0.1...v0.0.2) (2022-07-30)


### Bug Fixes

* branch ([571909b](https://github.com/mrpilot2/aide/commit/571909b3cac70452a0e65bf0f155e03e6c17ac04))

## [0.0.1](https://github.com/mrpilot2/aide/compare/v0.0.0...v0.0.1) (2022-07-30)


### Bug Fixes

* action name ([7400ee9](https://github.com/mrpilot2/aide/commit/7400ee956609c6657c385f7900850e5c6e7ceecb))
* add new action ([4b02459](https://github.com/mrpilot2/aide/commit/4b0245927dca574f0ec8a52a1a61f788180a663b))
* branch ([571909b](https://github.com/mrpilot2/aide/commit/571909b3cac70452a0e65bf0f155e03e6c17ac04))
* release type ([99826e9](https://github.com/mrpilot2/aide/commit/99826e9e1361f99a51ff94c0e335d37cd8ed4224))
* token name ([82d891d](https://github.com/mrpilot2/aide/commit/82d891d824af42e14b480900f6867b1c9c95014e))
* workflow file ([bb07c06](https://github.com/mrpilot2/aide/commit/bb07c06212b63ed4074c598aa4438c7622c57c06))


### Miscellaneous Chores

* release 0.0.1 ([c7e2b24](https://github.com/mrpilot2/aide/commit/c7e2b240c05fa1de756a59e55e43e8f8bcdb115f))
