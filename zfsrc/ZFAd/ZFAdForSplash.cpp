#include "ZFAdForSplash.h"
#include "protocol/ZFProtocolZFAdForSplash.h"

ZF_NAMESPACE_GLOBAL_BEGIN

zfclassNotPOD _ZFP_ZFAdForSplashPrivate {
public:
    void *nativeAd;
    zfauto eventHolder;
    zfbool started;
public:
    _ZFP_ZFAdForSplashPrivate(void)
    : nativeAd(zfnull)
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
ZFEVENT_REGISTER(ZFAdForSplash, AdOnTimer)
ZFEVENT_REGISTER(ZFAdForSplash, AdOnStop)

ZFPROPERTY_ON_UPDATE_DEFINE(ZFAdForSplash, zfstring, appId) {
    if(this->appId() && this->adId()) {
        ZFPROTOCOL_ACCESS(ZFAdForSplash)->nativeAdUpdate(this);
    }
}
ZFPROPERTY_ON_UPDATE_DEFINE(ZFAdForSplash, zfstring, adId) {
    if(this->appId() && this->adId()) {
        ZFPROTOCOL_ACCESS(ZFAdForSplash)->nativeAdUpdate(this);
    }
}

ZFMETHOD_DEFINE_0(ZFAdForSplash, void *, nativeAd) {
    return d->nativeAd;
}

ZFMETHOD_DEFINE_2(ZFAdForSplash, void, start
        , ZFMP_IN_OPT(const ZFListener &, onStop, zfnull)
        , ZFMP_IN_OPT(ZFUIRootWindow *, window, zfnull)
        ) {
    if(!d->started) {
        if(!this->appId() || !this->adId()) {
            this->observerNotify(zfself::E_AdOnError(), zfobj<v_zfstring>("appId and adId must be set before start"));
            return;
        }

        d->started = zftrue;
        zfRetain(this);
        if(window == zfnull) {
            window = ZFUIRootWindow::mainWindow();
        }
        this->observerNotify(zfself::E_AdOnStart(), window);

        if(window->nativeWindowIsCreated()) {
            ZFPROTOCOL_ACCESS(ZFAdForSplash)->nativeAdStart(this, window);
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
                ZFPROTOCOL_ACCESS(ZFAdForSplash)->nativeAdStart(owner, zfargs.sender());
            } ZFLISTENER_END()

            ZFObserverGroup(d->eventHolder, window)
                .observerAdd(ZFUIRootWindow::E_WindowOnCreate(), windowOnCreate, ZFLevelZFFrameworkPostNormal)
                ;
        }
    }
}
ZFMETHOD_DEFINE_0(ZFAdForSplash, void, stop) {
    if(d->started) {
        d->stop();
        ZFPROTOCOL_ACCESS(ZFAdForSplash)->nativeAdStop(this);
        this->observerNotify(zfself::E_AdOnStop(), zfobj<v_ZFResultType>(v_ZFResultType::e_Cancel));
        zfRelease(this);
    }
}
ZFMETHOD_DEFINE_0(ZFAdForSplash, zfbool, started) {
    return d->started;
}

void ZFAdForSplash::objectOnInit(void) {
    zfsuper::objectOnInit();
    d = zfpoolNew(_ZFP_ZFAdForSplashPrivate);
    d->nativeAd = ZFPROTOCOL_ACCESS(ZFAdForSplash)->nativeAdCreate(this);
}
void ZFAdForSplash::objectOnDealloc(void) {
    ZFPROTOCOL_ACCESS(ZFAdForSplash)->nativeAdDestroy(this);
    zfpoolDelete(d);
    zfsuper::objectOnDealloc();
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

