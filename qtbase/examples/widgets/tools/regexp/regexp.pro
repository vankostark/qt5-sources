HEADERS       = regexpdialog.h
SOURCES       = regexpdialog.cpp \
                main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tools/regexp
INSTALLS += target

QT += widgets

simulator: warning(This example might not fully work on Simulator platform)
