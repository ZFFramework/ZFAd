/**
 * @file ZFProtocolZFAdForReward.h
 * @brief protocol for ZFAdForReward
 */

#ifndef _ZFI_ZFProtocolZFAdForReward_h_
#define _ZFI_ZFProtocolZFAdForReward_h_

#include "ZFCore/ZFProtocol.h"
#include "../ZFAdForReward.h"
ZF_NAMESPACE_GLOBAL_BEGIN

/**
 * @brief protocol for ZFAdForReward
 *
 * impl must register by declaring a class named `ZFAdForRewardImpl_xxx`,
 * while `xxx` is the name passed from #ZFAdForReward::setup
 */
zfinterface ZFLIB_ZFAd ZFAdForRewardImpl : zfextend ZFInterface {
    ZFINTERFACE_DECLARE(ZFAdForRewardImpl, ZFInterface)

public:
    /** @brief create native ad */
    virtual void *nativeAdCreate(
            ZF_IN ZFAdForReward *ad
            , ZF_IN const zfstring &appId
            , ZF_IN const zfstring &adId
            ) zfpurevirtual;
    /** @brief destroy native ad */
    virtual void nativeAdDestroy(ZF_IN ZFAdForReward *ad) zfpurevirtual;

    /** @brief load native ad */
    virtual void nativeAdLoad(ZF_IN ZFAdForReward *ad) zfpurevirtual;
    /** @brief chech whether ad loaded and not expired */
    virtual zfbool nativeAdLoaded(ZF_IN ZFAdForReward *ad) zfpurevirtual;

    /** @brief start native ad */
    virtual void nativeAdStart(ZF_IN ZFAdForReward *ad) zfpurevirtual;

public:
    /** @brief util for impl to access impl from owner ad */
    static ZFAdForRewardImpl *implForAd(ZF_IN ZFAdForReward *ad) {
        return (ZFAdForRewardImpl *)ad->_ZFP_ZFAdForReward_impl();
    }

    // ============================================================
    // callbacks that implementations must notify
public:
    /** @brief see #ZFAdForReward::E_AdOnError */
    zffinal void notifyAdOnError(
            ZF_IN ZFAdForReward *ad
            , ZF_IN const zfstring &errorHint
            ) {
        ad->_ZFP_ZFAdForReward_stop(v_ZFResultType::e_Fail, errorHint);
    }
    /** @brief see #ZFAdForReward::E_AdOnDisplay */
    zffinal void notifyAdOnDisplay(ZF_IN ZFAdForReward *ad) {
        ad->observerNotify(ZFAdForReward::E_AdOnDisplay());
    }
    /** @brief see #ZFAdForReward::E_AdOnClick */
    zffinal void notifyAdOnClick(ZF_IN ZFAdForReward *ad) {
        ad->observerNotify(ZFAdForReward::E_AdOnClick());
    }

    /** @brief see #ZFAdForReward::E_AdOnLoad */
    zffinal void notifyAdOnLoad(
            ZF_IN ZFAdForReward *ad
            ) {
        ad->_ZFP_ZFAdForReward_onLoad();
    }
    /** @brief see #ZFAdForReward::E_AdOnStop */
    zffinal void notifyAdOnStop(
            ZF_IN ZFAdForReward *ad
            , ZF_IN ZFResultType resultType
            ) {
        ZFCoreAssert(resultType == v_ZFResultType::e_Success || resultType == v_ZFResultType::e_Cancel);
        ad->_ZFP_ZFAdForReward_stop(resultType, zfnull);
    }
};

ZF_NAMESPACE_GLOBAL_END
#endif // #ifndef _ZFI_ZFProtocolZFAdForReward_h_

