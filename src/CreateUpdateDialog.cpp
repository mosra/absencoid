#include "CreateUpdateDialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QStyle>
#include <QDialogButtonBox>
#include <QBoxLayout>
#include <QFileDialog>

namespace Absencoid {

/* Konstruktor */
CreateUpdateDialog::CreateUpdateDialog(QString& _file, QString& _note, QWidget* parent): QDialog(parent), file(_file), note(_note), filenameEdit(new QLineEdit), noteEdit(new QTextEdit) {

    /* Linka (jen pro čtení) s tlačítkem pro výběr souboru */
    filenameEdit->setReadOnly(true);
    QPushButton* fileButton = new QPushButton(style()->standardIcon(QStyle::SP_DialogOpenButton).pixmap(16, 16), QString());
    connect(fileButton, SIGNAL(clicked(bool)), this, SLOT(openFileDialog()));

    /* Layout pro linku s tlačítkem */
    QHBoxLayout* fileLayout = new QHBoxLayout;
    fileLayout->addWidget(filenameEdit, 1);
    fileLayout->addWidget(fileButton, 0);

    /* Textové políčko (jen čistý text) */
    noteEdit->setAcceptRichText(false);

    /* Tlačítka pro uložení a storno, jejich propojení s akcemi */
    buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));

    /* Zašednutí ukládacího tlačítka */
    checkAcceptButton();

    /* Celkový layout */
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(new QLabel(tr("Soubor pro uložení aktualizace:")));
    layout->addLayout(fileLayout);
    layout->addWidget(new QLabel(tr("Popisek aktualizace:")));
    layout->addWidget(noteEdit, 1);
    layout->addWidget(buttons);

    setLayout(layout);

    /* Titulek a velikost okna */
    setWindowTitle(tr("Vytvořit aktualizaci"));
    setFixedSize(480, 360);
}

/* Otevření souborového dialogu */
void CreateUpdateDialog::openFileDialog() {
    QString filename = QFileDialog::getSaveFileName(this, tr("Uložit aktualizaci"), "", tr("XML soubory (*.xml)"));

    /* Pokud jsme soubor nevybrali, necháme tam starý */
    if(!filename.isEmpty()) filenameEdit->setText(filename);

    /* Zkusíme odšednout ukládací tlačítko */
    checkAcceptButton();
}

/* Pokud o odšednutí ukládacího tlačítka */
void CreateUpdateDialog::checkAcceptButton() {
    /* Není vyplněn název souboru, zašednutí */
    if(filenameEdit->text().isEmpty())
        buttons->button(QDialogButtonBox::Save)->setDisabled(true);

    /* Vše je vyplněno, odšednutí */
    else buttons->button(QDialogButtonBox::Save)->setDisabled(false);
}

/* Potvrzení dialogu */
void CreateUpdateDialog::accept() {
    if(filenameEdit->text().isEmpty()) return;

    file = filenameEdit->text();
    note = noteEdit->document()->toPlainText();

    done(QDialog::Accepted);
}

}
