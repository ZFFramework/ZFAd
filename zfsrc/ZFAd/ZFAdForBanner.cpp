#include "ZFAdForBanner.h"
#include "protocol/ZFProtocolZFAdForBanner.h"

ZF_NAMESPACE_GLOBAL_BEGIN

// ============================================================
ZFOBJECT_REGISTER(ZFAdForBanner)

ZFEVENT_REGISTER(ZFAdForBanner, AdOnError)
ZFEVENT_REGISTER(ZFAdForBanner, AdOnDisplay)
ZFEVENT_REGISTER(ZFAdForBanner, AdOnClick)

ZFEVENT_REGISTER(ZFAdForBanner, AdOnClose)

ZFPROPERTY_ON_UPDATE_DEFINE(ZFAdForBanner, zfstring, appId) {
    if(this->appId() && this->adId()) {
        ZFPROTOCOL_ACCESS(ZFAdForBanner)->nativeAdUpdate(this);
    }
}
ZFPROPERTY_ON_UPDATE_DEFINE(ZFAdForBanner, zfstring, adId) {
    if(this->appId() && this->adId()) {
        ZFPROTOCOL_ACCESS(ZFAdForBanner)->nativeAdUpdate(this);
    }
}

ZFMETHOD_DEFINE_0(ZFAdForBanner, void *, nativeAd) {
    return this->nativeImplView();
}

void ZFAdForBanner::objectOnInit(void) {
    zfsuper::objectOnInit();

    ZFCoreAssert(this->nativeImplView() == zfnull);
    zfclassNotPOD NativeImplViewDestroy {
    public:
        static void action(ZF_IN zfanyT<ZFUIView> const &view) {
            ZFPROTOCOL_ACCESS(ZFAdForBanner)->nativeAdDestroy(view);
        }
    };
    this->nativeImplView(
        ZFPROTOCOL_ACCESS(ZFAdForBanner)->nativeAdCreate(this)
        , NativeImplViewDestroy::action
        );
}
void ZFAdForBanner::objectOnDealloc(void) {
    zfsuper::objectOnDealloc();
}

void ZFAdForBanner::layoutOnMeasure(
        ZF_OUT ZFUISize &ret
        , ZF_IN const ZFUISize &sizeHint
        , ZF_IN const ZFUISizeParam &sizeParam
        ) {
    ret = ZFPROTOCOL_ACCESS(ZFAdForBanner)->nativeAdMeasure(this, ZFUISizeApplyScale(sizeHint, this->UIScaleFixed()));
}

ZF_NAMESPACE_GLOBAL_END

