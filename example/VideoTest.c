#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vencoder.h"
#include <time.h>

int yu12_nv12(unsigned int width, unsigned int height, unsigned char *addr_uv, unsigned char *addr_tmp_uv)
{
	unsigned int i, chroma_bytes;
	unsigned char *u_addr = NULL;
	unsigned char *v_addr = NULL;
	unsigned char *tmp_addr = NULL;

	chroma_bytes = width*height/4;

	u_addr = addr_uv;
	v_addr = addr_uv + chroma_bytes;
	tmp_addr = addr_tmp_uv;

	for(i=0; i<chroma_bytes; i++)
	{
		*(tmp_addr++) = *(u_addr++);
		*(tmp_addr++) = *(v_addr++);
	}

	memcpy(addr_uv, addr_tmp_uv, chroma_bytes*2);	

	return 0;
}

int main(int argc , char ** argv)
{
	if (argc!=5) {
		fprintf(stderr,"%s W H profile CABAC\n");
		return -1;
	}
	unsigned int src_width,src_height,dst_width,dst_height;
	VencH264Param h264Param;
	
	src_width=dst_width=atoi(argv[1]);
	src_height=dst_height=atoi(argv[2]);
	char * profile = argv[3];	
	int cabac = atoi(argv[4]);

	if (strcmp(profile,"baseline")) {
		h264Param.sProfileLevel.nProfile = VENC_H264ProfileBaseline;
	} else if ( strcmp(profile, "main")) {
		h264Param.sProfileLevel.nProfile = VENC_H264ProfileMain;
	} else if ( strcmp(profile, "high")) {
		h264Param.sProfileLevel.nProfile = VENC_H264ProfileHigh;
	} else {
		fprintf(stderr,"WTFFFF\n");
		return -1;
	}

	VencBaseConfig baseConfig;
	VencAllocateBufferParam bufferParam;
	VideoEncoder* pVideoEnc = NULL;
	VencInputBuffer inputBuffer;
	VencOutputBuffer outputBuffer;
	VencHeaderData sps_pps_data;
	VencCyclicIntraRefresh sIntraRefresh;
	unsigned char *uv_tmp_buffer = NULL;
	


	/*src_width = 1920;
	src_height = 1080;
	dst_width = 1920;
	dst_height = 1080;*/
	//src_width = 640;
	//src_height = 480;
	//dst_width = 640;
	//dst_height = 480;


	//intraRefresh
	
	sIntraRefresh.bEnable = 1;
	sIntraRefresh.nBlockNumber = 10;


	//* h264 param
	h264Param.bEntropyCodingCABAC = cabac;
	h264Param.nBitrate = 4*1024*1024; /* bps */
	h264Param.nFramerate = 30; /* fps */
	h264Param.nCodingMode = VENC_FRAME_CODING;
//	h264Param.nCodingMode = VENC_FIELD_CODING;
	
	h264Param.nMaxKeyInterval = 200;
	//h264Param.sProfileLevel.nProfile = VENC_H264ProfileBaseline;
	//h264Param.sProfileLevel.nProfile = VENC_H264ProfileMain;
	//h264Param.sProfileLevel.nProfile = VENC_H264ProfileHigh;
	h264Param.sProfileLevel.nLevel = VENC_H264Level41;
	//h264Param.sProfileLevel.nLevel = VENC_H264Level2;
	h264Param.sQPRange.nMinqp = 10;
	h264Param.sQPRange.nMaxqp = 40;


	int codecType = VENC_CODEC_H264;
	int testNumber = 300;


	FILE *in_file = NULL;
	FILE *out_file = NULL;
		in_file = fopen("/mnt/test.yuv", "r");
		if(in_file == NULL)
		{
			fprintf(stderr,"open in_file fail\n");
			return -1;
		}
		

		char fn_buffer[512];
		sprintf(fn_buffer,"/mnt/out_%dx%d_%s_%s.h264", src_width,src_height, cabac == 1 ? "CABAC" : "NOCABAC" , profile);
		out_file = fopen(fn_buffer, "wb");
		if(out_file == NULL)
		{
			fprintf(stderr,"open out_file fail\n");
			return -1;
		}

	memset(&baseConfig, 0 ,sizeof(VencBaseConfig));
	memset(&bufferParam, 0 ,sizeof(VencAllocateBufferParam));

	baseConfig.nInputWidth= src_width;
	baseConfig.nInputHeight = src_height;
	baseConfig.nStride = src_width;
	
	baseConfig.nDstWidth = dst_width;
	baseConfig.nDstHeight = dst_height;
	baseConfig.eInputFormat = VENC_PIXEL_YUV420SP;

	bufferParam.nSizeY = baseConfig.nInputWidth*baseConfig.nInputHeight;
	bufferParam.nSizeC = baseConfig.nInputWidth*baseConfig.nInputHeight/2;
	bufferParam.nBufferNum = 4;
	
	pVideoEnc = VideoEncCreate(codecType);


		int value;
		
		VideoEncSetParameter(pVideoEnc, VENC_IndexParamH264Param, &h264Param);

		value = 0;
		VideoEncSetParameter(pVideoEnc, VENC_IndexParamIfilter, &value);

		value = 0; //degree
		VideoEncSetParameter(pVideoEnc, VENC_IndexParamRotation, &value);


		VideoEncSetParameter(pVideoEnc, VENC_IndexParamH264CyclicIntraRefresh, &sIntraRefresh);


	VideoEncInit(pVideoEnc, &baseConfig);

		unsigned int head_num = 0;
		VideoEncGetParameter(pVideoEnc, VENC_IndexParamH264SPSPPS, &sps_pps_data);
		fwrite(sps_pps_data.pBuffer, 1, sps_pps_data.nLength, out_file);
		fprintf(stderr,"sps_pps_data.nLength: %d", sps_pps_data.nLength);
		for(head_num=0; head_num<sps_pps_data.nLength; head_num++)
			fprintf(stderr,"the sps_pps :%02x\n", *(sps_pps_data.pBuffer+head_num));

	AllocInputBuffer(pVideoEnc, &bufferParam);

		uv_tmp_buffer = (unsigned char*)malloc(baseConfig.nInputWidth*baseConfig.nInputHeight/2);
		if(uv_tmp_buffer == NULL)
		{
			fprintf(stderr,"malloc uv_tmp_buffer fail\n");
			return -1;
		}
	while(testNumber > 0)
	{
		GetOneAllocInputBuffer(pVideoEnc, &inputBuffer);
		{
			unsigned int size1, size2;
				size1 = fread(inputBuffer.pAddrVirY, 1, baseConfig.nInputWidth*baseConfig.nInputHeight, in_file);
				size2 = fread(inputBuffer.pAddrVirC, 1, baseConfig.nInputWidth*baseConfig.nInputHeight/2, in_file);

			if((size1!= baseConfig.nInputWidth*baseConfig.nInputHeight) || (size2!= baseConfig.nInputWidth*baseConfig.nInputHeight/2))
			{
				fseek(in_file, 0L, SEEK_SET);
				size1 = fread(inputBuffer.pAddrVirY, 1, baseConfig.nInputWidth*baseConfig.nInputHeight, in_file);
				size2 = fread(inputBuffer.pAddrVirC, 1, baseConfig.nInputWidth*baseConfig.nInputHeight/2, in_file);
			}
			yu12_nv12(baseConfig.nInputWidth, baseConfig.nInputHeight, inputBuffer.pAddrVirC, uv_tmp_buffer);

		}
		
		inputBuffer.bEnableCorp = 0;
		inputBuffer.sCropInfo.nLeft =  240;
		inputBuffer.sCropInfo.nTop  =  240;
		inputBuffer.sCropInfo.nWidth  =  240;
		inputBuffer.sCropInfo.nHeight =  240;

		FlushCacheAllocInputBuffer(pVideoEnc, &inputBuffer);

		AddOneInputBuffer(pVideoEnc, &inputBuffer);
		VideoEncodeOneFrame(pVideoEnc);

		AlreadyUsedInputBuffer(pVideoEnc,&inputBuffer);
		ReturnOneAllocInputBuffer(pVideoEnc, &inputBuffer);

		GetOneBitstreamFrame(pVideoEnc, &outputBuffer);
		//logi("size: %d,%d", outputBuffer.nSize0,outputBuffer.nSize1);

		fwrite(outputBuffer.pData0, 1, outputBuffer.nSize0, out_file);

		if(outputBuffer.nSize1)
		{
			fwrite(outputBuffer.pData1, 1, outputBuffer.nSize1, out_file);
		}
			
		FreeOneBitStreamFrame(pVideoEnc, &outputBuffer);

		testNumber--;
	}

out:	
	fclose(out_file);
	fclose(in_file);
	if(uv_tmp_buffer)
		free(uv_tmp_buffer);

	return 0;
}
