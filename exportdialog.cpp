/**************************************************************************
**
** This file is part of PhyxCalc.
**
** PhyxCalc is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** PhyxCalc is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with PhyxCalc.  If not, see <http://www.gnu.org/licenses/>.
**
***************************************************************************/

#include "exportdialog.h"
#include "ui_exportdialog.h"

ExportDialog::ExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportDialog)
{
    ui->setupUi(this);

    QPushButton *button = ui->buttonBox->addButton(tr("Copy to Clipboard"), QDialogButtonBox::ActionRole);
    connect(button, SIGNAL(clicked()),
            this, SLOT(copyToClipboard()));

#if defined(Q_OS_SYMBIAN) || defined(Q_OS_ANDROID) || defined(Q_WS_SIMULATOR)
    this->setWindowState(Qt::WindowMaximized);
#else
    button = ui->buttonBox->addButton(tr("Open Formel Editor"), QDialogButtonBox::ActionRole);
    connect(button, SIGNAL(clicked()),
            this, SLOT(openFormelEditor()));
#endif
}

ExportDialog::~ExportDialog()
{
    delete ui;
}

void ExportDialog::setText(QString text)
{
    ui->plainTextEdit->setPlainText(text);
}

void ExportDialog::copyToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->plainTextEdit->toPlainText());
}

void ExportDialog::openFormelEditor()
{
    bool ok = system("libreoffice --math");
}
