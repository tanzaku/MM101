// C++11
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

/**
 * assertだとスタックトレースが表示されないので、その代わりに
 * gdb,vs,vscodeなどのデバッガを使う方がいい
 */
void SHOW_STACK_TRACE()
{
  volatile int *t = 0;
  *t = 1;
}

vector<string> split(string s, char delimiter)
{
  istringstream stream(s);

  string field;
  vector<string> result;
  while (getline(stream, field, delimiter)) {
    result.push_back(field);
  }
  return result;
}

vector<int> split_int(string s, char delimiter)
{
  istringstream stream(s);

  string field;
  vector<int> result;
  while (getline(stream, field, delimiter)) {
    result.push_back(atoi(field.data()));
  }
  return result;
}

template <typename T>
string join(T &s, char delimiter)
{
  string result;
  for (int i = 0; i < (int)s.size(); i++) {
    if (i != 0) result += delimiter;
    result += to_string(s[i]);
  }
  return result;
}

#include <cmath>
#include <cstdint>
#include <tuple>
class XorShiftL
{
  static const int LOG_CACHE_SIZE = 0x10000;

  int logIndex = 0;
  double logCache[LOG_CACHE_SIZE];

  std::uint64_t x = 88172645463325252LL;

public:
  XorShiftL()
  {
    for (int i = 0; i < LOG_CACHE_SIZE; i++) {
      logCache[i] = log((i + 0.5) / 0x10000);
    }
    shuffle(logCache);
  }

  std::uint64_t xor64()
  {
    x = x ^ (x << 7);
    return x = x ^ (x >> 9);
  }

  // [0, n)
  int nextInt(int n) { return xor64() % n; }

  // [low, high)
  int nextInt(int low, int high) { return xor64() % (high - low) + low; }
  double nextDouble() { return xor64() / double(std::numeric_limits<std::uint64_t>::max()); }

  /**
   * 2つの相異なる数をランダムに取得する
   * get<0>(result) < get<1>(result) が保証されている
   */
  std::tuple<int, int> nextIntPair(int low, int high)
  {
    int i = nextInt(low, high);
    int j = nextInt(low, high - 1);
    if (j >= i)
      j++;
    else
      std::swap(i, j);
    return std::make_tuple(i, j);
  }

  /**
   * 2つの相異なる数をランダムに取得する
   * get<0>(result) < get<1>(result) は保証されない
   */
  std::tuple<int, int> nextIntUnorderedPair(int low, int high)
  {
    int i = nextInt(low, high);
    int j = nextInt(low, high - 1);
    if (j >= i)
      j++;
    return std::make_tuple(i, j);
  }

  /**
   * ログを取得する
   */
  double nextLog()
  {
    const double res = logCache[logIndex++];
    if (logIndex == LOG_CACHE_SIZE) {
      logIndex = 0;
    }
    return res;
  }

  template <typename T>
  void shuffle(T &s)
  {
    for (int i = 1; i < (int)s.size(); i++) {
      int j = nextInt(0, i + 1);
      if (i != j) {
        std::swap(s[i], s[j]);
      }
    }
  }

  template <typename T, int n>
  void shuffle(T (&ary)[n])
  {
    for (int i = 1; i < n; i++) {
      int j = nextInt(0, i + 1);
      if (i != j) {
        std::swap(ary[i], ary[j]);
      }
    }
  }
};

/**
 * [0,n)の集合を扱うデータ構造
 * 操作をO(1)で実行できる
 */
class SeqSet
{
  struct OpHistory {
    int i, j;
    int d;

    OpHistory() {}
    OpHistory(int i, int j, int d) : i(i), j(j), d(d) {}
  };

  struct Iterator {
    vector<int>::iterator i;

    Iterator() {}
    Iterator(vector<int>::iterator i) : i(i) {}

    bool operator!=(Iterator it) const
    {
      return i != it.i;
    }

    void operator++()
    {
      i++;
    }

    int operator*()
    {
      return *i;
    }
  };

  int capacity;
  int len;
  vector<int> value;
  vector<int> index;
  vector<OpHistory> history;

  void swapValue(int i, int j)
  {
    auto &x = value[i];
    auto &y = value[j];
    std::swap(x, y);
    std::swap(index[x], index[y]);
  }

public:
  void init(int c)
  {
    capacity = c;
    len = 0;
    value.clear();
    value.reserve(capacity);
    index.clear();
    index.reserve(capacity);
    for (int i = 0; i < capacity; i++) {
      value.push_back(i);
      index.push_back(i);
    }
  }

  Iterator begin()
  {
    return Iterator(value.begin());
  }

  Iterator end()
  {
    return Iterator(value.begin() + len);
  }

  void commit()
  {
    history.clear();
  }

  void rollback()
  {
    while (!history.empty()) {
      auto h = history.back();
      history.pop_back();
      swapValue(h.i, h.j);
      len -= h.d;
    }
  }

  bool contains(int v) const
  {
    return index[v] < len;
  }

  int size() const
  {
    return len;
  }

  int unusedSize() const
  {
    return capacity - len;
  }

  void removeByValue(int v)
  {
    removeByIndex(index[v]);
  }

  void removeByIndex(int i)
  {
    if (i < 0 || i >= len) {
      cerr << "Error on SeqSet#remove: " << i << " " << len << endl;
      throw;
    }

    len--;
    swapValue(i, len);
    history.emplace_back(i, len, -1);
  }

  void insertByValue(int v)
  {
    insertByIndex(index[v]);
  }

  /**
   * i : [0, unusedSize)
   */
  void insertByIndex(int i)
  {
    if (i < 0 || i >= capacity) {
      cerr << "Error on SeqSet#insert: " << i << " " << capacity << endl;
      throw;
    }

    swapValue(i, len);
    history.emplace_back(i, len, 1);
    len++;
  }

  bool empty() const { return size() == 0; }
  bool full() const { return size() == capacity; }
  void pop_back() { removeByIndex(len - 1); }
  void push_back(int v) { insertByValue(v); }

  int operator[](int i) const { return value[i]; }

  void verify()
  {
    for (int v = 0; v < capacity; v++) {
      if (value[index[v]] != v) {
        cerr << "verify failed : " << v << " " << index[v] << " " << value[index[v]] << endl;
        throw;
      }
    }
  }
};

XorShiftL rng;
vector<int> unused;
// vector<int> orders[10];
SeqSet orders[10];
vector<char> position(10);

// const
vector<int> atk(30);
vector<int> def(30);
vector<int> agg(30);
vector<int> groupAtk;
vector<int> groupDef;
vector<int> groupAgg;
vector<vector<int>> groupMembers;

// random
double injuryRate;

// temp
vector<int> lineup(10);
vector<int> calcAtk(10);
vector<int> calcDef(10);
vector<int> calcAgg(10);
vector<bool> carded(30);
vector<bool> done(30);

void initSimulate()
{
  fill(lineup.begin(), lineup.end(), -1);
  fill(calcAgg.begin(), calcAgg.end(), 0);
  fill(calcAgg.begin(), calcAgg.end(), 0);
  fill(calcAgg.begin(), calcAgg.end(), 0);
  fill(carded.begin(), carded.end(), false);
  fill(done.begin(), done.end(), false);
  injuryRate = rng.nextDouble() * 0.05;
}

int getPosition(int p)
{
  auto it = find(lineup.begin(), lineup.end(), p);
  if (it == lineup.end()) return -1;
  return distance(lineup.begin(), it);
}

bool groupPresent(int g)
{
  for (int p : groupMembers[g]) {
    auto &os = lineup;
    if (find(os.begin(), os.end(), p) == os.end()) {
      return false;
    }
  }
  return true;
}

int createCurrentLineup()
{
  for (int i = 0; i < 10; i++) {
    if (lineup[i] >= 0) continue;
    for (auto o : orders[i]) {
      if (!done[o]) {
        lineup[i] = o;
        done[o] = true;
        break;
      }
    }
  }

  for (int i = 0; i < 10; i++) {
    calcAtk[i] = calcDef[i] = calcAgg[i] = 0;
    if (lineup[i] < 0) continue;
    // cerr << "position " << i << " " << position[i] << endl;
    calcAgg[i] = agg[lineup[i]];
    if (position[i] == 'F') {
      calcAtk[i] = atk[lineup[i]] * 2;
    }
    if (position[i] == 'D') {
      calcDef[i] = def[lineup[i]] * 2;
    }
    if (position[i] == 'M') {
      calcAtk[i] = atk[lineup[i]];
      calcDef[i] = def[lineup[i]];
    }
  }

  // Add group bonuses
  for (int i = 0; i < (int)groupMembers.size(); i++) {
    if (!groupPresent(i))
      continue;
    for (int gm : groupMembers[i]) {
      int p = getPosition(gm);
      // cerr << p << " " << i << " " << calcAgg.size() << " " << groupAgg.size() << " " << position[p] << endl;
      calcAgg[p] += groupAgg[i];
      if (position[p] == 'F') {
        calcAtk[p] += groupAtk[i] * 2;
      }
      if (position[p] == 'D') {
        calcDef[p] += groupDef[i] * 2;
      }
      if (position[p] == 'M') {
        calcAtk[p] += groupAtk[i];
        calcDef[p] += groupDef[i];
      }
    }
  }

  int totalAtk = 0;
  int totalDef = 0;
  for (int i = 0; i < 10; i++) {
    totalAtk += calcAtk[i];
    totalDef += calcDef[i];
    // cerr << i << " " << calcAtk[i] << " " << calcDef[i] << " " << lineup[i] << endl;
  }
  return std::min(totalAtk, totalDef);
}

void processLineup()
{
  for (int i = 0; i < 10; i++) {
    if (lineup[i] == -1) continue;
    if (rng.nextDouble() < injuryRate) {
      lineup[i] = -1;
      continue;
    }
    if (rng.nextInt(100) < calcAgg[i]) {
      if (carded[lineup[i]]) {
        lineup[i] = -1;
      } else {
        carded[lineup[i]] = true;
      }
    }
  }
}

int simulate()
{
  initSimulate();

  int score = 0;
  for (int i = 0; i < 5; i++) {
    score += createCurrentLineup();
    processLineup();
  }

  return score;
}

int evalCost()
{
  int cost = 0;
  for (int i = 0; i < 50 * 2; i++) {
    cost += -simulate();
  }
  return cost;
}

class WorldCupLineup
{
public:
  vector<string> selectPositions(vector<string> players, vector<string> groups)
  {
    for (int i = 0; i < 30; i++) {
      vector<int> splited = split_int(players[i], ',');
      atk[i] = splited[0];
      def[i] = splited[1];
      agg[i] = splited[2];
      // cerr << i << " " << atk[i] << " " << def[i] << " " << agg[i] << endl;
    }

    for (int i = 0; i < (int)groups.size(); i++) {
      vector<string> splited = split(groups[i], ' ');
      auto members = split_int(splited[0], ',');
      auto groupsStatus = split_int(splited[1], ',');
      groupMembers.push_back(members);
      groupAtk.push_back(groupsStatus[0]);
      groupDef.push_back(groupsStatus[1]);
      groupAgg.push_back(groupsStatus[2]);
    }

    for (auto &o : orders) {
      o.init(30);
    }

    for (int i = 0; i < 30; i++) {
      auto &o = orders[rng.nextInt(10)];
      o.push_back(i);
      o.commit();
    }

    for (int i = 0; i < 10; i++) {
      position[i] = "FMD"[rng.nextInt(3)];
    }

    int best = evalCost();
    for (int i = 0; i < 10000; i++) {
      if (rng.xor64() & 1) {
        int idx = rng.nextInt(10);
        auto &o = orders[idx];
        if (o.empty()) continue;
        int j = rng.nextInt(o.size());
        o.removeByIndex(j);
        int curCost = evalCost();
        if (best >= curCost) {
          cerr << "update : " << best << " " << curCost << endl;
          best = curCost;
          o.commit();
          continue;
        }
        o.rollback();
      } else {
        int idx = rng.nextInt(10);
        auto &o = orders[idx];
        if (o.full()) continue;
        int j = rng.nextInt(o.unusedSize());
        o.insertByIndex(j);
        int curCost = evalCost();
        if (best >= curCost) {
          cerr << "update : " << best << " " << curCost << endl;
          best = curCost;
          o.commit();
          continue;
        }
        o.rollback();
      }
    }

    // for (int i = 0; i < 100; i++) {
    //   int score = simulate();
    //   cerr << "score=" << score << endl;
    // }

    // return {"F 0", "F 1", "F 2", "M 3", "M 4", "M 5", "M 6", "D 7", "D 8", "D 9"};
    vector<string> result;
    for (int i = 0; i < 10; i++) {
      string s = string() + position[i] + " ";
      s += join(orders[i], ',');
      result.push_back(s);
      cerr << s << endl;
    }
    return result;
  }
};
// -------8<------- end of solution submitted to the website -------8<-------

template <class T>
void getVector(vector<T> &v)
{
  for (int i = 0; i < (int)v.size(); ++i)
    getline(cin, v[i]);
}

int main()
{
  WorldCupLineup sol;
  int H;
  cin >> H;
  cin.ignore(1);
  vector<string> players(H);
  getVector(players);
  cin >> H;
  cin.ignore(1);
  vector<string> groups(H);
  getVector(groups);

  vector<string> ret = sol.selectPositions(players, groups);
  cout << ret.size() << endl;
  for (int i = 0; i < (int)ret.size(); ++i)
    cout << ret[i] << endl;
  cout.flush();
}
