/**
 * @file ZFAdForBanner.h
 * @brief banner ad
 */

#ifndef _ZFI_ZFAdForBanner_h_
#define _ZFI_ZFAdForBanner_h_

#include "ZFAdDef.h"

ZF_NAMESPACE_GLOBAL_BEGIN

/**
 * @brief banner ad
 */
zfclass ZFLIB_ZFAd ZFAdForBanner : zfextend ZFUIView {
    ZFOBJECT_DECLARE(ZFAdForBanner, ZFUIView)

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
     * called when user want to close the ad
     */
    ZFEVENT(AdOnClose)

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

protected:
    zfoverride
    virtual void objectOnInit(void);
    zfoverride
    virtual void objectOnDealloc(void);

    zfoverride
    virtual void layoutOnMeasure(
            ZF_OUT ZFUISize &ret
            , ZF_IN const ZFUISize &sizeHint
            , ZF_IN const ZFUISizeParam &sizeParam
            );
};

ZF_NAMESPACE_GLOBAL_END
#endif // #ifndef _ZFI_ZFAdForBanner_h_

