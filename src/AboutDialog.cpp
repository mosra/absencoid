#include "AboutDialog.h"

#include <QLabel>
#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QScrollArea>
#include <QIcon>
#include <QFile>

#include "configure.h"

namespace Absencoid {

/* Kontruktor */
AboutDialog::AboutDialog(QWidget* parent): QDialog(parent) {
    setWindowIcon(QIcon(":/icon.png"));
    setWindowTitle(tr("O programu"));

    QLabel* logo = new QLabel;
    logo->setPixmap(QPixmap(":/logo.png"));

    QLabel* name = new QLabel(tr(
    "<big><strong>Absencoid %1</strong></big><br />"
    "<em>Inspired by Tylakoid</em><br /><br />"
    #ifdef ADMIN_VERSION
    "Správcovská edice"
    #else
    "Uživatelská edice"
    #endif
    "<br />Verze: %1 (%2)<br /><br />"
    "Program, logo, ikonky: © <em>Mosra</em>.<br />"
    "Dotazy a nalezené chyby pište na <a href=\"mailto:mosra@centrum.cz\">"
    "mosra@centrum.cz</a> (mail) nebo mosra@jabbim.cz (jabber).")
    .arg(APP_VERSION_LONG).arg(APP_VERSION));
    name->setWordWrap(true);

    /* Disclaimer */
    QLabel* disclaimerLabel = new QLabel(tr("Zřeknutí se odpovědnosti:"));

    /* Text disclaimeru ze souboru */
    QFile file(":/disclaimer.txt");
    file.open(QFile::ReadOnly | QFile::Text);
    QLabel* disclaimer = new QLabel(QString::fromUtf8(file.readAll().data()));
    disclaimer->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    disclaimer->setWordWrap(true);

    QScrollArea* area = new QScrollArea;
    area->setFrameShape(QFrame::NoFrame);
    area->setWidget(disclaimer);
    area->setWidgetResizable(true);

    /* Tlačítka */
    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));

    /* Layout loga a jména */
    QHBoxLayout* logoLayout = new QHBoxLayout;
    logoLayout->addWidget(logo);
    logoLayout->addWidget(name);

    /* Layout */
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(logoLayout);
    layout->addWidget(disclaimerLabel);
    layout->addWidget(area);
    layout->addWidget(buttons);

    setLayout(layout);

    buttons->setFocus();

    /* Velikost okna */
    setFixedSize(400, 360);
}

}
