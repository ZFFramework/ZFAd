/**
 * @file ZFProtocolZFAdForSplash.h
 * @brief protocol for ZFAdForSplash
 */

#ifndef _ZFI_ZFProtocolZFAdForSplash_h_
#define _ZFI_ZFProtocolZFAdForSplash_h_

#include "ZFCore/ZFProtocol.h"
#include "../ZFAdForSplash.h"
ZF_NAMESPACE_GLOBAL_BEGIN

/**
 * @brief protocol for ZFAdForSplash
 *
 * impl must register by declaring a class named `ZFAdForSplashImpl_xxx`,
 * while `xxx` is the name passed from #ZFAdForSplash::setup
 */
zfinterface ZFLIB_ZFAd ZFAdForSplashImpl : zfextend ZFInterface {
    ZFINTERFACE_DECLARE(ZFAdForSplashImpl, ZFInterface)

public:
    /** @brief create native ad */
    virtual void *nativeAdCreate(
            ZF_IN ZFAdForSplash *ad
            , ZF_IN const zfstring &appId
            , ZF_IN const zfstring &adId
            ) zfpurevirtual;
    /** @brief destroy native ad */
    virtual void nativeAdDestroy(ZF_IN ZFAdForSplash *ad) zfpurevirtual;

    /** @brief start native ad */
    virtual void nativeAdStart(
            ZF_IN ZFAdForSplash *ad
            , ZF_IN ZFUIRootWindow *window
            ) zfpurevirtual;

public:
    /** @brief util for impl to access impl from owner ad */
    static ZFAdForSplashImpl *implForAd(ZF_IN ZFAdForSplash *ad) {
        return (ZFAdForSplashImpl *)ad->_ZFP_ZFAdForSplash_impl();
    }

    // ============================================================
    // callbacks that implementations must notify
public:
    /** @brief see #ZFAdForSplash::E_AdOnError */
    zffinal void notifyAdOnError(
            ZF_IN ZFAdForSplash *ad
            , ZF_IN const zfstring &errorHint
            ) {
        ad->_ZFP_ZFAdForSplash_stop(v_ZFResultType::e_Fail, errorHint);
    }
    /** @brief see #ZFAdForSplash::E_AdOnDisplay */
    zffinal void notifyAdOnDisplay(ZF_IN ZFAdForSplash *ad) {
        ad->observerNotify(ZFAdForSplash::E_AdOnDisplay());
    }
    /** @brief see #ZFAdForSplash::E_AdOnClick */
    zffinal void notifyAdOnClick(ZF_IN ZFAdForSplash *ad) {
        ad->observerNotify(ZFAdForSplash::E_AdOnClick());
    }

    /** @brief see #ZFAdForSplash::E_AdOnStop */
    zffinal void notifyAdOnStop(
            ZF_IN ZFAdForSplash *ad
            , ZF_IN ZFResultType resultType
            ) {
        ZFCoreAssert(resultType == v_ZFResultType::e_Success || resultType == v_ZFResultType::e_Cancel);
        ad->_ZFP_ZFAdForSplash_stop(resultType, zfnull);
    }
};

ZF_NAMESPACE_GLOBAL_END
#endif // #ifndef _ZFI_ZFProtocolZFAdForSplash_h_

