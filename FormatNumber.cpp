// FormatNumber.cpp

// Declares the functions for formatting numbers





#include "Globals.h"
#include "FormatNumber.h"





QString formatBigNumber(quint64 a_Number)
{
	QString res("%1");
	res = res.arg(a_Number);

	// Insert spaces as a thousand-separator:
	auto i = res.lastIndexOf('.');
	if (i < 0)
	{
		i = res.length();
	}
	i -= 3;
	while(i > 0)
	{
		res.insert(i, ' ');
		i -= 3;
	}
	return res;
}





QString formatBigSignedNumber(qint64 a_Number)
{
	if (a_Number >= 0)
	{
		return formatBigNumber(static_cast<quint64>(a_Number));
	}
	else
	{
		return QString("-") + formatBigNumber(static_cast<quint64>(-a_Number));
	}
}





QString formatMemorySize(quint64 a_Size)
{
	return formatBigNumber((a_Size + 1023) / 1024);
}





QString formatSignedMemorySize(qint64 a_Size)
{
	if (a_Size >= 0)
	{
		return formatBigNumber(static_cast<quint64>((a_Size + 1023) / 1024));
	}
	a_Size = -a_Size;
	return QString("-") + formatBigNumber(static_cast<quint64>((a_Size + 1023) / 1024));
}




