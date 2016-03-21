// formatnumber.h

// Declares the functions for formatting numbers





#ifndef FORMATNUMBER_H
#define FORMATNUMBER_H





#include <QString>





/** Formats the number with a space as thousand separator, to make it more readable. */
QString formatBigNumber(quint64 a_Number);

/** Formats the number as memory size, converts to KiB (rounds up) and uses space as thousand separator. */
QString formatMemorySize(quint64 a_Size);





#endif // FORMATNUMBER_H




