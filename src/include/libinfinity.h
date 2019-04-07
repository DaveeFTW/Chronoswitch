/*

Copyright (C) 2015, David "Davee" Morgan

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#ifndef LIBINFINITY_H
#define LIBINFINITY_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
    @return The version of the persistent aspect of Infinity.
*/
unsigned int infGetVersion(void);

/**
    @return The version of the bootloader.
*/
unsigned int infGetCoreVersion(void);

/**
    @return non-zero if redirection is enabled else redirection is disabled and the logical IO operations can be performed.
*/
int infGetRedirectionStatus(void);

/**
    Set whether to enable or disable redirection.
    By default redirection is enabled and is required for infinity to operate. By disabling you enable the ability to format flash0 and write to the bootloader of infinity. Ensure to re-enable redirection if you intend to warm-reboot.
*/
void infSetRedirectionStatus(int enabled);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // LIBINFINITY_H
