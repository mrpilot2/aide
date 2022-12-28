

#ifndef ASIDE_AIDE_WRAPPER_MOCK_ABOUT_DIALOG_HPP
#define ASIDE_AIDE_WRAPPER_MOCK_ABOUT_DIALOG_HPP

#include "aideinformation.hpp"
#include "aideinformationpresenter.hpp"

namespace aide::tests
{
    class MockAboutDialog : public aide::core::AideInformationPresenter
    {
    public:
        void showAboutInformation(const core::AideInformation& info) override;

        const core::AideInformation& getInfo() const;

    private:
        core::AideInformation m_info;
    };
} // namespace aide::tests

#endif // ASIDE_AIDE_WRAPPER_MOCK_ABOUT_DIALOG_HPP
