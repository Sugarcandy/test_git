#include "stdio.h"
#include "MinHeap.h"
#include <iostream>
using namespace std; 

template<class Type>
class Traveling
{
	friend int main();
public:
	Type BBTSP(int v[]);
private:
	int n;		//图G的顶点数
	Type **a,	//图G的邻接矩阵
		NoEdge=0,		//图G的无边标识
		cc,			//当前费用
		bestc;		//当前最小费用
};

template<class Type>
class MinHeapNode
{
	friend Traveling<Type>;
public:
	operator Type() const
	{
		return lcost;
	}
private:
	Type lcost,		//子树费用的下届
		cc,		//当前费用
		rcost;	//x[s:n-1]中顶点最小出边费用和
	int s,			//根节点到当前节点的路径为x[0:s]
		*x;			//需要进一步搜索的顶点是x[s+1,n-1]
};

int main()
{
	int rec = 0;
	cout << "请输入测试次数：" << endl;
	cin >> rec;
	while (rec > 0) {
		int n = 0;//图的顶点数
		cout << "请输入图的顶点数";
		cin >> n;
		int *bestx = new int[n + 1];
		cout << "图的顶点个数 n=" << n << endl;

		int **a = new int*[n + 1];
		for (int i = 0; i <= n; i++)
		{
			a[i] = new int[n + 1];
		}

		cout << "请输入图的邻接矩阵:" << endl;

		for (int i = 1; i <= n; i++)
		{
			for (int j = 1; j <= n; j++)
			{
				cin >> a[i][j];
			}
		}

		Traveling<int> t;
		t.a = a;
		t.n = n;

		cout << "最短回路的长为：" << t.BBTSP(bestx) << endl;
		cout << "最短回路为：" << endl;
		for (int i = 1; i <= n; i++)
		{
			cout << bestx[i] << "-->";
		}
		cout << bestx[1] << endl;

		for (int i = 0; i <= n; i++)
		{
			delete[]a[i];
		}
		delete[]a;
		delete[]bestx;
		a = 0;
		rec--;
	}
	system("pause");
	return 0;
}

//解旅行员售货问题的优先队列式分支限界法
template<class Type>
Type Traveling<Type>::BBTSP(int v[])
{
	MinHeap<MinHeapNode<Type>> H(1000);
	Type * MinOut = new Type[n + 1];
	//计算MinOut[i] = 顶点i的最小出边费用
	Type MinSum = 0; //最小出边费用和
	for (int i = 1; i <= n; i++)
	{
		Type Min = NoEdge;
		for (int j = 1; j <= n; j++)
		{
			if (a[i][j] != NoEdge && (a[i][j]<Min || Min == NoEdge))
			{
				Min = a[i][j];
			}
		}
		if (Min == NoEdge)
		{
			return NoEdge;		//无回路
		}
		MinOut[i] = Min;
		MinSum += Min;
	}

	//初始化
	MinHeapNode<Type> E;
	E.x = new int[n];
	for (int i = 0; i<n; i++)
	{
		E.x[i] = i + 1;
	}
	E.s = 0;		//根节点到当前节点路径为x[0:s]
	E.cc = 0;		//当前费用
	E.rcost = MinSum;//最小出边费用和
	Type bestc = NoEdge;

	//搜索排列空间树
	while (E.s<n - 1)//非叶结点
	{
		if (E.s == n - 2)//当前扩展节点是叶节点的父节点
		{
			//再加2条边构成回路
			//所构成回路是否优于当前最优解
			if (a[E.x[n - 2]][E.x[n - 1]] != NoEdge && a[E.x[n - 1]][1] != NoEdge
				&& (E.cc + a[E.x[n - 2]][E.x[n - 1]] + a[E.x[n - 1]][1]<bestc
					|| bestc == NoEdge))
			{
				//费用更小的回路
				bestc = E.cc + a[E.x[n - 2]][E.x[n - 1]] + a[E.x[n - 1]][1];
				E.cc = bestc;
				E.lcost = bestc;
				E.s++;
				H.Insert(E);
			}
			else
			{
				delete[] E.x;//舍弃扩展节点
			}
		}
		else//产生当前扩展节点的儿子节点
		{
			for (int i = E.s + 1; i<n; i++)
			{
				if (a[E.x[E.s]][E.x[i]] != NoEdge)
				{
					//可行儿子节点
					Type cc = E.cc + a[E.x[E.s]][E.x[i]];
					Type rcost = E.rcost - MinOut[E.x[E.s]];
					Type b = cc + rcost;//下界
					if (b<bestc || bestc == NoEdge)
					{
						//子树可能含有最优解
						//节点插入最小堆
						MinHeapNode<Type> N;
						N.x = new int[n];
						for (int j = 0; j<n; j++)
						{
							N.x[j] = E.x[j];
						}
						N.x[E.s + 1] = E.x[i];
						N.x[i] = E.x[E.s + 1];
						N.cc = cc;
						N.s = E.s + 1;
						N.lcost = b;
						N.rcost = rcost;
						H.Insert(N);
					}
				}
			}
			delete[]E.x;//完成节点扩展
		}
		if (H.Size() == 0)
		{
			break;
		}
		H.DeleteMin(E);//取下一扩展节点
	}

	if (bestc == NoEdge)
	{
		return NoEdge;//无回路
	}
	//将最优解复制到v[1:n]
	for (int i = 0; i<n; i++)
	{
		v[i + 1] = E.x[i];
	}

	while (true)//释放最小堆中所有节点
	{
		delete[]E.x;
		if (H.Size() == 0)
		{
			break;
		}
		H.DeleteMin(E);//取下一扩展节点
	}
	return bestc;
}