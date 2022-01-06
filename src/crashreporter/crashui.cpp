/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "crashui.h"
#include "fontloader.h"
#include "imageproviderfactory.h"
#include "nebula.h"
#include "theme.h"
#include <iostream>
#include <QCoreApplication>
#include <QWindow>
#include "l18nstrings.h"
#include "mozillavpn.h"
#include "qmlengineholder.h"
#include "localizer.h"

using namespace std;

constexpr auto APP = "app";
constexpr auto QML_MAIN = "qrc:/crashui/main.qml";

CrashUI::CrashUI() {}

void CrashUI::initialize() {
  if (!m_initialized) {
    Nebula::Initialize(QmlEngineHolder::instance()->engine());
    FontLoader::loadFonts();
    auto provider = ImageProviderFactory::create(QCoreApplication::instance());
    if (provider) {
      QmlEngineHolder::instance()->engine()->addImageProvider(APP, provider);
    }
    qmlRegisterSingletonType<MozillaVPN>(
        "Mozilla.VPN", 1, 0, "VPNLocalizer",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
          auto obj = Localizer::instance();
          QQmlEngine::setObjectOwnership(obj, QQmlEngine::CppOwnership);
          return obj;
        });
    qmlRegisterSingletonType<L18nStrings>(
        "Mozilla.VPN", 1, 0, "VPNl18n",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
          auto obj = L18nStrings::instance();
          QQmlEngine::setObjectOwnership(obj, QQmlEngine::CppOwnership);
          return obj;
        });
    m_theme = make_shared<Theme>();
    m_theme->loadThemes();
    qmlRegisterSingletonType<Theme>(
        "Mozilla.VPN", 1, 0, "VPNTheme",
        [this](QQmlEngine*, QJSEngine*) -> QObject* {
          auto obj = m_theme.get();
          QQmlEngine::setObjectOwnership(obj, QQmlEngine::CppOwnership);
          return obj;
        });

    qmlRegisterSingletonType<CrashUI>(
        "Mozilla.VPN", 1, 0, "CrashController",
        [this](QQmlEngine*, QJSEngine*) -> QObject* {
          QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
          return this;
        });

    QObject::connect(QmlEngineHolder::instance()->engine(),
                     &QQmlApplicationEngine::objectCreated,
                     [](QObject* obj, const QUrl& path) {
                       if (obj == nullptr) {
                         std::cout << path.toString().toStdString()
                                   << " Failed to load.";
                       }
                     });
    const QUrl url(QML_MAIN);
    QmlEngineHolder::instance()->engine()->load(url);
    m_initialized = true;
  }
}

void CrashUI::showUI() {
  auto window = QmlEngineHolder::instance()->window();
  window->show();
  window->raise();
  window->requestActivate();
}

Q_INVOKABLE void CrashUI::sendReport() {
  auto window = QmlEngineHolder::instance()->window();
  window->hide();
  emit startUpload();

  return Q_INVOKABLE void();
}

Q_INVOKABLE void CrashUI::userDecline() {
  auto window = QmlEngineHolder::instance()->window();
  window->hide();
  emit cleanupDumps();
  return Q_INVOKABLE void();
}
