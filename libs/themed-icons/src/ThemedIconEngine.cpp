#include "ThemedIconEngine.h"

#include <QApplication>
#include <QPalette>
#include <QPainter>
#include <QPixmap>
#include <QFile>
#include <QSvgRenderer>
#include <QColor>

namespace
{
    //! Convertit une QColor en notation SVG "#rrggbb".
    QString toSvgHex(const QColor& c)
    {
        return c.name(QColor::HexRgb); // ex: "#1e1e1e"
    }
}

ThemedIconEngine::ThemedIconEngine(const QString& resourcePath)
    : m_resourcePath(resourcePath)
{
    // Charge le SVG une seule fois ; la recoloration se fera a chaque rendu.
    QFile f(resourcePath);
    if (f.open(QIODevice::ReadOnly))
        m_svgTemplate = f.readAll();
}

QByteArray ThemedIconEngine::themedSvg(QIcon::Mode mode) const
{
    const QPalette pal = QApplication::palette();

    // Couleur du trait : le texte/avant-plan. Suit clair comme sombre.
    QColor stroke = pal.color(QPalette::WindowText);
    // Couleur de l'interieur des cases a cocher : le fond.
    const QColor fill = pal.color(QPalette::Base);

    // Pour le mode "selected" (survol/selection dans une vue), Qt fournit
    // une couleur de texte dediee plus lisible sur fond de selection.
    if (mode == QIcon::Selected)
        stroke = pal.color(QPalette::HighlightedText);

    QByteArray svg = m_svgTemplate;

    // 1) Le trait : remplacer currentColor par la couleur d'avant-plan.
    //    Couvre stroke="currentColor" et fill="currentColor".
    svg.replace("currentColor", toSvgHex(stroke).toUtf8());

    // 2) L'interieur des cases : le blanc code en dur suit le fond.
    //    Couvre la forme CSS (style="fill:#ffffff") et l'attribut (fill="#ffffff"),
    //    en #fff comme en #ffffff, insensible a la casse via les deux variantes.
    const QByteArray fillHex = toSvgHex(fill).toUtf8();
    svg.replace("fill:#ffffff", "fill:" + fillHex);
    svg.replace("fill:#FFFFFF", "fill:" + fillHex);
    svg.replace("fill:#fff;",   "fill:" + fillHex + ";");
    svg.replace("fill=\"#ffffff\"", "fill=\"" + fillHex + "\"");
    svg.replace("fill=\"#FFFFFF\"", "fill=\"" + fillHex + "\"");

    return svg;
}

QPixmap ThemedIconEngine::renderColored(const QSize& size, qreal dpr,
                                        QIcon::Mode mode) const
{
    if (m_svgTemplate.isEmpty() || size.isEmpty())
        return QPixmap();

    // Rend a la resolution physique (taille logique * device pixel ratio)
    // pour rester net en HiDPI.
    const QSize physical = size * dpr;

    QPixmap pm(physical);
    pm.fill(Qt::transparent);

    QSvgRenderer renderer(themedSvg(mode));

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Mode desactive : QSvgRenderer ne gere pas QIcon::Disabled, on emule
    // en abaissant l'opacite (approche OpenMW).
    if (mode == QIcon::Disabled)
        p.setOpacity(0.35);

    renderer.render(&p);
    p.end();

    pm.setDevicePixelRatio(dpr);
    return pm;
}

void ThemedIconEngine::paint(QPainter* painter, const QRect& rect,
                             QIcon::Mode mode, QIcon::State state)
{
    Q_UNUSED(state); // l'etat on/off est porte par le contenu du SVG lui-meme
    const qreal dpr = painter->device()
                          ? painter->device()->devicePixelRatioF()
                          : 1.0;
    const QPixmap pm = renderColored(rect.size(), dpr, mode);
    painter->drawPixmap(rect, pm);
}

QPixmap ThemedIconEngine::pixmap(const QSize& size, QIcon::Mode mode,
                                 QIcon::State state)
{
    // Delegue a scaledPixmap : en Qt6 c'est le point d'entree privilegie,
    // QIcon appelant scaledPixmap avec le bon scale pour le HiDPI.
    const qreal dpr = qApp ? qApp->devicePixelRatio() : 1.0;
    return scaledPixmap(size, mode, state, dpr);
}

QPixmap ThemedIconEngine::scaledPixmap(const QSize& size, QIcon::Mode mode,
                                       QIcon::State state, qreal scale)
{
    Q_UNUSED(state); // l'etat on/off est porte par le contenu du SVG lui-meme
    return renderColored(size, scale, mode);
}

QIconEngine* ThemedIconEngine::clone() const
{
    return new ThemedIconEngine(m_resourcePath);
}

QString ThemedIconEngine::key() const
{
    return QStringLiteral("ThemedIconEngine");
}
