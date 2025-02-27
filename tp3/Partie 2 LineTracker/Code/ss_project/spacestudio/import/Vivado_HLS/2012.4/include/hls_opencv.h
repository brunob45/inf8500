/*****************************************************************************
 *
 *     Author: Xilinx, Inc.
 *
 *     This text contains proprietary, confidential information of
 *     Xilinx, Inc. , is distributed by under license from Xilinx,
 *     Inc., and may be used, copied and/or disclosed only pursuant to
 *     the terms of a valid license agreement with Xilinx, Inc.
 *
 *     XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"
 *     AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND
 *     SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,
 *     OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,
 *     APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION
 *     THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,
 *     AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE
 *     FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY
 *     WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE
 *     IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR
 *     REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF
 *     INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *     FOR A PARTICULAR PURPOSE.
 *
 *     Xilinx products are not intended for use in life support appliances,
 *     devices, or systems. Use in such applications is expressly prohibited.
 *
 *     (c) Copyright 2011 Xilinx Inc.
 *     All rights reserved.
 *
 *****************************************************************************/

/*
 * HLS OpenCV Image Translation File, Not Synthesizable.
 */

#ifndef ___HLS__VIDEO_OPENCV__
#define ___HLS__VIDEO_OPENCV__

//OpenCV Header Files
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

//HLS Video Header File
#include "hls_video.h"

/* From/To hls::Mat */

template<int ROWS, int COLS, int T>
void IplImage2hlsMat(IplImage *img, hls::Mat<ROWS, COLS, T>& hls_mat) {
    int i, j, k;
    CvScalar cv_pix;
    hls::Scalar<HLS_MAT_CN(T), HLS_TNAME(T)> hls_pix;
    assert(img && img->imageData && (img->nChannels == HLS_MAT_CN(T)));
    for (i = 0; i < hls_mat.rows; i++) {
        for (j = 0; j < hls_mat.cols; j++) {
            cv_pix = cvGet2D(img, i, j);
            for (k = 0; k < HLS_MAT_CN(T); k++) {
                hls_pix.val[k] = (HLS_TNAME(T))cv_pix.val[k];
            }
            hls_mat << hls_pix;
        }
    }
}

template<int ROWS, int COLS, int T>
void hlsMat2IplImage(hls::Mat<ROWS, COLS, T>& hls_mat, IplImage *img) {
    int i, j, k;
    CvScalar cv_pix;
    hls::Scalar<HLS_MAT_CN(T), HLS_TNAME(T)> hls_pix;
    assert(img && img->imageData && (img->nChannels == HLS_MAT_CN(T)));
    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            hls_mat >> hls_pix;
            for (k = 0; k < img->nChannels; k++) {
                cv_pix.val[k] = (double)hls_pix.val[k];
            }
            cvSet2D(img, i, j, cv_pix);
        }
    }
}

template<int ROWS, int COLS, int T>
void cvMat2hlsMat(cv::Mat& cv_mat, hls::Mat<ROWS, COLS, T>& hls_mat) {
    IplImage img = cv_mat;
    IplImage2hlsMat(&img, hls_mat);
}

template<int ROWS, int COLS, int T>
void hlsMat2cvMat(hls::Mat<ROWS, COLS, T>& hls_mat, cv::Mat& cv_mat) {
    IplImage img = cv_mat;
    hlsMat2IplImage(hls_mat, &img);
}

template<int ROWS, int COLS, int T>
void CvMat2hlsMat(CvMat* cvmat, hls::Mat<ROWS, COLS, T>& hls_mat) {
    cv::Mat cv_mat(cvmat);
    cvMat2hlsMat(cv_mat, hls_mat);
}

template<int ROWS, int COLS, int T>
void hlsMat2CvMat(hls::Mat<ROWS, COLS, T>& hls_mat, CvMat* cvmat) {
    cv::Mat cv_mat(cvmat);
    hlsMat2cvMat(hls_mat, cv_mat);
}

/* From/To AXI video stream */

template<int W>
void IplImage2AXIvideo(IplImage* img, hls::stream<hls::AXI_Base<W> >& AXI_video_strm) {
    int i, j, k;
    CvScalar cv_pix;
    hls::AXI_Base<W> axi;
    int depth = (img->depth & 0xff);
    assert(img && img->imageData && (W >= depth*img->nChannels));
    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            if ((i ==0) && (j == 0)) {
                axi.user = 1;
            } else {
                axi.user = 0;
            }
            if (j == (img->width -1)) {
                axi.last = 1;
            } else {
                axi.last = 0;
            }
            axi.data = -1;
            cv_pix = cvGet2D(img, i, j);
            for (k = 0; k < img->nChannels; k++) {
                switch(img->depth) {
                case IPL_DEPTH_8U:
                    hls::AXISetBitFields(axi, k*depth, depth, (unsigned char)cv_pix.val[k]);
                    break;
                case IPL_DEPTH_8S:
                    hls::AXISetBitFields(axi, k*depth, depth, (char)cv_pix.val[k]);
                    break;
                case IPL_DEPTH_16U:
                    hls::AXISetBitFields(axi, k*depth, depth, (unsigned short)cv_pix.val[k]);
                    break;
                case IPL_DEPTH_16S:
                    hls::AXISetBitFields(axi, k*depth, depth, (short)cv_pix.val[k]);
                    break;
                case IPL_DEPTH_32S:
                    hls::AXISetBitFields(axi, k*depth, depth, (int)cv_pix.val[k]);
                    break;
                case IPL_DEPTH_32F:
                    hls::AXISetBitFields(axi, k*depth, depth, (float)cv_pix.val[k]);
                    break;
                case IPL_DEPTH_64F:
                    hls::AXISetBitFields(axi, k*depth, depth, (double)cv_pix.val[k]);
                    break;
                default:
                    hls::AXISetBitFields(axi, k*depth, depth, (unsigned char)cv_pix.val[k]);
                    break;
                }
            }
            AXI_video_strm << axi;
        }
    }
}

template<int W>
void AXIvideo2IplImage(hls::stream<hls::AXI_Base<W> >& AXI_video_strm, IplImage* img) {
    int i, j, k;
    hls::AXI_Base<W> axi;
    CvScalar cv_pix;
    int depth = (img->depth & 0xff);
    bool sof = 0;
    assert(img && img->imageData && (W >= depth*img->nChannels));
    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            AXI_video_strm >> axi;
            if ((i == 0) && (j == 0)) {
                if (axi.user.to_int() == 1) {
                    sof = 1;
                } else {
                    j--;
                }
            }
            if (sof) {
                for (k = 0; k < img->nChannels; k++) {
                    switch(img->depth) {
                        case IPL_DEPTH_8U:
                        {   unsigned char temp;
                            hls::AXIGetBitFields(axi, k*depth, depth, temp);
                            cv_pix.val[k] = temp;
                        }
                        break;
                        case IPL_DEPTH_8S:
                        {   char temp;
                            hls::AXIGetBitFields(axi, k*depth, depth, temp);
                            cv_pix.val[k] = temp;
                        }
                        break;
                        case IPL_DEPTH_16U:
                        {   unsigned short temp;
                            hls::AXIGetBitFields(axi, k*depth, depth, temp);
                            cv_pix.val[k] = temp;
                        }
                        break;
                        case IPL_DEPTH_16S:
                        {   short temp;
                            hls::AXIGetBitFields(axi, k*depth, depth, temp);
                            cv_pix.val[k] = temp;
                        }
                        break;
                        case IPL_DEPTH_32S:
                        {   int temp;
                            hls::AXIGetBitFields(axi, k*depth, depth, temp);
                            cv_pix.val[k] = temp;
                        }
                        break;
                        case IPL_DEPTH_32F:
                        {   float temp;
                            hls::AXIGetBitFields(axi, k*depth, depth, temp);
                            cv_pix.val[k] = temp;
                        }
                        break;
                        case IPL_DEPTH_64F:
                        {   double temp;
                            hls::AXIGetBitFields(axi, k*depth, depth, temp);
                            cv_pix.val[k] = temp;
                        }
                        break;
                        default:
                        {   unsigned char temp;
                            hls::AXIGetBitFields(axi, k*depth, depth, temp);
                            cv_pix.val[k] = temp;
                        }
                        break;
                    }
                }
                cvSet2D(img, i, j, cv_pix);
            }
        }
    }
}

template<int W>
void cvMat2AXIvideo(cv::Mat& cv_mat, hls::stream<hls::AXI_Base<W> >& AXI_video_strm) {
    IplImage img = cv_mat;
    IplImage2AXIvideo<W>(&img, AXI_video_strm);
}

template<int W>
void AXIvideo2cvMat(hls::stream<hls::AXI_Base<W> >& AXI_video_strm, cv::Mat& cv_mat) {
    IplImage img = cv_mat;
    AXIvideo2IplImage<W>(AXI_video_strm, &img);
}

template<int W>
void CvMat2AXIvideo(CvMat* cvmat, hls::stream<hls::AXI_Base<W> >& AXI_video_strm) {
    cv::Mat cv_mat(cvmat);
    cvMat2AXIvideo<W>(cv_mat, AXI_video_strm);
}

template<int W>
void AXIvideo2CvMat(hls::stream<hls::AXI_Base<W> >& AXI_video_strm, CvMat* cvmat) {
    cv::Mat cv_mat(cvmat);
    AXIvideo2cvMat<W>(AXI_video_strm, cv_mat);
}

/* From/To hls::Window (for image processing kernels) */
/* must be single-channeled. */

template<int ROWS, int COLS, typename T>
void CvMat2hlsWindow(CvMat* cvmat, hls::Window<ROWS, COLS, T>& hls_window) {
    int i, j;
    assert(cvmat && cvmat->data.ptr);
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            hls_window.val[i][j] = (T)cvmGet(cvmat, i, j);
        }
    }
}

template<int ROWS, int COLS, typename T>
void hlsWindow2CvMat(hls::Window<ROWS, COLS, T>& hls_window, CvMat* cvmat) {
    int i, j;
    assert(cvmat && cvmat->data.ptr);
    for (i = 0; i < ROWS; i++) {
        for (j = 0; j < COLS; j++) {
            cvmSet(cvmat, i, j, (double)hls_window.val[i][j]);
        }
    }
}


#endif
