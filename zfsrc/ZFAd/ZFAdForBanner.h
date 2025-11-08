/**
 * @file ZFAdForBanner.h
 * @brief banner ad
 */

#ifndef _ZFI_ZFAdForBanner_h_
#define _ZFI_ZFAdForBanner_h_

#include "ZFAdDef.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclassFwd _ZFP_ZFAdForBannerPrivate;
/**
 * @brief banner ad
 */
zfclass ZFLIB_ZFAd ZFAdForBanner : zfextend ZFUIView {
    ZFOBJECT_DECLARE(ZFAdForBanner, ZFUIView)

public:
    /** @brief the native ad */
    ZFMETHOD_DECLARE_0(void *, nativeAd)

protected:
    zfoverride
    virtual void objectOnInit(void);
    zfoverride
    virtual void objectOnDealloc(void);

private:
    _ZFP_ZFAdForBannerPrivate *d;
};

ZF_NAMESPACE_GLOBAL_END
#endif // #ifndef _ZFI_ZFAdForBanner_h_

