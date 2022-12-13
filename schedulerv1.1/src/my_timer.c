#include <windows.h>
#include "my_timer.h"

#pragma comment(lib,"winmm.lib") //����winmm.lib��ý���

/* ȫ�ֱ��� */
HANDLE mainhandle;     //���߳̾��
CONTEXT Context;     //���߳��л�������
static void (*TimerCallFun)(void);  //�����û����ú���ָ��

/* �������� */
static void __stdcall TimerISR(unsigned int uTimerID, unsigned int uMsg, unsigned long dwUser, unsigned long dw1, unsigned long dw2);

//======================================================================================
// �������ܣ��û���Ҫ���õĶ�ʱ������(��ʼ��)����
// ��ڲ�����uDelay����ʱ����ʱʱ������λΪms
//     void (*UserFun)(void)��ָ���û����� void fun (void) �ĺ���ָ��
// �� �� ֵ����
//======================================================================================
void UserTimerSet(unsigned int uDelay, void (*UserFun)(void))
{
    HANDLE cp, ct;

    TimerCallFun = UserFun;     //�õ��û�����ʱ���õĺ����ĺ���ָ��
    Context.ContextFlags = CONTEXT_CONTROL;
    cp = GetCurrentProcess(); //�õ���ǰ���̾��
    ct = GetCurrentThread(); //�õ���ǰ�߳�α���
    DuplicateHandle(cp, ct, cp, &mainhandle, 0, TRUE, 2); //α���ת��,�õ��߳�����

    /*ģ�����ö�ʱ���ж�,����һ����ʱ���߳�*/
    timeSetEvent(uDelay, 0, TimerISR, 0, TIME_PERIODIC);
    /*�����Ҫȡ����ʱ��,����Ե���timeKillEvent()����,���MSDN*/
}

//======================================================================================
// �������ܣ�timeSetEvent��Ҫ��ʱ���õĺ���
// ��ڲ�����unsigned int uTimerID, unsigned int uMsg, unsigned long dwUser, unsigned long dw1, unsigned long dw2�����MSDN
// �� �� ֵ����
//======================================================================================
static void __stdcall TimerISR(unsigned int uTimerID, unsigned int uMsg, unsigned long dwUser, unsigned long dw1, unsigned long dw2)
{
    SuspendThread(mainhandle); //��ֹ���̵߳�����,ģ���жϲ���.��û�б���Ĵ���
    GetThreadContext(mainhandle, &Context); //�õ����߳�������,Ϊ�л�������׼��
    //===========================================================================================
    TimerCallFun();    //����TimerCallFun(); �������� �û��Զ���ʵ�ֵ��жϵ���
    //===========================================================================================
    ResumeThread(mainhandle); //ģ���жϷ���,���̵߳��Լ���ִ��
}
