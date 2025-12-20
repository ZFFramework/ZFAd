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
 *
 * impl must register by declaring a class named `ZFAdForBannerImpl_xxx`,
 * while `xxx` is the name passed from #ZFAdForBanner::setup
 */
zfinterface ZFLIB_ZFAd ZFAdForBannerImpl : zfextend ZFInterface {
    ZFINTERFACE_DECLARE(ZFAdForBannerImpl, ZFInterface)

public:
    /** @brief create native ad */
    virtual void *nativeAdCreate(
            ZF_IN ZFAdForBanner *ad
            , ZF_IN const zfstring &appId
            , ZF_IN const zfstring &adId
            ) zfpurevirtual;
    /** @brief destroy native ad */
    virtual void nativeAdDestroy(ZF_IN ZFAdForBanner *ad) zfpurevirtual;

    /** @brief destroy native ad */
    virtual ZFUISize nativeAdMeasure(
            ZF_IN ZFAdForBanner *ad
            , ZF_IN const ZFUISize &sizeHint
            ) zfpurevirtual;

public:
    /** @brief util for impl to access impl from owner ad */
    static ZFAdForBannerImpl *implForAd(ZF_IN ZFAdForBanner *ad) {
        return (ZFAdForBannerImpl *)ad->_ZFP_ZFAdForBanner_impl();
    }

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
};

ZF_NAMESPACE_GLOBAL_END
#endif // #ifndef _ZFI_ZFProtocolZFAdForBanner_h_

