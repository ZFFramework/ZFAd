/**
 * @file ZFAdForSplash.h
 * @brief splash ad
 */

#ifndef _ZFI_ZFAdForSplash_h_
#define _ZFI_ZFAdForSplash_h_

#include "ZFAdDef.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclassFwd _ZFP_ZFAdForSplashPrivate;
/**
 * @brief splash ad
 */
zfclass ZFLIB_ZFAd ZFAdForSplash : zfextend ZFObject, zfimplement ZFTaskId {
    ZFOBJECT_DECLARE(ZFAdForSplash, ZFObject)
    ZFIMPLEMENT_DECLARE(ZFTaskId)

public:
    // ============================================================
    // events
    /**
     * @brief see #ZFObject::observerNotify
     *
     * called when any error occurred,
     * param0 is a #v_zfstring holds error hint
     */
    ZFEVENT(AdOnError)
    /**
     * @brief see #ZFObject::observerNotify
     *
     * called when ad displayed successfully
     */
    ZFEVENT(AdOnDisplay)
    /**
     * @brief see #ZFObject::observerNotify
     *
     * called when ad clicked
     */
    ZFEVENT(AdOnClick)

    /**
     * @brief see #ZFObject::observerNotify
     *
     * called when #start,
     * param0 is the #ZFUIRootWindow
     */
    ZFEVENT(AdOnStart)
    /**
     * @brief see #ZFObject::observerNotify
     *
     * called when #stop or timeout or error,
     * param0 is a #v_zftimet indicates remaining time
     */
    ZFEVENT(AdOnTimer)
    /**
     * @brief see #ZFObject::observerNotify
     *
     * called when #stop or timeout or error,
     * param0 is a #ZFResultType indicates result type,
     * param1 is a #v_zfstring optionally holds the error hint
     * \n
     * for the result type:
     * -  success : ad has displayed and reached timeout
     * -  fail : error occurred, #E_AdOnError would also be fired before this event
     * -  cancel : user has clicked skip button, or #stop called
     */
    ZFEVENT(AdOnStop)

public:
    /** @brief app id */
    ZFPROPERTY_ASSIGN(zfstring, appId)
    ZFPROPERTY_ON_UPDATE_DECLARE(zfstring, appId)
    /** @brief app key */
    ZFPROPERTY_ASSIGN(zfstring, adId)
    ZFPROPERTY_ON_UPDATE_DECLARE(zfstring, adId)

public:
    /** @brief access the native ad */
    ZFMETHOD_DECLARE_0(void *, nativeAd)

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
    /** @brief stop the ad */
    ZFMETHOD_DECLARE_0(void, stop)
    /** @brief whether the ad started */
    ZFMETHOD_DECLARE_0(zfbool, started)

protected:
    zfoverride
    virtual void objectOnInit(void);
    zfoverride
    virtual void objectOnDealloc(void);

private:
    _ZFP_ZFAdForSplashPrivate *d;
public:
    void _ZFP_ZFAdForSplash_stop(
            ZF_IN ZFResultType resultType
            , ZF_IN const zfstring &errorHint
            );
};

ZF_NAMESPACE_GLOBAL_END
#endif // #ifndef _ZFI_ZFAdForSplash_h_

