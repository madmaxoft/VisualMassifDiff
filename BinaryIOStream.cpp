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





bool BinaryIOStream::readBool()
{
	char v;
	auto numBytesRead = m_IODevice.read(&v, 1);
	if (numBytesRead != 1)
	{
		throw BinaryIOStreamException("Failed to read bool");
	}
	return (v != 0);
}





qint32 BinaryIOStream::readInt32()
{
	return static_cast<qint32>(readUInt32());
}




qint64 BinaryIOStream::readInt64()
{
	return static_cast<qint64>(readUInt64());
}





quint32 BinaryIOStream::readUInt32()
{
	unsigned char v[4];
	auto numBytesRead = m_IODevice.read(reinterpret_cast<char *>(v), 4);
	if (numBytesRead != 4)
	{
		throw BinaryIOStreamException("Failed to read UInt32");
	}
	return ((v[0] << 24) | (v[1] << 16) | (v[2] << 8) | v[3]);
}





quint64 BinaryIOStream::readUInt64()
{
	quint64 upper = readUInt32();
	quint64 lower = readUInt32();
	return (upper << 32) | lower;
}





std::string BinaryIOStream::readString()
{
	auto len = readUInt64();
	if (len > std::numeric_limits<size_t>::max() - 1)
	{
		throw BinaryIOStreamException("String sanity check failed");
	}
	std::string res;
	res.reserve(len);
	static const size_t BUF_SIZE = 4096;
	while (len > 0)
	{
		char buf[BUF_SIZE];
		size_t toRead = std::min<size_t>(len, sizeof(buf));
		if (m_IODevice.read(buf, toRead) != static_cast<qint64>(toRead))
		{
			throw BinaryIOStreamException("Failed to read part of string");
		}
		res.append(buf, toRead);
		len -= toRead;
	}
	return res;
}





QString BinaryIOStream::readQString()
{
	auto utf8 = readString();
	return QString::fromUtf8(utf8.c_str(), static_cast<int>(utf8.size()));
}




