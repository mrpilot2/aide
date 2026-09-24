#ifndef AIDE_VCS_VCS_PROCESS_RUNNER_HPP
#define AIDE_VCS_VCS_PROCESS_RUNNER_HPP

#include <QObject>

#include <aide/loggerinterface.hpp>
#include <aide/vcsprocessrunnerinterface.hpp>

namespace aide
{
    /**
     * @brief QProcess-backed VcsProcessRunnerInterface (#171).
     *
     * Logs every constructed command line (with any embedded URL
     * credentials redacted) through the "aide.vcs" logger before starting
     * it. Each run() spawns its own QProcess, so calls may overlap.
     */
    class VcsProcessRunner
        : public QObject
        , public VcsProcessRunnerInterface
    {
        Q_OBJECT

    public:
        explicit VcsProcessRunner(LoggerPtr logger, QObject* parent = nullptr);

        void run(const std::string& workingDirectory,
                 const std::string& executable,
                 const std::vector<std::string>& arguments,
                 ResultCallback callback) override;

    private:
        LoggerPtr m_logger;
    };
} // namespace aide

#endif // AIDE_VCS_VCS_PROCESS_RUNNER_HPP
