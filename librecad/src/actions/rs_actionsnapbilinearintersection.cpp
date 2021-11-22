/****************************************************************************
**
** This file is part of the LibreCAD project, a 2D CAD program
**
** Copyright (C) 2021 Melwyn Francis Carlo
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
**********************************************************************/


#include <cmath>

#include "rs_debug.h"
#include "rs_preview.h"
#include "rs_graphicview.h"
#include "rs_dialogfactory.h"

#include "rs_actionsnapbilinearintersection.h"


/*
    This action class can snap (set) the relative-zero marker
    at the imaginary intersection point of two given lines.
*/


RS_ActionSnapBilinearIntersection::RS_ActionSnapBilinearIntersection( RS_EntityContainer& container, 
                                                                      RS_GraphicView& graphicView) 
                                                                      :
                                                                      RS_PreviewActionInterface("Snap Bilinear Intersection", container, graphicView)
{
    RS_DEBUG->print("RS_ActionSnapBilinearIntersection::RS_ActionSnapBilinearIntersection");

    actionType = RS2::ActionSnapBilinearIntersection;
}


RS_ActionSnapBilinearIntersection::~RS_ActionSnapBilinearIntersection() = default;


void RS_ActionSnapBilinearIntersection::init(int status)
{
    RS_DEBUG->print("RS_ActionSnapBilinearIntersection::init");

    RS_PreviewActionInterface::init(status);
}


void RS_ActionSnapBilinearIntersection::trigger()
{
    if (firstLine == secondLine)
    {
        RS_DEBUG->print(RS_Debug::D_ERROR, "RS_ActionSnapBilinearIntersection::trigger: same line clicked twice");
        return;
    }

    const double maximumTolerance = RS_MAXDOUBLE;

    const double x1 = firstLine->getStartpoint().x;
    const double y1 = firstLine->getStartpoint().y;

    const double x2 = firstLine->getEndpoint().x;
    const double y2 = firstLine->getEndpoint().y;

    const double x3 = secondLine->getStartpoint().x;
    const double y3 = secondLine->getStartpoint().y;

    const double x4 = secondLine->getEndpoint().x;
    const double y4 = secondLine->getEndpoint().y;

    const double denominator = ((x1 - x2) * (y3 - y4)) - ((y1 - y2) * (x3 - x4));

    if (abs(denominator) == 0.0)
    {
        RS_DEBUG->print(RS_Debug::D_ERROR, "RS_ActionSnapBilinearIntersection::trigger: solution is near infinity");
        return;
    }

    const double resultantPointX = ((((x1 * y2) - (y1 * x2)) * (x3 - x4))  -  (((x3 * y4) - (y3 * x4)) * (x1 - x2))) / denominator;
    const double resultantPointY = ((((x1 * y2) - (y1 * x2)) * (y3 - y4))  -  (((x3 * y4) - (y3 * x4)) * (y1 - y2))) / denominator;

    if ((abs(resultantPointX) > maximumTolerance) || (abs(resultantPointY) > maximumTolerance))
    {
        RS_DEBUG->print(RS_Debug::D_ERROR, "RS_ActionSnapBilinearIntersection::trigger: solution is near infinity");
        return;
    }

    const RS_Vector bilinearIntersectionPoint = RS_Vector(resultantPointX, resultantPointY, 0.0);

    graphicView->moveRelativeZero(bilinearIntersectionPoint);
}


void RS_ActionSnapBilinearIntersection::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        RS_Entity *caughtEntity = catchEntity(e, RS2::EntityLine, RS2::ResolveNone);

        if (caughtEntity == nullptr)
        {
            RS_DEBUG->print(RS_Debug::D_ERROR, "RS_ActionSnapBilinearIntersection::mouseReleaseEvent: line not selected");
            return;
        }

        switch (getStatus())
        {
            case SetFirstLine:
                firstLine = caughtEntity;
                setStatus(SetSecondLine);
                updateMouseButtonHints();
                init(getStatus());
                break;

            case SetSecondLine:
                secondLine = caughtEntity;
                trigger();
                setStatus(SetFirstLine);
                updateMouseButtonHints();
                init(getStatus());
                break;
        }
    }
    else if (e->button() == Qt::RightButton)
    {
        deletePreview();

        switch (getStatus())
        {
            case SetFirstLine:
                init(-1);
                break;

            default:
                setStatus(SetFirstLine);
                init(getStatus());
        }
    }
}


void RS_ActionSnapBilinearIntersection::updateMouseButtonHints()
{
    switch (getStatus())
    {
        case SetFirstLine:
            RS_DIALOGFACTORY->updateMouseWidget( tr("Select the first line"), 
                                                 tr("Cancel"));
            break;

        case SetSecondLine:
            RS_DIALOGFACTORY->updateMouseWidget( tr("Select the second line"), 
                                                 tr("Cancel"));
            break;

        default:
            RS_DIALOGFACTORY->updateMouseWidget();
            break;
    }
}

