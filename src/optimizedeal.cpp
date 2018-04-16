/*
	代码优化
*/

#include "optimizedeal.h"

namespace compiler{
	struct tree_node {
		//叶子节点对应的属性
		std::string name;	//节点所对应的变量名, 叶子结点才会有

		//中间节点对应属性
		int op_id;	// +,-,*,/,= 的操作符对应的midop编号, -1 代表其为叶子节点
		struct tree_node* lchild;	//左孩子node编号
		struct tree_node* rchild; //右孩子node编号		
		std::vector<std::string> var_names;	//该节点所对应的变量名称链表
		
		//共有
		bool is_mid_node;	//是否为中间节点
		std::vector<struct tree_node*> parents; //父节点, 在dealVarZ中对两个孩子节点的父节点进行更新操作

		
	};

	std::vector<tree_node*> nodeQueue;	//从DAG图导出中间代码时使用
	
	std::map<std::string, tree_node*> Node_Map;	//Dag图的变量名-节点图, 在每次全部导出之后清空
	std::map<std::string, std::string> F_S_Map;		//F变量名与原名称的对应关系

	std::vector<std::string> TVar;	//因为公共子表达式删除闲置下来的临时变量名, 在creatOptCode一开始进行生成

	std::map<std::string, std::string> Old_New_Map;	//临时变量的新旧变量名称对照表,以函数体为单位对导出后的中间代码段进行变量名称更新, 在creatOptCode中生成

	int F_Count = 0;	//每个局部块处理完之后清零
	
	//用于处理a0 = a这种情况, 统一用F0、F1等来代替
	std::string creatVarName() {
		return "F" + int_to_string(F_Count++);
	}
	
	
	bool isInNodeMap(tree_node* t) {
		for (auto iter = Node_Map.begin(); iter != Node_Map.end();++iter) {
			if (iter->second == t) {
				return true;
			}
		}
		return false;
	}

	void deleteInNodeMap(tree_node* t) {
		for (auto iter = Node_Map.begin(); iter != Node_Map.end();) {
			if (iter->second == t) {
				Node_Map.erase(iter++);
			}
			else {
				iter++;
			}
		}
	}
	
	//从Node_Map中得到一个没有父节点的中间节点， 得到后删除
	tree_node* getMidCode() {
		auto iter = Node_Map.begin();
		tree_node* t = NULL;
		for (; iter != Node_Map.end();++iter) {
			
			if (iter->second->is_mid_node && iter->second->parents.size() == 0) {
				t = iter->second;
				break;
			}
		}
		if(t != NULL) deleteInNodeMap(t);
		return t;
	}


	//在node_map表中寻找当前变量名是否已经存在, 若存在返回其节点指针
	tree_node* findInNodeMap(std::string name) {
		auto iter = Node_Map.find(name);
		if (iter == Node_Map.end()) return NULL;	//没找到
		else {
			return iter->second;
		}
	
	}

	bool isInOldNewMap(std::string name) {
		auto iter = Old_New_Map.find(name);
		if (iter == Old_New_Map.end()) return false;	//没找到
		else {
			return true;
		}
	}

	//判断当前处理的表达式是否已经有等价的节点, 若有则返回其节点指针
	tree_node* expEqual(int op, tree_node* lchild, tree_node* rchild) {
		auto iter = Node_Map.begin();
		tree_node* temp;
		for (; iter != Node_Map.end(); ++iter) {
			temp = iter->second;
			if ((temp->op_id == op) &&
				(((temp->lchild == lchild) && (temp->rchild == rchild)) || ((temp->lchild == rchild) && (temp->rchild == lchild)))) {
				return temp;
			}
		}
		//没有找到等价的
		return NULL;
	}

	/*
		参数:	name 变量名(有可能为数字, 数字肯定为叶子节点)
		返回值类型: tree_node*
		函数作用: 处理操作数的变量名, 若已经存在则返回其节点指针, 若不存在则生成一个叶子结点
	*/
	tree_node* dealVarXY(std::string name) {
		tree_node * nodep = findInNodeMap(name);
		if (nodep != NULL) return nodep;
		//作为操作数未在之前出现过, 则其为叶子节点
		nodep = new tree_node;
		nodep->op_id = -1;
		nodep->lchild = NULL;
		nodep->rchild = NULL;
		nodep->is_mid_node = false;
		nodep->name = name;

		Node_Map[name] = nodep;
		return nodep;
	}

	/*
		参数:	name 变量名(局部变量或者临时变量, 对于局部变量要同时插入两个map中)
				is_assign在简单的赋值中设置为1, 数组赋值设置为0
		返回值类型: void
		函数作用: 处理表达式结果的变量名, 表达式等价的条件: (op == op_id) && (((lparent == lparent) && (rparent == rparent)) || ((lparent == rparent) && (rparent == lparent))) 
		注意: 若变量名已经存在而表达式不等价, 相当于对原变量进行重新定义, 则需要进行类似a0 = a的处理
	*/

	void dealVarZ(std::string name, int op, tree_node* lchild, tree_node* rchild, bool is_assign) {
		tree_node *z_node = expEqual(op, lchild, rchild);
		if (z_node == NULL) {
			tree_node *temp = findInNodeMap(name);	//查找当前变量名是否已经作为节点使用, 对应a0 = a这种处理情况
			if (temp != NULL) {	//若已经存在该变量名的节点
				if (temp->is_mid_node) {	//中间节点
					std::vector<std::string>::iterator iter = std::find(temp->var_names.begin(), temp->var_names.end(), name);	//查找到在中间节点的位置
					if (iter == temp->var_names.end()) {
						//std::cout << "There is a bug in optimizedeal--dealVarZ: iter can't be temp->var_names.end()" << std::endl;
						return;
					}
					std::string new_name = creatVarName();
					F_S_Map[new_name] = *iter;
					*iter = new_name;
					Node_Map[*iter] = temp;
				}
				else {		//叶子节点
					std::string new_name = creatVarName();
					F_S_Map[new_name] = temp->name;
					temp->name = new_name;	//更改变量名
					Node_Map[temp->name] = temp;
				}
			}
			
			//简单赋值且为 某个值等于中间节点的情况, 则把该值加入到该变量的变量链表中
			if (is_assign && lchild->is_mid_node) {
				lchild->var_names.push_back(name);
				z_node = lchild;
			}
			else {
				z_node = new tree_node;
				z_node->is_mid_node = true;
				z_node->name = " ";
				z_node->lchild = lchild;
				z_node->rchild = rchild;
				z_node->op_id = op;
				z_node->var_names.push_back(name);
				lchild->parents.push_back(z_node);
				if (rchild != NULL)rchild->parents.push_back(z_node);
			}
			/*
				处理name已经存在在Node_Map中的情况
				对于 a0 = a的情况, 找因为局部表达式消除得到的临时变量来替换(若没有则对该代码块不进行优化处理)
			*/
		}
		else {		//已经有等价的表达式
			z_node->var_names.push_back(name);//将变量名加入该节点
		}
		Node_Map[name] = z_node;
	}

	//递归调用得到以root为根节点的子树
	void getNodeQueue(tree_node* root){
		if (root == NULL) {
			//std::cout << "There is a bug in optimizedeal--getNodeQueue: root be NULL" << std::endl;
			return;
		}
		if (!root->is_mid_node) {
			//std::cout << "There is a bug in optimizedeal--getNodeQueue: root must be midcode" << std::endl;
			return;
		}
		if (root->parents.size() != 0) {
			//std::cout << "There is a bug in optimizedeal--getNodeQueue: root'parents must be 0" << std::endl;
			return;
		}
		
		//将节点拿到队列中
		tree_node* lt = NULL;
		tree_node* rt = NULL;
		nodeQueue.push_back(root);
		
		deleteInNodeMap(root);
		
		//将root左子树和右子树的父节点进行更新
		lt = root->lchild;
		rt = root->rchild;
		//for (auto iter = lt->parents.begin(); iter != lt->parents.end(); iter++) {
		//	std::cout << "parents:"<< *iter << " "<<std::endl;
		//}
		//std::cout << "root:" << root << std::endl;
		//std::cout << "lt:" << lt << std::endl;
		//std::cout << "rt:" << rt << std::endl;
		//std::cout << std::endl;

		if (lt->is_mid_node && isInNodeMap(lt)) {

			std::vector<tree_node*>::iterator iter = std::find(lt->parents.begin(), lt->parents.end(), root);	//查找到在中间节点的位置
			if (iter == lt->parents.end()) {
				//std::cout << "There is a bug in optimizedeal--getNodeQueue: iter can't be lt->parents.end()" << std::endl;
				return;
			}
			lt->parents.erase(iter);
		}
		if (rt != NULL && rt->is_mid_node && isInNodeMap(rt)) {
			std::vector<tree_node*>::iterator iter = std::find(rt->parents.begin(), rt->parents.end(), root);
			if (iter == rt->parents.end()) {
				//std::cout << "There is a bug in optimizedeal--getNodeQueue: iter can't be rt->parents.end()" << std::endl;
				return;
			}
			rt->parents.erase(iter);
		}

		//选择进入左子树还是右子树还是直接return
		if (lt->is_mid_node && lt->parents.size() == 0 && isInNodeMap(lt)) {
			getNodeQueue(lt);
		}
		if (rt != NULL && rt->is_mid_node && rt->parents.size() == 0 && isInNodeMap(lt)) {
			getNodeQueue(rt);
		}
		return;
			 
	}

	
	//若需要被替换的F数量大于多余的临时变量, 返回false， 否则返回true
	bool dealNodeVarnames() {
		int i;
		int fcount = 0, tcount = 0;
		//遍历中间节点对F_Count计数
		for (i = 0; i < nodeQueue.size(); i++) {
			if (nodeQueue[i]->var_names.size() == 1) {
				if (nodeQueue[i]->var_names[0][0] == 'F') {	//只有一个名字且为F打头
					fcount++;
				}
				nodeQueue[i]->name = nodeQueue[i]->var_names[0];
			}
			else if (nodeQueue[i]->var_names.size() > 1) {
				auto iter = nodeQueue[i]->var_names.begin();
				bool only_F = false;
				bool have_others = false;
				bool have_svar = false;	//标志该节点是否有局部变量
				
				//先处理F
				for (; iter != nodeQueue[i]->var_names.end(); ) {
					if (iter[0][0] == 'F') {
						if (((iter + 1) == nodeQueue[i]->var_names.end()) && (have_others == false)) {
							only_F = true;
							nodeQueue[i]->name = *iter;
							fcount++;
							break;
						}
						else {
							//std::cout << "deal F  var_names.size:" << nodeQueue[i]->var_names.size() << std::endl;
							iter = nodeQueue[i]->var_names.erase(iter);
							//std::cout << "deal F  var_names.size:" << nodeQueue[i]->var_names.size() << std::endl;
							//std::cout <<(iter == nodeQueue[i]->var_names.end()) << std::endl;
						}
					}
					else {
						have_others = true;
						iter++;
					}
				}
				if (only_F) {
					if (nodeQueue[i]->var_names.size() != 1) {
						//std::cout << "There is a bug in optimizedeal--dealNodeVarnames:only_F == true " << std::endl;
						return false;
					}
					else {
						continue;
					}
				}
				
				//再处理T, 只做计数不进行删除
				for (iter = nodeQueue[i]->var_names.begin();iter != nodeQueue[i]->var_names.end() ; ) {
					if (iter[0][0] == 'T') {	
						if ((iter + 1 == nodeQueue[i]->var_names.end()) && (have_svar == false)) {
							nodeQueue[i]->name = *iter;		//最后一个临时变量的名字
							iter++;
						}
						else {
							tcount++;
							iter++;
						}
					}
					
					else {
						have_svar = true;
						nodeQueue[i]->name = *iter;		//变量的名字
						iter++;
					}
					
				}
				
			}
			else {
				//std::cout << "There is a bug in optimizedeal--dealNodeVarnames: nodeQueue[i]->var_names.size() < 1" << std::endl;
				return false;
			}
		}

		//遍历叶子结点对F_Count计数
		for (auto iter = Node_Map.begin(); iter != Node_Map.end(); iter++) {
			if (!iter->second->is_mid_node) {
				if (iter->second->name[0] == 'F') {
					fcount++;
				}
			}
		}
		if (fcount > tcount) {
			F_Count = F_Count - fcount;
			return false;
		}
		else return true;
	}
	
	/*
		参数: 无
		返回值类型: void
		函数作用: 利用启发式算法依据Node_Map 和 SVar_map导出优化后的中间代码, 首先从last_Z开始
	*/
	bool creatOptCode() {
		//顺序插入, 倒序导出
		
		tree_node* t = getMidCode();

		while (t != NULL) {
			//std::cout << "======" << std::endl;
			getNodeQueue(t);
			t = getMidCode();
			
		}
		//std::cout << "creatOptCode--after getNodeQueue" << std::endl;
		//此处t == NULL
		if (dealNodeVarnames()) {
			//std::cout << "creatOptCode--after dealNodeVarnames" << std::endl;
			for (auto iter = F_S_Map.begin(); iter != F_S_Map.end(); ++iter) {
				enterMidOptCode(4, iter->first, " ", iter->second, 0);
			}
			
			while (nodeQueue.size() != 0) {
				t = nodeQueue.back();	//倒序导出
				
				nodeQueue.erase(nodeQueue.end() - 1);
				if (t == NULL) {
					//std::cout << "There is a bug in optimizedeal--creatOptCode: t == NULL" << std::endl;
					return false;
				}
				if (!t->is_mid_node) {
					//std::cout << "There is a bug in optimizedeal--creatOptCode: t is not mid node" << std::endl;
					return false;
				}
				if (t->name == " ") {
					//std::cout << "There is a bug in optimizedeal--creatOptCode: t->name = " "" << std::endl;
					return false;
				}
				if (t->var_names.size() < 1) {
					//std::cout << "There is a bug in optimizedeal--creatOptCode: t->var_names.size() < 1 " << std::endl;
					return false;
				}
				
				if (t->rchild != NULL) {
					enterMidOptCode(t->op_id, t->name, t->lchild->name, t->rchild->name, 0);
				}
				else {	//简单赋值语句
					enterMidOptCode(t->op_id, t->name, " ", t->lchild->name, 0);
				}
				//std::cout << "creatOptCode--after enterMidOptCode" << std::endl;
				if (t->var_names.size() > 1) {
					auto iter = t->var_names.begin();
					for (; iter != t->var_names.end();) {
						if (*iter == t->name) {
							
							if(iter[0][0] != 'F')Old_New_Map[*iter] = t->name;
							iter++;
						}
						else {
							if (iter[0][0] == 'T') {
								TVar.push_back(*iter);
								Old_New_Map[*iter] = t->name;
							}
							else if (iter[0][0] == 'F') {
								//std::cout << "There is a bug in optimizedeal--creatOptCode: iter[0][0] can't be F";
								return false;
							}
							else {
								enterMidOptCode(4, *iter, " ", t->name, 0);
								Old_New_Map[*iter] = *iter;
							}
							iter++;
						}
					}
				}

			}

			return true;
		}
		else {
			return false;
		}
		/*
			导出时首先计算F_Count与被替换掉的临时变量个数大小, 若被替换掉的 T个数 < F_Count, 说明优化没有意义, 则返回false，
			依旧用源程序代码,  Old_New_Map中不插入新的东西, 否则正常优化处理
			这一点PPT上可以用来展示
		*/


		/*
			处理赋值语句导出的时候需要注意一下数组赋值和普通赋值
		*/
	}

	/*
		参数:	mid_exp_start: 局部表达式在MidCodeT表中的开始位置
				mid_exp_end: 局部表达式在MidCodeT表中的结束位置 (指向的指令op不是 + - * / =)
		返回值: void
		函数作用: 对中间代码的一个局部表达式进行优化, 优化范围[mid_exp_start, mid_exp_end)
	*/
	void dealLocalExp(int mid_exp_start, int mid_exp_end) {
		//std::cout << mid_exp_start << " " << mid_exp_end << std::endl;
		int i;
		tree_node *lchild = NULL, *rchild = NULL;
		midcode cur_mid;

		//生成树的过程
		for (i = mid_exp_start; i < mid_exp_end; i++) {
			cur_mid = MidCodeT.midcodeArray[i];
			if (cur_mid.op == 4) {
				if (cur_mid.x == " ") {		// z = y 设置一个中间节点
					//对于 a = 5, b = a, c = b这种只设置 一个叶子节点5和一个中间节点a, b, c都在a的中间节点表中 
					lchild = dealVarXY(cur_mid.y);
					//std::cout << "dealLocalExp--after cur_mid.op == 4--dealVarXY" << std::endl;
					rchild = NULL;
					dealVarZ(cur_mid.z, cur_mid.op, lchild, rchild, true);
					//std::cout << "dealLocalExp--after cur_mid.op == 4--dealVarZ" << std::endl;
				}
				else {			//z = y[x] 对于数组赋值, 左孩子为数组偏移, 右孩子为数组名
					
					lchild = dealVarXY(cur_mid.x);	
					rchild = dealVarXY(cur_mid.y);
					dealVarZ(cur_mid.z, cur_mid.op, lchild, rchild, false);
				}
			}
			else {
				lchild = dealVarXY(cur_mid.x);	//处理操作数x
				rchild = dealVarXY(cur_mid.y);	//处理操作数y
				dealVarZ(cur_mid.z, cur_mid.op, lchild, rchild, false);
			}
		}

		//std::cout << "dealLocalExp--after creat tree" << std::endl;

		//从DAG图中导出的过程  主要依据Node_Map 和 SVar_Map
		if (creatOptCode()) {
			F_S_Map.clear();
			Node_Map.clear();
			return;
		}
		else {		//不进行优化, 将原中间代码导出
			for (i = mid_exp_start; i < mid_exp_end; i++) {
				enterMidOptCode(MidCodeT.midcodeArray[i].op, MidCodeT.midcodeArray[i].z, MidCodeT.midcodeArray[i].x, MidCodeT.midcodeArray[i].y, MidCodeT.midcodeArray[i].isstart);
			}
			F_S_Map.clear();
			Node_Map.clear();
		}
	}


	/*
		参数:	mid_func_start: 函数在MidCodeT表中的开始标签位置
				mid_func_end: 下一个函数在MidCodeT表中的开始标签位置
		返回值: void
		函数作用: 对中间代码的一个函数体进行表达式优化, 优化范围[mid_func_start, mid_func_end) 只处理 + - * / =
	*/
	void dealFuncExp(int mid_func_start, int mid_func_end) {
		//std::cout << "mid_func_start:" << mid_func_start << " mid_func_end:" << mid_func_end << std::endl;
		int mid_exp_start, mid_exp_end;
		int midopt_func_start = MidCodeOptT.mdc;
		int i;
		for (i = mid_func_start; i < mid_func_end; i++) {
			if ((MidCodeT.midcodeArray[i].op >= 0) && (MidCodeT.midcodeArray[i].op <= 4)) {
				mid_exp_start = i;
				//处理连续的表达式
				for (; (MidCodeT.midcodeArray[i].op >= 0) && (MidCodeT.midcodeArray[i].op <= 4) && i < mid_func_end; i++);
				mid_exp_end = i;	//跳出来时i不满足条件
				if (mid_exp_end - mid_exp_start > 1) {
					dealLocalExp(mid_exp_start, mid_exp_end);
					enterMidOptCode(MidCodeT.midcodeArray[i].op, MidCodeT.midcodeArray[i].z, MidCodeT.midcodeArray[i].x, MidCodeT.midcodeArray[i].y, MidCodeT.midcodeArray[i].isstart);
				}
				else {
					enterMidOptCode(MidCodeT.midcodeArray[i-1].op, MidCodeT.midcodeArray[i-1].z, MidCodeT.midcodeArray[i-1].x, MidCodeT.midcodeArray[i-1].y, MidCodeT.midcodeArray[i-1].isstart);
					enterMidOptCode(MidCodeT.midcodeArray[i].op, MidCodeT.midcodeArray[i].z, MidCodeT.midcodeArray[i].x, MidCodeT.midcodeArray[i].y, MidCodeT.midcodeArray[i].isstart);
				}
			}
			else {
				enterMidOptCode(MidCodeT.midcodeArray[i].op, MidCodeT.midcodeArray[i].z, MidCodeT.midcodeArray[i].x, MidCodeT.midcodeArray[i].y, MidCodeT.midcodeArray[i].isstart);
			}
		}
		//if (F_Count > TVar.size()) std::cout << "F_Count:"<<F_Count<<" TVar.size():"<<TVar.size()<< std::endl;
		for (i = 0; i < F_Count; i++) {
			std::string F_name = "F" + int_to_string(i);
			if (TVar.size() == 0) {
				//std::cout << "There is a bug in optimizedeal--dealFuncExp: TVar.size() == 0" << std::endl;
				return;
			}
			std::string T_name = TVar.back();
			TVar.erase(TVar.end() - 1);
			Old_New_Map[F_name] = T_name;
		}


		//根据Old_New_Map更新变量名T
		for (i = midopt_func_start; i < MidCodeOptT.mdc; i++) {
			midcode cur_midopt = MidCodeOptT.midcodeArray[i];
			if (cur_midopt.op > 5 && cur_midopt.op < 12) {
				if (isInOldNewMap(cur_midopt.x) && cur_midopt.x[0] == 'T') {
					MidCodeOptT.midcodeArray[i].x = Old_New_Map[cur_midopt.x];
				}
				if (isInOldNewMap(cur_midopt.y) && cur_midopt.y[0] == 'T') {
					MidCodeOptT.midcodeArray[i].y = Old_New_Map[cur_midopt.y];
				}
			}
			else if ((cur_midopt.op == 12) ||
				(cur_midopt.op == 17) ||
				(cur_midopt.op == 18) ||
				(cur_midopt.op == 20)) {
				if (isInOldNewMap(cur_midopt.y) && cur_midopt.y[0] == 'T') {
					MidCodeOptT.midcodeArray[i].y = Old_New_Map[cur_midopt.y];
				}
			}
		}
		
		//根据Old_New_Map更新变量名F
		for (i = midopt_func_start; i < MidCodeOptT.mdc; i++) {
			midcode cur_midopt = MidCodeOptT.midcodeArray[i];
			if (cur_midopt.op < 12) {
				if (cur_midopt.z[0] == 'F') {
					MidCodeOptT.midcodeArray[i].z = Old_New_Map[cur_midopt.z];
				}
				if (cur_midopt.x[0] == 'F') {
					MidCodeOptT.midcodeArray[i].x = Old_New_Map[cur_midopt.x];
				}
				if (cur_midopt.y[0] == 'F') {
					MidCodeOptT.midcodeArray[i].y = Old_New_Map[cur_midopt.y];
				}
			}
			else if ((cur_midopt.op == 12) ||
				(cur_midopt.op == 17) ||
				(cur_midopt.op == 18) ||
				(cur_midopt.op == 20)) {
				if (cur_midopt.y[0] == 'F') {
					MidCodeOptT.midcodeArray[i].y = Old_New_Map[cur_midopt.y];
				}
			}
		}
		

		//清空vector和Map
		F_Count = 0;
		Old_New_Map.clear();
		nodeQueue.clear();
		TVar.clear();
		
	}
	
	//对外接口函数
	void expOpt() {
		int mid_func_start, mid_func_end;
		int i;
		for (i = 0; i < MidCodeT.mdc; i++) {
			if (MidCodeT.midcodeArray[i].op == 14) {	//函数开始位置标签
				mid_func_start = i;
				i++;
				for (; i < MidCodeT.mdc; i++) {
					if (MidCodeT.midcodeArray[i].op == 14) break;
				}

				//出循环的条件: MidCodeT.midcodeArray[i].isstart == 4 或者 i = MidCodeT.mdc
				// 此时为下一个函数的开始位置
				mid_func_end = i;
				
				//调用函数块优化
				dealFuncExp(mid_func_start, mid_func_end);
				//如果没有if语句那么在main的结束之后会陷入一个 i-- i++ 的死循环
				if(i != MidCodeT.mdc) i--;	//保证下一个开始位置标签不会被跳过, 上一个的end = 下一个的start
			}
		}
	}

}