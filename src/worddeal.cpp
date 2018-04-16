/*
	词法分析及处理
*/
#include "worddeal.h"
#include <iostream>
namespace compiler{

    int word_count = 1;
	int fileover_flag = 0;

    //根据Symbol的类型打印其具体信息，第一次词法分析需要
    std::string return_symbol_info(symbol sy){
    	return symbol_info[sy];
    }

    //read next character
    void nextch(){
        if(cc>=ll){
            //文件读完了
            if(inFile.eof()){                
				if (fileover_flag == 0) {
					fileover_flag = 1;
					return;
				}
				else {
					error(0, lc); //错误处理
					exit(1);//文件读完了,编译结束
				}
            }
           
            for(int i = 0; i < LLNG + 1; i++){
                line[i] = '\0';
            }
            ll = 0; //将行长度和行指针清零
            cc = 0;
            inFile.getline(line, LLNG, '\n');
            lc++;
            while(line[ll]!='\0') ++ll;
            line[ll] = ' '; //一行的末尾置为空格
        }
        ch = line[cc];
        cc = cc + 1;

    }

    //read next symbol
    void insymbol(){
        int k;
        while((ch == ' ')||(ch == '\t')){   //跳过空格和tab
            nextch();
        }
        if((ch == '_')||((ch >= 'a') && (ch <= 'z'))||((ch >= 'A') && (ch <= 'Z'))){//处理标识符,只能是字母('_',a-z,A-Z)或者数字,而且第一个必须是字母
            k = 0;
            for(int t = 0; t < NAME_SIZE; t++) id[t] = '\0';
            do{
                if (k < NAME_SIZE){
                    if((ch >= 'A') && (ch <= 'Z')) id[k] = ch + 32;
                    else id[k] = ch;
                    ++k;
                    nextch();
                }
				if (cc == 1) break;	//换行直接出来
            }while((ch == '_')||((ch >= 'a') && (ch <= 'z'))||((ch >= 'A') && (ch <= 'Z'))||((ch >= '0') && (ch <= '9')));
            //在关键字表中查询是否有该名字
            auto iter = KeyMap.find(id);
            //没有找到
            if(iter == KeyMap.end()) sy = ident;
            //找到了
            else sy = symbol(iter->second);
            word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<id<<std::endl;
        }else if((ch >= '0') && (ch <= '9')){   //开始对0和无符号整数进行识别, 0开头的数容错, 例如0123按照123存储,但是输出错误信息
            char firstchar = ch;
            inum = 0;
            sy = intcon;    //设置sy表示为整数
            do{
                inum = inum * 10 + (ch - '0');
                nextch();              
                if(inum > NMAX){
					error(1,lc);
                }
            }while((ch >= '0') && (ch <= '9'));

            if((firstchar == '0') && (inum != 0)){
				error(9, lc);
                word_out_file<<word_count<<" error: 0 can't be the first, but we accept it"<<std::endl;
            }
            word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<inum<<std::endl;
        }else if(ch == '<'){        //开始小于号识别
            nextch();
            if (ch == '='){
                sy = leq;   //小于等于号
                nextch();
                word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<"<="<<std::endl;
            }else{
                sy = lss;   // 小于号
                word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<"<"<<std::endl;
            }

        }else if(ch == '>'){
            nextch();
            if(ch == '='){
                sy = geq;   //大于等于号
                nextch();
                word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<">="<<std::endl;
            }else{
                sy = gtr;   //大于号
                word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<">"<<std::endl;
            }

        }else if(ch == '\''){   //单引号开始处理字符
            nextch();
            if(ch == '\''){
                error(2,lc);
                sy = charcon;
                inum = 0;
                word_out_file<<word_count++<<' '<<"error: char is empty"<<std::endl;
                //nextch();
            }else if((ch == '+') || (ch == '-') || (ch == '*') || (ch == '/') || (ch == '_') || ((ch >= '0') && (ch <= '9'))
                        || ((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'))){
                sy = charcon;
                inum = ch;
                word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<ch<<std::endl;
                nextch();
            }else{
                sy = charcon;
                inum = 0;
                error(8, lc);
                word_out_file<<word_count++<<' '<<"error: illegal char in char_def"<<std::endl;
                nextch();
            }

            if(ch != '\''){
                error(3,lc);
                word_out_file<<word_count++<<' '<<"error:must be '\''"<<std::endl;
            }else {
                nextch(); //是单引号则继续读取下一个字符
            }
        }else if(ch == '\"'){   //双引号开始处理字符串
            k = 0;
            nextch();
            while((ch != '\"') && (cc != 1)){
                if((ch == 32) || (ch == 33) || ((ch >= 35) && (ch <= 126))){    //合法字符
                    stab.stringArray[stab.sx + k] = ch;
                    ++k;
                }else{
                    error(4,lc);
                    word_out_file<<word_count++<<"error: illegal char in string, which will be skiped"<<std::endl;
                }
                if(stab.sx + k == SMAX){
                    fatal(0, lc);
                    word_out_file<<word_count++<<' '<<"error: stab is full"<<std::endl;
                }
                nextch();
            }
            if(ch == '\"'){ //字符串正常结束
                sy = stringcon;
                inum = stab.sx; //存储字符串开始时的下标
                sleng = k; //字符串的长度
                stab.sx = stab.sx + k;
                nextch();

                //词法分析打印使用
                char stemp[SMAX];
                for(int i = 0; i < k; i++){
                    stemp[i] = stab.stringArray[inum + i];
                }
                stemp[k] = '\0';
                word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<stemp<<std::endl;


            }else{          //cc = 1,未读到双引号就换行了,进行报错
                error(5, lc-1);
                word_out_file<<word_count++<<' '<<"error: no  \" but reach next line"<<std::endl;
            }
        }else if(ch == '!'){ //处理!=,'!'单独出现算错
            nextch();
            if(ch == '='){
                sy = neq;
                nextch();
                word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<"!="<<std::endl;
            }else{
                error(6,lc);
                sy = neq; //容错,将！作为 != 处理
                word_out_file<<word_count++<<' '<<"error: \'!\' can\'t appear alone"<<std::endl;
            }

		}
		else if (ch == '=') {
			nextch();
			if (ch == '=') {
				sy = eql;
				word_out_file << word_count++ << ' ' << "eql" << ' ' << "==" << std::endl;
				nextch();
			}
			else {
				sy = assign;
				word_out_file << word_count++ << ' ' << "assign" << ' ' << ch << std::endl;
			}
		}
		else if ((ch == '+') || (ch == '-') || (ch == '*') || (ch == '/') || (ch == '(')
                || (ch == ')') || (ch == '[') || (ch == ']') || (ch == '{') || (ch == '}') || (ch == ',')
                || (ch == ';') || (ch == ':')){

            sy = SpsMap[ch];
            word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<ch<<std::endl;
            nextch();
        }else {
            error(7,lc);
            word_out_file<<word_count++<<' '<<"error: illegal char in program"<<std::endl;
			nextch();
        }

    }
}
