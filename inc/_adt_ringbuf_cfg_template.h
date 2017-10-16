/*****************************************************************************
* \file      adt_ringbuf_cfg.h
* \author    Conny Gustafsson
* \date      2017-10-16
* \brief     Ringbuffer config file
*
* Copyright (c) 2013-2017 Conny Gustafsson
* Permission is hereby granted, free of charge, to any person obtaining a copy of
* this software and associated documentation files (the "Software"), to deal in
* the Software without restriction, including without limitation the rights to
* use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
* the Software, and to permit persons to whom the Software is furnished to do so,
* subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
******************************************************************************/

/***
* This is a template file.
* Steps to use:
* 1. Copy this file as adt_ringbuf_cfg.h into your local C project
* 2. Define ADT_RBF_CFG amongst your compiler defines (e.g. using cflags).
*
* Alternatives:
* You can instead add the defines below amongst your global compiler defines (e.g. using cflags).
*/

#ifndef ADT_RINGBUF_CFG_H__
#define ADT_RINGBUF_CFG_H__

#define ADT_RBFS_ENABLE 1    //ringbuffer static (fixed sized blocks)
#define ADT_RBFD_ENABLE 0    //ringbuffer dynamic (dynamically sized blocks)
#define ADT_RBFU16_ENABLE 1  //special ringbuffer for uint16 values

#endif //RINGBUF_CFG_H__
