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
zfclass ZFLIB_ZFAd ZFAdForSplash : zfextend ZFObject {
    ZFOBJECT_DECLARE(ZFAdForSplash, ZFObject)

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
     * called when loaded
     */
    ZFEVENT(AdOnLoad)
    /**
     * @brief see #ZFObject::observerNotify
     *
     * called when #start
     */
    ZFEVENT(AdOnStart)
    /**
     * @brief see #ZFObject::observerNotify
     *
     * called when timeout or error or skip clicked,
     * param0 is a #ZFResultType indicates result type,
     * param1 is a #v_zfstring optionally holds the error hint
     * \n
     * for the result type:
     * -  success : ad has displayed and reached timeout (or skip clicked on some impl)
     * -  fail : error occurred, #E_AdOnError would also be fired before this event
     * -  cancel : user has clicked skip button (may not work for some impl), or timeout
     */
    ZFEVENT(AdOnStop)

public:
    /**
     * @brief setup the native ad info
     */
    ZFMETHOD_DECLARE_3(void, setup
            , ZFMP_IN(const zfstring &, implName)
            , ZFMP_IN(const zfstring &, appId)
            , ZFMP_IN(const zfstring &, adId)
            )

public:
    /** @brief access the native ad */
    ZFMETHOD_DECLARE_0(void *, nativeAd)

    /** @brief owner window, null to use #ZFUIRootWindow::mainWindow */
    ZFMETHOD_DECLARE_0(ZFUIRootWindow *, window)
    /** @brief see #window */
    ZFMETHOD_DECLARE_1(void, window
            , ZFMP_IN(ZFUIRootWindow *, window)
            )

public:
    /**
     * @brief load the ad
     */
    ZFMETHOD_DECLARE_1(zfautoT<ZFTaskId>, load
            , ZFMP_IN_OPT(const ZFListener &, onLoaded, zfnull)
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
     */
    ZFMETHOD_DECLARE_0(void, start)
    /** @brief whether the ad started */
    ZFMETHOD_DECLARE_0(zfbool, started)

    /** @brief init with params, see #setup */
    ZFOBJECT_ON_INIT_DECLARE_3(
            ZFMP_IN(const zfstring &, implName)
            , ZFMP_IN(const zfstring &, appId)
            , ZFMP_IN(const zfstring &, adId)
            )

protected:
    zfoverride
    virtual void objectOnInit(void);
    zfoverride
    virtual void objectOnDealloc(void);
    zfoverride
    virtual void objectOnDeallocPrepare(void);

private:
    _ZFP_ZFAdForSplashPrivate *d;
public:
    void *_ZFP_ZFAdForSplash_impl(void);
    void _ZFP_ZFAdForSplash_onLoad(void);
    void _ZFP_ZFAdForSplash_stop(
            ZF_IN ZFResultType resultType
            , ZF_IN const zfstring &errorHint
            );
};

ZF_NAMESPACE_GLOBAL_END
#endif // #ifndef _ZFI_ZFAdForSplash_h_

