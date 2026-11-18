#pragma once
#ifndef SJTU_BIGINTEGER
#define SJTU_BIGINTEGER

// Integer 1:
// 实现一个有符号的大整数类，只需支持简单的加减

// Integer 2:
// 实现一个有符号的大整数类，支持加减乘除，并重载相关运算符

// 请不要使用除了以下头文件之外的其它头文件
#include <complex>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

// 请不要使用 using namespace std;

namespace sjtu {
class int2048 {
private:
  int flg;
  std::vector <long long> digits;
  static void fft(std::vector <std::complex<double>> &a,int type);
  // todo
public:
  // 构造函数
  void standardize();
  int2048();
  int2048(long long x);
  int2048(const std::string &s);
  int2048(const int2048 &y);

  // 以下给定函数的形式参数类型仅供参考，可自行选择使用常量引用或者不使用引用
  // 如果需要，可以自行增加其他所需的函数
  // ===================================
  // Integer1
  // ===================================

  // 读入一个大整数
  void read(const std::string &s);
  // 输出储存的大整数，无需换行
  void PPT(const long long x,bool type);
  void print();
  bool geq(const std::vector <long long> &a,const std::vector <long long> &b);
  // 加上一个大整数
  int2048 &add(const int2048 &Extra);
  // 返回两个大整数之和
  friend int2048 add(int2048 a, const int2048 &b);
  // 减去一个大整数
  int2048 &minus(const int2048 &extra);
  // 返回两个大整数之差
  friend int2048 minus(int2048 a, const int2048 &b);

  int2048 &naive_mul(const int2048 &Extra);
  // 返回两个大整数之差
  friend int2048 naive_mul(int2048 a, const int2048 &b);

  int2048 &mul(const int2048 &Extra);
  // 返回两个大整数之差
  friend int2048 mul(int2048 a, const int2048 &b);

  int2048 &naive_div(const int2048 &Extra);
  // 返回两个大整数之差
  friend int2048 naive_div(int2048 a, const int2048 &b);

  std::vector <int2048> REALDIV(int2048 a,int2048 b);

  int2048 &div(const int2048 &Extra);
  // 返回两个大整数之差
  friend int2048 div(int2048 a, const int2048 &b);
  // ===================================
  // Integer2
  // ===================================

  int2048 operator+() const;
  int2048 operator-() const;

  int2048 &operator=(const int2048 &)=default;

  int2048 &operator+=(const int2048 &extra);
  friend int2048 operator+(int2048 a, const int2048 &b);

  int2048 &operator-=(const int2048 &extra);
  friend int2048 operator-(int2048 a, const int2048 &b);

  int2048 &operator*=(const int2048 &extra);
  friend int2048 operator*(int2048 a, const int2048 &b);

  int2048 &operator/=(const int2048 &extra);
  friend int2048 operator/(int2048 a, const int2048 &b);

  int2048 &operator%=(const int2048 &extra);
  friend int2048 operator%(int2048 a, const int2048 &b);

  friend std::istream &operator>>(std::istream &in, int2048 &s);
  friend std::ostream &operator<<(std::ostream &out, const int2048 &s);

  friend bool operator==(const int2048 &a, const int2048 &b);
  friend bool operator!=(const int2048 &a, const int2048 &b);
  friend bool operator<(const int2048 &a, const int2048 &b);
  friend bool operator>(const int2048 &a, const int2048 &b);
  friend bool operator<=(const int2048 &a, const int2048 &b);
  friend bool operator>=(const int2048 &a, const int2048 &b);

  friend double change_double(const int2048& A) {
    double res = (A.flg == 1 ? 1.0 : -1.0), ret = 0.0;
    for(int i = (int)(A.digits.size()) - 1; i >= 0; i--) {
      ret = ret * 10000.0 + A.digits[i];
    }
		return ret * res;
  }
  friend long long change_ll(const int2048& A) {
    long long res = (A.flg == 1 ? 1 : -1), ret = 0;
    for(int i = (int)(A.digits.size()) - 1; i >= 0; i--) {
      ret = ret * 10000 + A.digits[i];
    }
		return ret * res;
  }
  friend std::string change_string(const int2048& A) {
    if(A.digits.size() == 0) {
      return "0";
    }
    std::string ret="";
    if(A.flg != 1) {
      ret += "-";
    }
    bool spj = false;
    for(int i = (int)(A.digits.size()) - 1; i >=0 ; i--) {
      int value = A.digits[i];
      if(spj || value / 1000) {
        ret += char('0' + value / 1000);
        spj = true;
      }
      value %= 1000;
      if(spj || value / 100) {
        ret += char('0' + value / 100);
        spj = true;
      }
      value %= 100;
      if(spj || value / 10) {
        ret += char('0' + value / 10);
        spj = true;
      }
      value %= 10;
      ret += char('0' + value);
      spj = true;
    }
		return ret;
  }
};
} // namespace sjtu

#endif
