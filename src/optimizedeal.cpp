/*
	�����Ż�
*/

#include "optimizedeal.h"

namespace compiler{
	struct tree_node {
		//Ҷ�ӽڵ��Ӧ������
		std::string name;	//�ڵ�����Ӧ�ı�����, Ҷ�ӽ��Ż���

		//�м�ڵ��Ӧ����
		int op_id;	// +,-,*,/,= �Ĳ�������Ӧ��midop���, -1 ������ΪҶ�ӽڵ�
		struct tree_node* lchild;	//����node���
		struct tree_node* rchild; //�Һ���node���		
		std::vector<std::string> var_names;	//�ýڵ�����Ӧ�ı�����������
		
		//����
		bool is_mid_node;	//�Ƿ�Ϊ�м�ڵ�
		std::vector<struct tree_node*> parents; //���ڵ�, ��dealVarZ�ж��������ӽڵ�ĸ��ڵ���и��²���

		
	};

	std::vector<tree_node*> nodeQueue;	//��DAGͼ�����м����ʱʹ��
	
	std::map<std::string, tree_node*> Node_Map;	//Dagͼ�ı�����-�ڵ�ͼ, ��ÿ��ȫ������֮�����
	std::map<std::string, std::string> F_S_Map;		//F��������ԭ���ƵĶ�Ӧ��ϵ

	std::vector<std::string> TVar;	//��Ϊ�����ӱ��ʽɾ��������������ʱ������, ��creatOptCodeһ��ʼ��������

	std::map<std::string, std::string> Old_New_Map;	//��ʱ�������¾ɱ������ƶ��ձ�,�Ժ�����Ϊ��λ�Ե�������м����ν��б������Ƹ���, ��creatOptCode������

	int F_Count = 0;	//ÿ���ֲ��鴦����֮������
	
	//���ڴ���a0 = a�������, ͳһ��F0��F1��������
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
	
	//��Node_Map�еõ�һ��û�и��ڵ���м�ڵ㣬 �õ���ɾ��
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


	//��node_map����Ѱ�ҵ�ǰ�������Ƿ��Ѿ�����, �����ڷ�����ڵ�ָ��
	tree_node* findInNodeMap(std::string name) {
		auto iter = Node_Map.find(name);
		if (iter == Node_Map.end()) return NULL;	//û�ҵ�
		else {
			return iter->second;
		}
	
	}

	bool isInOldNewMap(std::string name) {
		auto iter = Old_New_Map.find(name);
		if (iter == Old_New_Map.end()) return false;	//û�ҵ�
		else {
			return true;
		}
	}

	//�жϵ�ǰ����ı��ʽ�Ƿ��Ѿ��еȼ۵Ľڵ�, �����򷵻���ڵ�ָ��
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
		//û���ҵ��ȼ۵�
		return NULL;
	}

	/*
		����:	name ������(�п���Ϊ����, ���ֿ϶�ΪҶ�ӽڵ�)
		����ֵ����: tree_node*
		��������: ����������ı�����, ���Ѿ������򷵻���ڵ�ָ��, ��������������һ��Ҷ�ӽ��
	*/
	tree_node* dealVarXY(std::string name) {
		tree_node * nodep = findInNodeMap(name);
		if (nodep != NULL) return nodep;
		//��Ϊ������δ��֮ǰ���ֹ�, ����ΪҶ�ӽڵ�
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
		����:	name ������(�ֲ�����������ʱ����, ���ھֲ�����Ҫͬʱ��������map��)
				is_assign�ڼ򵥵ĸ�ֵ������Ϊ1, ���鸳ֵ����Ϊ0
		����ֵ����: void
		��������: ������ʽ����ı�����, ���ʽ�ȼ۵�����: (op == op_id) && (((lparent == lparent) && (rparent == rparent)) || ((lparent == rparent) && (rparent == lparent))) 
		ע��: ���������Ѿ����ڶ����ʽ���ȼ�, �൱�ڶ�ԭ�����������¶���, ����Ҫ��������a0 = a�Ĵ���
	*/

	void dealVarZ(std::string name, int op, tree_node* lchild, tree_node* rchild, bool is_assign) {
		tree_node *z_node = expEqual(op, lchild, rchild);
		if (z_node == NULL) {
			tree_node *temp = findInNodeMap(name);	//���ҵ�ǰ�������Ƿ��Ѿ���Ϊ�ڵ�ʹ��, ��Ӧa0 = a���ִ������
			if (temp != NULL) {	//���Ѿ����ڸñ������Ľڵ�
				if (temp->is_mid_node) {	//�м�ڵ�
					std::vector<std::string>::iterator iter = std::find(temp->var_names.begin(), temp->var_names.end(), name);	//���ҵ����м�ڵ��λ��
					if (iter == temp->var_names.end()) {
						//std::cout << "There is a bug in optimizedeal--dealVarZ: iter can't be temp->var_names.end()" << std::endl;
						return;
					}
					std::string new_name = creatVarName();
					F_S_Map[new_name] = *iter;
					*iter = new_name;
					Node_Map[*iter] = temp;
				}
				else {		//Ҷ�ӽڵ�
					std::string new_name = creatVarName();
					F_S_Map[new_name] = temp->name;
					temp->name = new_name;	//���ı�����
					Node_Map[temp->name] = temp;
				}
			}
			
			//�򵥸�ֵ��Ϊ ĳ��ֵ�����м�ڵ�����, ��Ѹ�ֵ���뵽�ñ����ı���������
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
				����name�Ѿ�������Node_Map�е����
				���� a0 = a�����, ����Ϊ�ֲ����ʽ�����õ�����ʱ�������滻(��û����Ըô���鲻�����Ż�����)
			*/
		}
		else {		//�Ѿ��еȼ۵ı��ʽ
			z_node->var_names.push_back(name);//������������ýڵ�
		}
		Node_Map[name] = z_node;
	}

	//�ݹ���õõ���rootΪ���ڵ������
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
		
		//���ڵ��õ�������
		tree_node* lt = NULL;
		tree_node* rt = NULL;
		nodeQueue.push_back(root);
		
		deleteInNodeMap(root);
		
		//��root���������������ĸ��ڵ���и���
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

			std::vector<tree_node*>::iterator iter = std::find(lt->parents.begin(), lt->parents.end(), root);	//���ҵ����м�ڵ��λ��
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

		//ѡ�������������������������ֱ��return
		if (lt->is_mid_node && lt->parents.size() == 0 && isInNodeMap(lt)) {
			getNodeQueue(lt);
		}
		if (rt != NULL && rt->is_mid_node && rt->parents.size() == 0 && isInNodeMap(lt)) {
			getNodeQueue(rt);
		}
		return;
			 
	}

	
	//����Ҫ���滻��F�������ڶ������ʱ����, ����false�� ���򷵻�true
	bool dealNodeVarnames() {
		int i;
		int fcount = 0, tcount = 0;
		//�����м�ڵ��F_Count����
		for (i = 0; i < nodeQueue.size(); i++) {
			if (nodeQueue[i]->var_names.size() == 1) {
				if (nodeQueue[i]->var_names[0][0] == 'F') {	//ֻ��һ��������ΪF��ͷ
					fcount++;
				}
				nodeQueue[i]->name = nodeQueue[i]->var_names[0];
			}
			else if (nodeQueue[i]->var_names.size() > 1) {
				auto iter = nodeQueue[i]->var_names.begin();
				bool only_F = false;
				bool have_others = false;
				bool have_svar = false;	//��־�ýڵ��Ƿ��оֲ�����
				
				//�ȴ���F
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
				
				//�ٴ���T, ֻ������������ɾ��
				for (iter = nodeQueue[i]->var_names.begin();iter != nodeQueue[i]->var_names.end() ; ) {
					if (iter[0][0] == 'T') {	
						if ((iter + 1 == nodeQueue[i]->var_names.end()) && (have_svar == false)) {
							nodeQueue[i]->name = *iter;		//���һ����ʱ����������
							iter++;
						}
						else {
							tcount++;
							iter++;
						}
					}
					
					else {
						have_svar = true;
						nodeQueue[i]->name = *iter;		//����������
						iter++;
					}
					
				}
				
			}
			else {
				//std::cout << "There is a bug in optimizedeal--dealNodeVarnames: nodeQueue[i]->var_names.size() < 1" << std::endl;
				return false;
			}
		}

		//����Ҷ�ӽ���F_Count����
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
		����: ��
		����ֵ����: void
		��������: ��������ʽ�㷨����Node_Map �� SVar_map�����Ż�����м����, ���ȴ�last_Z��ʼ
	*/
	bool creatOptCode() {
		//˳�����, ���򵼳�
		
		tree_node* t = getMidCode();

		while (t != NULL) {
			//std::cout << "======" << std::endl;
			getNodeQueue(t);
			t = getMidCode();
			
		}
		//std::cout << "creatOptCode--after getNodeQueue" << std::endl;
		//�˴�t == NULL
		if (dealNodeVarnames()) {
			//std::cout << "creatOptCode--after dealNodeVarnames" << std::endl;
			for (auto iter = F_S_Map.begin(); iter != F_S_Map.end(); ++iter) {
				enterMidOptCode(4, iter->first, " ", iter->second, 0);
			}
			
			while (nodeQueue.size() != 0) {
				t = nodeQueue.back();	//���򵼳�
				
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
				else {	//�򵥸�ֵ���
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
			����ʱ���ȼ���F_Count�뱻�滻������ʱ����������С, �����滻���� T���� < F_Count, ˵���Ż�û������, �򷵻�false��
			������Դ�������,  Old_New_Map�в������µĶ���, ���������Ż�����
			��һ��PPT�Ͽ�������չʾ
		*/


		/*
			����ֵ��䵼����ʱ����Ҫע��һ�����鸳ֵ����ͨ��ֵ
		*/
	}

	/*
		����:	mid_exp_start: �ֲ����ʽ��MidCodeT���еĿ�ʼλ��
				mid_exp_end: �ֲ����ʽ��MidCodeT���еĽ���λ�� (ָ���ָ��op���� + - * / =)
		����ֵ: void
		��������: ���м�����һ���ֲ����ʽ�����Ż�, �Ż���Χ[mid_exp_start, mid_exp_end)
	*/
	void dealLocalExp(int mid_exp_start, int mid_exp_end) {
		//std::cout << mid_exp_start << " " << mid_exp_end << std::endl;
		int i;
		tree_node *lchild = NULL, *rchild = NULL;
		midcode cur_mid;

		//�������Ĺ���
		for (i = mid_exp_start; i < mid_exp_end; i++) {
			cur_mid = MidCodeT.midcodeArray[i];
			if (cur_mid.op == 4) {
				if (cur_mid.x == " ") {		// z = y ����һ���м�ڵ�
					//���� a = 5, b = a, c = b����ֻ���� һ��Ҷ�ӽڵ�5��һ���м�ڵ�a, b, c����a���м�ڵ���� 
					lchild = dealVarXY(cur_mid.y);
					//std::cout << "dealLocalExp--after cur_mid.op == 4--dealVarXY" << std::endl;
					rchild = NULL;
					dealVarZ(cur_mid.z, cur_mid.op, lchild, rchild, true);
					//std::cout << "dealLocalExp--after cur_mid.op == 4--dealVarZ" << std::endl;
				}
				else {			//z = y[x] �������鸳ֵ, ����Ϊ����ƫ��, �Һ���Ϊ������
					
					lchild = dealVarXY(cur_mid.x);	
					rchild = dealVarXY(cur_mid.y);
					dealVarZ(cur_mid.z, cur_mid.op, lchild, rchild, false);
				}
			}
			else {
				lchild = dealVarXY(cur_mid.x);	//���������x
				rchild = dealVarXY(cur_mid.y);	//���������y
				dealVarZ(cur_mid.z, cur_mid.op, lchild, rchild, false);
			}
		}

		//std::cout << "dealLocalExp--after creat tree" << std::endl;

		//��DAGͼ�е����Ĺ���  ��Ҫ����Node_Map �� SVar_Map
		if (creatOptCode()) {
			F_S_Map.clear();
			Node_Map.clear();
			return;
		}
		else {		//�������Ż�, ��ԭ�м���뵼��
			for (i = mid_exp_start; i < mid_exp_end; i++) {
				enterMidOptCode(MidCodeT.midcodeArray[i].op, MidCodeT.midcodeArray[i].z, MidCodeT.midcodeArray[i].x, MidCodeT.midcodeArray[i].y, MidCodeT.midcodeArray[i].isstart);
			}
			F_S_Map.clear();
			Node_Map.clear();
		}
	}


	/*
		����:	mid_func_start: ������MidCodeT���еĿ�ʼ��ǩλ��
				mid_func_end: ��һ��������MidCodeT���еĿ�ʼ��ǩλ��
		����ֵ: void
		��������: ���м�����һ����������б��ʽ�Ż�, �Ż���Χ[mid_func_start, mid_func_end) ֻ���� + - * / =
	*/
	void dealFuncExp(int mid_func_start, int mid_func_end) {
		//std::cout << "mid_func_start:" << mid_func_start << " mid_func_end:" << mid_func_end << std::endl;
		int mid_exp_start, mid_exp_end;
		int midopt_func_start = MidCodeOptT.mdc;
		int i;
		for (i = mid_func_start; i < mid_func_end; i++) {
			if ((MidCodeT.midcodeArray[i].op >= 0) && (MidCodeT.midcodeArray[i].op <= 4)) {
				mid_exp_start = i;
				//���������ı��ʽ
				for (; (MidCodeT.midcodeArray[i].op >= 0) && (MidCodeT.midcodeArray[i].op <= 4) && i < mid_func_end; i++);
				mid_exp_end = i;	//������ʱi����������
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


		//����Old_New_Map���±�����T
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
		
		//����Old_New_Map���±�����F
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
		

		//���vector��Map
		F_Count = 0;
		Old_New_Map.clear();
		nodeQueue.clear();
		TVar.clear();
		
	}
	
	//����ӿں���
	void expOpt() {
		int mid_func_start, mid_func_end;
		int i;
		for (i = 0; i < MidCodeT.mdc; i++) {
			if (MidCodeT.midcodeArray[i].op == 14) {	//������ʼλ�ñ�ǩ
				mid_func_start = i;
				i++;
				for (; i < MidCodeT.mdc; i++) {
					if (MidCodeT.midcodeArray[i].op == 14) break;
				}

				//��ѭ��������: MidCodeT.midcodeArray[i].isstart == 4 ���� i = MidCodeT.mdc
				// ��ʱΪ��һ�������Ŀ�ʼλ��
				mid_func_end = i;
				
				//���ú������Ż�
				dealFuncExp(mid_func_start, mid_func_end);
				//���û��if�����ô��main�Ľ���֮�������һ�� i-- i++ ����ѭ��
				if(i != MidCodeT.mdc) i--;	//��֤��һ����ʼλ�ñ�ǩ���ᱻ����, ��һ����end = ��һ����start
			}
		}
	}

}