struct table  
{  
    string name;  
    string createSQL;  
};  
  
//获取表信息  
void GetTables(vector<table>& vecTable)  
{  
    char *szError= new char[256];  
    sqlite3_stmt *stmt = NULL;  
    sqlite3_prepare((sqlite3*)m_pDb,"select name,sql from sqlite_master where type='table'  order by name",-1,&stmt,0);   
    vector<string> vecTables;  
    if(stmt)   
    {  
        while(sqlite3_step(stmt) == SQLITE_ROW)   
        {         
            table tb;  
            tb.name =(char *)sqlite3_column_text(stmt,0);  
            tb.createSQL = (char *)sqlite3_column_text(stmt,1);  
            vecTable.push_back(tb);  
        }  
        sqlite3_finalize(stmt);  
        stmt = NULL;  
    }  
}  
  
//获取列名  
bool GetColName(vector<string>& vecColName, string strTableName)  
{  
    sqlite3_stmt *stmt = NULL;  
    char sql[200];  
    sprintf(sql, "SELECT * FROM %s limit 0,1", strTableName.c_str());  
    char **pRes=NULL;  
    int nRow=0, nCol=0;  
    char *pErr=NULL;  
  
    //第一行是列名称  
    sqlite3_get_table((sqlite3*)m_pDb, sql, &pRes, &nRow, &nCol, &pErr);      
    for (int i=0; i<nRow; i++)  
    {  
        for (int j=0; j<nCol; j++)  
        {  
            char *pv = *(pRes+nCol*i+j);  
            vecColName.push_back(pv);  
        }  
        break;  
    }  
  
    if (pErr!=NULL)  
    {  
        sqlite3_free(pErr);  
    }  
    sqlite3_free_table(pRes);  
    return true;  
}  
  
  
//获取列类型  
bool GetColType(vector<int>& vecType, string strTableName)  
{  
    sqlite3_stmt *stmt = NULL;  
    char sql[200];  
    sprintf(sql, "SELECT * FROM %s limit 0,1", strTableName.c_str());  
    sqlite3_prepare((sqlite3*)m_pDb, sql,-1,&stmt,0);     
    if(stmt)   
    {  
        while(sqlite3_step(stmt) == SQLITE_ROW)   
        {     
            int nCount = sqlite3_column_count(stmt);  
            for (int i=0; i<nCount; i++)  
            {  
                int nValue = sqlite3_column_int(stmt,0);  
                int nType = sqlite3_column_type(stmt, i);  
                vecType.push_back(nType);  
                switch (nType)  
                {  
                case 1:  
                    //SQLITE_INTEGER  
                    break;  
                case 2:  
                    //SQLITE_FLOAT  
                    break;  
                case 3:  
                    //SQLITE_TEXT  
                    break;  
                case 4:  
                    //SQLITE_BLOB  
                    break;  
                case 5:  
                    //SQLITE_NULL  
                    break;  
                }  
            }  
            break;  
        }  
        sqlite3_finalize(stmt);  
        stmt = NULL;  
    }  
    return true;  
}  