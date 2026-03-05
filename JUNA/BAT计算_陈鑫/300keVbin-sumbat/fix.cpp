#include <iostream>
#include <fstream>
#include <string>
#include <regex>

using namespace std;

void fix() {
    try {
        ifstream inputFile1("channel1_stack.C");
        ofstream outputFile1("ch1.C");
        ifstream inputFile2("channel2_stack.C");
        ofstream outputFile2("ch2.C");        

        if (!inputFile1|| !outputFile1) {
            cerr << "无法打开文件" << endl;
            return;
        }

        string line;
        while (getline(inputFile1, line)) {
            // 将 channel1_stack 替换为 chst1
            line = regex_replace(line, regex("channel1_stack"), "ch1");
            // 在创建 THStack 对象的语句前添加指针名 st
            size_t pos = line.find("THStack * = new THStack();");
            if (pos != string::npos) {
                line = line.substr(0, pos) + "THStack *st = new THStack();";
            }

            // 替换 " ->" 为 " st->"
            pos = 0;
            while ((pos = line.find(" ->", pos)) != string::npos) {
                line.replace(pos, 3, " st->");
                pos += 5; // 跳过 " st->"
            }
            outputFile1 << line << endl;
        }
                cout << "ch1处理完成！" << endl;
        inputFile1.close();
        outputFile1.close();

        while (getline(inputFile2, line)) {
            // 将 channel2_stack 替换为 chst2
            line = regex_replace(line, regex("channel2_stack"), "ch2");

            // 在创建 THStack 对象的语句前添加指针名 st
            size_t pos = line.find("THStack * = new THStack();");
            if (pos != string::npos) {
                line = line.substr(0, pos) + "THStack *st = new THStack();";
            }

            // 替换 " ->" 为 " st->"
            pos = 0;
            while ((pos = line.find(" ->", pos)) != string::npos) {
                line.replace(pos, 3, " st->");
                pos += 5; // 跳过 " st->"
            }
            outputFile2 << line << endl;
        }
                cout << "ch2处理完成！" << endl;
        inputFile2.close();
        outputFile2.close();

    } catch (const exception& e) {
        cerr << "处理文件时出现错误: " << e.what() << endl;
    }
}


