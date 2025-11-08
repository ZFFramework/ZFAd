/**
 * @file ZFProtocolZFAdForBanner.h
 * @brief protocol for ZFAdForBanner
 */

#ifndef _ZFI_ZFProtocolZFAdForBanner_h_
#define _ZFI_ZFProtocolZFAdForBanner_h_

#include "ZFCore/ZFProtocol.h"
#include "../ZFAdForBanner.h"
ZF_NAMESPACE_GLOBAL_BEGIN

/**
 * @brief protocol for ZFAdForBanner
 */
ZFPROTOCOL_INTERFACE_BEGIN(ZFLIB_ZFAd, ZFAdForBanner)
public:
    /** @brief create native ad view */
    virtual void *nativeAdViewCreate(ZF_IN ZFAdForBanner *ad) zfpurevirtual;
    /** @brief destroy native ad view */
    virtual void nativeAdViewDestroy(ZF_IN ZFAdForBanner *ad, ZF_IN void *nativeAd) zfpurevirtual;
ZFPROTOCOL_INTERFACE_END(ZFAdForBanner)

ZF_NAMESPACE_GLOBAL_END
#endif // #ifndef _ZFI_ZFProtocolZFAdForBanner_h_

