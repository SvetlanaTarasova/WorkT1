#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib> 
#include <vector>
#include <cmath>

using namespace std;

int T_func(uint32_t A0)
{ //таблица подстановок
	unsigned char Pi[8][16] =
	{
	  {12,4,6,2,10,5,11,9,14,8,13,7,0,3,15,1},
	  {6,8,2,3,9,10,5,12,1,14,4,7,11,13,0,15},
	  {11,3,5,8,2,15,10,13,14,1,7,4,12,9,6,0},
	  {12,8,2,1,13,4,15,6,7,0,10,5,3,14,9,11},
	  {7,15,5,10,8,1,6,13,0,9,3,14,11,4,2,12},
	  {5,13,15,6,9,2,12,10,11,7,8,1,4,3,14,0},
	  {8,14,2,5,6,9,1,12,15,4,11,0,13,10,3,7},
	  {1,7,14,13,0,5,8,3,4,15,10,6,9,12,11,2}
	};

	uint32_t X, B0 = 0;//вспомогательные переменные
	int i, prom;
	X = A0;
	//процесс подстановки
	for (i = 0; i < 8; i++)
	{
		prom = X & 0xf;
		prom = Pi[i][prom];
		B0 = B0 ^ (prom << (4 * i));
		X = X >> 4;
	}
	A0 = B0;
	return A0;
}


uint64_t SHIFR_func(uint64_t A, string  KEY)
{
	uint32_t A0, A1;//правая и левая половины шифруемого сообщения

	//cout << endl << endl << "Входящее сообщение: " << hex << A << endl;//если убрать hex,то все выводится в dec
	A0 = A & 0xffffffff;//Вторая(младшие биты) половина
	A1 = A >> 32;//Первая(старшие биты) половина

//развертывание ключа
	string a[1];
	vector<long long int> K;
	K.resize(8);
	for (int j = 0; j < 8; j++)
	{

		for (int i = 0; i < 8; i++)
		{
			a[0] = KEY[i + j * 8];
			const char* c = a[0].c_str();
			K[j] += strtol(c, NULL, 16) * pow(16, 7 - i);
		}
	}

	uint32_t CK[32];// циклический ключ
	//циклический ключ
	CK[0] = K[0];	CK[1] = K[1];	CK[2] = K[2];	CK[3] = K[3];
	CK[4] = K[4];	CK[5] = K[5];	CK[6] = K[6];	CK[7] = K[7];
	CK[8] = K[0];	CK[9] = K[1];	CK[10] = K[2];	CK[11] = K[3];
	CK[12] = K[4];	CK[13] = K[5];	CK[14] = K[6];	CK[15] = K[7];
	CK[16] = K[0];	CK[17] = K[1];	CK[18] = K[2];	CK[19] = K[3];
	CK[20] = K[4];	CK[21] = K[5];	CK[22] = K[6];	CK[23] = K[7];
	CK[24] = K[7];	CK[25] = K[6];	CK[26] = K[5];	CK[27] = K[4];
	CK[28] = K[3];	CK[29] = K[2];	CK[30] = K[1];	CK[31] = K[0];

	uint32_t B, L, M; //переменные преобразования
	for (int i = 0; i <= 30; i++)
	{
		B = A0;
		A0 = (A0 + CK[i]) & 0xffffffff;//Сложение по модулю 2 в 32
		A0 = T_func(A0);//Результат Т функции
		M = A0 >> 21;
		L = A0 << 11;
		A0 = M ^ L;//Результат 'сдвига на 11'
		A0 = A0 ^ A1;//Результат ксора старшей половины с модеф. младшими
		A1 = B;
	}

	//32-я итерация
	B = A0;
	A0 = (A0 + CK[31]) & 0xffffffff;
	A0 = T_func(A0);
	M = A0 >> 21;
	L = A0 << 11;
	A0 = M ^ L;
	A0 = A0 ^ A1;
	A1 = A0;
	A0 = B;

	A = A1;
	A = (A << 32) + A0;
	//cout << "Зашифрованное сообщение : ";
	return A;
}


void GPSCh_func(int Num_of_blocks, uint64_t A, string KEY)
{
	
	for (int i = 0; i < Num_of_blocks; i++)
	{
		A = SHIFR_func(A, KEY);
		cout << A<<" ";//пробел чтобы была четко видна последовательность чисел,а не смешивалась в одну последовательность из цифр
	}
	cout << endl;
}
int main()
{
	setlocale(LC_ALL, "Russian");

	int N;//длина будущей последовательности
	cin >> N;

	uint64_t A_rand = 0; 
	string At;//входящее сообщение считываемое из файла (A)
	ifstream A_file;
	A_file.open("A.txt");
	A_file >> At;
	A_rand = stoull(At, nullptr, 16);// перевод string в uint64_t для А

	ifstream K_k("K.txt");
	string KEY;
	
	getline(K_k, KEY);
	GPSCh_func(N, A_rand, KEY);

	K_k.close();
	A_file.close();
	return 0;
}