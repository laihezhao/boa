#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include <time.h>
#include <unistd.h>

#include "cgic.h"
#include "prefile.h"

#define TABLENAME "server"

static sqlite3 *db;

typedef struct Server
{
	char id[32];
	char hostname[32];
	char ip[32];
	char port[32];
	char enable[8];
}SERVER;

static SERVER sql_server[1];

static void show_update(void);
static void init_memory(void);
static int open_database(void);
static int close_database(void);
static int select_table(int);
static void get_form_value(char *,char *,int);
static int select_callback(void *,int,char **,char **);

int cgiMain(void)
{
	char data[24];
	
	bzero(data,24);

	open_database();

	cgiHeaderContentType("text/html");

	/*alter the table*/
	get_form_value("id",data,sizeof(data));
	
	if (data != NULL) {
		init_memory();
		select_table(atoi(data));
		show_update();
	}

	close_database();

	return 0;
}

static void init_memory(void)
{
	memset(sql_server,0,sizeof(sql_server));
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
		fprintf(stderr,"close database failure : %s\n",sqlite3_errmsg(db));
		return -1;
	}

	return 0;
}

static int select_callback(void *data,int argc,char **argv,char **azColName)
{
	
	{
		strcpy(sql_server[0].id,	argv[0] ? argv[0] : "");
		strcpy(sql_server[0].hostname,	argv[1] ? argv[1] : "");
		strcpy(sql_server[0].ip,	argv[2] ? argv[2] : "");
		strcpy(sql_server[0].port,	argv[3] ? argv[3] : "");
		strcpy(sql_server[0].enable,	argv[4] ? argv[4] : "");
	}


	return 0;
}

static int select_table(int n)
{
	char *zErrMsg = NULL;
	int ret;
	char sql[256];
	
	bzero(sql,sizeof(sql));
	
	snprintf(sql,sizeof(sql),"SELECT * FROM %s where ID = %d ;",TABLENAME,n);

	ret = sqlite3_exec(db,sql,select_callback,NULL,&zErrMsg);
	if (ret != SQLITE_OK) {
		fprintf(stderr,"SQL error: %s\n",zErrMsg);
		sqlite3_free(zErrMsg);
		return -1;
	}
	
	return 0;
}

static void show_update(void)
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
        printf("</script>\n");

        printf("<div class=\"right\" id=\"mainFrame\">\n");

        printf("<div class=\"right_cont\">\n");
        printf("<ul class=\"breadcrumb\">\n");
        printf("当前位置：\n");
        printf("<a href=\"#\">首页</a>\n");
        printf("<span class=\"divider\">/</span>\n");
        printf("<a href=\"#\">中心信息</a>\n");
        printf("<span class=\"divider\">/</span>\n");
        printf("修改记录\n");
        printf("</ul>\n");

        printf("<div class=\"title_right\"><strong>修改记录</strong></div>\n");
        printf("<div style=\"width:900px;margin:auto;\">\n");
        printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
        printf("<tr>\n");
        printf("<td width=\"10%%\" align=\"right\" nowrap=\"nowrap\" style=\"color:#0088cc;\" \
			bgcolor=\"#f1f1f1\">中心信息&nbsp;&nbsp;&nbsp;&nbsp;</td>\n");
        printf("<td width=\"90%%\"></td>\n");
        printf("</tr>\n");
        printf("</table>\n");

	printf("<form class=\"registerform\" action=\"/cgi-bin/server.cgi\">\n");

        printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tbody>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">  序号：</td>\n");
	printf("<td><input type=\"text\" name=\"update_id\" style=\"width:110px;\" value=\"%s\" class=\"inputxt\" \
			datatype=\"n1-10\" nullmsg=\"请输入序号(唯一性)！\" errormsg=\"序号1-10位数字(唯一性)！\" /></td>\n",sql_server[0].id);
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">序号1-10位数字(唯一性)！</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">中心名称：</td>\n");
	printf("<td><input type=\"text\" name=\"update_hostname\" style=\"width:110px;\" value=\"%s\" class=\"inputxt\" \
			datatype=\"*1-32\" nullmsg=\"请输入中心名称！\" errormsg=\"中心名称1-32任意字符!\" /></td>\n",sql_server[0].hostname);
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">中心名称1-32任意字符!</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">IP地址：</td>\n");
	printf("<td><input type=\"text\" name=\"update_ip\" style=\"width:110px;\" value=\"%s\" class=\"inputxt\" \
			datatype=\"*1-32\" nullmsg=\"请输入IP地址！\" errormsg=\"IP地址1-32任意字符！\" /></td>\n",sql_server[0].ip);
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">IP地址1-32任意字符！</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">  端口号：</td>\n");
	printf("<td><input type=\"text\" name=\"update_port\" style=\"width:110px;\" value=\"%s\" class=\"inputxt\" \
			datatype=\"n1-10\" nullmsg=\"请输入端口号！\" errormsg=\"端口号（7000）!\" /></td>\n",sql_server[0].port);
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">端口号（7000）!</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("</tr>\n");
	printf("</table>\n");
	
	printf("<table class=\"table table-bordered\" style=\"margin-bottom:0px;\">\n");
	printf("<tr>\n");
	printf("<td width=\"13%%\" align=\"right\" nowrap=\"nowrap\" bgcolor=\"#f1f1f1\">使能位：</td>\n");
	printf("<td><input type=\"text\" name=\"update_enable\" style=\"width:110px;\" value=\"%s\" class=\"inputxt\" \
			datatype=\"n1-1\" nullmsg=\"请输入使能位！\" errormsg=\"使能位！\" /></td>\n",sql_server[0].enable);
        printf("<td>\n");
        printf("<div class=\"info\"><span class=\"Validform_checktip\">使能位！</span> \
			<span class=\"dec\"><s class=\"dec1\">&#9670;</s><s class=\"dec2\">&#9670;</s></span></div>\n");
        printf("</td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("</tbody>\n");
	printf("</table>\n");

	printf("<table class=\"margin-bottom-20  table no-border\">\n");
	printf("<tr>\n");
	
	printf("<td class=\"text-center\"><input type=\"submit\" value=\"保存\" name=\"UPDATE\" \
			class=\"btn btn-info  \" style=\"width:80px;\" /></td>\n");
	printf("</tr>\n");
	printf("</table>\n");

	printf("</form>\n");

	printf("</div>\n");
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
	printf("left:left-110,\n");
	printf("top:top\n");
	printf("}).show().animate({\n");
	printf("top:top-100\n");	
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


































