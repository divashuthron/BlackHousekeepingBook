#pragma warning(disable: 4996)
#pragma comment(lib, "libmysql.lib")

#include "windows.h"
#include "mysql.h"
#include "TCHAR.H"
#include "time.h"
#include "stdio.h"
#include "stdlib.h"
#include "locale.h"

#include "resource.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg,
	WPARAM wParam, LPARAM
	lParam);

BOOL CALLBACK Main_Dialog_Proc(HWND mainDlg, UINT iMsg,
	WPARAM wParam, LPARAM
	lParam);

BOOL CALLBACK Income_Dialog_Proc(HWND incomeDlg, UINT iMsg,
	WPARAM wParam, LPARAM
	lParam);

BOOL CALLBACK Expense_Dialog_Proc(HWND expenseDlg, UINT iMsg,
	WPARAM wParam, LPARAM
	lParam);

BOOL CALLBACK Passbook_Dialog_Proc(HWND passbookDlg, UINT iMsg,
	WPARAM wParam, LPARAM
	lParam);

BOOL ConnectionMySQL();

BOOL GetUserID(TCHAR inputID[100]);

BOOL GetUserMoneyData(TCHAR inputID[100]);

BOOL PostUserCash(TCHAR inputID[100]);

BOOL PostUserPassbook(TCHAR inputID[100]);

BOOL ExpenseUserCash(TCHAR inputID[100]);

BOOL ExpenseUserPassbook(TCHAR inputID[100]);

BOOL GetIsOpen(TCHAR inputID[100]);

BOOL PostIsOpen(TCHAR inputID[100]);

HINSTANCE hInst;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int nCmdShow)
{
	HWND hwnd;
	MSG msg;
	WNDCLASS WndClass;
	hInst = hInstance;

	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = _T("housekeepingbook");
	RegisterClass(&WndClass);
	hwnd = CreateWindow(_T("housekeepingbook"),
		_T("[���� �����]"),
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		350,
		600,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

//ID ���� ����
static TCHAR id[100];

//�ݾ��� �� ����
static int totalMoney;
static int totalCash;
static int totalPassbook;
static int dailyIncome;
static int dailyExpense;

//���� ���� ���� Ȯ�� ����
static BOOL isOpen;

//������ ���� �̸�
static TCHAR bankName[100];

//�����ͺ��̽� ����
const char* host = "localhost";
const char* user = "root";
const char* pw = "0508";
const char* db = "bbs";

//MySQL ����ü
static MYSQL* connection = NULL;
static MYSQL conn;
static MYSQL_RES* sql_result;
static MYSQL_ROW sql_row;

BOOL ConnectionMySQL() {
	//MySQL �����ͺ��̽� ������ ����
	__try {
		mysql_init(&conn) == NULL;
	}
	//�������� �������� �ʾ��� ���
	__except (EXCEPTION_EXECUTE_HANDLER) {
		printf("���� : ������ ����");
	}

	printf("���� : ������ ����\n");
	
	//������ ���� ���� ��
	__try {
		//�����ͺ��̽� ���� ��û
		connection = mysql_real_connect(&conn, host, user, pw, db, 3306, (char*)NULL, 0);

		//���� ��û ��� �����ͺ��̽��� �������� ���� ���
		if (connection == NULL)
		{
			return FALSE;
		}
		//���� ������ �����Ǿ��� ���
		else {
			//bbs �����ͺ��̽� ����
			mysql_select_db(&conn, db);
			printf("���� : �����ͺ��̽� ����\n");
			return TRUE;
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		printf("���� : �����ͺ��̽� ����");
	}
	return FALSE;
}

BOOL GetUserID(TCHAR inputID[100])
{
	//SQL�� ����� ���ڿ� ���� ����
	char convertID[100];
	//ID ���� �޾ƿ� char Ÿ������ ��ȯ
	WideCharToMultiByte(CP_ACP, 0, inputID, 100, convertID, 100, NULL, NULL);

	printf("��ȯ�� ID�� : %s\n", convertID);

	//������, ���� ��� ���� ���� ����
	char query[1024] = "select id from housekeepingbook where id = '";

	strcat(query, convertID);
	strcat(query, "'");
	printf("������ ���빰 : %s\n", query);

	int state = 0;

	if (ConnectionMySQL()) {
		//������ ���� �� ���� ��� state ������ ����
		state = mysql_query(connection, query);
		printf("���� : ���� ��� state ������ ����\n");

		//���°��� 0�� ��� (������ ��ȸ ����� �����ϴ� ���)
		if (state == 0) {
			//RS �����Ϳ� ������ �� ��������
			sql_result = mysql_store_result(connection);
			printf("���� : RS �����Ϳ� ������ ���� ������\n");
			
			//������ ���� �����ϴµ� �������� ���
			if ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
				printf("���� : RS �����Ϳ� ������ ���� ����\n");
				if (strcmp(sql_row[0], convertID) == 0) {
					printf("���� : ����� ID�� ���� ����� ��ȸ�� ID ��\n");
					return TRUE;
				}
				else {
					printf("���� : ����� ID�� ���� ����� ��ȸ�� ID ��\n");
					return FALSE;
				}
			}
			else {
				printf("���� : RS �����Ϳ� ������ ���� ����\n");
				return FALSE;
			}
		}
		else {
			printf("���� : RS �����Ϳ� ������ ���� ������\n");
			return FALSE;
		}
	}
	else {
		printf("���� : ���� ��� state ������ ����\n");
		return FALSE;
	}
	return FALSE;
}

BOOL GetIsOpen(TCHAR inputID[100])
{
	//SQL�� ����� ���ڿ� ���� ����
	char convertID[100];
	//ID ���� �޾ƿ� char Ÿ������ ��ȯ
	WideCharToMultiByte(CP_ACP, 0, inputID, 100, convertID, 100, NULL, NULL);

	printf("��ȯ�� ID�� : %s\n", convertID);

	//������, ���� ��� ���� ���� ����
	char query[1024] = "select isopen from housekeepingbook where id = '";

	strcat(query, convertID);
	strcat(query, "'");
	printf("������ ���빰 : %s\n", query);

	int state = 0;

	if (ConnectionMySQL()) {
		//������ ���� �� ���� ��� state ������ ����
		state = mysql_query(connection, query);
		printf("���� : ���� ��� state ������ ����\n");

		//���°��� 0�� ��� (������ ��ȸ ����� �����ϴ� ���)
		if (state == 0) {
			//RS �����Ϳ� ������ �� ��������
			sql_result = mysql_store_result(connection);
			printf("���� : RS �����Ϳ� ������ ���� ������\n");

			//������ ���� �����ϴµ� �������� ���
			if ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
				printf("���� : RS �����Ϳ� ������ ���� ����\n");
				if (sql_row[0] == 0) {
					printf("���� : ������ ���� �Ǿ��ֽ��ϴ�.\n");
					return TRUE;
				}
				else {
					printf("���� : ������ ���� �Ǿ����� �ʽ��ϴ�.\n");
					return FALSE;
				}
			}
			else {
				printf("���� : RS �����Ϳ� ������ ���� ����\n");
				return FALSE;
			}
		}
		else {
			printf("���� : RS �����Ϳ� ������ ���� ������\n");
			return FALSE;
		}
	}
	else {
		printf("���� : ���� ��� state ������ ����\n");
		return FALSE;
	}
	return FALSE;
}

BOOL PostIsOpen(TCHAR inputID[100])
{
	//SQL�� ����� ���ڿ� ���� ����
	char convertID[100];
	char cBankName[100];
	char cTotalMoney[100];
	char cTotalPassbook[100];
	char cDailyIncome[100];
	//ID ���� �޾ƿ� char Ÿ������ ��ȯ
	WideCharToMultiByte(CP_ACP, 0, inputID, 100, convertID, 100, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, bankName, 100, cBankName, 100, NULL, NULL);

	printf("��ȯ�� ID�� : %s\n", convertID);
	printf("��ȯ�� ���� �̸��� : %s\n", cBankName);

	//������, ���� ��� ���� ���� ����
	char query[1024] = "update housekeepingbook set totalmoney = '";

	//int to char
	itoa(totalMoney, cTotalMoney, 10);
	itoa(totalPassbook, cTotalPassbook, 10);
	itoa(dailyIncome, cDailyIncome, 10);

	//������ ������ �� �̾���̱�
	strcat(query, cTotalMoney);
	strcat(query, "', totalPassbook= '");
	strcat(query, cTotalPassbook);
	strcat(query, "', dailyIncome= '");
	strcat(query, cDailyIncome);
	strcat(query, "', isOpen= true, bankName='");
	strcat(query, cBankName);
	strcat(query, "' where id = '");
	strcat(query, convertID);
	strcat(query, "'");
	printf("������ ���빰 : %s\n", query);

	int state = 0;

	if (ConnectionMySQL()) {
		//������ ���� �� ���� ��� state ������ ����
		state = mysql_query(connection, query);
		printf("���� : ���� ��� state ������ ����\n");

		//���°��� 0�� ��� (������ �� ������ ������ ���)
		if (state == 0) {
			printf("���� : ���� ���\n");
			return TRUE;
		}
		else {
			printf("���� : ���� ���\n");
			return FALSE;
		}
	}
	else {
		printf("���� : ���� ��� state ������ ����\n");
	}
	return FALSE;
}

BOOL GetUserMoneyData(TCHAR inputID[100])
{
	//SQL�� ����� ���ڿ� ���� ����
	char convertID[100];
	//ID ���� �޾ƿ� char Ÿ������ ��ȯ
	WideCharToMultiByte(CP_ACP, 0, inputID, 100, convertID, 100, NULL, NULL);

	printf("��ȯ�� ID�� : %s\n", convertID);

	//������, ���� ��� ���� ���� ����
	char query[1024] = "select totalMoney, totalCash, totalPassbook, dailyIncome, dailyExpense from housekeepingbook where id = '";

	strcat(query, convertID);
	strcat(query, "'");
	printf("������ ���빰 : %s\n", query);

	int state = 0;

	if (ConnectionMySQL()) {
		//������ ���� �� ���� ��� state ������ ����
		state = mysql_query(connection, query);
		printf("���� : ���� ��� state ������ ����\n");

		//���°��� 0�� ��� (������ ��ȸ ����� �����ϴ� ���)
		if (state == 0) {
			//RS �����Ϳ� ������ �� ��������
			sql_result = mysql_store_result(connection);
			printf("���� : RS �����Ϳ� ������ ���� ������\n");

			//������ ���� �����ϴµ� �������� ���
			while ((sql_row = mysql_fetch_row(sql_result)) != NULL) {
				printf("���� : RS �����Ϳ� ������ ���� ����\n");
				totalMoney =  atoi(sql_row[0]);
				totalCash = atoi(sql_row[1]);
				totalPassbook = atoi(sql_row[2]);
				dailyIncome = atoi(sql_row[3]);
				dailyExpense = atoi(sql_row[4]);
			}
		}
		else {
			printf("���� : RS �����Ϳ� ������ ���� ������\n");
			return FALSE;
		}
	}
	else {
		printf("���� : ���� ��� state ������ ����\n");
		return FALSE;
	}
	return FALSE;
}

BOOL PostUserCash(TCHAR inputID[100])
{
	//SQL�� ����� ���ڿ� ���� ����
	char convertID[100];
	char cTotalMoney[100];
	char cTotalCash[100];
	char cDailyIncome[100];
	//ID ���� �޾ƿ� char Ÿ������ ��ȯ
	WideCharToMultiByte(CP_ACP, 0, inputID, 100, convertID, 100, NULL, NULL);

	printf("��ȯ�� ID�� : %s\n", convertID);

	//������, ���� ��� ���� ���� ����
	char query[1024] = "update housekeepingbook set totalmoney = '";

	//int to char
	itoa(totalMoney, cTotalMoney, 10);
	itoa(totalCash, cTotalCash, 10);
	itoa(dailyIncome, cDailyIncome, 10);

	//������ ������ �� �̾���̱�
	strcat(query, cTotalMoney);
	strcat(query, "', totalcash= '");
	strcat(query, cTotalCash);
	strcat(query, "', dailyIncome= '");
	strcat(query, cDailyIncome);
	strcat(query, "' where id = '");
	strcat(query, convertID);
	strcat(query, "'");
	printf("������ ���빰 : %s\n", query);

	int state = 0;

	if (ConnectionMySQL()) {
		//������ ���� �� ���� ��� state ������ ����
		state = mysql_query(connection, query);
		printf("���� : ���� ��� state ������ ����\n");

		//���°��� 0�� ��� (������ �� ������ ������ ���)
		if (state == 0) {
			printf("���� : ���� ���\n");
			return TRUE;
		}
		else {
			printf("���� : ���� ���\n");
			return FALSE;
		}
	}
	else {
		printf("���� : ���� ��� state ������ ����\n");
	}
	return FALSE;
}

BOOL PostUserPassbook(TCHAR inputID[100])
{
	//SQL�� ����� ���ڿ� ���� ����
	char convertID[100];
	char cTotalMoney[100];
	char cTotalPassbook[100];
	char cDailyIncome[100];
	//ID ���� �޾ƿ� char Ÿ������ ��ȯ
	WideCharToMultiByte(CP_ACP, 0, inputID, 100, convertID, 100, NULL, NULL);

	printf("��ȯ�� ID�� : %s\n", convertID);

	//������, ���� ��� ���� ���� ����
	char query[1024] = "update housekeepingbook set totalmoney = '";

	//int to char
	itoa(totalMoney, cTotalMoney, 10);
	itoa(totalPassbook, cTotalPassbook, 10);
	itoa(dailyIncome, cDailyIncome, 10);

	//������ ������ �� �̾���̱�
	strcat(query, cTotalMoney);
	strcat(query, "', totalPassbook= '");
	strcat(query, cTotalPassbook);
	strcat(query, "', dailyIncome= '");
	strcat(query, cDailyIncome);
	strcat(query, "' where id = '");
	strcat(query, convertID);
	strcat(query, "'");
	printf("������ ���빰 : %s\n", query);

	int state = 0;

	if (ConnectionMySQL()) {
		//������ ���� �� ���� ��� state ������ ����
		state = mysql_query(connection, query);
		printf("���� : ���� ��� state ������ ����\n");

		//���°��� 0�� ��� (������ �� ������ ������ ���)
		if (state == 0) {
			printf("���� : ���� ���\n");
			return TRUE;
		}
		else {
			printf("���� : ���� ���\n");
			return FALSE;
		}
	}
	else {
		printf("���� : ���� ��� state ������ ����\n");
	}
	return FALSE;
}

BOOL ExpenseUserCash(TCHAR inputID[100])
{
	//SQL�� ����� ���ڿ� ���� ����
	char convertID[100];
	char cTotalMoney[100];
	char cTotalCash[100];
	char cDailyExpense[100];
	//ID ���� �޾ƿ� char Ÿ������ ��ȯ
	WideCharToMultiByte(CP_ACP, 0, inputID, 100, convertID, 100, NULL, NULL);

	printf("��ȯ�� ID�� : %s\n", convertID);

	//������, ���� ��� ���� ���� ����
	char query[1024] = "update housekeepingbook set totalmoney = '";

	//int to char
	itoa(totalMoney, cTotalMoney, 10);
	itoa(totalCash, cTotalCash, 10);
	itoa(dailyExpense, cDailyExpense, 10);

	//������ ������ �� �̾���̱�
	strcat(query, cTotalMoney);
	strcat(query, "', totalcash= '");
	strcat(query, cTotalCash);
	strcat(query, "', dailyExpense= '");
	strcat(query, cDailyExpense);
	strcat(query, "' where id = '");
	strcat(query, convertID);
	strcat(query, "'");
	printf("������ ���빰 : %s\n", query);

	int state = 0;

	if (ConnectionMySQL()) {
		//������ ���� �� ���� ��� state ������ ����
		state = mysql_query(connection, query);
		printf("���� : ���� ��� state ������ ����\n");

		//���°��� 0�� ��� (������ �� ������ ������ ���)
		if (state == 0) {
			printf("���� : ���� ���\n");
			return TRUE;
		}
		else {
			printf("���� : ���� ���\n");
			return FALSE;
		}
	}
	else {
		printf("���� : ���� ��� state ������ ����\n");
	}
	return FALSE;
}

BOOL ExpenseUserPassbook(TCHAR inputID[100])
{
	//SQL�� ����� ���ڿ� ���� ����
	char convertID[100];
	char cTotalMoney[100];
	char cTotalPassbook[100];
	char cDailyExpense[100];
	//ID ���� �޾ƿ� char Ÿ������ ��ȯ
	WideCharToMultiByte(CP_ACP, 0, inputID, 100, convertID, 100, NULL, NULL);

	printf("��ȯ�� ID�� : %s\n", convertID);

	//������, ���� ��� ���� ���� ����
	char query[1024] = "update housekeepingbook set totalmoney = '";

	//int to char
	itoa(totalMoney, cTotalMoney, 10);
	itoa(totalPassbook, cTotalPassbook, 10);
	itoa(dailyExpense, cDailyExpense, 10);

	//������ ������ �� �̾���̱�
	strcat(query, cTotalMoney);
	strcat(query, "', totalPassbook= '");
	strcat(query, cTotalPassbook);
	strcat(query, "', dailyExpense= '");
	strcat(query, cDailyExpense);
	strcat(query, "' where id = '");
	strcat(query, convertID);
	strcat(query, "'");
	printf("������ ���빰 : %s\n", query);

	int state = 0;

	if (ConnectionMySQL()) {
		//������ ���� �� ���� ��� state ������ ����
		state = mysql_query(connection, query);
		printf("���� : ���� ��� state ������ ����\n");

		//���°��� 0�� ��� (������ �� ������ ������ ���)
		if (state == 0) {
			printf("���� : ���� ���\n");
			return TRUE;
		}
		else {
			printf("���� : ���� ���\n");
			return FALSE;
		}
	}
	else {
		printf("���� : ���� ��� state ������ ����\n");
	}
	return FALSE;
}

BOOL CALLBACK Main_Dialog_Proc(HWND mainDlg, UINT iMsg,
	WPARAM wParam, LPARAM
	lParam)
{
	static HDC hdc;

	static HBRUSH dlgBrush;
	static HBRUSH staticBrush;

	TCHAR timeStr[128];
	SYSTEMTIME st;

	switch (iMsg) {
	case WM_INITDIALOG:
		GetLocalTime(&st);

		//����� ��� ��ȸ
		GetUserMoneyData(id);

		wsprintf(timeStr, _T("%d. %d. %d"), st.wYear, st.wMonth, st.wDay);

		SetDlgItemInt(mainDlg, EDIT_TOTAL, totalMoney, FALSE);
		SetDlgItemInt(mainDlg, EDIT_CASH, totalCash, FALSE);
		SetDlgItemInt(mainDlg, EDIT_PASSBOOK, totalPassbook, FALSE);
		SetDlgItemInt(mainDlg, EDIT_TOTAL_INCOME, dailyIncome, FALSE);
		SetDlgItemInt(mainDlg, EDIT_TOTAL_EXPENSE, dailyExpense, FALSE);

		SetDlgItemText(mainDlg, TEXT_TODAY, timeStr);
		return 1;

	case WM_CTLCOLORSTATIC:
		if (staticBrush) { 
			DeleteObject(staticBrush); 
			staticBrush = NULL; 
		}

		SetBkColor((HDC)wParam, RGB(35, 35, 35));
		SetTextColor((HDC)wParam, RGB(255, 255, 255));
		return (BOOL)(staticBrush = CreateSolidBrush(RGB(35, 35, 35)));

	case WM_CTLCOLORDLG:
		if (dlgBrush) {
			DeleteObject(dlgBrush);
			dlgBrush = NULL;
		}

		SetBkColor((HDC)wParam, RGB(35, 35, 35));
		SetTextColor((HDC)wParam, RGB(255, 255, 255));
		return (BOOL)(dlgBrush = CreateSolidBrush(RGB(35, 35, 35)));

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_NEW_INCOME:
			DialogBox(hInst, MAKEINTRESOURCE(Income_Dialog_View), mainDlg, Income_Dialog_Proc);
			
			SetDlgItemInt(mainDlg, EDIT_TOTAL, totalMoney, FALSE);
			SetDlgItemInt(mainDlg, EDIT_CASH, totalCash, FALSE);
			SetDlgItemInt(mainDlg, EDIT_PASSBOOK, totalPassbook, FALSE);
			SetDlgItemInt(mainDlg, EDIT_TOTAL_INCOME, dailyIncome, FALSE);
			break;
			
		case ID_NEW_EXPENSE:
			DialogBox(hInst, MAKEINTRESOURCE(Expense_Dialog_View), mainDlg, Expense_Dialog_Proc);

			SetDlgItemInt(mainDlg, EDIT_TOTAL, totalMoney, FALSE);
			SetDlgItemInt(mainDlg, EDIT_CASH, totalCash, FALSE);
			SetDlgItemInt(mainDlg, EDIT_PASSBOOK, totalPassbook, FALSE);
			SetDlgItemInt(mainDlg, EDIT_TOTAL_EXPENSE, dailyExpense, FALSE);
			break;

		case ID_NEW_PASSBOOK:
			if (GetIsOpen(id) == true) {
				MessageBox(mainDlg, _T("�̹� ������ �����ϼ̽��ϴ�."), _T("���"), MB_OK);
				break;
			}
			else {
				DialogBox(hInst, MAKEINTRESOURCE(Passbook_Dialog_View), mainDlg, Passbook_Dialog_Proc);

				SetDlgItemInt(mainDlg, EDIT_TOTAL, totalMoney, FALSE);
				SetDlgItemInt(mainDlg, EDIT_PASSBOOK, totalPassbook, FALSE);
				SetDlgItemInt(mainDlg, EDIT_TOTAL_INCOME, dailyIncome, FALSE);
			}
			break;

		case ID_EXIT:
			if (staticBrush)
				DeleteObject(staticBrush);
			if (dlgBrush)
				DeleteObject(dlgBrush);
			EndDialog(mainDlg, 0);
			break;
		}
	}
	return 0;
}

BOOL CALLBACK Income_Dialog_Proc(HWND incomeDlg, UINT iMsg,
	WPARAM wParam, LPARAM
	lParam) 
{
	static HDC hdc;
	static int checked;

	static HBRUSH dlgBrush;
	static HBRUSH staticBrush;

	switch (iMsg) {
	case WM_INITDIALOG:
		CheckRadioButton(incomeDlg, RADIO_INCOME_CASH, RADIO_INCOME_PASSBOOK, RADIO_INCOME_CASH);
		return 1;

	case WM_CTLCOLORSTATIC:
		if (staticBrush) {
			DeleteObject(staticBrush);
			staticBrush = NULL;
		}

		SetBkColor((HDC)wParam, RGB(35, 35, 35));
		SetTextColor((HDC)wParam, RGB(255, 255, 255));
		return (BOOL)(staticBrush = CreateSolidBrush(RGB(35, 35, 35)));

	case WM_CTLCOLORDLG:
		if (dlgBrush) {
			DeleteObject(dlgBrush);
			dlgBrush = NULL;
		}

		SetBkColor((HDC)wParam, RGB(35, 35, 35));
		SetTextColor((HDC)wParam, RGB(255, 255, 255));
		return (BOOL)(dlgBrush = CreateSolidBrush(RGB(35, 35, 35)));

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case RADIO_INCOME_CASH:
			checked = 0;
			break;

		case RADIO_INCOME_PASSBOOK:
			if (GetIsOpen(id) == TRUE) {
				checked = 1;
			}
			else {
				MessageBox(incomeDlg, _T("������ �������� �ʾҽ��ϴ�."), _T("���"), MB_OK);
				CheckRadioButton(incomeDlg, RADIO_INCOME_CASH, RADIO_INCOME_PASSBOOK, RADIO_INCOME_CASH);
				checked = 0;
			}
			break;

		case BTN_INCOME_DONE:
			if (GetDlgItemInt(incomeDlg, EDIT_INCOME_VALUE, NULL, FALSE) == 0) {
				MessageBox(incomeDlg, _T("�Էµ� ���� �����ϴ�."), _T("���"), MB_OK);
				SetDlgItemInt(incomeDlg, EDIT_INCOME_VALUE, NULL, FALSE);
				return 0;
			}
			if (checked == 0) {
				totalCash += GetDlgItemInt(incomeDlg, EDIT_INCOME_VALUE, NULL, FALSE);
				totalMoney += GetDlgItemInt(incomeDlg, EDIT_INCOME_VALUE, NULL, FALSE);
				dailyIncome += GetDlgItemInt(incomeDlg, EDIT_INCOME_VALUE, NULL, FALSE);
				PostUserCash(id);
				MessageBox(incomeDlg, _T("������ ��ϵǾ����ϴ�."), _T("��� ���"), MB_OK);
				EndDialog(incomeDlg, 0);
			}
			else if (checked == 1 && GetIsOpen(id) == TRUE) {
				totalPassbook += GetDlgItemInt(incomeDlg, EDIT_INCOME_VALUE, NULL, FALSE);
				totalMoney += GetDlgItemInt(incomeDlg, EDIT_INCOME_VALUE, NULL, FALSE);
				dailyIncome += GetDlgItemInt(incomeDlg, EDIT_INCOME_VALUE, NULL, FALSE);
				PostUserPassbook(id);
				MessageBox(incomeDlg, _T("������ ��ϵǾ����ϴ�."), _T("��� ���"), MB_OK);
				EndDialog(incomeDlg, 0);
			}
			else {
				MessageBox(incomeDlg, _T("������ �������� �ʾҽ��ϴ�."), _T("���"), MB_OK);
				return 0;
			}
			break;

		case BTN_INCOME_CALCEL:
			if (staticBrush)
				DeleteObject(staticBrush);
			if (dlgBrush)
				DeleteObject(dlgBrush);
			EndDialog(incomeDlg, 0);
			break;
		}
		break;
	}
	return 0;
}

BOOL CALLBACK Expense_Dialog_Proc(HWND expenseDlg, UINT iMsg,
	WPARAM wParam, LPARAM
	lParam) 
{
	static HDC hdc;
	static int checked;

	static HBRUSH dlgBrush;
	static HBRUSH staticBrush;

	switch (iMsg) {
	case WM_INITDIALOG:
		CheckRadioButton(expenseDlg, RADIO_EXPENSE_CASH, RADIO_EXPENSE_PASSBOOK, RADIO_EXPENSE_CASH);
		return 1;

	case WM_CTLCOLORSTATIC:
		if (staticBrush) {
			DeleteObject(staticBrush);
			staticBrush = NULL;
		}

		SetBkColor((HDC)wParam, RGB(35, 35, 35));
		SetTextColor((HDC)wParam, RGB(255, 255, 255));
		return (BOOL)(staticBrush = CreateSolidBrush(RGB(35, 35, 35)));

	case WM_CTLCOLORDLG:
		if (dlgBrush) {
			DeleteObject(dlgBrush);
			dlgBrush = NULL;
		}

		SetBkColor((HDC)wParam, RGB(35, 35, 35));
		SetTextColor((HDC)wParam, RGB(255, 255, 255));
		return (BOOL)(dlgBrush = CreateSolidBrush(RGB(35, 35, 35)));

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case RADIO_EXPENSE_CASH:
			checked = 0;
			break;

		case RADIO_EXPENSE_PASSBOOK:
			if (GetIsOpen(id) == true) {
				checked = 1;
			}
			else {
				MessageBox(expenseDlg, _T("������ �������� �ʾҽ��ϴ�."), _T("���"), MB_OK);
				CheckRadioButton(expenseDlg, RADIO_EXPENSE_CASH, RADIO_EXPENSE_PASSBOOK, RADIO_EXPENSE_CASH);
				checked = 0;
			}
			break;

		case BTN_EXPENSE_DONE:
			if (GetDlgItemInt(expenseDlg, EDIT_EXPENSE_VALUE, NULL, FALSE) == 0) {
				MessageBox(expenseDlg, _T("�Էµ� ���� �����ϴ�."), _T("���"), MB_OK);
				SetDlgItemInt(expenseDlg, EDIT_EXPENSE_VALUE, NULL, FALSE);
				return 0;
			}
			if (checked == 0) {
				totalCash -= GetDlgItemInt(expenseDlg, EDIT_EXPENSE_VALUE, NULL, FALSE);
				totalMoney -= GetDlgItemInt(expenseDlg, EDIT_EXPENSE_VALUE, NULL, FALSE);
				dailyExpense += GetDlgItemInt(expenseDlg, EDIT_EXPENSE_VALUE, NULL, FALSE);

				if (totalCash < 0) {
					totalCash = 0;
				}
				if (totalMoney < 0) {
					totalMoney = 0;
				}
				ExpenseUserCash(id);
				MessageBox(expenseDlg, _T("������ ��ϵǾ����ϴ�."), _T("��� ���"), MB_OK);
				EndDialog(expenseDlg, 0);
			}
			else if (checked == 1 && GetIsOpen(id) == TRUE) {
				totalPassbook -= GetDlgItemInt(expenseDlg, EDIT_EXPENSE_VALUE, NULL, FALSE);
				totalMoney -= GetDlgItemInt(expenseDlg, EDIT_EXPENSE_VALUE, NULL, FALSE);
				dailyExpense += GetDlgItemInt(expenseDlg, EDIT_EXPENSE_VALUE, NULL, FALSE);

				if (totalPassbook < 0) {
					totalPassbook = 0;
				}
				if (totalMoney < 0) {
					totalMoney = 0;
				}
				ExpenseUserPassbook(id);
				MessageBox(expenseDlg, _T("������ ��ϵǾ����ϴ�."), _T("��� ���"), MB_OK);
				EndDialog(expenseDlg, 0);
			}
			else {
				MessageBox(expenseDlg, _T("������ �������� �ʾҽ��ϴ�."), _T("���"), MB_OK);
				return 0;
			}
			break;

		case BTN_EXPENSE_CALCEL:
			if (staticBrush)
				DeleteObject(staticBrush);
			if (dlgBrush)
				DeleteObject(dlgBrush);
			EndDialog(expenseDlg, 0);
			break;
		}
	}
	return 0;
}

BOOL CALLBACK Passbook_Dialog_Proc(HWND passbookDlg, UINT iMsg,
	WPARAM wParam, LPARAM
	lParam)
{
	int i;
	static HDC hdc;

	static HWND bankCombo;
	TCHAR bankList[][20] = { _T("NHä��"), _T("��������") };
	TCHAR name[100];

	static HBRUSH dlgBrush;
	static HBRUSH staticBrush;

	switch (iMsg) {
	case WM_INITDIALOG:
		bankCombo = GetDlgItem(passbookDlg, COMBO_PASSBOOK_BANK);

		for (i = 0; i < 2; i++) {
			SendMessage(bankCombo, CB_ADDSTRING, 0, (LPARAM)bankList[i]);
		}
		SetWindowText(bankCombo, bankList[0]);
		return 1;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {

		case BTN_PASSBOOK_DONE:
			if (GetDlgItemInt(passbookDlg, EDIT_PASSBOOK_MONEY, NULL, FALSE) == 0) {
				MessageBox(passbookDlg, _T("�Էµ� ���� �����ϴ�."), _T("���"), MB_OK);
				SetDlgItemInt(passbookDlg, EDIT_PASSBOOK_MONEY, NULL, FALSE);
				break;
			}

			GetDlgItemText(passbookDlg, COMBO_PASSBOOK_BANK, name, 100);

			if (_tcscmp(name, _T(""))) {
				isOpen = TRUE;
				_tcscpy(bankName, name);
				totalPassbook = GetDlgItemInt(passbookDlg, EDIT_PASSBOOK_MONEY, NULL, FALSE);
				totalMoney += GetDlgItemInt(passbookDlg, EDIT_PASSBOOK_MONEY, NULL, FALSE);
				dailyIncome += GetDlgItemInt(passbookDlg, EDIT_PASSBOOK_MONEY, NULL, FALSE);
				PostIsOpen(id);
				MessageBox(passbookDlg, _T("������ ��ϵǾ����ϴ�."), _T("��� ���"), MB_OK);
				EndDialog(passbookDlg, 0);
				break;
			}
			break;

		case BTN_PASSBOOK_CALCEL:
			if (staticBrush)
				DeleteObject(staticBrush);
			if (dlgBrush)
				DeleteObject(dlgBrush);
			EndDialog(passbookDlg, 0);
			break;
		}
		break;

	case WM_CTLCOLORSTATIC:
		if (staticBrush) {
			DeleteObject(staticBrush);
			staticBrush = NULL;
		}

SetBkColor((HDC)wParam, RGB(35, 35, 35));
SetTextColor((HDC)wParam, RGB(255, 255, 255));
return (BOOL)(staticBrush = CreateSolidBrush(RGB(35, 35, 35)));

	case WM_CTLCOLORDLG:
		if (dlgBrush) {
			DeleteObject(dlgBrush);
			dlgBrush = NULL;
		}

		SetBkColor((HDC)wParam, RGB(35, 35, 35));
		SetTextColor((HDC)wParam, RGB(255, 255, 255));
		return (BOOL)(dlgBrush = CreateSolidBrush(RGB(35, 35, 35)));
	}
	return 0;
}

#define IDC_BUTTON_LOGIN 1000
#define IDC_EDITTEXT_ID 1001

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg,
	WPARAM wParam, LPARAM
	lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static RECT rect;

	HWND btnLogin;
	HWND edtID;

	static HBRUSH btnBrush;
	static HBRUSH staticBrush;

	HFONT nanumFont, OldTextFont;

	switch (iMsg)
	{
	case WM_CREATE:
		GetWindowRect(hwnd, &rect);

		AllocConsole(); //�ܼ�â ����
		_tfreopen(_T("CONOUT$"), _T("w"), stdout);
		_tfreopen(_T("CONIN$"), _T("r"), stdin);
		_tfreopen(_T("CONERR$"), _T("w"), stderr);
		_tsetlocale(LC_ALL, _T(""));
		printf("START\n");

		MoveWindow(hwnd, 0, 0, 350, 600, TRUE);

		btnLogin = CreateWindow(
			_T("button"),
			_T("�α���"),
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			rect.left + 20,
			rect.top + 100,
			80,
			25,
			hwnd,
			(HMENU)IDC_BUTTON_LOGIN,
			hInst,
			NULL
		);

		edtID = CreateWindow(
			_T("edit"),
			_T(""),
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			rect.left + 20,
			rect.top + 65,
			150,
			25,
			hwnd,
			(HMENU)IDC_EDITTEXT_ID,
			hInst,
			NULL
		);
		return 0;

	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMaxTrackSize.x = 350;
		((MINMAXINFO*)lParam)->ptMaxTrackSize.y = 650;
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 350;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 650;
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_LOGIN:
			GetDlgItemText(hwnd, IDC_EDITTEXT_ID, id, 100);

			if(GetUserID(id)) {
				DialogBox(hInst, MAKEINTRESOURCE(Main_Dialog_View), hwnd, Main_Dialog_Proc);
			}
			else {
				MessageBox(hwnd, _T("�������� ���� ������Դϴ�."), _T("���"), MB_OK);
				break;
			}
			return 0;
		}
		return 0;


	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		SetTextColor(hdc, RGB(255, 255, 255));
		SetBkColor(hdc, RGB(65, 65, 65));
		nanumFont = CreateFont(32, 0, 0, 0, 1000, 0, 0, 0, HANGEUL_CHARSET, 0, 0, 0,
			VARIABLE_PITCH | FF_ROMAN, TEXT("�������"));
		OldTextFont = (HFONT)SelectObject(hdc, nanumFont);
		TextOut(hdc, rect.left + 20, rect.top + 20, _T("���� �����"), _tcslen(_T("���� �����")));
		SelectObject(hdc, OldTextFont);
		DeleteObject(nanumFont);

		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		FreeConsole(); //�ܼ�â ����
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}
