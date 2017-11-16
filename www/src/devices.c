#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include <time.h>
#include <unistd.h>

#include "cgic.h"
#include "prefile.h"

#define TABLENAME "devices"
#define MAX_BUF_SIZE 100

static char sql_ID[MAX_BUF_SIZE];
static char sql_DEVNAME[MAX_BUF_SIZE];
static char sql_DEVTYPE[MAX_BUF_SIZE];
static char sql_DEVADDR[MAX_BUF_SIZE];
static char sql_COMTYPE[MAX_BUF_SIZE];
static char sql_PORT[MAX_BUF_SIZE];
static char sql_BAUD[MAX_BUF_SIZE];
static char sql_DATABITS[MAX_BUF_SIZE];
static char sql_STOPBITS[MAX_BUF_SIZE];
static char sql_PARIT[MAX_BUF_SIZE];

static sqlite3 *db;

typedef struct Devices
{
	char id[32];
	char devname[32];
	char devtype[32];
	char devaddr[32];
	char comtype[32];
	char port[32];
	char baud[32];
	char databits[32];
	char stopbits[32];
	char parit[32];
}DEVICES;

static DEVICES sql_devices[100];

static int row = 0;

static void init_memory(void);
static void show_form(void);
static int open_database(void);
static int close_database(void);

static int select_table(void);
static int update_table(void);
static int insert_table(void);
static int delete_table(int);

static void update_value(void);
static void insert_value(void);
static void get_form_value(char *,char *,int);

static int select_callback(void *,int,char **,char **);
static int update_callback(void *,int,char **,char **);
static int insert_callback(void *,int,char **,char **);
static int delete_callback(void *,int,char **,char **);


int cgiMain(void)
{
	char data[24];

	bzero(data,24);

	open_database();

	cgiHeaderContentType("text/html");

	/*delete the table*/
	get_form_value("Id",data,sizeof(data));
	if (data != NULL) {
		delete_table(atoi(data));	
	}
	/*judge the submit button*/
	if (cgiFormSubmitClicked("INSERT") == cgiFormSuccess) {
		init_memory();
		insert_table();
		usleep(100);
		select_table();
		show_form();
		return 0;
	}
	/*alter the table*/
	if (cgiFormSubmitClicked("UPDATE") == cgiFormSuccess) {
		init_memory();
		update_table();
		usleep(100);
	} 

	/*show the table*/
	init_memory();
	select_table();
	show_form();

	close_database();

	return 0;
}

/*init the memory*/
static void init_memory(void)
{
	memset(sql_devices, 0, sizeof(sql_devices));

	memset(sql_ID,	    0, sizeof(sql_ID));
	memset(sql_DEVNAME, 0, sizeof(sql_DEVNAME));
	memset(sql_DEVTYPE, 0, sizeof(sql_DEVTYPE));
	memset(sql_DEVADDR, 0, sizeof(sql_DEVADDR));
	memset(sql_COMTYPE, 0, sizeof(sql_COMTYPE));
	memset(sql_PORT,    0, sizeof(sql_PORT));
	memset(sql_BAUD,    0, sizeof(sql_BAUD));
	memset(sql_DATABITS,0, sizeof(sql_DATABITS));
	memset(sql_STOPBITS,0, sizeof(sql_STOPBITS));
	memset(sql_PARIT,   0, sizeof(sql_PARIT));
}

/*obtain the alter value*/
static void update_value(void)
{
	get_form_value("update_id",      sql_ID,      sizeof(sql_ID));
	get_form_value("update_devname", sql_DEVNAME, sizeof(sql_DEVNAME));
	get_form_value("update_devtype", sql_DEVTYPE, sizeof(sql_DEVTYPE));
	get_form_value("update_devaddr", sql_DEVADDR, sizeof(sql_DEVADDR));
	get_form_value("update_comtype", sql_COMTYPE, sizeof(sql_COMTYPE));
	get_form_value("update_port",    sql_PORT,    sizeof(sql_PORT));
	get_form_value("update_baud",    sql_BAUD,    sizeof(sql_BAUD));
	get_form_value("update_databits",sql_DATABITS,sizeof(sql_DATABITS));
	get_form_value("update_stopbits",sql_STOPBITS,sizeof(sql_STOPBITS));
	get_form_value("update_parit",   sql_PARIT,   sizeof(sql_PARIT));

}

static void insert_value(void)
{
	get_form_value("ID",      sql_ID,      sizeof(sql_ID));
	get_form_value("DEVNAME", sql_DEVNAME, sizeof(sql_DEVNAME));
	get_form_value("DEVTYPE", sql_DEVTYPE, sizeof(sql_DEVTYPE));
	get_form_value("DEVADDR", sql_DEVADDR, sizeof(sql_DEVADDR));
	get_form_value("COMTYPE", sql_COMTYPE, sizeof(sql_COMTYPE));
	get_form_value("PORT",    sql_PORT,    sizeof(sql_PORT));
	get_form_value("BAUD",    sql_BAUD,    sizeof(sql_BAUD));
	get_form_value("DATABITS",sql_DATABITS,sizeof(sql_DATABITS));
	get_form_value("STOPBITS",sql_STOPBITS,sizeof(sql_STOPBITS));
	get_form_value("PARIT",   sql_PARIT,   sizeof(sql_PARIT));
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
		fprintf(stdout,"open database failure : %s\n",sqlite3_errmsg(db));
		return -1;
	}
	
	return 0;
}

static int close_database(void)
{
	int ret;

	ret = sqlite3_close(db);
	if (ret != SQLITE_OK) {
		fprintf(stdout,"close database failure : %s\n",sqlite3_errmsg(db));
		return -1;
	}

	return 0;
}

static int select_callback(void *data,int argc,char **argv,char **azColName)
{
	
	{
		strcpy(sql_devices[row].id,	  argv[0] ? argv[0] : "");
		strcpy(sql_devices[row].devname,  argv[1] ? argv[1] : "");
		strcpy(sql_devices[row].devtype,  argv[2] ? argv[2] : "");
		strcpy(sql_devices[row].devaddr,  argv[3] ? argv[3] : "");
		strcpy(sql_devices[row].comtype,  argv[4] ? argv[4] : "");
		strcpy(sql_devices[row].port,	  argv[5] ? argv[5] : "");
		strcpy(sql_devices[row].baud,     argv[6] ? argv[6] : "");
		strcpy(sql_devices[row].databits, argv[7] ? argv[7] : "");
		strcpy(sql_devices[row].stopbits, argv[8] ? argv[8] : "");	
		strcpy(sql_devices[row].parit,	  argv[9] ? argv[9] : "");
	}

	row++;

	return 0;
}


static int update_callback(void *data,int argc,char **argv,char **azColName)
{
	return 0;
}

static int insert_callback(void *data,int argc,char **argv,char **azColName)
{
	return 0;
}

static int delete_callback(void *data,int argc,char **argv,char **azColName)
{
	return 0;
}

static int select_table(void)
{
	char *zErrMsg;
	int ret;
	char sql[256];
	
	bzero(sql,sizeof(sql));
	
	snprintf(sql,sizeof(sql),"SELECT * FROM %s ;",TABLENAME);

	ret = sqlite3_exec(db,sql,select_callback,NULL,&zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stdout,"SQL error: %s\n",zErrMsg);
		sqlite3_free(zErrMsg);
		return -1;
	}

	return 0;
}

static int update_table(void)
{
	char *zErrMsg;
	int ret;
	char sql[256];

	bzero(sql,sizeof(sql));

	update_value();

	snprintf(sql,sizeof(sql),"UPDATE %s SET devname=\'%s\',devtype=%d,devaddr=%d,comtype=%d,port=%d,\
			baud=%d,databits=%d,stopbits=%d,parit=\'%s\'  where ID=%d",\
			TABLENAME,sql_DEVNAME,atoi(sql_DEVTYPE),atoi(sql_DEVADDR),atoi(sql_COMTYPE),atoi(sql_PORT),\
			atoi(sql_BAUD),atoi(sql_DATABITS),atoi(sql_STOPBITS),sql_PARIT,atoi(sql_ID));

	ret = sqlite3_exec(db,sql,update_callback,NULL,&zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stdout,"SQL error: %s \n",zErrMsg);
		sqlite3_free(zErrMsg);
		return -1;
	}		
	
	return 0;
}


static int insert_table(void)
{
	char *zErrMsg;
	int ret;
	char sql[256];
	
	bzero(sql,sizeof(sql));
	
	insert_value();

	snprintf(sql,sizeof(sql),"INSERT INTO %s VALUES (%d,\'%s\',%d,%d,%d,%d,%d,%d,%d,\'%s\');",\
			TABLENAME,atoi(sql_ID),sql_DEVNAME,atoi(sql_DEVTYPE),atoi(sql_DEVADDR),atoi(sql_COMTYPE),\
			atoi(sql_PORT),atoi(sql_BAUD),atoi(sql_DATABITS),atoi(sql_STOPBITS),sql_PARIT);

	ret = sqlite3_exec(db,sql,insert_callback,NULL,&zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stdout,"SQL error: %s\n",zErrMsg);
		sqlite3_free(zErrMsg);
		return -1;
	}
	
	return 0;
}

static int delete_table(int flags)
{
	char *zErrMsg;
	int ret;
	char sql[256];
	
	bzero(sql,sizeof(sql));
	
	snprintf(sql,sizeof(sql),"DELETE FROM %s where ID=%d;",TABLENAME,flags);

	ret = sqlite3_exec(db,sql,delete_callback,NULL,&zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stdout,"SQL error: %s\n",zErrMsg);
		sqlite3_free(zErrMsg);
		return -1;
	}
	
	return 0;	
}


static void show_form(void)
{
	printf("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \
			\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n");
	printf("<html xmlns=\"http://www.w3.org/1999/xhtml\">\n");
	printf("<head>\n");
	printf("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n");
	printf("<title>工业智慧网关配置管理</title>\n");
	printf("<link rel=\"stylesheet\" href=\"../css/bootstrap.css\" />\n");
	printf("<link rel=\"stylesheet\" href=\"../css/css.css\" />\n");
	printf("<link rel=\"stylesheet\" href=\"../css/style.css\" type=\"text/css\" media=\"all\" />\n");

	printf("<script type=\"text/javascript\" src=\"../js/jquery1.9.0.min.js\"></script>\n");
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

	printf("<script type=\"text/javascript\">\n");
        printf("function Edit(id) {\n");
	printf("window.location = \"/cgi-bin/update_devices.cgi?id=\" + id;\n");
        printf("}\n");
    	printf("</script>\n");

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
	printf("<span>配置表</span>\n");

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

        printf("function Dele(Id) {\n");
        printf("if (confirm(\"确定删除吗？\")) {\n");
        printf("window.location = \"/cgi-bin/devices.cgi?Id=\" + Id;\n");
        printf("}\n");
        printf("}\n");

	printf("</script>\n");

	printf("<div class=\"right\" id=\"mainFrame\">\n");

	printf("<div class=\"right_cont\">\n");
	printf("<ul class=\"breadcrumb\">\n");
	printf("当前位置：\n");
	printf("<a href=\"#\">首页</a>\n");
	printf("<span class=\"divider\">/</span>\n");
	printf("<a href=\"#\">配置表</a>\n");
	printf("<span class=\"divider\">/</span>\n");
	printf("通信信息\n");
	printf("</ul>\n");


	printf("<div class=\"title_right\"><span class=\"pull-right margin-bottom-5\"> \
			<a class=\"btn btn-info btn-small\" id=\"modal-9735581\" \
			href=\"#modal-container-9735581\" role=\"button\" data-toggle=\"modal\"> \
			<i class=\"icon-plus icon-white\"></i> 添加通信信息</a> \
			</span><strong>通信信息</strong></div>\n");
	printf("<div id=\"modal-container-9735581\" class=\"modal hide fade\" role=\"dialog\" \
			aria-labelledby=\"myModalLabel\" aria-hidden=\"true\" style=\"width:600px; \
			margin-left:-300px; top:20%%\">\n");
	printf("<div class=\"modal-header\">\n");
	printf("<button type=\"button\" class=\"close\" data-dismiss=\"modal\" aria-hidden=\"true\">×</button>\n");
	printf("<h3 id=\"myModalLabel\">\n");
	printf("通信信息\n");
	printf("</h3>\n");													
	printf("</div>\n");

	printf("<div class=\"modal-body\">\n");
	printf("<form class=\"registerform\" action=\"/cgi-bin/devices.cgi\">\n");
	printf("<table class=\"table table-bordered\">\n");
	printf("<tbody>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">  序号：</td>\n");
	printf("<td><input type=\"text\" name=\"ID\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"n1-10\" nullmsg=\"请输入序号(唯一性)\" errormsg=\"序号1-10位数字(唯一性)\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">序号1-10位数字(唯一性)</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">通信名称：</td>\n");
	printf("<td><input type=\"text\" name=\"DEVNAME\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"*1-30\" nullmsg=\"请输入通信名称\" errormsg=\"通信名称\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">通信名称</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">通信类型：</td>\n");
	printf("<td><input type=\"text\" name=\"DEVTYPE\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"n1-10\" nullmsg=\"请输入通信类型！\" errormsg=\"通信类型（例如0/1）\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">通信类型（例如0/1）</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">  通信地址：</td>\n");
	printf("<td><input type=\"text\" name=\"DEVADDR\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"n1-5\" nullmsg=\"请输入通信地址！\" errormsg=\"通信地址（例如1）\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">通信地址（例如1）</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">通讯方式：</td>\n");
	printf("<td><input type=\"text\" name=\"COMTYPE\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"n1-10\" nullmsg=\"请输入通讯方式！\" errormsg=\"通讯方式（例如1/2）\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">通讯方式（例如1/2）</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">端口：</td>\n");
	printf("<td><input type=\"text\" name=\"PORT\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"n1-1\" nullmsg=\"请输入端口！\" errormsg=\"端口号（例如1-6）\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">端口号（例如1-6）</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">波特率：</td>\n");
	printf("<td><input type=\"text\" name=\"BAUD\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"n1-10\" nullmsg=\"请输入波特率！\" errormsg=\"波特率（例如9600）\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">波特率（例如9600）</span><span class=\"dec\"> \
			<s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">数据位：</td>\n");
	printf("<td><input type=\"text\" name=\"DATABITS\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"n1-1\" nullmsg=\"请输入数据位！\" errormsg=\"数据位（例如8）\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">数据位（例如8）</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">停止位：</td>\n");
	printf("<td><input type=\"text\" name=\"STOPBITS\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
		datatype=\"n1-1\" nullmsg=\"请输入停止位！\" errormsg=\"停止位（例如1）\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">停止位(例如1)!</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">奇偶校验：</td>\n");
	printf("<td><input type=\"text\" name=\"PARIT\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"s1-1\" nullmsg=\"请输入奇偶校验！\" errormsg=\"奇偶校验（例如N/E/O) \" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">奇偶校验（例如N/E/O）</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("</tbody>\n");
	printf("</table>\n");
	printf("</div>\n");
	printf("<div class=\"modal-footer\">\n");
	printf("<input type=\"submit\" name=\"INSERT\" style=\"width:80px\" aria-hidden=\"true\" value=\"保存\" />\n");
	printf("<input type=\"reset\" name=\"RESET\" style=\"width:80px\" aria-hidden=\"true\" value=\"取消\" />\n");

	printf("</div>\n");
	printf("</div>\n");
	printf("</form>\n");
	printf("<table class=\"table table-bordered table-striped table-hover\">\n");
	printf("<tbody>\n");
	printf("<tr align=\"center\">\n");
	printf("<td nowrap=\"nowrap\"><strong>序号</strong></td>\n");
	printf("<td nowrap=\"nowrap\"><strong>通信名称</strong></td>\n");
	printf("<td nowrap=\"nowrap\"><strong>通信类型</strong></td>\n");
	printf("<td nowrap=\"nowrap\"><strong>通信地址</strong></td>\n");
	printf("<td><strong>通讯方式</strong></td>\n");
	printf("<td><strong>端口</strong></td>\n");
	printf("<td><strong>波特率</strong></td>\n");
	printf("<td><strong>数据位</strong></td>\n");
	printf("<td><strong>停止位</strong></td>\n");
	printf("<td><strong>奇偶校验</strong></td>\n");
	printf("<td><strong>操作</strong></td>\n");
	printf("</tr>\n");

	int i = 0;
	for (i = 0;i < row;i++)
	{
		printf("<tr align=\"center\">\n");
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_devices[i].id);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_devices[i].devname);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_devices[i].devtype);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_devices[i].devaddr);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_devices[i].comtype);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_devices[i].port);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_devices[i].baud);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_devices[i].databits);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_devices[i].stopbits);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_devices[i].parit);
	        printf("<td nowrap=\"nowrap\"><a href=\"javascript:void(0)\" onclick=\"Dele(%s)\">删除</a> \
						<a href=\"javascript:void(0)\" onclick=\"Edit(%s)\">修改</a></td>\n",\
						sql_devices[i].id,sql_devices[i].id);
		printf("</tr>\n");
		
	}

	printf("</tbody>\n");
	printf("</table>\n");
	printf("<table class=\"margin-bottom-20  table no-border\">\n");
	printf("<tr>\n");
	printf("</tr>\n");
	printf("</table>\n");
	printf("</div>\n");
	printf("</div>\n");
	printf("</div>\n");

	printf("<script type=\"text/javascript\" src=\"../js/jquery-1.6.2.min.js\"></script>\n");
	printf("<script type=\"text/javascript\" src=\"../js/Validform_v5.3.2_min.js\"></script>\n");
	printf("<script type=\"text/javascript\">\n");
	printf("$(function(){\n");
	printf("//$(\".registerform\").Validform();  //就这一行代码！;\n");

	printf("$(\".registerform\").Validform({\n");
	printf("tiptype:function(msg,o,cssctl){\n");
	printf("//msg：提示信息;\n");
	printf("//o:{obj:*,type:*,curform:*}, obj指向的是当前验证的表单元素（或表单对象），type指示提示的状态，\
			值为1、2、3、4， 1：正在检测/提交数据，2：通过验证，3：验证失败，\
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
	printf("left:left-440,\n");
	printf("top:top\n");
	printf("}).show().animate({\n");
	printf("top:top-210\n");	
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





















