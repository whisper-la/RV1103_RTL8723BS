#ifndef ROCKIT_TESTAO_H
#define ROCKIT_TESTAO_H

#include <stdio.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include "rk_defines.h"
#include "rk_debug.h"
#include "rk_mpi_adec.h"
#include "rk_mpi_ao.h"
#include "rk_mpi_mb.h"
#include "rk_mpi_sys.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct _rkAOCtx {
    const char *srcFilePath;// 输入文件路径
    RK_S32      s32SampleRate;//打开声音采样率
    RK_S32      s32ReSmpSampleRate;//重采样率
    RK_S32      s32Channel;//输入通道
    RK_S32      s32DeviceChannel;//打开通道编号
    RK_S32      s32BitWidth;//
    RK_S32      s32DevId;//ID
    RK_S32      s32PeriodCount;//周期数，类似于帧结构数目
    RK_S32      s32PeriodSize;//周期长度，好像是每一帧的长度
    char       *chCardName;//声卡名
    RK_S32      s32ChnIndex;//通道编号，这个没有
    RK_S32      s32SetMute;
    RK_S32      s32SetFadeRate;//淡入淡出率
    RK_S32      s32GetVolume;//获取音量
    RK_S32      s32GetMute;//获取静音状态
    RK_S32      s32PauseResumeChn;//暂停或者重新启用通道
    RK_BOOL     playstatus;//播放状态
    RK_BOOL     new_play;//是否新要播放一个音频，是为1，否为0
    pthread_mutex_t AO_mutex;  //互斥锁--线程同步
    pthread_cond_t AO_cond;    //条件变量--阻塞线程，等待条件满足  互斥锁和条件变量均在反初始化销毁
    RK_BOOL AO_up_down;//AO设备开关，开是TRUE,关是FALSE
} AO_CTX_S;



void play_audio(const char *src_file, RK_S32 bitWidth, RK_S32 sound_mode, RK_S32 Volume);
void Set_Mute();
void Set_UnMute();
void Set_Volume(RK_S32 Volume);
RK_S32 Get_Volume();
RK_BOOL query_playstatus();
RK_BOOL query_Pause_status();
void pause_AO();
void resume_AO();
void startup();
void shutdown();
RK_S32 deinit_AO();


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif //ROCKIT_TESTAO_H


