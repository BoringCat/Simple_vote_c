#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <math.h>

char Name[12][30] = {'\0'};                                     //人名
unsigned int score[12] = {0};                                   //投票分数
unsigned int Nscore = 0;                                        //投票人数
double Np = 50.0;                                               //得票比例
int WinX = 0, WinY = 0;                                         //窗口大小
char LoadName[256] = {0};                                       //读取配置文件的名字

char *IsMMP(int *str){                                         //过滤中文奇怪的字符……
    char *s;
    s = (char *)calloc(3, sizeof(char));
    if (str[0] == 226 && str[1] == 128 && str[2] == 166) {      //如果输入了mmp的字符
        s[0] = 77;
        s[1] = 77;
        s[2] = 80;
    }
    return s;
}

//获取字符串的长度，用于光标定位
int longofstring(char *str, int Status){
    int i = 0, n = 0;                                           //定义变量i为字符串长度，n为字符串显示长度
    if (str[0] == '\0') {                                       //如果为空，返回0
        return 0;
    }
    do {                                                        //当数组中存在字符时
        if (str[i] > 0){                                        //如果是正常字符
            i++;                                                //长度+1
            n++;
        }
        else if (str[i] < 0) {                                  //如果是中文
            i += 3;                                             //长度+3
            n += 2;                                             //显示长度+2
        }
    } while(str[i] != '\0');
    if (Status==1) return n;                                    //状态1时返回显示长度
    else return i;                                              //状态0时返回字符串长度
}

//删除候选人名字
void delName() {
    for (size_t i = 0; i < 12; i++) {                           //遍历12个候选人
        for (size_t j = 0; j < 30; j++) {                       //遍历字符数组
            Name[i][j] = '\0';                                  //清除字符
        }
    }
}

//将char[]转换为double
double str2dbl(char *str, unsigned int n){
    int IsDot = 0, Doti = 0;                                    //定义变量：判断点和点后几位
    double Dbl = 0;                                             //传递的Double变量
    for (size_t i = 0; i < n; i++) {                            //在指定的数组长度中遍历
        if (str[i] == 46){                                      //如果找到点
            IsDot = 1;                                          //设定变量值
            continue;                                           //进行下一次循环
        }
        if (str[i] >= 48 && str[i] <= 57){                      //如果字符是数字
            if (IsDot == 0) {                                   //如果没有输入点
                Dbl = Dbl * 10 + (str[i] - 48);                 //将数字加在后面
            }
            else{                                               //如果有输入点
                Dbl += (str[i] - 48) * pow(0.1, ++Doti);        //将数字输入点后面
            }
        }
    }
    return Dbl;                                                 //返回Double变量
}

//将double转换为char[]
char* dbl2str(unsigned int n, double Dbl) {
    char *s;                                                    //定义字符指针变量
    s = (char *)calloc(n, sizeof(char));                        //初始化指针变量
    snprintf(s, n, "%.8g", Dbl);                                //将Double“打印”到字符串中
    return s;                                                   //返回指针
}

//搞事的帮助
void printhelp(WINDOW *win, int SW, int SH) {
    touchwin(win);                                                  //窗口获取焦点
    mvwprintw(win, 1, SW / 2 - 1, "帮助");                           //别想了，下面都是输出帮助
    mvwprintw(win, 3, 5, "简单的投票系统(v?.something.whenever)");
    mvwprintw(win, 5, 5, "1、感谢您有兴趣启动这个无聊的程序");
    mvwprintw(win, 6, 8, "这个程序对中文输入支持不太好(问ncurses去啊)");
    mvwprintw(win, 8, 5, "2、开始真正的帮助文档：");
    mvwprintw(win, 9, 8, "首先你会看到这些文字(早就看到了)————>");
    WINDOW *example;                                                //定义示例窗口
    example = newwin(12,23,6,WinX - 26);                            //将示例窗口定义到右边
    box(example, 0, 0);                                             //为示例窗口加上边框
    mvwprintw(example, 2, 2, "1——————创建(Create)");                 //继续输出帮助
    mvwprintw(example, 3, 2, "2——————修改(Update)");
    mvwprintw(example, 4, 2, "3——————投票(Set)");
    mvwprintw(example, 5, 2, "4——————显示(Show)");
    mvwprintw(example, 6, 2, "5——————保存(Save)");
    mvwprintw(example, 7, 2, "6——————帮助(Help)");
    mvwprintw(example, 8, 2, "7——————退出(Exit)");
    mvwprintw(example, 9, 2, "选择(Select)：");
    mvwprintw(win, 10, 8, "然后就选择操作就行了。");
    mvwprintw(win, 12, 5, "3、(1).选创建就会创建新的候选人数据哦，");
    mvwprintw(win, 13, 12, "原本的会消失的");
    mvwprintw(win, 14, 8, "(2).想改候选人姓名或者得票比例的话，");
    mvwprintw(win, 15, 12, "就选修改哦");
    mvwprintw(win, 16, 8, "(3).所谓投票嘛，就是不停点点点哦，注意退出");
    mvwprintw(win, 17, 12, "之前要先选”下一个“哦");
    mvwprintw(win, 18, 8, "(4).“显示”下谁的得票最高哦！(当然是“今”啦)");
    mvwprintw(win, 19, 8, "(5).如果想把数据存起来就选保存哦，记得写文件名哦");
    mvwprintw(win, 20, 8, "(6).来嘛，来这里看看人家嘛");
    mvwprintw(win, 21, 8, "(7).退出的时候要轻轻的哦，不要那么粗暴嘛，人家会不高兴的啦");
    mvwprintw(win, 23, 5, "4、要记得，这个程序是有窗口的是有窗口的是有窗口的，不要单纯的认为");
    mvwprintw(win, 24, 8, "是命令行模拟的(其实就是)");
    mvwprintw(win, 26, 5, "5、其实这程序有规定窗口大小的啦(就是作者懒)。");
    mvwprintw(win, 27, 8, "偷偷告诉你哦，终端大小小于 101x31 的都看不到这些(ge)内容的");
    touchwin(example);                                              //将示例窗口移动到顶层
    wrefresh(win);                                                  //刷新显示窗口
    refresh();                                                      //刷新主窗口
    wrefresh(example);                                              //刷新示例窗口
    delwin(example);
}

//保存数据到文件
void savefile(WINDOW *win, int SW, int SH) {
    int key = 0, p = 0, pn = 0, x, y;                               //定义键盘输入变量，字符串长度和显示长度
    char file[256] = {0};                                           //定义输入的文件名
    int i = 0;                                                      //定义光标位置
    int IsInputChinese = 0;                                         //定义中文输入判断
    int Chinese[3];                                                 //定义中文变量
    FILE *fw;                                                       //定义文件指针变量
    if ((p = longofstring(LoadName, 0)) != 0){                      //如果用户加载了配置文件
        strcpy(file, LoadName);                                     //将配置文件定为默认文件名
        pn = longofstring(LoadName, 1);                             //获取配置文件名的显示长度
    }
    else {                                                          //没有加载配置文件
        strcpy(file, "User.cfg");                                   //设为默认文件名
        p = 8;
        pn = 8;
    }
    WINDOW *Savef[2];                                               //定义输入窗口
    Savef[0] = newwin(10, 50, WinY/2-5, WinX/2-13);                 //设定显示窗口大小和位置
    Savef[1] = newwin(3, 48, WinY/2-1, WinX/2-12);                  //设定输入窗口大小和位置
    box(Savef[0], 0, 0);                                            //为显示窗口绘制边框
    box(Savef[1], 0, 0);                                            //为输入窗口绘制边框
    refresh();                                                      //刷新主窗口以显示新建的窗口
    mvwprintw(Savef[0], 2, 1, "请输入你想保存的文件名(默认:User.cfg):"); //输出文字提示
    mvwprintw(Savef[0], 8, 12, "<取消>");                            //输出按钮
    mvwprintw(Savef[0], 8, 32, "<确定>");
    mvwprintw(Savef[1], 1, 1, "%s", file);                          //输出默认文件名
    wrefresh(Savef[0]);                                             //刷新新建的窗口
    wrefresh(Savef[1]);
    touchwin(Savef[1]);                                             //将输入出口移动到最顶端
    keypad(Savef[0], TRUE);                                         //允许窗口接键盘值
    wmove(Savef[0], 5, pn + 2);                                     //移动光标到文件名末尾
    while (1) {
        key = wgetch(Savef[0]);                                     //从键盘读取值，类似于getchar()
        switch (key) {
            case 9:                                                 //用户按下了TAB
                if (i == 0) {                                       //如果光标在输入窗口
                    i++;                                            //移动光标
                    wmove(Savef[0], 8, 13);                         //移到<取消>上
                    break;
                }
                if (i == 1) {                                       //如果光标在<>上
                    i++;                                            //移动光标
                    wmove(Savef[0], 8, 33);                         //移到<确定>上
                    break;
                }
                if (i == 2) {                                       //如果光标在<确定>上
                    i = 0;                                          //移动光标
                    wmove(Savef[0], 5, 2);                          //移到输入窗口上
                    break;
                }
            break;
            case 10:                                                //用户按下了Enter
                if (i == 1) goto end;                               //如果光标在<取消>上，就退出
                if (i == 2) goto start;                             //如果光标在<确定>上，就继续
            break;
            case 127:                                               //按下退格键
                getyx(Savef[0],y,x);                                //获取光标位置，储存在x和y中
                mvwprintw(Savef[0], 7, 5, "             ");         //清除提示信息
                wmove(Savef[0], y, x);                              //光标复位
                if (x - 1 >= 2) {                                   //如果有数据需要删除
                    mvwprintw(Savef[0], y, x - 1, " ");             //清除按下按键产生的字符和需要删除的字符
                    wmove(Savef[0], y, x - 1);                      //光标复位
                }
                if (p > 0) {                                        //判断是否有数据需要与屏幕上的数据一起删除
                    if (file[p-3] < 0 && file[p-2] < 0 && file[p-1] < 0){ //如果删除的文字是中文
                        mvwprintw(Savef[0], y, x - 2, "  ");        //清除按下按键产生的字符和需要删除的字符
                        wmove(Savef[0], y, x - 2);                  //光标复位
                        p -= 3;                                     //字符串长度-3
                        pn -= 2;                                    //字符串显示长度-2
                        file[p] = '\0';                             //删除中文第一个字符
                        file[p+1] = '\0';                           //删除中文第二个字符
                        file[p+2] = '\0';                           //删除中文第三个字符
                    }
                    else{                                           //如果是标准键盘值
                        p--;                                        //字符串长度-1
                        pn--;                                       //字符串显示长度-1
                        file[p] = '\0';                             //删除最后一个字符
                    }
                }
            break;
            case KEY_DOWN:                                          //过滤掉不知道为什么出现的方向键值
            case KEY_LEFT:
            case KEY_RIGHT:
            case KEY_UP:
            break;
            default:
                getyx(Savef[0], y, x);                              //获取光标位置，储存在x和y中
                if (p > 45) {                                       //如果超出显示范围
                    mvwprintw(Savef[0], 7, 5, "文件名过长！");        //提示文件名过长
                    wmove(Savef[0], y, x);                          //光标复位
                    break;
                }
                if (i == 0) {                                       //如果光标在输入窗口
                    if ((key >= 40 && key <= 125) || key >= 128) {  //如果是适合的收入
                        if (key >= 128){                            //如果输入了中文
                            Chinese[IsInputChinese] = key;          //将中文临时储存
                            IsInputChinese++;                       //中文+1
                            if (p + 2 < 45){                        //如果输入中文以后没有超出显示范围
                                if (IsInputChinese == 3) {          //如果中文输入完成
                                    char *Temp;
                                    Temp = IsMMP(Chinese);
                                    if (Temp[0] != '\0') {          //如果输入了mmp的字符
                                        for (size_t l = 0; l < 3; l++) {
                                            mvwprintw(Savef[0], 5, p + 2 + l, "%c", Temp[l]);//输出输入的字符串
                                            file[p+l] = Temp[l];    //将输入的字符串放到文件名里面
                                            Chinese[l] = 0;         //清除输入的中文
                                        }
                                        free(Temp);
                                        pn++;                       //调整字符串变更以后的字符串显示长度
                                    }
                                    else{                           //如果输入了正常的中文
                                        for (size_t l = 0; l < 3; l++) {
                                            mvwprintw(Savef[0], 5, pn + 2, "%c", Chinese[l]);//输出输入的字符串
                                            file[p+l] = (char)Chinese[l];//将输入的字符串放到文件名里面
                                            Chinese[l] = 0;         //清除输入的中文
                                        }
                                    }
                                    IsInputChinese = 0;             //设定中文输入数量
                                    p += 3;                         //字符串长度+3
                                    pn += 2;                        //字符串显示长度+2
                                }
                                break;
                            }
                            else{                                   //如果输入中文以后超出了显示范围
                                Chinese[0] = 0;                     //清除输入的中文
                                Chinese[1] = 0;                     //清除输入的中文
                                Chinese[2] = 0;                     //清除输入的中文
                                IsInputChinese = 0;                 //设定中文输入数量
                                mvwprintw(Savef[0], 7, 5, "文件名过长！");//输出提示信息
                                wmove(Savef[0], y, x);              //光标复位
                                break;
                            }
                        }
                        else {
                            file[p] = (char)key;                    //将输入的字符加入到名字中
                            mvwprintw(Savef[0], 5, pn + 2 , "%c", key);//输出输入的字符
                            p++;                                    //字符串长度+1
                            pn++;                                   //字符串显示长度+1
                        }
                    }
                }
            break;
        }
    }
    start:                                                          //开始保存文件
    delwin(Savef[0]);                                               //删除输入窗口
    delwin(Savef[1]);
    if (longofstring(file, 256) == 0)                               //如果用户啥都没输入
        strcpy(file, "User.cfg");                                   //设定文件名为默认文件名
    if ((fw = fopen(file, "w")) == NULL) {
        wclear(win);                                                //清除输出窗口
        mvwprintw(win, SH / 2, SW / 2 - 8, "%s", "无法打开保存文件");
        wrefresh(win);                                              //刷新输出窗口
    }
    for (size_t i = 0; i < 12; i++) {
        if (Name[i][0] == '\0') {
            fprintf(fw, "%s\t%d\n", "(NULL)", score[i]);            //将数据按格式输出到文件中
        }
        else{
            for (size_t n = 0; n < 30; n++) {
                if (Name[i][n] == ' ') Name[i][n] = '_';
                if (Name[i][n] == '\0') break;
            }
            fprintf(fw, "%s\t%d\n", Name[i], score[i]);             //将数据按格式输出到文件中
        }

    }
    fprintf(fw, "%d|%lf", Nscore, Np);
    fflush(fw);                                                     //清除缓存区
    fclose(fw);                                                     //关闭文件
    end:
    return;
}

//数据创建及修改函数
void WinCU(WINDOW *win, int SW, int SH, int Status) {
    int p[13] = {0};                                                //名字字符串长度
    int psl[13] = {0};                                              //名字字符串长度
    int IsInputChinese = 0;                                         //定义中文输入判断
    int IsInputDot = 0;                                             //定义点输入判断
    int Chinese[3];                                                 //定义中文储存变量
    int key, i = 1, kx = 5, ky = 10, kmy = 10, ys = 0;              //设定光标移动所需变量
    int x, y;                                                       //光标的坐标值
    char setNp[12] = {0};                                           //定义得票比例的设置值
    char *s;                                                        //定义得票比例的字符串指针
    s = setNp;                                                      //指定指针
    int setNpi = 0;                                                 //定义得票比例输入的个数
    touchwin(win);                                                  //窗口获取焦点
    keypad(win, TRUE);                                              //允许处理键盘输入值
    switch (Status) {                                               //确定操作 0：创建 1：修改
        case 0:                                                     //创建操作下不会获取名字字符串长度
            mvwprintw(win, 2, SW /2 - 10 , "请指定得票比例%%");        //输出提示
            mvwprintw(win, SH -  2, 10 , "按<ESC>退出");
            wmove(win, 2, SW /2 +4);                                //移动光标到输入位置
            while (1) {
                getyx(win, y, x);                                   //获取光标位置，储存在x和y中
                key = wgetch(win);                                  //从键盘读取值，类似于getchar()
                switch(key){                                        //判断按下的值
                    case 10:                                        //按下回车
                        goto endNp;                                 //退出得票比例设置
                    break;
                    case 127:                                       //按下退格键
                        if (x - 1 < SW /2 + 4) break;               //如果屏幕上没有数据就忽略
                        mvwprintw(win, y, x - 1, "%%  ");           //将上一个字符覆盖，并移动%号
                        wmove(win, y, x - 1);                       //移动光标
                        if (setNp[setNpi - 1] == 46) IsInputDot = 0;//如果删除了点，更改判断
                        setNp[--setNpi] = 0;                        //从字符串中删除该字符
                    break;
                    case 27:                                        //按下ESC
                        goto end;                                   //退出创建
                        break;
                    case 46:                                        //按下点
                        if (IsInputDot == 1) break;                 //如果输入过点就忽略
                    default:
                        if (setNpi >= 12) break;                    //如果数值太大就忽略
                        if (key == 46) IsInputDot = 1;              //如果输入的是点就设置已经输入过点
                        if ((key >= 48 && key <= 57) || key == 46) {//如果输入的是数字
                            setNp[setNpi++] = (char)(key>=128?key-256:key);//将数字先存入字符串中
                            mvwprintw(win, y, x, "%c%%", key);      //输出到屏幕上
                            wmove(win, y, x+1);                     //移动光标
                        }
                    break;
                }
            }
            endNp:                                                  //得票比例跳出点
            Np = str2dbl(setNp, 12);                                //设置得票比例
            if (Np > 100) Np = 100;                                 //如果大于100就设成100
            wclear(win);                                            //清除窗口重绘
            box(win, 0, 0);                                         //为窗口加上边框
            mvwprintw(win, 1, SW/2 - 3, "%s", "投票系统");            //输出标题
            mvwprintw(win, 2, 1, "%s", "共有12位候选人：");            //输出提示
            mvwprintw(win, 2, SW - 21, "%s", "请为你喜欢的候选人+1s");
            for (size_t j = 1; j <= 12; j++) {                      //设置候选人输入位置
                mvwprintw(win, 5 + (int)((j-1) / 2) * 3, (j%2==0)?SW / 2 + 5:5, "%s%d%s", "NO.", j, ":");
                score[j-1] = 0;                                     //初始化得票
            }
            delName();                                              //初始化候选人名字
            Nscore = 0;                                             //初始化投票次数
        break;
        case 1:                                                     //如果用户选择修改
            mvwprintw(win, 1, SW/2 - 3, "%s", "投票系统");            //输出标题
            mvwprintw(win, 2, 1, "%s", "共有12位候选人：");            //输出提示
            mvwprintw(win, 2, SW - 21, "%s", "请为你喜欢的候选人+1s");
            for (size_t k = 0; k < 6; k++) {                        //设置候选人输入位置以及输出名字
                mvwprintw(win, 5 + (int)k * 3, 5, "%s%d%s%s", "NO.", k * 2 + 1, ":", Name[k*2]);//输出已输入的候选人名字
                mvwprintw(win, 5 + (int)k * 3, SW / 2 + 5, "%s%d%s%s", "NO.", k * 2 + 2, ":", Name[k*2+1]);
                p[k*2] =longofstring(Name[k*2],0);                  //获取名字字符串长度
                p[k*2+1] =longofstring(Name[k*2+1],0);
                psl[k*2] =longofstring(Name[k*2],1);                //获取名字字符串显示长度
                psl[k*2+1] =longofstring(Name[k*2+1],1);
            }
        break;
    }
    s = dbl2str(10, Np);                                            //将得票比例转换为字符串保存，方便输入
    mvwprintw(win, 24, 5, "%s%s%%", "得票比例：", s);                 //输出得票比例输入位置
    p[12] = longofstring(s,0);                                      //设置得票比例的字符串长度
    psl[12] = 4 + longofstring(s,1);                                //设置得票比例的字符串显示长度
    mvwprintw(win, SH - 2, 5, "%s", "方向键移动光标\t\t<Enter>保存\t\t没得不保存退出");//输出提示
    kx = 5;                                                         //初始化光标坐标变量
    ky = 10 + psl[0];
    wmove(win, kx, ky);                                             //将光标移动到一号上
    wrefresh(win);                                                  //刷新窗口
    while(1){                                                       //获取键盘操作
        key = wgetch(win);                                          //从键盘读取值，类似于getchar()
        switch(key){                                                //判断按下的值
            case KEY_UP:                                            //按下方向键上
                if(i > 2){                                          //判断光标位置索引
                    if (i == 13) kx -= 4;                           //如果光标在得票比例上，向上移动4格
                    else kx -=3;                                    //否则就移动3格
                        i -= 2;                                     //更改光标索引
                        if (i < 10) ys = 0;                         //如果光标移动到10号以前，取消偏移值
                        if (i % 2 == 0) {                           //判断光标纵向位置
                            kmy = SW / 2 + 10 + ys;                 //设置输入范围开始位置
                            ky = kmy + psl[i-1];                    //计算字符串结尾位置
                        }
                        else {                                      //光标在第一列
                            kmy = 10 + ys;                          //设置输入范围开始位置
                            ky = kmy + psl[i-1];                    //计算字符串结尾位置
                    }
                    wmove(win, kx, ky);                             //移动光标
                }
                break;
            case KEY_DOWN:                                          //按下方向键下
                if(i < 11){                                         //判断光标位置索引
                    i += 2;                                         //更改光标索引
                    if (i >= 10 ) ys = 1;                           //如果光标移动到10号以后，设置偏移值
                    kx += 3;                                        //光标向下移动3格
                    if (i % 2 == 0) {                               //判断光标纵向位置，光标在第二列
                        kmy = SW / 2 + 10 + ys;                     //设置输入范围开始位置
                        ky = kmy + psl[i-1];                        //计算字符串结尾位置
                    }
                    else {                                          //光标在第一列
                        kmy = 10 + ys;                              //设置输入范围开始位置
                        ky = kmy + psl[i-1];                        //计算字符串结尾位置
                    }
                    wmove(win, kx, ky);                             //移动光标
                }
                else if (i == 11 || i == 12){                       //光标在第十一或第十二位上
                    i = 13;                                         //此时只需把光标移至第十三位
                    if (i >= 10 ) ys = 1;                           //如果光标移动到10号以后，设置偏移值
                    kx = 24;                                        //光标向下移动到得票比例上
                    kmy = 10 + ys;
                    ky = kmy + psl[i-1];
                    wmove(win, kx, ky);                             //移动光标
                }
                break;
            case 353:                                               //按下Shift+TAB
            case KEY_LEFT:                                          //按下方向键左
                if (i > 1) {                                        //判断光标位置索引
                    if((i % 2) == 0){                               //判断光标纵向位置，光标在第二列
                        i -= 1;                                     //更改光标索引
                        if (i < 10) ys = 0;                         //如果光标移动到10号以前，取消偏移值
                        kmy = 10 + ys;                              //设置输入范围开始位置
                        ky = kmy + psl[i-1];                        //计算字符串结尾位置
                        wmove(win, kx, ky);                         //移动光标
                    }
                    else{                                           //光标在第一列
                        if (i == 13) kx -=4;                        //如果光标在得票比例上，向上移动4格
                        else kx -= 3;                               //否则就移动3格
                        i -= 1;                                     //更改光标索引
                        if (i < 10) ys = 0;                         //如果光标移动到10号以前，取消偏移值
                        kmy = SW / 2 + 10 + ys;                     //设置输入范围开始位置
                        ky = kmy + psl[i-1];                        //计算字符串结尾位置
                        wmove(win, kx, ky);                         //移动光标
                    }
                }
                break;
            case 9:                                                 //按下TAB
            case KEY_RIGHT:                                         //按下方向键左
                if (i < 12) {                                       //判断光标位置索引
                    if((i % 2) == 0){                               //判断光标纵向位置，光标在第二列
                        i += 1;                                     //更改光标索引
                        if (i >= 10 ) ys = 1;                       //如果光标移动到10号以后，设置偏移值
                        kmy = 10 + ys;                              //设置输入范围开始位置
                        ky = kmy + psl[i-1];                        //计算字符串结尾位置
                        kx += 3;                                    //光标向下移动3格
                        wmove(win, kx, ky);                         //移动光标
                    }
                    else{                                           //光标在第一列
                        i += 1;                                     //更改光标索引
                        if (i >= 10 ) ys = 1;                       //如果光标移动到10号以后，设置偏移值
                        kmy = SW / 2 + 10 + ys;                     //设置输入范围开始位置
                        ky = kmy + psl[i-1];                        //计算字符串结尾位置
                        wmove(win, kx, ky);                         //移动光标
                    }
                }
                else if (i == 12) {                                 //光标在第十二位上
                    i = 13;                                         //此时只需把光标移至第十三位
                    if (i >= 10) ys = 1;                            //如果光标移动到10号以后，设置偏移值
                    kx = 24;                                        //光标向下移动到得票比例上
                    kmy = 10 + ys;
                    ky = kmy + psl[i-1];
                    wmove(win, kx, ky);                             //移动光标
                }
                break;
            case 10:                                                //按下回车
                goto end;                                           //确认保存并退出
                break;
            case 27:                                                //按下ESC
                break;                                              //啥都不干
            case 127:                                               //按下退格键
                getyx(win,y,x);                                     //获取光标位置，储存在x和y中
                if (i == 13){                                       //光标在得票比例上
                    if (x - 1 >= kmy + 4) {                         //如果有数据需要删除
                        mvwprintw(win, y, x - 1, "%% ");            //将上一个字符覆盖，并移动%号
                        wmove(win, y, x - 1);                       //光标复位
                        if (s[p[i-1]-1] == 46) IsInputDot = 0;      //如果删除了点，更改判断
                        s[--p[i-1]] = '\0';                         //从字符串中删除该字符
                        psl[i-1]--;
                        if (p[i-1] == 0) {                          //如果得票比例为空
                            Np = 50;                                //设置为50%
                        }
                        else{
                            Np = str2dbl(s, 12);                    //设置得票比例
                            Np = Np>100?100:Np;                     //如果大于100就设成100
                        }
                    }
                    break;
                }
                if (x - 1 >= kmy) {                                 //如果有数据需要删除
                    mvwprintw(win, y, x - 1, " ");                  //清除按safe下按键产生的字符和需要删除的字符
                    wmove(win, y, x - 1);                           //光标复位
                }
                if (p[i-1] > 0) {                                   //判断是否有数据需要与屏幕上的数据一起删除
                    if (Name[i-1][p[i-1]-3] < 0 && Name[i-1][p[i-1]-2] < 0 && Name[i-1][p[i-1]-1] < 0){
                        mvwprintw(win, y, x - 2, "  ");             //清除按下按键产生的字符和需要删除的字符
                        wmove(win, y, x - 2);                       //光标复位
                        p[i-1] -= 3;                                //字符串长度-1
                        psl[i-1] -= 2;                              //字符串长度-1
                        Name[i-1][p[i-1]+2] = '\0';                 //删除最后一个字符
                        Name[i-1][p[i-1]+1] = '\0';                 //删除最后一个字符
                        Name[i-1][p[i-1]] = '\0';                   //删除最后一个字符
                    }
                    else{
                        p[i-1]--;                                   //字符串长度-1
                        psl[i-1]--;                                 //字符串长度-1
                        Name[i-1][p[i-1]] = '\0';                   //删除最后一个字符
                    }
                }
                break;
            case 46:                                                //按下点
                if (IsInputDot == 1 && i == 13) break;              //光标在的得票比例上且输入过点就忽略
            default:
                getyx(win,y,x);                                     //获取光标位置，储存在x和y中
                if (i == 13){                                       //如果光标在的得票比例上
                    if (p[i-1] < 12) {                              //如果没有超过得票比例长度
                        if (key == 46) IsInputDot = 1;              //如果输入过点就忽略
                        if ((key >= 48 && key <= 57) || key == 46) {//如果输入的是数字
                            s[p[i-1]++] = (char)(key>=128?key-256:key);//将数字先存入字符串中
                            mvwprintw(win, y, x, "%c%%", s[p[i-1]-1]);//输出到屏幕上
                            psl[i-1]++;                             //字符串长度+1
                            wmove(win, y, x+1);                     //移动光标
                            Np = str2dbl(s, 12);                    //设置得票比例
                            Np = Np>100?100:Np;                     //如果大于100就设成100
                        }
                    }
                    break;
                }
                if (p[i-1] < 30) {                                  //如果光标在的候选人上且没有超过长度
                    if (key == 32 || (key >= 48 && key <= 57) || (key >= 65 && key <= 90) || (key >= 97 && key <= 122) || key == 46 || key >= 128) {//如果输入的是数字、字母和中文
                        if (key >= 128){                            //如果输入的是中文
                            Chinese[IsInputChinese] = key;          //将中文临时储存
                            IsInputChinese++;                       //中文+1
                            if (p[i-1] + 3 < 30){                   //如果输入中文以后没有超出显示范围
                                if (IsInputChinese == 3) {          //如果中文输入完成
                                    char *Temp;
                                    Temp = IsMMP(Chinese);
                                    if (Temp[0] != '\0') {          //如果输入了mmp的字符
                                        for (size_t l = 0; l < 3; l++) {
                                            mvwprintw(win, y, x + l, "%c", Temp[l]);//输出输入的字符串
                                            Name[i-1][p[i-1]+(int)l] = Temp[l];    //将输入的字符串放到文件名里面
                                            Chinese[l] = 0;         //清除输入的中文
                                        }
                                        free(Temp);
                                        psl[i-1]++;                 //调整字符串变更以后的字符串显示长度
                                    }
                                    else{                           //如果输入了正常的中文
                                        for (size_t l = 0; l < 3; l++) {
                                            mvwprintw(win, y, x, "%c", Chinese[l]);//输出输入的字符串
                                            Name[i-1][p[i-1]+(int)l] = (char)(Chinese[l]-256);//将输入的字符串放到候选人姓名里面
                                            Chinese[l] = 0;         //清除输入的中文
                                        }
                                    }
                                    IsInputChinese = 0;             //设定中文输入数量
                                    p[i-1] += 3;                    //字符串长度+3
                                    psl[i-1] += 2;                  //字符串显示长度+2
                                }
                                break;
                            }
                        }
                        else {
                            Name[i-1][p[i-1]] = (char)key;          //将输入的字符加入到候选人姓名中
                            mvwprintw(win, y, x, "%c" , key);       //输出输入的字符
                            p[i-1]++;                               //字符串长度+1
                            psl[i-1]++;                             //字符串显示长度+1
                        }
                    }
                }
                else {                                              //如果超过输入长度
                    IsInputChinese = 0;                             //清除中文输入数量
                    Chinese[0] = 0;                                 //清除输入的中文
                    Chinese[1] = 0;                                 //清除输入的中文
                    Chinese[2] = 0;                                 //清除输入的中文
                }
            break;
        }
        wrefresh(win);                                              //刷新窗口
    }
    end:                                                            //跳出位置
    return;
}

//投票函数
void WinSet(WINDOW *win, int SW) {
    int p[14];                                                      //名字字符串长度
    touchwin(win);                                                  //窗口移至顶端
    mvwprintw(win, 1, SW/2 - 2, "%s", "请投票");                     //在屏幕上输出候选人的名字和投票勾选框
    mvwprintw(win, 2, 1, "%s", "共有12位候选人：");
    mvwprintw(win, 2, SW - 22, "%s", "请为你喜欢的候选人+1s");
    mvwprintw(win, 3, 1, "%s%d%s", "已有",Nscore,"人投票");
    for (size_t k = 0; k < 6; k++) {
        p[k*2] =longofstring(Name[k*2],1);                          //获取名字字符串长度
        p[k*2+1] =longofstring(Name[k*2+1],1);                      //获取名字字符串长度
        mvwprintw(win, 5 + (int)k * 3, 5, "%s", Name[k*2]);
        mvwprintw(win, 5 + (int)k * 3, 5 + p[k*2], ":[ ]");
        mvwprintw(win, 5 + (int)k * 3, SW / 2 + 5, "%s", Name[k*2+1]);
        mvwprintw(win, 5 + (int)k * 3, SW / 2 + 5 + p[k*2+1], ":[ ]");
    }
    mvwprintw(win, 23, 14, "%s", "<退出>");
    mvwprintw(win, 23, SW /2 + 14, "%s", "<下一个>");
    p[12] = 8;                                                      //设定选项的偏差值
    p[13] = 8;
    int key, i = 1, kx = 5, ky = 5 + p[0] + 2;                      //设定光标移动所需变量
    int setscore[12] = {0};                                         //设定本次的投票
    wmove(win, kx, ky);                                             //将光标移动到一号上
    keypad(win, TRUE);                                              //允许处理键盘输入值
    wrefresh(win);                                                  //刷新窗口
    while(1){                                                       //获取键盘操作
        key = wgetch(win);                                          //从键盘读取值，类似于getchar()
        switch(key){                                                //判断按下的值
            case KEY_UP:                                            //按下方向键上
                if(i > 2){                                          //判断光标位置索引
                    i -= 2;                                         //更改光标索引
                    kx -= 3;                                        //光标向上移动3格
                    if (i % 2 == 0) {                               //判断光标纵向位置，光标在第二列
                        ky = SW / 2 + 5 + p[i-1] + 2;               //设置光标横向位置
                    }
                    else {                                          //光标在第一列
                        ky = 5 + p[i-1] + 2;                        //设置光标横向位置
                    }
                    wmove(win, kx, ky);                             //移动光标
                }
                break;
            case KEY_DOWN:                                          //按下方向键下
                if(i < 13){                                         //判断光标位置索引
                    i += 2;                                         //更改光标索引
                    kx += 3;                                        //光标向下移动3格
                    if (i % 2 == 0) {                               //判断光标纵向位置，光标在第二列
                        ky = SW / 2 + 5 + p[i-1] + 2;               //设置光标横向位置
                    }
                    else {                                          //光标在第一列
                        ky = 5 + p[i-1] + 2;                        //设置光标横向位置
                    }
                    wmove(win, kx, ky);                             //移动光标
                }
                break;
            case 353:                                               //按下Shift+TAB
            case KEY_LEFT:                                          //按下方向键左
                if (i > 1) {                                        //判断光标位置索引
                    if((i % 2) == 0){                               //判断光标纵向位置，光标在第一列
                        i -= 1;                                     //更改光标索引
                        ky = 5 + p[i-1] + 2;                        //设置光标横向位置
                        wmove(win, kx, ky);                         //移动光标
                    }
                    else{                                           //光标在第二列
                        i -= 1;                                     //更改光标索引
                        kx -= 3;                                    //光标向上移动3格
                        ky = SW / 2 + 5 + p[i-1] + 2;               //设置光标横向位置
                        wmove(win, kx, ky);                         //移动光标
                    }
                }
                break;
            case 9:                                                 //按下TAB
            case KEY_RIGHT:                                         //按下方向键右
                if (i < 14){                                        //判断光标位置索引
                    if((i % 2) == 0){                               //判断光标纵向位置，光标在第二列
                        i += 1;                                     //更改光标索引
                        ky = 5 + p[i-1] + 2;                        //设置光标横向位置
                        kx += 3;                                    //光标向下移动3格
                        wmove(win, kx, ky);                         //移动光标
                    }
                    else{                                           //光标在第一列
                        i += 1;                                     //更改光标索引
                        ky = SW / 2 + 5 + p[i-1] + 2;               //设置光标横向位置
                        wmove(win, kx, ky);                         //移动光标
                    }
                }
                break;
            case 27:                                                //按下ESC
                i = 13;                                             //跳到退出键上
                ky = 5 + p[i-1] + 2;                                //设置光标横向位置
                kx = 23;                                            //退出键纵向坐标
                wmove(win, kx, ky);                                 //移动光标
            case 10:                                                //按下回车
                if (i == 13) {                                      //如果光标在<退出>上
                    WINDOW *sure;                                   //新建警告窗口
                    sure = newwin(8, 30, WinY / 2 - 4, WinX / 2 - 15);//确定窗口大小和位置
                    box(sure, 0, 0);                                //绘制边框
                    mvwprintw(sure, 2, 8, "%s", "确定要退出吗？");     //输出确认信息
                    for (size_t n = 0; n < 12; n++) {               //检索残余的数据
                        if (setscore[n] != 0) {                     //如果有投票
                        mvwprintw(sure, 3, 7, "%s", "本次投票尚未保存！");//输出多一句警告
                        }
                    }
                    mvwprintw(sure, 5, 4, "%s","<取消>");             //绘制"取消"键
                    mvwprintw(sure, 5, 20, "%s","<确定>");            //绘制"确定"键
                    touchwin(sure);                                 //警告窗口获取焦点
                    wrefresh(win);                                  //刷新父窗口
                    wmove(sure, 5, 5);                              //移动光标到<取消>上
                    keypad(sure, TRUE);                             //允许处理键盘输入值
                    wrefresh(sure);                                 //刷新窗口
                    int si = 0;                                     //光标位置索引
                    while (1) {                                     //获取键盘操作
                        key = wgetch(win);                          //从键盘读取值，类似于getchar()
                        switch(key) {                               //判断按下的值
                            case KEY_LEFT:                          //按下方向键左
                                if (si == 1) {                      //判断光标位置索引
                                    wmove(sure, 5, 5);              //移动光标到<取消>上
                                    si = 0;                         //设置光标索引在<取消>上
                                }
                                break;
                            case KEY_RIGHT:                         //按下方向键右
                                if (si == 0) {                      //判断光标位置索引
                                    wmove(sure, 5, 21);             //移动光标到<确定>上
                                    si = 1;                         //设置光标索引在<确定>上
                                }
                                break;
                            case 10:                                //按下回车
                                delwin(sure);                       //删除窗口
                                refresh();                          //刷新终端窗口
                                wrefresh(win);                      //刷新父窗口
                                if (si == 1) {                      //判断光标位置索引在<确定>上
                                    goto end;                       //跳出整个函数
                                }
                                goto endsure;                       //跳出<退出>窗口判定
                                break;
                        }
                        wrefresh(sure);                             //每更改一次光标位置就刷新一次
                    }
                    endsure:                                        //<退出>窗口跳出点
                    touchwin(win);                                  //将焦点还给父窗口
                    wmove(win, kx, ky);                             //还原光标位置
                    break;
                }
                if (i == 14) {                                      //如果光标在<下一个>上
                    Nscore++;                                       //投票人递加1
                    for (size_t i = 0; i < 12; i++) {               //检索投票情况
                        if (setscore[i] == 1) score[i]++;           //如果投了他一票
                        ky = (i % 2 == 0) ?  5 + p[i] + 2 : SW / 2 + 5 + p[i] + 2 ;//确定复选框位置
                        kx = (int)(i / 2)*3 + 5;
                        setscore[i] = 0;                            //清除投票信息
                        mvwprintw(win, kx, ky, " ");                //清除屏幕信息
                    }
                    i = 1;                                          //初始化光标索引
                    kx = 5;                                         //初始化光标纵向位置
                    ky = 5 + p[0] + 2;                              //初始化光标横向位置
                    mvwprintw(win, 3, 1, "%s%d%s", "已有",Nscore,"人投票");//更新投票信息
                    wmove(win, kx, ky);                             //初始化光标位置
                    break;
                }                                                   //如果光标不在操作按钮上
                case 32:
                if (i >= 13) break;
                if (setscore[i-1] == 0) {                           //判断是否已投票
                    setscore[i-1] = 1;                              //进行投票
                    mvwprintw(win, kx, ky, "Y");                    //输出到屏幕
                    wmove(win, kx, ky);                             //复位光标
                }
                else {                                              //如果已投票
                    setscore[i-1] = 0;                              //取消投票
                    mvwprintw(win, kx, ky, " ");                    //清除屏幕信息
                    wmove(win, kx, ky);                             //复位光标
                }
                break;
        }
        wrefresh(win);                                              //刷新窗口
    }
    end:                                                            //全函数跳出点
    refresh();                                                      //刷新终端窗口
    wrefresh(win);                                                  //刷新主窗口
    return;
}

//投票统计函数
void WinShow(WINDOW *win, int SW) {
    WINDOW *cwin[5];                                                //创建信息窗口
    double p = 0;                                                   //定义候选人得票比例
    touchwin(win);                                                  //窗口获取焦点
    mvwprintw(win, 1, SW/2 - 3, "%s", "投票结果");                   //输出标题
    mvwprintw(win, 2, (SW-70)/2, "%s%d%s", "总共", Nscore, "人投票"); //输出投票信息
    mvwprintw(win, 2, (SW-70)/2 + 54, "%s%-.4g%s", "得票比例", Np, "%即通过");//输出投票信息
    cwin[0] = newwin(15, 14, 5, (SW-74)/2 + 25);                    //信息窗口大小和位置
    cwin[1] = newwin(15, 34, 5, (SW-74)/2 + 38);
    cwin[2] = newwin(15, 8, 5, (SW-74)/2 + 71);
    cwin[3] = newwin(15, 10, 5, (SW-74)/2 + 78);
    cwin[4] = newwin(15, 12, 5, (SW-74)/2 + 87);
    for (size_t n = 0; n < 5; n++) {
        box(cwin[n], 0, 0);                                         //绘制边框
    }
    mvwprintw(cwin[0], 1, 2, "%s", "候选者编号");                    //输出信息窗口标题
    mvwprintw(cwin[1], 1, 14, "%s", "姓名");
    mvwprintw(cwin[2], 1, 2, "%s", "票数");
    mvwprintw(cwin[3], 1, 2, "%s", "得票率");
    mvwprintw(cwin[4], 1, 2, "%s", "是否中选");
    for (size_t i = 0; i < 12; i++) {                               //检索投票结果
        if (i<9) mvwprintw(cwin[0], 2 + (int)i, 6, "%d", i+1);      //判断编号长度
        else mvwprintw(cwin[0], 2 + (int)i, 5, "%d", i+1);
        mvwprintw(cwin[1], 2 + (int)i, 2, "%s", Name[i]);           //输出候选人姓名
        mvwprintw(cwin[2], 2 + (int)i, 3, "%d", score[i]);          //输出候选人得票
        if (Nscore != 0) {                                          //判断候选人得票
            p = score[i]*100/(double)Nscore;                        //将得票百分比储存到p中
            if ((int)p == 100) {                                    //判断全票的人，避免输出1x10%2%
                mvwprintw(cwin[3], 2 + (int)i, 3, "100%%");         //输出100%
            }
            else mvwprintw(cwin[3], 2 + (int)i, 3, "%.3g%%", p);    //输出得票比例
            if (p >= Np) mvwprintw(cwin[4], 2 + (int)i, 3, "%s", "True");//判断是否达标
            else mvwprintw(cwin[4], 2 + (int)i, 3, "%s", "False");
        }
        else{                                                       //如果没有得票
            mvwprintw(cwin[3], 2 + (int)i, 3, "0%%");               //输出0%
            mvwprintw(cwin[4], 2 + (int)i, 3, "%s", "False");       //输出False
        }
    }
    wrefresh(win);                                                  //刷新窗口
    for (size_t n = 0; n < 5; n++) {                                //循环销毁数据窗口
        wrefresh(cwin[n]);                                          //刷新数据窗口
        delwin(cwin[n]);                                            //销毁数据窗口
    }
}

//主函数，负责文件读取和操作选择
int main(int argc, char const *argv[]) {
    char inp;                                                       //储存用户输入
    int x, y, W1, H1;                                               //设定终端窗口大小变量
    int T = 0;                                                      //定义跳出变量
    int p = 0;                                                      //定义判断变量
    WINDOW *win[2];                                                 //设定窗口
    char input[10];                                                 //设定输入字符串
    setlocale(LC_ALL,"");                                           //设定字符串编码以支持中文
    initscr();                                                      //初始化终端窗口
    getmaxyx(stdscr,WinY,WinX);                                     //获取终端大小，存在x与y中
    endwin();                                                       //释放终端
    if (WinY < 31 || WinX < 101) {                                  //判断终端大小
        printf("终端大小不够(%dx%d)，建议大于(101x31) , 是否继续?[Y/N] ", WinX, WinY);
        scanf("%c", &inp);
        if (inp != 'y' && inp != 'Y')  exit(1);
        scanf("%c", &inp);
    }
    inp = '\0';                                                     //清空输入字符
    FILE *fr;                                                       //定义读取文件
    if (argc >= 2) {                                                //如果有传入变量
        for (size_t i = 1; (int)i < argc; i++) {                    //遍历字符串
            if ((fr = fopen(argv[i], "r")) != NULL) {               //如果字符串是文件且存在能打开
                printf("是否确定读取文件%s? [Y/N] ", argv[i]);         //询问用户是否读取该文件
                scanf("%c", &inp);                                  //获取用户输入
                if (inp == 'y' || inp == 'Y') {                     //判断以后输入值
                    for (size_t k = 0; k < 12; k++) {               //遍历文件数据并格式化读取
                        fscanf(fr, "%s\t%d\n", Name[k], &score[k]);
                        if (strcmp(Name[k], "(NULL)") == 0) {       //判断名称是否为空
                            for (size_t n = 0; n <= 5; n++) {
                                Name[k][n] = '\0';                  //清除字符串
                            }
                        }
                        else{
                            for (size_t l = 0; l < 30; l++) {
                                if (Name[k][l] == '_') Name[k][l] = ' ';//还原空格
                                if (Name[k][l] == '\0') break;
                            }
                        }
                    }
                    fscanf(fr, "%d|%lf", &Nscore, &Np);             //读取投票数和得票比例
                    fclose(fr);
                    strcpy(LoadName, argv[i]);
                    goto ReadFin;                                   //跳出配置文件读取
                }
                continue;
            }
        }
    }
    ReadFin:                                                        //配置文件读取跳出点
    initscr();                                                      //初始化终端窗口
    noecho();
    win[0]=newwin(10,21,1,3);                                       //设定选择窗口的大小和位置
    win[1]=newwin(WinY - 2, WinX - 25,1,24);                        //设定操作窗口的大小和位置
    W1 = WinX - 25;                                                 //储存操作窗口的大小
    H1 = WinY - 2;
    mvwprintw(win[0],1,1,"%s","1——————创建(Create)");                //输出选项
    mvwprintw(win[0],2,1,"%s","2——————修改(Update)");
    mvwprintw(win[0],3,1,"%s","3——————投票(Set)");
    mvwprintw(win[0],4,1,"%s","4——————显示(Show)");
    mvwprintw(win[0],5,1,"%s","5——————保存(Save)");
    mvwprintw(win[0],6,1,"%s","6——————帮助(Help)");
    mvwprintw(win[0],7,1,"%s","7——————退出(Exit)");
    mvwprintw(win[0],8,1,"%s","选择(Select)：");
    box(win[1],0,0);                                                //为操作窗口设定边框
    refresh();                                                      //刷新终端
    wrefresh(win[0]);                                               //刷新选择窗口
    wrefresh(win[1]);                                               //刷新操作窗口
    keypad(win[0], TRUE);
    printhelp(win[1], W1, H1);
    do {
        p = 0;
        getmaxyx(stdscr,y,x);
        if (WinX != x || WinY != y) {
            WinX = x;
            WinY = y;
            wresize(win[1], y - 2, x - 25);
            refresh();                                              //刷新终端
        }
        W1 = WinX - 27;                                             //储存操作窗口的大小
        H1 = WinY - 2;
        touchwin(win[0]);                                           //选择窗口获取焦点
        mvwprintw(win[0], 8, 15, "   ");                            //覆盖之前的输入
        wmove(win[0], 8, 15);                                       //复位光标
        echo();
        wgetnstr(win[0], input, 1);                                 //获取一个选择字符
        noecho();
        mvwprintw(win[0], 8, 15, "%s", input);
        mvwprintw(win[0], 9, 1, "%s", "          ");                //覆盖之前的错误提示
        wrefresh(win[0]);
        if (strchr(input, '1') != 0) p = 1;                         //判断用户输入
        if (strchr(input, '2') != 0) p = 2;
        if (strchr(input, '3') != 0) p = 3;
        if (strchr(input, '4') != 0) p = 4;
        if (strchr(input, '5') != 0) p = 5;
        if (strchr(input, '6') != 0) p = 6;
        if (strchr(input, '7') != 0) p = 7;
        if (p == 7) {                                               //用户输入退出
            T = 1;                                                  //设定退出函数
        }
        else{
            switch (p) {
                case 1:                                             //用户输入创建
                    wclear(win[1]);                                 //清除操作窗口
                    box(win[1],0,0);                                //为操作窗口重新设定边框
                    WinCU(win[1], W1, H1, 0);
                break;
                case 2:                                             //用户输入修改
                    wclear(win[1]);                                 //清除操作窗口
                    box(win[1],0,0);                                //为操作窗口重新设定边框
                    WinCU(win[1], W1, H1, 1);
                break;
                case 3:                                             //用户输入投票
                    wclear(win[1]);                                 //清除操作窗口
                    box(win[1],0,0);                                //为操作窗口重新设定边框
                    WinSet(win[1], W1);
                break;
                case 4:                                             //用户输入显示
                    wclear(win[1]);                                 //清除操作窗口
                    box(win[1],0,0);                                //为操作窗口重新设定边框
                    WinShow(win[1], W1);
                break;
                case 5:                                             //用户输入保存
                    wclear(win[1]);                                 //清除操作窗口
                    box(win[1],0,0);                                //为操作窗口重新设定边框
                    savefile(win[1], W1, H1);
                    WinShow(win[1], W1);
                    break;
                case 6:                                             //用户输入帮助
                    wclear(win[1]);                                 //清除操作窗口
                    box(win[1],0,0);                                //为操作窗口重新设定边框
                    printhelp(win[1], W1, H1);
                    break;
                default:
                    mvwprintw(win[0], 9, 1, "%s", "输入错误!");      //截获其他输入
                    wrefresh(win[0]);                               //刷新选择窗口
                    mvwprintw(win[0], 8, 15, "  ");                 //覆盖输入
                    break;
            }
            refresh();
            wrefresh(win[1]);                                       //刷新操作窗口
        }
    } while(T != 1);                                                //退出函数判断
    delwin(win[0]);                                                 //删除操作窗口
    delwin(win[1]);                                                 //删除选择窗口
    endwin();                                                       //释放终端
    return 0;
}
