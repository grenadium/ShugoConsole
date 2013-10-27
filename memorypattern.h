#ifndef MEMORYPATTERN_H
#define MEMORYPATTERN_H

#include <cstdint>
#include <cstring>
#include <QByteArray>

class MemoryPattern
{
private:
    size_t offset;
    QByteArray pattern;
public:
    MemoryPattern(size_t _offset, const QByteArray& _pattern)
        : offset(_offset), pattern(_pattern)
    {
    }

    bool match(const uint8_t *mem) const
    {
        return memcmp(mem + offset, pattern.constData(), pattern.size()) == 0;
    }

    size_t totalSize() const { return offset + pattern.size(); }
};

#endif // MEMORYPATTERN_H
