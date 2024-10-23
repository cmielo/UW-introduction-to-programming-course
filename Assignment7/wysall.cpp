#include <bits/stdc++.h>

#include "wys.h"

namespace {
int _maxQ = 15; // maksymalna "dopuszczalna" ilość zapytań od użytkownika
int _n, _k;
int _g = 1 + _maxQ + _maxQ*(_maxQ-1)/2 + _maxQ*(_maxQ-1)*(_maxQ-2)/6;
int _x;
int ileGier, ileZapytan, maxZapytan;
int ones = 0;
std::vector<bool> lie;

void outputCase()
{
    std::cout << "x = " << _x << ", sekwencja klamstw: ";
    for(auto b : lie) std::cout << b;
    std::cout << '\n';
}

void nextPerm()
{
    bool add1 = 1;
    for(int i = (int)lie.size() - 1; i >= (int)lie.size() - ones; i--)
        if(lie[i] == 0) add1 = 0;
    if(add1)
    {
        for(int i = (int)lie.size() - 1; i >= (int)lie.size() - ones; i--) lie[i] = 0;
        ++ones;
        for(int i = 0; i < ones; i++) lie[i] = 1;
        return;
    }
    int cnt = 0;
    for(int i = (int)lie.size() - 1; i >= 0; i--)
    {
        if(lie[i] == 1 && (i == (int)lie.size() - 1 || lie[i+1] == 1)) cnt++;
        else if(lie[i] == 1 && lie[i+1] == 0) 
        {
            lie[i] = 0;
            ++cnt;
            for(int x = 1; x <= cnt; x++) lie[i+x] = 1;
            break;
        }
        if(i < (int)lie.size() - 1) lie[i+1] = 0;
    }
}

void nextGame()
{
    if(_x < _n) _x++;
    else
    {
        _x = 1;
        nextPerm();
    }
}
}

void dajParametry(int &n, int &k, int &g) {
  std::cin >> _n >> _k;
  n = _n; k = _k;
  if(k < 3) _g -= _maxQ*(_maxQ-1)*(_maxQ-2)/6;
  if(k < 2) _g -= _maxQ*(_maxQ-1)/2;
  if(k < 1) _g -= _maxQ;
  _g *= n;
  g = _g;
  _x = 1;
  lie.resize(_maxQ);
  for(int i = 0; i < 15; i++) lie[i] = 0;
  ileGier = ileZapytan = maxZapytan = 0;
  std::cout << "n = " << _n << ", k = " << _k << ", g = " << _g << '\n';
}

bool mniejszaNiz(int y) {
  if (ileZapytan >= (int)lie.size() || lie[ileZapytan++] == 0)
    return _x < y;
  else
    return !(_x < y);
}

void odpowiedz(int x) {
  if (x != _x) {
    std::cout << "ZLE. Bledna odpowiedz w grze #" << ileGier << ": oczekiwano " << _x << " a uzyskano odpowiedz " << x << '\n';
    outputCase();
    exit(1);
  }
  maxZapytan = std::max(maxZapytan, ileZapytan);
  ++ileGier;
  if (ileGier == _g) {
    std::cout << "OK. Zadano maksymalnie " << maxZapytan << " zapytan.\n";
    //outputCase();
    //exit(0);
  }
  //outputCase();
  nextGame();
  ileZapytan = 0;
}
