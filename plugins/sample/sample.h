/*****************************************************************************/
/*  sample.h - plugin example for LibreCAD                                   */
/*                                                                           */
/*  Copyright (C) 2011 Rallaz, rallazz@gmail.com                             */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 2 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

#ifndef SAMPLE_H
#define SAMPLE_H

#include "qc_plugininterface.h"
#include <QDialog>
class QLineEdit;

class LC_Sample : public QObject, QC_PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(QC_PluginInterface)
    Q_PLUGIN_METADATA(IID LC_DocumentInterface_iid FILE  "sample.json")

 public:
    virtual PluginCapabilities getCapabilities() const Q_DECL_OVERRIDE;
    virtual QString name() const Q_DECL_OVERRIDE;
    virtual void execComm(Document_Interface *doc,
                          QWidget *parent, QString cmd) Q_DECL_OVERRIDE;
};

class lc_Sampledlg : public QDialog
{
    Q_OBJECT

public:
    explicit lc_Sampledlg(QWidget *parent = 0);
    ~lc_Sampledlg() override;

public slots:
//    void processAction(QStringList *commandList);
    void processAction(Document_Interface *doc);
    void checkAccept();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void readSettings();
    void writeSettings();
    bool failGUI(QString *msg);

private:
    QString errmsg;
    QLineEdit *startxedit;
    QLineEdit *startyedit;
    QLineEdit *endxedit;
    QLineEdit *endyedit;
};

#endif // SAMPLE_H
