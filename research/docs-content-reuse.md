# Research: reusing library-authored End-User Help in a consumer's docs site

Resolves wayfinder ticket [#186](https://github.com/mrpilot2/aide/issues/186) (child of map #184).
Researched 2026-10-15 against primary sources (official docs, READMEs and source of the tools named).

## Question

aIDE authors **End-User Help** pages (first: the keymap settings page). A client
application built on aIDE has its own help site; the client's **Help Base URL** +
a **Help Topic ID** (e.g. `keymap`, `settings/keymap`) must resolve to a page.
Which mechanisms let the client's site reuse aIDE-authored pages, and how does each
handle version pinning, per-page override, theming, maintenance cost and
SSG-agnosticism?

## Facts about aIDE that constrain the answer

- **How clients get aIDE today.** The Developer Guide recommends `FetchContent_Declare(aide GIT_REPOSITORY … GIT_TAG v0.3.0)`; Conan 2 is used only to provide `fmt`/`spdlog` (`docs/getting-started/installation.md`). The repo has no `conanfile.py` recipe for aIDE itself (only `conanfile.txt` for its own deps). aIDE also has `install()` rules + CPack (`aide/packaging/CMakeLists.txt`) and already installs a non-code payload: `licenses.html` to `${CMAKE_INSTALL_DOCDIR}/`.
- **FetchContent already ships the help sources.** After `FetchContent_MakeAvailable()`, `<lowercaseName>_SOURCE_DIR` points at the populated checkout; `FETCHCONTENT_SOURCE_DIR_<NAME>` can redirect it to a local checkout ([CMake FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html)). So any Markdown in the aIDE repo is already on disk in the client's build tree, at exactly the `GIT_TAG` the client links against.
- **Current Developer Guide deploy.** `.github/workflows/doxygen.yml` runs `mkdocs build --strict` and publishes `site/` to `gh-pages` root with `peaceiris/actions-gh-pages@v4`, unversioned. That action removes existing files in the publish branch (or in `destination_dir`) unless `keep_files: true` ([peaceiris README](https://github.com/peaceiris/actions-gh-pages#%EF%B8%8F-keeping-existing-files-keep_files)). A second, aIDE-hosted End-User Help site on the same Pages host would need its own `destination_dir`/subpath (or its own repo) so the two deploys do not clobber each other.
- **URL shape.** MkDocs with the default `use_directory_urls: true` turns `docs/keymap.md` into `…/keymap/`; this applies to every MkDocs-based option below and to redirect stubs ([mkdocs-redirects README](https://github.com/mkdocs/mkdocs-redirects)).

Versions checked on PyPI on 2026-10-15: mkdocs-material 9.7.7, pymdown-extensions 12.3, mkdocs-monorepo-plugin 1.1.2 (2025-06-05, "Production/Stable"), mkdocs-multirepo-plugin 0.8.3 (2024-08-13, "Alpha"), mkdocs-redirects 1.2.3, mike 2.2.0.

## Two layers: transport vs. assembly

The candidates fall into two layers that combine freely:

- **Transport** — how aIDE's Markdown reaches the client's machine/CI at a pinned version: FetchContent source dir, CMake/Conan install payload, git submodule, git subtree, plain URL fetch.
- **Assembly** — how the client's SSG turns those files into pages under its Help Base URL: pymdownx.snippets, mkdocs-monorepo-plugin, mkdocs-multirepo-plugin (does its own transport), a copy step, or not assembling at all (link/redirect to aIDE-hosted pages).

---

## Transport options

### T1. FetchContent source directory (already in place)

- **Pinning:** identical to the library pin — the same `GIT_TAG` (CMake advises a commit hash for content you don't control). Help can never drift from the linked binary.
- **Consumer access:** `${aide_SOURCE_DIR}/<help-dir>`; a client can `configure_file`/`file(COPY)` it into its docs tree or pass the path to its docs build.
- **Caveat:** only works when the client builds its docs from the same CMake configure (or reads the path out of it). If FetchContent is satisfied by a dependency provider, `aide_SOURCE_DIR` may be empty ([FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html)).
- **SSG-agnostic:** yes (plain files).

### T2. Installed payload via CMake `install()` (and a future Conan package)

- **Mechanism:** `install(DIRECTORY help/ DESTINATION ${CMAKE_INSTALL_DOCDIR}/help COMPONENT Help)` — directory copied verbatim; `COMPONENT` lets packagers include/exclude it ([install()](https://cmake.org/cmake/help/latest/command/install.html)). `CMAKE_INSTALL_DOCDIR` defaults to `DATAROOTDIR/doc/PROJECT_NAME` ([GNUInstallDirs](https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html)). aIDE already does this for `licenses.html`.
- **Conan:** if aIDE ever ships a Conan recipe, `package()` copies arbitrary files into `package_folder` (directly or via `cmake.install()`) ([package()](https://docs.conan.io/2/reference/conanfile/methods/package.html)); `cpp_info.resdirs` exists for resource dirs and is empty by default ([package_info()](https://docs.conan.io/2/reference/conanfile/methods/package_info.html)). A consumer reads it in `generate()` via `self.dependencies["aide"].cpp_info` (absolute paths) or `.package_folder` (None for editables) and can `copy()` from it ([generate()](https://docs.conan.io/2/reference/conanfile/methods/generate.html)). CMakeDeps does **not** document a variable exposing `resdirs` ([CMakeDeps](https://docs.conan.io/2/reference/tools/cmake/cmakedeps.html)), so a client needs a `generate()` step or an aIDE-exported CMake variable to find the files.
- **Pinning:** same as the library package version.
- **SSG-agnostic:** yes.

### T3. git submodule

- **Mechanism:** superproject records one commit of the submodule as a gitlink (mode `160000`) ([Pro Git: Submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules)).
- **Pinning:** check out the release tag inside the submodule and commit the pointer; `git submodule update --remote` follows a branch.
- **Costs:** clones need `--recurse-submodules` or `submodule update --init`; plain `git pull` does not update it; detached HEAD and unpushed-pointer pitfalls (same source). Pulls the whole aIDE repo (C++ sources included) unless help lives in its own repo. It is a **second pin** next to the FetchContent/Conan pin, so help and binary can drift.
- **SSG-agnostic:** yes. monorepo-plugin explicitly cites submodules as its cross-repo story ([mkdocs-monorepo-plugin](https://github.com/backstage/mkdocs-monorepo-plugin)).

### T4. git subtree

- **Mechanism:** `git subtree add/pull --prefix=<dir> [--squash]` merges another project into a subdirectory; `--squash` imports a single commit; `push` splits local changes back upstream ([git-subtree docs](https://github.com/git/git/blob/master/contrib/subtree/git-subtree.adoc), lives in git's `contrib/`).
- **Pinning:** the ref passed to `add`/`pull`.
- **Override:** files are ordinary files in the client repo — the client can edit any page in place; the price is merge conflicts on the next `subtree pull`.
- **Costs:** same second-pin drift as T3; contrib tool not installed everywhere.
- **SSG-agnostic:** yes.

### T5. URL fetch at build time

Covered by snippets (`url_download`) and multirepo below; no local copy, pin = tag/commit in the URL.

---

## Assembly options

### A1. pymdownx.snippets (include into client-owned stub pages)

- **Mechanism:** client writes `docs/keymap.md` containing `--8<-- "aide-help/keymap.md"`. Files are resolved against `base_path` (list, first match wins); `url_download: true` allows URL snippets (e.g. `raw.githubusercontent.com/mrpilot2/aide/v0.4.0/…`), bounded by `url_max_size`, `url_timeout`, retries; `check_paths: true` fails the build on a missing snippet (default: silently dropped); `restrict_base_path` (default true) keeps includes inside base paths. Line ranges (`file.md:4:6`) and named sections (`[start:name]`/`[end:name]` markers, include with `file.md:name`) allow partial reuse. Snippets inside URL snippets may only be URLs. Runs as a preprocessor, i.e. also inside fenced code ([Snippets docs](https://facelessuser.github.io/pymdown-extensions/extensions/snippets/)). aIDE's own `mkdocs.yml` already enables snippets.
- **Pinning:** via the transport (T1–T4) or the tag in the URL (T5).
- **Override one page:** natural — the client owns every stub page: it can wrap aIDE content with its own intro, include only some sections, or replace the include with its own text. Topic ID → page mapping is fully in the client's hands.
- **Theming:** rendered by the client's SSG → fully native look/nav/search.
- **Maintenance:** one stub file per reused topic in each client; aIDE must keep section markers / file names stable (they become a content API). Relative links and images inside the included text resolve relative to the client's stub page, not aIDE's file, so aIDE content should avoid relative cross-links/images or use a convention clients mirror.
- **SSG-agnostic:** no — Python-Markdown/MkDocs-specific syntax (other SSGs have their own include syntax, so the stubs would be rewritten, the aIDE source could stay the same).

### A2. mkdocs-monorepo-plugin (`!include` an aIDE mkdocs.yml)

- **Mechanism:** client `nav` entry `- aIDE: '!include <path>/aide-help/mkdocs.yml'`; `*include` with a glob since 0.5.2. Each included `mkdocs.yml` needs `site_name` (the plugin aborts without it) and the included docs are placed under that name ([README](https://github.com/backstage/mkdocs-monorepo-plugin), [parser.py](https://github.com/backstage/mkdocs-monorepo-plugin/blob/master/mkdocs_monorepo_plugin/parser.py)). So aIDE help lands at `<client-site>/<site_name>/keymap/` — the Help Topic ID the client maps to would include that prefix.
- **Pinning:** via transport (path typically from a submodule; T1/T2 paths work too since the path is resolved to an absolute path).
- **Override one page:** not provided by the plugin — the client can drop the whole include, or point the topic (via aIDE's planned topic remap API) to its own page; it cannot shadow a single included page.
- **Theming:** native (one merged MkDocs build).
- **Maintenance:** aIDE must maintain a self-contained help `mkdocs.yml` + nav; plugin maintained by Backstage (TechDocs), 1.1.2 stable.
- **SSG-agnostic:** no (MkDocs only).

### A3. mkdocs-multirepo-plugin (`!import` from git at build time)

- **Mechanism:** `!import https://github.com/mrpilot2/aide?branch=v0.4.0&docs_dir=help/*&config=…` in `nav`, or `repos`/`nav_repos` in plugin config (`nav_repos` imports selected files/globs); clones into a temp dir at build time; tokens needed for private repos ([README](https://github.com/jdoiro3/mkdocs-multirepo-plugin)).
- **Pinning:** `branch=` accepts a tag (git-clone semantics); a separate pin from the library pin.
- **Override one page:** with `nav_repos` the client picks which files to import and can omit one and supply its own; no documented shadowing.
- **Theming:** native.
- **Maintenance / risk:** README status badge "Inactive", lifecycle "beta"; PyPI classifier "3 - Alpha", last release 0.8.3 (Aug 2024). Network access at docs-build time.
- **SSG-agnostic:** no.

### A4. Plain copy step (no plugin)

Client CI copies the transported files (T1–T4) into its `docs_dir` before `mkdocs build` (or any SSG). Override = client's copy step skips/overwrites a file. Native theming; SSG-agnostic as long as aIDE's Markdown avoids SSG-specific extensions (admonitions, tabs etc. are mkdocs-material/pymdownx dialect). MkDocs `INHERIT` can share config, but lists (`nav`, list-style `plugins`/`markdown_extensions`) are not merged, only nested dicts ([MkDocs configuration](https://www.mkdocs.org/user-guide/configuration/)), so aIDE cannot inject nav entries into the client's config that way.

### A5. aIDE hosts the pages; the client links or redirects

- **Mechanism (hosting):** aIDE builds and publishes End-User Help itself, ideally versioned with mike: each `mike deploy <version>` commits that version to its own directory on `gh-pages`, aliases (`latest`, `stable`) via symlink/redirect/copy, `set-default` redirects the root, `versions.json` feeds selectors ([mike](https://github.com/jimporter/mike)); mkdocs-material shows a version selector with `extra.version.provider: mike` and supports an "outdated version" banner ([Material: versioning](https://squidfunk.github.io/mkdocs-material/setup/setting-up-versioning/)). Must coexist with the unversioned Developer Guide deploy (subpath/`destination_dir` or separate repo, see above).
- **Mechanism (client side), two variants:**
  1. **Redirect stubs on the client site:** `mkdocs-redirects` `redirect_maps` accepts external URL targets (`'keymap.md': 'https://…/aide/help/0.4/keymap/'`); it writes an HTML page per mapped path with `<meta http-equiv="refresh">`, a JS redirect that preserves `#anchor`, and `rel=canonical` ([README](https://github.com/mkdocs/mkdocs-redirects), [plugin.py](https://github.com/mkdocs/mkdocs-redirects/blob/master/mkdocs_redirects/plugin.py)). Client's Help Base URL + `keymap` resolves on the client domain, then bounces. Any static host / web server can do the same without MkDocs.
  2. **No client site involvement:** the C++ help API resolves aIDE-provided topics against an aIDE-owned base URL (versioned) instead of the client's Help Base URL. This is an API decision for map #184, not a docs mechanism.
- **Pinning:** version segment in the URL (mike directory/alias); a client pinning `latest` gets drift, a client pinning `0.4` matches its library version.
- **Override one page:** replace that redirect with a real client page (or remap the topic in the API).
- **Theming:** inconsistent — users leave the client's site for aIDE-branded pages with aIDE's nav/search; aIDE content cannot name the client application or reflect the client's changed shortcuts.
- **Maintenance:** lowest for clients (a map of redirects, or nothing); aIDE carries a versioned hosting pipeline. Depends on aIDE's site staying online at stable URLs for every released version.
- **SSG-agnostic:** yes.

---

## Comparison

Transport (rows T*) and assembly (rows A*) combine; e.g. "T1 + A1" = FetchContent source dir + snippets stubs.

| Option | Version pinning | Override a single page | Theming consistency | Maintenance cost | SSG-agnostic |
|---|---|---|---|---|---|
| T1 FetchContent source dir | Same `GIT_TAG` as the library (no drift) | n/a (transport) | n/a | None extra on aIDE side; client must read `aide_SOURCE_DIR` | Yes |
| T2 CMake/Conan install payload | Same as library package version | n/a | n/a | aIDE adds `install(DIRECTORY …)` (+ recipe/resdirs if Conan); client needs `generate()`/path discovery | Yes |
| T3 git submodule | Gitlink commit/tag; separate pin, can drift | n/a (read-only copy) | n/a | Clone/update ceremony; pulls whole repo | Yes |
| T4 git subtree | `--prefix` ref; separate pin | Edit in place (merge conflicts on pull) | n/a | contrib tool; merge upkeep | Yes |
| A1 pymdownx.snippets stubs | Via transport, or tag in URL | Easy: client owns each stub (wrap, partial sections, replace) | Native | One stub per topic per client; aIDE keeps file/section names stable; relative links/images fragile | No (MkDocs/Python-Markdown) |
| A2 mkdocs-monorepo-plugin | Via transport | Not per page (drop include or remap topic) | Native | aIDE maintains a help `mkdocs.yml`; topics live under `<site_name>/` | No |
| A3 mkdocs-multirepo-plugin | `branch=<tag>` at build time; separate pin | Partial via `nav_repos` file selection | Native | Plugin inactive/alpha; network at build | No |
| A4 Plain copy step | Via transport | Skip/overwrite in copy script | Native (if Markdown dialect matches) | Small script per client | Yes, if content avoids SSG-specific syntax |
| A5 aIDE-hosted + client redirects/links | Version in URL (mike) | Replace redirect / remap topic | Inconsistent (aIDE look, leaves client site, no client-specific wording) | Lowest for clients; aIDE runs versioned hosting forever | Yes |

## Cross-cutting observations (no decision)

- Only T1/T2 tie the help version to the linked library version automatically; T3/T4/A3/URL snippets introduce a second pin.
- Every in-site option (A1–A4) gives native theming and a page on the client's domain; content dialect (mkdocs-material/pymdownx extensions) decides how SSG-agnostic the reused Markdown really is.
- Fine-grained client override is strongest with A1 (client-owned stubs) and A4/T4 (file-level control); weakest with A2/A5, where the planned topic-remap API carries the override instead.
- None of these mechanisms adapts wording to the client (application name, changed shortcuts); that needs templating (e.g. a macros-style plugin) and is listed as "not yet specified" on map #184.
- A "client CI check that each declared Help Topic ID resolves" (also open on #184) is easiest where the pages are built in the client's tree (A1–A4: `mkdocs build --strict` + `check_paths: true`) and needs HTTP checks for A5.
