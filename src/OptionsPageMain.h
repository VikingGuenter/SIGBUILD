#pragma once

#include <coreplugin/dialogs/ioptionspage.h>

namespace Sigbuild
{

class OptionsPageMainWidget;
class Settings;

class OptionsPageMain : public QObject, public Core::IOptionsPage
{
    Q_OBJECT

public:
    OptionsPageMain(Settings * settings, QObject * parent = nullptr);

    QWidget * widget() override;
    void apply()  override;
    void finish()  override;

signals:
    void SettingsChanged();

private:
    Settings * mSettings = nullptr;

    OptionsPageMainWidget * mWidget = nullptr;
};

} // namespace Sigbuild
