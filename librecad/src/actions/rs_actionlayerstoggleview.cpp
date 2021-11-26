/****************************************************************************
**
** This file is part of the LibreCAD project, a 2D CAD program
**
** Copyright (C) 2010 R. van Twisk (librecad@rvt.dds.nl)
** Copyright (C) 2001-2003 RibbonSoft. All rights reserved.
**
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software 
** Foundation and appearing in the file gpl-2.0.txt included in the
** packaging of this file.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
**
** This copyright notice MUST APPEAR in all copies of the script!  
**
** *************************************************************************
** ChangeLog
** *************************************************************************
** This file was changed in 2016 by ravas. See ChangeLogs/r-a-v-a-s.txt
****************************************************************************/

#include "rs_actionlayerstoggleview.h"

#include <QAction>
#include "rs_graphic.h"
#include "rs_debug.h"
#include "rs_layer.h"


RS_ActionLayersToggleView::RS_ActionLayersToggleView(
                                RS_EntityContainer& container,
                                RS_GraphicView& graphicView,
                                RS_Layer* layer)
    : RS_ActionInterface("Toggle Layer Visibility", container, graphicView)
    , a_layer(layer) {}

void RS_ActionLayersToggleView::trigger() {
    RS_DEBUG->print("toggle layer");
    if (graphic) {
        RS_LayerList* ll = graphic->getLayerList();
        unsigned cnt = 0;
        // toggle selected layers
        for (auto layer: *ll) {
            if (!layer) continue;
            if (!layer->isSelectedInLayerList()) continue;
            graphic->toggleLayer(layer);
            cnt++;
        }
        // if there wasn't selected layers, toggle active layer
        if (!cnt) {
            graphic->toggleLayer(a_layer);
        }
        graphic->updateInserts();
        container->calculateBorders();
    }
    finish(false);
}

void RS_ActionLayersToggleView::init(int status) {
    RS_ActionInterface::init(status);
    trigger();
}

// EOF
