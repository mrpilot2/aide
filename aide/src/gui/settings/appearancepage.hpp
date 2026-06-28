#ifndef AIDE_APPEARANCE_PAGE_HPP
#define AIDE_APPEARANCE_PAGE_HPP

#include <QWidget>

#include <aide/settings/settingspage.hpp>

class QComboBox;
class QFontComboBox;
class QSpinBox;

namespace aide
{
    class AppearanceManager;

    namespace gui
    {
        class AppearancePage
            : public QWidget
            , public core::SettingsPage
        {
            Q_OBJECT

        public:
            explicit AppearancePage(aide::AppearanceManager& manager,
                                    QWidget* parent = nullptr);

            QWidget* widget() override;
            [[nodiscard]] bool isModified() const override;
            void reset() override;
            void apply() override;

        private:
            void syncControlsToManager();

            aide::AppearanceManager& m_manager;
            QComboBox* m_themeCombo{nullptr};
            QFontComboBox* m_fontCombo{nullptr};
            QSpinBox* m_fontSizeSpinBox{nullptr};

            QString m_appliedTheme;
            QString m_appliedFontFamily;
            int m_appliedFontSize{0};
        };
    } // namespace gui
} // namespace aide

#endif // AIDE_APPEARANCE_PAGE_HPP
