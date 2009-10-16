#include "Style.h"

namespace Absencoid {

Style* Style::object = 0;

/* Konstruktor */
Style::Style(const QString& file, QObject* parent): QObject(parent), iconFile(file) {
    object = this;
}

/* Získání ikony */
const QIcon& Style::icon(Icon icon) {

    /* Jestli ještě není ikona načtená, načteme ji */
    if(!icons.contains(icon))
        icons[icon] = iconFile.copy(((int) icon & 0x0F)*16, ((int) icon & 0xF0), 16, 16);

    return icons[icon];
}

}
