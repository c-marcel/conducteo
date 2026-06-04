#ifndef THEMEDICONENGINE_H
#define THEMEDICONENGINE_H

#include <QIconEngine>
#include <QByteArray>
#include <QString>

/*! \brief Moteur d'icone "theme-aware" pour les SVG Tabler du projet.
 *
 *  Probleme resolu :
 *  Les 63 icones SVG utilisent stroke="currentColor". QtSvg (SVG Tiny) ne gere
 *  pas correctement currentColor : il rend alors en noir, ce qui les rend
 *  quasi invisibles en theme sombre. Par ailleurs, 5 icones de case a cocher
 *  (dxf-on/off, image-on/off, save-as) ont un interieur de case en
 *  fill:#ffffff code en dur, qui doit suivre la couleur de FOND, pas celle du
 *  trait.
 *
 *  Approche :
 *  A chaque rendu, on lit la couleur courante de la palette applicative et on
 *  reecrit le contenu du SVG avant de le confier a QSvgRenderer :
 *    - currentColor      -> palette WindowText  (le trait : visible en clair ET sombre)
 *    - fill:#ffffff       -> palette Base        (l'interieur des cases : suit le fond)
 *  Le rendu suit ainsi automatiquement tout changement de palette (la mise a
 *  jour effective des widgets a chaud necessite en plus de propager un
 *  QEvent::PaletteChange ; voir ThemedIcon::installAutoRefresh()).
 *
 *  Le mode QIcon::Disabled est emule (QSvgRenderer n'ayant pas de notion de
 *  mode, ni en Qt6) en reduisant l'opacite, approche heritee d'OpenMW.
 *
 *  Cible : Qt6. currentColor n'est toujours pas resolu par QSvgRenderer en
 *  Qt6 ; la substitution de chaine reste donc la methode adaptee.
 */
class ThemedIconEngine : public QIconEngine
{
public:
    //! Construit le moteur a partir d'une ressource SVG (ex: ":/icons/copy.svg").
    explicit ThemedIconEngine(const QString& resourcePath);

    //! Rend l'icone dans \a painter, sur le rectangle \a rect.
    void paint(QPainter* painter, const QRect& rect,
               QIcon::Mode mode, QIcon::State state) override;

    //! Pixmap a la taille demandee (delegue a scaledPixmap, scale=1).
    QPixmap pixmap(const QSize& size, QIcon::Mode mode,
                   QIcon::State state) override;

    //! Pixmap HiDPI : \a scale vaut typiquement le device pixel ratio.
    //! Virtuelle directe depuis Qt6 (en Qt5 elle passait par virtual_hook).
    QPixmap scaledPixmap(const QSize& size, QIcon::Mode mode,
                         QIcon::State state, qreal scale) override;

    //! Clone requis par QIcon (copie profonde du SVG source).
    QIconEngine* clone() const override;

    //! Nom de l'engine (utile au debug / serialisation).
    QString key() const override;

private:
    //! Rend le SVG recolorise dans un pixmap de \a size px physiques.
    QPixmap renderColored(const QSize& size, qreal dpr, QIcon::Mode mode) const;

    //! Renvoie le SVG source avec les couleurs substituees selon la palette.
    QByteArray themedSvg(QIcon::Mode mode) const;

    QString    m_resourcePath; //!< Chemin de ressource d'origine (pour clone/key).
    QByteArray m_svgTemplate;  //!< Contenu brut du SVG, charge une fois.
};

#endif // THEMEDICONENGINE_H
