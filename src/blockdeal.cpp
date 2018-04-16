/*
	语法语义分析, 将源程序转换为四元式中间指令
*/

//注意对整数的处理拆开来判断
#include <iostream>
#include "blockdeal.h"
namespace compiler{
	int temp_num = 0;	//生成临时变量时使用
	//std::map<std::string, int> MidLabelMap;		//标签与位置索引表
	int PC = 0;				//当前PC值
	std::fstream gram_out_file;
	int return_flag = 0; //在每次进入函数定义的时候设置为0,在return语句中设置为1或2,其中1代表没有返回值,2代表有返回值
						//然后在函数定义结束时进行检查,void函数必须为0或1,int函数为2, char函数为3

	symbol	block_start_sys[] = {constsy, voidsy, intsy, charsy};	//4个
	symbol  statement_start_sys[] = { semicolon, ifsy, dosy, lcbrack, ident, scanfsy, printfsy, switchsy, returnsy}; //9个
	symbol  statement_end_sys[] = { semicolon, rparent, rcbrack }; //3个
	symbol  expression_start_sys[] = {ident, plus, minus, intcon, charcon, lparent}; //6个
	//symbol  expression_end_sys[] = { ident, rsbrack, intcon, charcon, rparent }; //5个

	symbol	follow_expression_sys[] = { semicolon, rparent, rsbrack, lss, leq, gtr, geq, neq, eql, comma};	//10个,最后一个在处理函数调用语句时候才用

	symbol  factor_start_sys[] = { ident, plus, minus, intcon, charcon, lparent }; //6个
	symbol  factor_end_sys[] = { ident, rsbrack, intcon, charcon, rparent };//5个

	//部分函数声明
	std::string reUseFuncStatement();
	std::string expression(bool& is_char);
	std::string factor(bool& is_char);
	void statement();


	//跳读源程序直到取到的字符属于给定的符号集st, n为数组大小
	void skip(symbol st[], int n) {
		while (!find_sy(st, n)) insymbol();
	}

	//生成标签,参数l_kind为标签类型, 在s的基础上增加标签  pc为当前中间指令的位置(MidCodeT此时的索引值),索引label时使用
	std::string creat_label(std::string s, int l_kind, int pc){
		std::string label;
		switch(l_kind){
			case 0:	//函数类起始标签
				label =  s + "_begin_";
				break;
			case 1:	//函数类结束标签
				label =  s + "_end_";
				break;
			case 2: //if、while等跳转,在statement的末尾使用
				label = "_" + int_to_string(pc) + "_label_";
				break;
			case 3: //switch_end标签
				label = "switch_" + int_to_string(pc) + "_end_";
				break;
			case 4: //call函数之后返回地址标签,用于标记入口语句
				label = "call_" + int_to_string(pc) + "_";
				break;
			default:
				label = "ERROR";
				break;
		}
		//MidLabelMap[label] = pc;
		return label;
	}

	//生成临时变量
	std::string creat_tmp_var(){
		std::string tmp = int_to_string(temp_num);
		std::string t_var = "T" + tmp;
		var_info t_info;
		temp_num++;
		t_info.inReg = false;
		t_info.r_addr = -1;
		t_info.stack_addr = -1;
		t_info.isTFirst = true;
		T_Map[t_var] = t_info;
		return t_var;
	}

	//sy to types
	types sy_to_types(symbol sym){
		switch(sym){
			case intsy:
				return ints;
			case charsy:
				return chars;
			default:
				return notyp;
		}
	}

	//<常量说明> <常量定义> 处理
	void constDeal(){
		//std::cout<<"This is a const statement"<<std::endl;
		gram_out_file<<"This is a const statement"<<std::endl;
		types tmp_typ;
		insymbol();
		if ((sy != intsy) && (sy != charsy)) {
			error(26,lc);
			skip(block_start_sys, 4);
			return;
		}
		tmp_typ = sy_to_types(sy);


		do {
			insymbol();
			if (sy != ident) {
				error(11,lc);
				skip(block_start_sys, 4);
				return;
			}
			enter(id, constant, tmp_typ);//将常量登记入表
			insymbol();

			if (sy != assign) {
				error(20,lc);
				skip(block_start_sys, 4);
				return;
			}

			insymbol();	//读到的是等号后面的单词

			if ((sy == plus) || (sy == minus) || (sy == intcon)) {
				//进入整数分支
				if (tmp_typ != ints) {
					error(27,lc);
					skip(block_start_sys, 4);
					return;
				}
				int flag = 1;
				if ((sy == plus) || (sy == minus)) {
					if (sy == minus) flag = -1;
					insymbol();
				}
				if (sy != intcon) {
					error(27, lc);
					skip(block_start_sys, 4);
					return;
				}
				inum = inum * flag;
				enterVar(tab.t - 1, inum);//将常量的值登录进去
			}
			else if (sy == charcon) {
				if (tmp_typ != chars) {
					error(27,lc);
					skip(block_start_sys, 4);
					return;
				}
				enterVar(tab.t - 1, inum);
			}
			else {
				error(27, lc);
				skip(block_start_sys, 4);
				return;
			}

			insymbol();
		} while (sy == comma);
		if (sy != semicolon) {
			error(19, lc-1);
			return;
		}
		insymbol();//读取分号后面一个字符
	}

	//<变量说明> <变量定义> 处理   参数n为该行第一个标识符在数据栈的相对地址
	void varDeal(types typ, int &n){
		//std::cout << "This is a var_declare statement" << std::endl;
		gram_out_file << "This is var_declare statement" << std::endl;
		//进入时id为 int/char 后面第一个标识符，sy == comma || sy == semicolon || sy = lsbrack
		if (sy == lsbrack) {
			enter(id, array, typ);
			insymbol();
			if ((sy == intcon) && (inum != 0)) {
				enterVar(tab.t - 1, n);	//把数组的第一个元素相对地址存放起来
				n += inum;	//inum个空间都是该数组的
				enterArray(tab.t - 1, typ, inum);
				insymbol();
				if (sy != rsbrack) {
					error(16, lc);
				}
				else {
					insymbol();
				}
			}
			else {
				error(30, lc);
				skip(block_start_sys,4);
				return;
			}
			if ((sy != comma) && (sy != semicolon)) {
				error(28, lc);
				skip(block_start_sys, 4);
				return;
			}
		}
		else {
			enter(id, variable, typ);
			enterVar(tab.t - 1, n++);
		}
		//截止到这里 sy == comma || sy == semicolon
		while (sy == comma) {
			insymbol();
			if (sy != ident) {	//非标识符,跳读完一行处理下一行
				error(11,lc);
				skip(block_start_sys, 4);
				return;
			}
			insymbol();
			if ((sy != lsbrack) && (sy != comma) && (sy != semicolon)) {
				error(28, lc);
				skip(block_start_sys, 4);
				return;
			}
			if (sy == lsbrack) {	//该if结束后sy=comma or sy = semicolon
				enter(id, array, typ);
				insymbol();
				if ((sy == intcon) && (inum != 0)) {
					enterVar(tab.t - 1, n);	//把数组的第一个元素相对地址存放起来
					n += inum;	//inum个空间都是该数组的
					enterArray(tab.t - 1, typ, inum);
					insymbol();
					if (sy != rsbrack) {
						error(16, lc);
					}
					else {
						insymbol();
					}
				}
				else {
					error(30, lc);
					skip(block_start_sys, 4);
					return;
				}
				if ((sy != comma) && (sy != semicolon)) {
					error(28, lc);
					skip(block_start_sys, 4);
					return;
				}
			}
			else {
				enter(id, variable, typ);
				enterVar(tab.t - 1, n++);
			}
		}
		if (sy != semicolon) {
			error(19,lc);
			return;
		}
		insymbol();//读取分号之后第一个单词
	}

	//<因子>
	//返回值为操作数
	std::string factor(bool& is_char) {
		//进因子时,sy为 factor_start_sys[]中的元素
		std::string z, y, x;
		bool char_tmp = false;
		int pos, tmp;	//tmp 在minus中使用
		symbol tmp_sys[] = { rparent };
		switch (sy) {
		case ident:	//标识符、标识符[表达式]、有返回值函数调用语句
			pos = find_in_tab(id);	//在tab表中查询当前单词是啥
			if (pos == 0) error(23, lc);
			
			if (pos != 0) {
				if (tab.tabArray[pos].type == chars) is_char = true;
				switch (tab.tabArray[pos].obj) {
				case constant:	//常量直接返回数
					return int_to_string(tab.tabArray[pos].adr);
				case variable:	//变量返回变量名
					return id;
				case array:		//标识符[表达式]
					y = id;
					z = creat_tmp_var();
					insymbol();
					if (sy != lsbrack) {
						error(15, lc);
						skip(factor_end_sys, 5);
					}
					insymbol();
					skip(expression_start_sys, 6);
					x = expression(char_tmp);	//处理中括号的表达式,出来时sy为follow_expression_sys[]中的元素
					if (char_tmp) {	//tmp为表示为字符类型
						error(49, lc);
					}
					else {
						if (string_is_num(x)) {
							if (string_to_int(x) >= atab.atabArray[tab.tabArray[pos].ref].high) {
								error(49, lc);
							}
						}
					}
					if (sy != rsbrack) {
						error(16, lc);
						skip(factor_end_sys, 5);
					}

					enterMidCode(4, z, x, y, 0);	//z = y[x]
													//此时sy为rsbrack
					return z;
				case function:
					if (tab.tabArray[pos].type == notyp) {	//无返回值函数调用
						error(34, lc);
						skip(tmp_sys,1);
						return "0";
					}
					if (tab.tabArray[pos].type == chars) is_char = true;
					else is_char = false;
					//进入reUseFuncStatement时,sy == identsy(函数名)
					z = reUseFuncStatement();

					//此时 sy为 rparent
					return z;	//返回有返回值函数的调用最后的赋值结果操作数, 即z = RET
				default:
					std::cout << "There is a bug in blockdeal--factor, switch-ident" << std::endl;
					return "0";
				}
			}
			else {	//标识符未定义
				return "0";
			}
			//程序不可能运行到这里
			std::cout << "There is a bug in blockdeal-factor, switch-ident" << std::endl;
		case plus:	// 整数: +
			insymbol();
			if ((sy == intcon) && (intcon != 0)) {
				return int_to_string(inum);
			}
			else {
				error(33, lc);
				skip(factor_end_sys, 5);
				return "0";
			}
			//程序不可能运行到这里
			std::cout << "There is a bug in blockdeal-factor, switch-plus" << std::endl;
		case minus:	// 整数: -
			tmp = -1;
			insymbol();
			if ((sy == intcon) && (intcon != 0)) {
				inum = inum * tmp;
				return int_to_string(inum);
			}
			else {
				error(33, lc);
				skip(factor_end_sys, 5);
				return "0";
			}
			//程序不可能运行到这里
			std::cout << "There is a bug in blockdeal-factor, switch-minus" << std::endl;
		case intcon: // 整数: 0 或者无符号整数
			is_char = false;
			return int_to_string(inum);
		case charcon:	//字符
			is_char = true;
			return int_to_string(inum);
		case lparent:
			insymbol();
			if (!find_sy(expression_start_sys, 6)) {
				error(33, lc);
				skip(expression_start_sys, 6);
			}
			return expression(is_char);
		default:
			std::cout << "There is a bug in blockdeal--factor, switch-default" << std::endl;
			break;
		}

		//程序不可能运行到这里
		std::cout << "There is a bug in blockdeal-factor, end" << std::endl;
		return " ";
		//出因子时,sy为 factor_end_sys[]中的元素
	}


	//<项>
	//返回值为项处理得到的最终的z操作数
	std::string item(int minus_flag, bool& is_char){
		//进入时sy为factor_start_sys[]中的元素
		std::string z, x, y;
		x = factor(is_char);
		if (minus_flag == 1) {		//表达式的第一个项才会遇到
			z = creat_tmp_var();//产生临时变量
			y = x;
			x = "0";
			enterMidCode(1, z, x, y, 0);
			x = z;
		}
		//离开factor时, sy为factor_end_sys[]中的一个
		insymbol();
		while ((sy == mult) || (sy == rdiv)) {
			is_char = false;		// 有乘法运算符,肯定不是字符类型了
			int op;
			bool no_use;
			if (sy == mult) op = 2;
			else if (sy == rdiv) op = 3;
			else std::cout << "There is a bug in blockdeal--item, while" << std::endl;
			insymbol();
			skip(factor_start_sys, 6);
			y = factor(no_use);
			z = creat_tmp_var();//产生临时变量
			if ((op == 3) && string_is_num(y) && (string_to_int(y) == 0)) {
				error(44, lc);
			}
			enterMidCode(op, z, x, y, 0);
			x = z;

			insymbol();
		}
		return x;
		//结束时 sy 为项后面一个单词
	}

	
	//<表达式>			
	//返回值为最终的一个四元式表达式的z值, 传参: 表达式处理完是否为char类型
	std::string expression(bool& is_char){
		//进入表达式时sy为expression_start_sys[]中的元素

		//std::cout << "This is an expression statement" << std::endl;
		gram_out_file << "This is an expression statement" << std::endl;
		
		int minus_flag = 0;
		std::string z, x, y;
		if ((sy == plus) || (sy == minus)) {
			if (sy == minus) {
				minus_flag = 1;
			}
			insymbol();
		}

		skip(factor_start_sys, 6);
		x = item(minus_flag, is_char);//进入项处理

		//出来时sy为 项后面的第一个单词
		symbol tmp_sys[] = { semicolon, rparent, rsbrack ,plus, minus, lss, leq, gtr, geq, neq, eql, comma};	//此时 sy应该为 follow[expression] or 加法运算符
		if (!find_sy(tmp_sys, 12)) {
			error(33, lc);
			//std::cout<<"1"<<std::endl;
			skip(tmp_sys, 11);
		}
		while ((sy == plus) || (sy == minus)) {
			is_char = false;	// 有加法运算符则肯定不是字符型
			int op;
			bool no_use;
			if (sy == plus) op = 0;
			else if (sy == minus) op = 1;
			else std::cout << "There is a bug in blockdeal--expression, while" << std::endl;
			insymbol();
			y = item(0,no_use);
			//sy为项后面的一个单词,所以不用insymbol()
			z = creat_tmp_var();//产生临时变量

			enterMidCode(op, z, x, y, 0);
			x = z;
		}
		if (!find_sy(follow_expression_sys, 10)) {
			error(33,lc);
			skip(follow_expression_sys, 10);
		}
		
		
		return x;
		//出expression时,sy为follow_expression_sys[]中的元素
	}


	//<参数表>
	// 传入值为参数的个数引用、pv_addr为目前变量处于函数数据区的相对地址,返回值为最后一个参数在tab表的登录位置
	int paraDeal(int& para_num, int& pv_addr) {
		//进入时sy == intsy || sy == charsy
		if ((sy != intsy) && (sy != charsy)) {
			std::cout << "There is a bug in blockdeal--paraDeal, sy must be intsy or charsy" << std::endl;
		}
		//std::cout << "This is a paraDeal statement" << std::endl;
		gram_out_file << "This is a paraDeal statement" << std::endl;
		types tmp_typ = sy_to_types(sy);
		insymbol();
		if (sy != ident) {
			error(11, lc);
		}
		else {
			//sy == ident
			enter(id, variable, tmp_typ);
			enterVar(tab.t-1,pv_addr++);
			para_num++;
			enterMidCode(15," ",types_info[tmp_typ],id,0);
			insymbol();
		}
		symbol tmp_sys[] = { comma, rparent };
		skip(tmp_sys, 2);
		while (sy == comma) {
			insymbol();
			if ((sy != intsy) && (sy != charsy)) {
				error(31,lc);
				skip(tmp_sys, 2);
				continue;
			}
			tmp_typ = sy_to_types(sy);
			insymbol();
			if (sy != ident) {	//如果不是标识符则跳读
				error(31, lc);
				skip(tmp_sys, 2);
				continue;
			}
			//到达此处sy == ident
			enter(id, variable, tmp_typ);
			enterVar(tab.t - 1, pv_addr++);
			para_num++;
			enterMidCode(15, " ", types_info[tmp_typ], id, 0);
			insymbol();
			if ((sy != comma) && (sy != rparent)) {
				error(31, lc);
				skip(tmp_sys, 2);
			}
		}

		//到达此处的条件是sy == rparent
		if (sy != rparent) {
			std::cout << "There is a bug in blockdeal--paraDeal, sy must be rparent" << std::endl;
		}
		insymbol();
		//出去的时候sy为')'后面一个单词
		return tab.t - 1;
	}


	//<条件>
	//返回该跳转指令所在的中间代码中的位置,以便后续回填跳转的标签
	int conditionDeal(int kind){			// kind = 0, if 条件的调用; kind = 1, while条件的调用, 两者跳转的条件不同
		// 进入时, sy为左括号后的第一个单词
		//std::cout << "This is a condition statement" << std::endl;
		gram_out_file << "This is a condition statement" << std::endl;
		std::string z, x, y;
		int op, op_flag = 0;	//op_flag = 1表示需要在跳转之前进行x-y操作，进而比较x-y与0的大小关系
		bool no_use;
		symbol relation_op_sys[] = { lss, leq, gtr, geq, neq, eql };	//6个

		if (!find_sy(expression_start_sys, 6)) {
			error(33, lc);
			skip(expression_start_sys, 6);
		}

		x = expression(no_use);	//进入表达式处理
		//出expression时,sy为follow_expression_sys[]中的元素

		if (find_sy(relation_op_sys, 6)) {
			if (kind == 0) {
				//有关系运算符,注意op的取值, if是不符合条件才跳转
				switch (sy) {
				case lss://<  意味着x >= y的时候不执行if内的语句,进行跳转
					op = 8;
					break;
				case leq://<=  x > y
					op = 9;
					break;
				case gtr://>  x<=y
					op = 10;
					break;
				case geq://>=  x<y
					op = 11;
					break;
				case neq://!=  x==y 跳转
					op = 6;
					break;
				case eql://==  x!=y
					op = 7;
					break;
				default:
					std::cout << "There is a bug in blockdeal--conditionDeal, switch-default" << std::endl;
					break;
				}
			}
			else {				//while 符合条件则跳转
				switch (sy) {
				case lss://<  
					op = 11;
					break;
				case leq://<=  
					op = 10;
					break;
				case gtr://>  
					op = 9;
					break;
				case geq://>=  
					op = 8;
					break;
				case neq://!=  
					op = 7;
					break;
				case eql://==  
					op = 6;
					break;
				default:
					std::cout << "There is a bug in blockdeal--conditionDeal, switch-default" << std::endl;
					break;
				}
			}
			insymbol();
			skip(expression_start_sys, 6);
			y = expression(no_use);	//进入表达式处理
		}
		else {
			if(kind == 0) op = 10; //单独一个表达式, if, 则在 x <= 0的时候跳转
			else op = 9;
			y = "0";
		}
		if (op_flag = 1) {
			//z = creat_tmp_var();
			//enterMidCode(1, z, x, y, 0);					//执行 z = x-y操作
			enterMidCode(op, " ", x, y, 0);
		}
		else {		// == 或 !=
			enterMidCode(op, " ", x, y, 0);
		}
		
		//此时sy属于follow_expression_sys[]中的元素
		if (sy != rparent) {
			error(50, lc);
		}
		return MidCodeT.mdc - 1; //返回跳转指令所在的位置,便于之后回填
		// 出去时, sy == rparent
	}

	//<条件语句>
	void ifStatement(){
		//进入时 sy == ifsy
		//std::cout<<"This is a if statement"<<std::endl;
		gram_out_file<< "This is a if statement" << std::endl;
		int code_pos = 0;
		std::string j_label;

		insymbol();
		if (sy != lparent) {
			error(13, lc);
			skip(statement_start_sys, 9);
			return;
		}
		insymbol();
		if (sy == rparent) {
			//说明没有条件语句
			error(21, lc);
		}
		else {
			code_pos = conditionDeal(0);
			//出来时 sy == rparent
		}

		if (sy != rparent) std::cout << "There is a bug in blockdeal--ifStatement, sy must be rparent" << std::endl;
		
		insymbol();
		
		if (!find_sy(statement_start_sys, 9)) {
			error(32, lc);
			skip(statement_start_sys, 9);
		}

		statement();

		j_label = creat_label(" ", 2, MidCodeT.mdc);
		enterMidCode(13, " ", " ",j_label, 1);//设置标签
		MidCodeT.midcodeArray[code_pos].z = j_label;
		//出去时 sy 为 statement_end_sys中的元素
	}

	//<循环语句>
	void do_whileStatement(){
		//进入时 sy == dosy
		//std::cout << "This is a do_while statement" << std::endl;
		gram_out_file << "This is do_while statement" << std::endl;
		std::string j_label;
		int j_pos; //跳转指令的位置
		insymbol();

		if (!find_sy(statement_start_sys, 9)) {
			error(32, lc);
			skip(statement_start_sys, 9);
		}

		//do-while 指令条件在后面, 跳到前面
		j_label = creat_label(" ", 2, MidCodeT.mdc);
		enterMidCode(13, " ", " ", j_label, 1);

		statement();

		insymbol();
		if (sy != whilesy) {
			error(35, lc);
			symbol tmp_sys[] = { rparent };
			skip(tmp_sys, 1);
			return;
		}
		insymbol();
		if (sy != lparent) {
			error(13,lc);
			symbol tmp_sys[] = { rparent };
			skip(tmp_sys, 1);
			return;
		}
		insymbol();
		if (!find_sy(expression_start_sys, 6)) {
			symbol tmp_sys[] = { rparent };
			skip(tmp_sys, 1);
			error(21, lc - 1);
			return;
		}
		//进入条件处理语句
		j_pos = conditionDeal(1);
		MidCodeT.midcodeArray[j_pos].z = j_label;
		if (sy != rparent) std::cout << "There is a bug in blockdeal--do_while, sy must be rparent" << std::endl;
		//出去时 sy == rparent
	}

	//<有返回值函数调用语句>
	//涉及的四元式指令  call, push, jal,  set call_label,  z = RET, 
	//返回值为 z = RET 的z
	std::string reUseFuncStatement(){
		//进入时 sy == ident(函数名)
		//std::cout << "This is a reUseFuncStatement" << std::endl;
		gram_out_file << "This is a reUseFuncStatement" << std::endl;
		int id_pos = find_in_tab(id);	//id_pos为该函数名在tab表中的位置
		if(id_pos == 0) error(23, lc);
		int paranum = btab.btabArray[tab.tabArray[id_pos].ref].paranum;
		int paracount = 0;
		std::string para, z;
		std::string func_name = id ;
		symbol tmp_sys[] = { rparent };
		
		std::map <int, types> para_typ_map;
		tabe tmp = tab.tabArray[btab.btabArray[tab.tabArray[id_pos].ref].lastpar];
		for (int i = paranum - 1; i > -1; i--) {
			para_typ_map[i] = tmp.type;
			tmp = tab.tabArray[tmp.link];
		}

		enterMidCode(19," ", " ", func_name,0);	//call指令调用
		insymbol();

		if (sy != lparent) {
			error(13, lc);
			skip(tmp_sys, 1);
			return " ";
		}
		insymbol();
		if (sy == rparent) { //无参数调用的情况
			if (paracount != paranum) {
				error(36, lc);
				return " ";
			}
		}
		else if(find_sy(expression_start_sys, 6)){
			bool is_char = false;
			para = expression(is_char);
			if (is_char) {
				if (para_typ_map[paracount] != chars) {
					error(45, lc);
				}
			}
			else {
				if (para_typ_map[paracount] != ints) {
					error(45, lc);
				}
			}

			enterMidCode(18, " ", " ", para, 0);	//push指令
			paracount++;
			while (sy == comma) {
				insymbol();
				if (!find_sy(expression_start_sys, 6)) {
					error(33,lc);
					skip(tmp_sys, 1);
					return " ";
				}
				para = expression(is_char);
				if (is_char) {
					if (para_typ_map[paracount] != chars) {
						error(45, lc);
					}
				}
				else {
					if (para_typ_map[paracount] != ints) {
						error(45, lc);
					}
				}
				enterMidCode(18, " ", " ", para, 0);	//push指令
				paracount++;		
			}
			if (paracount != paranum) {
				error(36, lc);
				skip(tmp_sys, 1);
				return " ";
			}
			if (sy != rparent) {
				error(14, lc);
				skip(tmp_sys, 1);
				return " ";
			}
		}
		else {
			skip(tmp_sys, 1);
			return " ";
		}
		
		if (sy != rparent) std::cout << "There is a bug in blockdeal--reUseFuncStatement, sy must be rparent" << std::endl;
		enterMidCode(21,func_name + "_begin_"," "," ", 0);		//jal语句
		z = creat_tmp_var();
		
		enterMidCode(13," "," ",creat_label(" ",4,MidCodeT.mdc),3);

		//有返回值调用函数才有
		enterMidCode(4,z, " ", "RET",0);				//z = RET
		//出去时sy == rparent
		return z;
	}

	//<无返回值函数调用语句>
	//涉及的四元式指令  call, push, jal, set call_label
	void voUseFuncStatement(){
		//进入时 sy == ident(函数名)
		//std::cout << "This is a voUseFuncStatement" << std::endl;
		gram_out_file << "This is a voUseFuncStatement" << std::endl;
		int id_pos = find_in_tab(id);	//id_pos为该函数名在tab表中的位置
		if (id_pos == 0) error(23, lc);
		int paranum = btab.btabArray[tab.tabArray[id_pos].ref].paranum;
		int paracount = 0;
		std::string para, z;
		std::string func_name = id;
		symbol tmp_sys[] = { rparent };

		std::map <int, types> para_typ_map;
		tabe tmp = tab.tabArray[btab.btabArray[tab.tabArray[id_pos].ref].lastpar];
		for (int i = paranum - 1; i > -1; i--) {
			para_typ_map[i] = tmp.type;
			tmp = tab.tabArray[tmp.link];
		}

		enterMidCode(19, " ", " ", func_name, 0);	//call指令调用
		insymbol();

		if (sy != lparent) {
			error(13, lc);
			skip(tmp_sys, 1);
			return;
		}
		insymbol();
		if (sy == rparent) { //无参数调用的情况
			if (paracount != paranum) {
				error(36, lc);
				return;
			}
		}
		else if (find_sy(expression_start_sys, 6)) {
			bool is_char = false;
			para = expression(is_char);
			//std::cout << is_char << std::endl;
			//std::cout << types_info[para_typ_map[paracount]] << std::endl;
			if (is_char) {
				if (para_typ_map[paracount] != chars) {
					error(45, lc);
				}
			}
			else {
				if (para_typ_map[paracount] != ints) {
					error(45, lc);
				}
			}
			enterMidCode(18, " ", " ", para, 0);	//push指令
			paracount++;
			while (sy == comma) {
				insymbol();
				if (!find_sy(expression_start_sys, 6)) {
					error(33, lc);
					skip(tmp_sys, 1);
					return;
				}
				para = expression(is_char);
				if (is_char) {
					if (para_typ_map[paracount] != chars) {
						error(45, lc);
					}
				}
				else {
					if (para_typ_map[paracount] != ints) {
						error(45, lc);
					}
				}
				enterMidCode(18, " ", " ", para, 0);	//push指令
				paracount++;
			}
			if (paracount != paranum) {
				error(36, lc);
				skip(tmp_sys, 1);
				return;
			}
			if (sy != rparent) {
				error(14, lc);
				skip(tmp_sys, 1);
				return;
			}
		}
		else {
			skip(tmp_sys, 1);
			return ;
		}

		if (sy != rparent) std::cout << "There is a bug in blockdeal--reUseFuncStatement, sy must be rparent" << std::endl;
		enterMidCode(21, func_name + "_begin_", " ", " ", 0);		//jal语句

		enterMidCode(13, " ", " ", creat_label(" ", 4, MidCodeT.mdc), 3);
		//出去时sy == rparent
	}

	//<赋值语句>
	//array_flag = 1, 表示当前ident代表数组; 0代表为变量
	void assignStatement(int array_flag){
		//进入时sy == ident
		//std::cout << "This is a assign statement" << std::endl;
		gram_out_file << "This is a assign statement" << std::endl;
		std::string z, y, x = " ";
		symbol tmp_sys[] = { semicolon };
		int op;
		bool is_char = false;
		int z_pos;		//z在符号表中的位置
		z = id;
		z_pos = find_in_tab(z);
		if (z_pos == 0) error(23, lc);
		if (array_flag == 1) {
			op = 20;
			insymbol();
			if (sy != lsbrack) {
				error(15, lc);
				skip(tmp_sys, 1);
				return;
			}
			insymbol();
			skip(expression_start_sys, 6);
			bool no_use = false;
			x = expression(no_use);	//处理中括号的表达式,出来时sy为follow_expression_sys[]中的元素
			if (no_use) {	//tmp为表示为字符类型
				//std::cout << "1" << std::endl;
				error(49, lc);
			}
			else {
				if (string_is_num(x)) {
					if (string_to_int(x) >= atab.atabArray[tab.tabArray[z_pos].ref].high) {
						//std::cout << "2" << std::endl;
						error(49, lc);
					}
				}
			}
			if (sy != rsbrack) {
				error(16, lc);
				skip(tmp_sys, 1);
				return;
			}
		}
		else {
			op = 4;
			
		}

		insymbol();
		if (sy != assign) {
			error(38, lc);
			skip(tmp_sys, 1);
			return;
		}
		insymbol();
		skip(expression_start_sys, 6);
		y = expression(is_char);

		if (tab.tabArray[z_pos].type == chars) {
			if (!is_char) {
				error(46,lc - 1);
			}
		}
		enterMidCode(op, z, x, y, 0);

		if (sy != semicolon) {
			error(19, lc);
			skip(tmp_sys, 1);
		}
		
		//结束时 sy = semicolon
	}

	//<读语句>
	void scanfStatement(){
		//进入时 sy == scanfsy
		//std::cout << "This is a scanf statement" << std::endl;
		gram_out_file << "This is scanf statement" << std::endl;
		symbol tmp_sys[] = { semicolon };
		insymbol();
		if (sy != lparent) {
			error(13, lc);
			skip(tmp_sys, 1);
			return;
		}
		do {
			insymbol();
			if (sy != ident) {
				error(11, lc);
				skip(tmp_sys, 1);
				return;
			}
			int t = find_in_tab(id);
			if(t == 0) error(23, lc);
			enterMidCode(16," ", " ",id,0);
			insymbol();
			if ((sy != comma) && (sy != rparent)) {
				error(39,lc);
				skip(tmp_sys, 1);
				return;
			}
		} while (sy == comma);
		if (sy != rparent)	std::cout << "There is a bug in blockdeal--scanfStatement" << std::endl;
		insymbol();
		if (sy != semicolon) {
			error(19, lc);
			skip(tmp_sys, 1);
		}
		//出来时 sy == semicolon
	}

	//<写语句>
	void printfStatement(){
		//进入时 sy == printfsy
		//std::cout << "This is a printf statement" << std::endl;
		gram_out_file << "This is printf statement" << std::endl;
		std::string z = " ", x = " ", y = " ";
		symbol tmp_sys[] = { semicolon };
		bool is_char = false;
		insymbol();
		if (sy != lparent) {
			error(13, lc);
			skip(tmp_sys, 1);
			return;
		}
		insymbol();
		if (sy == stringcon) {
			char stemp[SMAX];
			for (int i = 0; i < sleng; i++) {
				stemp[i] = stab.stringArray[inum + i];
			}
			stemp[sleng] = '\0';
			x = stemp;
			insymbol();
			if (sy == comma) {
				insymbol();
				if (find_sy(expression_start_sys, 6)) {
					y = expression(is_char);
					if (is_char) z = "char";
					if (sy != rparent) {
						error(14, lc);
						skip(tmp_sys, 1);
						return;
					}
					enterMidCode(17, z, x, y, 0);
					insymbol(); //读后面的分号
					if (sy != semicolon) {
						error(19, lc);
						skip(tmp_sys, 1);
						return;
					}
				}
				else {
					error(40, lc);
					skip(tmp_sys, 1);
					return;
				}
			}
			else {
				if (sy != rparent) {
					error(14, lc);
				}
				else {
					insymbol();
				}
				if (sy != semicolon) {
					error(19, lc);
				}
				skip(tmp_sys, 1);
				enterMidCode(17, " ", x, " ", 0);
			}
		}
		else if (find_sy(expression_start_sys, 6)) {
			y = expression(is_char);
			if (is_char) z = "char";
			if (sy != rparent) {
				error(14, lc);
				skip(tmp_sys, 1);
				return;
			}
			enterMidCode(17, z," ",y,0);
			insymbol();
			if (sy != semicolon) {
				error(19, lc);
				skip(tmp_sys, 1);
				return;
			}
		}
		else {
			error(40, lc);
			skip(tmp_sys, 1);
			return;
		}
		//出去时 sy == semicolon
	}


	//<情况表>
	//参数x为switch中的表达式,switch_end_label为switch结束的位置标签,用于每一个case后面的break
	void caseTableDeal(std::string x, std::string switch_end_label, bool is_char){
		//进入时sy == lcbrack
		//std::cout << "This is case-table statement" << std::endl;
		gram_out_file << "This is case-table statement" << std::endl;
		std::string z, y, j_label;
		int pos = 0;
		symbol tmp_sys[] = { rcbrack };

		insymbol();
		if (sy != casesy) {
			error(43, lc);
			skip(tmp_sys, 1);
			return;
		}

		do {
			//std::cout << "This is a case statement" << std::endl;
			gram_out_file << "This is a case statement" << std::endl;
			insymbol();
			switch (sy) {
			case plus:
				if (is_char) error(47, lc);
				insymbol();
				if ((sy == intcon) && (sy != 0)) {
					y = int_to_string(inum);
				}
				else {
					error(22, lc);
					skip(tmp_sys, 1);
					return;
				}
				break;
			case minus:
				if (is_char) error(47, lc);
				insymbol();
				if ((sy == intcon) && (sy != 0)) {
					inum = inum * -1;
					y = int_to_string(inum);
				}
				else {
					error(22, lc);
					skip(tmp_sys, 1);
					return;
				}
				break;
			case intcon:
				if (is_char) error(47, lc);
				y = int_to_string(inum);
				break;
			case charcon:
				if (!is_char) error(47, lc);
				y = int_to_string(inum);
				break;
			default:
				error(22,lc);
				skip(tmp_sys, 1);
				return;
			}
			insymbol();
			if (sy != colon) {
				error(43, lc);
				skip(tmp_sys, 1);
				return;
			}
			
			pos = MidCodeT.mdc;
			enterMidCode(7, " ", x, y, 0);	//等于的时候继续执行,不等于的时候跳转,所以此处的跳转指令应该为不等号

			insymbol();
			if (!find_sy(statement_start_sys, 9)) {
				error(32, lc);
				skip(statement_start_sys, 9);
			}
			statement();
			
			//case语句处理完,跳转到switch结束位置
			enterMidCode(5, switch_end_label, " ", " ", 0);
			j_label = creat_label(" ", 2, MidCodeT.mdc);
			enterMidCode(13, " ", " ", j_label, 1);
			//跳转指令回填,跳到case结束的位置,即goto语句的后面
			MidCodeT.midcodeArray[pos].z = j_label;		//此处的j_label 为pc-1,因为在前面又填了一个goto语句

			//此处sy为statement_end_sys中的一个
			insymbol();
		} while (sy == casesy);

		if (sy != rcbrack) {
			error(43,lc);
			skip(tmp_sys, 1);
		}
		//结束时sy == rcbrack
	}

	//<情况语句>
	void switchStatement() {
		//进入时 sy == switchsy
		//std::cout << "This is a switch statement" << std::endl;
		gram_out_file << "This is switch statement" << std::endl;
		std::string z, x, y;	//x作为switch()括号里的操作数,y代表各个case的操作数
		std::string switch_end_label = creat_label(" ",3,MidCodeT.mdc);
		symbol tmp_sys[] = { rcbrack };
		bool is_char = false;
		insymbol();
		if (sy != lparent) {
			error(13, lc);
			skip(tmp_sys, 1);
			return;
		}
		insymbol();
		if (!find_sy(expression_start_sys, 6)) {
			error(42, lc);
			skip(tmp_sys, 1);
			return;
		}
		x = expression(is_char);
		//出expression时 sy为follow(expression)里面的东西
		if (sy != rparent) {
			error(14, lc);
			skip(tmp_sys, 1);
			return;
		}
		insymbol();
		if (sy != lcbrack) {
			error(17, lc);
			skip(tmp_sys, 1);
			return;
		}
		caseTableDeal(x, switch_end_label, is_char);
		//出caseTableDeal时sy == rcbrack
		
		enterMidCode(13, " ", " ", switch_end_label, 1);
		//出来时 sy 为rcbrack
	}


	//<返回语句>
	void returnStatement(){
		//进入时 sy == returnsy
		//std::cout << "This is a return statement" << std::endl;
		gram_out_file << "This is return statement" << std::endl;
		symbol tmp_sys[] = { semicolon };
		std::string y;

		bool is_char = false;
		insymbol();
		
		if (sy == semicolon) {	//单独一个return
			return_flag = 1;
			enterMidCode(12," ", " "," ", 0);
			
			return;
		}
		else if(sy == lparent){
			insymbol();
			if (!find_sy(expression_start_sys, 6)) {
				error(33, lc);
				skip(tmp_sys, 1);
				return;
			}
			y = expression(is_char);
			//出expression时sy为follow_expression_sys中的元素
			if(is_char)return_flag = 3;	//返回值为chars
			else return_flag = 2;	//返回值为ints
			enterMidCode(12, " ", " ", y, 0);
			
			if (sy != rparent) {
				error(41, lc);
				skip(tmp_sys, 1);
				return;
			}
			insymbol();
			if (sy != semicolon) {
				error(19,lc);
				skip(tmp_sys, 1);
				return;
			}
		}
		else {
			error(41, lc);
			skip(tmp_sys, 1);
			return;
		}
		//出去时 sy == semicolon
	}

	//<语句>
	void statement() {
		//进入statement时, sy为statement_start_sys中的元素
		// 调用地方: statement(),statementCompond, ifStatement, do_whileStatement, 
		int pos;
		std::string z, x, y;
		switch (sy) {
		case semicolon:	//单独一个分号处理
			//std::cout << "This is a single ; in statement" << std::endl;
			break;
		case ifsy:	//条件语句处理
			//进入时sy == if
			ifStatement();
			break;//此时 sy 为statement_end_sys中的元素
		case dosy:	//循环语句
			//进入时sy == dosy
			do_whileStatement();
			break;//此时sy == rparent
		case lcbrack:	//{ 语句 } 处理
			insymbol();
			while (sy != rcbrack) {
				skip(statement_start_sys, 9);
				//进入statement时, sy为statement_start_sys中的元素
				statement();	//进入语句处理函数
				//出statement时, sy 为 statement_end_sys中的元素
				insymbol();	//读语句后面的一个单词
			}
			break;	//此时sy == rcbrack
		case ident:		//有返回值函数调用、无返回值函数调用、赋值语句处理
			pos = find_in_tab(id);	//在tab表中查询当前单词是啥
			if(pos == 0) error(23, lc);
			if (pos != 0) {
				switch (tab.tabArray[pos].obj) {
				case constant:	//赋值语句
					error(37, lc);
					skip(statement_end_sys, 3);
					break;
				case variable:	//赋值语句
					assignStatement(0);
					//此时sy == semicolon
					break;
				case array:		//标识符[表达式]  赋值语句
					assignStatement(1);
					//此时sy == semicolon
					break;
				case function:
					if (tab.tabArray[pos].type == notyp) {	//无返回值函数调用
						//进入voUseFuncStatement时,sy == identsy(函数名)
						voUseFuncStatement();
					}
					else {
						//进入reUseFuncStatement时,sy == identsy(函数名)
						reUseFuncStatement();
					}
					
					//此时 sy为 rparent
					insymbol();
					if (sy != semicolon) error(19, lc);
					//sy == semicolon
					break;
				default:
					std::cout << "There is a bug in blockdeal--statement, ident-switch-obj" << std::endl;
					break;
				}
			}
			else {
				skip(statement_end_sys, 3);
			}
			break;	//此时 sy == semicolon
		case scanfsy:	//读语句处理
			//进入时sy == scanfsy
			scanfStatement();
			break;	//此时 sy == semicolon
		case printfsy:	//写语句处理
			//进入时sy == printfsy
			printfStatement();
			break;//此时sy == semicolon
		case switchsy:	//情况语句处理
			//进入时 sy == switchsy
			switchStatement();
			if (sy != rcbrack) std::cout << "There is a bug in blockdeal--statement, after switchStatement()" << std::endl;
			break; //此时 sy == rcbrack;
		case returnsy:	//返回语句处理
			//进入时 sy == returnsy
			returnStatement();
			break;//此时 sy == semicolon
		default:
			error(32, lc);
			skip(statement_end_sys, 3);
			break;
		}
		if (!find_sy(statement_end_sys, 3)) std::cout << "There is a bug in blockdeal--statement, at the end of the function" << std::endl;

		//出statement时, sy为 statement_end_sys中的一个
	}

	//<复合语句>
	//传入值为函数局部变量所占空间的大小、返回值类型的、目前变量所处函数数据区相对地址,返回值为该函数最后一个局部变量在tab中的位置
	int statementCompond(int& var_size, types& return_typ, int& pv_addr){
		//入口时sy指的是左括号之后的第一个单词
		symbol statementCompond_start_sys[] = { constsy, intsy, charsy, semicolon, ifsy, dosy, lcbrack, ident, scanfsy, printfsy, switchsy, returnsy, rcbrack};//13个
		types tmp_type;
		int start_pv_addr = pv_addr;
		int var_flag = 0;	//是否有局部变量的标志位,用于后面的return返回

		if (!find_sy(statementCompond_start_sys, 13)) {
			error(32, lc);
			skip(statementCompond_start_sys, 13);
		}
		
		//局部常量声明
		while (sy == constsy) {
			constDeal();	//从constDeal里面出来时sy为';'后面的单词
		}
		skip(statementCompond_start_sys, 13);	//跳读, 此时sy不可能为constsy,否则出不了循环
		


		//局部变量声明
		while ((sy == intsy) || (sy == charsy)) {

			tmp_type = sy_to_types(sy);
			insymbol();
			if (sy != ident) {
				error(11, lc);
				skip(statementCompond_start_sys, 13);
				//std::cout << "This is a var_declare statement" << std::endl;
				gram_out_file << "This is var_declare statement" << std::endl;
				continue;
			}

			insymbol();

			//test sy 是否为 comma, semicolon, lsbrack 都不是的话报错
			symbol tmpsys[] = { comma, semicolon, lsbrack };
			if (!find_sy(tmpsys, 3)) {
				error(28, lc);
				skip(statementCompond_start_sys, 13);
				//std::cout << "This is a var_declare statement" << std::endl;
				gram_out_file << "This is var_declare statement" << std::endl;
				continue;
			}

			//此时 sy == comma || sy == semicolon || sy == lsbrack
			varDeal(tmp_type, pv_addr);	//出来时读到得是';'之后的单词
			var_flag = 1;
		}
		var_size = pv_addr - start_pv_addr; //局部变量所占的空间大小
		
		//从此往下不会再进行enter系列操作

		//此处为变量声明的分号结束之后的第一个单词
		symbol tmp_sys[] = { semicolon, ifsy, dosy, lcbrack, ident, scanfsy, printfsy, switchsy, returnsy, rcbrack };
		skip(tmp_sys, 10);

		while (sy != rcbrack) {
			skip(statement_start_sys, 9);
			//进入statement时, sy为statement_start_sys中的元素
			statement();	//进入语句处理函数
			//出statement时, sy 为 statement_end_sys中的元素
			insymbol();
			if ((sy != rcbrack) && (!find_sy(statement_start_sys, 9))) {
				error(48,lc);
			}
		}

		
		if (var_flag == 1) return tab.t - 1;
		else return btab.btabArray[btab.b-1].lastpar;
		//出复合语句处理时,sy == rcbrack
	}

	//<无返回值函数定义>
	void voFuncDeal(types typ){
		//进该模块时,sy == lparent
		//std::cout << "This is a voidFunc declare statement" << std::endl;
		gram_out_file << "This is a voidFunc declare statement" << std::endl;
		int para_num = 0, var_size = 0,	pv_addr = 3;	//pv_addr 相对于函数基址的相对地址, 0、1、2为内务区
		types return_typ = notyp;//最后比对return_typ与typ是否一致  typ为声明时的返回值类型
		std::string func_name = id;

		int init_t_num, last_t_num;		//该函数定义中第一个和(最后一个临时变量 + 1) 的编号, 用于后面得到t_varnum和各个临时变量的相对地址

		init_t_num = temp_num;

		return_flag = 0;

		enter(id, function, typ);
		enterFunc(tab.t - 1, typ);

		level++;	//进入分程序层(局部)
		display[level] = btab.b - 1;	//更新当前层分程序的索引表

		//压入函数声明四元式指令
		enterMidCode(14, " ", types_info[typ], func_name, 0);

		insymbol();

		if (sy == rparent) {	//无参数
			btab.btabArray[btab.b - 1].lastpar = 0;
			btab.btabArray[btab.b - 1].paranum = 0;
			insymbol();
		}
		else if ((sy == intsy) || (sy == charsy)) {	//有参数
			//在参数表中压入相应的四元式指令
			btab.btabArray[btab.b - 1].lastpar = paraDeal(para_num, pv_addr);//处理参数表,出来后为')'之后的单词
			btab.btabArray[btab.b - 1].paranum = para_num;
		}
		else {
			//出错跳读直到遇见左大括号
			error(31, lc);
			symbol tmp_sys[] = { lcbrack };
			skip(tmp_sys, 1);
		}
		if (sy != lcbrack) {
			error(17, lc);
			symbol tmp_sys[] = { lcbrack };
			skip(tmp_sys, 1);
		}

		//到达此处的条件是遇到左大括号

		//函数参数声明结束,将goto语句压入四元式
		//enterMidCode(5, func_name + "_end_", " ", " ", 0);

		//函数开始label,压入四元式 setlab
		enterMidCode(13, " ", " ", creat_label(func_name, 0, MidCodeT.mdc), 4);		//4表示函数开始标签

		insymbol();
		if (sy == rcbrack) {
			//相当于复合语句直接为空,就不进去了,免得尴尬
			btab.btabArray[btab.b - 1].last = btab.btabArray[btab.b - 1].lastpar;
			btab.btabArray[btab.b - 1].varsize = 0;
			//结束函数跳转四元式 本来是在复合语句中处理,但这里没有进入所以要处理一下
			enterMidCode(12, " ", " ", " ", 0);
		}
		else {
			//进入复合语句,此时sy为左括号之后的第一个单词
			btab.btabArray[btab.b - 1].last = statementCompond(var_size, return_typ, pv_addr);
			btab.btabArray[btab.b - 1].varsize = var_size;
			//if (return_typ != typ)	error(12, lc);	//声明类型与返回类型不匹配
			if ((return_flag != 0) && (return_flag != 1)) error(12, lc - 1);
			if (MidCodeT.midcodeArray[MidCodeT.mdc - 1].op != 12) {
				//函数最后一条不是返回语句
				enterMidCode(12, " ", " ", " ", 0);	//void的函数直接增加一条,要不跳不回去
			}
		}

		last_t_num = temp_num;
		btab.btabArray[btab.b - 1].t_varnum = last_t_num - init_t_num;

		//设置T_MAP
		for (int i = init_t_num; i < last_t_num; i++) {
			int offset = pv_addr + i - init_t_num;
			T_Map["T" + int_to_string(i)].stack_addr = offset;
		}

		//此处sy == rcbrack才是从复合语句中正常出来
		if (sy != rcbrack) {
			std::cout << "There is a bug in blockdeal--voFuncDeal, after the transfer of statementCompond" << std::endl;
		}
		//在此处压入函数结束标签
		//enterMidCode(13, " ", " ", creat_label(func_name, 1, MidCodeT.mdc), 1);	//创建label与PC的对应关系时mdc指的刚好是setlab的位置,还未进行++操作
		level--;	//退出分程序层
		insymbol();	//'}'的下一个单词
	}

	//<有返回值函数定义>
	void reFuncDeal(types typ){
		//进该模块时,sy == lparent
		//std::cout << "This is a returnFunc declare statement"<<std::endl;
		gram_out_file << "This is a returnFunc declare statement" << std::endl;
		int para_num = 0, var_size = 0, pv_addr = 3;	//pv_addr为参数、局部变量相对于当前数据区的相对地址 从3开始, 0、1、2的位置为内务区
		types return_typ = notyp;//最后比对return_typ与typ是否一致  typ为声明时的返回值类型
		std::string func_name = id;
		int init_t_num, last_t_num;

		init_t_num = temp_num;

		return_flag = 0;

		enter(id, function, typ);
		enterFunc(tab.t - 1, typ);
		
		//进入时level = 0, 出去时也得是0, 函数内部处理为1
		level++; //当前分程序层为1
		display[level] = btab.b - 1;	//更新当前层分程序的索引表

		//压入函数声明四元式指令
		enterMidCode(14," ",types_info[typ],func_name, 0);

		insymbol();
		if (sy == rparent) {	//无参数
			btab.btabArray[btab.b - 1].lastpar = 0;
			btab.btabArray[btab.b - 1].paranum = 0;
			insymbol();
		}
		else if ((sy == intsy) || (sy == charsy)) {	//有参数
			//在参数表中压入相应的四元式指令
			btab.btabArray[btab.b-1].lastpar = paraDeal(para_num, pv_addr);//处理参数表,出来后为')'之后的单词
			btab.btabArray[btab.b - 1].paranum = para_num;
		}
		else {
			//出错跳读直到遇见左大括号
			error(31, lc);
			symbol tmp_sys[] = { lcbrack };
			skip(tmp_sys,1);
		}
		//从参数表处理出来读的单词是 ')'后面一个单词
		if (sy != lcbrack) {
			error(17,lc);
			symbol tmp_sys[] = { lcbrack };
			skip(tmp_sys, 1);
		}

		//到达此处的条件是遇到左大括号

		//函数参数声明结束,将goto语句压入四元式
		//enterMidCode(5, func_name + "_end_", " ", " ", 0);

		//函数开始label,压入四元式 setlab
		enterMidCode(13," ", " ",creat_label(func_name,0,MidCodeT.mdc),4);

		insymbol();
		if (sy == rcbrack) {
			//相当于复合语句直接为空,就不进去了,免得尴尬
			btab.btabArray[btab.b - 1].last = btab.btabArray[btab.b - 1].lastpar;
			btab.btabArray[btab.b - 1].varsize = 0;
			error(12, lc - 1);	//有返回值函数才会报错
			//结束函数跳转四元式 本来是在复合语句中处理,但这里没有进入所以要处理一下
			enterMidCode(12, " ", " ", " ", 0);
		}
		else {
			//进入复合语句
			btab.btabArray[btab.b - 1].last = statementCompond(var_size, return_typ, pv_addr);
			btab.btabArray[btab.b - 1].varsize = var_size;
			if (return_flag == 2) {	//返回值为ints
				if (typ != ints) error(12, lc - 1);
			}
			else if (return_flag == 3) {	//返回值为chars
				if (typ != chars) error(12, lc - 1);
			}
			else {
				error(12, lc - 1);
			}
			if (MidCodeT.midcodeArray[MidCodeT.mdc - 1].op != 12) {
				//函数最后一条不是返回语句
				enterMidCode(12, " ", " ", " ", 0);	//直接增加一条,返回调用位置
			}
		}

		last_t_num = temp_num;
		btab.btabArray[btab.b - 1].t_varnum = last_t_num - init_t_num;

		//设置T_MAP
		for (int i = init_t_num; i < last_t_num; i++) {
			int offset = pv_addr + i - init_t_num;
			T_Map["T" + int_to_string(i)].stack_addr = offset;
		}



		//此处sy == rcbrack才是从复合语句中正常出来
		if (sy != rcbrack) {
			std::cout << "There is a bug in blockdeal--reFuncDeal, after the transfer of statementCompond" << std::endl;
		}
		//在此处压入函数结束标签
		//enterMidCode(13," "," ",creat_label(func_name, 1, MidCodeT.mdc),1);
		
		level--; //退出当前分程序层
		
		insymbol();	//'}'的下一个单词
	}

	//<主运行函数处理>
	void mainFuncDeal(){
		//进入该模块时 sy == lparent
		//std::cout << "This is a main declare statement" << std::endl;
		gram_out_file << "This is a main declare statement" << std::endl;
		int para_num = 0, var_size = 0, pv_addr = 3;	//pv_addr 相对于函数基址的相对地址, 3个内务区,为了call指令翻译为mips的一致性
		types return_typ = notyp;//最后比对return_typ与typ是否一致  typ为声明时的返回值类型
		std::string func_name = "main";
		int init_t_num, last_t_num;

		init_t_num = temp_num;


		enter(func_name, function, notyp);
		enterFunc(tab.t - 1, notyp);

		level++;	//进入分程序层(局部)
		display[level] = btab.b - 1;	//更新当前层分程序的索引表

		//压入函数声明四元式指令
		enterMidCode(14, " ", types_info[notyp], func_name, 0);

		insymbol();
		if (sy != rparent) {	//main函数无参数
			error(10, lc);

		}
		//直接跳读到左大括号
		symbol tmp_sys[] = { lcbrack };
		skip(tmp_sys, 1);


		//函数开始label,压入四元式 setlab
		enterMidCode(13, " ", " ", "main", 2);		//2标志为main函数开始位置

		//到达此处的条件是遇到左大括号
		insymbol();
		if (sy == rcbrack) {
			//相当于复合语句直接为空,就不进去了,免得尴尬
			btab.btabArray[btab.b - 1].last = btab.btabArray[btab.b - 1].lastpar;
			btab.btabArray[btab.b - 1].varsize = 0;
		}
		else {
			//进入复合语句
			btab.btabArray[btab.b - 1].last = statementCompond(var_size, return_typ, pv_addr);
			btab.btabArray[btab.b - 1].varsize = var_size;
			if (return_typ != notyp)	error(12, lc);	//声明类型与返回类型不匹配
			
		}

		last_t_num = temp_num;
		btab.btabArray[btab.b - 1].t_varnum = last_t_num - init_t_num;

		//设置T_MAP
		for (int i = init_t_num; i < last_t_num; i++) {
			int offset = pv_addr + i - init_t_num;
			T_Map["T" + int_to_string(i)].stack_addr = offset;
		}

		//此处sy == rcbrack才是从复合语句中正常出来
		if (sy != rcbrack) {
			std::cout << "There is a bug in blockdeal--mainFuncDeal, after the transfer of statementCompond" << std::endl;
		}
		//在此处压入函数结束标签
		//enterMidCode(13, " ", " ", creat_label(func_name, 1, MidCodeT.mdc), 1);
		level--;	//退出分程序层
	}

	// <程序> 处理
	void block(){
		types tmp_type;
		symbol tmp_sy;	//判别是否为miansy的时候使用
		int global_var_addr = 0, global_var_num = 0;
		gram_out_file.open("some_info_out/gram_output.txt", std::fstream::out);

		insymbol();
		skip(block_start_sys, 4);
		//进行const处理
		while(sy == constsy){
			constDeal();	//从constDeal里面出来时sy为';'后面的单词
		}

		//全局变量处理
		while(1){
			skip(block_start_sys, 4);//检测 sy 是否为 constsy, intsy, charsy 或者 voidsy, 都不是的话跳读
			if (sy == constsy) {
				error(29, lc);
				insymbol();
				skip(block_start_sys, 4);
				continue;
			}
			tmp_type = sy_to_types(sy);
			insymbol();
			tmp_sy = sy;
			if ((sy != ident) && (sy != mainsy)) {
				error(11, lc);
				skip(block_start_sys, 4);
				//std::cout << "This is a var_declare statement" << std::endl;
				gram_out_file << "This is var_declare statement" << std::endl;
				continue;
			}

			insymbol();

			//test sy 是否为 comma, semicolon, lparent, lsbrack 都不是的话报错
			symbol tmpsys[] = { comma, semicolon, lparent, lsbrack};
			if (!find_sy(tmpsys, 4)) {
				error(28, lc);
				skip(block_start_sys, 4);
				//std::cout << "This is a var_declare statement" << std::endl;
				gram_out_file << "This is var_declare statement" << std::endl;
				continue;
			}

			if(sy == lparent) break;	//函数定义
			//进变量定义之前的条件
			if (tmp_type == voidsy) {	//变量只能是char或者int类型
				error(28, lc);
				skip(block_start_sys, 4);
				//std::cout << "This is a var_declare statement" << std::endl;
				gram_out_file << "This is var_declare statement" << std::endl;
				continue;
			}
			varDeal(tmp_type,global_var_addr);	//出来时读到得是';'之后的单词
		}


		//处理btab.btabArray[0],把全局加进去
		btab.btabArray[0].last = tab.t - 1;
		btab.btabArray[0].varsize = global_var_addr; //全局变量所占数据区的总大小

		//把全局变量压栈
		for (int i = 0; i < tab.t; i++) {
			Stack.space[Stack.topaddr++] = tab.tabArray[i].adr;//把全局变量的值压进去
		}
		Stack.globalvaraddr = 0;	//全局变量开始的位置为0
		


		//从全局变量处理出来的条件:  sy == lparent
		if (sy != lparent) {
			std::cout << "There is a bug in blockdeal, before funcDeal" << std::endl;
		}

		if (tmp_sy != mainsy) {
			//处理出来以后的第一个函数
			if (tmp_type == notyp) {	//无返回值函数定义
				voFuncDeal(tmp_type);
			}
			else {		//有返回函数定义
				reFuncDeal(tmp_type);
			}
			//从函数处理出来后为'}'后面一个单词
			//循环处理剩下的函数,从int、char或者void开始
			while (1) {
				skip(block_start_sys, 4);	//检测 sy 是否为 constsy, intsy, charsy 或者 voidsy, 都不是的话跳读
				if (sy == constsy) {
					error(29, lc);
					insymbol();
					skip(block_start_sys, 4);
					continue;
				}
				tmp_type = sy_to_types(sy);
				insymbol();
				tmp_sy = sy;

				if ((sy != ident) && (sy != mainsy)) {
					error(11, lc);
					skip(block_start_sys, 4);
					continue;
				}

				insymbol();
				if (sy != lparent) {
					error(13, lc);
					skip(block_start_sys, 4);
					continue;
				}

				//在此位置 sy == lparent;
				if (tmp_sy == mainsy) break;	//主运行函数
				if (tmp_type == notyp) {	//无返回值函数定义
					voFuncDeal(tmp_type);
				}
				else {		//有返回函数定义
					reFuncDeal(tmp_type);
				}
				//打印tab信息
				//printTab();
				//printATab();
				//printBTab();

			}
		}

		//程序运行到此步骤的条件是tmp_sy为mainsy
		//且 sy == lparent
		if (tmp_sy != mainsy) {
			std::cout << "There is a bug in blockdeal, before mainFuncDeal" << std::endl;
		}
		//std::cout << "Begin main deal" << std::endl;
		//main函数处理,此时id为main,sy为mainsy
		if (tmp_type != notyp) {	//此时出现notyp只能是void,因为在上述while里面的main函数break前面进行了skip检测
			error(10, lc);
		}
		//进入时sy == lparent
		mainFuncDeal();

		//退出时 sy == rcbrack
		//打印tab信息
		printTab();
		printATab();
		printBTab();
		printMidCode();
		printTMap();
	}
}
