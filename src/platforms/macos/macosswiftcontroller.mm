#include "macosswiftcontroller.h"
#include "keys.h"
#include "device.h"
#include "server.h"
#include "Firefox_Private_Network_VPN-Swift.h"

#import <Cocoa/Cocoa.h>

#include <QDebug>
#include <QByteArray>

// Our Swift singleton.
static MacOSControllerImpl *impl = nullptr;

// static
void MacOSSwiftController::activate(const Server* server, std::function<void(bool)> && a_callback)
{
    qDebug() << "MacOSSWiftController - activate";

    Q_ASSERT(impl);

    std::function<void(bool)> callback = std::move(a_callback);

    [impl connectWithServerIpv4Gateway:server->ipv4Gateway().toNSString()
                     serverIpv6Gateway:server->ipv6Gateway().toNSString()
                       serverPublicKey:server->publicKey().toNSString()
                      serverIpv4AddrIn:server->ipv4AddrIn().toNSString()
                            serverPort:server->choosePort()
                               closure:^(BOOL status) {
                                   qDebug() << "MacOSSWiftController - connect status:" << status;
                                   callback(status);
                               }];
}

// static
void MacOSSwiftController::deactivate(std::function<void(bool)> && a_callback)
{
    Q_ASSERT(impl);
    qDebug() << "MacOSSWiftController - deactivate";

    std::function<void(bool)> callback = std::move(a_callback);

    [impl disconnectWithClosure:^(BOOL status) {
        qDebug() << "MacOSSWiftController - disconnect status:" << status;
        callback(status);
    }];
}

// static
void MacOSSwiftController::maybeInitialize(const Device* device, const Keys* keys, std::function<void(bool)>&& a_callback)
{
    std::function<void(bool)> callback = std::move(a_callback);

    if (impl) {
        callback(true);
        return;
    }

    qDebug() << "Initializing Swift Controller";

    static bool creating = false;
    // No nested creation!
    Q_ASSERT(creating == false);
    creating = true;

    QByteArray key = QByteArray::fromBase64(keys->privateKey().toLocal8Bit());

    impl = [[MacOSControllerImpl alloc] initWithPrivateKey:key.toNSData()
                                               ipv4Address:device->ipv4Address().toNSString()
                                               ipv6Address:device->ipv6Address().toNSString()
                                                   closure:^(BOOL status) {
                                                       qDebug() << "Creation completed with status"
                                                                << status;
                                                       creating = false;
                                                       if (status == false) {
                                                           [impl dealloc];
                                                           impl = nullptr;
                                                       }
                                                       callback(status);
                                                   }];
}
