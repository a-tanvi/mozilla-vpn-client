/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "adjusthandler.h"
#include "adjustproxy.h"
#include "constants.h"
#include "logger.h"
#include "mozillavpn.h"
#include "settingsholder.h"

#ifdef MVPN_IOS
#  include "platforms/ios/iosadjusthelper.h"
#endif
#ifdef MVPN_ANDROID
#  include "platforms/android/androidutils.h"
#  if QT_VERSION >= 0x060000
#    include <QJniObject>
#  else
#    include <QAndroidJniObject>
typedef QAndroidJniObject QJniObject;
#  endif
#endif

#include <QString>
#include <QRandomGenerator>

namespace {
Logger logger(LOG_ADJUST, "AdjustHandler");
bool s_initialized = false;
AdjustProxy* s_adjustProxy = nullptr;
}  // namespace

void AdjustHandler::initialize() {
  if (s_initialized) {
    return;
  }

  SettingsHolder* settingsHolder = SettingsHolder::instance();
  Q_ASSERT(settingsHolder);

  if (settingsHolder->firstExecution() &&
      !settingsHolder->hasAdjustActivatable()) {
    // We want to activate Adjust only for new users.
    logger.debug() << "First execution detected. Let's make adjust activatable";
    settingsHolder->setAdjustActivatable(true);
  }

  MozillaVPN* vpn = MozillaVPN::instance();
  Q_ASSERT(vpn);

  // If the app has not started yet, let's wait.
  if (vpn->state() == MozillaVPN::StateInitialize) {
    QObject::connect(vpn, &MozillaVPN::stateChanged, AdjustHandler::initialize);
    return;
  }

  s_initialized = true;

  if (!settingsHolder->gleanEnabled()) {
    // The user doesn't want to be tracked. Good!
    logger.debug() << "Telemetry policy disabled. Bail out";
    return;
  }

  if (!settingsHolder->adjustActivatable()) {
    // This is a pre-adjustSDK user. We don't want to activate the tracking.
    logger.debug() << "Adjust is not activatable. Bail out";
    return;
  }

  QObject::connect(settingsHolder, &SettingsHolder::gleanEnabledChanged,
                   [](const bool& gleanEnabled) {
                     if (!gleanEnabled) {
                       forget();
                       // Let's keep the proxy on. Maybe Adjust needs to send
                       // requests to remove data on their servers.
                       return;
                     }
                   });

  s_adjustProxy = new AdjustProxy(vpn);
  QObject::connect(vpn->controller(), &Controller::readyToQuit, s_adjustProxy,
                   &AdjustProxy::close);
  QObject::connect(s_adjustProxy, &AdjustProxy::acceptError,
                   [](QAbstractSocket::SocketError socketError) {
                     logger.error()
                         << "Adjust Proxy connection error: " << socketError;
                   });
  for (int i = 0; i < 5; i++) {
    quint16 port = QRandomGenerator::global()->bounded(1024, 65536);
    bool succeeded = s_adjustProxy->initialize(port);
    if (succeeded) {
      break;
    }
  }

  if (!s_adjustProxy->isListening()) {
    logger.error() << "Adjust Proxy listening failed";
    return;
  }

#ifdef MVPN_ANDROID
  QJniObject::callStaticMethod<void>(
      "org/mozilla/firefox/vpn/qt/VPNApplication", "onVpnInit", "(ZI)V",
      Constants::inProduction(), s_adjustProxy->serverPort());
#endif

#ifdef MVPN_IOS
  IOSAdjustHelper::initialize(s_adjustProxy->serverPort());
#endif
}

void AdjustHandler::trackEvent(const QString& event) {
  if (!s_adjustProxy || !s_adjustProxy->isListening()) {
    logger.error() << "Adjust Proxy not listening; event tracking failed";
    return;
  }

  if (!SettingsHolder::instance()->adjustActivatable()) {
    logger.debug() << "Adjust is not activatable. Bail out";
    return;
  }

#ifdef MVPN_ANDROID
  QJniObject javaMessage = QJniObject::fromString(event);
  QJniObject::callStaticMethod<void>(
      "org/mozilla/firefox/vpn/qt/VPNApplication", "trackEvent",
      "(Ljava/lang/String;)V", javaMessage.object<jstring>());
#endif

#ifdef MVPN_IOS
  IOSAdjustHelper::trackEvent(event);
#endif
}

void AdjustHandler::forget() {
  logger.debug() << "Adjust Proxy forget";

  if (!s_adjustProxy || !s_adjustProxy->isListening()) {
    logger.error()
        << "Adjust Proxy cannot forget because proxy is not listening!";
    return;
  }

#ifdef MVPN_ANDROID
  QJniObject activity = AndroidUtils::getActivity();
  QJniObject::callStaticMethod<void>(
      "org/mozilla/firefox/vpn/qt/VPNApplication", "forget",
      "(Landroid/app/Activity;)V", activity.object());
#endif

#ifdef MVPN_IOS
  IOSAdjustHelper::forget();
#endif
}
