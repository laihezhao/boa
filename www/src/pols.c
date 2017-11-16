#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include <time.h>
#include <unistd.h>

#include "cgic.h"
#include "prefile.h"

#define TABLENAME "pols"
#define MAX_BUF_SIZE 100

static char sql_ID[MAX_BUF_SIZE];
static char sql_POLID[MAX_BUF_SIZE];
static char sql_POLNAME[MAX_BUF_SIZE];
static char sql_UPVALUE[MAX_BUF_SIZE];
static char sql_LOWVALUE[MAX_BUF_SIZE];
static char sql_UNIT[MAX_BUF_SIZE];
static char sql_ISSTAT[MAX_BUF_SIZE];
static char sql_POLTYPE[MAX_BUF_SIZE];

static sqlite3 *db;

typedef struct Pols
{
	char id[32];
	char polid[32];
	char polname[32];
	char upvalue[32];
	char lowvalue[32];
	char unit[32];
	char isstat[32];
	char poltype[32];
}POLS;

static POLS sql_pols[100];
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
	/*insert the table*/
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
	memset(sql_pols,    0, sizeof(sql_pols));

	memset(sql_ID,	    0, sizeof(sql_ID));
	memset(sql_POLID,   0, sizeof(sql_POLID));
	memset(sql_POLNAME, 0, sizeof(sql_POLNAME));
	memset(sql_UPVALUE, 0, sizeof(sql_UPVALUE));
	memset(sql_LOWVALUE,0, sizeof(sql_LOWVALUE));
	memset(sql_UNIT,    0, sizeof(sql_UNIT));
	memset(sql_ISSTAT,  0, sizeof(sql_ISSTAT));
	memset(sql_POLTYPE, 0, sizeof(sql_POLTYPE));
}
/*obtain the update value*/
static void update_value(void)
{
	get_form_value("update_id",      sql_ID,      sizeof(sql_ID));
	get_form_value("update_polid",   sql_POLID,   sizeof(sql_POLID));
	get_form_value("update_polname", sql_POLNAME, sizeof(sql_POLNAME));
	get_form_value("update_upvalue", sql_UPVALUE, sizeof(sql_UPVALUE));
	get_form_value("update_lowvalue",sql_LOWVALUE,sizeof(sql_LOWVALUE));
	get_form_value("update_unit",    sql_UNIT,    sizeof(sql_UNIT));
	get_form_value("update_isstat",  sql_ISSTAT,  sizeof(sql_ISSTAT));
	get_form_value("update_poltype", sql_POLTYPE, sizeof(sql_POLTYPE));

}

static void insert_value(void)
{
	
	get_form_value("ID",      sql_ID,      sizeof(sql_ID));
	get_form_value("POLID",   sql_POLID,   sizeof(sql_POLID));
	get_form_value("POLNAME", sql_POLNAME, sizeof(sql_POLNAME));
	get_form_value("UPVALUE", sql_UPVALUE, sizeof(sql_UPVALUE));
	get_form_value("LOWVALUE",sql_LOWVALUE,sizeof(sql_LOWVALUE));
	get_form_value("UNIT",    sql_UNIT,    sizeof(sql_UNIT));
	get_form_value("ISSTAT",  sql_ISSTAT,  sizeof(sql_ISSTAT));
	get_form_value("POLTYPE", sql_POLTYPE, sizeof(sql_POLTYPE));

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
		fprintf(stderr,"open database failure : %s\n",sqlite3_errmsg(db));
		return -1;
	}
	
	return 0;
}

static int close_database(void)
{
	int ret;

	ret = sqlite3_close(db);
	if (ret != SQLITE_OK) {
		fprintf(stderr,"close database failure : %s\n",sqlite3_errmsg(db));
		return -1;
	}

	return 0;
}

static int select_callback(void *data,int argc,char **argv,char **azColName)
{
	
	{
		strcpy(sql_pols[row].id,	argv[0] ? argv[0] : "");
		strcpy(sql_pols[row].polid,	argv[1] ? argv[1] : "");
		strcpy(sql_pols[row].polname,	argv[2] ? argv[2] : "");
		strcpy(sql_pols[row].upvalue,	argv[3] ? argv[3] : "");
		strcpy(sql_pols[row].lowvalue,	argv[4] ? argv[4] : "");
		strcpy(sql_pols[row].unit,	argv[5] ? argv[5] : "");
		strcpy(sql_pols[row].isstat,	argv[6] ? argv[6] : "");	
		strcpy(sql_pols[row].poltype,	argv[7] ? argv[7] : "");
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
		fprintf(stderr,"SQL error: %s\n",zErrMsg);
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

	snprintf(sql,sizeof(sql),"UPDATE %s SET polid = \'%s\',polname = \'%s\',upvalue = \'%s\',lowvalue = \'%s\',\
				unit = \'%s\',isstat = %d,poltype = %d where ID = %d;",\
			TABLENAME,sql_POLID,sql_POLNAME,sql_UPVALUE,sql_LOWVALUE,sql_UNIT,atoi(sql_ISSTAT),\
			atoi(sql_POLTYPE),atoi(sql_ID));
	ret = sqlite3_exec(db,sql,update_callback,NULL,&zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr,"SQL error: %s \n",zErrMsg);
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

	snprintf(sql,sizeof(sql),"INSERT INTO %s VALUES (\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\');",\
			TABLENAME,sql_ID,sql_POLID,sql_POLNAME,sql_UPVALUE,sql_LOWVALUE,sql_UNIT,sql_ISSTAT,sql_POLTYPE);

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
		fprintf(stderr,"SQL error: %s\n",zErrMsg);
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
	printf("window.location = \"/cgi-bin/update_pols.cgi?id=\" + id;\n");
        printf("}\n");
    	printf("</script>\n");

	printf("</head>\n");
	printf("<body>\n");
	printf("<div class=\"header\">\n");
	printf("<div class=\"logo\"><img src=\"../img/logo.png\" /></div>\n");

	printf("<div class=\"header-right\">\n");
	
	printf("<a id=\"modal-973558\" \
			href=\"#modal-container-973558\" role=\"button\" data-toggle=\"modal\"></a>\n");

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

        printf("function Dele(Id) {\n");
        printf("if (confirm(\"确定删除吗？\")) {\n");
        printf("window.location = \"/cgi-bin/pols.cgi?Id=\" + Id;\n");
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
	printf("因子信息\n");
	printf("</ul>\n");

	printf("<div class=\"title_right\"><span class=\"pull-right margin-bottom-5\"> \
			<a class=\"btn btn-info btn-small\" id=\"modal-9735581\" \
			href=\"#modal-container-9735581\" role=\"button\" data-toggle=\"modal\"> \
			<i class=\"icon-plus icon-white\"></i> 添加因子信息</a> \
			</span><strong>因子信息</strong></div>\n");
	printf("<div id=\"modal-container-9735581\" class=\"modal hide fade\" role=\"dialog\" \
			aria-labelledby=\"myModalLabel\" aria-hidden=\"true\" style=\"width:600px; \
			margin-left:-300px; top:20%%\">\n");
	printf("<div class=\"modal-header\">\n");
	printf("<button type=\"button\" class=\"close\" data-dismiss=\"modal\" aria-hidden=\"true\">×</button>\n");
	printf("<h3 id=\"myModalLabel\">\n");
	printf("因子信息\n");
	printf("</h3>\n");	
											
	printf("</div>\n");
	printf("<div class=\"modal-body\">\n");

	printf("<form class=\"registerform\" action=\"/cgi-bin/pols.cgi\">\n");
	printf("<table class=\"table table-bordered\">\n");	
	printf("<tbody>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">序号：</td>\n");
	printf("<td><input type=\"text\" name=\"ID\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"n1-10\" nullmsg=\"请输入序号(唯一性)！\" errormsg=\"序号1-10位数字(唯一性)！\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">序号1-10位数字(唯一性)！</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");

	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">仪表编号：</td>\n");
	printf("<td><input type=\"text\" name=\"POLID\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"*1-12\" nullmsg=\"请输入仪表编号！\" errormsg=\"仪表编号1-12任意字符!\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">仪表编号1-12任意字符!</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">仪表名称：</td>\n");
	printf("<td><input type=\"text\" name=\"POLNAME\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"*1-24\" nullmsg=\"请输入仪表名称！\" errormsg=\"仪表名称1-24位任意字符！\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">仪表名称1-24位任意字符！</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">量程上限：</td>\n");
	printf("<td><input type=\"text\" name=\"UPVALUE\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"*1-12\" nullmsg=\"请输入量程上限！\" errormsg=\"量程上限1-12任意字符!\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">量程上限1-12任意字符!</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">量程下限：</td>\n");
	printf("<td><input type=\"text\" name=\"LOWVALUE\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"*1-12\" nullmsg=\"请输入量程下限！\" errormsg=\"量程下限1-12任意字符!\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">量程下限1-12任意字符!</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">单位：</td>\n");
	printf("<td><input type=\"text\" name=\"UNIT\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"*1-12\" nullmsg=\"请输入单位！\" errormsg=\"单位1-12位任意字符！\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">单位1-12位任意字符！</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">统计：</td>\n");
	printf("<td><input type=\"text\" name=\"ISSTAT\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"n1-1\" nullmsg=\"请输入统计类型！\" errormsg=\"统计(0:不统计,1:统计)！\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">统计(0:不统计,1:统计)！</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">仪表类型：</td>\n");
	printf("<td><input type=\"text\" name=\"POLTYPE\" style=\"width:110px;\" value=\"\" class=\"inputxt\" \
			datatype=\"n1-1\" nullmsg=\"请输入仪表类型！\" errormsg=\"仪表类型(0:辅助参数,1:污染因子)!\" /></td>\n");
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">仪表类型(0:辅助参数,1:污染因子)!</span> \
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
	printf("<td nowrap=\"nowrap\"><strong>仪表编号</strong></td>\n");
	printf("<td nowrap=\"nowrap\"><strong>仪表名称</strong></td>\n");
	printf("<td nowrap=\"nowrap\"><strong>量程上限</strong></td>\n");
	printf("<td><strong>量程下限</strong></td>\n");
	printf("<td><strong>单位</strong></td>\n");
	printf("<td><strong>统计</strong></td>\n");
	printf("<td><strong>仪表类型</strong></td>\n");
	printf("<td><strong>操作</strong></td>\n");
	printf("</tr>\n");

	int i = 0;
	for (i = 0;i < row;i++)
	{
		printf("<tr align=\"center\">\n");
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_pols[i].id);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_pols[i].polid);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_pols[i].polname);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_pols[i].upvalue);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_pols[i].lowvalue);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_pols[i].unit);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_pols[i].isstat);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_pols[i].poltype);
	        printf("<td nowrap=\"nowrap\"><a href=\"javascript:void(0)\" onclick=\"Dele(%s)\">删除</a> \
						<a href=\"javascript:void(0)\" onclick=\"Edit(%s)\">修改</a></td>\n",\
						sql_pols[i].id,sql_pols[i].id);
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






























