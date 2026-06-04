#ifndef THEMEDICON_H
#define THEMEDICON_H

#include <QIcon>
#include <QString>

/*! \brief Fabrique d'icones theme-aware.
 *
 *  Remplace les appels directs QIcon(":/icons/x.svg") par
 *  ThemedIcon::get(":/icons/x.svg"). L'icone retournee est adossee a un
 *  ThemedIconEngine qui recolorise le SVG selon la palette applicative a
 *  chaque rendu (trait -> WindowText, interieur de case -> Base).
 *
 *  Migration : remplacer dans tout le code
 *      QIcon(":/icons/copy.svg")
 *  par
 *      ThemedIcon::get(":/icons/copy.svg")
 *
 *  Note "a chaud" (pour une PR ulterieure) : un QIconEngine recalcule la
 *  couleur a chaque paint(), mais Qt ne repeint pas spontanement toutes les
 *  icones quand la palette change. Pour un basculement clair/sombre sans
 *  redemarrage, il faudra, a la reception de QEvent::ApplicationPaletteChange,
 *  forcer le rafraichissement des widgets concernes (update()).
 */
namespace ThemedIcon
{
    //! Retourne une QIcon recolorisable a partir d'une ressource SVG.
    QIcon get(const QString& resourcePath);
}

#endif // THEMEDICON_H
