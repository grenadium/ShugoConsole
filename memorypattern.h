#ifndef MEMORYPATTERN_H
#define MEMORYPATTERN_H

#include <cstdint>
#include <cstring>
#include <QByteArray>

class CVarMemoryPattern
{
private:
    size_t offset;
    QByteArray pattern;
public:
    CVarMemoryPattern(size_t _offset, const QByteArray& _pattern)
        : offset(_offset), pattern(_pattern)
    {
    }

    bool match(const uint8_t *mem) const
    {
        return (mem[offset] == 0 || mem[offset] == 1) && (memcmp(mem + offset + 1, pattern.constData(), pattern.size()) == 0);
    }

    size_t totalSize() const { return offset + pattern.size(); }
};

#endif // MEMORYPATTERN_H
