#include "AboutDialog.h"

#include <QLabel>
#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QScrollArea>

#include "configure.h"

namespace Absencoid {

/* Kontruktor */
AboutDialog::AboutDialog(QWidget* parent): QDialog(parent) {
    setWindowTitle(tr("O programu"));

    QLabel* name = new QLabel(tr(
    "<big><strong>Absencoid %1</strong></big><br />"
    "<em>Inspired by Tylakoid</em><br /><br />"
    #ifdef ADMIN_VERSION
    "Správcovská edice"
    #else
    "Uživatelská edice"
    #endif
    "<br />Verze: %1 (%2-%3)<br />")
    .arg(APP_VERSION_LONG).arg(APP_VERSION).arg(SVN_VERSION));

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

    /* Layout */
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(name);
    layout->addWidget(disclaimerLabel);
    layout->addWidget(area);
    layout->addWidget(buttons);

    setLayout(layout);

    buttons->setFocus();

    /* Velikost okna */
    setFixedSize(400, 300);
}

}
