#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include <time.h>
#include <unistd.h>

#include "cgic.h"
#include "prefile.h"

#define TABLENAME "minutedata"
#define COLUMN_SIZE 20

static sqlite3 *db;
static int row = 0;

typedef struct Minutedata
{
	char id[32];
	char datatime[32];
	char polid[32];
	char pol_min[32];
	char pol_avg[32];
	char pol_max[32];
	char pol_col[32];
	char centre1[8];
	char centre2[8];
	char centre3[8];
	char centre4[8];
	char centre5[8];
	char centre6[8];
	char flag[8];
}MINUTEDATA;

static MINUTEDATA sql_minutedata[100];


static void init_memory(void);
static void show_form(void);
static int open_database(void);
static int close_database(void);
static int select_table(void);
static int count_column(void);
static int select_callback(void *,int,char **,char **);

static unsigned int page = 1;
static unsigned int column_max = 0;
static unsigned int page_max = 0;

int cgiMain(void)
{
	char up[24];
	char down[24];
	char jump[24];

	bzero(up,  sizeof(up));
	bzero(down,sizeof(down));
	bzero(jump,sizeof(jump));

	open_database();

	cgiHeaderContentType("text/html");
	/**/
	column_max = count_column();	
	/**/
	if ((column_max % COLUMN_SIZE) == 0) {
		page_max = column_max / COLUMN_SIZE;
	} else {
		page_max = column_max / COLUMN_SIZE + 1;
	}

	if (cgiFormString("pageDown",down,24) != cgiFormSuccess) {
		page = 1;
		if (cgiFormString("pageUp",up,24) == cgiFormSuccess) {
			page = atoi(up);
			if (page == 0 ) {
				page = 1;
			}
		} else if (cgiFormString("pageJump",jump,24) == cgiFormSuccess) {
			page = atoi(jump);
			/**/
			if (page <= 0 || page > page_max) {
				page = 1;	
			}
		}
	} else {
		page = atoi(down);
		if (page > page_max) {
			page = 1;
		}
	}
	/**/
	init_memory();
	/**/
	select_table();
	/**/
	show_form();

	close_database();

	return 0;
}
/*init the memory*/
static void init_memory(void)
{
	memset(sql_minutedata,0, sizeof(sql_minutedata));
}

static int count_column(void)
{
	char sql[256];
	int ret;
	int iCount;
	sqlite3_stmt *stmt;

	snprintf(sql,sizeof(sql),"SELECT count(*) FROM %s;",TABLENAME);
	
	ret = sqlite3_prepare(db,sql,strlen(sql),&stmt,0);
	if (ret != SQLITE_OK) {
		fprintf(stderr,"count the columns failure\n");
		return -1;
	}
	
	ret = sqlite3_step(stmt);
	iCount = sqlite3_column_int(stmt,0);
	sqlite3_finalize(stmt);
	
	return iCount;
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
		strcpy(sql_minutedata[row].id,		argv[0] ? argv[0] : "");
		strcpy(sql_minutedata[row].datatime,	argv[1] ? argv[1] : "");
		strcpy(sql_minutedata[row].polid,	argv[2] ? argv[2] : "");
		strcpy(sql_minutedata[row].pol_min,	argv[3] ? argv[3] : "");
		strcpy(sql_minutedata[row].pol_avg,	argv[4] ? argv[4] : "");
		strcpy(sql_minutedata[row].pol_max,	argv[5] ? argv[5] : "");
		strcpy(sql_minutedata[row].pol_col,	argv[6] ? argv[6] : "");
		strcpy(sql_minutedata[row].centre1,	argv[7] ? argv[7] : "");
		strcpy(sql_minutedata[row].centre2,	argv[8] ? argv[8] : "");
		strcpy(sql_minutedata[row].centre3,	argv[9] ? argv[9] : "");
		strcpy(sql_minutedata[row].centre4,	argv[10] ? argv[10] : "");
		strcpy(sql_minutedata[row].centre5,	argv[11] ? argv[11] : "");
		strcpy(sql_minutedata[row].centre6,	argv[12] ? argv[12] : "");
		strcpy(sql_minutedata[row].flag,	argv[13] ? argv[13] : "");
	}
	
	row++;
	return 0;
}

static int select_table(void)
{
	char *zErrMsg = NULL;
	int ret;
	char sql[256];

	bzero(sql,sizeof(sql));

	snprintf(sql,sizeof(sql),"SELECT * FROM %s LIMIT %d OFFSET %d ;",TABLENAME,COLUMN_SIZE,COLUMN_SIZE*(page -1));
	
	ret = sqlite3_exec(db,sql,select_callback,NULL,&zErrMsg);
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
	printf("<link href=\"../css/syspage.css\" rel=\"stylesheet\" />\n");

	printf("<script type=\"text/javascript\" src=\"../js/jquery1.9.0.min.js\"></script>\n");
	printf("<script type=\"text/javascript\" src=\"../js/bootstrap.min.js\"></script>\n");
	printf("<script type=\"text/javascript\" src=\"../js/sdmenu.js\"></script>\n");
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
	printf("分钟数据\n");
	printf("</ul>\n");
	printf("<div class=\"title_right\"><span class=\"pull-right margin-bottom-5\"></span><strong>分钟数据</strong></div>\n");

	printf("<table class=\"table table-bordered table-striped table-hover\">\n");
	printf("<tbody>\n");
	printf("<tr align=\"center\">\n");
	printf("<td nowrap=\"nowrap\"><strong>序号</strong></td>\n");
	printf("<td nowrap=\"nowrap\"><strong>上传时间</strong></td>\n");
	printf("<td nowrap=\"nowrap\"><strong>仪表ID</strong></td>\n");
	printf("<td nowrap=\"nowrap\"><strong>最小值</strong></td>\n");
	printf("<td><strong>平均值</strong></td>\n");
	printf("<td><strong>最大值</strong></td>\n");
	printf("<td><strong>统计值</strong></td>\n");
	printf("<td><strong>中心1</strong></td>\n");
	printf("<td><strong>中心2</strong></td>\n");
	printf("<td><strong>中心3</strong></td>\n");
	printf("<td><strong>中心4</strong></td>\n");
	printf("<td><strong>中心5</strong></td>\n");
	printf("<td><strong>中心6</strong></td>\n");
	printf("<td><strong>标志</strong></td>\n");
	printf("</tr>\n");

	int i = 0;
	for (i = 0;i < row;i++)
	{
		printf("<tr align=\"center\">\n");
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_minutedata[i].id);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_minutedata[i].datatime);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_minutedata[i].polid);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_minutedata[i].pol_min);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_minutedata[i].pol_avg);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_minutedata[i].pol_max);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_minutedata[i].pol_col);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_minutedata[i].centre1);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_minutedata[i].centre2);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_minutedata[i].centre3);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_minutedata[i].centre4);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_minutedata[i].centre5);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_minutedata[i].centre6);
		printf("<td nowrap=\"nowrap\"> %s </td>\n",sql_minutedata[i].flag);
		printf("</tr>\n");
		
	}

	printf("</tbody>\n");
	printf("</table>\n");

        printf("<div class=\"pages\"> <div id=\"Pagination\"> <div style=\"margin:0;\" class=\"pagination\"> \
			<a href=\"?firstload=1&amp;pageUp=%d\" class=\"prev\">上一页</a>\n",page - 1);
        printf("<a href=\"javascript:void(0);\" class=\"current\">%d</a><a href=\"?firstload=1&amp;pageDown=%d\" \
			class=\"next\">下一页</a></div></div>\n",page,page + 1);
	printf("<div class=\"searchPage\">\n");
	printf("<span class=\"page-sum\">共<strong class=\"allPage\">&nbsp;%d&nbsp;</strong>页&nbsp;%d&nbsp;条记录 \
			</span>\n",page_max,column_max); 
	printf("<span class=\"page-go\">跳转<input id=\"GoTo\" type=\"text\" />页</span> <a class=\"page-btn\" \
			href=\"javascript:void(0);\" onclick=\"GoTo()\">GO</a>\n");

	printf("</div>\n");
	printf("</div>\n");
	printf("</div>\n");

	printf("<script type=\"text/javascript\">function GoTo()\n");
	printf("{window.location = \"?firstload=1&pageJump=\"+ document.getElementById('GoTo').value;}\n");
	printf("</script>\n");

	printf("</body>\n");
	printf("</html>\n");
}




