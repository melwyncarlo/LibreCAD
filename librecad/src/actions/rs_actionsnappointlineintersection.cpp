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


#include "rs_debug.h"
#include "rs_preview.h"
#include "rs_graphicview.h"
#include "rs_dialogfactory.h"

#include "rs_actionsnappointlineintersection.h"


/*
    This action class can snap (set) the relative-zero marker
    at the imaginary perpendicular intersection point of 
    a given point and a given line.
*/


RS_ActionSnapPointLineIntersection::RS_ActionSnapPointLineIntersection( RS_EntityContainer& container, 
                                                                        RS_GraphicView& graphicView) 
                                                                        :
                                                                        RS_PreviewActionInterface("Snap Point Line Intersection", container, graphicView)
{
    RS_DEBUG->print("RS_ActionSnapPointLineIntersection::RS_ActionSnapPointLineIntersection");

    actionType = RS2::ActionSnapPointLineIntersection;
}


RS_ActionSnapPointLineIntersection::~RS_ActionSnapPointLineIntersection() = default;


void RS_ActionSnapPointLineIntersection::init(int status)
{
    RS_DEBUG->print("RS_ActionSnapPointLineIntersection::init");

    if (status == SetFirstPointOrLine)
    {
        inputPoint = nullptr;
        inputLine  = nullptr;
    }

    RS_PreviewActionInterface::init(status);
}


void RS_ActionSnapPointLineIntersection::trigger()
{
    const double maximumTolerance = RS_MAXDOUBLE;

    const double x1 = inputLine->getStartpoint().x;
    const double y1 = inputLine->getStartpoint().y;

    const double x2 = inputLine->getEndpoint().x;
    const double y2 = inputLine->getEndpoint().y;

    const double x3 = inputPoint->getStartpoint().x;
    const double y3 = inputPoint->getStartpoint().y;

    const double k = (((x3 - x1) * (x2 - x1)) + ((y3 - y1) * (y2 - y1))) 
                   / (((x2 - x1) * (x2 - x1)) + ((y2 - y1) * (y2 - y1)));

    if (abs(k) > maximumTolerance)
    {
        RS_DEBUG->print(RS_Debug::D_ERROR, "RS_ActionSnapBilinearIntersection::trigger: solution is very large");
        return;
    }

    const double resultantPointX = x1 + (k * (x2 - x1));
    const double resultantPointY = y1 + (k * (y2 - y1));

    if ((abs(resultantPointX) > maximumTolerance) || (abs(resultantPointY) > maximumTolerance))
    {
        RS_DEBUG->print(RS_Debug::D_ERROR, "RS_ActionSnapBilinearIntersection::trigger: solution is very large");
        return;
    }

    const RS_Vector pointLineIntersectionPoint = RS_Vector(resultantPointX, resultantPointY, 0.0);

    graphicView->moveRelativeZero(pointLineIntersectionPoint);
}


void RS_ActionSnapPointLineIntersection::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        RS_Entity *caughtEntity = catchEntity(e, RS2::EntityPoint, RS2::ResolveNone);

        if ((caughtEntity == nullptr) || (inputPoint != nullptr))
        {
            if (inputLine != nullptr)
            {
                RS_DEBUG->print(RS_Debug::D_ERROR, "RS_ActionSnapBilinearIntersection::mouseReleaseEvent: point not selected");
                return;
            }

            caughtEntity = catchEntity(e, RS2::EntityLine, RS2::ResolveNone);

            if (caughtEntity == nullptr)
            {
                if (inputPoint != nullptr)
                {
                    RS_DEBUG->print(RS_Debug::D_ERROR, "RS_ActionSnapBilinearIntersection::mouseReleaseEvent: line not selected");
                }
                else
                {
                    RS_DEBUG->print(RS_Debug::D_ERROR, "RS_ActionSnapBilinearIntersection::mouseReleaseEvent: point or line not selected");
                }
                return;
            }

            inputLine = caughtEntity;

            if (getStatus() == SetFirstPointOrLine) setStatus(SetSecondPoint);
            else
            {
                trigger();
                setStatus(SetFirstPointOrLine);
            }
        }
        else
        {
            inputPoint = caughtEntity;

            if (getStatus() == SetFirstPointOrLine) setStatus(SetSecondLine);
            else
            {
                trigger();
                setStatus(SetFirstPointOrLine);
            }
        }

        updateMouseButtonHints();
        init(getStatus());
    }
    else if (e->button() == Qt::RightButton)
    {
        deletePreview();

        switch (getStatus())
        {
            case SetFirstPointOrLine:
                init(-1);
                break;

            default:
                setStatus(SetFirstPointOrLine);
                init(getStatus());
        }
    }
}


void RS_ActionSnapPointLineIntersection::updateMouseButtonHints()
{
    switch (getStatus())
    {
        case SetFirstPointOrLine:
            RS_DIALOGFACTORY->updateMouseWidget( tr("Select a point or a line"), 
                                                 tr("Cancel"));
            break;

        case SetSecondPoint:
            RS_DIALOGFACTORY->updateMouseWidget( tr("Select a point"), 
                                                 tr("Cancel"));
            break;

        case SetSecondLine:
            RS_DIALOGFACTORY->updateMouseWidget( tr("Select a line"), 
                                                 tr("Cancel"));
            break;

        default:
            RS_DIALOGFACTORY->updateMouseWidget();
            break;
    }
}

