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
	bool is_const;	//�ж��Ƿ�Ϊ����
	int num;	//��ֵ
	bool is_assign;	//�Ƿ񱻸�ֵ
	vector<pair<int, int>> less;	//����С������
	vector<pair<int, int>> more;	//����������
	int max;	//�Ͻ�
	int min;	//�½�
	bool domain[N];		//�ܹ�ȡֵ��ֵ��
	int count;	//��¼��ǰ�����￪ʼ
};

class chart
{
public:
	bool line[N][N];	//��ά����ʵ���ϵڶ�άָ�������֣�����˵����һ����ȡ��ô�ڶ�ά��0��Ϊtrue��
	bool row[N][N];
	node data[N][N];
	int count_const;	//��һ���ж��ٸ��̶���ֵ

	chart();	//����ܹ���ȡ����;
	void changeC(int i, int j, int label);	//����ĳһ�����ı����������min,max��ֵ��
	//void unchangeC(int i, int j);	//��ԭһ��ֵ����ʱû�ã���ע�͵���
	bool assign1(int i, int j);
	bool assign2(int i, int j);
	bool findsolution();
	void show();
	bool test(int i, int j, int num);
};
chart::chart()
{
	count_const = 0;
	for (int i = 0; i < N; i++)	//�������е��к����е��ж����Ա���ֵ��Ҳ�������еĽڵ㶼����ȡһ����ֵ;
		for(int j = 0; j<N; j++)
			line[i][j] = row[i][j] = true;
	for(int i = 0; i<N; i++)	//���Ƕ��ѭ��������������;
		for (int j = 0; j < N; j++)
		{
			cin >> data[i][j].num;
			if (data[i][j].num)
			{
				data[i][j].is_const = data[i][j].is_assign = true;
				row[i][data[i][j].num-1] = line[j][data[i][j].num-1] = false;	//��������ִ���1������Ҫ��һ����;
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
	while (cin >> p1.first)	//�����ڵ��Ĵ�С��ϵ;
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

void chart::changeC(int i1, int j1, int label)	//�ı��������ҵ�ֵ,labelΪ0ִ��more��labelΪ1ִ��less��ʵ���ϲ���1��ִ��more�ˡ�
{
	if (data[i1][j1].is_assign)
		return;
	int max = N+1;
	int min = 0;
	if (label == 1)
	{
		for (int i = 0; i < data[i1][j1].less.size(); i++)	//��minֵ
		{
			int x = data[i1][j1].less[i].first;
			int y = data[i1][j1].less[i].second;
			if (data[x][y].is_assign)
			{
				if (data[x][y].num >= min)	//num��Сֵ��1��min��Сֵ��0��num���ֵ��N��max���ֵ��N.
					min = data[x][y].num + 1;
			}
			else if (data[x][y].min >= min)
				min = data[x][y].min + 1;
		}
		data[i1][j1].min = min;
	}
	else
	{
		for (int i = 0; i < data[i1][j1].more.size(); i++)	//��maxֵ
		{
			int x = data[i1][j1].more[i].first;
			int y = data[i1][j1].more[i].second;
			if (data[x][y].is_assign)
			{
				if (data[x][y].num <= max)	//num��Сֵ��1��min��Сֵ��0��num���ֵ��N��max���ֵ��N.
					max = data[x][y].num - 1;
			}
			else if (data[x][y].max <= max)
				max = data[x][y].max - 1;
		}
		data[i1][j1].max = max;
	}
}

bool chart::assign1(int i1, int j1)	//��һ�θ�ֵ
{
	int count = 0;	//��һ�θ�ֵ��ʱ����һ���Լ��ܹ�ȡֵ��ֵ��count��¼ֵ�ĸ�����
	for (int i = data[i1][j1].min - 1; i < data[i1][j1].max; i++)
	{
		data[i1][j1].domain[i] = row[i1][i] && line[j1][i];
		if (data[i1][j1].domain[i])
			count += 1;
	}
	if (!count)	//����Ҳ����ܹ���ֵ�����֣���ô�ͷ���false��
		return false;
	for (int i = data[i1][j1].min - 1; i < data[i1][j1].max; i++)
	{
		if (!data[i1][j1].domain[i])//���Ϊ����ô�ͼ���
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

bool chart::assign2(int i1, int j1)	//�ڶ��θ�ֵ
{
	data[i1][j1].is_assign = false;
	line[j1][data[i1][j1].num - 1] = true;	//��ԭȡֵ��Χ
	row[i1][data[i1][j1].num - 1] = true;
	data[i1][j1].count += 1;
	for (int tempi = 0; tempi < data[i1][j1].less.size(); tempi++)
		changeC(data[i1][j1].less[tempi].first, data[i1][j1].less[tempi].second, 0);
	for (int tempi = 0; tempi < data[i1][j1].more.size(); tempi++)
		changeC(data[i1][j1].more[tempi].first, data[i1][j1].more[tempi].second, 1);
	for (int i = data[i1][j1].count; i < data[i1][j1].max; i++)
	{
		if (!data[i1][j1].domain[i])//���Ϊ����ô�ͼ���
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
			if (assign1(i, j))//�����һ���ܹ���ֵ
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
					if (!my_stack.size())	//�Ҳ�����
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
	cout << "��������ʱ��:" << end - begin << "s" << endl;
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
