#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include "sqlite3.h"
#include <windows.h>
#define SUCCESS 0
#define ERROR   -1
using namespace std;
typedef struct{
    string modeId;
    string start_time;
    string end_time;
}ModeBroadCast;
ModeBroadCast m_ModeBroadCast[10];
int count;
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   cout<<argc<<"  "<<*argv<<" "<<endl;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      //cout<<argv[i]<<"  ";

   }
   printf("\n");
   return 0;
}

int QuerySQL(string DbName,string sql){
   sqlite3 *db;

   char *errmsg=NULL;
   char **dbResult; //是 char ** 类型，两个*号
   int nRow, nColumn;

   int rc;
   int index;
   rc = sqlite3_open((const char *)DbName.c_str(), &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return ERROR;
   }else{
      fprintf(stderr, "Opened database successfully\n");
   }
   rc=sqlite3_get_table(db,sql.c_str(),&dbResult, &nRow, &nColumn, &errmsg);
   if(rc==SUCCESS){
        index=nColumn;
        printf("查询到%d条记录\n",nRow);
        count=nRow;
        for(int i=0;i<nRow;i++){
            printf("第%d条记录\n",i+1);
            for(int j=0;j<nColumn;j++){
                printf("%s = %s\n",dbResult[j],dbResult[index]);
                if(j==0){
                    m_ModeBroadCast[i].modeId=dbResult[index];
                }
                if(j==1){
                    m_ModeBroadCast[i].start_time=dbResult[index];
                }
                if(j==2){
                    m_ModeBroadCast[i].end_time=dbResult[index];

                }
                ++index;
            }
            printf("----------\n");
        }
   }
   sqlite3_free_table(dbResult);
   sqlite3_close(db);
   return 0;
}

int ExcuteSQL(string DbName,string sql){
   sqlite3 *db;
   char *zErrMsg = 0;
   char **dbResult; //是 char ** 类型，两个*号
   int nRow, nColumn;

   int rc;
   int index;
   rc = sqlite3_open((const char *)DbName.c_str(), &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return ERROR;
   }else{
      fprintf(stderr, "Opened database successfully\n");
   }

   rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);


   if( rc != SQLITE_OK ){
   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "SQL successfully\n");
   }

   sqlite3_close(db);
   return 0;
}
void init(){
    unsigned char msg[1024]={0};
}

int ChangeStringToBCD(unsigned char *_Src,unsigned char *_Dst,int outlen)
{
	int i= 0;
	int j= 0;

	for(i=0,j=0;i<outlen*2;i++,j++)
	{
		_Dst[j]=((_Src[i]- 0x30)<<4 | (_Src[i+1]-0x30)) & 0xFF;
		i++;
	}
	return 0;
}

int main()
{
    int input;
    int modeID;
    char ModeStart[15]={0};
    char ModeEnd[15]={0};
    char tmp[32]={0};
    unsigned char flag=0;

    unsigned char msg[1024]={0x30,0x30,0x30,0x31,0x30,0x34,0x32,0x33,0x30,0x30,0x30,0x31,
                             0x32,0x30,0x31,0x36,0x31,0x32,0x31,0x39,0x31,0x34,0x31,0x34,
                             0x32,0x34,0x31,0x35,0x31};
// 单个模式广播
//30 30 30 31    记录数
//30 34 32 33    降级模式线路车站ID
//30 30 30 31    记录数
//32 30 31 36 31 32 31 39 31 34 31 34 32 34  发起时间
//31 35  降级运营模式ID
//31  降级运营模式开关

    memcpy(tmp,&msg[26],2);
    modeID=atoi(tmp);
    cout<<"modeID:"<<modeID<<endl;
    flag=msg[28]-0x30;
    if(flag){//1  模式打开
        memcpy(ModeStart,&msg[12],14);
        memset(ModeEnd,0x30,14);
    }else{
        memcpy(ModeEnd,&msg[12],14);
        memset(ModeStart,0x30,14);
    }
    cout<<"ModeStart:"<<ModeStart<<endl;
    cout<<"ModeEnd:"<<ModeEnd<<endl;
    unsigned char aa[8]={0};
    ChangeStringToBCD((unsigned char*)ModeStart,aa,7);
    for (int i = 0; i < 7; ++i)
    {
        //printf("aa[%d] = 0x%02X\n", i, aa[i]);
        printf("%02X",aa[i]);
    }
    cout<<endl;
    string m_DbName="test1.db";
    string sql="";
    string sql_format="";
    char sql_tmp[1024]={0};
    string tmp_end_time;
    string tmp_modeid;
    while(1){
        cout<<"************提示************"<<endl;
        cout<<"*1.建表"<<endl;
        cout<<"*2.插入数据"<<endl;
        cout<<"*3.查询"<<endl;
        cout<<"*4.修改"<<endl;
        cout<<"*5.删除"<<endl;
        cout<<"****************************"<<endl;
        cout<<"Please input:";
        cin>>input;
        switch(input){
            case 1:
                sql = "CREATE TABLE MODECAST("  \
                      "MODEID     TEXT       NOT NULL," \
                      "START_TIME       TEXT    NOT NULL," \
                      "END_TIME         TEXT     NOT NULL);";
                      ExcuteSQL(m_DbName,sql);
                break;
            case 2:

                sql ="INSERT INTO MODECAST(MODEID,START_TIME,END_TIME) "  \
                    "VALUES ('14','20190321134000','00000000000000'); " \
                    "INSERT INTO MODECAST(MODEID,START_TIME,END_TIME) "  \
                    "VALUES ('15','20190321094000','00000000000000'); " \
                    "INSERT INTO MODECAST (MODEID,START_TIME,END_TIME) "  \
                    "VALUES ('15','00000000000000','20190321103000'); "     \
                    "INSERT INTO MODECAST(MODEID,START_TIME,END_TIME) "  \
                    "VALUES ('03','20190319063000','00000000000000'); " \
                    "INSERT INTO MODECAST (MODEID,START_TIME,END_TIME) "  \
                    "VALUES ('03','00000000000000','20190319073000'); " \
                    "INSERT INTO MODECAST(MODEID,START_TIME,END_TIME)" \
                    "VALUES ('03','20190319120000','20190319130000');" \
                    "INSERT INTO MODECAST(MODEID,START_TIME,END_TIME)" \
                    "VALUES ('11','20190320120000','00000000000000');" \
                    "INSERT INTO MODECAST (MODEID,START_TIME,END_TIME)" \
                    "VALUES ('11','00000000000000','20190320150000');";
                    ExcuteSQL(m_DbName,sql);
                /*
                sql = "INSERT INTO MODECAST(MODEID,START_TIME,END_TIME) "  \
                    "VALUES (%d,\'%s\',\'%s\');" ;
                sprintf(sql_tmp,sql.c_str(),modeID,ModeStart,ModeEnd);
                cout<<"sql_tmp:"<<sql_tmp<<endl;
                sql=string(sql_tmp);
                */
                break;
            case 3:
                cout<<"MODEID    START_TIME    END_TIME"<<endl;
                sql = "SELECT * from MODECAST ORDER BY MODEID ASC;";
                //sql = "SELECT * from MODECAST where START_TIME='00000000000000' or END_TIME='00000000000000';";
                QuerySQL(m_DbName,sql);
                cout<<"count:"<<count<<endl;
                cout<<"MODEID    START_TIME    END_TIME"<<endl;
                for(int i=0;i<count;i++){
                    cout<<m_ModeBroadCast[i].modeId<<"    "<<m_ModeBroadCast[i].start_time<<"    "<<m_ModeBroadCast[i].end_time<<endl;
                }


                break;
            case 4:
                sql = "SELECT * from MODECAST where START_TIME='00000000000000' or END_TIME='00000000000000' ORDER BY MODEID ASC;";
                QuerySQL(m_DbName,sql);
                cout<<count<<endl;
                sql_format = "UPDATE MODECAST set END_TIME=\'%s\' where MODEID=\'%s\' and END_TIME='00000000000000';";
                if(count>=2){
                    for(int i=0;i<count;i++){
                        if(m_ModeBroadCast[i].start_time.compare("00000000000000")==0){
                            cout<<"i="<<i<<endl;
                            tmp_end_time=m_ModeBroadCast[i].end_time;
                            tmp_modeid=m_ModeBroadCast[i].modeId;
                            sql="DELETE FROM MODECAST WHERE MODEID='"+m_ModeBroadCast[i].modeId+"' and START_TIME='00000000000000';";
                            cout<<sql<<endl;
                            ExcuteSQL(m_DbName,sql);
                            sprintf(sql_tmp,sql_format.c_str(),tmp_end_time.c_str(),tmp_modeid.c_str());
                            cout<<"sql_tmp:"<<sql_tmp<<endl;
                            ExcuteSQL(m_DbName,string(sql_tmp));
                        }
                    }
                }
                QuerySQL(m_DbName,"SELECT * from MODECAST ORDER BY MODEID ASC;");
                cout<<"MODEID    START_TIME    END_TIME"<<endl;
                for(int i=0;i<count;i++){
                    cout<<m_ModeBroadCast[i].modeId<<"    "<<m_ModeBroadCast[i].start_time<<"    "<<m_ModeBroadCast[i].end_time<<endl;
                }

                //fprintf(sql_tmp,sql_format,)
                //ExcuteSQL(m_DbName,sql);
                break;
            case 5:
                sql = "DELETE from MODECAST;";
                ExcuteSQL(m_DbName,sql);
                break;
            default:
                break;
        }
    }
    return 0;
}
