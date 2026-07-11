/**
 * @file ZFAdForRewardHelper.h
 * @brief reward ad helper
 */

#ifndef _ZFI_ZFAdForRewardHelper_h_
#define _ZFI_ZFAdForRewardHelper_h_

#include "ZFAdForReward.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclassFwd _ZFP_ZFAdForRewardHelperPrivate;
/**
 * @brief reward ad helper
 *
 * different envs may require different appId/adId configs,
 * use this helper class to config quickly:
 * @code
 *   ZFAdForRewardHelper::instance()
 *       // each config
 *       ->c_cfg("implName 1", "appId 1", "adId 1", "iOS")
 *       ->c_cfg("implName 2", "appId 2", "adId 2", "Android", "CN")
 *       ->c_cfg("implName 3", "appId 3", "adId 3", "Android", "US")
 *       // simply attach the ad, the helper would try each config
 *       ->start(onStop);
 * @endcode
 *
 * how it works:
 * -# try each config if env matches, in order
 *   -  systemName/localeId/localeLangId are matched by #ZFRegExpMatch
 * -# try next one if some impl failed
 * -# simulate all events of #ZFAdForReward
 */
zfclass ZFLIB_ZFAd ZFAdForRewardHelper : zfextend ZFObject {
    ZFOBJECT_DECLARE(ZFAdForRewardHelper, ZFObject)

public:
    /**
     * @brief access instance
     */
    ZFMETHOD_DECLARE_STATIC_1(zfautoT<ZFAdForRewardHelper>, instance
            , ZFMP_IN_OPT(ZFUIRootWindow *, window, zfnull)
            )

public:
    /**
     * @brief config one impl, see #ZFAdForRewardHelper
     */
    ZFMETHOD_DECLARE_6(ZFAdForRewardHelper *, cfg
            , ZFMP_IN(const zfstring &, implName)
            , ZFMP_IN(const zfstring &, appId)
            , ZFMP_IN(const zfstring &, adId)
            , ZFMP_IN_OPT(const zfstring &, systemName, zfnull)
            , ZFMP_IN_OPT(const zfstring &, localeId, zfnull)
            , ZFMP_IN_OPT(const zfstring &, localeLangId, zfnull)
            )

public:
    /**
     * @brief owner window
     */
    ZFMETHOD_DECLARE_0(zfanyT<ZFUIRootWindow>, window)

    /**
     * @brief timeout if no impl can display success
     */
    ZFPROPERTY_ASSIGN(zftimet, timeout, 5000)

public:
    /**
     * @brief load the ad
     *
     * param0 is a #v_ZFResultType indicates resultType,
     * param1 is a #v_zfstring optionally holds the errorHint
     */
    ZFMETHOD_DECLARE_1(zfautoT<ZFTaskId>, load
            , ZFMP_IN_OPT(const ZFListener &, onLoadStop, zfnull)
            )
    /**
     * @brief whether ad loaded
     *
     * can be chaned to false if previously loaded ad expired,
     * always use #load to check is recommended
     */
    ZFMETHOD_DECLARE_0(zfbool, loaded)

    /**
     * @brief start the ad, auto retain until stop
     *
     * onStop's param0 is a #ZFResultType indicates result type,
     * param1 is a #v_zfstring optionally holds the error hint
     */
    ZFMETHOD_DECLARE_1(void, start
            , ZFMP_IN_OPT(const ZFListener &, onStop, zfnull)
            )
    /** @brief whether the ad started */
    ZFMETHOD_DECLARE_0(zfbool, started)

protected:
    zfoverride
    virtual void objectOnInit(void);
    zfoverride
    virtual void objectOnDealloc(void);

private:
    _ZFP_ZFAdForRewardHelperPrivate *d;
    friend zfclassFwd _ZFP_ZFAdForRewardHelperPrivate;
};

ZF_NAMESPACE_GLOBAL_END
#endif // #ifndef _ZFI_ZFAdForRewardHelper_h_

