// formatnumber.cpp

// Declares the functions for formatting numbers





#include "globals.h"
#include "formatnumber.h"





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





QString formatMemorySize(quint64 a_Size)
{
	return formatBigNumber((a_Size + 1023) / 1024);
}




