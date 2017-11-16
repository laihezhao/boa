#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include <time.h>
#include <unistd.h>

#include "cgic.h"
#include "prefile.h"

#define TABLENAME "siteconfig"
#define USERNAME "admin"
#define PASSWORD "123456"
#define MAX_BUF_VALUE 100

static sqlite3 *db;

static char *cvalue[100];

static char sql_MN[MAX_BUF_VALUE];
static char sql_PW[MAX_BUF_VALUE];
static char sql_SITETYPE[MAX_BUF_VALUE];
static char sql_CENTER1IP[MAX_BUF_VALUE];
static char sql_CENTER1PORT[MAX_BUF_VALUE];
static char sql_CENTER2IP[MAX_BUF_VALUE];
static char sql_CENTER2PORT[MAX_BUF_VALUE];
static char sql_CONNECT2CENTER1[MAX_BUF_VALUE];
static char sql_CONNECT2CENTER2[MAX_BUF_VALUE];
static char sql_SAMPLEINTERVAL[MAX_BUF_VALUE];
static char sql_OVERTIME[MAX_BUF_VALUE];
static char sql_RECOUNT[MAX_BUF_VALUE];
static char sql_DINUMS[MAX_BUF_VALUE];
static char sql_AINUMS[MAX_BUF_VALUE];
static char sql_COMNUMS[MAX_BUF_VALUE];
static char sql_APN[MAX_BUF_VALUE];
static char sql_DIALNUMBER[MAX_BUF_VALUE];
static char sql_DIALUSER[MAX_BUF_VALUE];
static char sql_DIALPASSWORD[MAX_BUF_VALUE];
static char sql_RTDINTERVAL[MAX_BUF_VALUE];
static char sql_RSINTERVAL[MAX_BUF_VALUE];

static void init_memory(void);
static void show_form(void);
static int open_database(void);
static int close_database(void);
static int select_table(void);
static int update_table(void);
static void reset_str(void);
static void show_login(void);

static void store_form_value(void);
static void get_form_value(char *,char *,int);

static int select_callback(void *,int ,char **,char **);
static int update_callback(void *,int ,char **,char **);

int cgiMain(void)
{
	char user[32];
	char passwd[32];

	bzero(user,  sizeof(user));
	bzero(passwd,sizeof(passwd));

	cgiHeaderContentType("text/html");

	get_form_value("user",  user,  sizeof(user));
	get_form_value("passwd",passwd,sizeof(passwd));

	open_database();

	if (cgiFormSubmitClicked("login") == cgiFormSuccess) {
		if (strcmp(user,USERNAME) != 0 || strcmp(passwd,PASSWORD) != 0) {
			show_login();
			return -1;
		}
	}
	/*judge the submit button*/
	if (cgiFormSubmitClicked("submit") == cgiFormSuccess) {
		init_memory();
		update_table();
		show_form();
	}else{
		init_memory();
		select_table();
		show_form();
	}	

	close_database();

	return 0;
}

static void init_memory(void)
{
	memset(sql_MN,		   0, sizeof(sql_MN));
	memset(sql_PW,		   0, sizeof(sql_PW));
	memset(sql_SITETYPE,       0, sizeof(sql_SITETYPE));
	memset(sql_CENTER1IP,      0, sizeof(sql_CENTER1IP));
	memset(sql_CENTER1PORT,    0, sizeof(sql_CENTER1PORT));
	memset(sql_CENTER2IP,      0, sizeof(sql_CENTER2IP));
	memset(sql_CONNECT2CENTER1,0, sizeof(sql_CONNECT2CENTER1));
	memset(sql_CONNECT2CENTER2,0, sizeof(sql_CONNECT2CENTER2));
	memset(sql_SAMPLEINTERVAL, 0, sizeof(sql_SAMPLEINTERVAL));
	memset(sql_OVERTIME,       0, sizeof(sql_OVERTIME));
	memset(sql_RECOUNT,        0, sizeof(sql_RECOUNT));
	memset(sql_DINUMS,         0, sizeof(sql_DINUMS));
	memset(sql_AINUMS,         0, sizeof(sql_AINUMS));
	memset(sql_COMNUMS,        0, sizeof(sql_COMNUMS));
	memset(sql_APN,            0, sizeof(sql_APN));
	memset(sql_DIALNUMBER,     0, sizeof(sql_DIALNUMBER));
	memset(sql_DIALUSER,       0, sizeof(sql_DIALUSER));
	memset(sql_DIALPASSWORD,   0, sizeof(sql_DIALPASSWORD));
	memset(sql_RTDINTERVAL,    0, sizeof(sql_RTDINTERVAL));
	memset(sql_RSINTERVAL,     0, sizeof(sql_RSINTERVAL));
}


static void store_form_value(void)
{
	get_form_value("MN",		 sql_MN, 	     sizeof(sql_MN));
	get_form_value("PW",		 sql_PW, 	     sizeof(sql_PW));
	get_form_value("SiteType",	 sql_SITETYPE, 	     sizeof(sql_SITETYPE));
	get_form_value("center1ip", 	 sql_CENTER1IP,      sizeof(sql_CENTER1IP));
	get_form_value("center1port", 	 sql_CENTER1PORT,    sizeof(sql_CENTER1PORT));
	get_form_value("center2ip",      sql_CENTER2IP,      sizeof(sql_CENTER2IP));
	get_form_value("center2port",    sql_CENTER2PORT,    sizeof(sql_CENTER2PORT));
	get_form_value("connect2center1",sql_CONNECT2CENTER1,sizeof(sql_CONNECT2CENTER1));
	get_form_value("connect2center2",sql_CONNECT2CENTER2,sizeof(sql_CONNECT2CENTER2));
	get_form_value("sampleinterval", sql_SAMPLEINTERVAL, sizeof(sql_SAMPLEINTERVAL));
	get_form_value("overtime",       sql_OVERTIME,       sizeof(sql_OVERTIME));
	get_form_value("recount",        sql_RECOUNT,        sizeof(sql_RECOUNT));
	get_form_value("dinums",         sql_DINUMS,         sizeof(sql_DINUMS));
	get_form_value("ainums",         sql_AINUMS,         sizeof(sql_AINUMS));
	get_form_value("comnums",        sql_COMNUMS,        sizeof(sql_COMNUMS));
	get_form_value("apn",            sql_APN,            sizeof(sql_APN));
	get_form_value("dialnumber",     sql_DIALNUMBER,     sizeof(sql_DIALNUMBER));
	get_form_value("dialuser",       sql_DIALUSER,       sizeof(sql_DIALUSER));
	get_form_value("dialpassword",   sql_DIALPASSWORD,   sizeof(sql_DIALPASSWORD));
	get_form_value("rtdinterval",    sql_RTDINTERVAL,    sizeof(sql_RTDINTERVAL));
	get_form_value("rsinterval",     sql_RSINTERVAL,     sizeof(sql_RSINTERVAL));
}

static void get_form_value(char *src,char *dest,int max)
{
	cgiFormString(src,dest,max);
}

static int open_database(void)
{
	int ret;

	ret = sqlite3_open(DATABASE,&db);
	if (ret != SQLITE_OK) {
		fprintf(stderr,"open database failure :%s\n",sqlite3_errmsg(db));
		return -1;
	}
	
	return 0;
}

static int close_database(void)
{
	int ret;

	ret = sqlite3_close(db);
	if (ret != SQLITE_OK) {
		fprintf(stderr,"close database failure :%s\n",sqlite3_errmsg(db));
		return -1;
	}

	return 0;
}

static int update_callback(void *data,int argc,char **argv,char **azColName)
{
	return 0;
}


static int select_callback(void *data,int argc,char **argv,char **azColName)
{	
	if (strcmp(argv[1],"MN") == 0) {
		strcpy(sql_MN,		  	 argv[2] ? argv[2] : "");
	}

	if (strcmp(argv[1],"PW") == 0) {
		strcpy(sql_PW,		   	 argv[2] ? argv[2] : "");
	}
	
	if (strcmp(argv[1],"SiteType") == 0) {
		strcpy(sql_SITETYPE,	   	argv[2] ? argv[2] : "");
	}

	if (strcmp(argv[1],"Sample_Interval") == 0) {
		strcpy(sql_SAMPLEINTERVAL,	argv[2] ? argv[2] : "");
	}

	if (strcmp(argv[1],"OverTime") == 0) {
		strcpy(sql_OVERTIME,		argv[2] ? argv[2] : "");
	}

	if (strcmp(argv[1],"ReCount") == 0) {
		strcpy(sql_RECOUNT,		argv[2] ? argv[2] : "");
	}

	if (strcmp(argv[1],"DINums") == 0) {
		strcpy(sql_DINUMS,		argv[2] ? argv[2] : "");
	}

	if (strcmp(argv[1],"AINums") == 0) {
		strcpy(sql_AINUMS,		argv[2] ? argv[2] : "");
	}

	if (strcmp(argv[1],"COMNums") == 0) {
		strcpy(sql_COMNUMS,		argv[2] ? argv[2] : "");
	}

	if (strcmp(argv[1],"APN") == 0) {
		strcpy(sql_APN,			argv[2] ? argv[2] : "");
	}

	if (strcmp(argv[1],"DialNumber") == 0){
		strcpy(sql_DIALNUMBER,		argv[2] ? argv[2] : "");
	}

	if (strcmp(argv[1],"DialUser") == 0) {
		strcpy(sql_DIALUSER,		argv[2] ? argv[2] : "");
	}

	if (strcmp(argv[1],"DialPassword") == 0) {
		strcpy(sql_DIALPASSWORD,	argv[2] ? argv[2] : "");
	}

	if (strcmp(argv[1],"RtdInterval") == 0) {
		strcpy(sql_RTDINTERVAL,		argv[2] ? argv[2] : "");
	}

	if (strcmp(argv[1],"RSInterval") == 0) {
		strcpy(sql_RSINTERVAL,		argv[2] ? argv[2] : "");
	}

	return 0;
}

static int select_table(void)
{
	char *zErrMsg;
	int ret;
	char sql[256];
	
	bzero(sql,sizeof(sql));
	
	snprintf(sql,sizeof(sql),"SELECT * FROM %s;",TABLENAME);

	ret = sqlite3_exec(db,sql,select_callback,NULL,&zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr,"SQL error: %s\n",zErrMsg);
		sqlite3_free(zErrMsg);
		return -1;
	}
	
	return 0;
}

static void reset_str(void)
{
	cvalue[0] = sql_MN;
	cvalue[1] = sql_PW;
	cvalue[2] = sql_SITETYPE;
	cvalue[3] = sql_CENTER1IP;
	cvalue[4] = sql_CENTER1PORT;
	cvalue[5] = sql_CENTER2IP;
	cvalue[6] = sql_CENTER2PORT;
	cvalue[7] = sql_CONNECT2CENTER1;
	cvalue[8] = sql_CONNECT2CENTER2;
	cvalue[9] = sql_SAMPLEINTERVAL;
	cvalue[10] = sql_OVERTIME;
	cvalue[11] = sql_RECOUNT;
	cvalue[12] = sql_DINUMS;
	cvalue[13] = sql_AINUMS;
	cvalue[14] = sql_COMNUMS;
	cvalue[15] = sql_APN;
	cvalue[16] = sql_DIALNUMBER;
	cvalue[17] = sql_DIALUSER;
	cvalue[18] = sql_DIALPASSWORD;
	cvalue[19] = sql_RTDINTERVAL;
	cvalue[20] = sql_RSINTERVAL;
}


static int update_table(void)
{
	char *zErrMsg;
	int ret,i;
	char sql[256];
	
	bzero(sql,sizeof(sql));

	store_form_value();
	
	reset_str();

	for (i = 0; i < 21; i++) 
	{
		snprintf(sql,sizeof(sql),"UPDATE %s SET cvalue = \'%s\' WHERE ID = %d;", TABLENAME,cvalue[i],i + 1);

		ret = sqlite3_exec(db,sql,update_callback,NULL,&zErrMsg);
		if (ret != SQLITE_OK) {
			fprintf(stderr,"SQL error: %s \n",zErrMsg);
			sqlite3_free(zErrMsg);
			return -1;
		}

		bzero(sql,sizeof(sql));		
	}
	
	return 0;
}

static void show_login(void)
{
	printf("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n");
	printf("<html xmlns=\"http://www.w3.org/1999/xhtml\">\n");
	printf("<head>\n");
	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n");
	printf("<title>工业智慧网关配置管理</title>\n");
	printf("<style type=\"text/css\">\n");
	printf("body {\n");
	printf("background-color: #2f3135;\n");
	printf("margin-left: 0px;\n");
	printf("margin-top: 0px;\n");
	printf("margin-right: 0px;\n");
	printf("margin-bottom: 0px;\n");
	printf("}\n");

	printf("body,td,th {\n");
	printf("font-family: \"微软雅黑\";\n");
	printf("font-size: 16px;\n");
	printf("color: #CCCCCC;\n");
	printf("}\n");

	printf("</style>\n");
	printf("</head>\n");
	printf("<body>\n");
	printf("<div style=\"margin:120px auto; width:656px; height:344px; background-image:url(\'../img/login_bg.png\');\">\n");
    	printf("<form method=\"GET\" class=\"registerform\" action=\"/cgi-bin/siteconfig.cgi\">\n");
    	printf("<div style=\"float:left; margin-left:258px; margin-top:139px; width:230px;\">\n");
        printf("<div style=\"background-image:url(\'../img/login_user.png\'); background-position:left center; background-repeat:no-repeat; padding-left:35px; height:30px;\">\n");
        printf("<input type=\"text\" name=\"user\" value=\"请输用户名\" onfocus=\"if (this.value == '请输用户名') { this.value = ''; }\" onblur=\"if (this.value == '') { this.value = '请输用户名' }\" style=\"width:187px; height:22px; background-color:#8c8c8c; border:1px solid #cccccc; color:#CCC; font-size:14px; font-family:'微软雅黑'; margin-top:2px; padding-left:3px;\" />\n");
        printf("</div>\n");
        printf("<div style=\"background-image:url(\'../img/login_ps.png\'); background-position:left center; background-repeat:no-repeat; padding-left:35px; height:30px; margin-top:20px;\">\n");
        printf("<input type=\"password\" name=\"passwd\" value=\"请输密码\" onfocus=\"if (this.value == '请输密码') { this.value = ''; }\" onblur=\"if (this.value == '') { this.value = '请输密码' }\" style=\"width:187px; height:22px; background-color:#8c8c8c; border:1px solid #cccccc; color:#CCC; font-size:14px; font-family:'微软雅黑'; margin-top:2px; padding-left:3px;\" />\n");

	printf("<div color=\"red\" style=\"font-size:16px; margin-top:9px; font-family:'微软雅黑'; color:#ff0000;\"> 用 户 名 或 密 码 错 误</div>\n");
        
	printf("</div>\n");
    	printf("</div>\n");
    	printf("<div style=\"float:right; margin-right:90px; margin-top:139px; width:70px;\">\n");
        printf("<input type=\"submit\" id=\"btnLogin\" name=\"login\" value=\"登 录\" style=\"width:65px; height:76px; background-color:#8c8c8c; border:1px solid #cccccc; margin-top:2px; color:#ffffff;\" />\n");
    	printf("</div>\n");

	printf("</form>\n");
	printf("</div>\n");
	printf("</body>\n");
	printf("</html>\n");
}


static void show_form(void)
{
	printf("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \
			\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n");
	printf("<html xmlns=\"http://www.w3.org/1999/xhtml\">\n");
	printf("<head>\n");
	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n");
	printf("<title>工业智慧网关配置管理</title>\n");
	printf("<link rel=\"stylesheet\" href=\"../css/bootstrap.css\" />\n");
	printf("<link rel=\"stylesheet\" href=\"../css/css.css\" />\n");
	printf("<link rel=\"stylesheet\" href=\"../css/style.css\" type=\"text/css\" media=\"all\" />\n");
	printf("<script type=\"text/javascript\" src=\"../js/bootstrap.min.js\"></script>\n");
	printf("<script type=\"text/javascript\" src=\"../js/sdmenu.js\"></script>\n");

	printf("<style type=\"text/css\">\n");
	printf("td{line-height:38px;}\n");
	printf(".Validform_checktip{margin-left:0;}\n");
	printf(".info{\n");
	printf("border:1px solid #ccc;\n"); 
	printf("padding:2px 20px 2px 5px;\n");
	printf("color:#666;\n");
	printf("position:absolute;\n");
	printf("display:none;\n");
	printf("line-height:20px;\n");
	printf("background-color:#fff;\n");
	printf("}\n");
	printf(".dec {\n");
    	printf("bottom: -8px;\n");
    	printf("display: block;\n");
    	printf("height: 8px;\n");
    	printf("overflow: hidden;\n");
    	printf("position: absolute;\n");
    	printf("left: 10px;\n");
    	printf("width: 17px;\n");
	printf("}\n");
	printf(".dec s {\n");
    	printf("font-family: simsun;\n");
    	printf("font-size: 16px;\n");
    	printf("height: 19px;\n");
    	printf("left: 0;\n");
    	printf("line-height: 21px;\n");
    	printf("position: absolute;\n");
    	printf("text-decoration: none;\n");
    	printf("top: -9px;\n");
    	printf("width: 17px;\n");
	printf("}\n");
	printf(".dec .dec1 {\n");
    	printf("color: #ccc;\n");
	printf("}\n");
	printf(".dec .dec2 {\n");
    	printf("color: #fff;\n");
    	printf("top: -10px;\n");
	printf("}\n");
	printf("</style>\n");

	printf("</head>\n");
	printf("<body>\n");

	printf("<div class=\"header\">\n");
	printf("<div class=\"logo\"><img src=\"../img/logo.png\" /></div>\n");
	printf("<div class=\"header-right\">\n");

	printf("<a id=\"modal-973558\" href=\"#modal-container-973558\" \
					role=\"button\" data-toggle=\"modal\"></a>\n");
	printf("</div>\n");
	printf("</div>\n");
	printf("<!-- 顶部 -->\n");
	printf("<div id=\"middle\">\n");
	printf("<div class=\"left\">\n");
	printf("<script type=\"text/javascript\">\n");
	printf("var myMenu;\n");
	printf("window.onload = function () {\n");
	printf("myMenu = new SDMenu(\"my_menu\");\n");
	printf("myMenu.init();\n");
	printf("};\n");
	printf("</script>\n");
	printf("<div id=\"my_menu\" class=\"sdmenu\">\n");
	printf("<div>\n");
	printf("<span> 配置表</span>\n");

	printf("<a href=\"/cgi-bin/siteconfig.cgi\">站点信息</a>\n");
	printf("<a href=\"/cgi-bin/server.cgi\">中心信息</a>\n");
	printf("<a href=\"/cgi-bin/devices.cgi\">通信信息</a>\n");
	printf("<a href=\"/cgi-bin/pols.cgi\">因子信息</a>\n");\
	printf("<a href=\"/cgi-bin/sitesbs.cgi\">设备信息</a>\n");
	printf("<a href=\"/cgi-bin/sitepols.cgi\">因子参数</a>\n");
	printf("<a href=\"/cgi-bin/sbparam.cgi\">设备参数</a>\n");
	printf("<a href=\"/cgi-bin/ctlparam.cgi\">控制参数</a>\n");
	printf("<a href=\"/cgi-bin/polsalarm.cgi\">报警参数</a>\n");

	printf("<a href=\"/cgi-bin/runstatus.cgi\">运行状态</a>\n");
	printf("<a href=\"/cgi-bin/dailyruntime.cgi\">运行时间</a>\n");
	printf("<a href=\"/cgi-bin/rtdata.cgi\">实时数据</a>\n");
	printf("<a href=\"/cgi-bin/minutedata.cgi\">分钟数据</a>\n");
	printf("<a href=\"/cgi-bin/hourlydata.cgi\">小时数据</a>\n");
	printf("<a href=\"/cgi-bin/dailydata.cgi\">每天数据</a>\n");
	printf("<a href=\"/cgi-bin/alarm.cgi\">报警信息</a>\n");
	printf("<a href=\"/cgi-bin/fault.cgi\">故障信息</a>\n");
	
	printf("</div>\n");
	printf("</div>\n");
	printf("</div>\n");
	printf("<div class=\"Switch\"></div>\n");
	printf("<script type=\"text/javascript\">\n");
	printf("$(document).ready(function (e) {\n");
	printf("$(\".Switch\").click(function () {\n");
	printf("$(\".left\").toggle();\n");
	printf("});\n");
	printf("});\n");
	printf("</script>\n");
	printf("<div class=\"right\" id=\"mainFrame\">\n");
	printf("<div class=\"right_cont\">\n");
	printf("<ul class=\"breadcrumb\">\n");
	printf("当前位置：\n");
	printf("<a href=\"#\">首页</a>\n");
	printf("<span class=\"divider\">/</span>\n");
	printf("<a href=\"#\">配置表</a>\n");
	printf("<span class=\"divider\">/</span>\n");
	printf("站点信息\n");
	printf("</ul>\n");
	printf("<div class=\"title_right\"><strong>站点信息</strong></div>\n");
	printf("<div style=\"width:900px;margin:auto;\">\n");

	printf("<form class=\"registerform\" action=\"/cgi-bin/siteconfig.cgi\">\n");
	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" style=\"color:#0088cc;\" \
			bgcolor=\"#f1f1f1\">基本参数&nbsp;&nbsp;&nbsp;&nbsp;</td>\n");
	printf("<td width=\"90%%\"></td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:25px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">设备编号：</td>\n");
	printf("<td><input type=\"text\" name=\"MN\" style=\"width:110px;\" value=\"%s\" class=\"inputxt\" \
			datatype=\"n1-18\" nullmsg=\"请输入设备编号！\" errormsg=\"设备编号1-18位数字！\" /></td>\n",sql_MN);
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">设备编号1-18位数字</span><span class=\"dec\"> \
			<s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">设备类型：</td>\n");
	printf("<td><input type=\"text\" name=\"SiteType\" style=\"width:110px;\" value=\"%s\" class=\"inputxt\" \
			datatype=\"n1-1\" nullmsg=\"请输入设备类型！\" errormsg=\"站点类型（1：监测站 2：治理设备）\" \
			/></td>\n",sql_SITETYPE);
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">站点类型（1：监测站 2：治理设备）</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">访问密码：</td>\n");
	printf("<td><input type=\"text\" name=\"PW\" style=\"width:110px;\" value=\"%s\" class=\"inputxt\" \
			datatype=\"*6-12\" nullmsg=\"请输入访问密码！\" errormsg=\"访问密码6-12位任意字符！\" /></td>\n",sql_PW);
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">访问密码6-12位任意字符！</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" style=\"color:#0088cc;\" \
			bgcolor=\"#f1f1f1\">高级参数&nbsp;&nbsp;&nbsp;&nbsp;</td>\n");
	printf("<td width=\"90%%\"></td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:25px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">测试周期：</td>\n");
	printf("<td><input type=\"text\" name=\"sampleinterval\" style=\"width:110px;\" value=\"%s\" \
			class=\"inputxt\" datatype=\"n1-10\" nullmsg=\"请输入测试周期！\" \
			errormsg=\"测试周期1-10位数字！\" />毫秒</td>\n",sql_SAMPLEINTERVAL);
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">测试周期1-10位数字！</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">超时周期：</td>\n");
	printf("<td><input type=\"text\" name=\"overtime\" style=\"width:110px;\" value=\"%s\" class=\"inputxt\" \
			datatype=\"n1-10\" nullmsg=\"请输入超时周期！\" errormsg=\"超时周期1-10位数字！\" />毫秒</td>\n",sql_OVERTIME);
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">超时周期1-10位数字！</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">重发次数：</td>\n");
	printf("<td><input type=\"text\" name=\"recount\" style=\"width:110px;\" value=\"%s\" class=\"inputxt\" \
			datatype=\"n1-10\"  nullmsg=\"请输入重发次数！\" errormsg=\"重发次数1-10位数字！\" />次</td>\n",sql_RECOUNT);
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">重发次数1-10位数字！</span> \
			 <span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" style=\"color:#0088cc;\" \
			bgcolor=\"#f1f1f1\">设备参数&nbsp;&nbsp;&nbsp;&nbsp;</td>\n");
	printf("<td width=\"90%%\"></td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:25px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">开关量个数：</td>\n");
	printf("<td><input type=\"text\" name=\"dinums\" style=\"width:110px;\" value=\"%s\" class=\"inputxt\" \
			datatype=\"n1-5\" nullmsg=\"请输入开关量个数！\" errormsg=\"开关量1-5位数字！\" /> 个</td>\n",sql_DINUMS);
	printf("<td>\n");
	printf("<div class=\"info\"><span class=\"Validform_checktip\">开关量1-5位数字！</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
	printf("</td>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">模拟量个数：</td>\n");
	printf("<td><input type=\"text\" name=\"ainums\" style=\"width:110px;\" value=\"%s\" class=\"inputxt\" \
			datatype=\"n1-5\" nullmsg=\"请输入模拟量个数！\" errormsg=\"模拟量1-5位数字！\" /> 个</td>\n",sql_AINUMS);
	printf("<td>\n");
	printf("<div class=\"info\"><span class=\"Validform_checktip\">模拟量1-5位数字！</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
	printf("</td>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">串口个数：</td>\n");
	printf("<td><input type=\"text\" name=\"comnums\" style=\"width:110px;\" value=\"%s\" class=\"inputxt\" \
			datatype=\"n1-3\" nullmsg=\"请输入串口个数！\" errormsg=\"串口个数1-3位数字！\" />个</td>\n",sql_COMNUMS);
	printf("<td>\n");
	printf("<div class=\"info\"><span class=\"Validform_checktip\">串口个数1-3位数字</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
	printf("</td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" style=\"color:#0088cc;\" \
			bgcolor=\"#f1f1f1\">上报周期&nbsp;&nbsp;&nbsp;&nbsp;</td>\n");
	printf("<td width=\"90%%\"></td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:25px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">实时数据：</td>\n");
	printf("<td><input type=\"text\" name=\"rtdinterval\" style=\"width:110px;\" value=\"%s\" class=\"inputxt\" \
			datatype=\"n1-10\" nullmsg=\"请输入实时数据上报时间间隔！\" \
			errormsg=\"实时数据上报时间间隔1-10位数字！\" />秒</td>\n",sql_RTDINTERVAL);
	printf("<td>\n");
	printf("<div class=\"info\"><span class=\"Validform_checktip\">实时数据上报时间间隔1-10位数字</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
	printf("</td>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">实时状态：</td>\n");
	printf("<td><input type=\"text\" name=\"rsinterval\" style=\"width:110px;\" value=\"%s\" class=\"inputxt\" \
			datatype=\"n1-10\" nullmsg=\"请输入实时数据上报时间间隔！\" \
			errormsg=\"实时数据上报时间间隔1-10位数字！\" />秒</td>\n",sql_RSINTERVAL);
	printf("<td>\n");
	printf("<div class=\"info\"><span class=\"Validform_checktip\">实时数据上报时间间隔1-10位数字！\
			</span><span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" style=\"color:#0088cc;\" \
			bgcolor=\"#f1f1f1\">拨号参数&nbsp;&nbsp;&nbsp;&nbsp;</td>\n");
	printf("<td width=\"90%%\"></td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:25px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">APN：</td>\n");
	printf("<td><input type=\"text\" name=\"apn\" style=\"width:110px;\" value=\"%s\" class=\"inputxt\" /></td>\n",sql_APN);
	printf("<td>\n");
	printf("<div class=\"info\"><span class=\"Validform_checktip\"></span><span class=\"dec\"> \
			<s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
	printf("</td>\n");
	printf("</table>\n");				

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:25px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">拨号号码：</td>\n");
	printf("<td><input type=\"text\" name=\"dialnumber\" style=\"width:110px;\" value=\"%s\" \
			class=\"inputxt\" datatype=\"*1-20\" nullmsg=\"请输入拨号号码！\"  \
			errormsg=\"拨号用户1-20位字符！\" /></td>\n",sql_DIALNUMBER);
	printf("<td>\n");
	printf("<div class=\"info\"><span class=\"Validform_checktip\">拨号用户1-20位字符！</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
	printf("</td>\n");				

	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">拨号用户：</td>\n");
	printf("<td><input type=\"text\" name=\"dialuser\" style=\"width:110px;\" value=\"%s\" \
			class=\"inputxt\" datatype=\"*1-20\" nullmsg=\"请输入拨号用户！\" \
			errormsg=\"拨号用户1-20位任意字符！\" /></td>\n",sql_DIALUSER);
	printf("<td>\n");
	printf("<div class=\"info\"><span class=\"Validform_checktip\">拨号用户1-20位任意字符！\
			</span><span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
	printf("</td>\n");
	printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">拨号密码：</td>\n");
	printf("<td><input type=\"text\" name=\"dialpassword\" style=\"width:110px;\" value=\"%s\" \
			class=\"inputxt\" datatype=\"*1-20\" errormsg=\"拨号密码1-20位任意字符！\" /></td>\n",sql_DIALPASSWORD);
	printf("<td>\n");
	printf("<div class=\"info\"><span class=\"Validform_checktip\">拨号密码1-20位任意字符</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
	printf("</td>\n");
	printf("</tr>\n");
	printf("</table>\n");
	printf("<table class=\"margin-bottom-20  table no-border\">\n");
	printf("<tr>\n");
	printf("<td class=\"text-center\"><input type=\"submit\" value=\"保存\" name=\"submit\" \
			class=\"btn btn-info  \" style=\"width:80px;\" /></td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("</div>\n");
	printf("</div>\n");
	printf("</div>\n");
	printf("</div>\n");
	printf("</form>\n");
	printf("<script type=\"text/javascript\" src=\"../js/jquery-1.6.2.min.js\"></script>\n");
	printf("<script type=\"text/javascript\" src=\"../js/Validform_v5.3.2_min.js\"></script>\n");
	printf("<script type=\"text/javascript\">\n");
	printf("$(function(){\n");
	printf("//$(\".registerform\").Validform();  //就这一行代码！;\n");

	printf("$(\".registerform\").Validform({\n");
	printf("tiptype:function(msg,o,cssctl){\n");
	printf("//msg：提示信息;\n");
	printf("//o:{obj:*,type:*,curform:*}, obj指向的是当前验证的表单元素（或表单对象），\
			type指示提示的状态，值为1、2、3、4， 1：正在检测/提交数据，2：通过验证，3：验证失败，\
			4：提示ignore状态, curform为当前form对象;\n");
	printf("//cssctl:内置的提示信息样式控制函数，该函数需传入两个参数：显示提示信息的对象 和 \
			当前提示的状态（既形参o中的type）;\n");

	printf("if(!o.obj.is(\"form\")){//验证表单元素时o.obj为该表单元素，全部验证通过提交表单时o.obj为该表单对象;\n");
	printf("var objtip=o.obj.parents(\"td\").next().find(\".Validform_checktip\");\n");
	printf("cssctl(objtip,o.type);\n");
	printf("objtip.text(msg);\n");

	printf("var infoObj=o.obj.parents(\"td\").next().find(\".info\");\n");
	printf("if(o.type==2){\n");
	printf("infoObj.fadeOut(200);\n");
	printf("}else{\n");
	printf("if(infoObj.is(\":visible\")){return;}\n");
	printf("var left=o.obj.offset().left,\n");
	printf("top=o.obj.offset().top;\n");

	printf("infoObj.css({\n");
	printf("left:left-130,\n");
	printf("top:top\n");
	printf("}).show().animate({\n");
	printf("top:top-95\n");	
	printf("},200);\n");
	printf("}\n");

	printf("}\n");	
	printf("},\n");
	printf("usePlugin:{\n");
	printf("passwordstrength:{\n");
	printf("minLen:6,\n");
	printf("maxLen:18,\n");
	printf("trigger:function(obj,error){\n");
	printf("if(error){\n");
	printf("obj.parents(\"td\").next().find(\".passwordStrength\").hide();\n");
	printf("}else{\n");
	printf("obj.parents(\"td\").next().find(\".info\").hide();\n");
	printf("obj.parents(\"td\").next().find(\".passwordStrength\").show();\n");	
	printf("}\n");
	printf("}\n");
	printf("}\n");
	printf("}\n");
	printf("});\n");
	printf("})\n");
	printf("</script>\n");
	printf("</body>\n");
	printf("</html>\n");
}





















