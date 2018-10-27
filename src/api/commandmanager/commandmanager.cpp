
#include "commandmanager.hpp"

using aide::CommandList;
using aide::CommandManager;

const CommandList& aide::CommandManager::commands() const
{
    return m_commands;
}

void CommandManager::registerAction(QAction* action)
{
    if (action == nullptr)
    {
        throw std::invalid_argument("Cannot register a nullptr as action");
    }
}
