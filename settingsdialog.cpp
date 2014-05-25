#include "settingsdialog.h"
#include "ui_settingsdialog.h"
//#include "checkersstate.h"
#include "checkersgame.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
	ui->spinBox->setMinimum(3);
	ui->spinBox->setMaximum(7);
	ui->comboBoxColor->addItem(tr("White"));
	ui->comboBoxColor->addItem(tr("Black"));

	connect( this, SIGNAL(accepted()), this, SLOT(saveSettings()));
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::saveSettings() {
    QSettings settings("QCheckers");
    int color, depth;
    if ( ui->comboBoxColor->currentIndex() == 0)
		color = WHITE;
	else
		color = BLACK;
	depth = ui->spinBox->value();
	settings.setValue("color",color);
	settings.setValue("depth",depth);
}

void SettingsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
