#include <iostream>
#include <fstream>
#include <set>
#include <time.h>

using namespace std;

bool prime(uint64_t n) // тест простоты
{
	if (n < 4)
		return n > 1;
	else if (n % 2 == 0 || n % 3 == 0)
		return false;
	else // все простые числа больше 3 имеют вид 6k+-1, где k натуральное
		for (uint64_t i = 5; i * i <= n; i += 6)
			if (n % i == 0 || n % (i + 2) == 0)
				return false;
	return true;
}

uint64_t gcd(uint64_t a, uint64_t b) // наибольший общий делитель
{
	uint64_t t;
	while (b)
	{
		t = a % b;
		a = b;
		b = t;
	}
	return a;
}

uint64_t modularMult(uint64_t a, uint64_t b, uint64_t m) // модулярное произведение
{
	if (a == 0 || b < m / a)
		return (a * b) % m;
	uint64_t result = 0;
	while (b > 0)
	{
		if (b & 1)
			result = (result + a) % m;
		a = (2 * a) % m; // место, где возможно переполнение (при 2 * m > 2 ^ 64 - 1)
		b >>= 1;
	}
	return result;
}

uint64_t modularExp(uint64_t b, uint64_t e, uint64_t m) // быстрое модулярное возведение в степень
{
	b %= m;
	uint64_t result = 1;
	while (e > 0)
	{
		if (e & 1) result = modularMult(result, b, m);
		b = modularMult(b, b, m);
		e >>= 1;
	}
	return result;
}

bool primitiveRoot(uint64_t p, uint64_t g) // тест первообразности, p - простое
{
	set<uint64_t, less<uint64_t>> factors; // простые множители значения ф-ии Эйлера

	if (g % p == 0) // g и p должны быть взаимно простыми
		return false;

	if (p > 4) // разложение значения ф-ии на простые множители
	{
		uint64_t f = p - 1;
		for (uint64_t j = 2; j <= f; j++)
		{
			if (f % j == 0)
			{
				factors.insert(j);
				f /= j;
				while (f % j == 0)
					f /= j;
			}
		}
	}
	else if (p > 2)
		factors.insert(p - 1);

	// проверка, что g ^ euler(p)/factors(p) mod p != 1
	for (auto i : factors)
	{
		if (modularExp(g, (p - 1) / i, p) == 1)
			return false;
	}
	// проверка, что g ^ euler(p) mod p == 1
	if (modularExp(g, p - 1, p) == 1)
		return true;
	else
		return false;
}

void keyGen() // создание ключей
{
	uint64_t p, g, x, y;

	cout << "enter p, g:" << endl;
	cin >> p >> g;

	if (!prime(p))
	{
		cout << "p is not prime" << endl;
		return;
	}
	if (!primitiveRoot(p, g))
	{
		cout << g << " is not primitive root modulo " << p << endl;
		return;
	}

	// создание закрытого ключа x
	srand(time(NULL));
	do	x = rand() % (p - 3) + 2; // x принадлежит [2, p - 2]
	while (gcd(x, p - 1) != 1);
	cout << "private key x:" << endl << x << endl;

	// создание открытого ключа y
	y = modularExp(g, x, p);
	cout << "public key y:" << endl << y << endl;

	return;
}

void encrypt()
{
	uint64_t p, g, y, m, k, a, b;

	cout << "enter p, g, y:" << endl;
	cin >> p >> g >> y;

	cout << "enter your message m:" << endl;
	cin >> m;
	if (m >= p)
	{
		cout << "m is not less than p" << endl;
		return;
	}

	// создание сессионного ключа k
	srand(time(NULL));
	do	k = rand() % (p - 3) + 2; // k принадлежит [2, p - 2]
	while (gcd(k, p - 1) != 1);

	// генерация шифр-текста a, b
	a = modularExp(g, k, p);
	b = (modularExp(y, k, p) * m) % p;
	cout << "cipher text (a, b):" << endl << a << " " << b << endl;

	return;
}

void decrypt()
{
	uint64_t p, g, x, a, b, m;

	cout << "enter p, g, x:" << endl;
	cin >> p >> g >> x;

	cout << "enter cipher text (a, b):" << endl;
	cin >> a >> b;

	m = (modularExp(a, p - 1 - x, p) * b) % p;
	cout << "decrypted message m:" << endl << m << endl;

	return;
}

int main()
{
	do
	{
		uint32_t sel = 0;
		cout << "1) generate a key" << endl;
		cout << "2) encrypt a message" << endl;
		cout << "3) decrypt a ciphered message" << endl;
		cout << "4) exit" << endl;
		while (sel < 1 || sel > 4)
		{
			cout << "choose your action (1-4): ";
			cin >> sel;
		}
		switch (sel)
		{
		case 1:
			keyGen();
			break;
		case 2:
			encrypt();
			break;
		case 3:
			decrypt();
			break;
		case 4:
			return 0;
			break;
		}
		cout << endl;
	} while (true);
	return 0;
}
