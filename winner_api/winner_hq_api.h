#ifndef WINNER_API_SDF32DF_H_
#define WINNER_API_SDF32DF_H_

#ifdef  WINNER_API_EXPORTS
    #define WINNER_API_IMEXPORT  __declspec(dllexport)
#else
    #define WINNER_API_IMEXPORT  __declspec(dllimport)
#endif

//APIʹ������Ϊ: Ӧ�ó����ȵ���WinnerHisHq_Connect����Ӯ����ʷ������,Ȼ��ſ��Ե��������ӿڻ�ȡ��������,Ӧ�ó���Ӧ���д��������������, �ӿ����̰߳�ȫ��
//������ߣ���������api������api�᷵���Ѿ����ߵĴ�����Ϣ��Ӧ�ó���Ӧ���ݴ˴�����Ϣ�������ӷ�������

//3.������������˵��
/// <summary>
/// ����Ӯ�����������
/// </summary>
/// <param name="IP">������IP,����ȯ��ͨ���������¼���桰ͨѶ���á���ť�ڲ��</param>
/// <param name="Port">�������˿�</param>
/// <param name="Result">��APIִ�з��غ�  </param>
/// <param name="ErrInfo">��APIִ�з��غ�������������˴�����Ϣ˵����һ��Ҫ����256�ֽڵĿռ䡣û����ʱΪ���ַ�����</param>
/// <returns>�ɹ�����true, ʧ�ܷ���false</returns>
typedef bool (__stdcall* WinnerHisHq_ConnectDelegate)(char* IP, int Port, char* Result, char* ErrInfo);

/// <summary>
/// �Ͽ�ͬ������������
/// </summary>
typedef void(__stdcall* WinnerHisHq_DisconnectDelegate)();

/// <summary>
/// ��ȡ֤ȯָ����Χ�ĵ�K������
/// </summary>
/// <param name="Category">K������, 0->5����K�� 1->15����K�� 2->30����K�� 3->1СʱK�� 4->��K�� 5->��K�� 6->��K�� 7->1���� 8->1����K�� 9->��K�� 10->��K�� 11->��K��< / param>
/// <param name="Market">�г�����, 0->���� 1->�Ϻ�</param>
/// <param name="Zqdm">֤ȯ����</param>
/// <param name="Start">��Χ�Ŀ�ʼλ��,���һ��K��λ����0, ǰһ����1, ��������</param>
/// <param name="Count">��Χ�Ĵ�С��APIִ��ǰ,��ʾ�û�Ҫ�����K����Ŀ, APIִ�к�,������ʵ�ʷ��ص�K����Ŀ, ���ֵ800</param>
/// <param name="Result">��APIִ�з��غ�Result�ڱ����˷��صĲ�ѯ����, ��ʽΪ������ݣ�������֮��ͨ��\n�ַ��ָ������֮��ͨ��\t�ָ���һ��Ҫ����1024*1024�ֽڵĿռ䡣����ʱΪ���ַ�����</param>
/// <param name="ErrInfo">��APIִ�з��غ�������������˴�����Ϣ˵����һ��Ҫ����256�ֽڵĿռ䡣û����ʱΪ���ַ�����</param>
/// <returns>�ɹ�����true, ʧ�ܷ���false</returns>
typedef bool(__stdcall* TdxHq_GetSecurityBarsDelegate)(byte Category, byte Market, char* Zqdm, short Start, short& Count, char* Result, char* ErrInfo);
/// <summary>

#endif // WINNER_API_SDF32DF_H_