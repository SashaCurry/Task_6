#include "iostream"
#include "vector"
#include "string"
#include "boost/multiprecision/cpp_int.hpp"

using namespace std;
using namespace boost::multiprecision;

class Pattern {
private:
	static vector <cpp_int> deg2(cpp_int el, cpp_int n) {//Раскладываем число на степени двойки
		vector <cpp_int> res;
		while (n != 0) {
			if (n / el == 1) {
				res.push_back(el);
				n -= el;
				el = 1;
			}
			else
				el *= 2;
		}
		return res;
	}


	static cpp_int multMod(cpp_int n, cpp_int mod, vector <pair <cpp_int, cpp_int>> lst) {//Умножаем число по модулю
		if (lst.size() == 1) {
			cpp_int res = 1;
			for (int i = 0; i < lst[0].second; i++)
				res = res * lst[0].first % mod;
			return res;
		}
		else if (lst[0].second == 1) {
			cpp_int el = lst[0].first;
			lst.erase(lst.begin());
			return (el * multMod(n, mod, lst)) % mod;
		}
		else {
			for (int i = 0; i < lst.size(); i++)
				if (lst[i].second > 1) {
					lst[i].first = (lst[i].first * lst[i].first) % mod;
					lst[i].second /= 2;
				}
			return multMod(n, mod, lst);
		}
	}


	static int partition(vector <cpp_int>& a, int start, int end) { //Для quicksort
		cpp_int pivot = a[end];
		int pIndex = start;

		for (int i = start; i < end; i++) {
			if (a[i] <= pivot) {
				swap(a[i], a[pIndex]);
				pIndex++;
			}
		}

		swap(a[pIndex], a[end]);
		return pIndex;
	}


	static bool checkMutualSimplicity(vector <pair <cpp_int, cpp_int>> raws) { //Для gcTheorem
		for (unsigned short i = 0; i < raws.size(); i++)
			for (unsigned short j = i + 1; j < raws.size(); j++) {
				if (raws[i].second > raws[j].second) {
					if (usualEuclid(raws[i].second, raws[j].second) != 1)
						return false;
				}
				else
					if (usualEuclid(raws[j].second, raws[i].second) != 1)
						return false;
			}
		return true;
	}
public:
	static cpp_int powClosed(cpp_int x, cpp_int y, cpp_int mod) {//Возводим число в степени по модулю
		if (y == 0)
			return 1;

		vector <cpp_int> lst = deg2(1, y);
		vector <pair <cpp_int, cpp_int>> xDegs;
		for (int i = 0; i < lst.size(); i++)
			xDegs.push_back(make_pair(x, lst[i]));

		cpp_int res = multMod(x, mod, xDegs);
		return res;
	}


	static string binForm(cpp_int x) {
		string bitter = "";
		while (x != 0) {
			bitter = (x % 2 == 0 ? "0" : "1") + bitter;
			x = x / 2;
		}
		if (bitter == "")
			return "0";
		return bitter;
	}


	static cpp_int decForm(string x) {
		cpp_int res = 0, deg = 1;
		if (x.back() == '1')
			res += 1;
		for (int i = 1; i < x.length(); i++) {
			deg = deg * 2;
			if (x[x.length() - i - 1] == '1')
				res += deg;
		}
		return res;
	}


	static cpp_int usualEuclid(cpp_int a, cpp_int b) {
		if (a < b)
			swap(a, b);
		if (a < 0 || b < 0)
			throw string{ "Выполнение невозможно: a < 0 или b < 0" };
		else if (b == 0)
			return a;

		cpp_int r = a % b;
		return usualEuclid(b, r);
	}


	static cpp_int funEuler(cpp_int n) {
		cpp_int res = 1;
		for (int i = 2; i < n; i++)
			if (usualEuclid(n, i) == 1)
				res++;
		return res;
	}


	static bool miller_rabin(cpp_int n, int k = 10) {
		if (n == 0 || n == 1)
			return false;

		cpp_int d = n - 1;
		cpp_int s = 0;
		while (d % 2 == 0) {
			s++;
			d = d / 2;
		}

		cpp_int nDec = n - 1;
		for (int i = 0; i < k; i++) {
			cpp_int a = rand() % nDec;
			if (a == 0 || a == 1)
				a = a + 2;

			cpp_int x = powClosed(a, d, n);
			if (x == 1 || x == nDec)
				continue;

			bool flag = false;
			for (int j = 0; j < s; j++) {
				x = (x * x) % n;
				if (x == nDec) {
					flag = true;
					break;
				}
			}
			if (!flag)
				return false;
		}

		return true;
	}


	static cpp_int generateSimpleNum(unsigned short m) {
		cpp_int q = rand() % 1000;
		while (funEuler(q) != q - 1)
			q++;

		cpp_int s, n = 2, nDec;
		while (!miller_rabin(n)) {
			string sBin = "1";
			int sBinSize = rand() % (m / 2) + m / 2;
			for (int i = 0; i < sBinSize; i++)
				sBin = sBin + to_string(rand() % 2);
			s = decForm(sBin);

			n = (q * s) + 1;
			nDec = n - 1;
		}

		return n;
	}


	static void quicksort(vector <cpp_int>& a, int start, int end) {
		if (start >= end) {
			return;
		}

		int pivot = partition(a, start, end);
		quicksort(a, start, pivot - 1);
		quicksort(a, pivot + 1, end);
	}


	static pair <cpp_int, cpp_int> gcTheorem(vector <pair <cpp_int, cpp_int>> raws) {
		if (!checkMutualSimplicity)
			throw string{ "Модули m не являются попарно взаимно простыми!" };

		cpp_int M = 1;
		for (unsigned short i = 0; i < raws.size(); i++)
			M *= raws[i].second;

		cpp_int res = 0;
		for (unsigned short i = 0; i < raws.size(); i++) {
			cpp_int Mi = 1;
			for (unsigned short j = 0; j < raws.size(); j++) {
				if (i == j)
					continue;
				Mi *= raws[j].second;
			}
			for (cpp_int j = 1;; j++)
				if ((Mi * j) % raws[i].second == raws[i].first) {
					res += Mi * j;
					break;
				}
		}

		return make_pair(res % M, M);
	}
};



class Mignotte {
private:
	bool checkMutualSimplicity(vector <pair <cpp_int, cpp_int>> raws) {
		for (unsigned short i = 0; i < raws.size(); i++)
			for (unsigned short j = i + 1; j < raws.size(); j++) {
				if (raws[i].second > raws[j].second) {
					if (Pattern::usualEuclid(raws[i].second, raws[j].second) != 1)
						return false;
				}
				else
					if (Pattern::usualEuclid(raws[j].second, raws[i].second) != 1)
						return false;
			}
		return true;
	}
public:
	int n, k;
	vector <cpp_int> simpleNums, Is;
	cpp_int alpha = 1, beta = 1, S;


	Mignotte(int n, int k) {
		this->n = n;
		this->k = k;
	}
	~Mignotte() {
	}


	void selectPrimeNums(unsigned short m) {
		for (unsigned short i = 0; i < n; i++) {
			cpp_int p = Pattern::generateSimpleNum(m);
			this->simpleNums.push_back(p);
		}
		Pattern::quicksort(simpleNums, 0, n - 1);

		cout << "\nПоследовательность Миньотта: ";
		for (unsigned short i = 0; i < simpleNums.size(); i++)
			cout << "\np" << i + 1 << " = " << simpleNums[i];
	}


	void selectS(unsigned short m) {
		for (unsigned short i = 0; i < k; i++)
			alpha *= simpleNums[i];
		for (unsigned short i = n - k + 1; i < n; i++)
			beta *= simpleNums[i];
		cout << "\n\nalpha = " << alpha << ", beta = " << beta;

		S = (Pattern::generateSimpleNum(2 * m) + beta) % alpha;
		cout << "\nS (beta < S < alpha) = " << S;
	}


	void selectIs() {
		for (unsigned short i = 0; i < n; i++)
			this->Is.push_back(S % simpleNums[i]);

		cout << "\n\nДоли пользователей:";
		for (unsigned short i = 0; i < n; i++)
			cout << "\nI" << i + 1 << ": " << Is[i];
	}
};

int main() {
	setlocale(LC_ALL, "ru");
	srand(time(NULL));

	cout << "\tСхема Миньотта (китайскай теорема об остатках)";
	int n, k;
	cout << "\nКоличество людей, между которыми необходимо разделить секрет: ";
	cin >> n;
	cout << "Количество людей, которые могут восстановить секрет: ";
	cin >> k;
	if (n < 2 || (k < 2 || k > n)) {
		cout << "\nПараметры k или n указаны неверно!";
		return 0;
	}

	cout << "Параметр безопасности > 5: ";
	unsigned short m;
	cin >> m;

	try {
		Mignotte mignotte(n, k);
		mignotte.selectPrimeNums(m);
		mignotte.selectS(m);
		mignotte.selectIs();

		cout << "\n\nКоличество пользователей, которые хотят восстановить секрет: ";
		cin >> m;
		if (m < 1 || m > n)
			throw string{ "Неверное количество пользователей!" };

		cout << "\n\nПо греко-китайской теореме об остатках: ";
		vector <pair <cpp_int, cpp_int>> raws;
		for (unsigned short i = 0; i < mignotte.Is.size(); i++)
			raws.push_back(make_pair(mignotte.Is[i], mignotte.simpleNums[i]));
		while (raws.size() != m)
			raws.erase(raws.begin() + rand() % raws.size());
		for (unsigned short i = 0; i < raws.size(); i++)
			cout << "\nx = " << raws[i].first << " (mod " << raws[i].second << ")";

		pair <cpp_int, cpp_int> res = Pattern::gcTheorem(raws);
		cout << "\nРешение данной системы линейных сравнений: x = " << res.first << " (mod " << res.second << ")";
		cout << "\nИзначальное значение S: " << mignotte.S << endl;
	}
	catch (string& message) {
		cout << message;
	}

	cout << endl;
	return 0;
}