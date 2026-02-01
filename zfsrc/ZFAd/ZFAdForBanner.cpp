#include "ZFAdForBanner.h"
#include "protocol/ZFProtocolZFAdForBanner.h"

ZF_NAMESPACE_GLOBAL_BEGIN

// ============================================================
ZFOBJECT_REGISTER(ZFAdForBanner)

ZFEVENT_REGISTER(ZFAdForBanner, AdOnError)
ZFEVENT_REGISTER(ZFAdForBanner, AdOnDisplay)
ZFEVENT_REGISTER(ZFAdForBanner, AdOnClick)

ZFEVENT_REGISTER(ZFAdForBanner, AdOnClose)

ZFMETHOD_DEFINE_3(ZFAdForBanner, void, setup
        , ZFMP_IN(const zfstring &, implName)
        , ZFMP_IN(const zfstring &, appId)
        , ZFMP_IN(const zfstring &, adId)
        ) {
    if(d) {
        this->nativeImplView(zfnull, zfnull);
        d = zfnull;
    }

    ZFCoreAssert(this->nativeImplView() == zfnull);

    const ZFClass *cls = ZFClass::classForName(zfstr("ZFAdForBannerImpl_%s", implName));
    if(cls == zfnull) {
        ZFLogTrim("[ZFAdForBanner] no impl: %s", implName);
        return;
    }
    zfautoT<ZFAdForBannerImpl> impl = cls->newInstance();
    if(zfcast(ZFAdForBannerImpl *, impl) == zfnull) {
        ZFLogTrim("[ZFAdForBanner] invalid impl: %s", cls);
        return;
    }

    void *nativeAd = impl->nativeAdCreate(this, appId, adId);
    if(nativeAd == zfnull) {
        ZFLogTrim("[ZFAdForBanner] unable to setup(%s, %s, %s)", implName, appId, adId);
        return;
    }

    d = impl;
    zfclassNotPOD NativeImplViewDestroy {
    public:
        static void action(ZF_IN zfanyT<ZFUIView> const &view) {
            ZFAdForBanner *owner = view;
            owner->d.to<ZFAdForBannerImpl *>()->nativeAdDestroy(owner);
            owner->d = zfnull;
        }
    };
    this->nativeImplView(
        nativeAd
        , NativeImplViewDestroy::action
        );

    if(this->viewTreeInWindow()) {
        impl->nativeAdAttach(this);
    }
}

ZFMETHOD_DEFINE_0(ZFAdForBanner, void *, nativeAd) {
    return this->nativeImplView();
}

ZFOBJECT_ON_INIT_DEFINE_3(ZFAdForBanner
        , ZFMP_IN(const zfstring &, implName)
        , ZFMP_IN(const zfstring &, appId)
        , ZFMP_IN(const zfstring &, adId)
        ) {
    this->setup(implName, appId, adId);
}

void ZFAdForBanner::objectOnInit(void) {
    zfsuper::objectOnInit();
    d = zfnull;
}
void ZFAdForBanner::objectOnDealloc(void) {
    ZFCoreAssert(d == zfnull);
    zfsuper::objectOnDealloc();
}
void ZFAdForBanner::objectOnDeallocPrepare(void) {
    this->nativeImplView(zfnull, zfnull);
    zfsuper::objectOnDeallocPrepare();
}

void ZFAdForBanner::viewTreeInWindowOnUpdate(void) {
    zfsuper::viewTreeInWindowOnUpdate();
    ZFAdForBannerImpl *impl = d;
    if(impl) {
        if(this->viewTreeInWindow()) {
            impl->nativeAdAttach(this);
        }
        else {
            impl->nativeAdDetach(this);
        }
    }
}

void ZFAdForBanner::layoutOnMeasure(
        ZF_OUT ZFUISize &ret
        , ZF_IN const ZFUISize &sizeHint
        , ZF_IN const ZFUISizeParam &sizeParam
        ) {
    if(d) {
        ret = ZFUISizeApplyScaleReversely(
                d.to<ZFAdForBannerImpl *>()->nativeAdMeasure(this, ZFUISizeApplyScale(sizeHint, this->UIScaleFixed()))
                , this->UIScaleFixed()
                );
    }
}

void *ZFAdForBanner::_ZFP_ZFAdForBanner_impl(void) {
    return (void *)d.to<ZFAdForBannerImpl *>();
}

ZF_NAMESPACE_GLOBAL_END

