include("Tests.pri")

SOURCES += $${PWD}/Tests.cpp

CONFIG += qtestlib
macx {
  CONFIG -= app_bundle
}
TARGET = Tests
