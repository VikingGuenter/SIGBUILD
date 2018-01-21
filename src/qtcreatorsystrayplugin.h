#pragma once

#include "qtcreatorsystray_global.h"

#include <extensionsystem/iplugin.h>

class QMenu;
class QSoundEffect;
class QSystemTrayIcon;

namespace QtCreatorSysTray {

class QtCreatorSysTrayPlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "QtCreatorSysTray.json")

public:
	QtCreatorSysTrayPlugin();
	~QtCreatorSysTrayPlugin();

	bool initialize(const QStringList & arguments, QString * errorString) override;
	void extensionsInitialized() override;
	ShutdownFlag aboutToShutdown() override;

private:
	QMenu * mTrayMenu = nullptr;
	QSystemTrayIcon * mTrayIcon  = nullptr;

	QSoundEffect * mSoundSuccess = nullptr;
	QSoundEffect * mSoundFail = nullptr;

	int mTimeNotification = 5000;
};

} // namespace QtCreatorSysTray
