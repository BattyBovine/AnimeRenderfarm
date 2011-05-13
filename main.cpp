#include <QtGui/QApplication>
#include "animerenderfarm.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("Batty Bovine Productions, LLC");
    a.setOrganizationDomain("battybovine.com");
    a.setApplicationName("Anime Renderfarm");
    a.setApplicationVersion("0.1");

    AnimeRenderfarm w;
    w.show();
    return a.exec();
}
