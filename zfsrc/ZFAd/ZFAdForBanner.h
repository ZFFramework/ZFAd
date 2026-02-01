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
    /**
     * @brief setup the native ad info
     */
    ZFMETHOD_DECLARE_3(void, setup
            , ZFMP_IN(const zfstring &, implName)
            , ZFMP_IN(const zfstring &, appId)
            , ZFMP_IN(const zfstring &, adId)
            )

public:
    /** @brief access the native ad, valid only after #setup */
    ZFMETHOD_DECLARE_0(void *, nativeAd)

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

    zfoverride
    virtual void viewTreeInWindowOnUpdate(void);

    zfoverride
    virtual void layoutOnMeasure(
            ZF_OUT ZFUISize &ret
            , ZF_IN const ZFUISize &sizeHint
            , ZF_IN const ZFUISizeParam &sizeParam
            );
private:
    zfauto d;
public:
    void *_ZFP_ZFAdForBanner_impl(void);
};

ZF_NAMESPACE_GLOBAL_END
#endif // #ifndef _ZFI_ZFAdForBanner_h_

