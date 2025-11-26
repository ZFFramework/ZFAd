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
    /** @brief create native ad */
    virtual void *nativeAdCreate(ZF_IN ZFAdForBanner *ad) zfpurevirtual;
    /** @brief destroy native ad */
    virtual void nativeAdDestroy(ZF_IN ZFAdForBanner *ad) zfpurevirtual;

    /** @brief destroy native ad */
    virtual ZFUISize nativeAdMeasure(
            ZF_IN ZFAdForBanner *ad
            , ZF_IN const ZFUISize &sizeHint
            ) zfpurevirtual;

    /** @brief update app id */
    virtual void appIdUpdate(ZF_IN ZFAdForBanner *ad) zfpurevirtual;
    /** @brief update app key */
    virtual void adIdUpdate(ZF_IN ZFAdForBanner *ad) zfpurevirtual;

    // ============================================================
    // callbacks that implementations must notify
public:
    /** @brief see #ZFAdForBanner::E_AdOnError */
    zffinal void notifyAdOnError(
            ZF_IN ZFAdForBanner *ad
            , ZF_IN const zfstring &errorHint
            ) {
        ad->observerNotify(ZFAdForBanner::E_AdOnError(), zfobj<v_zfstring>(errorHint));
    }
    /** @brief see #ZFAdForBanner::E_AdOnDisplay */
    zffinal void notifyAdOnDisplay(ZF_IN ZFAdForBanner *ad) {
        ad->observerNotify(ZFAdForBanner::E_AdOnDisplay());
    }
    /** @brief see #ZFAdForBanner::E_AdOnClick */
    zffinal void notifyAdOnClick(ZF_IN ZFAdForBanner *ad) {
        ad->observerNotify(ZFAdForBanner::E_AdOnClick());
    }

    /** @brief see #ZFAdForBanner::E_AdOnClose */
    zffinal void notifyAdOnClose(ZF_IN ZFAdForBanner *ad) {
        ad->observerNotify(ZFAdForBanner::E_AdOnClose());
    }
ZFPROTOCOL_INTERFACE_END(ZFAdForBanner)

ZF_NAMESPACE_GLOBAL_END
#endif // #ifndef _ZFI_ZFProtocolZFAdForBanner_h_

