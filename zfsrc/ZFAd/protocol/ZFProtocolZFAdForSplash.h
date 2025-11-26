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
 */
ZFPROTOCOL_INTERFACE_BEGIN(ZFLIB_ZFAd, ZFAdForSplash)
public:
    /** @brief create native ad */
    virtual void *nativeAdCreate(ZF_IN ZFAdForSplash *ad) zfpurevirtual;
    /** @brief destroy native ad */
    virtual void nativeAdDestroy(ZF_IN ZFAdForSplash *ad) zfpurevirtual;

    /** @brief update app id */
    virtual void appIdUpdate(ZF_IN ZFAdForSplash *ad) zfpurevirtual;
    /** @brief update app key */
    virtual void adIdUpdate(ZF_IN ZFAdForSplash *ad) zfpurevirtual;

    /** @brief start native ad */
    virtual void start(ZF_IN ZFAdForSplash *ad) zfpurevirtual;
    /** @brief stop native ad */
    virtual void stop(ZF_IN ZFAdForSplash *ad) zfpurevirtual;

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

    /** @brief see #ZFAdForSplash::E_AdOnTimer */
    zffinal void notifyAdOnTimer(
            ZF_IN ZFAdForSplash *ad
            , ZF_IN zftimet remainingTime
            ) {
        ad->observerNotify(ZFAdForSplash::E_AdOnTimer(), zfobj<v_zftimet>(remainingTime));
    }
    /** @brief see #ZFAdForSplash::E_AdOnStop */
    zffinal void notifyAdOnStop(
            ZF_IN ZFAdForSplash *ad
            , ZF_IN ZFResultType resultType
            ) {
        ZFCoreAssert(resultType == v_ZFResultType::e_Success || resultType == v_ZFResultType::e_Fail);
        ad->_ZFP_ZFAdForSplash_stop(resultType, zfnull);
    }
ZFPROTOCOL_INTERFACE_END(ZFAdForSplash)

ZF_NAMESPACE_GLOBAL_END
#endif // #ifndef _ZFI_ZFProtocolZFAdForSplash_h_

