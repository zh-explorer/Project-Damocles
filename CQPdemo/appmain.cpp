/*
* CoolQ Demo for VC++ 
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "string"
#include "cqp.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "appmain.h" //应用AppID等信息，请正确填写，否则酷Q可能无法加载
#include <time.h>

#include "C:\\Python27\include\Python.h"
#pragma comment(lib,"C:\\Python27\\libs\\python27.lib")
using namespace std;

typedef struct {
	char **word;
	int len;
}keyword;

int flag;

char *welcome[] = { "我们的征途是星辰与大海！\n","溜金哇啦啊酷咧！\n","さあ、ステキなパーティしましょ！\n","大黑客挂了要重修\n","土土挂了大物\n","竜が我が敵を喰らえ!\n","c语言我只服c primer plus\n","土土没过四级\n","Just hack for fun\n","As we do, as you know \n" };
int lenWelcode = sizeof(welcome) / 4;
CRITICAL_SECTION  _critical;

int ac = -1; //AuthCode 调用酷Q的方法时需要用到
bool enabled = false;
PyObject *pModule;
void checkImage(int64_t fromGroup, int64_t fromQQ, const char *msg);
/* 
* 返回应用的ApiVer、Appid，打包后将不会调用
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/* 
* 接收应用AuthCode，酷Q读取应用信息后，如果接受该应用，将会调用这个函数并传递AuthCode。
* 不要在本函数处理其他任何代码，以免发生异常情况。如需执行初始化代码请在Startup事件中执行（Type=1001）。
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 酷Q启动
* 无论本应用是否被启用，本函数都会在酷Q启动后执行一次，请在这里执行应用初始化代码。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventStartup, 0)() {

	return 0;
}


/*
* Type=1002 酷Q退出
* 无论本应用是否被启用，本函数都会在酷Q退出前执行一次，请在这里执行插件关闭代码。
* 本函数调用完毕后，酷Q将很快关闭，请不要再通过线程等方式执行其他代码。
*/
CQEVENT(int32_t, __eventExit, 0)() {

	return 0;
}

/*
* Type=1003 应用已被启用
* 当应用被启用后，将收到此事件。
* 如果酷Q载入时应用已被启用，则在_eventStartup(Type=1001,酷Q启动)被调用后，本函数也将被调用一次。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	enabled = true;
	Py_SetPythonHome("C:\\python27");
	Py_Initialize();
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('C:\\module')");
	pModule = PyImport_ImportModule("CQTools");
	InitializeCriticalSection(&_critical);
	return 0;
}


/*
* Type=1004 应用将被停用
* 当应用被停用前，将收到此事件。
* 如果酷Q载入时应用已被停用，则本函数*不会*被调用。
* 无论本应用是否被启用，酷Q关闭前本函数都*不会*被调用。
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	enabled = false;
	Py_Finalize();
	DeleteCriticalSection(&_critical);
	return 0;
}


/*
* Type=21 私聊消息
* subType 子类型，11/来自好友 1/来自在线状态 2/来自群 3/来自讨论组
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, int32_t font) {
	if (fromQQ == 87294982) {
		char *getImage = "[CQ:image,file=";
		char *get = strstr((char *)msg, getImage);
		if (get != NULL) {
			checkImage(0, fromQQ, msg);
		}
	}
	//如果要回复消息，请调用酷Q方法发送，并且这里 return EVENT_BLOCK - 截断本条消息，不再继续处理  注意：应用优先级设置为"最高"(10000)时，不得使用本返回值
	//如果不回复消息，交由之后的应用/过滤器处理，这里 return EVENT_IGNORE - 忽略本条消息
	return EVENT_IGNORE;
}

void News(int64_t fromGroup) {
	PyObject *pFunc,*pRet;
	char * bp;
	pFunc = PyObject_GetAttrString(pModule, "getNews");
	pRet = PyObject_CallObject(pFunc, NULL);
	if (pRet == NULL) {
		bp = "[CQ:at,qq=87294982]每日推送获取失败啦，快去修！！";
	}
	else {
		bp = PyString_AsString(pRet);
		Py_DECREF(pRet);
	}
	CQ_sendGroupMsg(ac, fromGroup, bp);
	Py_DECREF(pFunc);
}

void recentNews(int64_t fromGroup) {
	PyObject *pFunc, *pRet;
	char * bp;
	pFunc = PyObject_GetAttrString(pModule, "getRecentNews");
	pRet = PyObject_CallObject(pFunc, NULL);
	if (pRet == NULL) {
		bp = "[CQ:at,qq=87294982]每日推送获取失败啦，快去修！！";
	}
	else {
		bp = PyString_AsString(pRet);
		Py_DECREF(pRet);
	}
	CQ_sendGroupMsg(ac, fromGroup, bp);
	Py_DECREF(pFunc);
}
void startGetNews(int64_t fromGroup) {
	PyObject *pFunc, *pRet;
	char * bp;
	pFunc = PyObject_GetAttrString(pModule, "getDailyNews");
	pRet = PyObject_CallObject(pFunc, NULL);
	if (pRet == NULL) {
		bp = "[CQ:at,qq=87294982]每日推送获取失败啦，快去修！！！";
	}
	else {
		bp = PyString_AsString(pRet);
		Py_DECREF(pRet);
	}
	CQ_addLog(ac, 1, "test", bp);
	if (strcmp(bp,"null")) {
		CQ_sendGroupMsg(ac, fromGroup, bp);
	}
	Py_DECREF(pFunc);
}
void adminCmd(int64_t fromGroup, const char * msg) {
	int64_t QQId;
	char *t;
	long int ti;
	char *bp = (char *)malloc(0x1000);
	if (strncmp(msg, "ban:", 4) == 0) {
		QQId = strtoll(&msg[4],&t,10);
		ti = atoi(t+1);
		flag = 1;
		while (flag) {
			CQ_setGroupBan(ac, fromGroup, QQId, 60 * ti);
			Sleep(1000 * 60 * ti);
		}
	}
	if (strncmp(msg, "unban", 6) == 0) {
		flag = 0;
	}
	if (strncmp(msg, "news", 4) == 0) {
		while (1) {
			startGetNews(fromGroup);
			Sleep(1000 * 60 * 60);
		}
	}
	if (strncmp(msg, "at:", 3) == 0) {
		QQId = atoll(&msg[3]);
		sprintf(bp, "[CQ:at,qq=%lld] hello", QQId);
		CQ_sendGroupMsg(ac, fromGroup, bp);
	}
	free(bp);
}

int64_t times[200];

void requestAt(int64_t fromGroup, int64_t fromQQ, const char *msg) {
	char *bp = (char *)malloc(0x1000);
	for (int i = 0; i < 200; i++) {
		if (times[i * 2] == fromQQ) {
			times[i * 2 + 1]++;
			if (times[i * 2 + 1] == 5) {
				times[i * 2 + 1] = 0;
				CQ_setGroupBan(ac, fromGroup, fromQQ, 3600);
				sprintf(bp, "[CQ:at,qq=%lld] 老是找我是想干啥？", fromQQ);
				CQ_sendGroupMsg(ac, fromGroup, bp);
				return;
			}
			break;
		}
		else if (times[i * 2] == 0) {
			times[i * 2] = fromQQ;
			break;
		}
	}
	//sprintf(bp, "[CQ:at,qq=%lld] 找我干什么", fromQQ);
	//CQ_sendGroupMsg(ac, fromGroup, bp);
	free(bp);
}

void checkImage(int64_t fromGroup, int64_t fromQQ,const char *msg) {
	PyObject *pFunc,*pArg,*pRet;
	int time;
	char *bp = (char *)malloc(0x1000);
	pFunc = PyObject_GetAttrString(pModule, "aliCheck");
	pArg = Py_BuildValue("(s)", msg);
	pRet = PyObject_CallObject(pFunc, pArg);
	time = PyInt_AsLong(pRet);
	if (time == -1) {
		CQ_addLog(ac, 1, "test", "error");
	}
	else if (time != 0) {
		CQ_setGroupBan(ac, fromGroup, fromQQ, time);
		sprintf(bp, "[CQ:at,qq=%lld] 都说了多少次了，我们是正规群！", fromQQ);
		CQ_sendGroupMsg(ac, fromGroup, bp);
		Py_DECREF(pRet);
	}
	free(bp);
	Py_DECREF(pFunc);
	Py_DECREF(pArg);
	
}
char *(keyIsa[]) = {"安协","协会","信息安全协会"};
char *(keyWhere[]) = { "哪","位置","地址","怎么去","怎么走","咋去"};
int lenIsa = sizeof(keyIsa) / 4;
int lenWhere = sizeof(keyWhere) / 4;
keyword where[2] = { {keyIsa,lenIsa},{keyWhere,lenWhere} };

char *(keyWhat[]) = { "c语言","C语言","编程","黑客","信安","安全", "c", "C"};
int lenWhat = sizeof(keyWhat) / 4;
char *(keyHow[]) = { "什么","怎么","应该","如何","哪些","有没有" ,"有关", "想","教","咋","么?","么？","呢","吗","没？","没?" };
int lenHow = sizeof(keyHow) / 4;
char *(keyLearn[]) = { "学","了解","书","写","看","入门", "当", "做"};
int lenLearn = sizeof(keyLearn) / 4;
keyword learn[] = { { keyWhat,lenWhat },{ keyHow,lenHow },{ keyLearn,lenLearn } };

char *(keyHack[]) = {"日","拿","黑","入侵","攻击","拖","盗","刷"};
int lenHack = sizeof(keyHack) / 4;
char *(keyWeb[]) = { "数据","站","杭电","官网", "库" ,"QQ","qq","钻","会员","号","挂"};
int lenWeb = sizeof(keyWeb) / 4;
keyword hack[] = { { keyHack ,lenHack },{ keyWeb ,lenWeb }, { keyHow ,lenHow } };

char *(keyPersion[]) = {"人"};
int lenPersion = sizeof(keyPersion) / 4;
keyword persion[] = { { keyHow,lenHow },{ keyPersion,lenPersion },{ keyIsa,lenIsa } };

int checkExist(keyword *key, const char *msg, int len) {
	int i;
	for (i = 0; i < key[len - 1].len; i++) {
		if (strstr(msg, key[len-1].word[i])) {
			if (len != 1) {
				return checkExist(key, msg, len - 1);
			}
			else {
				return 1;
			}
		}
	}
	return 0;
}

void checkWord(int64_t fromGroup, int64_t fromQQ, const char *msg) {
	char *bp = (char *)malloc(0x1000);
	struct tm *local;
	time_t t;
	if (checkExist(where, msg, 2)) {
		sprintf(bp, "[CQ:at,qq=%lld] 如果你是想问信息安全协会地址的话。是在一教（信仁楼）106。\n欢迎随时过来[CQ:face,id=21]", fromQQ);
		CQ_sendGroupMsg(ac, fromGroup, bp);
	}
	if (checkExist(learn, msg, 3)) {
		sprintf(bp, "[CQ:at,qq=%lld] 如果想入门的话，还是要以c语言为基础。\n至于学习c语言最有效的还是看C primer plus。http://t.cn/R5eqrV2 \nPS: 最好不要看谭浩强，XX天精通或者是从入门到精通系列 [CQ:face,id=21]", fromQQ);
		CQ_sendGroupMsg(ac, fromGroup, bp);
	}
	if (checkExist(hack, msg, 3)) {
		sprintf(bp, "[CQ:at,qq=%lld] 国家刑法第二百八十六条规定，\n关于恶意利用计算机犯罪相关条文对于违反国家规定，对计算机信息系统功能进行删除、修改、增加、干扰，造成计算机信息系统不能正常运行，后果严重的，处五年以下有期徒刑或者拘役；后果特别严重的，处五年以上有期徒刑。\n违反国家规定，对计算机信息系统中存储、处理或者传输的数据和应用程序进行删除、修改、增加的操作，后果严重的，依照前款的规定处罚。", fromQQ);
		CQ_sendGroupMsg(ac, fromGroup, bp);
	}
	if (checkExist(persion, msg, 3)) {
		t = time(NULL);
		local = localtime(&t);
		char timeb[200];
		if (local->tm_hour <= 6) {
			sprintf(bp, "[CQ:at,qq=%lld] 这么早估计没什么人起来", fromQQ);
		}
		else if (local->tm_hour >= 7 && local->tm_hour <= 9) {
			sprintf(bp, "[CQ:at,qq=%lld] 大概都在睡觉吧，不知道今天有没有人早起", fromQQ);
		}
		else if (local->tm_hour >= 10 && local->tm_hour <= 11) {
			sprintf(bp, "[CQ:at,qq=%lld] 这点应该有人开门了", fromQQ);
		}
		else if (local->tm_hour == 12) {
			sprintf(bp, "[CQ:at,qq=%lld] 不清楚，没有集体去吃午饭的话应该有人", fromQQ);
		}
		else if (local->tm_hour >= 13 && local->tm_hour <= 16) {
			sprintf(bp, "[CQ:at,qq=%lld] 下午一般都有人在的", fromQQ);
		}
		else if (local->tm_hour >= 17 && local->tm_hour <= 18) {
			sprintf(bp, "[CQ:at,qq=%lld] 不清楚，没有集体去吃晚饭的话应该有人", fromQQ);
		}
		else if (local->tm_hour >= 19 && local->tm_hour <= 21) {
			sprintf(bp, "[CQ:at,qq=%lld] 人应该都没回去呢", fromQQ);
		}
		else if (local->tm_hour >= 22 && local->tm_hour <= 23) {
			sprintf(bp, "[CQ:at,qq=%lld] 这么晚了，除非有人通宵，不然应该没人了", fromQQ);
		}
		else {
			sprintf(bp, "[CQ:at,qq=87294982] 你代码bug了", fromQQ);
		}
		sprintf(timeb, " %02d:%02d", local->tm_hour, local->tm_min);
		strcat(bp, timeb);
		CQ_sendGroupMsg(ac, fromGroup, bp);
	}
	free(bp);
}
void checkWord1(int64_t fromGroup, int64_t fromQQ, const char *msg) {
	int i;
	char *get;
	char *bp = (char *)malloc(0x1000);
	for (i = 0; i < lenIsa; i++) {
		if (strstr((char *)msg, keyIsa[i])) {
			break;
		}
	}
	if (i != lenIsa) {
		for (i = 0; i < lenWhere; i++) {
			if (strstr((char *)msg, keyWhere[i])) {
				break;
			}
		}
		if (i != lenWhere) {
			sprintf(bp, "[CQ:at,qq=%lld] 如果你是想问信息安全协会地址的话。是在一教（信仁楼）106。\n欢迎随时过来[CQ:face,id=21]", fromQQ);
			CQ_sendGroupMsg(ac, fromGroup, bp);
		}
		free(bp);
	}
}
/*
* Type=2 群消息
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {
	if (fromGroup == 555091662 && fromQQ == 87294982 && *msg == '$') {
		adminCmd(fromGroup,&msg[1]);
	}
	if (fromGroup == 555091662) {
		char *atMe = "[CQ:at,qq=858325880]";
		char *at = strstr((char *)msg, atMe);

		if (strstr(msg, "禁言我") || strstr(msg, "求禁言") || 
			(strstr(msg, "求") && strstr(msg, "禁言") && (msg, "我"))) {

			srand((unsigned)time(NULL));
			// talk is cheap, show me your face~ :)
			CQ_setGroupBan(ac, fromGroup, fromQQ, rand() % (6 * 3600));
			CQ_sendGroupMsg(ac, fromGroup, "来呀！互相伤害啊！");
		}

		if (at != NULL) {
			requestAt(fromGroup, fromQQ, msg);
		}
	}
	if (fromGroup == 555091662) {
		char *getImage = "[CQ:image,file=";
		char *get = strstr((char *)msg, getImage);
		if (get != NULL) {
			EnterCriticalSection(&_critical);
			checkImage(fromGroup, fromQQ, msg);
			LeaveCriticalSection(&_critical);
		}
	}
	if (fromGroup == 555091662 ) {
		checkWord(fromGroup, fromQQ, msg);
	}

	if (fromGroup == 555091662 || fromGroup == 536559442) {
		if (!strcmp(msg, "每日消息推送") || !strcmp(msg, "每日新闻") || !strcmp(msg, "今日新闻")) {
			News(fromGroup);
		}
	}

	if (fromGroup == 555091662 || fromGroup == 536559442) {
		if (!strcmp(msg, "近期消息推送") || !strcmp(msg, "最近消息推送") || !strcmp(msg, "最近新闻")) {
			recentNews(fromGroup);
		}
	}

	
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=4 讨论组消息
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t sendTime, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=101 群事件-管理员变动
* subType 子类型，1/被取消管理员 2/被设置管理员
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=102 群事件-群成员减少
* subType 子类型，1/群员离开 2/群员被踢 3/自己(即登录号)被踢
* fromQQ 操作者QQ(仅subType为2、3时存在)
* beingOperateQQ 被操作QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=103 群事件-群成员增加
* subType 子类型，1/管理员已同意 2/管理员邀请
* fromQQ 操作者QQ(即管理员QQ)
* beingOperateQQ 被操作QQ(即加群的QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {
	char *bp = (char *)malloc(0x1000);
	if (fromGroup == 555091662) {
		srand(time(NULL));
		int index = rand() % lenWelcode;
		sprintf(bp, "[CQ:at,qq=%lld] 欢迎加入信息安全协会2016届新生群\n请先阅读以下事项：\n1、协会官网: http://hduisa.org \nwiki：http://t.cn/R5BI2h5 \ndrops：http://t.cn/R5BILcO \n2、协会简介请移步：http://t.cn/R5BIyba \n3、如有任何疑问，请在群里艾特管理员提问 \n PS:%s", beingOperateQQ, welcome[index]);
		CQ_sendGroupMsg(ac, fromGroup, bp);
	}
	if (fromGroup == 198508284) {
		sprintf(bp, "欢迎加入杭州电子科技大学2016级网络空间安全学院新生群。\n为了让大家更好的相互了解，请先更改一下群名片。\n备注格式为专业-省份-姓名\nPS:自觉爆照哦[CQ:face,id=21]", beingOperateQQ);
		CQ_sendGroupMsg(ac, fromGroup, bp);
	}
	free(bp);
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=201 好友事件-好友已添加
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=301 请求-好友添加
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=302 请求-群添加
* subType 子类型，1/他人申请入群 2/自己(即登录号)受邀入群
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else if (subType == 2) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
	//}

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}

/*
* 菜单，可在 .json 文件中设置菜单数目、函数名
* 如果不使用菜单，请在 .json 及此处删除无用菜单
*/
CQEVENT(int32_t, __menuA, 0)() {
	MessageBoxA(NULL, "这是menuA，在这里载入窗口，或者进行其他工作。", "" ,0);
	return 0;
}

CQEVENT(int32_t, __menuB, 0)() {
	MessageBoxA(NULL, "这是menuB，在这里载入窗口，或者进行其他工作。", "" ,0);
	return 0;
}
