/** 
 * Copyright 2011 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */ 


#include <QDebug>

#ifdef INTERFACE_COMMANDLINE
#include <QCoreApplication>
#include "CLI/CLI.h"
#endif

#ifdef INTERFACE_GRAPHICAL
#include <QApplication>
#include <QSettings>
#include "UI/MainWindow.h"
#endif

int main(int argc, char *argv[]) {
#ifdef INTERFACE_COMMANDLINE
    QCoreApplication app(argc, argv);

    CLI * cli = new CLI();
    if (!cli->start())
        exit(1);

    return app.exec();
#endif

#ifdef INTERFACE_GRAPHICAL
    const int RESTART_CODE = 1000;
    int r;

    do {
        QApplication app(argc, argv);

        QCoreApplication::setOrganizationName("WimLeers");
        QCoreApplication::setOrganizationDomain("wimleers.com");
        QCoreApplication::setApplicationName("WPO Analytics");

        MainWindow * mainWindow = new MainWindow();
        mainWindow->show();

        r = app.exec();
    } while (r == RESTART_CODE);

    return r;
#endif
}
