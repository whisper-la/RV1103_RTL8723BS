/*
 * 使用方法:将源文件和头文件一同置于工程中,可以调用相应的接口
 * 接口函数说明:
 * ①RK_S32 init_AO():初始化函数,初始化输出设备和输出通道.若初始化成功则返回RK_SUCCESS,否则返回RK_FAILED.
 * ②RK_S32 deinit_AO():反初始化函数,关闭输出设备和输出通道,若成功关闭则返回RK_SUCCESS,否则返回RK_FAILED.注意,调用此函数后
 * 则设备不可用,下次播放则需要重新初始化设备.
 * ③RK_S32 play_audio(const char *src_file, RK_S32 bitWidth, RK_S32 sound_mode, RK_S32 Volume):启动音频播放函数,
 * 若成功关闭则返回RK_SUCCESS,否则返回RK_FAILED.
 * 其参数含义如下:
 * src_file:音频文件路径,支持pcm格式,例如"/root/2.pcm"
 * bitWidth:pcm音频位宽度,可选参数8,16和24,输入其他参数无效.
 * sound_mode:音频播放模式,以单声道或者双声道播放,可选参数1,2.1为单声道,2是双声道,输入其他参数无效.
 * Volume:音量,范围0~100.
 * ④void Set_Volume(RK_S32 Volume):设置音量大小函数,参数范围0~100.
 * ⑤RK_S32 Get_Volume():查询音量大小函数,返回值为音量值.
 * ⑥void Set_Mute():设置静音函数,调用则输出静音.
 * ⑦void Set_UnMute():解除静音函数,调用则解除静音.
 * ⑧void pause_AO():暂停设备播放音频,注意一定要调用resume_AO()解除,否则将无法播放音频.
 * ⑨void resume_AO():恢复音频播放,音频会从上次中断的位置继续播放.
 */

#include "testao.h"
#include <sys/time.h>

static AUDIO_SOUND_MODE_E find_sound_mode(RK_S32 ch);
static AUDIO_BIT_WIDTH_E find_bit_width(RK_S32 bit);
RK_S32 test_open_device_ao(AO_CTX_S *ctx);
RK_S32 test_init_mpi_ao(AO_CTX_S *params);
RK_S32 Init_AO();
RK_S32 deinit_mpi_ao(AUDIO_DEV aoDevId, AO_CHN aoChn);



static AO_CTX_S  *ctx;
static RK_BOOL play_permission;
static pthread_mutex_t g_mutex;  //互斥锁--线程同步
static pthread_cond_t g_cond;    //条件变量--阻塞线程，等待条件满足

struct timeval Val;

static AUDIO_SOUND_MODE_E find_sound_mode(RK_S32 ch) {//查询或者设置声音模式（单声道、立体声）
    AUDIO_SOUND_MODE_E channel = AUDIO_SOUND_MODE_BUTT;
    switch (ch) {
        case 1:
            channel = AUDIO_SOUND_MODE_MONO;
            break;
        case 2:
            channel = AUDIO_SOUND_MODE_STEREO;
            break;
        default:
            RK_LOGE("channel = %d not support", ch);
            return AUDIO_SOUND_MODE_BUTT;
    }

    return channel;
}

static AUDIO_BIT_WIDTH_E find_bit_width(RK_S32 bit) {//bit宽度
    AUDIO_BIT_WIDTH_E bitWidth = AUDIO_BIT_WIDTH_BUTT;
    switch (bit) {
        case 8:
            bitWidth = AUDIO_BIT_WIDTH_8;
            break;
        case 16:
            bitWidth = AUDIO_BIT_WIDTH_16;
            break;
        case 24:
            bitWidth = AUDIO_BIT_WIDTH_24;
            break;
        default:
            RK_LOGE("bitwidth(%d) not support", bit);
            return AUDIO_BIT_WIDTH_BUTT;
    }

    return bitWidth;
}

RK_S32 test_open_device_ao(AO_CTX_S *ctx) {//设置参数并打开AO设备
    AUDIO_DEV aoDevId = ctx->s32DevId;
    AUDIO_SOUND_MODE_E soundMode;

    AIO_ATTR_S aoAttr;//AIO参数信息
    memset(&aoAttr, 0, sizeof(AIO_ATTR_S));

    if (ctx->chCardName) {
        snprintf(reinterpret_cast<char *>(aoAttr.u8CardName),
                 sizeof(aoAttr.u8CardName), "%s", ctx->chCardName);
    }

    aoAttr.soundCard.channels = ctx->s32DeviceChannel;
    aoAttr.soundCard.sampleRate = ctx->s32SampleRate;
    aoAttr.soundCard.bitWidth = AUDIO_BIT_WIDTH_16;//赋值声卡的参数 包括采样率等

    AUDIO_BIT_WIDTH_E bitWidth = find_bit_width(ctx->s32BitWidth);
    if (bitWidth == AUDIO_BIT_WIDTH_BUTT) {//如果为屁股，则失败
        goto __FAILED;
    }
    aoAttr.enBitwidth = bitWidth;
    aoAttr.enSamplerate = (AUDIO_SAMPLE_RATE_E)ctx->s32ReSmpSampleRate;
    soundMode = find_sound_mode(ctx->s32Channel);
    if (soundMode == AUDIO_SOUND_MODE_BUTT) {//声音模式为屁股，则失败
        goto __FAILED;
    }
    aoAttr.enSoundmode = soundMode;
    aoAttr.u32FrmNum = ctx->s32PeriodCount;
    aoAttr.u32PtNumPerFrm = ctx->s32PeriodSize;

    aoAttr.u32EXFlag = 0;
    aoAttr.u32ChnCnt = 2;

    RK_MPI_AO_SetPubAttr(aoDevId, &aoAttr);//设置AO设备（声卡）参数，pstAttr 音频输出设备属性。

    RK_MPI_AO_Enable(aoDevId);//启用AO设备

    return RK_SUCCESS;
    __FAILED:
    return RK_FAILURE;
}

RK_S32 test_init_mpi_ao(AO_CTX_S *params) {//初始化采样通道
    RK_S32 result;

    result =  RK_MPI_AO_EnableChn(params->s32DevId, params->s32ChnIndex);//启用AO通道
    if (result != 0) {
        RK_LOGE("ao enable channel fail, aoChn = %d, reason = %x", params->s32ChnIndex, result);//启用失败
        return RK_FAILURE;
    }

    // set sample rate of input data
    result = RK_MPI_AO_EnableReSmp(params->s32DevId, params->s32ChnIndex,
                                   (AUDIO_SAMPLE_RATE_E)params->s32ReSmpSampleRate);//启用AO重采样 设置ID 通道和重采样率
    if (result != 0) {
        RK_LOGE("ao enable channel fail, reason = %x, aoChn = %d", result, params->s32ChnIndex);
        return RK_FAILURE;
    }

    return RK_SUCCESS;
}

RK_S32 deinit_mpi_ao(AUDIO_DEV aoDevId, AO_CHN aoChn) {//反初始化 关闭通道关闭采样 关闭AO
    RK_S32 result = RK_MPI_AO_DisableReSmp(aoDevId, aoChn);
    if (result != 0) {
        RK_LOGE("ao disable resample fail, reason = %d", result);
        return RK_FAILURE;
    }

    result = RK_MPI_AO_DisableChn(aoDevId, aoChn);
    if (result != 0) {
        RK_LOGE("ao disable channel fail, reason = %d", result);
        return RK_FAILURE;
    }

    result =  RK_MPI_AO_Disable(aoDevId);
    if (result != 0) {
        RK_LOGE("ao disable  fail, reason = %d", result);
        return RK_FAILURE;
    }

    return RK_SUCCESS;
}

RK_S32 Init_AO() {




    RK_MPI_SYS_Init();
    RK_PRINT("初始化\n");
    if(test_open_device_ao(ctx)!=RK_SUCCESS)
    {
        goto __FAILED;
    }

    if(test_init_mpi_ao(ctx)!=RK_SUCCESS)
    {
        goto __FAILED;
    }


    return RK_SUCCESS;
    __FAILED:
    return RK_FAILURE;
}

RK_S32 deinit_AO(){
    RK_PRINT("反初始化\n");
//    pthread_mutex_destroy(&g_mutex);  //配套销毁互斥锁
//    pthread_cond_destroy(&g_cond);    //配套销毁条件变量
    if(deinit_mpi_ao(ctx->s32DevId, ctx->s32ChnIndex)!=RK_SUCCESS)
    {
        goto __FAILED;
    }
    RK_MPI_SYS_Exit();//反初始化RK MPI系统。退出前调用
    return RK_SUCCESS;
    __FAILED:
    return RK_FAILURE;
}

void* sendDataThread(void * ptr) {//发送音频数据
    RK_PRINT("线程开启");
    pthread_detach(pthread_self());


    RK_PRINT("init ......");
    ctx = reinterpret_cast<AO_CTX_S *>(malloc(sizeof(AO_CTX_S)));
    RK_PRINT("init var");
    memset(ctx, 0, sizeof(AO_CTX_S));
    ctx->srcFilePath        = RK_NULL;
    ctx->s32SampleRate      = 16000;
    ctx->s32ReSmpSampleRate = 16000;
    ctx->s32DeviceChannel   = 2;
    ctx->s32Channel         = 2;
    ctx->s32BitWidth        = 16;
    ctx->s32PeriodCount     = 4;
    ctx->s32PeriodSize      = 1024;
    ctx->chCardName         = "hw:0,0";
    ctx->s32DevId           = 0;
    ctx->s32SetMute         = 0;
    ctx->s32SetFadeRate     = 0;
    ctx->s32GetVolume       = 0;
    ctx->s32GetMute         = 0;
    ctx->s32PauseResumeChn  = 0;
    ctx->s32ChnIndex        = 0;
    ctx->playstatus         = RK_FALSE;//播放标志初始化
    ctx->new_play           = RK_FALSE;//初始化新播放为0
    ctx->AO_mutex           = PTHREAD_MUTEX_INITIALIZER;//线程锁
    ctx->AO_cond            = PTHREAD_COND_INITIALIZER;//线程信号量
    ctx->AO_up_down         = RK_TRUE;//音频设备开

    MB_POOL_CONFIG_S pool_config;
    // set default value for struct
    RK_U8 *srcData = RK_NULL;
    AUDIO_FRAME_S frame;
    RK_U64 timeStamp = 0;
    RK_S32 s32MilliSec = -1;//等待播放结束，为5最多等待5ms,如果为-1则阻塞至播放结束
    RK_S32 size = 0;
    RK_S32 result = 0;
    FILE *file=RK_NULL;
    pthread_mutex_init(&g_mutex, nullptr);  //初始化互斥锁
    pthread_cond_init(&g_cond, nullptr);    //初始化条件变量

    while(1){
        Init_AO();
        RK_PRINT("锁住之前\n");
        pthread_mutex_lock(&g_mutex);  //加锁
        pthread_cond_wait(&g_cond, &g_mutex); //阻塞线程，且该函数会对互斥锁解锁；且接解除阻塞之后，对互斥锁进行加锁操作
        gettimeofday(&Val, NULL);
        RK_PRINT("阻塞时间sec --- %ld, usec --- %ld\n", Val.tv_sec, Val.tv_usec);
        pthread_mutex_unlock(&g_mutex);  //解锁,开始播放音频

        __NEWPLAY:
        ctx->playstatus=RK_TRUE;//开始播放
        file = fopen(ctx->srcFilePath, "rb");

        if (file == RK_NULL) {
            RK_LOGE("open save file %s failed because %s.", ctx->srcFilePath, strerror(errno));
            goto __EXIT;
        }
        else
        {
            RK_PRINT("正在播放%s\n",ctx->srcFilePath);
        }
        if(ctx->s32PauseResumeChn)
        {
            RK_PRINT("start AO failed, because AO is pausing\n");
            goto __EXIT;
        }
        srcData = reinterpret_cast<RK_U8 *>(calloc(256, sizeof(RK_U8)));
        memset(srcData, 0, 256);
        RK_PRINT("读取文件结束\n");
        ctx->new_play=RK_FALSE;//new_play置位

        while (!ctx->new_play) {//当不需要播放新音频的时候，则继续循环;否则结束音频播放，执行退出的程序，并且给new_play置位
            size = fread(srcData, 1, 256, file);
            frame.u32Len = size;
            frame.u64TimeStamp = timeStamp++;
            frame.enBitWidth = find_bit_width(ctx->s32BitWidth);
            frame.enSoundMode = find_sound_mode(ctx->s32Channel);
            frame.bBypassMbBlk = RK_FALSE;

            MB_EXT_CONFIG_S extConfig;
            memset(&extConfig, 0, sizeof(extConfig));
            extConfig.pOpaque = srcData;
            extConfig.pu8VirAddr = srcData;
            extConfig.u64Size = size;
            RK_MPI_SYS_CreateMB(&(frame.pMbBlk), &extConfig);//创建一个内存缓存块

            __RETRY:
            result = RK_MPI_AO_SendFrame(ctx->s32DevId, ctx->s32ChnIndex, &frame, -1);//发送AO音频帧。pstData 音频帧结构体指针。
            if (result < 0) {
                RK_LOGE("send frame fail, result = %d, TimeStamp = %lld, s32MilliSec = %d",
                        result, frame.u64TimeStamp, s32MilliSec);
                goto __RETRY;
            }
            RK_MPI_MB_ReleaseMB(frame.pMbBlk);//释放缓存块

            if (size <= 0) {
                RK_LOGI("eof");
                break;
            }
        }
        if(ctx->new_play)//如果立刻打断并且新播放的，直接更换源头,不必退出
        {
            if (file)
            {
                fclose(file);
                file = RK_NULL;
            }
            goto __NEWPLAY;
        }

        __EXIT:
        gettimeofday(&Val, NULL);
        RK_PRINT("退出播放时间sec --- %ld, usec --- %ld\n", Val.tv_sec, Val.tv_usec);
//        RK_MPI_ADEC_ClearChnBuf(ctx->s32ChnIndex);
//        RK_MPI_AO_ClearChnBuf(ctx->s32DevId,ctx->s32ChnIndex);
        RK_MPI_AO_WaitEos(ctx->s32DevId, ctx->s32ChnIndex, 0);//等待指定设备和通道播放完成。
        ctx->playstatus=RK_FALSE;//播放完毕
        if (file) {
            fclose(file);
            file = RK_NULL;
        }
        RK_PRINT("exit audio play\n");

        if(RK_FAILURE== deinit_AO() ){
            RK_PRINT("AO device deinit failed\n");
        }
    }
    return RK_NULL;
}
/**
  * @brief  播放音频
  * @param  src_file：音频文件路径 bitWidth：音频位宽度，一般为16bit sound_mode：音频声道数目，1或2   Volume:音量
  * @retval None
  */
void play_audio(const char *src_file, RK_S32 bitWidth, RK_S32 sound_mode, RK_S32 Volume)
{
    if(ctx->playstatus)
    {
        RK_PRINT("AO device is playing\n");
        ctx->new_play=RK_TRUE;
    }
    else {
        ctx->new_play=RK_FALSE;
    }
    ctx->playstatus=RK_TRUE;//播放标志置位
    ctx->srcFilePath=src_file;
    ctx->s32BitWidth=bitWidth;
    ctx->s32Channel=sound_mode;
    RK_MPI_AO_SetVolume(ctx->s32DevId, Volume);//设置音量
    RK_PRINT("播放\n");
    pthread_cond_signal(&g_cond);  //通知音频播放的线程，解除阻塞
    RK_PRINT("主线程播放\n");
}



void Set_Mute()
{
    if(ctx->s32GetMute==0)//未静音状态才能静音
    {
        AUDIO_FADE_S aFade;
        aFade.bFade = RK_FALSE;
        aFade.enFadeOutRate = (AUDIO_FADE_RATE_E)ctx->s32SetFadeRate;
        aFade.enFadeInRate = (AUDIO_FADE_RATE_E)ctx->s32SetFadeRate;//淡入淡出率
        RK_MPI_AO_SetMute(ctx->s32DevId, RK_TRUE, &aFade);//设置静音状态，其与params的有关，即上面的
        ctx->s32SetMute=1;
        ctx->s32GetMute=1;
    }
}
void Set_UnMute()
{
    if(ctx->s32GetMute==1)//
    {
        AUDIO_FADE_S aFade;
        aFade.bFade = RK_FALSE;
        aFade.enFadeOutRate = (AUDIO_FADE_RATE_E)ctx->s32SetFadeRate;
        aFade.enFadeInRate = (AUDIO_FADE_RATE_E)ctx->s32SetFadeRate;//淡入淡出率
        RK_MPI_AO_SetMute(ctx->s32DevId, RK_FALSE, &aFade);//设置静音状态，其与params的有关，即上面的
        ctx->s32SetMute=0;
        ctx->s32GetMute=0;

    }
}
void Set_Volume(RK_S32 Volume)
{
    RK_MPI_AO_SetVolume(ctx->s32DevId, Volume);//设置音量
}
RK_S32 Get_Volume(){
    RK_S32 volume = 0;
    RK_MPI_AO_GetVolume(ctx->s32DevId, &volume);
    return volume;
}
/**
  * @name query_playstatus()
  * @brief 返回播放状态和准备播放状态
  */
RK_BOOL query_playstatus()
{
    return ctx->playstatus;
}
/**
  * @name query_Pause_status()
  * @brief 查询暂停状态
  */
RK_BOOL query_Pause_status(){
    if(ctx->s32PauseResumeChn)//其不为0即暂停
        return RK_TRUE;
    else
        return RK_FALSE;
}
/**
  * @name  pause_AO()
  * @details 调用暂停后请务必调用恢复，否则可能会导致崩溃
  */
void pause_AO(){
    if(ctx->s32PauseResumeChn==0)//只有为0的时候才能暂停，0代表没有暂停
    {
        RK_MPI_AO_PauseChn(ctx->s32DevId, ctx->s32ChnIndex);//暂停AO通道
        ctx->s32PauseResumeChn=1;//暂停状态
    }
}
void resume_AO(){
    if(ctx->s32PauseResumeChn==1)
    {
        RK_MPI_AO_ResumeChn(ctx->s32DevId, ctx->s32ChnIndex);//恢复AO通道
        ctx->s32PauseResumeChn=0;
    }
}
void startup(){
    ctx->AO_up_down=RK_TRUE;
}
void shutdown(){
    ctx->AO_up_down=RK_FALSE;
}

int main()
{
    pthread_t thread_tid_AO;//线程id
    pthread_create(&thread_tid_AO, NULL, sendDataThread, NULL);//创建音频播放线程
    sleep(1);
    play_audio("/root/imageRFID/voice/bad.pcm", 16, 2, 100);
    sleep(1);
    play_audio("/root/imageRFID/voice/exist.pcm", 16, 2, 100);
    sleep(5);
    RK_PRINT("播放了吗\n");
    play_audio("/root/imageRFID/voice/bad.pcm", 16, 2, 100);
    sleep(1);
    play_audio("/root/imageRFID/voice/empty.pcm", 16, 2, 100);
    sleep(1);
    play_audio("/root/imageRFID/voice/bad.pcm", 16, 2, 100);
    sleep(1);
    play_audio("/root/imageRFID/voice//bad.pcm", 16, 2, 100);
    sleep(5);
}
