/**
 * @file ZFAdForSplashHelper.h
 * @brief splash ad helper
 */

#ifndef _ZFI_ZFAdForSplashHelper_h_
#define _ZFI_ZFAdForSplashHelper_h_

#include "ZFAdForSplash.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclassFwd _ZFP_ZFAdForSplashHelperPrivate;
/**
 * @brief splash ad helper
 *
 * different envs may require different appId/adId configs,
 * use this helper class to config quickly:
 * @code
 *   ZFAdForSplashHelper::instance()
 *       // each config
 *       ->c_cfg("implName 1", "appId 1", "adId 1", "iOS")
 *       ->c_cfg("implName 2", "appId 2", "adId 2", "Android", "CN")
 *       ->c_cfg("implName 3", "appId 3", "adId 3", "Android", "US")
 *       // simply attach the ad, the helper would:
 *       // * start the ad when window showed
 *       // * try each config and:
 *       //     * display #ZFAdForSplashHelper::bg during ad loading
 *       //     * display ad if success
 *       ->attach();
 * @endcode
 *
 * how it works:
 * -# try each config if env matches, in order
 *   -  systemName/localeId/localeLangId are matched by #ZFRegExpMatch
 * -# try next one if some impl failed
 * -# simulate all events of #ZFAdForSplash
 */
zfclass ZFLIB_ZFAd ZFAdForSplashHelper : zfextend ZFObject {
    ZFOBJECT_DECLARE(ZFAdForSplashHelper, ZFObject)

public:
    /**
     * @brief access instance
     */
    ZFMETHOD_DECLARE_STATIC_1(zfautoT<ZFAdForSplashHelper>, instance
            , ZFMP_IN_OPT(ZFUIRootWindow *, window, zfnull)
            )

public:
    /**
     * @brief config one impl, see #ZFAdForSplashHelper
     */
    ZFMETHOD_DECLARE_6(ZFAdForSplashHelper *, cfg
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
    /**
     * @brief bg content displaying during ad loading,
     *   null would result to a plain black color view
     */
    ZFPROPERTY_RETAIN(zfanyT<ZFUIView>, bg)

public:
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

    // ============================================================
public:
    /**
     * @brief after showing a splash ad,
     *   do not show again during this duration (affect by #attach only)
     */
    ZFPROPERTY_ASSIGN(zftimet, silentDuration, zftimetOneMinute() * 5)

public:
    /**
     * @brief more convenient method to manage splash ad
     *
     * how it works:
     * -# attach observers to #window
     * -# auto #start when #window show, according to #silentDuration
     */
    ZFMETHOD_DECLARE_0(void, attach)
    /**
     * @brief see #attach
     */
    ZFMETHOD_DECLARE_0(void, detach)
    /**
     * @brief see #attach
     */
    ZFMETHOD_DECLARE_0(zfbool, attached)

protected:
    zfoverride
    virtual void objectOnInit(void);
    zfoverride
    virtual void objectOnDealloc(void);

private:
    _ZFP_ZFAdForSplashHelperPrivate *d;
};

ZF_NAMESPACE_GLOBAL_END
#endif // #ifndef _ZFI_ZFAdForSplashHelper_h_

