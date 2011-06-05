/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                         *
 *  Anime Renderfarm - A remote batch renderer for Anime Studio            *
 *  Copyright (C) 2011 Batty Bovine Productions                            *
 *                                                                         *
 *  This program is free software: you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation, either version 3 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <QtGui/QApplication>
#include "animerenderfarm.h"
#include "dialogues/preferences.h"
#include "objects/serverthread.h"

bool cmdArgs(QStringList args)
{
    bool server = false;
    if(args.lastIndexOf("-server")>=0)
        server = true;

    Preferences *prefsman = new Preferences(NULL);

    if(args.lastIndexOf("-exe")>=0) {
        prefsman->saveAnimeStudioPath(args.at(args.lastIndexOf("-exe")+1)); }
    if(args.lastIndexOf("-out")>=0) {
        prefsman->saveOutputDirectory(args.at(args.lastIndexOf("-out")+1)); }
    if(args.lastIndexOf("-format")>=0) {
        prefsman->saveOutputFormat(args.at(args.lastIndexOf("-format")+1).toInt()); }

    prefsman->saveSetFrameRange(true);

    if(args.lastIndexOf("-startframe")>=0) {
        prefsman->saveStartFrame(args.at(args.lastIndexOf("-startframe")+1).toInt());
        prefsman->saveSetFrameRange(true); }
    else { prefsman->saveSetFrameRange(false); }
    if(args.lastIndexOf("-endframe")>=0) {
        prefsman->saveEndFrame(args.at(args.lastIndexOf("-endframe")+1).toInt());
        prefsman->saveSetFrameRange(true); }
    else { prefsman->saveSetFrameRange(false); }

    if(args.lastIndexOf("-aa")>=0) {
        prefsman->saveAntialiasedEdges(args.at(args.lastIndexOf("-aa")+1)=="yes"?true:false); }
    if(args.lastIndexOf("-shapefx")>=0) {
        prefsman->saveApplyShapeEffects(args.at(args.lastIndexOf("-shapefx")+1)=="yes"?true:false); }
    if(args.lastIndexOf("-layerfx")>=0) {
        prefsman->saveApplyLayerEffects(args.at(args.lastIndexOf("-layerfx")+1)=="yes"?true:false); }
    if(args.lastIndexOf("-halfsize")>=0) {
        prefsman->saveRenderAtHalfDimensions(args.at(args.lastIndexOf("-halfsize")+1)=="yes"?true:false); }
    if(args.lastIndexOf("-halffps")>=0) {
        prefsman->saveRenderAtHalfFramerate(args.at(args.lastIndexOf("-halffps")+1)=="yes"?true:false); }
    if(args.lastIndexOf("-fewparticles")>=0) {
        prefsman->saveReducedParticles(args.at(args.lastIndexOf("-fewparticles")+1)=="yes"?true:false); }
    if(args.lastIndexOf("-extrasmooth")>=0) {
        prefsman->saveExtraSmoothImages(args.at(args.lastIndexOf("-extrasmooth")+1)=="yes"?true:false); }
    if(args.lastIndexOf("-ntscsafe")>=0) {
        prefsman->saveUseNTSCSafeColours(args.at(args.lastIndexOf("-ntscsafe")+1)=="yes"?true:false); }
    if(args.lastIndexOf("-premultiply")>=0) {   // This option is weird
        prefsman->saveDoNotPremultiplyAlpha(args.at(args.lastIndexOf("-premultiply")+1)=="yes"?true:false); }
    if(args.lastIndexOf("-nopremultiply")>=0) {
        prefsman->saveDoNotPremultiplyAlpha(!(args.at(args.lastIndexOf("-nopremultiply")+1)=="yes"?true:false)); }
    if(args.lastIndexOf("-variablewidths")>=0) {
        prefsman->saveVariableLineWidths(args.at(args.lastIndexOf("-variablewidths")+1)=="yes"?true:false); }

    prefsman->deleteLater();
    return server;
}



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("Batty Bovine Productions, LLC");
    a.setOrganizationDomain("animerenderfarm.battybovine.com");
    a.setApplicationName("Anime Renderfarm");
    a.setApplicationVersion("0.1.1");

    // Handle command line arguments
    if(cmdArgs(a.arguments())) {
        // If the arguments indicate we want to run in server mode, do so
        ServerThread *st = new ServerThread();
        st->start();
    } else {
        // Otherwise, start the main window of the GUI interface
        AnimeRenderfarm w;
        w.show();
        return a.exec();
    }
}
