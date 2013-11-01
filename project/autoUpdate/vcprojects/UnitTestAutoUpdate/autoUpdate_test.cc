#include "gtest/gtest.h"
#include "../../src/auto_update/auto_update.h"
#include <iostream>
using namespace std;

CAutoUpdate *pAutoUpdate = new CAutoUpdate;

class MyAutoUpdateEvent:public IAutoUpdateEvent
{
	void RunEvent(int event_type) 
	{
		switch (event_type)
		{
		case DOWNLOAD_FILE_START:
			cout << "Download file Start...\n"; 
			break;
		case DOWNLOAD_FILE_STOP:
			cout << "Download file Stop...\n"; 
			break;
		case DOWNLOAD_FILE_DONE:
			cout << "Download file Done...\n"; 
			break;
		case DOWNLOAD_FILE_FAILED:
			cout << "Download file Failed...\n"; 
			break;
		case CHECK_FILE_START:
			cout << "Check file Start...\n"; 
			break;
		case CHECK_FILE_DONE:
			cout << "Check file Done...\n"; 
			break;
		case CHECK_FILE_FAILED:
			cout << "Check file Failed...\n"; 
			break;
		case UNZIP_FILE_START:
			cout << "Unzip file Start...\n"; 
			break;
		case UNZIP_FILE_DONE:
			cout << "Unzip file Done...\n"; 
			break;
		case UNZIP_FILE_FAILED:
			cout << "Unzip file Failed...\n"; 
			break;
		default:
			break;
		}
	}
	void DownLoadProgress(const double total, const double downloaded)
	{
		cout << "Total = " << total << ", downloaded = " << downloaded << endl;
	}
};

TEST(UserClientSDK, reentrant_autoUpdate)
{
	UPDATE_FILE_INFO updateFileInfo;
	//updateFileInfo.file_url = "http://download.tuxfamily.org/notepadplus/6.3.3/npp.6.3.3.bin.zip";
	//updateFileInfo.file_checksum = "628ea35aec4ec633e6963b33715a1c70";
	updateFileInfo.file_url = "http://60.208.139.11/jenkins/job/UpdatePacket/ws/config1101.zip";
	updateFileInfo.file_checksum = "6779ee768c62e723fc58e487046792d8";
	IAutoUpdateEvent *pAutoUpdateEvent = new MyAutoUpdateEvent;
	std::string download_path = "d:\\";
	std::string replace_path = "D:\\npp.6.3.3";

	pAutoUpdate->Init(updateFileInfo, download_path, replace_path);
	pAutoUpdate->SetAutoUpdateEvent(pAutoUpdateEvent);
	pAutoUpdate->Run();


	/*Sleep(60000);
	pAutoUpdate->StopDownloadFile();*/

}

//TEST(UserClientSDK, reentrant_CheckMD5)
//{
//	std::string source_file = "d:\\yy.zip";
//	std::string get_md5 = "828034e560b6abd8ccac2dd9095800ef";
//	ASSERT_TRUE(pAutoUpdate->CheckMD5(get_md5, source_file));
//}
//
//TEST(UserClientSDK, reentrant_UnzipFile)
//{
//
//	std::string source_file = "d:\\yy.zip";
//	std::string dest_path	= "d:\\yy";
//	pAutoUpdate->UnzipFile(source_file, dest_path);
//}
