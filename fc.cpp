#include <iostream>
#include <stack>
#include <queue>
#include <algorithm>
#include <vector>
#include <sys/time.h>

#define GET_TIME(now) { \
	struct timeval t; \
	gettimeofday(&t, NULL); \
	now = t.tv_sec + t.tv_usec/1000000.0; \
}
#define N 9
#define DEBUG
using namespace std;

struct node
{
	bool is_const;	//判断是否为常数
	int num;	//赋值
	bool is_assign;	//是否被赋值
	vector<pair<int, int>> less;	//比你小的数字
	vector<pair<int, int>> more;	//比你大的数字
	int max;	//上界
	int min;	//下界
	bool domain[N];		//能够取值的值域
	int count;	//记录当前从哪里开始
};

class chart
{
public:
	bool line[N][N];	//二维数组实质上第二维指的是数字，比如说数字一可以取那么第二维的0就为true。
	bool row[N][N];
	node data[N][N];
	int count_const;	//数一下有多少个固定的值

	chart();	//这个能够读取数据;
	void changeC(int i, int j, int label);	//根据某一个数改变参数（调整min,max的值）
	//void unchangeC(int i, int j);	//还原一个值，暂时没用，先注释掉。
	bool assign1(int i, int j);
	bool assign2(int i, int j);
	bool findsolution();
	void show();
	bool test(int i, int j, int num);
};
chart::chart()
{
	count_const = 0;
	for (int i = 0; i < N; i++)	//首先所有的行和所有的列都可以被赋值，也就是所有的节点都可以取一样的值;
		for(int j = 0; j<N; j++)
			line[i][j] = row[i][j] = true;
	for(int i = 0; i<N; i++)	//这个嵌套循环读入所有数据;
		for (int j = 0; j < N; j++)
		{
			cin >> data[i][j].num;
			if (data[i][j].num)
			{
				data[i][j].is_const = data[i][j].is_assign = true;
				row[i][data[i][j].num-1] = line[j][data[i][j].num-1] = false;	//读入的数字大于1，所以要减一操作;
				data[i][j].max = data[i][j].min = data[i][j].num;
				count_const += 1;
			}
			else
			{
				data[i][j].max = N;
				data[i][j].min = 1;
				data[i][j].is_const = data[i][j].is_assign = false;
			}
		}
	pair<int, int> p1;
	pair<int, int> p2;
	while (cin >> p1.first)	//建立节点间的大小关系;
	{
		cin >> p1.second >> p2.first >> p2.second;
		data[p1.first][p1.second].more.push_back(p2);
		data[p2.first][p2.second].less.push_back(p1);
	}
	for(int i = 0; i<N; i++)
		for (int j = 0; j < N; j++)
		{
			if (data[i][j].is_const == true)
			{
				for (int i1 = 0; i1 < data[i][j].less.size(); i1++)
				{
					data[data[i][j].less[i1].first][data[i][j].less[i1].second].max = data[i][j].num - 1;
				}
				for (int i1 = 0; i1 < data[i][j].more.size(); i1++)
				{
					data[data[i][j].more[i1].first][data[i][j].more[i1].second].min = data[i][j].num + 1;
				}
			}
		}
}

void chart::changeC(int i1, int j1, int label)	//改变上下左右的值,label为0执行more，label为1执行less，实际上不是1就执行more了。
{
	if (data[i1][j1].is_assign)
		return;
	int max = N+1;
	int min = 0;
	if (label == 1)
	{
		for (int i = 0; i < data[i1][j1].less.size(); i++)	//找min值
		{
			int x = data[i1][j1].less[i].first;
			int y = data[i1][j1].less[i].second;
			if (data[x][y].is_assign)
			{
				if (data[x][y].num >= min)	//num最小值是1，min最小值是0。num最大值是N，max最大值是N.
					min = data[x][y].num + 1;
			}
			else if (data[x][y].min >= min)
				min = data[x][y].min + 1;
		}
		data[i1][j1].min = min;
	}
	else
	{
		for (int i = 0; i < data[i1][j1].more.size(); i++)	//找max值
		{
			int x = data[i1][j1].more[i].first;
			int y = data[i1][j1].more[i].second;
			if (data[x][y].is_assign)
			{
				if (data[x][y].num <= max)	//num最小值是1，min最小值是0。num最大值是N，max最大值是N.
					max = data[x][y].num - 1;
			}
			else if (data[x][y].max <= max)
				max = data[x][y].max - 1;
		}
		data[i1][j1].max = max;
	}
}

bool chart::assign1(int i1, int j1)	//第一次赋值
{
	int count = 0;	//第一次赋值的时候找一下自己能够取值的值域，count记录值的个数。
	for (int i = data[i1][j1].min - 1; i < data[i1][j1].max; i++)
	{
		data[i1][j1].domain[i] = row[i1][i] && line[j1][i];
		if (data[i1][j1].domain[i])
			count += 1;
	}
	if (!count)	//如果找不到能够赋值的数字，那么就返回false。
		return false;
	for (int i = data[i1][j1].min - 1; i < data[i1][j1].max; i++)
	{
		if (!data[i1][j1].domain[i])//如果为假那么就继续
			continue;
		/*bool test1 = test(i1, j1, i + 1);
		if (!test1)
			continue;*/
		//cout << i + 1 << endl;
		data[i1][j1].num = i+1;
		data[i1][j1].count = i;
		row[i1][i] = false;
		line[j1][i] = false;
		data[i1][j1].is_assign = true;
		for (int tempi = 0; tempi < data[i1][j1].less.size(); tempi++)
			changeC(data[i1][j1].less[tempi].first, data[i1][j1].less[tempi].second, 0);
		for (int tempi = 0; tempi < data[i1][j1].more.size(); tempi++)
			changeC(data[i1][j1].more[tempi].first, data[i1][j1].more[tempi].second, 1);
		return true;
	}
	return false;
}

bool chart::assign2(int i1, int j1)	//第二次赋值
{
	data[i1][j1].is_assign = false;
	line[j1][data[i1][j1].num - 1] = true;	//还原取值范围
	row[i1][data[i1][j1].num - 1] = true;
	data[i1][j1].count += 1;
	for (int tempi = 0; tempi < data[i1][j1].less.size(); tempi++)
		changeC(data[i1][j1].less[tempi].first, data[i1][j1].less[tempi].second, 0);
	for (int tempi = 0; tempi < data[i1][j1].more.size(); tempi++)
		changeC(data[i1][j1].more[tempi].first, data[i1][j1].more[tempi].second, 1);
	for (int i = data[i1][j1].count; i < data[i1][j1].max; i++)
	{
		if (!data[i1][j1].domain[i])//如果为假那么就继续
			continue;
		/*bool test1 = test(i1, j1, i + 1);
		if (!test1)
			continue;*/
		data[i1][j1].num = i + 1;
		data[i1][j1].count = i;
		row[i1][i] = false;
		line[j1][i] = false;
		data[i1][j1].is_assign = true;
		for (int tempi = 0; tempi < data[i1][j1].less.size(); tempi++)
			changeC(data[i1][j1].less[tempi].first, data[i1][j1].less[tempi].second, 0);
		for (int tempi = 0; tempi < data[i1][j1].more.size(); tempi++)
			changeC(data[i1][j1].more[tempi].first, data[i1][j1].more[tempi].second, 1);
		return true;
	}
	return false;
}

bool chart::findsolution()
{
	double begin, end;
	GET_TIME(begin);
	stack<pair<int, int>> my_stack;
	pair<int, int>temp;
	int i, j;
	i = j = 0;
	int size = 0;
	int count1 = 0;
	while (my_stack.size() != N * N - count_const)
	{
		//cout << my_stack.size() << endl;
		//if (my_stack.size() == 31)
		//{
			//cout << my_stack.top().first << my_stack.top().second << endl;
		//}
		temp.first = i;
		temp.second = j;
		//cout << my_stack.size() << endl;
		/*if (my_stack.size() == 9)
		{
			show();
			cout << count1 << endl;
			count1 += 1;
		}*/
		if (!data[temp.first][temp.second].is_const)
		{
			if (assign1(i, j))//如果第一次能够赋值
			{
				my_stack.push(temp);
				j += 1;
				if (j >= N)
				{
					j = 0;
					i += 1;
				}
			}
			else
			{
				bool temp_bool = false;
				while (!temp_bool)
				{
					if (!my_stack.size())	//找不到了
					{
						cout << size << endl;
						return false;
					}
					temp = my_stack.top();
					my_stack.pop();
					i = temp.first;
					j = temp.second;
					temp_bool = assign2(i,j);
				}
				my_stack.push(temp);
				j += 1;
				if (j >= N)
				{
					i += 1;
					j = 0;
				}
			}
		}
		else
		{
			j += 1;
			if (j >= N)
			{
				j = 0;
				i += 1;
			}
		}
	}
	GET_TIME(end);
	cout << "计算运行时间:" << end - begin << "s" << endl;
	return true;
}

void chart::show()
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			cout << data[i][j].num << " ";
		cout << endl;
	}
}

bool chart::test(int i1, int j1, int num)
{
	for (int i = 0; i < data[i1][j1].less.size(); i++)
	{
		int x = data[i1][j1].less[i].first;
		int y = data[i1][j1].less[i].second;
		if (data[x][y].is_assign && num <= data[x][y].num)
		{
			return false;
		}
	}
	for (int i = 0; i < data[i1][j1].more.size(); i++)
	{
		int x = data[i1][j1].more[i].first;
		int y = data[i1][j1].more[i].second;
		if (data[x][y].is_assign && num >= data[x][y].num)
			return false;
	}
	return true;
}

#ifdef DEBUG
int main()
{
	chart mychart;
	mychart.show();
	bool temp = mychart.findsolution();
	mychart.show();
	/*for(int i = 0; i<N; i++)
		for (int j = 0; j < N; j++)
		{
			cout << mychart.data[i][j].min << " " << mychart.data[i][j].max << endl;
		}*/
	//cout << mychart.data[7][8].min << " " << mychart.data[7][8].max << endl;
	//cout << "???" << endl;
	int count = 0;
	for(int i = 0; i<N; i++)
		for (int j = 0; j < N; j++)
		{
			count += mychart.data[i][j].less.size();
		}
	cout << count << endl;
	return 0;
}
#endif // DEBUG


#ifndef DEBUG
int main()
{
	return 0;
}
#endif // !DEBUG
