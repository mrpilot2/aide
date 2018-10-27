
#pragma once

#include <string>

#include <QAction>
#include <QList>

namespace aide
{
    using CommandList = QList<std::string>;

    class CommandManager
    {
    public:
        void registerAction(QAction* action) noexcept(false);

        const CommandList& commands() const;

    private:

        CommandList m_commands;
    };

} // end namespace aide
