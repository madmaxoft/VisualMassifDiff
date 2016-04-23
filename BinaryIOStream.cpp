// BinaryIOStream.h

// Implements the BinaryIOStream class representing a QIODevice wrapper for reading and writing binary data in a platform-independent way





#include "Globals.h"
#include "BinaryIOStream.h"





BinaryIOStream::BinaryIOStream(QIODevice & a_Device):
	m_IODevice(a_Device)
{
}




void BinaryIOStream::writeBool(bool a_Value)
{
	unsigned char v =  a_Value ? 1 : 0;
	m_IODevice.write(reinterpret_cast<const char *>(&v), 1);
}





void BinaryIOStream::writeInt32(qint32 a_Value)
{
	writeUInt32(static_cast<quint32>(a_Value));
}





void BinaryIOStream::writeInt64(qint64 a_Value)
{
	writeUInt64(static_cast<quint64>(a_Value));
}





void BinaryIOStream::writeUInt32(quint32 a_Value)
{
	unsigned char v[4] =
	{
		static_cast<unsigned char>((a_Value >> 24) & 0xff),
		static_cast<unsigned char>((a_Value >> 16) & 0xff),
		static_cast<unsigned char>((a_Value >> 8)  & 0xff),
		static_cast<unsigned char>(a_Value         & 0xff),
	};
	if (m_IODevice.write(reinterpret_cast<const char *>(v), 4) != 4)
	{
		throw BinaryIOStreamException("Failed to write UInt32");
	}
}





void BinaryIOStream::writeUInt64(quint64 a_Value)
{
	unsigned char v[8] =
	{
		static_cast<unsigned char>((a_Value >> 56) & 0xff),
		static_cast<unsigned char>((a_Value >> 48) & 0xff),
		static_cast<unsigned char>((a_Value >> 40) & 0xff),
		static_cast<unsigned char>((a_Value >> 32) & 0xff),
		static_cast<unsigned char>((a_Value >> 24) & 0xff),
		static_cast<unsigned char>((a_Value >> 16) & 0xff),
		static_cast<unsigned char>((a_Value >> 8)  & 0xff),
		static_cast<unsigned char>(a_Value         & 0xff),
	};
	if (m_IODevice.write(reinterpret_cast<const char *>(v), 8) != 8)
	{
		throw BinaryIOStreamException("Failed to write UInt64");
	}
}





void BinaryIOStream::writeString(const char * a_Value)
{
	writeString(a_Value, strlen(a_Value));
}





void BinaryIOStream::writeString(const char * a_Value, size_t a_Length)
{
	writeUInt64(a_Length);
	auto bytesWritten = m_IODevice.write(a_Value, a_Length);
	if (bytesWritten != static_cast<decltype(bytesWritten)>(a_Length))
	{
		throw BinaryIOStreamException("Failed to write string payload");
	}
}





void BinaryIOStream::writeString(const std::string & a_Value)
{
	writeString(a_Value.data(), a_Value.size());
}





void BinaryIOStream::writeString(const QString & a_Value)
{
	auto value = a_Value.toUtf8();
	writeUInt64(value.size());
	if (m_IODevice.write(value) != value.size())
	{
		throw BinaryIOStreamException("Failed to write QString payload");
	}
}




