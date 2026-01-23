/**
 * @file ZFAdForBannerHelper.h
 * @brief banner ad helper
 */

#ifndef _ZFI_ZFAdForBannerHelper_h_
#define _ZFI_ZFAdForBannerHelper_h_

#include "ZFAdForBanner.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclassFwd _ZFP_ZFAdForBannerHelperPrivate;
/**
 * @brief banner ad helper
 *
 * different envs may require different appId/adId configs,
 * use this helper class to config quickly:
 * @code
 *   zfobj<ZFAdForBannerHelper> ad;
 *   ad
 *      ->cfg("implName 1", "appId 1", "adId 1", "iOS")
 *      ->cfg("implName 2", "appId 2", "adId 2", "Android", "CN")
 *      ->cfg("implName 3", "appId 3", "adId 3", "Android", "US")
 *      ;
 *
 *   // simply add it to parent
 *   // the helper would try each config and:
 *   // * display ad if success
 *   // * measure as zero height if all cfgs failed
 *   parent->child(ad);
 * @endcode
 *
 * how it works:
 * -# try each config if env matches, in order
 * -# try next one if some impl failed
 * -# simulate all events of #ZFAdForBanner, except #ZFAdForBanner::E_AdOnError
 */
zfclass ZFLIB_ZFAd ZFAdForBannerHelper : zfextend ZFUIView {
    ZFOBJECT_DECLARE(ZFAdForBannerHelper, ZFUIView)

public:
    /**
     * @brief config one impl, see #ZFAdForBannerHelper
     */
    ZFMETHOD_DECLARE_6(ZFAdForBannerHelper *, cfg
            , ZFMP_IN(const zfstring &, implName)
            , ZFMP_IN(const zfstring &, appId)
            , ZFMP_IN(const zfstring &, adId)
            , ZFMP_IN_OPT(const zfstring &, systemName, zfnull)
            , ZFMP_IN_OPT(const zfstring &, localeName, zfnull)
            , ZFMP_IN_OPT(const zfstring &, localeLangName, zfnull)
            )

protected:
    zfoverride
    virtual void objectOnInit(void);
    zfoverride
    virtual void objectOnDealloc(void);
    zfoverride
    virtual void objectOnDeallocPrepare(void);

    zfoverride
    virtual void layoutOnMeasure(
            ZF_OUT ZFUISize &ret
            , ZF_IN const ZFUISize &sizeHint
            , ZF_IN const ZFUISizeParam &sizeParam
            );
private:
    _ZFP_ZFAdForBannerHelperPrivate *d;
};

ZF_NAMESPACE_GLOBAL_END
#endif // #ifndef _ZFI_ZFAdForBannerHelper_h_

