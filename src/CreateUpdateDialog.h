#ifndef ABSENCOID_CREATEUPDATEDIALOG_H
#define ABSENCOID_CREATEUPDATEDIALOG_H

#include <QDialog>

class QDialogButtonBox;
class QLineEdit;
class QTextEdit;

namespace Absencoid {

class CreateUpdateDialog: public QDialog {
    Q_OBJECT

    public:
        /**
         * @brief Konstruktor
         *
         * @param   file    Ukazatel pro uložení získaného jména souboru
         * @param   note    Ukazatel pro uložení popisku aktualizace
         * @param   parent  Rodičovský widget
         */
        CreateUpdateDialog(QString& _file, QString& _note, QWidget* parent = 0);

    private:
        QString& file;              /** @brief Ukazatel pro uložení získaného jména souboru */
        QString& note;              /** @brief Ukazatel pro uložení popisku aktualizace */

        QLineEdit* filenameEdit;    /** @brief Linka se jménem souboru */
        QTextEdit* noteEdit;        /** @brief Textové políčko s popiskem aktualizace */
        QDialogButtonBox* buttons;  /** @brief Tlačítka dialogu */

    public slots:
        /**
         * @brief Potvrzení dialogu
         */
        virtual void accept();

    private slots:
        /**
         * @brief Zobrazení dialogu pro uložení souboru
         */
        void openFileDialog();

        /**
         * @brief Zkontrolování, zda můžeme odšednout tlačítko pro uložení
         */
        void checkAcceptButton();
};

}

#endif
