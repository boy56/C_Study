/*
	�ʷ�����������
*/
#include "worddeal.h"
#include <iostream>
namespace compiler{

    int word_count = 1;
	int fileover_flag = 0;

    //����Symbol�����ʹ�ӡ�������Ϣ����һ�δʷ�������Ҫ
    std::string return_symbol_info(symbol sy){
    	return symbol_info[sy];
    }

    //read next character
    void nextch(){
        if(cc>=ll){
            //�ļ�������
            if(inFile.eof()){                
				if (fileover_flag == 0) {
					fileover_flag = 1;
					return;
				}
				else {
					error(0, lc); //������
					exit(1);//�ļ�������,�������
				}
            }
           
            for(int i = 0; i < LLNG + 1; i++){
                line[i] = '\0';
            }
            ll = 0; //���г��Ⱥ���ָ������
            cc = 0;
            inFile.getline(line, LLNG, '\n');
            lc++;
            while(line[ll]!='\0') ++ll;
            line[ll] = ' '; //һ�е�ĩβ��Ϊ�ո�
        }
        ch = line[cc];
        cc = cc + 1;

    }

    //read next symbol
    void insymbol(){
        int k;
        while((ch == ' ')||(ch == '\t')){   //�����ո��tab
            nextch();
        }
        if((ch == '_')||((ch >= 'a') && (ch <= 'z'))||((ch >= 'A') && (ch <= 'Z'))){//�����ʶ��,ֻ������ĸ('_',a-z,A-Z)��������,���ҵ�һ����������ĸ
            k = 0;
            for(int t = 0; t < NAME_SIZE; t++) id[t] = '\0';
            do{
                if (k < NAME_SIZE){
                    if((ch >= 'A') && (ch <= 'Z')) id[k] = ch + 32;
                    else id[k] = ch;
                    ++k;
                    nextch();
                }
				if (cc == 1) break;	//����ֱ�ӳ���
            }while((ch == '_')||((ch >= 'a') && (ch <= 'z'))||((ch >= 'A') && (ch <= 'Z'))||((ch >= '0') && (ch <= '9')));
            //�ڹؼ��ֱ��в�ѯ�Ƿ��и�����
            auto iter = KeyMap.find(id);
            //û���ҵ�
            if(iter == KeyMap.end()) sy = ident;
            //�ҵ���
            else sy = symbol(iter->second);
            word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<id<<std::endl;
        }else if((ch >= '0') && (ch <= '9')){   //��ʼ��0���޷�����������ʶ��, 0��ͷ�����ݴ�, ����0123����123�洢,�������������Ϣ
            char firstchar = ch;
            inum = 0;
            sy = intcon;    //����sy��ʾΪ����
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
        }else if(ch == '<'){        //��ʼС�ں�ʶ��
            nextch();
            if (ch == '='){
                sy = leq;   //С�ڵ��ں�
                nextch();
                word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<"<="<<std::endl;
            }else{
                sy = lss;   // С�ں�
                word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<"<"<<std::endl;
            }

        }else if(ch == '>'){
            nextch();
            if(ch == '='){
                sy = geq;   //���ڵ��ں�
                nextch();
                word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<">="<<std::endl;
            }else{
                sy = gtr;   //���ں�
                word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<">"<<std::endl;
            }

        }else if(ch == '\''){   //�����ſ�ʼ�����ַ�
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
                nextch(); //�ǵ������������ȡ��һ���ַ�
            }
        }else if(ch == '\"'){   //˫���ſ�ʼ�����ַ���
            k = 0;
            nextch();
            while((ch != '\"') && (cc != 1)){
                if((ch == 32) || (ch == 33) || ((ch >= 35) && (ch <= 126))){    //�Ϸ��ַ�
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
            if(ch == '\"'){ //�ַ�����������
                sy = stringcon;
                inum = stab.sx; //�洢�ַ�����ʼʱ���±�
                sleng = k; //�ַ����ĳ���
                stab.sx = stab.sx + k;
                nextch();

                //�ʷ�������ӡʹ��
                char stemp[SMAX];
                for(int i = 0; i < k; i++){
                    stemp[i] = stab.stringArray[inum + i];
                }
                stemp[k] = '\0';
                word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<stemp<<std::endl;


            }else{          //cc = 1,δ����˫���žͻ�����,���б���
                error(5, lc-1);
                word_out_file<<word_count++<<' '<<"error: no  \" but reach next line"<<std::endl;
            }
        }else if(ch == '!'){ //����!=,'!'�����������
            nextch();
            if(ch == '='){
                sy = neq;
                nextch();
                word_out_file<<word_count++<<' '<<return_symbol_info(sy)<<' '<<"!="<<std::endl;
            }else{
                error(6,lc);
                sy = neq; //�ݴ�,������Ϊ != ����
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
