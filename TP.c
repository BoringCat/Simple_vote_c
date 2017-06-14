#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <math.h>

char Name[12][30] = {'\0'};                                     //人名
unsigned int score[12] = {0};                                   //投票分数
unsigned int Nscore = 0;                                        //投票人数
double Np = 50.0;
int WinX = 0, WinY = 0;
char LoadName[256] = {0};

//获取字符串的长度，用于光标定位
int longofstring(char *str, int Status){
    int i = 0, n = 0;
    if (str[0] == '\0') {                                       //如果为空，返回0
        return 0;
    }
    do {
        if (str[i] > 0){
            i++;
            n++;
        }
        else if (str[i] < 0) {
            i += 3;
            n += 2;
        }
    } while(str[i] != '\0');
    if (Status==1) return n;
    else return i;
}

int delName() {
    for (size_t i = 0; i < 12; i++) {
        for (size_t j = 0; j < 30; j++) {
            Name[i][j] = '\0';
        }
    }
    return 0;
}

double str2dbl(char *str, unsigned int n){
    int IsDot = 0, Doti = 0;
    double Dbl = 0;
    for (size_t i = 0; i < n; i++) {
        if (str[i] == 46){
            IsDot = 1;
            continue;
        }
        if (str[i] >= 48 && str[i] <= 57){
            if (IsDot == 0) {
                Dbl = Dbl * 10 + (str[i] - 48);
            }
            else{
                Dbl += (str[i] - 48) * pow(0.1, ++Doti);
            }
        }
    }
    return Dbl;
}

char* dbl2str(unsigned int n, double Dbl) {
    char *s;
    s = (char *)calloc(n, sizeof(char));
    snprintf(s, n, "%.8g", Dbl);
    return s;
}

void printhelp(WINDOW *win, int SW, int SH) {
    touchwin(win);
    mvwprintw(win, 1, SW / 2 - 1, "帮助");
    mvwprintw(win, 3, 5, "简单的投票系统(v?.something.whenever)");
    mvwprintw(win, 5, 5, "1、感谢您有兴趣启动这个无聊的程序");
    mvwprintw(win, 6, 8, "这个程序对中文输入支持不太好(问ncurses去啊)");
    mvwprintw(win, 8, 5, "2、开始真正的帮助文档：");
    mvwprintw(win, 9, 8, "首先你会看到这些文字(早就看到了)——————————————————>");
    WINDOW *example;
    example = newwin(12,23,6,WinX - 26);
    box(example, 0, 0);
    mvwprintw(example, 2, 2, "1——————创建(Create)");                //输出选项
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
    touchwin(example);
    wrefresh(win);
    refresh();
    wrefresh(example);
}

//保存数据到文件
void savefile(WINDOW *win, int SW, int SH) {
    int key = 0, p = 0, pn = 0, x, y;
    char file[256] = {0};
    int i = 0;
    int IsInputChinese = 0;
    int Chinese[3];
    FILE *fw;
    if ((p = longofstring(LoadName, 0)) != 0){
        strcpy(file, LoadName);
    }
    WINDOW *Savef[2];
    Savef[0] = newwin(10, 50, WinY/2-5, WinX/2-13);
    Savef[1] = newwin(3, 48, WinY/2-1, WinX/2-12);
    box(Savef[0], 0, 0);
    box(Savef[1], 0, 0);
    refresh();
    mvwprintw(Savef[0], 2, 1, "请输入你想保存的文件名(默认:User.cfg):");
    mvwprintw(Savef[0], 8, 12, "<取消>");
    mvwprintw(Savef[0], 8, 32, "<确定>");
    mvwprintw(Savef[1], 1, 1, "%s", file);
    wrefresh(Savef[0]);
    wrefresh(Savef[1]);
    touchwin(Savef[1]);
    keypad(Savef[0], TRUE);
    wmove(Savef[0], 5, p + 2);
    while (1) {
        key = wgetch(Savef[0]);
        switch (key) {
            case 9:
                if (i == 0) {
                    i++;
                    wmove(Savef[0], 8, 13);
                    break;
                }
                if (i == 1) {
                    i++;
                    wmove(Savef[0], 8, 33);
                    break;
                }
                if (i == 2) {
                    i = 0;
                    wmove(Savef[0], 5, 2);
                    break;
                }
            break;
            case 10:
                if (i == 1) goto end;
                if (i == 2) goto start;
            break;
            case 127:                                           //按下退格键
                getyx(Savef[0],y,x);                                 //获取光标位置，储存在x和y中
                mvwprintw(Savef[0], 7, 5, "              ");
                wmove(Savef[0], y, x);                       //光标复位
                if (x - 1 >= 2) {                             //如果有数据需要删除
                    mvwprintw(Savef[0], y, x - 1, " ");            //清除按safe下按键产生的字符和需要删除的字符
                    wmove(Savef[0], y, x - 1);                       //光标复位
                }
                if (p > 0) {                               //判断是否有数据需要与屏幕上的数据一起删除
                    if (file[p-3] < 0 && file[p-2] < 0 && file[p-1] < 0){
                        mvwprintw(Savef[0], y, x - 2, "  ");            //清除按下按键产生的字符和需要删除的字符
                        wmove(Savef[0], y, x - 2);                     //光标复位
                        p -= 3;                                   //字符串长度-1
                        pn -= 2;                                   //字符串长度-1
                        file[p] = '\0';                   //删除最后一个字符
                        file[p+1] = '\0';                   //删除最后一个字符
                        file[p+2] = '\0';                   //删除最后一个字符
                    }
                    else{
                        p--;                                   //字符串长度-1
                        pn--;                                   //字符串长度-1
                        file[p] = '\0';                   //删除最后一个字符
                    }
                }
            break;
            case KEY_DOWN:
            case KEY_LEFT:
            case KEY_RIGHT:
            case KEY_UP:
            break;
            default:
                getyx(Savef[0], y, x);
                if (p > 45) {
                    mvwprintw(Savef[0], 7, 5, "文件名过长！");
                    wmove(Savef[0], y, x);
                    break;
                }
                if (i == 0) {
                    if ((key >= 40 && key <= 125) || key >= 128) {
                        if (key >= 128){
                            Chinese[IsInputChinese] = key;
                            IsInputChinese++;
                            if (p + 3 < 45){
                                if (IsInputChinese == 3) {
                                    if (Chinese[0] == 226 && Chinese[1] == 128 && Chinese[2] == 166) {
                                        Chinese[0] = 77;
                                        Chinese[1] = 77;
                                        Chinese[2] = 80;
                                        for (size_t l = 0; l < 3; l++) {
                                            mvwprintw(Savef[0], 5, p + 2 + l, "%c", Chinese[l]);
                                            file[p+l] = (char)Chinese[l];
                                            Chinese[l] = 0;
                                        }
                                        pn++;
                                    }
                                    else{
                                        for (size_t l = 0; l < 3; l++) {
                                            mvwprintw(Savef[0], 5, pn + 2, "%c", Chinese[l]);
                                            file[p+l] = (char)Chinese[l];
                                            Chinese[l] = 0;
                                        }
                                    }
                                    IsInputChinese = 0;
                                    p += 3;                                //字符串长度-1
                                    pn += 2;
                                }
                                break;
                            }
                            else{
                                mvwprintw(Savef[0], 7, 5, "文件名过长！");
                                wmove(Savef[0], y, x);
                                break;
                            }
                        }
                        else {
                            file[p] = (char)key;              //将输入的字符加入到名字中
                            mvwprintw(Savef[0], 5, pn + 2 , "%c", key);
                            p++;                            //字符串长度-1
                            pn++;
                        }
                    }
                }
            break;
        }
    }
    start:
    delwin(Savef[0]);
    delwin(Savef[1]);
    if (longofstring(file, 256) == 0)
    strcpy(file, "User.cfg");
    if ((fw = fopen(file, "w")) == NULL) {
        wclear(win);                                                //清除输出窗口
        mvwprintw(win, SH / 2, SW / 2 - 8, "%s", "无法打开保存文件");
        wrefresh(win);                                              //刷新输出窗口
    }
    for (size_t i = 0; i < 12; i++) {
        if (Name[i][0] == '\0') {
            fprintf(fw, "%s\t%d\n", "(NULL)", score[i]);                 //将数据按格式输出到文件中
        }
        else{
            for (size_t n = 0; n < 30; n++) {
                if (Name[i][n] == ' ') Name[i][n] = '_';
                if (Name[i][n] == '\0') break;
            }
            fprintf(fw, "%s\t%d\n", Name[i], score[i]);                 //将数据按格式输出到文件中
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
    int psl[13] = {0};                                                //名字字符串长度
    int IsInputChinese = 0;
    int IsInputDot = 0;
    int Chinese[3];
    int key, i = 1, kx = 5, ky = 10, kmy = 10, ys = 0;   //设定光标移动所需变量
    int x, y;                                                   //光标的坐标值
    char setNp[12] = {0};
    char *s;
    s = setNp;
    int setNpi = 0;
    touchwin(win);                                                  //窗口获取焦点
    keypad(win, TRUE);                                          //允许处理键盘输入值
    switch (Status) {                                               //确定操作 0：创建 1：修改
        case 0:                                                     //创建操作下不会获取名字字符串长度
            mvwprintw(win, 2, SW /2 - 10 , "请指定得票比例%%");
            mvwprintw(win, SH -  2, 10 , "按<ESC>退出");
            wmove(win, 2, SW /2 +4);
            while (1) {
                getyx(win, y, x);
                key = wgetch(win);                                      //从键盘读取值，类似于getchar()
                switch(key){                                            //判断按下的值
                    case 10:
                        goto endNp;
                    break;
                    case 127:
                        if (x - 1 < SW /2 + 4) break;
                        mvwprintw(win, y, x - 1, "%%  ");
                        wmove(win, y, x - 1);
                        if (setNp[setNpi - 1] == 46) IsInputDot = 0;
                        setNp[--setNpi] = 0;
                    break;
                    case 27:
                        goto end;
                        break;
                    case 46:
                        if (IsInputDot == 1) break;
                    default:
                        if (setNpi >= 12) break;
                        if (key == 46) IsInputDot = 1;
                        if ((key >= 48 && key <= 57) || key == 46) {
                            setNp[setNpi++] = (char)(key>=128?key-256:key);
                            mvwprintw(win, y, x, "%c%%", key);
                            wmove(win, y, x+1);
                        }
                    break;
                }
            }
            endNp:
            Np = str2dbl(setNp, 12);
            if (Np > 100) Np = 100;
            wclear(win);
            box(win, 0, 0);
            mvwprintw(win, 1, SW/2 - 3, "%s", "投票系统");
            mvwprintw(win, 2, 1, "%s", "共有12位候选人：");
            mvwprintw(win, 2, SW - 21, "%s", "请为你喜欢的候选人+1s");
            for (size_t j = 1; j <= 12; j++) {
                mvwprintw(win, 5 + (int)((j-1) / 2) * 3, (j%2==0)?SW / 2 + 5:5, "%s%d%s", "NO.", j, ":");
                score[j-1] = 0;
            }
            delName();
            Nscore = 0;

        break;
        case 1:
            mvwprintw(win, 1, SW/2 - 3, "%s", "投票系统");
            mvwprintw(win, 2, 1, "%s", "共有12位候选人：");
            mvwprintw(win, 2, SW - 21, "%s", "请为你喜欢的候选人+1s");
            for (size_t k = 0; k < 6; k++) {
                mvwprintw(win, 5 + (int)k * 3, 5, "%s%d%s%s", "NO.", k * 2 + 1, ":", Name[k*2]);
                mvwprintw(win, 5 + (int)k * 3, SW / 2 + 5, "%s%d%s%s", "NO.", k * 2 + 2, ":", Name[k*2+1]);
                p[k*2] =longofstring(Name[k*2],0);                    //获取名字字符串长度
                p[k*2+1] =longofstring(Name[k*2+1],0);
                psl[k*2] =longofstring(Name[k*2],1);                    //获取名字字符串长度
                psl[k*2+1] =longofstring(Name[k*2+1],1);
            }
        break;
    }
    s = dbl2str(10, Np);
    mvwprintw(win, 24, 5, "%s%s%%", "得票比例：", s);
    p[12] = longofstring(s,1);
    psl[12] = 4 + longofstring(s,1);
    mvwprintw(win, SH - 2, 5, "%s", "方向键移动光标\t\t<Enter>保存\t\t没得退出");
    kx = 5;
    ky = 10 + psl[0];
    wmove(win, kx, ky);                                         //将光标移动到一号上
    wrefresh(win);                                              //刷新窗口
    while(1){                                                   //获取键盘操作
        key = wgetch(win);                                      //从键盘读取值，类似于getchar()
        switch(key){                                            //判断按下的值
            case KEY_UP:                                        //按下方向键上
                if(i > 2){                                      //判断光标位置索引
                    if (i == 13) kx -= 4;
                    else kx -=3;
                        i -= 2;
                        if (i < 10) ys = 0;
                        if (i % 2 == 0) {                           //判断光标纵向位置
                            kmy = SW / 2 + 10 + ys;
                            ky = kmy + psl[i-1];
                        }
                        else {
                            kmy = 10 + ys;
                            ky = kmy + psl[i-1];
                    }
                    wmove(win, kx, ky);                         //移动光标
                }
                break;
            case KEY_DOWN:                                      //按下方向键下
                if(i < 11){                                     //判断光标位置索引
                    i += 2;
                    if (i >= 10 ) ys = 1;
                    kx += 3;
                    if (i % 2 == 0) {                           //判断光标纵向位置
                        kmy = SW / 2 + 10 + ys;
                        ky = kmy + psl[i-1];
                    }
                    else {
                        kmy = 10 + ys;
                        ky = kmy + psl[i-1];
                    }
                    wmove(win, kx, ky);                         //移动光标
                }
                else if (i == 11 || i == 12){                              //光标在第十一位上
                    i = 13;                                     //此时只需把光标移至第十二位
                    if (i >= 10 ) ys = 1;
                    kx = 24;
                    kmy = 10 + ys;
                    ky = kmy + psl[i-1];
                    wmove(win, kx, ky);                         //移动光标
                }
                break;
            case 353:                                           //按下Shift+TAB
            case KEY_LEFT:                                      //按下方向键左
                if (i > 1) {                                    //判断光标位置索引
                    if((i % 2) == 0){                           //判断光标纵向位置
                        i -= 1;
                        if (i < 10) ys = 0;
                        kmy = 10 + ys;
                        ky = kmy + psl[i-1];
                        wmove(win, kx, ky);                     //移动光标
                    }
                    else{
                        if (i == 13) kx -=4;
                        else kx -= 3;
                        i -= 1;
                        if (i < 10) ys = 0;
                        kmy = SW / 2 + 10 + ys;
                        ky = kmy + psl[i-1];
                        wmove(win, kx, ky);                     //移动光标
                    }
                }
                break;
            case 9:                                             //按下TAB
            case KEY_RIGHT:                                     //按下方向键左
                if (i < 12) {                                   //判断光标位置索引
                    if((i % 2) == 0){                           //判断光标纵向位置
                        i += 1;
                        if (i >= 10 ) ys = 1;
                        kmy = 10 + ys;
                        ky = kmy + psl[i-1];
                        kx += 3;
                        wmove(win, kx, ky);                     //移动光标
                    }
                    else{
                        i += 1;
                        if (i >= 10 ) ys = 1;
                        kmy = SW / 2 + 10 + ys;
                        ky = kmy + psl[i-1];
                        wmove(win, kx, ky);                     //移动光标
                    }
                }
                else if (i == 12) {
                    i = 13;                                     //此时只需把光标移至第十二位
                    if (i >= 10) ys = 1;
                    kx = 24;
                    kmy = 10 + ys;
                    ky = kmy + psl[i-1];
                    wmove(win, kx, ky);                         //移动光标
                }
                break;
            case 10:                                            //按下回车
                goto end;
                break;
            case 27:                                            //按下ESC
                break;
            case 127:                                           //按下退格键
                getyx(win,y,x);                                 //获取光标位置，储存在x和y中
                if (i == 13){
                    if (x - 1 >= kmy + 4) {                             //如果有数据需要删除
                        mvwprintw(win, y, x - 1, "%% ");            //清除按safe下按键产生的字符和需要删除的字符
                        wmove(win, y, x - 1);                       //光标复位
                        if (s[p[i-1]-1] == 46) IsInputDot = 0;
                        s[--p[i-1]] = '\0';
                        psl[i-1]--;
                    }
                    break;
                }
                if (x - 1 >= kmy) {                             //如果有数据需要删除
                    mvwprintw(win, y, x - 1, " ");            //清除按safe下按键产生的字符和需要删除的字符
                    wmove(win, y, x - 1);                       //光标复位
                }
                if (p[i-1] > 0) {                               //判断是否有数据需要与屏幕上的数据一起删除
                    if (Name[i-1][p[i-1]-3] < 0 && Name[i-1][p[i-1]-2] < 0 && Name[i-1][p[i-1]-1] < 0){
                        mvwprintw(win, y, x - 2, "  ");            //清除按下按键产生的字符和需要删除的字符
                        wmove(win, y, x - 2);                     //光标复位
                        p[i-1] -= 3;                                   //字符串长度-1
                        psl[i-1] -= 2;                                   //字符串长度-1
                        Name[i-1][p[i-1]+2] = '\0';                   //删除最后一个字符
                        Name[i-1][p[i-1]+1] = '\0';                   //删除最后一个字符
                        Name[i-1][p[i-1]] = '\0';                   //删除最后一个字符
                    }
                    else{
                        p[i-1]--;                                   //字符串长度-1
                        psl[i-1]--;                                   //字符串长度-1
                        Name[i-1][p[i-1]] = '\0';                   //删除最后一个字符
                    }
                }
                break;
            case 46:
                if (IsInputDot == 1 && i == 13) break;
            default:
                getyx(win,y,x);                     //获取光标位置，储存在x和y中
                if (i == 13){
                    if (p[i-1] < 12) {
                        if (key == 46) IsInputDot = 1;
                        if ((key >= 48 && key <= 57) || key == 46) {
                            s[p[i-1]++] = (char)(key>=128?key-256:key);
                            mvwprintw(win, y, x, "%c%%", s[p[i-1]-1]);
                            psl[i-1]++;
                            wmove(win, y, x+1);
                            Np = str2dbl(s, 12);
                            Np = Np>100?100:Np;
                        }
                    }
                    break;
                }
                if (p[i-1] < 30) {
                    if (key == 32 || (key >= 48 && key <= 57) || (key >= 65 && key <= 90) || (key >= 97 && key <= 122) || key == 46 || key >= 128) {
                        if (key >= 128){
                            Chinese[IsInputChinese] = key;
                            IsInputChinese++;
                            if (p[i-1] + 3 < 30){
                                if (IsInputChinese == 3) {
                                    if (Chinese[0] == 226 && Chinese[1] == 128 && Chinese[2] == 166) {
                                        Chinese[0] = 77;
                                        Chinese[1] = 77;
                                        Chinese[2] = 80;
                                        for (size_t l = 0; l < 3; l++) {
                                            mvwprintw(win, y, x+(int)l, "%c", Chinese[l]);
                                            Name[i-1][p[i-1]+(int)l] = (char)Chinese[l];
                                            Chinese[l] = 0;
                                        }
                                    }
                                    else{
                                        for (size_t l = 0; l < 3; l++) {
                                            mvwprintw(win, y, x, "%c", Chinese[l]);
                                            Name[i-1][p[i-1]+(int)l] = (char)(Chinese[l]-256);
                                            Chinese[l] = 0;
                                        }
                                    }
                                    IsInputChinese = 0;
                                    p[i-1] += 3;
                                    psl[i-1] += 2;                                   //字符串长度-1
                                }
                                break;
                            }
                        }
                        else {
                            Name[i-1][p[i-1]] = (char)key;              //将输入的字符加入到名字中
                            mvwprintw(win, y, x, "%c" , key);
                            p[i-1]++;
                            psl[i-1]++;                                   //字符串长度-1
                        }
                    }
                }
                else {
                    IsInputChinese = 0;
                    Chinese[0] = 0;
                    Chinese[1] = 0;
                    Chinese[2] = 0;
                }
            break;
        }
        wrefresh(win);                                          //刷新窗口
    }
    end:                                                        //跳出位置
    return;
}

//投票函数
void WinSet(WINDOW *win, int SW) {
    int p[14];                                                  //名字字符串长度
    touchwin(win);
    mvwprintw(win, 1, SW/2 - 2, "%s", "请投票");                 //在屏幕上输出候选人的名字和投票勾选框
    mvwprintw(win, 2, 1, "%s", "共有12位候选人：");
    mvwprintw(win, 2, SW - 22, "%s", "请为你喜欢的候选人+1s");
    mvwprintw(win, 3, 1, "%s%d%s", "已有",Nscore,"人投票");
    for (size_t k = 0; k < 6; k++) {
        p[k*2] =longofstring(Name[k*2],1);                            //获取名字字符串长度
        p[k*2+1] =longofstring(Name[k*2+1],1);                            //获取名字字符串长度
        mvwprintw(win, 5 + (int)k * 3, 5, "%s", Name[k*2]);
        mvwprintw(win, 5 + (int)k * 3, 5 + p[k*2], ":[ ]");
        mvwprintw(win, 5 + (int)k * 3, SW / 2 + 5, "%s", Name[k*2+1]);
        mvwprintw(win, 5 + (int)k * 3, SW / 2 + 5 + p[k*2+1], ":[ ]");
    }
    mvwprintw(win, 23, 14, "%s", "<退出>");
    mvwprintw(win, 23, SW /2 + 14, "%s", "<下一个>");
    p[12] = 8;                                                  //设定选项的偏差值
    p[13] = 8;
    int key, i = 1, kx = 5, ky = 5 + p[0] + 2;                  //设定光标移动所需变量
    int setscore[12] = {0};                                     //设定本次的投票
    wmove(win, kx, ky);                                         //将光标移动到一号上
    keypad(win, TRUE);                                          //允许处理键盘输入值
    wrefresh(win);                                              //刷新窗口
    while(1){                                                   //获取键盘操作
        key = wgetch(win);                                      //从键盘读取值，类似于getchar()
        switch(key){                                            //判断按下的值
            case KEY_UP:                                        //按下方向键上
                if(i > 2){                                      //判断光标位置索引
                    i -= 2;
                    kx -= 3;
                    if (i % 2 == 0) {                           //判断光标纵向位置
                        ky = SW / 2 + 5 + p[i-1] + 2;
                    }
                    else {
                        ky = 5 + p[i-1] + 2;
                    }
                    wmove(win, kx, ky);                         //移动光标
                }
                break;
            case KEY_DOWN:                                      //按下方向键下
                if(i < 13){                                     //判断光标位置索引
                    i += 2;
                    kx += 3;
                    if (i % 2 == 0) {                           //判断光标纵向位置
                        ky = SW / 2 + 5 + p[i-1] + 2;
                    }
                    else {
                        ky = 5 + p[i-1] + 2;
                    }
                    wmove(win, kx, ky);                         //移动光标
                }
                break;
            case 353:                                           //按下Shift+TAB
            case KEY_LEFT:                                      //按下方向键左
                if (i > 1) {                                    //判断光标位置索引
                    if((i % 2) == 0){                           //判断光标纵向位置
                        i -= 1;
                        ky = 5 + p[i-1] + 2;
                        wmove(win, kx, ky);                     //移动光标
                    }
                    else{
                        i -= 1;
                        kx -= 3;
                        ky = SW / 2 + 5 + p[i-1] + 2;
                        wmove(win, kx, ky);                     //移动光标
                    }
                }
                break;
            case 9:                                             //按下TAB
            case KEY_RIGHT:                                     //按下方向键右
                if (i < 14){                                    //判断光标位置索引
                    if((i % 2) == 0){                           //判断光标纵向位置
                        i += 1;
                        ky = 5 + p[i-1] + 2;
                        kx += 3;
                        wmove(win, kx, ky);                     //移动光标
                    }
                    else{
                        i += 1;
                        ky = SW / 2 + 5 + p[i-1] + 2;
                        wmove(win, kx, ky);                     //移动光标
                    }
                }
                break;
            case 27:
                i = 13;
                ky = 5 + p[i-1] + 2;
                kx = 23;
                wmove(win, kx, ky);                             //移动光标
            case 10:                                            //按下回车
                if (i == 13) {                                  //如果光标在<退出>上
                    WINDOW *sure;                               //新建警告窗口
                    sure = newwin(8, 30, WinY / 2 - 4, WinX / 2 - 15);//确定窗口大小和位置
                    box(sure, 0, 0);                            //绘制边框
                    mvwprintw(sure, 2, 8, "%s", "确定要退出吗？"); //输出确认信息
                    for (size_t n = 0; n < 12; n++) {           //检索残余的数据
                        if (setscore[n] != 0) {                 //如果有投票
                        mvwprintw(sure, 3, 7, "%s", "本次投票尚未保存！");//输出多一句警告
                        }
                    }
                    mvwprintw(sure, 5, 4, "%s","<取消>");        //绘制"取消"键
                    mvwprintw(sure, 5, 20, "%s","<确定>");       //绘制"确定"键
                    touchwin(sure);                             //警告窗口获取焦点
                    wrefresh(win);                              //刷新父窗口
                    wmove(sure, 5, 5);                          //移动光标到<取消>上
                    keypad(sure, TRUE);                         //允许处理键盘输入值
                    wrefresh(sure);                             //刷新窗口
                    int si = 0;                                 //光标位置索引
                    while (1) {                                 //获取键盘操作
                        key = wgetch(win);                      //从键盘读取值，类似于getchar()
                        switch(key) {                           //判断按下的值
                            case KEY_LEFT:                      //按下方向键左
                                if (si == 1) {                  //判断光标位置索引
                                    wmove(sure, 5, 5);          //移动光标到<取消>上
                                    si = 0;                     //设置光标索引在<取消>上
                                }
                                break;
                            case KEY_RIGHT:                     //按下方向键右
                                if (si == 0) {                  //判断光标位置索引
                                    wmove(sure, 5, 21);         //移动光标到<确定>上
                                    si = 1;                     //设置光标索引在<确定>上
                                }
                                break;
                            case 10:                            //按下回车
                                delwin(sure);               //删除窗口
                                refresh();                  //刷新终端窗口
                                wrefresh(win);              //刷新父窗口
                                if (si == 1) {                  //判断光标位置索引在<确定>上
                                    goto end;                   //跳出整个函数
                                }
                                goto endsure;                   //跳出<退出>窗口判定
                                break;
                        }
                        wrefresh(sure);                         //每更改一次光标位置就刷新一次
                    }
                    endsure:                                    //<退出>窗口跳出点
                    touchwin(win);                              //将焦点还给
                    wmove(win, kx, ky);                         //还原光标位置
                    break;
                }
                if (i == 14) {                                  //如果光标在<下一个>上
                    Nscore++;                                   //投票人递加1
                    for (size_t i = 0; i < 12; i++) {           //检索投票情况
                        if (setscore[i] == 1) score[i]++;       //如果投了他一票
                        ky = (i % 2 == 0) ?  5 + p[i] + 2 : SW / 2 + 5 + p[i] + 2 ;//确定复选框位置
                        kx = (int)(i / 2)*3 + 5;
                        setscore[i] = 0;                        //清除投票信息
                        mvwprintw(win, kx, ky, " ");            //清除屏幕信息
                    }
                    i = 1;                                      //初始化光标索引
                    kx = 5;                                     //初始化光标纵向位置
                    ky = 5 + p[0] + 2;                          //初始化光标横向位置
                    mvwprintw(win, 3, 1, "%s%d%s", "已有",Nscore,"人投票");//更新投票信息
                    wmove(win, kx, ky);                         //初始化光标位置
                    break;
                }                                               //如果光标不在操作按钮上
                case 32:
                if (i >= 13) break;
                if (setscore[i-1] == 0) {                       //判断是否已投票
                    setscore[i-1] = 1;                          //进行投票
                    mvwprintw(win, kx, ky, "Y");                //输出到屏幕
                    wmove(win, kx, ky);                         //复位光标
                }
                else {                                          //如果已投票
                    setscore[i-1] = 0;                          //取消投票
                    mvwprintw(win, kx, ky, " ");                //清除屏幕信息
                    wmove(win, kx, ky);                         //复位光标
                }
                break;
        }
        wrefresh(win);                                          //刷新窗口
    }
    end:                                                        //全函数跳出点
    refresh();                                                  //刷新终端窗口
    wrefresh(win);                                              //刷新主窗口
    return;
}

//投票统计函数
void WinShow(WINDOW *win, int SW) {
    WINDOW *cwin[5];                                            //创建信息窗口
    double p = 0;
    touchwin(win);                                              //窗口获取焦点
    mvwprintw(win, 1, SW/2 - 3, "%s", "投票结果");               //输出标题
    mvwprintw(win, 2, (SW-70)/2, "%s%d%s", "总共", Nscore, "人投票");     //输出投票信息
    mvwprintw(win, 2, (SW-70)/2 + 54, "%s%-.4g%s", "得票比例", Np, "%即通过");     //输出投票信息
    cwin[0] = newwin(15, 14, 5, (SW-74)/2 + 25);                            //信息窗口大小和位置
    cwin[1] = newwin(15, 34, 5, (SW-74)/2 + 38);
    cwin[2] = newwin(15, 8, 5, (SW-74)/2 + 71);
    cwin[3] = newwin(15, 10, 5, (SW-74)/2 + 78);
    cwin[4] = newwin(15, 12, 5, (SW-74)/2 + 87);
    for (size_t n = 0; n < 5; n++) {
        box(cwin[n], 0, 0);                                     //绘制边框
    }
    mvwprintw(cwin[0], 1, 2, "%s", "候选者编号");                //输出信息窗口标题
    mvwprintw(cwin[1], 1, 14, "%s", "姓名");
    mvwprintw(cwin[2], 1, 2, "%s", "票数");
    mvwprintw(cwin[3], 1, 2, "%s", "得票率");
    mvwprintw(cwin[4], 1, 2, "%s", "是否中选");
    for (size_t i = 0; i < 12; i++) {                           //检索投票结果
        if (i<9) mvwprintw(cwin[0], 2 + (int)i, 6, "%d", i+1);  //判断编号长度
        else mvwprintw(cwin[0], 2 + (int)i, 5, "%d", i+1);
        mvwprintw(cwin[1], 2 + (int)i, 2, "%s", Name[i]);       //输出候选人姓名
        mvwprintw(cwin[2], 2 + (int)i, 3, "%d", score[i]);      //输出候选人得票
        if (Nscore != 0) {                                      //判断候选人得票
            p = score[i]*100/(double)Nscore;             //将得票百分比储存到p中
            if ((int)p == 100) {                                //判断全票的人，避免输出1x10%2%
                mvwprintw(cwin[3], 2 + (int)i, 3, "100%%");     //输出100%
            }
            else mvwprintw(cwin[3], 2 + (int)i, 3, "%.3g%%", p);//输出得票比例
            if (p >= Np) mvwprintw(cwin[4], 2 + (int)i, 3, "%s", "True");//判断是否达标
            else mvwprintw(cwin[4], 2 + (int)i, 3, "%s", "False");
        }
        else{                                                   //如果没有得票
            mvwprintw(cwin[3], 2 + (int)i, 3, "0%%");           //输出0%
            mvwprintw(cwin[4], 2 + (int)i, 3, "%s", "False");   //输出False
        }
    }
    wrefresh(win);                                              //刷新窗口
    for (size_t n = 0; n < 5; n++) {                            //循环销毁数据窗口
        wrefresh(cwin[n]);                                      //刷新数据窗口
        delwin(cwin[n]);                                        //输出数据窗口
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
    endwin();                                                   //释放终端
    if (WinY < 32 || WinX < 101) {
        printf("终端大小不够(%dx%d)，建议大于(101x31) , 是否继续?[Y/N] ", WinX, WinY);
        scanf("%c", &inp);
        if (inp != 'y' && inp != 'Y')  exit(1);
        scanf("%c", &inp);
    }
    inp = '\0';
    FILE *fr;                                                       //定义读取文件
    if (argc >= 2) {                                                //如果有传入变量
        for (size_t i = 1; (int)i < argc; i++) {                    //遍历字符串
            if ((fr = fopen(argv[i], "r")) != NULL) {               //如果字符串是文件且存在能打开
                printf("是否确定读取文件%s? [Y/N] ", argv[i]);         //询问用户是否读取该文件
                scanf("%c", &inp);                                  //获取用户输入
                if (inp == 'y' || inp == 'Y') {                     //判断以后输入值
                    for (size_t k = 0; k < 12; k++) {               //遍历文件数据并格式化读取
                        fscanf(fr, "%s\t%d\n", Name[k], &score[k]);
                        if (strcmp(Name[k], "(NULL)") == 0) {
                            for (size_t n = 0; n <= 5; n++) {
                                Name[k][n] = '\0';
                            }
                        }
                        else{
                            for (size_t l = 0; l < 30; l++) {
                                if (Name[k][l] == '_') Name[k][l] = ' ';
                                if (Name[k][l] == '\0') break;
                            }
                        }
                    }
                    fscanf(fr, "%d|%lf", &Nscore, &Np);
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
    win[0]=newwin(10,21,1,3);                                        //设定选择窗口的大小和位置
    win[1]=newwin(WinY - 2, WinX - 25,1,24);                              //设定操作窗口的大小和位置
    W1 = WinX - 25;                                                    //储存操作窗口的大小
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
    do {
        p = 0;
        getmaxyx(stdscr,y,x);
        if (WinX != x || WinY != y) {
            WinX = x;
            WinY = y;
            wresize(win[1], y - 2, x - 25);
            refresh();                                                      //刷新终端
        }
        W1 = WinX - 27;                                                    //储存操作窗口的大小
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
        wclear(win[1]);                                             //清除操作窗口
        box(win[1],0,0);                                            //为操作窗口重新设定边框
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
                    WinCU(win[1], W1, H1, 0);
                break;
                case 2:                                             //用户输入修改
                    WinCU(win[1], W1, H1, 1);
                break;
                case 3:                                             //用户输入投票
                    WinSet(win[1], W1);
                break;
                case 4:                                             //用户输入显示
                    WinShow(win[1], W1);
                break;
                case 5:                                             //用户输入保存
                    savefile(win[1], W1, H1);
                    WinShow(win[1], W1);
                    break;
                case 6:                                             //用户输入帮助
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
