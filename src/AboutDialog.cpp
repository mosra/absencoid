#include "AboutDialog.h"

#include <QLabel>
#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QScrollArea>
#include <QIcon>

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
    "<br />Verze: %1 (%2-%3)<br /><br />"
    "Program, logo, ikonky: © <em>Mosra</em>.<br />"
    "Dotazy a nalezené chyby pište na <a href=\"mailto:mosra@centrum.cz\">"
    "mosra@centrum.cz</a> (mail) nebo mosra@jabbim.cz (jabber).")
    .arg(APP_VERSION_LONG).arg(APP_VERSION).arg(SVN_VERSION));
    name->setWordWrap(true);

    /* Disclaimer */
    QLabel* disclaimerLabel = new QLabel(tr("Zřeknutí se odpovědnosti:"));
    QLabel* disclaimer = new QLabel(tr(
    "<strong>Autor v žádném případě neodpovídá za škody způsobené vaší "
    "přílišnou důvěrou v tento amatérský pokus o uchránění vás před komiskami. "
    "Data vypočtená tímto programem už z principu zákona schválnosti nemohou "
    "být úplně správná, takže je vyloženě hovadina na ně spoléhat a hákovat "
    "jak o život."
    "<br /><br />"
    "Toto prohlášení platí i pro druhou stranu barikády, autor nenese žádnou "
    "odpovědnost za nárůst počtu absencí, za což by se této aplikaci nebo "
    "nedejbože autorovi mohla mylně klást vina. Tato aplikace není příčina, "
    "ale důsledek absencí.</strong>"
    "<br /><br />"
    "Odpovědnost se nevztahuje ani na poškození softwaru, hardwaru či jiné "
    "smrtelné újmy na zdraví, způsobené přímo i nepřímo používáním této "
    "aplikace ani jejích odvozenin."));
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
