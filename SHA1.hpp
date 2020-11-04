#pragma once

#include <iostream>

// Сделано на основе псевдокода с https://ru.wikipedia.org/wiki/SHA-1, а также проектов с открытым исходным кодом
// Использование: SHA1(<string>).get_hash(); выдаст хэш в виде массива байтов, не СИ-строка. Для вывода в шестнадцатеричном виде: SHA1::hexdigest(SHA1(<string>).get_hash(), 20);
// _rotl - функция циклического сдвига влево, встроенная в компилятор MSVS19; если нет её в вашем - возьмите чей-то код из открытых источников и подставьте вместо _rotl

class SHA1
{
private:
	unsigned char* block;	// постарался не жрать память у стека, поэтому массивы я выделяю и чищу
	unsigned char* hash;
	unsigned int* H;

	size_t offset;
	__int64 size;

	// Correct func, works fine
	unsigned int fromStrToBE(const unsigned char* bytes)
	{
		return (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + (bytes[3]);
	}
	// Correct func, works fine, have implemented right
	void fromBEToStr(unsigned char* buf, __int64 num)
	{
		size_t len = 8;
		for (size_t i = 0; i < len; ++i)
			buf[i] = (unsigned char)(num >> ((len - i - 1) * 8));
	}
public:
	static void hexdigest(const unsigned char* digest, int len = 20)
	{
		for (int i = 0; i < len; ++i)
			std::cout << std::hex << (unsigned int)digest[i] << " ";
		std::cout << std::endl;
	}

	SHA1() { this->reset(); }
	SHA1(const char* input, int len)
	{
		this->reset();

		this->add(input, len);
		this->finilize();
	}
	SHA1(std::string& str)
	{
		this->reset();

		this->add(str.c_str(), str.size());
		this->finilize();
	}
	void reset()
	{
		hash = new unsigned char[20];
		block = new unsigned char[64];
		for (int i = 0; i < 64; ++i) block[i] = 0;

		size = 0;
		offset = 0;

		H = new unsigned int[5];
		H[0] = 0x67452301;
		H[1] = 0xefcdab89;
		H[2] = 0x98badcfe;
		H[3] = 0x10325476;
		H[4] = 0xc3d2e1f0;
	}
	~SHA1()
	{
		delete[] block;
		delete[] hash;
		delete[] H;
	}
	void add(std::string& str)
	{
		this->add(str.c_str(), str.size());
	}
	void add(const char* input, size_t len)
	{
		if (offset + len < 64)
		{
			size += len;
			// strcat_s не подойдёт, т.к строка у нас не Сишная, это просто массив
			for (size_t i = offset; i < offset + len; ++i)
				block[i] = (unsigned char)input[i - offset];
			offset += len;
		}
		else
		{
			size_t actual_len = 64 - offset;
			size += actual_len;
			len = (offset + len) - 64;	// узнаём остаток строки
			for (size_t i = offset; i < 64; ++i)
				block[i] = (unsigned char)input[i - offset];
			process_block();
			offset = 0;
			// так можно, потому что offset зануляется
			this->add((input + actual_len), len);
		}
	}
	void finilize()
	{
		block[offset] = char(128);	// записываем мы побайтово, поэтому сразу будет 8 бит, а нужен 1 бит, значит записть надо число 2^7
		if (offset < 55)
			for (++offset; offset < 55;)	block[++offset] = 0;
		else if (offset > 55)
		{
			for (++offset; offset < 64; ++offset) block[offset] = 0;
			process_block();
			offset = 0; // постулируем начало нового блока
			for (offset; offset < 55;) block[++offset] = 0;
		}
		fromBEToStr(block + offset + 1, size * 8);	// длина сообщения должна быть в битах
		process_block();
	}
	unsigned char* get_hash()
	{
		for (int i = 0; i < 5; ++i)
		{
			hash[i * 4 + 0] = (unsigned char)(H[i] >> 24);
			hash[i * 4 + 1] = (unsigned char)(H[i] >> 16);
			hash[i * 4 + 2] = (unsigned char)(H[i] >> 8);
			hash[i * 4 + 3] = (unsigned char)(H[i] >> 0);
		}
		return hash;
	}
	void process_block()
	{
		unsigned int f = 0;
		unsigned int k = 0;
		unsigned int temp = 0;

		unsigned int* W = new unsigned int[80];
		unsigned int* K = new unsigned int[4];
		K[0] = 0x5A827999;
		K[1] = 0x6ED9EBA1;
		K[2] = 0x8F1BBCDC;
		K[3] = 0xCA62C1D6;

		int i = 0;
		for (i; i < 16; ++i)
			W[i] = fromStrToBE(&block[i * 4]);
		for (i; i < 80; ++i)
			W[i] = _rotl(W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16], 1);

		unsigned int A = H[0];
		unsigned int B = H[1];
		unsigned int C = H[2];
		unsigned int D = H[3];
		unsigned int E = H[4];

		for (int i = 0; i < 80; ++i)
		{
			if (i < 20)
				f = (B & C) | ((~B) & D);
			else if (i < 40)
				f = B ^ C ^ D;
			else if (i < 60)
				f = (B & C) | (B & D) | (C & D);
			else // i < 80
				f = B ^ C ^ D;

			temp = (_rotl(A, 5) + f + E + K[i / 20] + W[i]);
			E = D;
			D = C;
			C = _rotl(B, 30);
			B = A;
			A = temp;
		}

		H[0] += (A);
		H[1] += (B);
		H[2] += (C);
		H[3] += (D);
		H[4] += (E);

		for (int i = 0; i < 64; ++i) block[i] = 0;

		delete[] W;
		delete[] K;
	}
};