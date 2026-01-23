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
 *   zfobj<ZFAdForSplashHelper> ad;
 *   ad
 *      ->cfg("implName 1", "appId 1", "adId 1", "iOS")
 *      ->cfg("implName 2", "appId 2", "adId 2", "Android", "CN")
 *      ->cfg("implName 3", "appId 3", "adId 3", "Android", "US")
 *      ;
 *
 *   // simply start the ad
 *   // the helper would try each config and:
 *   // * display #ZFAdForSplashHelper::bg during ad loading
 *   // * display ad if success
 *   ad->start();
 * @endcode
 *
 * how it works:
 * -# try each config if env matches, in order
 * -# try next one if some impl failed
 * -# simulate all events of #ZFAdForSplash, except #ZFAdForSplash::E_AdOnError
 */
zfclass ZFLIB_ZFAd ZFAdForSplashHelper : zfextend ZFObject {
    ZFOBJECT_DECLARE(ZFAdForSplashHelper, ZFObject)

public:
    /**
     * @brief config one impl, see #ZFAdForSplashHelper
     */
    ZFMETHOD_DECLARE_6(ZFAdForSplashHelper *, cfg
            , ZFMP_IN(const zfstring &, implName)
            , ZFMP_IN(const zfstring &, appId)
            , ZFMP_IN(const zfstring &, adId)
            , ZFMP_IN_OPT(const zfstring &, systemName, zfnull)
            , ZFMP_IN_OPT(const zfstring &, localeName, zfnull)
            , ZFMP_IN_OPT(const zfstring &, localeLangName, zfnull)
            )

public:
    /**
     * @brief owner window
     */
    ZFMETHOD_DECLARE_0(zfanyT<ZFUIRootWindow>, window)
    /** @brief see #window */
    ZFMETHOD_DECLARE_1(ZFAdForSplashHelper *, window
            , ZFMP_IN(ZFUIRootWindow *, v)
            )

    /** @brief window level for #bg */
    ZFPROPERTY_ASSIGN(zfint, windowLevel, 1500)

    /**
     * @brief timeout if no impl can display success
     */
    ZFPROPERTY_ASSIGN(zftimet, timeout, 3000)
    /**
     * @brief bg content displaying during ad loading,
     *   null would result to a plain white color view
     */
    ZFPROPERTY_RETAIN(zfanyT<ZFUIView>, bg)

public:
    /**
     * @brief start the ad, auto retain until stop
     *
     * onStop's param0 is a #ZFResultType indicates result type,
     * param1 is a #v_zfstring optionally holds the error hint
     */
    ZFMETHOD_DECLARE_2(void, start
            , ZFMP_IN_OPT(const ZFListener &, onStop, zfnull)
            , ZFMP_IN_OPT(ZFUIRootWindow *, window, zfnull)
            )
    /** @brief whether the ad started */
    ZFMETHOD_DECLARE_0(zfbool, started)

    /** @brief init with owner window */
    ZFOBJECT_ON_INIT_DECLARE_1(
            ZFMP_IN(ZFUIRootWindow *, window)
            )

protected:
    zfoverride
    virtual void objectOnInit(void);
    zfoverride
    virtual void objectOnDealloc(void);
    zfoverride
    virtual void objectOnDeallocPrepare(void);

private:
    _ZFP_ZFAdForSplashHelperPrivate *d;
};

ZF_NAMESPACE_GLOBAL_END
#endif // #ifndef _ZFI_ZFAdForSplashHelper_h_

