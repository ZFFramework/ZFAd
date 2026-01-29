#include "ZFAdForSplash.h"
#include "protocol/ZFProtocolZFAdForSplash.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclassNotPOD _ZFP_ZFAdForSplashPrivate {
public:
    zfautoT<ZFAdForSplashImpl> impl;
    void *nativeAd;
    zfauto eventHolder;
    zfbool started;
public:
    _ZFP_ZFAdForSplashPrivate(void)
    : impl()
    , nativeAd(zfnull)
    , eventHolder()
    , started(zffalse)
    {
    }
public:
    void stop(void) {
        this->started = zffalse;
        if(this->eventHolder) {
            ZFObserverGroupRemove(this->eventHolder);
            this->eventHolder = zfnull;
        }
    }
};

// ============================================================
ZFOBJECT_REGISTER(ZFAdForSplash)

ZFEVENT_REGISTER(ZFAdForSplash, AdOnError)
ZFEVENT_REGISTER(ZFAdForSplash, AdOnDisplay)
ZFEVENT_REGISTER(ZFAdForSplash, AdOnClick)

ZFEVENT_REGISTER(ZFAdForSplash, AdOnStart)
ZFEVENT_REGISTER(ZFAdForSplash, AdOnStop)

ZFMETHOD_DEFINE_3(ZFAdForSplash, void, setup
        , ZFMP_IN(const zfstring &, implName)
        , ZFMP_IN(const zfstring &, appId)
        , ZFMP_IN(const zfstring &, adId)
        ) {
    if(d->nativeAd) {
        d->impl->nativeAdDestroy(this);
        d->nativeAd = zfnull;
        d->impl = zfnull;
    }

    const ZFClass *cls = ZFClass::classForName(zfstr("ZFAdForSplashImpl_%s", implName));
    if(cls == zfnull) {
        ZFLogTrim("[ZFAdForSplash] no impl: %s", implName);
        return;
    }
    zfautoT<ZFAdForSplashImpl> impl = cls->newInstance();
    if(zfcast(ZFAdForSplashImpl *, impl) == zfnull) {
        ZFLogTrim("[ZFAdForSplash] invalid impl: %s", cls);
        return;
    }

    void *nativeAd = impl->nativeAdCreate(this, appId, adId);
    if(nativeAd == zfnull) {
        ZFLogTrim("[ZFAdForSplash] unable to setup(%s, %s, %s)", implName, appId, adId);
        return;
    }

    d->impl = impl;
    d->nativeAd = nativeAd;
}

ZFMETHOD_DEFINE_0(ZFAdForSplash, void *, nativeAd) {
    return d->nativeAd;
}

ZFMETHOD_DEFINE_2(ZFAdForSplash, void, start
        , ZFMP_IN_OPT(const ZFListener &, onStop, zfnull)
        , ZFMP_IN_OPT(ZFUIRootWindow *, window, zfnull)
        ) {
    if(!d->started) {
        if(d->nativeAd == zfnull) {
            this->observerNotify(zfself::E_AdOnError(), zfobj<v_zfstring>("ad has not been setup"));
            return;
        }

        d->started = zftrue;
        zfRetain(this);
        if(window == zfnull) {
            window = ZFUIRootWindow::mainWindow();
        }
        this->observerNotify(zfself::E_AdOnStart(), window);

        if(window->nativeWindowIsCreated()) {
            d->impl->nativeAdStart(this, window);
        }
        else {
            d->eventHolder = zfobj<ZFObject>();
            zfself *owner = this;

            ZFLISTENER_1(windowOnCreate
                    , zfweakT<zfself>, owner
                    ) {
                if(owner->d->eventHolder) {
                    ZFObserverGroupRemove(owner->d->eventHolder);
                    owner->d->eventHolder = zfnull;
                }
                owner->d->impl->nativeAdStart(owner, zfargs.sender());
            } ZFLISTENER_END()

            ZFObserverGroup(d->eventHolder, window)
                .observerAdd(ZFUIRootWindow::E_WindowOnCreate(), windowOnCreate, ZFLevelZFFrameworkPostNormal)
                ;
        }
    }
}
ZFMETHOD_DEFINE_0(ZFAdForSplash, zfbool, started) {
    return d->started;
}

ZFOBJECT_ON_INIT_DEFINE_3(ZFAdForSplash
        , ZFMP_IN(const zfstring &, implName)
        , ZFMP_IN(const zfstring &, appId)
        , ZFMP_IN(const zfstring &, adId)
        ) {
    this->setup(implName, appId, adId);
}

void ZFAdForSplash::objectOnInit(void) {
    zfsuper::objectOnInit();
    d = zfpoolNew(_ZFP_ZFAdForSplashPrivate);
}
void ZFAdForSplash::objectOnDealloc(void) {
    zfpoolDelete(d);
    zfsuper::objectOnDealloc();
}
void ZFAdForSplash::objectOnDeallocPrepare(void) {
    if(d->nativeAd) {
        d->impl->nativeAdDestroy(this);
        d->nativeAd = zfnull;
        d->impl = zfnull;
    }
    zfsuper::objectOnDeallocPrepare();
}

void *ZFAdForSplash::_ZFP_ZFAdForSplash_impl(void) {
    return (void *)d->impl.to<ZFAdForSplashImpl *>();
}
void ZFAdForSplash::_ZFP_ZFAdForSplash_stop(
        ZF_IN ZFResultType resultType
        , ZF_IN const zfstring &errorHint
        ) {
    if(d->started) {
        d->stop();
        if(resultType == v_ZFResultType::e_Fail) {
            this->observerNotify(zfself::E_AdOnError(), zfobj<v_zfstring>(errorHint));
        }
        this->observerNotify(zfself::E_AdOnStop(), zfobj<v_ZFResultType>(resultType), zfobj<v_zfstring>(errorHint));
        zfRelease(this);
    }
}

ZF_NAMESPACE_GLOBAL_END

