

#include "mockaboutdialog.hpp"

using aide::tests::MockAboutDialog;

void MockAboutDialog::showAboutInformation(
    const aide::core::AideInformation& info)
{
    m_info = info;
}

const aide::core::AideInformation& MockAboutDialog::getInfo() const
{
    return m_info;
}
