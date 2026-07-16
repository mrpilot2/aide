
#ifndef AIDE_SHOW_LOG_IN_FILE_MANAGER_USE_CASE_HPP
#define AIDE_SHOW_LOG_IN_FILE_MANAGER_USE_CASE_HPP

#include <aide/loggerinterface.hpp>

#include "filemanagerlauncher.hpp"

namespace aide::core
{
    class ShowLogInFileManagerUseCase
    {
    public:
        ShowLogInFileManagerUseCase(FileManagerLauncherPtr launcher,
                                    LoggerPtr logger);

        void showLogInFileManager() const;

    private:
        FileManagerLauncherPtr m_launcher;
        LoggerPtr m_logger;
    };
} // namespace aide::core

#endif // AIDE_SHOW_LOG_IN_FILE_MANAGER_USE_CASE_HPP
