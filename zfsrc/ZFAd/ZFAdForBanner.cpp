#include "ZFAdForBanner.h"
#include "protocol/ZFProtocolZFAdForBanner.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclassNotPOD _ZFP_ZFAdForBannerPrivate {
public:
    void *nativeAd;
};

// ============================================================
ZFOBJECT_REGISTER(ZFAdForBanner)

ZFMETHOD_DEFINE_0(ZFAdForBanner, void *, nativeAd) {
    return d->nativeAd;
}

void ZFAdForBanner::objectOnInit(void) {
    zfsuper::objectOnInit();
    d = zfpoolNew(_ZFP_ZFAdForBannerPrivate);
    d->nativeAd = ZFPROTOCOL_ACCESS(ZFAdForBanner)->nativeAdViewCreate(this);
}
void ZFAdForBanner::objectOnDealloc(void) {
    ZFPROTOCOL_ACCESS(ZFAdForBanner)->nativeAdViewDestroy(this, d->nativeAd);
    zfsuper::objectOnDealloc();
}

ZF_NAMESPACE_GLOBAL_END

