#include <vector>
#include <algorithm>
#include <iostream>
#include <iterator>

using namespace std;

vector<int> erase1(vector<int> v, int val)
{
  for (auto i=v.begin(); i != v.end(); )
  {
    if (*i == val)
      i = v.erase(i);
    else
      ++i;
  }

  return v;
}

vector<int> erase2(vector<int> v, int val)
{
  auto f = [val](int i) { return i == val; };
  v.erase(remove_if(v.begin(), v.end(), f));

  return v;
}

void pr(vector<int> v)
{
  auto o = [](int i) { cout << i << " "; };
  for_each(v.begin(), v.end(), o); cout << endl;
}

int main(int argc, char const* argv[])
{
  int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

  vector<int> v(arr, arr+10);

  pr(erase1(v, 5));
  pr(erase2(v, 5));

  return 0;
}
