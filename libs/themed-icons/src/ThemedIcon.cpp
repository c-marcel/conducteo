#include "ThemedIcon.h"
#include "ThemedIconEngine.h"

namespace ThemedIcon
{
    QIcon get(const QString& resourcePath)
    {
        // QIcon prend possession de l'engine et gere sa duree de vie.
        return QIcon(new ThemedIconEngine(resourcePath));
    }
}
